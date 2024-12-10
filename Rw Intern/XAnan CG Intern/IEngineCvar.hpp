#pragma once
#include <vector>
#include "fnv1a.h"
#include "Vector.h"
#include "UtlLinkedList.h"
#include <unordered_map>
union CvarValueTypes {
	bool Bool;
	short Int16;
	uint16_t Uint16;
	int Int;
	uint32_t Uint32;
	int64_t Int64;
	uint64_t Uint64;
	float Float;
	double Double;
	const char* String;
	uint8_t Color[4];
	Vector3 Vector2D;
	Vector3 Vector3D;
	Vector3 Vector4D;
	Vector3 Angles;
};

class CConVar {
public:
	const char* szName;
	CConVar* m_pNext;
	char pad_01[0x10];
	const char* szDescription;
	uint32_t nType;
	uint32_t nRegistered;
	uint32_t nFlags;
	std::uint32_t m_unk3;
	std::uint32_t m_nCallbacks;
	std::uint32_t m_unk4;
	CvarValueTypes Value;
	CvarValueTypes ValueOld;
	template <typename T>
	T GetValue() {
		return *reinterpret_cast<T*>(&Value);
	}

	bool GetBool() {
		return this->Value.Bool;
	}

	float GetFloat() {
		return this->Value.Float;
	}
	/*
	void SetString(const char* Value) {
		CallClassFn<void, 0>(this, Value);
	}

	void SetFloat(const float Value) {
		CallClassFn<void, 1>(this, Value);
	}

	void SetInt(const int Value) {
		CallClassFn<void, 2>(this, Value);
	}

	void SetColor(const Color Value) {
		CallClassFn<void, 3>(this, Value);
	}

	const char* GetName() {
		CallClassFn<const char*, 4>(this);
	}

	const char* GetBaseName() {
		CallClassFn<const char*, 5>(this);
	}

	bool IsFlagSet(int nFlag) {
		CallClassFn<bool, 6>(this, nFlag);
	}

	int GetSplitScreenPlayerSlot() {
		CallClassFn<int, 7>(this);
	}*/
};

inline std::unordered_map<uint64_t, CConVar*> mapCvars = {};
class IEngineCvar {
	char pad_01[0x40];
	CUtlLinkedList<CConVar*> listConvars;
public:
	static IEngineCvar* Get() {
		static IEngineCvar* pEngineCvar = reinterpret_cast<IEngineCvar*>(Memory::CreateInterface("tier0.dll", "VEngineCvar007"));
		return pEngineCvar;
	}

	void Setup() {
		mapCvars.clear();
		for (int i = listConvars.Head(); i != listConvars.InvalidIndex(); i = listConvars.Next(i)) {
			CConVar* pConVar = listConvars.Element(i);
			if (pConVar == nullptr) {
				continue;
			}

			uint64_t nHash = HASH_RT(pConVar->szName);
			if (mapCvars.find(nHash) == mapCvars.end()) {
				mapCvars.emplace(nHash, pConVar);
			}
		}
	}

	CConVar* Find(uint64_t uHashedName)
	{
		for (int i = listConvars.Head(); i != listConvars.InvalidIndex(); i = listConvars.Next(i)) {
			CConVar* pConVar = listConvars.Element(i);
			if (pConVar == nullptr)
				continue;

			if (HASH_RT(pConVar->szName) == uHashedName)
				return pConVar;
		}

		return nullptr;
	}

	CConVar* Find(std::string szCvarName)
	{
		auto iterCvar = mapCvars.find(HASH_RT(szCvarName.c_str()));
		if (iterCvar == mapCvars.end()) {
			return nullptr;
		}

		return iterCvar->second;
	}
};
