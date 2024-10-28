#include "mem Aimbot.h"
#include "imgui/imgui.h"



Vector3 LastAngles{};
bool ShotFired = false;
Vector3 WantAngele{};

bool Aimbot::GetBestTarget()
{
	Player LocalPlayer{};

	LocalPlayer.control = Address::GetLocalPlayerControl();

	if (!LocalPlayer.control)
		return false;

	LocalPlayer.pawn = Get::PlayerPawnAddress(LocalPlayer.control);

	if (!LocalPlayer.pawn)
		return false;

	LocalPlayer.team = Get::PlayerTeam(LocalPlayer.pawn);

	if (!Get::PlayerAlive(LocalPlayer.pawn))
	{
		return false;
	}

	int Distance = 0;
	int LastDistance = 999999999;

	for (int i{ 0 }; i < 64; ++i)
	{

		Player Entity{};

		Entity.control = Address::GetEntityBase(i);

		if (!Entity.control)
			continue;

		if (!Get::PawnAlive(Entity.control))
		{
			continue;
		}

		Entity.pawn = Get::PlayerPawnAddress(Entity.control);

		if (!Entity.pawn)
			continue;



		Entity.team = Get::PlayerTeam(Entity.pawn);

		Entity.health = Get::PlayerHealth(Entity.pawn);

		if (Get::IsDormant(Entity.pawn))
		{
			continue;
		}

		if (Entity.team == LocalPlayer.team && !Menu::Aimbot::Team)
		{
			continue;
		}

		if (Entity.team != 2 && Entity.team != 3 && !Menu::Aimbot::Team)
		{
			continue;
		}

		if (!Get::PlayerAlive(Entity.pawn))
		{
			continue;
		}


		if(Entity.pawn == LocalPlayer.pawn)
		{
			continue;
		}

		// initialize trace, construct filterr and initialize ray
		GameTrace_t trace = GameTrace_t();
		TraceFilter_t filter = TraceFilter_t(0x1C3003, pLocalPawn, nullptr, 4);
		Ray_t ray = Ray_t();

		// cast a ray from local player eye positon -> player head bone
		// @note: would recommend checking for nullptrs
		I::GameTraceManager->TraceShape(&ray, pLocalPawn->GetEyePosition(), pPawn->GetGameSceneNode()->GetSkeletonInstance()->pBoneCache->GetOrigin(6), &filter, &trace);
		// check if the hit entity is the one we wanted to check and if the trace end point is visible
		if (trace.m_pHitEntity != pPawn || !trace.IsVisible())
			continue;



		Vector3 AimPos = Get::BonePos(Entity.pawn, Menu::Aimbot::AimPos);

		Vector3 EndPos{};

		Vector3 Window = Get::WindowSize();

		if (!Utils::WorldToScreen(AimPos, EndPos, Address::GetViewMatrixPtr(), Window.x, Window.y))
			continue;

		if (EndPos.x > Window.x / 2 - Menu::Aimbot::AimSize && EndPos.x < Window.x / 2 + Menu::Aimbot::AimSize && EndPos.y > Window.y / 2 - Menu::Aimbot::AimSize && EndPos.y < Window.y / 2 + Menu::Aimbot::AimSize)
		{
			int left = Window.x / 2 - EndPos.x ;
			int right = Window.y / 2 - EndPos.y;
			Distance = Math::distance(left,right);
			if(Distance < LastDistance)
			{
				LastDistance = Distance;
				Target::addr= Entity.pawn;
			}
		}
	}

	return true;
}


void Aimbot::NormalizePitch(float& pPitch)
{
	pPitch = (pPitch < -89.0f) ? -89.0f : pPitch;

	pPitch = (pPitch > 89.f) ? 89.0f : pPitch;
}

void Aimbot::NormalizeYaw(float& pYaw)
{
	while (pYaw > 180.f) pYaw -= 360.f;

	while (pYaw < -180.f) pYaw += 360.f;
}

float Aimbot::GetMagnitude(const Vector3& pVec)
{
	return ::sqrtf((pVec.x * pVec.x) +
		(pVec.y * pVec.y) +
		(pVec.z * pVec.z));
}

Vector3 Aimbot::GetTargetAngle(Vector3 pTargetPos)
{
	Vector3 targetAngle{};
	Player LocalPlayer{};

	LocalPlayer.control = Address::GetLocalPlayerControl();

	LocalPlayer.pawn = Get::PlayerPawnAddress(LocalPlayer.control);

	Vector3 lpPos = Get::LastCameraPos(LocalPlayer.pawn);

	const Vector3 deltaPos{ pTargetPos - lpPos };

	const float distPos{ GetMagnitude(deltaPos) };

	constexpr float radToDegree{ 57.295776f };
	targetAngle.x = -asinf(deltaPos.z / distPos) * radToDegree;
	targetAngle.y = atan2f(deltaPos.y, deltaPos.x) * radToDegree;

	NormalizePitch(targetAngle.x);

	return targetAngle;
}

void Aimbot::ReleaseMouseButton()
{
	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
}

bool Aimbot::ShotTarget()
{
	static Vector3 vecSmooth; 
	Vector3 targetAngle{};
	Vector3 LastAngles = {};
	Player LocalPlayer{};
	float steps = Menu::Aimbot::Smooth * 0.003f; 



	LocalPlayer.control = Address::GetLocalPlayerControl();


	LocalPlayer.pawn = Get::PlayerPawnAddress(LocalPlayer.control);

	LastAngles = LocalPlayer::GetViewAngles(); // 获取当前视角角度
	targetAngle = Aimbot::GetTargetAngle(Get::BonePos(Target::addr, Menu::Aimbot::AimPos)); // 获取目标角度

	Vector3 delta = targetAngle - vecSmooth;


	// 设置最大角度变化
	float maxAngleChange = 60.0f; // 可以根据需要动态调整

	// 限制角度变化
	if (delta.Length() > maxAngleChange) {
		delta = delta.Normalized() * maxAngleChange; 
	}

	// 更新 vecSmooth
	vecSmooth = vecSmooth + delta;


		if (steps <= 0)
		{
			LocalPlayer::SetViewAngles(targetAngle);

		}
		else
		{
			vecSmooth = vecSmooth.Lerp(targetAngle, steps);
			LocalPlayer::SetViewAngles(vecSmooth);
		}
		ShotFired = true;
	



}

bool Aimbot::Start()
{
	bool CanAim = GetBestTarget();

	if (ShotFired)
	{

		ShotFired = false;
	}



	if (GetAsyncKeyState(Menu::Aimbot::AimKey) && CanAim)
	{
		if (Get::PlayerAlive(Target::addr))
		{
			ShotTarget();
		}
		else
			Target::addr = {};
	}
	else
	{
		Target::addr = 0;
	}

	if (!GetAsyncKeyState(Menu::Aimbot::AimKey)) {
		// 允许鼠标控制的视角不被修改
		LastAngles = LocalPlayer::GetViewAngles(); // 获取当前视角
		LocalPlayer::SetViewAngles(LastAngles);
	}






}

void Aimbot::DrawAimbotFOV()
{
	ImVec2 windowSize = ImGui::GetIO().DisplaySize;
	ImVec2 screenCenter = ImVec2(windowSize.x / 2, windowSize.y / 2);

	int aimSize = Menu::Aimbot::AimSize;

	ImVec2 rectMin = ImVec2(screenCenter.x - aimSize, screenCenter.y - aimSize);
	ImVec2 rectMax = ImVec2(screenCenter.x + aimSize, screenCenter.y + aimSize);

	float thickness = 2.0f; // 矩形边框厚度

	ImDrawList* drawList = ImGui::GetForegroundDrawList();
	drawList->AddRect(rectMin, rectMax, Menu::Aimbot::rectColor, 0.0f, ImDrawFlags_None, thickness);

}

