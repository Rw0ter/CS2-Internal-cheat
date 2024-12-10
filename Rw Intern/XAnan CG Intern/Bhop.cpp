#include <cstdint>
#include <iostream>

#include "Bhop.hpp"
#include "Search.h"
#include "mem Aimbot.h"
#include "imgui/imgui.h"
#include "GameTace.hpp"
#include "mem.hpp"
#include "CUserCmd.h"
#include "CCSGOInput.hpp"
#include "IEngineCvar.hpp"

#define GetField(pBase, Class, Field, Type) *reinterpret_cast<Type*>(static_cast<uintptr_t>(pBase) + cs2_dumper::schemas::client_dll::Class::Field);
static float NormalizePitch(float pPitch)
{
	pPitch = (pPitch < -89.0f) ? -89.0f : pPitch;

	pPitch = (pPitch > 89.f) ? 89.0f : pPitch;
	return pPitch;
}

static float NormalizeYaw(float pYaw)
{
	while (pYaw > 180.f) pYaw -= 360.f;

	while (pYaw < -180.f) pYaw += 360.f;
	return pYaw;
}

template <typename T = float>
static T Clamp(T flValue, T flMin, T flMax) {
	const T varMin = min(flMin, flMax);
	const T varMax = max(flMin, flMax);
	return max(min(flValue, varMax), varMin);
}

void MovementCorrection() {
	CUserCMD* pCmd = CUserCMD::Get();
	CCSGOInput* pCSGOInput = CCSGOInput::Get();
	if (!pCmd || !pCSGOInput) {
		return;
	}

	Vector3 vecTargetAngles = pCSGOInput->GetViewAngles();
	CBaseUserCmd* pBase = pCmd->csgoUserCmd.pBaseCmd;
	if (!pBase || !pBase->pViewAngles) {
		return;
	}
	vecTargetAngles.x = vecTargetAngles.z = 0.f;
	Vector3 vecForward, vecOldForward, vecRight, vecOldRight, vecUp, vecOldUp;
	Vector3 vecCorrectionAngles = Vector3(0.f, pBase->pViewAngles->m_view_angles.y, 0.f);
	vecCorrectionAngles.AnglesToVectors(&vecForward, &vecRight, &vecUp);
	vecTargetAngles.AnglesToVectors(&vecOldForward, &vecOldRight, &vecOldUp);
	vecOldForward.z = vecOldRight.z = vecOldUp.x = vecOldUp.y = vecForward.z = vecRight.z = vecUp.x = vecUp.y = 0.0f;

	const float flRollUp = vecUp.z * pBase->flUpMove;
	const float flYawSide = vecRight.y * pBase->flSideMove;
	const float flPitchSide = vecRight.x * pBase->flSideMove;
	const float flYawForward = vecForward.y * pBase->flForwardMove;
	const float flPitchForward = vecForward.x * pBase->flForwardMove;

	pBase->flUpMove = Clamp(vecOldUp.x * flYawSide + vecOldUp.y * flPitchSide + vecOldUp.x * flYawForward + vecOldUp.y * flPitchForward + vecOldUp.z * flRollUp, -1.f, 1.f);
	pBase->flSideMove = Clamp(vecOldRight.x * flPitchSide + vecOldRight.y * flYawSide + vecOldRight.x * flPitchForward + vecOldRight.y * flYawForward + vecOldRight.z * flRollUp, -1.f, 1.f);
	pBase->flForwardMove = Clamp(vecOldForward.x * flPitchSide + vecOldForward.y * flYawSide + vecOldForward.x * flPitchForward + vecOldForward.y * flYawForward + vecOldForward.z * flRollUp, -1.f, 1.f);
}

static void RotationMove(CUserCMD* pUserCmd, const float flMoveYaw, const bool bRoundMovementInput) {
	CBaseUserCmd* pBaseCmd = pUserCmd->csgoUserCmd.pBaseCmd;
	if (bRoundMovementInput) {
		pBaseCmd->pViewAngles->m_view_angles.y = flMoveYaw + 180.f;
	} else {
		const float flRotation = Deg2Rad(pBaseCmd->pViewAngles->m_view_angles.y - flMoveYaw);
		const float flMoveSide = std::sin(flRotation) * pBaseCmd->flForwardMove + std::cos(flRotation) * pBaseCmd->flSideMove;
		const float flMoveForward = std::cos(flRotation) * pBaseCmd->flForwardMove - std::sin(flRotation) * pBaseCmd->flSideMove;

		pBaseCmd->flSideMove = Clamp(- flMoveSide, - 1.f, 1.f);
		pBaseCmd->flForwardMove = Clamp(flMoveForward, - 1.f, 1.f);
		pUserCmd->nButtons.nValue &= ~(IN_BACK | IN_FORWARD | IN_MOVELEFT | IN_MOVERIGHT);
		if (pBaseCmd->flForwardMove < 0.f) {
			pUserCmd->nButtons.nValue |= IN_BACK;
		} else if (pBaseCmd->flForwardMove > 0.f) {
			pUserCmd->nButtons.nValue |= IN_FORWARD;
		}

		if (pBaseCmd->flSideMove > 0.f) {
			pUserCmd->nButtons.nValue |= IN_MOVELEFT;
		} else if (pBaseCmd->flSideMove < 0.f) {
			pUserCmd->nButtons.nValue |= IN_MOVERIGHT;
		}
	}
};

void StandaloneQuickStop() {
	CUserCMD* pUserCmd = CUserCMD::Get();
	CCSGOInput* pCSGOInput = CCSGOInput::Get();
	if (!pUserCmd || !pCSGOInput) {
		return;
	}

	Player LocalPlayer{};
	LocalPlayer.control = Address::GetLocalPlayerControl();
	LocalPlayer.pawn = Get::PlayerPawnAddress(LocalPlayer.control);
	if (!LocalPlayer.pawn) {
		return;
	}

	CBaseUserCmd* pBaseCmd = pUserCmd->csgoUserCmd.pBaseCmd;
	if (!pBaseCmd || !pBaseCmd->pViewAngles) {
		return;
	}

	BitFlag nButtons = BitFlag(pUserCmd->nButtons.nValue);
	const Vector3 vecAbsVelocity = GetField(LocalPlayer.pawn, C_BaseEntity, m_vecAbsVelocity, Vector3);
	const bool bInMovement = nButtons.Contact(IN_FORWARD) || nButtons.Contact(IN_BACK) || nButtons.Contact(IN_MOVELEFT) || nButtons.Contact(IN_MOVERIGHT);
	if (bInMovement || vecAbsVelocity.Length2D() < 10.f) {
		return;
	}

	BitFlag nFlags = GetField(LocalPlayer.pawn, C_BaseEntity, m_fFlags, uint64_t);
	if (!nFlags.Contact(fl_onground)) {
		return;
	}

	Vector3 vecForward = {};
	Vector3 vecCameraAngles = pCSGOInput->GetViewAngles();
	const float flMoveAngle = NormalizeYaw(Rad2Deg(std::atan2f(vecAbsVelocity.y, vecAbsVelocity.x)));
	vecCameraAngles.y -= flMoveAngle;
	vecCameraAngles.AnglesToVectors(&vecForward);

	pBaseCmd->flSideMove = vecForward.y;
	pBaseCmd->flForwardMove = - vecForward.x;
}

void AutoStrafe() {
    CUserCMD* pUserCmd = CUserCMD::Get();
	CCSGOInput* pCSGOInput = CCSGOInput::Get();
    if (!pUserCmd || !pCSGOInput) {
        return;
    }

	static BitFlag uLastPressed = 0;
	static BitFlag uLastButtons = 0;
	Player LocalPlayer{};
	LocalPlayer.control = Address::GetLocalPlayerControl();
	LocalPlayer.pawn = Get::PlayerPawnAddress(LocalPlayer.control);
	if (!LocalPlayer.pawn) {
		return;
	}

	CBaseUserCmd* pBaseCmd = pUserCmd->csgoUserCmd.pBaseCmd;
	if (!pBaseCmd || !pBaseCmd->pViewAngles) {
		return;
	}

	// settings
	bool bEasyStrafe = Menu::Misc::AutoStrafe; // 最后一次按键按的是wasd的哪一个就保持那个方向前进, 不开启并且没有按下wasd时就自动向前strafe
	float flAutoStrafeSmoothing = Menu::Misc::flAutoStrafeSmoothing; // 0.f - 100.f
	bool bAutoStrafeMovementKeys = Menu::Misc::bAutoStrafeMovementKeys; // wasd的strafe
	bool bAutoStrafeViewAngles = Menu::Misc::bAutoStrafeViewAngles; // 根据yaw角度的strafe


	Vector3 CameraAngles = pCSGOInput->GetViewAngles();
	float flAngles = NormalizeYaw(CameraAngles.y);
	const BitFlag bButtons = pUserCmd->nButtons.nValue;
	BitFlag nFlags = GetField(LocalPlayer.pawn, C_BaseEntity, m_fFlags, uint64_t);
	const Vector3 vecAbsVelocity = GetField(LocalPlayer.pawn, C_BaseEntity, m_vecAbsVelocity, Vector3);
	const auto BindButton = [&](const uint64_t uBindButton) {
		if (bButtons.Contact(uBindButton) && (!uLastButtons.Contact(uBindButton) ||
			uBindButton & IN_MOVELEFT && !(uLastPressed.Contact(IN_MOVERIGHT)) ||
			uBindButton & IN_MOVERIGHT && !(uLastPressed.Contact(IN_MOVELEFT)) ||
			uBindButton & IN_FORWARD && !(uLastPressed.Contact(IN_BACK)) ||
			uBindButton & IN_BACK && !(uLastPressed.Contact(IN_FORWARD))))
		{
			if (bEasyStrafe) {
				if (uBindButton & IN_MOVELEFT) {
					uLastPressed.Remove(IN_MOVERIGHT);
				} else if (uBindButton & IN_MOVERIGHT) {
					uLastPressed.Remove(IN_MOVELEFT);
				} else if (uBindButton & IN_FORWARD) {
					uLastPressed.Remove(IN_BACK);
				} else if (uBindButton & IN_BACK) {
					uLastPressed.Remove(IN_FORWARD);
				}

				uLastPressed.Add(uBindButton);
			}

		} else if (!(bButtons.Contact(uBindButton))) {
			uLastPressed.Remove(uBindButton);
		}
	};

	BindButton(IN_BACK);
	BindButton(IN_FORWARD);
	BindButton(IN_MOVELEFT);
	BindButton(IN_MOVERIGHT);
	uLastButtons = bButtons;
	if (nFlags.Contact(fl_onground) || (!bAutoStrafeMovementKeys && !bAutoStrafeViewAngles)) {
		return;
	}

	CConVar* pServerQuantizeMovementInput = IEngineCvar::Get()->Find("sv_quantize_movement_input");
	if (!pServerQuantizeMovementInput) {
		return;
	}

	const bool bRoundMovementInput = pServerQuantizeMovementInput->GetBool();
	if (bAutoStrafeMovementKeys) {
		float flStrafeYaw = 0.f;
		if (uLastPressed.Contact(IN_MOVELEFT)) {
			flStrafeYaw += 90.f;
		} else if (uLastPressed.Contact(IN_MOVERIGHT)) {
			flStrafeYaw -= 90.f;
		}

		if (uLastPressed.Contact(IN_FORWARD)) {
			flStrafeYaw *= 0.5f;
		} else if (uLastPressed.Contact(IN_BACK)) {
			flStrafeYaw = - flStrafeYaw * 0.5f + 180.f;
		}

		pBaseCmd->StopMovement();
		flAngles += flStrafeYaw;
		RotationMove(pUserCmd, flAngles, bRoundMovementInput);
		if (!bAutoStrafeViewAngles && flStrafeYaw == 0.f) {
			return;
		}
	}

	if (pBaseCmd->IsMovement()) {
		return;
	}

	pBaseCmd->flForwardMove = 0.f;
	const float flLength2D = vecAbsVelocity.Length2D();
	float flMoveAngle = NormalizeYaw(Rad2Deg(std::atan2f(vecAbsVelocity.y, vecAbsVelocity.x)));
	flMoveAngle -= floorf(flMoveAngle / 360.f + 0.5f) * 360.f;

	const float flStrafeStep = Clamp(Rad2Deg(std::atan2(15.f, flLength2D)), 0.f, 45.f);
	const float flAnglesDelta = (100.f - flAutoStrafeSmoothing) * 0.02f * (flStrafeStep * 2.f);

	const float flMoveDelta = NormalizeYaw(flAngles - flMoveAngle);
	if (fabsf(flMoveDelta) > 170.f && flLength2D > 80.f || flMoveDelta > flAnglesDelta && flLength2D > 80.f) {
		pBaseCmd->flSideMove = - 1.f;
		RotationMove(pUserCmd, flAnglesDelta + flMoveAngle, bRoundMovementInput);
		return;
	}

	const bool bSideSwitch = pBaseCmd->nLegacyCommandNumber % 2 == 0;
	if (-flAnglesDelta <= flMoveDelta || flLength2D <= 80.f) {
		pBaseCmd->flSideMove = bSideSwitch ? - 1.f : 1.f;
		flAngles += (bSideSwitch ? - flStrafeStep : flStrafeStep);
		RotationMove(pUserCmd, flAngles, bRoundMovementInput);
	} else {
		pBaseCmd->flSideMove = 1.f;
		flAngles = flMoveAngle - flAnglesDelta;
		RotationMove(pUserCmd, flAngles, bRoundMovementInput);
	}
}


void AntiAim() { // test move correct
	static float flRotationYaw = 0.f;
	CUserCMD* pUserCmd = CUserCMD::Get();
	if (!pUserCmd) {
		return;
	}

	CBaseUserCmd* pBaseCmd = pUserCmd->csgoUserCmd.pBaseCmd;
	Vector3 vecCameraAngles = CCSGOInput::Get()->GetViewAngles();
	if (!pBaseCmd || !pBaseCmd->pViewAngles) {
		return;
	}
	if (Menu::Antiaim::Spinbot) {
		flRotationYaw += Menu::Antiaim::Spinspeed;
		pBaseCmd->pViewAngles->m_view_angles.x = Menu::Antiaim::PitchOffset;
		pBaseCmd->pViewAngles->m_view_angles.y += flRotationYaw;
		pBaseCmd->pViewAngles->m_view_angles.y = NormalizeYaw(pBaseCmd->pViewAngles->m_view_angles.y);
	}
	else {
		pBaseCmd->pViewAngles->m_view_angles.x = Menu::Antiaim::PitchOffset;
		pBaseCmd->pViewAngles->m_view_angles.y = vecCameraAngles.y+Menu::Antiaim::YawOffset;
		pBaseCmd->pViewAngles->m_view_angles.y = NormalizeYaw(pBaseCmd->pViewAngles->m_view_angles.y);
	}
}


void Bhop()
{
    Player LocalPlayer{};
    LocalPlayer.control = Address::GetLocalPlayerControl();
    LocalPlayer.pawn = Get::PlayerPawnAddress(LocalPlayer.control);

    uintptr_t baseAddress = static_cast<uintptr_t>(LocalPlayer.pawn);
    short* flagsPointer = reinterpret_cast<short*>(baseAddress + cs2_dumper::schemas::client_dll::C_BaseEntity::m_fFlags);

    // 读取当前玩家的flags
    short flags = *flagsPointer;
    CUserCMD* pUserCmd = CUserCMD::Get();
    if (!pUserCmd) {
        return; 
    }
    std::uint64_t currentButtons = pUserCmd->nButtons.nValue;

    if (flags & (1 << 0) && (currentButtons & IN_JUMP) && Menu::Misc::Bhop) {
        pUserCmd->nButtons.nValue &= ~IN_JUMP; 
    }
}
