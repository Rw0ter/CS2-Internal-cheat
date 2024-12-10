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
	head = 6,           // ͷ��
	neck_0 = 5,         // ����
	spine_1 = 4,        // ��׵1
	spine_2 = 2,        // ��׵2
	pelvis = 0,         // ����
	arm_upper_L = 8,    // ���ϱ�
	arm_lower_L = 9,    // ��ǰ��
	hand_L = 10,        // ����
	arm_upper_R = 13,   // ���ϱ�
	arm_lower_R = 14,   // ��ǰ��
	hand_R = 15,        // ����
	leg_upper_L = 22,   // �����
	leg_lower_L = 23,   // ��С��
	ankle_L = 24,       // �����
	leg_upper_R = 25,   // �Ҵ���
	leg_lower_R = 26,   // ��С��
	ankle_R = 27,       // �ҽ���
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

