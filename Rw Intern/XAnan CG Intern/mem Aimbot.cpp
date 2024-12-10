#include "mem Aimbot.h"
#include "imgui/imgui.h"
#include "GameTace.hpp"
#include "menu.h"
#include "Search.h"
#include "mem.hpp"
#include "CUserCmd.h"
#include "weaponcheak.hpp"
#include "Entity.h"

#include "CCSGOInput.hpp"

Vector3 LastAngles{};
bool ShotFired = false;
Vector3 WantAngele{};
int32_t AimPos;


mem::Process cs2(TEXT("cs2.exe"));
auto client = cs2.get_module_handle(TEXT("client.dll"));

std::optional<Vector3> GetLocalEye()  noexcept {
	if (!client)
		return std::nullopt;

	Player LocalPlayer{};

	LocalPlayer.control = Address::GetLocalPlayerControl();

	LocalPlayer.pawn = Get::PlayerPawnAddress(LocalPlayer.control);

	uintptr_t pGameSceneNode = *reinterpret_cast<uintptr_t*>(LocalPlayer.pawn + cs2_dumper::schemas::client_dll::C_BaseEntity::m_pGameSceneNode);

	auto* Origin = reinterpret_cast<Vector3*>(pGameSceneNode + cs2_dumper::schemas::client_dll::CGameSceneNode::m_vecOrigin);
	auto* ViewOffset = reinterpret_cast<Vector3*>(LocalPlayer.pawn + cs2_dumper::schemas::client_dll::C_BaseModelEntity::m_vecViewOffset);

	Vector3 LocalEye = *Origin + *ViewOffset;
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

		int aimSize = Menu::Aimbot::AimSize;

		//头部
		Vector3 HeadPos = Get::BonePos(Entity.pawn, BoneIndex::head);
		//躯干
		Vector3 NeckPos = Get::BonePos(Entity.pawn, BoneIndex::neck_0);
		Vector3 Spine1Pos = Get::BonePos(Entity.pawn, BoneIndex::spine_1);
		Vector3 Spine2Pos = Get::BonePos(Entity.pawn, BoneIndex::spine_2);
		Vector3 PelvisPos = Get::BonePos(Entity.pawn, BoneIndex::pelvis);
		//左手臂
		Vector3 ArmUpperLPos = Get::BonePos(Entity.pawn, BoneIndex::arm_upper_L);
		Vector3 ArmLowerLPos = Get::BonePos(Entity.pawn, BoneIndex::arm_lower_L);
		Vector3 handLPos = Get::BonePos(Entity.pawn, BoneIndex::hand_L);
		//右手臂
		Vector3 ArmUpperRPos = Get::BonePos(Entity.pawn, BoneIndex::arm_upper_R);
		Vector3 ArmLowerRPos = Get::BonePos(Entity.pawn, BoneIndex::arm_lower_R);
		Vector3 handRPos = Get::BonePos(Entity.pawn, BoneIndex::hand_R);
		//左腿
		Vector3 LegUpperLPos = Get::BonePos(Entity.pawn, BoneIndex::leg_upper_L);
		Vector3 LegLowerLPos = Get::BonePos(Entity.pawn, BoneIndex::leg_lower_L);
		Vector3 AnkleLPos = Get::BonePos(Entity.pawn, BoneIndex::ankle_L);
		//右腿
		Vector3 LegUpperRPos = Get::BonePos(Entity.pawn, BoneIndex::leg_upper_R);
		Vector3 LegLowerRPos = Get::BonePos(Entity.pawn, BoneIndex::leg_lower_R);
		Vector3 AnkleRPos = Get::BonePos(Entity.pawn, BoneIndex::ankle_R);


		Vector3 TempAimPos;
		Vector3 EndPos{};
		Vector3 Window = Get::WindowSize();


		if (Menu::Aimbot::bVisibleCheck) {
			//多部位可视判断
			void* pawnPtr = reinterpret_cast<void*>(LocalPlayer.pawn);
			//头部可视判断
			CGameTrace EyepTrace = CEngineTrace::Get()->TraceShape(LocalEye.value(), EntityEye.value(), pawnPtr);
			//躯干可视判断
			CGameTrace NeckpTrace = CEngineTrace::Get()->TraceShape(LocalEye.value(), NeckPos, pawnPtr);
			CGameTrace Spine1pTrace = CEngineTrace::Get()->TraceShape(LocalEye.value(), Spine1Pos, pawnPtr);
			CGameTrace Spine2pTrace = CEngineTrace::Get()->TraceShape(LocalEye.value(), Spine2Pos, pawnPtr);
			CGameTrace PelvispTrace = CEngineTrace::Get()->TraceShape(LocalEye.value(), PelvisPos, pawnPtr);
			//左手臂可视判断
			CGameTrace ArmUpperLpTrace = CEngineTrace::Get()->TraceShape(LocalEye.value(), ArmUpperLPos, pawnPtr);
			CGameTrace ArmLowerLpTrace = CEngineTrace::Get()->TraceShape(LocalEye.value(), ArmLowerLPos, pawnPtr);
			CGameTrace handLpTrace = CEngineTrace::Get()->TraceShape(LocalEye.value(), handLPos, pawnPtr);
			//右手臂可视判断
			CGameTrace ArmUpperRpTrace = CEngineTrace::Get()->TraceShape(LocalEye.value(), ArmUpperRPos, pawnPtr);
			CGameTrace ArmLowerRpTrace = CEngineTrace::Get()->TraceShape(LocalEye.value(), ArmLowerRPos, pawnPtr);
			CGameTrace handRpTrace = CEngineTrace::Get()->TraceShape(LocalEye.value(), handRPos, pawnPtr);
			//左腿可视判断
			CGameTrace LegUpperLpTrace = CEngineTrace::Get()->TraceShape(LocalEye.value(), LegUpperLPos, pawnPtr);
			CGameTrace LegLowerLpTrace = CEngineTrace::Get()->TraceShape(LocalEye.value(), LegLowerLPos, pawnPtr);
			CGameTrace AnkleLpTrace = CEngineTrace::Get()->TraceShape(LocalEye.value(), AnkleLPos, pawnPtr);
			//右腿可视判断
			CGameTrace LegUpperRpTrace = CEngineTrace::Get()->TraceShape(LocalEye.value(), LegUpperRPos, pawnPtr);
			CGameTrace LegLowerRpTrace = CEngineTrace::Get()->TraceShape(LocalEye.value(), LegLowerRPos, pawnPtr);
			CGameTrace AnkleRpTrace = CEngineTrace::Get()->TraceShape(LocalEye.value(), AnkleRPos, pawnPtr);



			if (!EyepTrace.IsVisible() &&
				!NeckpTrace.IsVisible() && !Spine1pTrace.IsVisible() && !Spine2pTrace.IsVisible() && !PelvispTrace.IsVisible() &&
				!ArmUpperLpTrace.IsVisible() && !ArmLowerLpTrace.IsVisible() && !handLpTrace.IsVisible() &&
				!ArmUpperRpTrace.IsVisible() && !ArmLowerRpTrace.IsVisible() && !handRpTrace.IsVisible() &&
				!LegUpperLpTrace.IsVisible() && !LegLowerLpTrace.IsVisible() && !AnkleLpTrace.IsVisible() &&
				!LegUpperRpTrace.IsVisible() && !LegLowerRpTrace.IsVisible() && !AnkleRpTrace.IsVisible())
				continue;


			if (Menu::SafeMode) {
				if (EyepTrace.IsVisible()) {
					TempAimPos = HeadPos;
					if (!Utils::WorldToScreen(TempAimPos, EndPos, Address::GetViewMatrixPtr(), Window.x, Window.y))
						continue;
					int left = Window.x / 2 - EndPos.x;
					int right = Window.y / 2 - EndPos.y;
					float Distance = Math::distance(left, right);

					printf("现在锁头\n");
					if (Distance <= aimSize) {
						AimPos = BoneIndex::head;
						Target::addr = Entity.pawn;
						bHasScanTarget = true;

					}

				}
				else if (NeckpTrace.IsVisible() || Spine1pTrace.IsVisible() || Spine2pTrace.IsVisible() || PelvispTrace.IsVisible()) {
					printf("现在锁躯干\n");
					Vector3 BonePositions[4] = {
					  NeckPos,
					  Spine1Pos,
					  Spine2Pos,
					  PelvisPos
					};
					int32_t BoneTempIndex[4] = {
						BoneIndex::neck_0,
						BoneIndex::spine_1,
						BoneIndex::spine_2,
						BoneIndex::pelvis
					};
					for (int i{ 0 }; i < 4; ++i) {
						TempAimPos = BonePositions[i]; // 当前骨骼点位置
						// 骨骼点转屏幕坐标
						if (!Utils::WorldToScreen(TempAimPos, EndPos, Address::GetViewMatrixPtr(), Window.x, Window.y))
							continue;
						int left = Window.x / 2 - EndPos.x;
						int right = Window.y / 2 - EndPos.y;
						float Distance = Math::distance(left, right);
						if (Distance < LastDistance && Distance <= aimSize) {
							LastDistance = Distance;
							AimPos = BoneTempIndex[i]; // 瞄准点更新为当前骨骼点
							Target::addr = Entity.pawn;
							bHasScanTarget = true;

						}
					}

				}
				else if (ArmUpperLpTrace.IsVisible() || ArmLowerLpTrace.IsVisible() || handLpTrace.IsVisible() ||
					      ArmUpperRpTrace.IsVisible() || ArmLowerRpTrace.IsVisible() || handRpTrace.IsVisible() ||
					      LegUpperLpTrace.IsVisible() || LegLowerLpTrace.IsVisible() || AnkleLpTrace.IsVisible() ||
					      LegUpperRpTrace.IsVisible() || LegLowerRpTrace.IsVisible() || AnkleRpTrace.IsVisible()) {
					       printf("现在锁四肢\n");
					       Vector3 BonePositions[12] = {
						          ArmUpperLPos,
					              ArmLowerLPos,
					                handLPos,
					             ArmUpperRPos,
					             ArmLowerRPos,
					              handRPos,
					              LegUpperLPos,
					            LegLowerLPos,
					              AnkleLPos,
					           LegUpperRPos,
					            LegLowerRPos,
					           AnkleRPos

					       };
			          		int32_t BoneTempIndex[12] = {
		          			   BoneIndex::arm_upper_L,
			           		   BoneIndex::arm_lower_L,
		         			   BoneIndex::hand_L,
		          			   BoneIndex::arm_upper_R,
		        			   BoneIndex::arm_lower_R,
		          			   BoneIndex::hand_R,
		         			   BoneIndex::leg_upper_L,
		           			   BoneIndex::leg_lower_L,
		         			   BoneIndex::ankle_L,
		          			   BoneIndex::leg_upper_R,
		         			   BoneIndex::leg_lower_R,
		         			   BoneIndex::ankle_R
		        			};

					        for (int i{ 0 }; i < 12; ++i) {
					         	TempAimPos = BonePositions[i]; // 当前骨骼点位置
				          		if (!Utils::WorldToScreen(TempAimPos, EndPos, Address::GetViewMatrixPtr(), Window.x, Window.y))
				        			continue;
					        	int left = Window.x / 2 - EndPos.x;
					        	int right = Window.y / 2 - EndPos.y;
				         		float Distance = Math::distance(left, right);
				          		if (Distance < LastDistance && Distance <= aimSize) {
				           			LastDistance = Distance;
				          			AimPos = BoneTempIndex[i]; // 瞄准点更新为当前骨骼点
				           			Target::addr = Entity.pawn;
				          			printf("最近四肢的Bone索引是： %d\n", BoneTempIndex[i]);
				          			bHasScanTarget = true;

			           			}
			        		}


				     }
			}
			else {
				if (EyepTrace.IsVisible()) {
					TempAimPos = HeadPos;
					printf("现在锁头\n");
					AimPos = BoneIndex::head;
					Target::addr = Entity.pawn;
					bHasScanTarget = true;

				}
				else if (NeckpTrace.IsVisible() || Spine1pTrace.IsVisible() || Spine2pTrace.IsVisible() || PelvispTrace.IsVisible()) {
					printf("现在锁躯干\n");
					Vector3 BonePositions[4] = {
					  NeckPos,
					  Spine1Pos,
					  Spine2Pos,
					  PelvisPos
					};
					int32_t BoneTempIndex[4] = {
						BoneIndex::neck_0,
						BoneIndex::spine_1,
						BoneIndex::spine_2,
						BoneIndex::pelvis
					};
					for (int i{ 0 }; i < 4; ++i) {
						TempAimPos = BonePositions[i]; // 当前骨骼点位置
						// 骨骼点转屏幕坐标
						if (!Utils::WorldToScreen(TempAimPos, EndPos, Address::GetViewMatrixPtr(), Window.x, Window.y))
							continue;
						int left = Window.x / 2 - EndPos.x;
						int right = Window.y / 2 - EndPos.y;
						float Distance = Math::distance(left, right);
						if (Distance < LastDistance) {
							LastDistance = Distance;
							AimPos = BoneTempIndex[i]; // 瞄准点更新为当前骨骼点
							Target::addr = Entity.pawn;
							bHasScanTarget = true;

						}
					}

				}
				else if (ArmUpperLpTrace.IsVisible() || ArmLowerLpTrace.IsVisible() || handLpTrace.IsVisible() ||
				      	ArmUpperRpTrace.IsVisible() || ArmLowerRpTrace.IsVisible() || handRpTrace.IsVisible() ||
				     	LegUpperLpTrace.IsVisible() || LegLowerLpTrace.IsVisible() || AnkleLpTrace.IsVisible() ||
				      	LegUpperRpTrace.IsVisible() || LegLowerRpTrace.IsVisible() || AnkleRpTrace.IsVisible()) {
				    	printf("现在锁四肢\n");
			       		Vector3 BonePositions[12] = {
			      			   ArmUpperLPos,
			    			   ArmLowerLPos,
			      				 handLPos,
		    				  ArmUpperRPos,
			     			  ArmLowerRPos,
		    				   handRPos,
		       				   LegUpperLPos,
			     			 LegLowerLPos,
			     			   AnkleLPos,
			     			LegUpperRPos,
			     			 LegLowerRPos,
		    				AnkleRPos

		       			};
				      	int32_t BoneTempIndex[12] = {
				       	   BoneIndex::arm_upper_L,
				     	   BoneIndex::arm_lower_L,
				     	   BoneIndex::hand_L,
				     	   BoneIndex::arm_upper_R,
				     	   BoneIndex::arm_lower_R,
				      	   BoneIndex::hand_R,
				    	   BoneIndex::leg_upper_L,
				    	   BoneIndex::leg_lower_L,
			     		   BoneIndex::ankle_L,
			     		   BoneIndex::leg_upper_R,
			      		   BoneIndex::leg_lower_R,
			    		   BoneIndex::ankle_R
			     		};

				     	for (int i{ 0 }; i < 12; ++i) {
				    		TempAimPos = BonePositions[i]; // 当前骨骼点位置
				    		if (!Utils::WorldToScreen(TempAimPos, EndPos, Address::GetViewMatrixPtr(), Window.x, Window.y))
				    			continue;
			      			int left = Window.x / 2 - EndPos.x;
				       		int right = Window.y / 2 - EndPos.y;
			     			float Distance = Math::distance(left, right);
			    			if (Distance < LastDistance) {
			     				LastDistance = Distance;
			      				AimPos = BoneTempIndex[i]; // 瞄准点更新为当前骨骼点
			       				Target::addr = Entity.pawn;
			     				printf("最近四肢的Bone索引是： %d\n", BoneTempIndex[i]);
		    					bHasScanTarget = true;

				    		}
				    	}


				     }
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


bool Aimbot::ShotTarget() {
	static auto lastShotTime = std::chrono::high_resolution_clock::now(); // 初始化上次开枪时间
	static bool bFlipFire = true;

	CUserCMD* pUserCmd = CUserCMD::Get();
	if (!pUserCmd) {
		return false;
	}

	Player LocalPlayer{};
	float steps = Menu::Aimbot::Smooth * 0.01f;

	// Setting
	bool bSilent = Menu::Aimbot::bSilent;
	bool bAutoFire = true;
	bool bAutoRecoil = Menu::Aimbot::bAutoRecoil;
	bool bPrefectSilent = Menu::Aimbot::bPrefectSilent;
	float flRecoilAmount = Menu::Aimbot::flRecoilAmount; // 100%

	LocalPlayer.control = Address::GetLocalPlayerControl();
	LocalPlayer.pawn = Get::PlayerPawnAddress(LocalPlayer.control);
	CUtlVector<Vector3>& vecAimPunchs = Get::GetAimPunch(LocalPlayer.pawn);

	Vector3 vecCameraAngles = CCSGOInput::Get()->GetViewAngles();
	Vector3 targetAngle = Aimbot::GetTargetAngle(Get::BonePos(Target::addr, AimPos));

	// Recoil compensation
	if (vecAimPunchs.Count() > 0 && vecAimPunchs.Count() < 0xFFFF && bAutoRecoil) {
		Vector3 vecAimPunch = vecAimPunchs.Element(vecAimPunchs.Count() - 1);
		targetAngle -= ((vecAimPunch * 2.f) * flRecoilAmount);
	}

	// Silent aimbot logic
	if (bSilent && !Menu::SafeMode) {
		pUserCmd->SetHistoryAngles(targetAngle);
		CBaseUserCmd* pBaseCmd = pUserCmd->csgoUserCmd.pBaseCmd;
		if (pBaseCmd && pBaseCmd->pViewAngles && !bPrefectSilent) {
			pBaseCmd->pViewAngles->m_view_angles = targetAngle;
		}

		if (pUserCmd->nButtons.nValue & IN_ATTACK) {
			pUserCmd->InvalidatetAttackIndex();
		}
	}
	else {
		Vector3 delta = targetAngle - vecCameraAngles;
		Vector3 vecSmoothAngles = delta * (1.000001f - steps);
		Vector3 vecTargetAngles = vecCameraAngles + vecSmoothAngles;
		if (steps <= 0.f) {
			vecTargetAngles = targetAngle;
		}

		CCSGOInput::Get()->SetViewAngles(vecTargetAngles);
	}

	// Auto fire logic with delay
	if (bAutoFire && bFlipFire) {
		auto now = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> elapsed = now - lastShotTime;

		if (elapsed.count() >= Menu::Tiggerbot::TriggerTargetDelay) { // FireDelay 是设置的延迟时间，单位秒
			pUserCmd->nButtons.nValue |= IN_ATTACK;
			lastShotTime = now; // 更新上次开枪时间
		}
	}

	bFlipFire = !bFlipFire;
	ShotFired = true;
	return true;
}
bool Aimbot::Start()
{
	bool CanAim = GetBestTarget();

	if (ShotFired)
        ShotFired = !ShotFired;

	if (!Menu::SafeMode) {
		if (GetAsyncKeyState(Menu::Aimbot::AimKey) || Menu::Aimbot::AimbotAlwaysOn && CanAim && !Menu::ShowMenu) {
			if (Get::PlayerAlive(Target::addr)) {
				ShotTarget();
			}
			else {
				Target::addr = 0;
			}

		}
		else {
			Target::addr = 0;
		}
	}
	else {
		if (GetAsyncKeyState(Menu::Aimbot::AimKey) && CanAim && !Menu::ShowMenu) {
			if (Get::PlayerAlive(Target::addr)) {
				ShotTarget();
			}
			else {
				Target::addr = 0;
			}

		}
		else {
			Target::addr = 0;
		}
	}
	
	return Target::addr != 0;

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

void Aimbot::OnTriggerBot() {
	CUserCMD* pUserCmd = CUserCMD::Get();
	CCSGOInput* pCSGOInput = CCSGOInput::Get();
	if (!pUserCmd || !pCSGOInput) {
		return;
	}

	static bool bFlipFire = true;
	bool bAllowTriggerBot = false;
	if (GetAsyncKeyState(Menu::Tiggerbot::HotKey) && !Menu::ShowMenu && weaponcheck()) {
		bAllowTriggerBot = true;
	}

	if (!bAllowTriggerBot) {
		return;
	}

	Player LocalPlayer{};
	LocalPlayer.control = Address::GetLocalPlayerControl();

	if (!LocalPlayer.control)
		return;

	LocalPlayer.pawn = Get::PlayerPawnAddress(LocalPlayer.control);

	if (!LocalPlayer.pawn)
		return;

	auto LocalEye = GetLocalEye();

	Vector3 vecForward = {};
	Vector3 vecCameraAngles = pCSGOInput->GetViewAngles();
	vecCameraAngles.AnglesToVectors(&vecForward);
	const float flLength = 8192.f; // 最大武器攻击距离, 值: CCSGOWeaponVData -> flRange
	Vector3 vecFinal = LocalEye.value() + (vecForward * flLength);
	CGameTrace pTrace = CEngineTrace::Get()->TraceShape(LocalEye.value(), vecFinal, (void*)LocalPlayer.pawn, 0x1C300B);
	if (!pTrace.pHitEntity || !Get::IsPlayer((intptr_t)pTrace.pHitEntity)) {
		return;
	}

	LocalPlayer.team = Get::PlayerTeam(LocalPlayer.pawn);
	int nTargetTeam = Get::PlayerTeam((intptr_t)pTrace.pHitEntity);
	if (LocalPlayer.team == nTargetTeam) {
		return;
	}

	if (bFlipFire) {
		pUserCmd->nButtons.nValue |= IN_ATTACK;
	}

	bFlipFire = !bFlipFire;
}

//void Aimbot::Tiggerbot()
//{
//	bool shouldShoot = false;//射击变量
//	Player LocalPlayer{};
//	LocalPlayer.control = Address::GetLocalPlayerControl();
//
//	if (!LocalPlayer.control)
//		return;
//
//	LocalPlayer.pawn = Get::PlayerPawnAddress(LocalPlayer.control);
//
//	if (!LocalPlayer.pawn)
//		return;
//
//	LocalPlayer.team = Get::PlayerTeam(LocalPlayer.pawn);
//
//	if (!Get::PlayerAlive(LocalPlayer.pawn))
//		return;
//
//	int LastDistance = 999999999;
//
//	for (int i{ 0 }; i < 64; ++i)
//	{
//		Player Entity{};
//		Entity.control = Address::GetEntityBase(i);
//
//		if (!Entity.control || !Get::PawnAlive(Entity.control))
//			continue;
//
//		Entity.pawn = Get::PlayerPawnAddress(Entity.control);
//
//		if (!Entity.pawn || Entity.pawn == LocalPlayer.pawn)
//			continue;
//
//		Entity.team = Get::PlayerTeam(Entity.pawn);
//		Entity.health = Get::PlayerHealth(Entity.pawn);
//
//		if (Get::IsDormant(Entity.pawn))
//			continue;
//
//		if (Entity.team == LocalPlayer.team && !Menu::Aimbot::Team)
//			continue;
//
//		if (Entity.team != 2 && Entity.team != 3 && !Menu::Aimbot::Team)
//			continue;
//
//		auto LocalEye = GetLocalEye();
//		auto EntityEye = GetEntityEye(Entity);
//
//		if (!LocalEye || !EntityEye)
//			continue;
//
//
//		//头部
//		Vector3 HeadPos = Get::BonePos(Entity.pawn, BoneIndex::head);
//		//躯干
//		Vector3 NeckPos = Get::BonePos(Entity.pawn, BoneIndex::neck_0);
//		Vector3 Spine1Pos = Get::BonePos(Entity.pawn, BoneIndex::spine_1);
//		Vector3 Spine2Pos = Get::BonePos(Entity.pawn, BoneIndex::spine_2);
//		Vector3 PelvisPos = Get::BonePos(Entity.pawn, BoneIndex::pelvis);
//		//左手臂
//		Vector3 ArmUpperLPos = Get::BonePos(Entity.pawn, BoneIndex::arm_upper_L);
//		Vector3 ArmLowerLPos = Get::BonePos(Entity.pawn, BoneIndex::arm_lower_L);
//		Vector3 handLPos = Get::BonePos(Entity.pawn, BoneIndex::hand_L);
//		//右手臂
//		Vector3 ArmUpperRPos = Get::BonePos(Entity.pawn, BoneIndex::arm_upper_R);
//		Vector3 ArmLowerRPos = Get::BonePos(Entity.pawn, BoneIndex::arm_lower_R);
//		Vector3 handRPos = Get::BonePos(Entity.pawn, BoneIndex::hand_R);
//		//左腿
//		Vector3 LegUpperLPos = Get::BonePos(Entity.pawn, BoneIndex::leg_upper_L);
//		Vector3 LegLowerLPos = Get::BonePos(Entity.pawn, BoneIndex::leg_lower_L);
//		Vector3 AnkleLPos = Get::BonePos(Entity.pawn, BoneIndex::ankle_L);
//		//右腿
//		Vector3 LegUpperRPos = Get::BonePos(Entity.pawn, BoneIndex::leg_upper_R);
//		Vector3 LegLowerRPos = Get::BonePos(Entity.pawn, BoneIndex::leg_lower_R);
//		Vector3 AnkleRPos = Get::BonePos(Entity.pawn, BoneIndex::ankle_R);
//
//		//printf("HeadPos Vec3  x: %f y : %f z : %f\n", HeadPos.x, HeadPos.y, HeadPos.z);
//		//printf("NeckPos Vec3  x: %f y : %f z : %f\n", NeckPos.x, NeckPos.y, NeckPos.z);
//		//printf("Spine1Pos Vec3  x: %f y : %f z : %f\n", Spine1Pos.x, Spine1Pos.y, Spine1Pos.z);
//		//printf("Spine2Pos Vec3  x: %f y : %f z : %f\n", Spine2Pos.x, Spine2Pos.y, Spine2Pos.z);
//		//printf("PelvisPos Vec3  x: %f y : %f z : %f\n", PelvisPos.x, PelvisPos.y, PelvisPos.z);
//		//printf("ArmLPos Vec3  x: %f y : %f z : %f\n", ArmUpperLPos.x, ArmUpperLPos.y, ArmUpperLPos.z);
//		//printf("ArmRPos Vec3  x: %f y : %f z : %f\n", ArmUpperRPos.x, ArmUpperRPos.y, ArmUpperRPos.z);
//		
//		
//		Vector3 HeadEndPos{};
//		Vector3 NeckEndPos{};
//		Vector3 Spine1EndPos{};
//		Vector3 Spine2EndPos{};
//		Vector3 PelvisEndPos{};
//
//		Vector3 ArmLEndPos{};
//		Vector3 ArmL2EndPos{};
//		Vector3 handLEndPos{};
//
//		Vector3 ArmREndPos{};
//		Vector3 ArmR2EndPos{};
//		Vector3 handREndPos{};
//
//
//		Vector3 LegLEndPos{};
//		Vector3 LegL2EndPos{};
//		Vector3 AnkleLEndPos{};
//
//		Vector3 LegREndPos{};
//		Vector3 LegR2EndPos{};
//		Vector3 AnkleREndPos{};
//
//		Vector3 Window = Get::WindowSize();
//
//		bool ToScreen1 = false;
//		bool ToScreen2 = false;
//		bool ToScreen3 = false;
//		bool ToScreen4 = false;
//		bool ToScreen5 = false;
//		bool ToScreen6 = false;
//		bool ToScreen7 = false;
//		bool ToScreen8 = false;
//		bool ToScreen9 = false;
//		bool ToScreen10 = false;
//		bool ToScreen11 = false;
//		bool ToScreen12 = false;
//		bool ToScreen13 = false;
//		bool ToScreen14 = false;
//		bool ToScreen15 = false;
//		bool ToScreen16 = false;
//		bool ToScreen17 = false;
//
//		ToScreen1 = Utils::WorldToScreen(HeadPos, HeadEndPos, Address::GetViewMatrixPtr(), Window.x, Window.y);
//		ToScreen2 = Utils::WorldToScreen(NeckPos, NeckEndPos, Address::GetViewMatrixPtr(), Window.x, Window.y);
//		ToScreen3 = Utils::WorldToScreen(Spine1Pos, Spine1EndPos, Address::GetViewMatrixPtr(), Window.x, Window.y);
//		ToScreen4 = Utils::WorldToScreen(Spine2Pos, Spine2EndPos, Address::GetViewMatrixPtr(), Window.x, Window.y);
//		ToScreen5 = Utils::WorldToScreen(PelvisPos, PelvisEndPos, Address::GetViewMatrixPtr(), Window.x, Window.y);
//
//		ToScreen6 = Utils::WorldToScreen(ArmUpperLPos, ArmLEndPos, Address::GetViewMatrixPtr(), Window.x, Window.y);
//		ToScreen7 = Utils::WorldToScreen(ArmLowerLPos, ArmL2EndPos, Address::GetViewMatrixPtr(), Window.x, Window.y);
//		ToScreen8 = Utils::WorldToScreen(handLPos, handLEndPos, Address::GetViewMatrixPtr(), Window.x, Window.y);
//
//		ToScreen9 = Utils::WorldToScreen(ArmUpperRPos, ArmREndPos, Address::GetViewMatrixPtr(), Window.x, Window.y);
//		ToScreen10 = Utils::WorldToScreen(ArmLowerRPos, ArmR2EndPos, Address::GetViewMatrixPtr(), Window.x, Window.y);
//		ToScreen11 = Utils::WorldToScreen(handRPos, ArmLEndPos, Address::GetViewMatrixPtr(), Window.x, Window.y);
//
//		ToScreen12 = Utils::WorldToScreen(LegUpperLPos, LegLEndPos, Address::GetViewMatrixPtr(), Window.x, Window.y);
//		ToScreen13 = Utils::WorldToScreen(LegLowerLPos, LegL2EndPos, Address::GetViewMatrixPtr(), Window.x, Window.y);
//		ToScreen14 = Utils::WorldToScreen(AnkleLPos, AnkleLEndPos, Address::GetViewMatrixPtr(), Window.x, Window.y);
//
//		ToScreen15 = Utils::WorldToScreen(LegUpperRPos, LegREndPos, Address::GetViewMatrixPtr(), Window.x, Window.y);
//		ToScreen16 = Utils::WorldToScreen(LegLowerRPos, LegR2EndPos, Address::GetViewMatrixPtr(), Window.x, Window.y);
//		ToScreen17 = Utils::WorldToScreen(AnkleRPos, AnkleREndPos, Address::GetViewMatrixPtr(), Window.x, Window.y);
//
//		//printf("head %d\n", ToScreen1);
//		//printf("neck %d\n", ToScreen2);
//		//printf("spi1 %d\n", ToScreen3);
//		//printf("spi2 %d\n", ToScreen4);
//		//printf("pelv %d\n", ToScreen5);
//		//printf("arm1 %d\n", ToScreen6);
//		//printf("arm2 %d\n", ToScreen7);
//
//		void* pawnPtr = reinterpret_cast<void*>(LocalPlayer.pawn);
//		//头部可视判断
//		CGameTrace EyepTrace = CEngineTrace::Get()->TraceShape(LocalEye.value(), EntityEye.value(), pawnPtr);
//		//躯干可视判断
//		CGameTrace NeckpTrace = CEngineTrace::Get()->TraceShape(LocalEye.value(), NeckPos, pawnPtr);
//		CGameTrace Spine1pTrace = CEngineTrace::Get()->TraceShape(LocalEye.value(), Spine1Pos, pawnPtr);
//		CGameTrace Spine2pTrace = CEngineTrace::Get()->TraceShape(LocalEye.value(), Spine2Pos, pawnPtr);
//		CGameTrace PelvispTrace = CEngineTrace::Get()->TraceShape(LocalEye.value(), PelvisPos, pawnPtr);
//		//左手臂可视判断
//		CGameTrace ArmUpperLpTrace = CEngineTrace::Get()->TraceShape(LocalEye.value(), ArmUpperLPos, pawnPtr);
//		CGameTrace ArmLowerLpTrace = CEngineTrace::Get()->TraceShape(LocalEye.value(), ArmLowerLPos, pawnPtr);
//		CGameTrace handLpTrace = CEngineTrace::Get()->TraceShape(LocalEye.value(), handLPos, pawnPtr);
//		//右手臂可视判断
//		CGameTrace ArmUpperRpTrace = CEngineTrace::Get()->TraceShape(LocalEye.value(), ArmUpperRPos, pawnPtr);
//		CGameTrace ArmLowerRpTrace = CEngineTrace::Get()->TraceShape(LocalEye.value(), ArmLowerRPos, pawnPtr);
//		CGameTrace handRpTrace = CEngineTrace::Get()->TraceShape(LocalEye.value(), handRPos, pawnPtr);
//		//左腿可视判断
//		CGameTrace LegUpperLpTrace = CEngineTrace::Get()->TraceShape(LocalEye.value(), LegUpperLPos, pawnPtr);
//		CGameTrace LegLowerLpTrace = CEngineTrace::Get()->TraceShape(LocalEye.value(), LegLowerLPos, pawnPtr);
//		CGameTrace AnkleLpTrace = CEngineTrace::Get()->TraceShape(LocalEye.value(), AnkleLPos, pawnPtr);
//		//右腿可视判断
//		CGameTrace LegUpperRpTrace = CEngineTrace::Get()->TraceShape(LocalEye.value(), LegUpperRPos, pawnPtr);
//		CGameTrace LegLowerRpTrace = CEngineTrace::Get()->TraceShape(LocalEye.value(), LegLowerRPos, pawnPtr);
//		CGameTrace AnkleRpTrace = CEngineTrace::Get()->TraceShape(LocalEye.value(), AnkleRPos, pawnPtr);
//		
//		if (!EyepTrace.IsVisible() &&
//			!NeckpTrace.IsVisible() && !Spine1pTrace.IsVisible() && !Spine2pTrace.IsVisible() && !PelvispTrace.IsVisible() &&
//			!ArmUpperLpTrace.IsVisible() && !ArmLowerLpTrace.IsVisible() && !handLpTrace.IsVisible() &&
//			!ArmUpperRpTrace.IsVisible() && !ArmLowerRpTrace.IsVisible() && !handRpTrace.IsVisible() &&
//			!LegUpperLpTrace.IsVisible() && !LegLowerLpTrace.IsVisible() && !AnkleLpTrace.IsVisible() &&
//			!LegUpperRpTrace.IsVisible() && !LegLowerRpTrace.IsVisible() && !AnkleRpTrace.IsVisible())
//			continue;
//
//
//		//多部位屏幕可见判断
//		if (!ToScreen1 && !ToScreen2 &&!ToScreen3 && !ToScreen4 &&!ToScreen5 && !ToScreen6 && !ToScreen7 && !ToScreen8 && !ToScreen9 && !ToScreen10 && !ToScreen11 && !ToScreen12 && !ToScreen13 && !ToScreen14 && !ToScreen15 && !ToScreen16 && !ToScreen17)
//			continue;
//
//
//		//printf("HeadEndPos Vec3  x: %f y : %f z : %f\n", HeadEndPos.x, HeadEndPos.y, HeadEndPos.z);
//		//printf("NeckEndPos Vec3  x: %f y : %f z : %f\n", NeckEndPos.x, NeckEndPos.y, NeckEndPos.z);
//		//printf("Spine1EndPos Vec3  x: %f y : %f z : %f\n", Spine1EndPos.x, Spine1EndPos.y, Spine1EndPos.z);
//		//printf("Spine2EndPos Vec3  x: %f y : %f z : %f\n", Spine2EndPos.x, Spine2EndPos.y, Spine2EndPos.z);
//		//printf("PelvisEndPos Vec3  x: %f y : %f z : %f\n", PelvisEndPos.x, PelvisEndPos.y, PelvisEndPos.z);
//		//printf("ArmLEndPos Vec3  x: %f y : %f z : %f\n", ArmLEndPos.x, ArmLEndPos.y, ArmLEndPos.z);
//		//printf("ArmREndPos Vec3  x: %f y : %f z : %f\n", ArmREndPos.x, ArmREndPos.y, ArmREndPos.z);
//
//		ImVec2 windowSize = ImGui::GetIO().DisplaySize;
//		ImVec2 screenCenter = ImVec2(windowSize.x / 2, windowSize.y / 2);
//		int TiggerSize = 5;//触发fov
//
//
//		//触发检查
//
//		//head 
//		if (HeadEndPos.x > screenCenter.x - TiggerSize && HeadEndPos.x < screenCenter.x + TiggerSize &&
//			HeadEndPos.y > screenCenter.y - TiggerSize && HeadEndPos.y < screenCenter.y + TiggerSize)
//		{
//			printf("1\n");
//			shouldShoot = true;
//
//		}
//
//		//Neck
//		if (NeckEndPos.x > screenCenter.x - TiggerSize && NeckEndPos.x < screenCenter.x + TiggerSize &&
//			NeckEndPos.y > screenCenter.y - TiggerSize && NeckEndPos.y < screenCenter.y + TiggerSize)
//		{
//			printf("2\n");
//			shouldShoot = true;
//
//		}
//
//		// Spine1
//		if (Spine1EndPos.x > screenCenter.x - TiggerSize && Spine1EndPos.x < screenCenter.x + TiggerSize &&
//			Spine1EndPos.y > screenCenter.y - TiggerSize && Spine1EndPos.y < screenCenter.y + TiggerSize)
//		{
//			printf("3\n");
//			shouldShoot = true;
//
//		}
//
//
//		//Spine2
//		if (Spine2EndPos.x > screenCenter.x - TiggerSize && Spine2EndPos.x < screenCenter.x + TiggerSize &&
//			Spine2EndPos.y > screenCenter.y - TiggerSize && Spine2EndPos.y < screenCenter.y + TiggerSize)
//		{
//			printf("4\n");
//			shouldShoot = true;
//
//		}
//
//		// Pelvis
//		if (PelvisEndPos.x > screenCenter.x - TiggerSize && PelvisEndPos.x < screenCenter.x + TiggerSize &&
//			PelvisEndPos.y > screenCenter.y - TiggerSize && PelvisEndPos.y < screenCenter.y + TiggerSize)
//		{
//			printf("5\n");
//			shouldShoot = true;
//		}
//
//
//		//ArmL
//		if (ArmLEndPos.x > screenCenter.x - TiggerSize && ArmLEndPos.x < screenCenter.x + TiggerSize &&
//			ArmLEndPos.y > screenCenter.y - TiggerSize && ArmLEndPos.y < screenCenter.y + TiggerSize)
//		{
//			printf("6\n");
//			shouldShoot = true;
//		}
//
//		//ArmL2
//		if (ArmL2EndPos.x > screenCenter.x - TiggerSize && ArmL2EndPos.x < screenCenter.x + TiggerSize &&
//			ArmL2EndPos.y > screenCenter.y - TiggerSize && ArmL2EndPos.y < screenCenter.y + TiggerSize)
//		{
//			printf("7\n");
//			shouldShoot = true;
//		}
//
//		//handL
//		if (handLEndPos.x > screenCenter.x - TiggerSize && handLEndPos.x < screenCenter.x + TiggerSize &&
//			handLEndPos.y > screenCenter.y - TiggerSize && handLEndPos.y < screenCenter.y + TiggerSize)
//		{
//			printf("8\n");
//			shouldShoot = true;
//		}
//
//
//		//ArmR
//		if (ArmREndPos.x > screenCenter.x - TiggerSize && ArmREndPos.x < screenCenter.x + TiggerSize &&
//			ArmREndPos.y > screenCenter.y - TiggerSize && ArmREndPos.y < screenCenter.y + TiggerSize)
//		{
//			printf("9\n");
//			shouldShoot = true;
//		}
//
//
//		//ArmR2
//		if (ArmR2EndPos.x > screenCenter.x - TiggerSize && ArmR2EndPos.x < screenCenter.x + TiggerSize &&
//			ArmR2EndPos.y > screenCenter.y - TiggerSize && ArmR2EndPos.y < screenCenter.y + TiggerSize)
//		{
//			printf("10\n");
//			shouldShoot = true;
//		}
//
//		//handR
//		if (handREndPos.x > screenCenter.x - TiggerSize && handREndPos.x < screenCenter.x + TiggerSize &&
//			handREndPos.y > screenCenter.y - TiggerSize && handREndPos.y < screenCenter.y + TiggerSize)
//		{
//			printf("11\n");
//			shouldShoot = true;
//		}
//
//		//LegL
//		if (LegLEndPos.x > screenCenter.x - TiggerSize && LegLEndPos.x < screenCenter.x + TiggerSize &&
//			LegLEndPos.y > screenCenter.y - TiggerSize && LegLEndPos.y < screenCenter.y + TiggerSize)
//		{
//			printf("12\n");
//			shouldShoot = true;
//		}
//
//
//		//LegL2
//		if (LegL2EndPos.x > screenCenter.x - TiggerSize && LegL2EndPos.x < screenCenter.x + TiggerSize &&
//			LegL2EndPos.y > screenCenter.y - TiggerSize && LegL2EndPos.y < screenCenter.y + TiggerSize)
//		{
//			printf("13\n");
//			shouldShoot = true;
//		}
//
//		//AnkleL
//		if (AnkleLEndPos.x > screenCenter.x - TiggerSize && AnkleLEndPos.x < screenCenter.x + TiggerSize &&
//			AnkleLEndPos.y > screenCenter.y - TiggerSize && AnkleLEndPos.y < screenCenter.y + TiggerSize)
//		{
//			printf("14\n");
//			shouldShoot = true;
//		}
//
//
//		//LegR
//		if (LegREndPos.x > screenCenter.x - TiggerSize && LegREndPos.x < screenCenter.x + TiggerSize &&
//			LegREndPos.y > screenCenter.y - TiggerSize && LegREndPos.y < screenCenter.y + TiggerSize)
//		{
//			printf("15\n");
//			shouldShoot = true;
//		}
//
//
//		//LegR2
//		if (LegR2EndPos.x > screenCenter.x - TiggerSize && LegR2EndPos.x < screenCenter.x + TiggerSize &&
//			LegR2EndPos.y > screenCenter.y - TiggerSize && LegR2EndPos.y < screenCenter.y + TiggerSize)
//		{
//			printf("16\n");
//			shouldShoot = true;
//		}
//
//		//AnkleR
//		if (AnkleREndPos.x > screenCenter.x - TiggerSize && AnkleREndPos.x < screenCenter.x + TiggerSize &&
//			AnkleREndPos.y > screenCenter.y - TiggerSize && AnkleREndPos.y < screenCenter.y + TiggerSize)
//		{
//			printf("17\n");
//			shouldShoot = true;
//		}
//
//
//
//
//	}
//
//	// 在循环外执行射击动作
//	if (shouldShoot && GetAsyncKeyState(Menu::Tiggerbot::HotKey) && !Menu::ShowMenu && weaponcheck()) {
//		auto currentTime = std::chrono::high_resolution_clock::now();
//		static auto lastShotTime = std::chrono::high_resolution_clock::now();
//		auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastShotTime).count();
//		if (elapsedTime >= Menu::Tiggerbot::TriggerTargetDelay) {
//			mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
//			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
//			lastShotTime = currentTime;
//		}
//	}
//}

void Aimbot::DrawTiggerbotFOV()
{
	ImVec2 windowSize = ImGui::GetIO().DisplaySize;
	ImVec2 screenCenter = ImVec2(windowSize.x / 2, windowSize.y / 2);

	int aimSize = 3;

	float thickness = 2.0f;

	ImDrawList* drawList = ImGui::GetForegroundDrawList();
	drawList->AddCircle(screenCenter, aimSize, Menu::Tiggerbot::rectColor, 1024, thickness);
}
