#pragma once
#include "Tace Memory.hpp"
#include "CMaterial2.hpp"
#include "UtlStrongHandle.h"
class IMaterialSystem2 {
public:
	static IMaterialSystem2* Get() {
		static IMaterialSystem2* pMaterialSystem = reinterpret_cast<IMaterialSystem2*>(Memory::CreateInterface("materialsystem2.dll", "VMaterialSystem2_001"));
		return pMaterialSystem;
	};

	CStrongHandle<CMaterial2>* FindOrCreateMaterialFromResource(CStrongHandle<CMaterial2>* IMaterial, const char* MaterialName) {
		return Memory::CallClassFn<CStrongHandle<CMaterial2>*, 14>(this, IMaterial, MaterialName);
	}

	CMaterial2** CreateNewMaterial(CStrongHandle<CMaterial2>* IMaterial, const char* MaterialName, void* MaterialData) {
		return Memory::CallClassFn<CMaterial2**, 29>(this, IMaterial, MaterialName, MaterialData, 0, 0, 0, 0, 0, 1);
	}

	void SetCreateDataByMaterial(CStrongHandle<CMaterial2>* IMaterial, const void* MaterialData) {
		return Memory::CallClassFn<void, 38>(this, IMaterial, MaterialData);
	}

	CStrongHandle<CMaterial2> Create(const char* szMaterial, const char* szMaterialResource, const char* szVmtBuffer, const bool bBlendMode = true, const bool bTranslucent = true, const char* szForceShader = nullptr);
};