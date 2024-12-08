
#include <cstdint>
#include <iostream>

#include "Search.h"
#include "mem Aimbot.h"
#include "imgui/imgui.h"
#include "GameTace.hpp"
#include "mem.hpp"
#include "CUserCmd.h"
#include "Anti-aim.hpp"
#include "CCSGOInput.hpp"




void Antiaim()
{

	Vector3 targetAngle{};
	Player LocalPlayer{};

	LocalPlayer.control = Address::GetLocalPlayerControl();

	LocalPlayer.pawn = Get::PlayerPawnAddress(LocalPlayer.control);

	Vector3 lpPos = Get::LastCameraPos(LocalPlayer.pawn);

	Vector3 vecCameraAngles = CCSGOInput::Get()->GetViewAngles();

	printf("vecCameraAngles Vec3  x: %f y : %f z : %f\n", vecCameraAngles.x, vecCameraAngles.y, vecCameraAngles.z);

	float backward = 180.0f+ vecCameraAngles.y;

    Aimbot::NormalizeYaw(backward);

	printf("backward  %f", backward); 

	CUserCMD* pUserCmd = CUserCMD::Get();
	if (pUserCmd)
	{
		CBaseUserCmd* pBaseCmd = pUserCmd->csgoUserCmd.pBaseCmd;
		pBaseCmd->pViewAngles->m_view_angles =Vector3(89, backward ,0) ;
	}
}