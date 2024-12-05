#include "mem Aimbot.h"
#include "imgui/imgui.h"
#include "GameTace.hpp"
#include "menu.h"
#include "Search.h"
#include "mem.hpp"
#include "CUserCmd.h"
#include "weaponcheak.hpp"

#include "CCSGOInput.hpp"

Vector3 LastAngles{};
bool ShotFired = false;
Vector3 WantAngele{};


mem::Process cs2(TEXT("cs2.exe"));
auto client = cs2.get_module_handle(TEXT("client.dll"));

std::optional<Vector3> GetLocalEye()  noexcept {
	if (!client)
		return std::nullopt;

	Player LocalPlayer{};

	LocalPlayer.control = Address::GetLocalPlayerControl();

	LocalPlayer.pawn = Get::PlayerPawnAddress(LocalPlayer.control);


	auto* Origin = reinterpret_cast<Vector3*>(LocalPlayer.pawn + cs2_dumper::schemas::client_dll::C_BasePlayerPawn::m_vOldOrigin);
	auto* ViewOffset = reinterpret_cast<Vector3*>(LocalPlayer.pawn + cs2_dumper::schemas::client_dll::C_BaseModelEntity::m_vecViewOffset);

	Vector3 LocalEye = *Origin + *ViewOffset;
	printf(" LocalEye Vec3   x: %f y : %f z : %f\n", LocalEye.x, LocalEye.y, LocalEye.z);
	if (!std::isfinite(LocalEye.x) || !std::isfinite(LocalEye.y) || !std::isfinite(LocalEye.z))
		return std::nullopt;

	if (LocalEye.Length() < 0.1f)
		return std::nullopt;

	return LocalEye;
}

std::optional<Vector3> GetEntityEye(const Player& Entity) noexcept {
	if (!Entity.pawn)
		return std::nullopt;

	auto* Origin = reinterpret_cast<Vector3*>(Entity.pawn + cs2_dumper::schemas::client_dll::C_BasePlayerPawn::m_vOldOrigin);
	auto* ViewOffset = reinterpret_cast<Vector3*>(Entity.pawn + cs2_dumper::schemas::client_dll::C_BaseModelEntity::m_vecViewOffset);

	Vector3 Result = *Origin + *ViewOffset;
	if (!std::isfinite(Result.x) || !std::isfinite(Result.y) || !std::isfinite(Result.z))
		return std::nullopt;

	return Result;
}


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



	bool bHasScanTarget = false;
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
		auto LocalEye = GetLocalEye();
		auto EntityEye = GetEntityEye(Entity);



		if (Menu::Aimbot::bVisibleCheck) {
			void* pawnPtr = reinterpret_cast<void*>(LocalPlayer.pawn);
			CGameTrace pTrace = CEngineTrace::Get()->TraceShape(LocalEye.value(), EntityEye.value(), pawnPtr);
			if (!pTrace.IsVisible()) {
				continue;
			}
		}

		Vector3 AimPos = Get::BonePos(Entity.pawn, Menu::Aimbot::AimPos);
		printf("aimpos Vec3  x: %f y : %f z : %f\n", AimPos.x, AimPos.y, AimPos.z);
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
				bHasScanTarget = true;
				LastDistance = Distance;
				Target::addr= Entity.pawn;
			}
		}
	}

	return bHasScanTarget;
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
	Player LocalPlayer{};
	float steps = Menu::Aimbot::Smooth * 0.01f; 

	// setting
	bool bSilent = Menu::Aimbot::bSilent;
	bool bAutoRecoil = Menu::Aimbot::bAutoRecoil;
	bool bPrefectSilent = Menu::Aimbot::bPrefectSilent;
	float flRecoilAmount = Menu::Aimbot::flRecoilAmount; // 100%

	LocalPlayer.control = Address::GetLocalPlayerControl();


	LocalPlayer.pawn = Get::PlayerPawnAddress(LocalPlayer.control);
	CUtlVector<Vector3>& vecAimPunchs = Get::GetAimPunch(LocalPlayer.pawn);

	Vector3 vecCameraAngles = CCSGOInput::Get()->GetViewAngles();
	Vector3 targetAngle = Aimbot::GetTargetAngle(Get::BonePos(Target::addr, Menu::Aimbot::AimPos)); 
	if (vecAimPunchs.Count() > 0 && vecAimPunchs.Count() < 0xFFFF) {
		Vector3 vecAimPunch = vecAimPunchs.Element(vecAimPunchs.Count() - 1);
		targetAngle -= ((vecAimPunch * 2.f) * flRecoilAmount);
	}

	if (bSilent) {
		CUserCMD* pUserCmd = CUserCMD::Get();
		if (pUserCmd) {
			pUserCmd->SetHistoryAngles(targetAngle);
			CBaseUserCmd* pBaseCmd = pUserCmd->csgoUserCmd.pBaseCmd;
			if (pBaseCmd && pBaseCmd->pViewAngles && !bPrefectSilent) {
				pBaseCmd->pViewAngles->m_view_angles = targetAngle;
			}

			if (pUserCmd->nButtons.nValue & IN_ATTACK) {
				pUserCmd->InvalidatetAttackIndex();
			}
		}

	} else {
		Vector3 delta = targetAngle - vecCameraAngles;
		Vector3 vecSmoothAngles = delta * (1.000001f - steps);
		Vector3 vecTargetAngles = vecCameraAngles + vecSmoothAngles;
		if (steps <= 0.f) {
			vecTargetAngles = targetAngle;
		}

		CCSGOInput::Get()->SetViewAngles(vecTargetAngles);
	}


	ShotFired = true;
}

bool Aimbot::Start()
{
	bool CanAim = GetBestTarget();

	if (ShotFired)
        ShotFired = !ShotFired;


	if (GetAsyncKeyState(Menu::Aimbot::AimKey)|| Menu::Aimbot::AimbotAlwaysOn && CanAim && !Menu::ShowMenu) {
		if (Get::PlayerAlive(Target::addr)) {
			ShotTarget();
		} else {
			Target::addr = 0;
		}

	} else {
		Target::addr = 0;
	}
	


}

void Aimbot::DrawAimbotFOV()
{
	ImVec2 windowSize = ImGui::GetIO().DisplaySize;
	ImVec2 screenCenter = ImVec2(windowSize.x / 2, windowSize.y / 2);

	int aimSize = Menu::Aimbot::AimSize;

	float thickness = 2.0f; 

	ImDrawList* drawList = ImGui::GetForegroundDrawList();
	drawList->AddCircle(screenCenter, aimSize, Menu::Aimbot::rectColor, 1024, thickness);
}

void Aimbot::Tiggerbot()
{
	Player LocalPlayer{};
	LocalPlayer.control = Address::GetLocalPlayerControl();

	if (!LocalPlayer.control)
		return;

	LocalPlayer.pawn = Get::PlayerPawnAddress(LocalPlayer.control);

	if (!LocalPlayer.pawn)
		return;

	LocalPlayer.team = Get::PlayerTeam(LocalPlayer.pawn);

	if (!Get::PlayerAlive(LocalPlayer.pawn))
		return;

	int LastDistance = 999999999;

	for (int i{ 0 }; i < 64; ++i)
	{
		Player Entity{};
		Entity.control = Address::GetEntityBase(i);

		if (!Entity.control || !Get::PawnAlive(Entity.control))
			continue;

		Entity.pawn = Get::PlayerPawnAddress(Entity.control);

		if (!Entity.pawn || Entity.pawn == LocalPlayer.pawn)
			continue;

		Entity.team = Get::PlayerTeam(Entity.pawn);
		Entity.health = Get::PlayerHealth(Entity.pawn);

		if (Get::IsDormant(Entity.pawn))
			continue;

		if (Entity.team == LocalPlayer.team && !Menu::Aimbot::Team)
			continue;

		if (Entity.team != 2 && Entity.team != 3 && !Menu::Aimbot::Team)
			continue;

		auto LocalEye = GetLocalEye();
		auto EntityEye = GetEntityEye(Entity);

		if (!LocalEye || !EntityEye)
			continue;

		void* pawnPtr = reinterpret_cast<void*>(LocalPlayer.pawn);
		CGameTrace pTrace = CEngineTrace::Get()->TraceShape(LocalEye.value(), EntityEye.value(), pawnPtr);
		if (!pTrace.IsVisible())
			continue;

		Vector3 AimPos = Get::BonePos(Entity.pawn, Menu::Aimbot::AimPos);
		Vector3 EndPos{};
		Vector3 Window = Get::WindowSize();

		if (!Utils::WorldToScreen(AimPos, EndPos, Address::GetViewMatrixPtr(), Window.x, Window.y))
			continue;

		ImVec2 windowSize = ImGui::GetIO().DisplaySize;
		ImVec2 screenCenter = ImVec2(windowSize.x / 2, windowSize.y / 2);
		int TiggerSize = 5;

		if (EndPos.x > screenCenter.x - TiggerSize && EndPos.x < screenCenter.x + TiggerSize &&
			EndPos.y > screenCenter.y - TiggerSize && EndPos.y < screenCenter.y + TiggerSize)
		{
			int Distance = Math::distance(screenCenter.x - EndPos.x, screenCenter.y - EndPos.y);

			if (Distance < LastDistance && GetAsyncKeyState(Menu::Tiggerbot::HotKey) && !Menu::ShowMenu && weaponcheck() == true)
			{
				LastDistance = Distance;
				static auto lastShotTime = std::chrono::high_resolution_clock::now();
				auto currentTime = std::chrono::high_resolution_clock::now();
				auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastShotTime).count();
				if (elapsedTime >= Menu::Tiggerbot::TriggerTargetDelay) {
					mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
					mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
					lastShotTime = std::chrono::high_resolution_clock::now();
					break; 
				}
			}
		}
	}
}

void Aimbot::DrawTiggerbotFOV()
{
	ImVec2 windowSize = ImGui::GetIO().DisplaySize;
	ImVec2 screenCenter = ImVec2(windowSize.x / 2, windowSize.y / 2);

	int aimSize = 3;

	float thickness = 2.0f;

	ImDrawList* drawList = ImGui::GetForegroundDrawList();
	drawList->AddCircle(screenCenter, aimSize, Menu::Tiggerbot::rectColor, 1024, thickness);
}

void Aimbot::AutoFire()
{
	Player LocalPlayer{};
	LocalPlayer.control = Address::GetLocalPlayerControl();

	if (!LocalPlayer.control)
		return;

	LocalPlayer.pawn = Get::PlayerPawnAddress(LocalPlayer.control);

	if (!LocalPlayer.pawn)
		return;

	LocalPlayer.team = Get::PlayerTeam(LocalPlayer.pawn);

	if (!Get::PlayerAlive(LocalPlayer.pawn))
		return;

	int LastDistance = 999999999;

	for (int i{ 0 }; i < 64; ++i)
	{
		Player Entity{};
		Entity.control = Address::GetEntityBase(i);

		if (!Entity.control || !Get::PawnAlive(Entity.control))
			continue;

		Entity.pawn = Get::PlayerPawnAddress(Entity.control);

		if (!Entity.pawn || Entity.pawn == LocalPlayer.pawn)
			continue;

		Entity.team = Get::PlayerTeam(Entity.pawn);
		Entity.health = Get::PlayerHealth(Entity.pawn);

		if (Get::IsDormant(Entity.pawn))
			continue;

		if (Entity.team == LocalPlayer.team && !Menu::Aimbot::Team)
			continue;

		if (Entity.team != 2 && Entity.team != 3 && !Menu::Aimbot::Team)
			continue;

		auto LocalEye = GetLocalEye();
		auto EntityEye = GetEntityEye(Entity);

		if (!LocalEye || !EntityEye)
			continue;

		void* pawnPtr = reinterpret_cast<void*>(LocalPlayer.pawn);
		CGameTrace pTrace = CEngineTrace::Get()->TraceShape(LocalEye.value(), EntityEye.value(), pawnPtr);
		if (!pTrace.IsVisible())
			continue;

		Vector3 AimPos = Get::BonePos(Entity.pawn, Menu::Aimbot::AimPos);
		Vector3 EndPos{};
		Vector3 Window = Get::WindowSize();

		if (!Utils::WorldToScreen(AimPos, EndPos, Address::GetViewMatrixPtr(), Window.x, Window.y))
			continue;

		ImVec2 windowSize = ImGui::GetIO().DisplaySize;
		ImVec2 screenCenter = ImVec2(windowSize.x / 2, windowSize.y / 2);
		int FireSize = Menu::Aimbot::AimSize;

		if (EndPos.x > screenCenter.x - FireSize && EndPos.x < screenCenter.x + FireSize &&
			EndPos.y > screenCenter.y - FireSize && EndPos.y < screenCenter.y + FireSize)
		{

			if (Menu::Tiggerbot::AutoFire && Menu::Aimbot::bSilent && !Menu::ShowMenu && weaponcheck() == TRUE)
			{
				static auto lastShotTime = std::chrono::high_resolution_clock::now();
				auto currentTime = std::chrono::high_resolution_clock::now();
				auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastShotTime).count();
				if (elapsedTime >= Menu::Tiggerbot::TriggerTargetDelay) {
					mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
					mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
					lastShotTime = std::chrono::high_resolution_clock::now();
					break;
				}
			}
		}

	}
}