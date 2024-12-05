#pragma once
#include "Vector.h"
#include "Tace Memory.hpp"
class CCSGOInput {
public:
	static CCSGOInput* Get() {
		static CCSGOInput* pCCSGOInput = *reinterpret_cast<CCSGOInput**>(Memory::Relative(Memory::FindPattern("client.dll", "48 8B 0D ?? ?? ?? ?? 8B D3 E8 ?? ?? ?? ?? 44 8B 86 48 12"), 3, 7));
		return pCCSGOInput;
	}

	void SetViewAngles(Vector3 vecAngles) {
		static auto fnSetViewAngles = reinterpret_cast<void(__fastcall*)(void*, int, Vector3*)>(Memory::FindPattern("client.dll", "85 D2 75 3F 48 63"));
		fnSetViewAngles(this, 0, &vecAngles);
	}

	Vector3 GetViewAngles() {
		static auto fnGetViewAngles = reinterpret_cast<Vector3*(__fastcall*)(void*, int)>(Memory::FindPattern("client.dll", "4C 8B C1 85 D2 74 08 48 8D 05 ?? ?? ?? ?? C3"));
		return *fnGetViewAngles(this, 0);
	};
};