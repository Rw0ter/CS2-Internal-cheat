#pragma once
#include "Tace Memory.hpp"
class IGameEntitySystem;
class IGameResourceService {
	char pad[0x58];
public:
	static IGameResourceService* Get() {
		static IGameResourceService* pGameResourceService = reinterpret_cast<IGameResourceService*>(Memory::CreateInterface("engine2.dll", "GameResourceServiceClientV001"));
		return pGameResourceService;
	}

	IGameEntitySystem* pGameEntitySystem;
};

class IGameEntitySystem {
public:
	static IGameEntitySystem* Get() {
		static IGameResourceService* pGameResourceService = IGameResourceService::Get();
		return pGameResourceService->pGameEntitySystem;
	}

	void* GetBaseEntity(const int nIndex) {
		static auto fnGetClientEntity = reinterpret_cast<void*(__fastcall*)(void*, int)>(Memory::FindPattern("client.dll", "81 FA ?? ?? ?? ?? 77 36 8B C2 C1 F8 09 83 F8 3F 77 2C 48 98"));
		return fnGetClientEntity(this, nIndex);
	}

	void* GetFromHandle(const uint32_t hEntry) {
		static auto fnGetClientEntity = reinterpret_cast<void* (__fastcall*)(void*, int)>(Memory::FindPattern("client.dll", "81 FA ?? ?? ?? ?? 77 36 8B C2 C1 F8 09 83 F8 3F 77 2C 48 98"));
		return GetBaseEntity(static_cast<int>(hEntry & 0x7FFF));
	}
};