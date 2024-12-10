#include "CKeyValues3.hpp"
#include "IMaterialSystem2.hpp"
CStrongHandle<CMaterial2> IMaterialSystem2::Create(const char* szMaterial, const char* szMaterialResource, const char* szVmtBuffer, const bool bBlendMode, const bool bTranslucent, const char* szForceShader) {
	CStrongHandle<CMaterial2> hMaterialPrototype = {};
	FindOrCreateMaterialFromResource(&hMaterialPrototype, szMaterialResource);
	if (!hMaterialPrototype) {
		return {};
	}

	CKeyValues3* pKeyValues = CKeyValues3::Create();
	if (!pKeyValues || !pKeyValues->LoadFromKv3(szVmtBuffer, szForceShader)) {
		return {};
	}

	CStrongHandle<CMaterial2> ppMaterialResource;
	SetCreateDataByMaterial(&hMaterialPrototype, pKeyValues);
	if (szForceShader) {
		pKeyValues->SetShaderType(szForceShader);
	}

	pKeyValues->SetMaterialFunction("F_BLEND_MODE", static_cast<int>(bBlendMode));
	pKeyValues->SetMaterialFunction("F_TRANSLUCENT", static_cast<int>(bTranslucent));
	CreateNewMaterial(&ppMaterialResource, szMaterial, pKeyValues);
	if (!ppMaterialResource) {
		return {};
	}

	return ppMaterialResource;
}
