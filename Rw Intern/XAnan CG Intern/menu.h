#pragma once
#include"GameState.h"
#include <Windows.h>
#include <iostream>
#include <map>
#include <vector>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_internal.h"
#include "imgui/color.h"

inline ImFont* ico_subtab;
inline ImFont* tab_text1;
inline ImFont* ico = nullptr;
inline ImFont* tab_text2;
inline ImFont* ico_2 = nullptr;
inline ImFont* ico_minimize = nullptr;
inline ImFont* tab_textA;
inline ImFont* tab_text3;
inline ImFont* ico_logo;
inline ImFont* chinesefont;

inline float dpi_scale = 1.0f;

namespace Gui 
{
	inline int tabs = 0;
	inline int sub_tabs = 0;
	inline static float tab_alpha = 0.f;
	inline static float tab_add;
	inline static int active_tab = 0;
}

namespace Menu
{

	inline bool ShowMenu = { true };
	inline bool SafeMode = { true };
	inline bool bESP = { true };
	inline bool bAimBot = { true };
	inline bool TeamCheck = { true };
	inline int FovValue = { 90 };
	inline bool AimLock = { false };
	inline bool AimHead = { false };
	inline int smoothvalue = { 10 };


	namespace ESP 
	{
		
		inline int bChoose = { 0 };
		
		inline bool Team = { false };
		inline bool Box = { true };
		inline int BoxType = { 0 };
		inline bool Health = { true };
		inline bool Name = { true };
		inline bool Weapon = { false };
		inline bool Line = { false };
		inline int LineType = { 1 };
		inline bool AimCricle = { false };
		inline int CricleType = { 0 };
		inline bool Bone = { true };
		inline bool HeadCricle = { true };
		inline bool Glow = { true };
		inline bool FOV = { false };
		inline int FOVnumber = { 90 };

		inline bool bLocalPlayerChams = { false };
		inline int LocalPlayerChams = { 0 };

		inline bool bTeammateVisibleChams = { false };
		inline bool bTeammateInVisibleChams = { false };
		inline int TeammateVisibleChams = { 0 };
		inline int TeammateInVisibleChams = { 0 };

		inline bool bEnemiesVisibleChams = { false };
		inline bool bEnemiesInVisibleChams = { false };
		inline int EnemiesVisibleChams = { 0 };
		inline int EnemiesInVisibleChams = { 0 };

	}

	namespace Misc
	{
		inline bool Rander = { true };
		inline bool bStandaloneQuickStop = { false };

		inline bool Thirdperson = { false };
		inline int ThirdpersonKey = { 18 };
		inline float  flThirdpersonDistance = { 0.f };

		inline bool Bhop = { false };
		inline bool AutoStrafe = { false };
		inline float flAutoStrafeSmoothing = { 0.f };
		inline bool bAutoStrafeMovementKeys = { false };
		inline bool bAutoStrafeViewAngles = { false };

	}

	namespace Color
	{
		inline ImColor BoxColor = ImColor(192, 192, 192, 255);
		inline ImColor FilledColor = ImColor(252, 150, 237, 80);
		inline ImColor BoneColor = ImColor(255, 255, 255, 255);
		inline ImColor HeadCricleColor = ImColor(255, 255, 255, 255);
		inline ImColor EyeRayColor = ImColor(255, 0, 0, 255);
		inline ImColor NameColor = ImColor(0, 255, 255, 255);
		inline ImColor LineColor = ImColor(255, 255, 255, 220);
		inline ImColor AimCricleColor = ImColor(255, 255, 255, 255);
		inline ImColor HealthColor = ImColor(0, 0, 0, 255);
		inline ImColor WeaponColor = ImColor(252, 150, 237, 255);


		inline ImColor LocalPlayerVisibleColor = ImColor(0, 255, 255, 255);

		inline ImColor TeammateVisibleColor = ImColor(0, 255, 255, 255);
		inline ImColor TeammateInVisibleColor = ImColor(0, 255, 255, 255);

		inline ImColor EnemiesVisibleColor = ImColor(0, 255, 255, 255);
		inline ImColor EnemiesInVisibleColor = ImColor(0, 255, 255, 255);

	}

	namespace Antiaim
	{
		inline bool bEnable = { false };
		inline bool Spinbot = { false };
		inline float YawOffset = { 0.0f };
		inline float Spinspeed = { 0.0f };
		inline float PitchOffset = { 0.0f };
	}

	namespace Aimbot
	{
		inline bool AimbotAlwaysOn = { false };
		inline int AimShotPosComb = { 0 };
		inline bool showFOV = { false };
		inline bool bVisibleCheck = { true };
		inline bool Client = { true };
		inline bool TeamCheck = { true };
		inline bool Team = { false };
		inline int AimSize = { 100 };
		inline int AimKey = { 18 };
		inline int32_t AimPos = { 6 };
		inline float Smooth = { 90.0f };

		inline bool bSilent = false;
		inline bool bAutoRecoil = true;
		inline bool bPrefectSilent = false;
		inline float flRecoilAmount = 1.f; // 100%

		inline ImColor rectColor = ImColor(0, 255, 0, 255);
	}

	namespace Tiggerbot
	{
		inline bool AutoFire = { false };
		inline bool TriggerBot = { false };
		inline int HotKey = { 18 };
		inline int TriggerShotDelay = { 0 };
		inline float TriggerTargetDelay = { 0.0f };
		inline ImColor rectColor = ImColor(0, 255, 0, 255);
	}


	void start();
}

