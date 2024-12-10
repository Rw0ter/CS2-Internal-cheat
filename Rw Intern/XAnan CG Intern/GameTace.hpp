#pragma once
#include <optional>
#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <dwmapi.h>
#include <memory>
#include <atomic>
#include <wrl/client.h>
#include <mutex>


#include "kiero.h"
#include "imgui/imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "Vector.h"
class CTraceRay {
public:
    Vector3 vecStart;
    Vector3 vecEnd;
    Vector3 vecMins;
    Vector3 vecMaxs;
    CTraceRay() = default;
    CTraceRay(Vector3 Start, Vector3 End, Vector3 Mins, Vector3 Maxs) : vecStart(Start), vecEnd(End), vecMins(Mins), vecMaxs(Maxs) {};
private:
    char pad_01[0x5];
};

class CTraceFilter {
    char pad_01[0x8];
public:
    int64_t nTraceMask;
    int64_t arrUnknown[2];
    int32_t arrSkipHandles[4];
    int16_t arrCollisions[2];
    int16_t nUnknown2;
    uint8_t nUnknown3;
    uint8_t nUnknown4;
    uint8_t nUnknown5;
};

class CGameTrace {
public:
    void* pSurface;
    void* pHitEntity;
    void* pHitboxData;
    char pad_01[0x38];
    uint32_t nContents;
    char pad_02[0x24];
    Vector3 vecStart;
    Vector3 vecEnd;
    Vector3 vecNormal;
    Vector3 vecPosition;
    char pad_03[0x4];
    float flFraction;
    char pad_04[0x6];
    bool bStartSolid;
    char pad_05[0x4D];
    bool DidHit() const noexcept {
        return (this->flFraction < 1.0f || bStartSolid);
    }

    bool IsVisible() const noexcept {
        return this->flFraction > 0.9f;
    }
};

class CEngineTrace {
    using CBaseEntity = void;
    using fnCreateFilter = void(__fastcall*)(CTraceFilter*, void*, uint64_t, uint8_t, uint16_t);
    using fnTraceShape = bool(__fastcall*)(void*, CTraceRay*, Vector3*, Vector3*, CTraceFilter*, CGameTrace*);

    bool bValid = false;
    void* pTraceManager = nullptr;
    fnTraceShape pfnTraceShape = nullptr;
    fnCreateFilter pfnCreateFilter = nullptr;
public:
    CEngineTrace() noexcept;
    static std::shared_ptr<CEngineTrace>& Get() noexcept {
        static std::shared_ptr<CEngineTrace> pEngineTrace = std::make_shared<CEngineTrace>();
        return pEngineTrace;
    }

    CGameTrace TraceShape(Vector3& vecStart, Vector3& vecEnd, CBaseEntity* pSkipEntity = nullptr, const uint64_t nMask = 0x1C3003);
};