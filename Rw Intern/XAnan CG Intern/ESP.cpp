#include <iostream>
#include "ESP.h"
#include "menu.h"
#include "mem.hpp"
#include "mem Aimbot.h"
#include "CS2_Dumper/offsets.hpp"
#include "CS2_Dumper/client_dll.hpp"
//thanks weedptr https://github.com/MitilcC/CS2-Internal-Cheat/issues/2
void ESP::DrawHealth(float MaxHealth, float CurrentHealth, ImVec2 Pos, ImVec2 Size, bool Horizontal)
{
	ImDrawList* DrawList = ImGui::GetBackgroundDrawList();

	float Proportion = CurrentHealth / MaxHealth;

	ImColor FirstStageColor = ImColor(96, 246, 113, 220);
	ImColor SecondStageColor = ImColor(247, 214, 103, 220);
	ImColor ThirdStageColor = ImColor(255, 95, 95, 220);
	ImColor BackGroundColor = ImColor(90, 90, 90, 220);
	ImColor Color;
	if (Proportion > 0.5)
		Color = FirstStageColor;
	else if (Proportion > 0.25)
		Color = SecondStageColor;
	else
		Color = ThirdStageColor;

	DrawList->AddRectFilled(Pos, { Pos.x + Size.x, Pos.y + Size.y }, BackGroundColor);

	if (Horizontal)
	{
		DrawList->AddRectFilled(Pos, { Pos.x + Size.x * Proportion, Pos.y + Size.y }, Color);
	}
	else
	{
		float healthHeight = Size.y * Proportion;
		DrawList->AddRectFilled({ Pos.x, Pos.y + Size.y - healthHeight }, { Pos.x + Size.x, Pos.y + Size.y }, Color);
	}

	ImColor BorderColor = ImColor(45, 45, 45, 220);
	DrawList->AddRect(Pos, { Pos.x + Size.x, Pos.y + Size.y }, BorderColor);

}


bool ESP::Start() 
{

	Player LocalPlayer{};

	LocalPlayer.control = Address::GetLocalPlayerControl();

	if (!LocalPlayer.control) 
		return false;

	LocalPlayer.pawn = Get::PlayerPawnAddress(LocalPlayer.control);

	if (!LocalPlayer.pawn) 
		return false;

	LocalPlayer.team = Get::PlayerTeam(LocalPlayer.pawn);

	for (int i{ 0 }; i < 64; ++i)
	{

		Vector4 Draw {};
		int distance = 0;

		Player Entity{};

		Entity.control = Address::GetEntityBase(i);

		if (!Entity.control) 
			continue;

		if(!Get::PawnAlive(Entity.control))
		{
			continue;
		}

		Entity.pawn = Get::PlayerPawnAddress(Entity.control);

		if (!Entity.pawn)
			continue;
	
		Entity.team = Get::PlayerTeam(Entity.pawn);

		Entity.health = Get::PlayerHealth(Entity.pawn);

		if(Get::IsDormant(Entity.pawn))
		{
			continue;
		}

		if (Entity.pawn == LocalPlayer.pawn)
		{
			continue;
		}

		if (Entity.team == LocalPlayer.team && !Menu::ESP::Team)
		{
			continue;
		}

		if (Entity.team != 2 && Entity.team != 3 && !Menu::ESP::Team)
		{
			continue;
		}

		if (!Get::PlayerAlive(Entity.pawn))
		{
			continue;
		}

		Entity.name = Get::PlayerName(Entity.control);

		Entity.pos = Get::PlayerPos(Entity.pawn);

		const float  w  { ImGui::GetIO().DisplaySize.x };
		const float  h  { ImGui::GetIO().DisplaySize.y };

		Vector3 currBotPos = Get::BonePos(Entity.pawn, BoneIndex::ankle_L) ;

		Vector3 currTopPos = Get::BonePos(Entity.pawn, BoneIndex::head) ;


		Vector3 curr2DBot{};
		Vector3 curr2DTop{};


		if (!Utils::WorldToScreen(currBotPos, curr2DBot, Address::GetViewMatrixPtr(), w, h))
			continue;
		
		
		if (!Utils::WorldToScreen(currTopPos, curr2DTop, Address::GetViewMatrixPtr(), w, h))
			continue;

		const float height{ ::abs(curr2DTop.y - curr2DBot.y) * 1.25f };
		const float width{ height / 2.f };
		const float x = curr2DTop.x - (width / 2.f);
		const float y = curr2DTop.y - (width / 2.5f);


		if(Menu::Misc::Rander)
			Set::RadarHack(Entity.pawn);
		
		if(Menu::ESP::Glow)
			Set::GlowHack(Entity.pawn);
		
		if (Menu::ESP::Box) 
		{
			if(Menu::ESP::BoxType == 0)
			    ImGui::GetBackgroundDrawList()->AddRect(ImVec2(x, y), ImVec2(x + width, y + height), Menu::Color::BoxColor, 3);
			else
				ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + width, y + height), Menu::Color::FilledColor, 3);
		}

		if (Menu::ESP::Line)
		{
			if (Menu::ESP::LineType == 0)
			    ImGui::GetBackgroundDrawList()->AddLine(ImVec2(w / 2, h), ImVec2(x + height / 2, y + height), Menu::Color::LineColor);
			else
				ImGui::GetBackgroundDrawList()->AddLine(ImVec2(w / 2, 0), ImVec2(x + height / 2, y), Menu::Color::LineColor);
		}


		if (Menu::ESP::Health)
		{
			ImVec2 healthBarPos = ImVec2(x - 10, y); // Adjust position as needed
			ImVec2 healthBarSize = ImVec2(5, height); // Adjust size as needed

			DrawHealth(100.0f, static_cast<float>(Entity.health), healthBarPos, healthBarSize, false);
		}

		if (Menu::ESP::Name)
		{
			ImGui::PushFont(chinesefont);
			ImGui::GetBackgroundDrawList()->AddText(ImVec2(x, y - 10), Menu::Color::NameColor, Entity.name.c_str());
			ImGui::PopFont();
		}

		if(Menu::ESP::Weapon)
		{
			ImGui::GetBackgroundDrawList()->AddText(ImVec2(x, y + height + 10), Menu::Color::WeaponColor,Get::GetWeaponName(Entity.pawn).c_str());
		}

		if (Menu::ESP::AimCricle) 
		{
			if (Menu::ESP::CricleType == 0) 
			{
				currBotPos = Get::BonePos(Entity.pawn, Menu::Aimbot::AimPos);
				if (!Utils::WorldToScreen(currBotPos, curr2DBot, Address::GetViewMatrixPtr(), w, h))
					continue;

				ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(curr2DBot.x, curr2DBot.y), Menu::Aimbot::AimSize, Menu::Color::AimCricleColor, 0, 1.5);
			}
			else
			{
				ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(w / 2, h / 2), Menu::Aimbot::AimSize, Menu::Color::AimCricleColor, 0, 1.5);
			}
		
		}

		if (Menu::ESP::Bone) 
		{
			Bone::Start(Entity.pawn, Menu::Color::BoneColor);
		}



		if (Menu::ESP::HeadCricle)
		{
			Bone::HeadCricle(Entity.pawn, Menu::Color::HeadCricleColor);
		}
	    
		if (Menu::ESP::FOV)
		{
			ESP::FovChange();
		}

		if (Menu::Aimbot::showFOV)
		{
			Aimbot::DrawAimbotFOV();

		}
	}

	return true;
}

void ESP::FovChange()
{
	mem::Process cs2(TEXT("cs2.exe"));
	auto client = cs2.get_module_handle(TEXT("client.dll"));
	auto local_player = cs2.read<mem::addr64>(client + cs2_dumper::offsets::client_dll::dwLocalPlayerPawn);
	auto cam_ser = cs2.read<mem::addr64>(local_player + cs2_dumper::schemas::client_dll::C_BasePlayerPawn::m_pCameraServices);
	auto fov_addr = cam_ser + cs2_dumper::schemas::client_dll::CCSPlayerBase_CameraServices::m_iFOV;
	cs2.write(fov_addr,Menu::ESP::FOVnumber); 
}

void ESP::ViewModelChange()
{
	mem::Process cs2(TEXT("cs2.exe"));
	auto client = cs2.get_module_handle(TEXT("client.dll"));
	auto local_player = cs2.read<mem::addr64>(client + cs2_dumper::offsets::client_dll::dwLocalPlayerPawn);
	auto cam_ViewModelx = cs2.read<mem::addr64>(local_player + cs2_dumper::schemas::client_dll:: C_CSPlayerPawn::m_flViewmodelOffsetX);
	cs2.write(cam_ViewModelx, Menu::ESP::FOVnumber);

}