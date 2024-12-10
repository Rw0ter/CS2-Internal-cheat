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
	head = 6,           // Õ∑≤ø
	neck_0 = 5,         // æ±≤ø
	spine_1 = 4,        // ºπ◊µ1
	spine_2 = 2,        // ºπ◊µ2
	pelvis = 0,         // π«≈Ë
	arm_upper_L = 8,    // ◊Û…œ±€
	arm_lower_L = 9,    // ◊Û«∞±€
	hand_L = 10,        // ◊Û ÷
	arm_upper_R = 13,   // ”“…œ±€
	arm_lower_R = 14,   // ”“«∞±€
	hand_R = 15,        // ”“ ÷
	leg_upper_L = 22,   // ◊Û¥ÛÕ»
	leg_lower_L = 23,   // ◊Û–°Õ»
	ankle_L = 24,       // ◊ÛΩ≈ı◊
	leg_upper_R = 25,   // ”“¥ÛÕ»
	leg_lower_R = 26,   // ”“–°Õ»
	ankle_R = 27,       // ”“Ω≈ı◊
};

class CSchemaClassInfoData {
    char pad[0x8];
public:
    const char* szName;
    const char* szModule;
    int32_t nSize;
    int16_t nFieldSize;
    int16_t nStaticSize;
    int16_t nMetadataSize;
    uint8_t nBaseClassesCount;
    char pad2[0x4];
    void* pFields;
    char pad3[0x8];
    void* pBaseClasses;
    char pad4[0x28];
};

inline ImColor ImGlowColor;

int ConvertImColorToInt(const ImColor& color);

namespace Get
{
	intptr_t PlayerPawnAddress(intptr_t addr);
	bool IsPlayer(intptr_t addr);
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
	inline CSchemaClassInfoData* GetSchemaBinding(intptr_t addr) {
		CSchemaClassInfoData* pBuffer = {};
		Memory::CallClassFn<void, 38>((void*)addr, &pBuffer);
		return pBuffer;
	}
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

