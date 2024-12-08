#pragma once
#include "Windows.h"
#include "Offsets.h"
#include "Address.h"
#include "Vector.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include <string>
#include <bitset>
#include <map>
#include "UtlVector.h"


class Player
{
public:
	intptr_t control;
	intptr_t pawn;
	int health;
	int team;
	Vector3 pos;
	std::string name;
};

enum BoneIndex {
	head = 6,           // Í·²¿
	neck_0 = 5,         // ¾±²¿
	spine_1 = 4,        // ¼¹×µ1
	spine_2 = 2,        // ¼¹×µ2
	pelvis = 0,         // ¹ÇÅè
	arm_upper_L = 8,    // ×óÉÏ±Û
	arm_lower_L = 9,    // ×óÇ°±Û
	hand_L = 10,        // ×óÊÖ
	arm_upper_R = 13,   // ÓÒÉÏ±Û
	arm_lower_R = 14,   // ÓÒÇ°±Û
	hand_R = 15,        // ÓÒÊÖ
	leg_upper_L = 22,   // ×ó´óÍÈ
	leg_lower_L = 23,   // ×óĞ¡ÍÈ
	ankle_L = 24,       // ×ó½Åõ×
	leg_upper_R = 25,   // ÓÒ´óÍÈ
	leg_lower_R = 26,   // ÓÒĞ¡ÍÈ
	ankle_R = 27,       // ÓÒ½Åõ×
};


inline ImColor ImGlowColor;

int ConvertImColorToInt(const ImColor& color);

namespace Get
{
	intptr_t PlayerPawnAddress(intptr_t addr);
	bool PawnAlive(intptr_t addr);
	bool PlayerAlive(intptr_t addr);
	int PlayerTeam(intptr_t addr);
	int PlayerHealth(intptr_t addr);
	Vector3 PlayerPos(intptr_t addr);
	Vector3 BonePos(intptr_t addr, int32_t index);
	bool IsDormant(intptr_t addr);
	std::string PlayerName(intptr_t addr);
	Vector3 WindowSize();
	Vector3 LastCameraPos(intptr_t addr);
	std::string GetWeaponName(intptr_t addr);
	CUtlVector<Vector3>& GetAimPunch(intptr_t addr);
}

namespace Set
{
	void RadarHack(intptr_t addr);
	void GlowHack(intptr_t addr);
}

struct BoneJoint
{
	Vector3 pos{};
	float scale;
	float rotation[4];
};

