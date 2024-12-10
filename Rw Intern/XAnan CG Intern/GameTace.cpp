#include "Tace Memory.hpp"
#include "GameTace.hpp"
CEngineTrace::CEngineTrace() noexcept {
	pfnTraceShape = reinterpret_cast<decltype(pfnTraceShape)>(Memory::FindPattern("client.dll", "48 89 5C 24 20 48 89 4C 24 08 55 56 41 55 41 56"));
	pTraceManager = *reinterpret_cast<void**>(Memory::Relative(Memory::FindPattern("client.dll", "48 8B 0D ?? ?? ?? ?? 4C 8B C3 66 89 44 24"), 3, 7));
	pfnCreateFilter = reinterpret_cast<decltype(pfnCreateFilter)>(Memory::FindPattern("client.dll", "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20 0F B6 41 37 33"));
	bValid = pTraceManager != nullptr && pfnTraceShape != nullptr && pfnCreateFilter != nullptr;
}

CGameTrace CEngineTrace::TraceShape(Vector3& vecStart, Vector3& vecEnd, CBaseEntity* pSkipEntity, const uint64_t nMask) {
	if (!bValid) {
		return {};
	}

	CTraceRay pTraceRay = {};
	CTraceFilter pFilter = {};
	CGameTrace pGameTrace = {};
	pfnCreateFilter(&pFilter, pSkipEntity, nMask, 4, 7);
	pfnTraceShape(pTraceManager, &pTraceRay, &vecStart, &vecEnd, &pFilter, &pGameTrace);
	return pGameTrace;
}