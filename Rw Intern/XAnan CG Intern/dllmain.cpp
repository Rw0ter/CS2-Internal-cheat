// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "include.h"
#include "Cheat.h"
#include "Search.h"
#include "Menu.h"
#include "imgui/font.h"
#include "Console.h"
#include "Tace Memory.hpp"
#include <Windows.h>
#include <cstdint>
#include "kiero/minhook/include/MinHook.h"
#include "GameState.h"
#include "weaponcheak.hpp"
#include "CCSGOInput.hpp"
#include "Bhop.hpp"
#include "IEngineCvar.hpp"
#include "CViewSetup.hpp"
#include "GameTace.hpp"
#include "CSceneAnimatableObjectDest.hpp"
#include "CMaterial2.hpp"
#include <functional>
#include "IGameResourceService.hpp"
#include "IMaterialSystem2.hpp"

bool init = false;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Present oPresent;
HWND window = NULL;
WNDPROC oWndProc;
ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;
ID3D11RenderTargetView* mainRenderTargetView;

void InitImGui()
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
	io.Fonts->AddFontFromMemoryTTF(&inter, sizeof inter, 16 * dpi_scale, NULL, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
	chinesefont = io.Fonts->AddFontFromFileTTF("c:/windows/fonts/msyh.ttc", 15.0f, NULL, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
	tab_text1 = io.Fonts->AddFontFromMemoryTTF(&inter, sizeof inter, 12 * dpi_scale, NULL, io.Fonts->GetGlyphRangesCyrillic());
	tab_text2 = io.Fonts->AddFontFromMemoryTTF(&inter, sizeof inter, 24 * dpi_scale, NULL, io.Fonts->GetGlyphRangesCyrillic());
	tab_text3 = io.Fonts->AddFontFromMemoryTTF(&inter, sizeof inter, 40 * dpi_scale, NULL, io.Fonts->GetGlyphRangesCyrillic());
	ico = io.Fonts->AddFontFromMemoryTTF(&icon, sizeof icon, 25 * dpi_scale, NULL, io.Fonts->GetGlyphRangesCyrillic());
	ico_2 = io.Fonts->AddFontFromMemoryTTF(&Menuicon, sizeof Menuicon, 20 * dpi_scale, NULL, io.Fonts->GetGlyphRangesCyrillic());
	ico_subtab = io.Fonts->AddFontFromMemoryTTF(&icon, sizeof icon, 35 * dpi_scale, NULL, io.Fonts->GetGlyphRangesCyrillic());
	ico_logo = io.Fonts->AddFontFromMemoryTTF(&icon, sizeof icon, 31 * dpi_scale, NULL, io.Fonts->GetGlyphRangesCyrillic());
	tab_textA = io.Fonts->AddFontFromMemoryTTF(&inter, sizeof inter, 19 * dpi_scale, NULL, io.Fonts->GetGlyphRangesCyrillic());
	ico_minimize = io.Fonts->AddFontFromMemoryTTF(&icon, sizeof icon, 27 * dpi_scale, NULL, io.Fonts->GetGlyphRangesCyrillic());

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(pDevice, pContext);
}

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	if (true && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}



HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	if (!init)
	{
		if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice)))
		{
			pDevice->GetImmediateContext(&pContext);
			DXGI_SWAP_CHAIN_DESC sd;
			pSwapChain->GetDesc(&sd);
			window = sd.OutputWindow;
			ID3D11Texture2D* pBackBuffer;
			pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
			pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
			pBackBuffer->Release();
			oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
			InitImGui();
			init = true;
		}

		else
			return oPresent(pSwapChain, SyncInterval, Flags);
	}


	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	Cheat::Run();

	ImGui::Render();

	pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	return oPresent(pSwapChain, SyncInterval, Flags);
}

static void(__fastcall* fnOriginalCreateMove)(void*, int, uint8_t) = nullptr;
static void hkCreateMove(void* pCSGOInput, int nSlot, uint8_t bActive) {
	fnOriginalCreateMove(pCSGOInput, nSlot, bActive);
	if (GameState::IsMatchStarted()) {
		Aimbot::OnTriggerBot();
		if (Menu::bAimBot && weaponcheck() == true) {
			Aimbot::Start();
		}

		Bhop();
		if (Menu::Antiaim::bEnable && !Menu::SafeMode) { AntiAim(); } // 测试 move correct
		if (Menu::Misc::bStandaloneQuickStop) { StandaloneQuickStop(); }
		MovementCorrection();
		if (!Menu::SafeMode) { AutoStrafe(); }
	}
}

inline void(__fastcall* fnOriginalValidateInput)(CCSGOInput*, int) = nullptr;
static void __fastcall hkValidateInput(CCSGOInput* input, int a2) {
	Vector3 angOriginalAngle = input->GetViewAngles();
	fnOriginalValidateInput(input, a2);
	input->SetViewAngles(angOriginalAngle);
} 

inline void*(__fastcall* fnOriginalLevelInit)(void*, void*) = nullptr;
static void* hkLevelInit(void* pClientMode, void* pszMapName) {
	void* pResult = fnOriginalLevelInit(pClientMode, pszMapName);
	IEngineCvar::Get()->Setup();
	return pResult;
}

inline void* (__fastcall* fnOriginalLevelShutDown)(void*) = nullptr;
static void* hkLevelShutDown(void* pClientMode) {
	void* pResult = fnOriginalLevelShutDown(pClientMode);
	IEngineCvar::Get()->Setup();
	return pResult;
}

static void Thirdperson(CViewSetup* pViewSetup) {
	Player LocalPlayer{};
	LocalPlayer.control = Address::GetLocalPlayerControl();
	LocalPlayer.pawn = Get::PlayerPawnAddress(LocalPlayer.control);
	if (!LocalPlayer.pawn) {
		return;
	}

	uintptr_t pGameSceneNode = *reinterpret_cast<uintptr_t*>(LocalPlayer.pawn + cs2_dumper::schemas::client_dll::C_BaseEntity::m_pGameSceneNode);
	if (!pGameSceneNode) {
		return;
	}

	// setting
	float flThirdpersonDistance = Menu::Misc::flThirdpersonDistance; // 第三人称距离


	Vector3 vecOrigin = *reinterpret_cast<Vector3*>(pGameSceneNode + cs2_dumper::schemas::client_dll::CGameSceneNode::m_vecOrigin);
	Vector3 vecViewOffset = *reinterpret_cast<Vector3*>(LocalPlayer.pawn + cs2_dumper::schemas::client_dll::C_BaseModelEntity::m_vecViewOffset);
	Vector3 vecEyePosition = vecOrigin + vecViewOffset;

	Vector3 vecCameraAngles = pViewSetup->angles;
	Vector3 vecForward = {}, vecRight = {}, vecUp = {};
	vecCameraAngles.AnglesToVectors(&vecForward, &vecRight, &vecUp);


	Vector3 vecThirdpersonPosition = vecEyePosition + (vecForward * - flThirdpersonDistance) + vecRight + vecUp;
	CGameTrace pTrace = CEngineTrace::Get()->TraceShape(
		vecEyePosition, vecThirdpersonPosition, (void*)LocalPlayer.pawn
	);

	pViewSetup->origin = pTrace.vecEnd;
}

inline void* (__fastcall* fnOriginalOverrideView)(void*, CViewSetup*) = nullptr;
static void hkOverrideView(void* ClientSharedMode, CViewSetup* ViewSetup) {
	fnOriginalOverrideView(ClientSharedMode, ViewSetup);


	static bool ThirdpersonToggle = true;
	static DWORD lastToggleTime = 0; 

	if (GetAsyncKeyState(Menu::Misc::ThirdpersonKey) & 0x8000) { 
		DWORD currentTime = GetTickCount(); 
		if (currentTime - lastToggleTime > 300) { 
			ThirdpersonToggle = !ThirdpersonToggle; 
			lastToggleTime = currentTime; 
		}
	}

	if (Menu::Misc::Thirdperson && ThirdpersonToggle) {
		Thirdperson(ViewSetup);
	}
}

class CDrawMaterials {
public:
	static std::unique_ptr<CDrawMaterials>& Get() {
		static std::unique_ptr<CDrawMaterials> pMaterial = std::make_unique<CDrawMaterials>();
		return pMaterial;
	}

	CStrongHandle<CMaterial2> pFlatVisible = IMaterialSystem2::Get()->Create( // flat - 可视材质
		"Flat - Visible", "materials/dev/primary_white.vmat",
		R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d} format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} --> {
			F_IGNOREZ = 0
			F_DISABLE_Z_WRITE = 0
			F_RENDER_BACKFACES = 1
			F_DISABLE_Z_BUFFERING = 0
			Shader = "csgo_unlitgeneric.vfx"
			g_vColorTint = [1.0, 1.0, 1.0, 1.0]
			g_tNormal = resource:"materials/default/default_normal_tga_7652cb.vtex"
			g_tColor = resource:"materials/dev/primary_white_color_tga_21186c76.vtex"
			g_tRoughness = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
			g_tMetalness = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
			g_tAmbientOcclusion = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
		})"
	);

	CStrongHandle<CMaterial2> pFlatInVisible = IMaterialSystem2::Get()->Create( // flat - 不可视材质
		"Flat - InVisible", "materials/dev/primary_white.vmat",
		R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d} format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} --> {
			F_IGNOREZ = 1
			F_DISABLE_Z_WRITE = 1
			F_RENDER_BACKFACES = 1
			F_DISABLE_Z_BUFFERING = 1
			Shader = "csgo_unlitgeneric.vfx"
			g_vColorTint = [1.0, 1.0, 1.0, 1.0]
			g_tNormal = resource:"materials/default/default_normal_tga_7652cb.vtex"
			g_tColor = resource:"materials/dev/primary_white_color_tga_21186c76.vtex"
			g_tRoughness = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
			g_tMetalness = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
			g_tAmbientOcclusion = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
		})"
	);

	CStrongHandle<CMaterial2> pRegularVisible = IMaterialSystem2::Get()->Create( // regular - 可视材质
		"Regular - Visible", "materials/dev/primary_white.vmat",
		R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d} format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} --> {
			F_IGNOREZ = 0
			F_TRANSLUCENT = 1
			F_DISABLE_Z_WRITE = 0
			F_RENDER_BACKFACES = 0
			F_DISABLE_Z_BUFFERING = 0
			F_PAINT_VERTEX_COLORS = 1
			Shader = "csgo_complex.vfx"
			g_vColorTint = [1.000000, 1.000000, 1.000000, 1.000000]
			TextureNormal = resource:"materials/default/default_normal.tga"
			g_tColor = resource:"materials/default/default_color_tga_72dcfbfd.vtex"
			g_tNormal = resource:"materials/default/default_normal_tga_7be61377.vtex"
			g_tAmbientOcclusion = resource:"materials/default/default_ao_tga_559f1ac6.vtex"
		})"
	);

	CStrongHandle<CMaterial2> pRegularInVisible = IMaterialSystem2::Get()->Create( // regular - 不可视材质
		"Regular - InVisible", "materials/dev/primary_white.vmat",
		R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d} format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} --> {
			F_IGNOREZ = 0
			F_TRANSLUCENT = 1
			F_DISABLE_Z_WRITE = 1
			F_RENDER_BACKFACES = 0
			F_DISABLE_Z_BUFFERING = 1
			F_PAINT_VERTEX_COLORS = 1
			Shader = "csgo_complex.vfx"
			g_vColorTint = [1.000000, 1.000000, 1.000000, 1.000000]
			TextureNormal = resource:"materials/default/default_normal.tga"
			g_tColor = resource:"materials/default/default_color_tga_72dcfbfd.vtex"
			g_tNormal = resource:"materials/default/default_normal_tga_7be61377.vtex"
			g_tAmbientOcclusion = resource:"materials/default/default_ao_tga_559f1ac6.vtex"
		})"
	);

	CStrongHandle<CMaterial2> pLatexVisible = IMaterialSystem2::Get()->Create( // latex - 可视材质
		"Latex - Visible", "materials/dev/primary_white.vmat",
		R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d} format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} --> {
			F_BLEND_MODE = 1
			g_bFogEnabled = 0
			g_flMetalness = 0.000
			F_DISABLE_Z_WRITE = 0
			F_DISABLE_Z_BUFFERING = 0
			Shader = "csgo_character.vfx"
			g_vColorTint = [1.0, 1.0, 1.0, 1.0]
			g_tColor = resource:"materials/dev/primary_white_color_tga_21186c76.vtex"
			g_tNormal = resource:"materials/default/default_normal_tga_1b833b2a.vtex"
			g_tMetalness = resource:"materials/default/default_metal_tga_8fbc2820.vtex"
			g_tAmbientOcclusion = resource:"materials/default/default_ao_tga_79a2e0d0.vtex"
		})"
	);

	CStrongHandle<CMaterial2> pLatexInVisible = IMaterialSystem2::Get()->Create( // latex - 不可视材质
		"Latex - InVisible", "materials/dev/primary_white.vmat",
		R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d} format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} --> {
			F_BLEND_MODE = 1
			g_bFogEnabled = 0
			F_DISABLE_Z_WRITE = 1
			g_flMetalness = 0.000
			F_DISABLE_Z_PREPASS = 1
			F_DISABLE_Z_BUFFERING = 1
			Shader = "csgo_character.vfx"
			g_vColorTint = [1.0, 1.0, 1.0, 1.0]
			g_tColor = resource:"materials/dev/primary_white_color_tga_21186c76.vtex"
			g_tNormal = resource:"materials/default/default_normal_tga_1b833b2a.vtex"
			g_tMetalness = resource:"materials/default/default_metal_tga_8fbc2820.vtex"
			g_tAmbientOcclusion = resource:"materials/default/default_ao_tga_79a2e0d0.vtex"
		})"
	);

	CStrongHandle<CMaterial2> pGlassVisible = IMaterialSystem2::Get()->Create( // glass - 可视材质
		"Glass - Visible", "materials/dev/primary_white.vmat",
		R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d} format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} --> {
			F_IGNOREZ = 0
			F_BLEND_MODE = 1
			F_TRANSLUCENT = 1
			g_flFresnelMin = 1
			F_ADDITIVE_BLEND = 1
			g_flFresnelMax = 0.0
			F_DISABLE_Z_WRITE = 0
			g_flFresnelFalloff = 1
			F_RENDER_BACKFACES = 1
			g_flOpacityScale = 0.45
			F_DISABLE_Z_BUFFERING = 0
			g_flFresnelExponent = 0.75
			Shader = "csgo_effects.vfx"
			g_vColorTint = [1.0, 1.0, 1.0, 0.0]
			g_tMask1 = resource:"materials/default/default_mask_tga_344101f8.vtex"
			g_tMask2 = resource:"materials/default/default_mask_tga_344101f8.vtex"
			g_tMask3 = resource:"materials/default/default_mask_tga_344101f8.vtex"
			g_tNormal = resource:"materials/default/default_normal_tga_7652cb.vtex"
			g_tColor = resource:"materials/dev/primary_white_color_tga_21186c76.vtex"
		})"
	);

	CStrongHandle<CMaterial2> pGlassInVisible = IMaterialSystem2::Get()->Create( // glass - 不可视材质
		"Glass - InVisible", "materials/dev/primary_white.vmat",
		R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d} format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} --> {
			F_IGNOREZ = 1
			F_BLEND_MODE = 1
			F_TRANSLUCENT = 1
			g_flFresnelMin = 1
			F_ADDITIVE_BLEND = 1
			g_flFresnelMax = 0.0
			F_DISABLE_Z_WRITE = 1
			g_flFresnelFalloff = 1
			F_RENDER_BACKFACES = 1
			g_flOpacityScale = 0.45
			F_DISABLE_Z_BUFFERING = 1
			g_flFresnelExponent = 0.75
			Shader = "csgo_effects.vfx"
			g_vColorTint = [1.0, 1.0, 1.0, 0.0]
			g_tMask1 = resource:"materials/default/default_mask_tga_344101f8.vtex"
			g_tMask2 = resource:"materials/default/default_mask_tga_344101f8.vtex"
			g_tMask3 = resource:"materials/default/default_mask_tga_344101f8.vtex"
			g_tNormal = resource:"materials/default/default_normal_tga_7652cb.vtex"
			g_tColor = resource:"materials/dev/primary_white_color_tga_21186c76.vtex"
		})"
	);

	CStrongHandle<CMaterial2> pBloomVisible = IMaterialSystem2::Get()->Create( // bloom - 可视材质
		"Bloom - Visible", "materials/dev/primary_white.vmat",
		R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d} format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} --> {
			F_IGNOREZ = 0
			F_SELF_ILLUM = 1
			F_TRANSLUCENT = 1 
			F_DISABLE_Z_WRITE = 0
			Shader = "solidcolor.vfx"
			F_DISABLE_Z_BUFFERING = 0
			F_PAINT_VERTEX_COLORS = 1
			g_vColorTint = [20.000000, 20.000000, 20.000000, 20.000000]
			g_flSelfIllumScale = [5.000000, 5.000000, 5.000000, 5.000000]
			g_vSelfIllumTint = [10.000000, 10.000000, 10.000000, 10.000000]
			g_flSelfIllumBrightness = [5.000000, 5.000000, 5.000000, 5.000000]
			g_tAmbientOcclusion = resource:"materials/debug/particleerror.vtex"
			g_tColor = resource:"materials/default/default_mask_tga_fde710a5.vtex"
			g_tNormal = resource:"materials/default/default_mask_tga_fde710a5.vtex"
			TextureAmbientOcclusion = resource:"materials/debug/particleerror.vtex"
			g_tRoughness = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
			g_tMetalness = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
			g_tSelfIllumMask = resource:"materials/default/default_mask_tga_fde710a5.vtex"
		})"
	);

	CStrongHandle<CMaterial2> pBloomInVisible = IMaterialSystem2::Get()->Create( // bloom - 不可视材质
		"Bloom - InVisible", "materials/dev/primary_white.vmat",
		R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d} format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} --> {
			F_IGNOREZ = 0
			F_SELF_ILLUM = 1
			F_TRANSLUCENT = 1 
			F_DISABLE_Z_WRITE = 1
			Shader = "solidcolor.vfx"
			F_DISABLE_Z_BUFFERING = 1
			F_PAINT_VERTEX_COLORS = 1
			g_vColorTint = [20.000000, 20.000000, 20.000000, 20.000000]
			g_flSelfIllumScale = [5.000000, 5.000000, 5.000000, 5.000000]
			g_vSelfIllumTint = [10.000000, 10.000000, 10.000000, 10.000000]
			g_flSelfIllumBrightness = [5.000000, 5.000000, 5.000000, 5.000000]
			g_tAmbientOcclusion = resource:"materials/debug/particleerror.vtex"
			g_tColor = resource:"materials/default/default_mask_tga_fde710a5.vtex"
			g_tNormal = resource:"materials/default/default_mask_tga_fde710a5.vtex"
			TextureAmbientOcclusion = resource:"materials/debug/particleerror.vtex"
			g_tRoughness = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
			g_tMetalness = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
			g_tSelfIllumMask = resource:"materials/default/default_mask_tga_fde710a5.vtex"
		})"
	);

	CStrongHandle<CMaterial2> pGlowVisible = IMaterialSystem2::Get()->Create( // glow - 可视材质
		"Glow - Visible", "materials/dev/primary_white.vmat",
		R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d} format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} --> {
			F_IGNOREZ = 0
			g_flFresnelMax = 0.1
			g_flFresnelMin = 1.0
			F_DISABLE_Z_WRITE = 0
			F_RENDER_BACKFACES = 0
			F_DISABLE_Z_BUFFERING = 0
			g_flFresnelExponent = 7.0
			g_flFresnelFalloff = 10.0
			Shader = "csgo_effects.vfx"
			g_flBeginMixingRoughness = 1.0
			g_flToolsVisCubemapReflectionRoughness = 1.0
			g_vColorTint = [1.000000, 1.000000, 1.000000, 0]
			g_tMask1 = resource:"materials/default/default_mask_tga_fde710a5.vtex"
			g_tMask2 = resource:"materials/default/default_mask_tga_fde710a5.vtex"
			g_tMask3 = resource:"materials/default/default_mask_tga_fde710a5.vtex"
			g_tColor = resource:"materials/dev/primary_white_color_tga_21186c76.vtex"
			g_tSceneDepth = resource:"materials/default/default_mask_tga_fde710a5.vtex"
		})"
	);

	CStrongHandle<CMaterial2> pGlowInVisible = IMaterialSystem2::Get()->Create( // glow - 不可视材质
		"Glow - InVisible", "materials/dev/primary_white.vmat",
		R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d} format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} --> {
			F_IGNOREZ = 1
			g_flFresnelMax = 0.1
			g_flFresnelMin = 1.0
			F_DISABLE_Z_WRITE = 1
			F_RENDER_BACKFACES = 0
			F_DISABLE_Z_BUFFERING = 1
			g_flFresnelExponent = 7.0
			g_flFresnelFalloff = 10.0
			Shader = "csgo_effects.vfx"
			g_flBeginMixingRoughness = 1.0
			g_flToolsVisCubemapReflectionRoughness = 1.0
			g_vColorTint = [1.000000, 1.000000, 1.000000, 0]
			g_tMask1 = resource:"materials/default/default_mask_tga_fde710a5.vtex"
			g_tMask2 = resource:"materials/default/default_mask_tga_fde710a5.vtex"
			g_tMask3 = resource:"materials/default/default_mask_tga_fde710a5.vtex"
			g_tColor = resource:"materials/dev/primary_white_color_tga_21186c76.vtex"
			g_tSceneDepth = resource:"materials/default/default_mask_tga_fde710a5.vtex"
		})"
	);

	CStrongHandle<CMaterial2> pOrderlessVisible = IMaterialSystem2::Get()->Create( // orderless - 可视材质
		"Orderless - Visible", "materials/dev/primary_white.vmat",
		R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d} format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} --> {
			F_IGNOREZ = 0
			F_BLEND_MODE = 1
			F_DISABLE_Z_WRITE = 0
			F_RENDER_BACKFACES = 0
			Shader = "solidcolor.vfx"
			F_DISABLE_Z_BUFFERING = 0
			g_vColorTint = [50.0, 50.0, 50.0, 50.0]
			g_tNormal = resource:"materials/default/default_normal_tga_7652cb.vtex"
			g_tColor = resource:"materials/dev/primary_white_color_tga_21186c76.vtex"
			g_tRoughness = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
			g_tMetalness = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
			g_tAmbientOcclusion = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
		})"
	);

	CStrongHandle<CMaterial2> pOrderlessInVisible = IMaterialSystem2::Get()->Create( // orderless - 不可视材质
		"Orderless - InVisible", "materials/dev/primary_white.vmat",
		R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d} format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} --> {
			F_IGNOREZ = 0
			F_BLEND_MODE = 1
			F_DISABLE_Z_WRITE = 1
			F_RENDER_BACKFACES = 0
			Shader = "solidcolor.vfx"
			F_DISABLE_Z_BUFFERING = 1
			g_vColorTint = [50.0, 50.0, 50.0, 50.0]
			g_tNormal = resource:"materials/default/default_normal_tga_7652cb.vtex"
			g_tColor = resource:"materials/dev/primary_white_color_tga_21186c76.vtex"
			g_tRoughness = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
			g_tMetalness = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
			g_tAmbientOcclusion = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
		})"
	);
};

enum eMaterialStyle : uint8_t { // 共6种风格上色模式
	eFlatStyle,
	eRegularStyle,
	eLatexStyle,
	eGlassStyle,
	eBloomStyle,
	eGlowStyle,
	eOrderlessStyle
};

static CStrongHandle<CMaterial2>& GetMaterial(const eMaterialStyle nStyle, const bool bVisible) {
	switch (nStyle) {
		case eFlatStyle: return (bVisible ? CDrawMaterials::Get()->pFlatVisible : CDrawMaterials::Get()->pFlatInVisible);
		case eRegularStyle: return (bVisible ? CDrawMaterials::Get()->pRegularVisible : CDrawMaterials::Get()->pRegularInVisible);
		case eLatexStyle: return (bVisible ? CDrawMaterials::Get()->pLatexVisible : CDrawMaterials::Get()->pLatexInVisible);
		case eGlassStyle: return (bVisible ? CDrawMaterials::Get()->pGlassVisible : CDrawMaterials::Get()->pGlassInVisible);
		case eBloomStyle: return (bVisible ? CDrawMaterials::Get()->pBloomVisible : CDrawMaterials::Get()->pBloomInVisible);
		case eGlowStyle: return (bVisible ? CDrawMaterials::Get()->pGlowVisible : CDrawMaterials::Get()->pGlowInVisible);
		case eOrderlessStyle: return (bVisible ? CDrawMaterials::Get()->pOrderlessVisible : CDrawMaterials::Get()->pOrderlessInVisible);
	}

	return bVisible ? CDrawMaterials::Get()->pFlatVisible : CDrawMaterials::Get()->pFlatInVisible;
}

inline void(__fastcall* fnOriginalDrawSceneObject)(void*, void*, CMeshData*, int, void*, void*, void*, void*) = nullptr;
static bool Chams(void* pSceneObject, void* pDevice, CMeshData* pData, int nDataSize, void* pView, void* pLayer, void* pArg7, void* pArg8) {
	CSceneAnimatableObject* pSceneAnimatableObject = pData->m_pSceneAnimatableObject;
	if (!pSceneAnimatableObject) {
		return false;
	}

	IGameEntitySystem* pGameEntitySystem = IGameEntitySystem::Get();
	if (!pGameEntitySystem) {
		return false;
	}

	void* pLocalPawn = reinterpret_cast<void*>(Get::PlayerPawnAddress(Address::GetLocalPlayerControl()));
	if (!pLocalPawn) {
		return false;
	}

	void* pEntity = pGameEntitySystem->GetFromHandle(pSceneAnimatableObject->m_hEntity);
	if (!pEntity) {
		return false;
	}

	CSchemaClassInfoData* pClassInfo = Get::GetSchemaBinding(reinterpret_cast<intptr_t>(pEntity));
	if (!pClassInfo || !pClassInfo->szName || !*pClassInfo->szName) {
		return false;
	}

	uint64_t nHashClasses = HASH_RT(pClassInfo->szName);
	CConVar* pTeammateAreEnemies = IEngineCvar::Get()->Find("mp_teammates_are_enemies"); // 判定有没有开启全体伤害
	if (!pTeammateAreEnemies) {
		return false;
	}

	std::string szMaterialName = pData->pMaterial->GetName();
	// settings
	// 自己上色设置
	bool bLocalPlayerModelChams = Menu::ESP::bLocalPlayerChams;
	Color clrLocalPlayerModel = Menu::Color::LocalPlayerVisibleColor;
	eMaterialStyle nLocalPlayerMaterial = eMaterialStyle::eFlatStyle;
	int LocalPlayercurrentStyleIndex = Menu::ESP::LocalPlayerChams;
	eMaterialStyle LocalPlayerselectedStyle = static_cast<eMaterialStyle>(LocalPlayercurrentStyleIndex);

	// 获取材质
	switch (LocalPlayerselectedStyle) {
	case eFlatStyle:
		nLocalPlayerMaterial = eMaterialStyle::eFlatStyle;
		break;
	case eRegularStyle:
		nLocalPlayerMaterial = eMaterialStyle::eRegularStyle;
		break;
	case eLatexStyle:
		nLocalPlayerMaterial = eMaterialStyle::eLatexStyle;
		break;
	case eGlassStyle:
		nLocalPlayerMaterial = eMaterialStyle::eGlassStyle;
		break;
	case eBloomStyle:
		nLocalPlayerMaterial = eMaterialStyle::eBloomStyle;
		break;
	case eGlowStyle:
		nLocalPlayerMaterial = eMaterialStyle::eGlowStyle;
		break;
	case eOrderlessStyle:
		nLocalPlayerMaterial = eMaterialStyle::eOrderlessStyle;
		break;
	default:
		nLocalPlayerMaterial = eMaterialStyle::eFlatStyle;
		break;
	}




	// 队友上色设置
	bool bTeammateVisibleChams = Menu::ESP::bTeammateVisibleChams;
	bool bTeammateInVisibleChams = Menu::ESP::bTeammateInVisibleChams;
	Color clrTeammateVisible = Menu::Color::TeammateVisibleColor;
	Color clrTeammateInVisible = Menu::Color::TeammateInVisibleColor;
	eMaterialStyle nTeammateVisibleMaterial = eMaterialStyle::eFlatStyle;
	eMaterialStyle nTeammateInVisibleMaterial = eMaterialStyle::eFlatStyle;
	int TeammatecurrentStyleIndex = Menu::ESP::	TeammateVisibleChams;
	int TeammateIncurrentStyleIndex = Menu::ESP::TeammateInVisibleChams;
	eMaterialStyle TeammateselectedStyle = static_cast<eMaterialStyle>(TeammatecurrentStyleIndex);
	eMaterialStyle TeammateInselectedStyle = static_cast<eMaterialStyle>(TeammateIncurrentStyleIndex);


	// 可见材质
	switch (TeammateselectedStyle) {
	case eFlatStyle:
		nTeammateVisibleMaterial = eMaterialStyle::eFlatStyle;
		break;
	case eRegularStyle:
		nTeammateVisibleMaterial = eMaterialStyle::eRegularStyle;
		break;
	case eLatexStyle:
		nTeammateVisibleMaterial = eMaterialStyle::eLatexStyle;
		break;
	case eGlassStyle:
		nTeammateVisibleMaterial = eMaterialStyle::eGlassStyle;
		break;
	case eBloomStyle:
		nTeammateVisibleMaterial = eMaterialStyle::eBloomStyle;
		break;
	case eGlowStyle:
		nTeammateVisibleMaterial = eMaterialStyle::eGlowStyle;
		break;
	case eOrderlessStyle:
		nTeammateVisibleMaterial = eMaterialStyle::eOrderlessStyle;
		break;
	default:
		nTeammateVisibleMaterial = eMaterialStyle::eFlatStyle;
		break;
	}



	// 不可见材质
	switch (TeammateInselectedStyle) {
	case eFlatStyle:
		nTeammateInVisibleMaterial = eMaterialStyle::eFlatStyle;
		break;
	case eRegularStyle:
		nTeammateInVisibleMaterial = eMaterialStyle::eRegularStyle;
		break;
	case eLatexStyle:
		nTeammateInVisibleMaterial = eMaterialStyle::eLatexStyle;
		break;
	case eGlassStyle:
		nTeammateInVisibleMaterial = eMaterialStyle::eGlassStyle;
		break;
	case eBloomStyle:
		nTeammateInVisibleMaterial = eMaterialStyle::eBloomStyle;
		break;
	case eGlowStyle:
		nTeammateInVisibleMaterial = eMaterialStyle::eGlowStyle;
		break;
	case eOrderlessStyle:
		nTeammateInVisibleMaterial = eMaterialStyle::eOrderlessStyle;
		break;
	default:
		nTeammateInVisibleMaterial = eMaterialStyle::eFlatStyle;
		break;
	}





	// 敌人上色设置
	bool bEnemiesVisibleChams = Menu::ESP::bEnemiesVisibleChams;
	bool bEnemiesInVisibleChams = Menu::ESP::bEnemiesInVisibleChams;
	Color clrEnemiesVisible = Menu::Color::EnemiesVisibleColor;
	Color clrEnemiesInVisible = Menu::Color::EnemiesInVisibleColor;
	eMaterialStyle nEnemiesVisibleMaterial = eMaterialStyle::eFlatStyle;
	eMaterialStyle nEnemiesInVisibleMaterial = eMaterialStyle::eFlatStyle;
	int EnemiescurrentStyleIndex = Menu::ESP::EnemiesVisibleChams;
	int EnemiesIncurrentStyleIndex = Menu::ESP::EnemiesInVisibleChams;
	eMaterialStyle EnemiesselectedStyle = static_cast<eMaterialStyle>(EnemiescurrentStyleIndex);
	eMaterialStyle EnemiesInselectedStyle = static_cast<eMaterialStyle>(EnemiesIncurrentStyleIndex);


	// 可见材质
	switch (EnemiesselectedStyle) {
	case eFlatStyle:
		nEnemiesVisibleMaterial = eMaterialStyle::eFlatStyle;
		break;
	case eRegularStyle:
		nEnemiesVisibleMaterial = eMaterialStyle::eRegularStyle;
		break;
	case eLatexStyle:
		nEnemiesVisibleMaterial = eMaterialStyle::eLatexStyle;
		break;
	case eGlassStyle:
		nEnemiesVisibleMaterial = eMaterialStyle::eGlassStyle;
		break;
	case eBloomStyle:
		nEnemiesVisibleMaterial = eMaterialStyle::eBloomStyle;
		break;
	case eGlowStyle:
		nEnemiesVisibleMaterial = eMaterialStyle::eGlowStyle;
		break;
	case eOrderlessStyle:
		nEnemiesVisibleMaterial = eMaterialStyle::eOrderlessStyle;
		break;
	default:
		nEnemiesVisibleMaterial = eMaterialStyle::eFlatStyle;
		break;
	}



	// 不可见材质
	switch (EnemiesInselectedStyle) {
	case eFlatStyle:
		nEnemiesInVisibleMaterial = eMaterialStyle::eFlatStyle;
		break;
	case eRegularStyle:
		nEnemiesInVisibleMaterial = eMaterialStyle::eRegularStyle;
		break;
	case eLatexStyle:
		nEnemiesInVisibleMaterial = eMaterialStyle::eLatexStyle;
		break;
	case eGlassStyle:
		nEnemiesInVisibleMaterial = eMaterialStyle::eGlassStyle;
		break;
	case eBloomStyle:
		nEnemiesInVisibleMaterial = eMaterialStyle::eBloomStyle;
		break;
	case eGlowStyle:
		nEnemiesInVisibleMaterial = eMaterialStyle::eGlowStyle;
		break;
	case eOrderlessStyle:
		nEnemiesInVisibleMaterial = eMaterialStyle::eOrderlessStyle;
		break;
	default:
		nEnemiesInVisibleMaterial = eMaterialStyle::eFlatStyle;
		break;
	}






	// 下面3个暂时懒得写了所以.eg
	if (nHashClasses == HASH_RT("C_CSWeaponBase")) { // 第三人称视角手持武器
		// .eg, 待添加
	} else if (nHashClasses == HASH_RT("C_CSGOViewModel")) { // 第一人称手持武器, 只会绘制自己的手臂因为只有本地玩家才有第一人称模型绘制
		// .eg, 待添加
	} else if (szMaterialName.find("arms") != std::string::npos || szMaterialName.find("glove") != std::string::npos || szMaterialName.find("sleeve") != std::string::npos) { // 第一人选下自己的手臂, 袖口等衣角的上色
		// .eg, 待添加
	} else if (nHashClasses == HASH_RT("C_CSPlayerPawn")) { // 是玩家
		const int nTeam = Get::PlayerTeam(reinterpret_cast<intptr_t>(pEntity));
		const int nLocalTeam = Get::PlayerTeam(reinterpret_cast<intptr_t>(pLocalPawn));
		if (pEntity == pLocalPawn) { // 自己
			if (bLocalPlayerModelChams) { // 允许给自己上色
				pData->SetColor(clrLocalPlayerModel);
				pData->ForceOverrideMaterial(GetMaterial(nLocalPlayerMaterial, true));
				fnOriginalDrawSceneObject(pSceneObject, pDevice, pData, nDataSize, pView, pLayer, pArg7, pArg8);
				return true;
			}

		} else if (nTeam == nLocalTeam && (!pTeammateAreEnemies->GetBool())) { // 队友
			bool bSkipOriginalModel = false;
			if (bTeammateInVisibleChams) { // 允许显示队友不可视上色
				bSkipOriginalModel = true;
				pData->SetColor(clrTeammateInVisible);
				pData->ForceOverrideMaterial(GetMaterial(nTeammateInVisibleMaterial, false));
				fnOriginalDrawSceneObject(pSceneObject, pDevice, pData, nDataSize, pView, pLayer, pArg7, pArg8);
			}

			if (bTeammateVisibleChams) { // 允许显示队友可视上色
				bSkipOriginalModel = true;
				pData->SetColor(clrTeammateVisible);
				pData->ForceOverrideMaterial(GetMaterial(nTeammateVisibleMaterial, true));
				fnOriginalDrawSceneObject(pSceneObject, pDevice, pData, nDataSize, pView, pLayer, pArg7, pArg8);
			}

			return bSkipOriginalModel;
		} else { // 敌人
			bool bSkipOriginalModel = false;
			if (bEnemiesInVisibleChams) { // 允许显示敌人不可视上色
				bSkipOriginalModel = true;
				pData->SetColor(clrEnemiesInVisible);
				pData->ForceOverrideMaterial(GetMaterial(nEnemiesInVisibleMaterial, false));
				fnOriginalDrawSceneObject(pSceneObject, pDevice, pData, nDataSize, pView, pLayer, pArg7, pArg8);
			}

			if (bEnemiesVisibleChams) { // 允许显示敌人可视上色
				bSkipOriginalModel = true;
				pData->SetColor(clrEnemiesVisible);
				pData->ForceOverrideMaterial(GetMaterial(nEnemiesVisibleMaterial, true));
				fnOriginalDrawSceneObject(pSceneObject, pDevice, pData, nDataSize, pView, pLayer, pArg7, pArg8);
			}

			return bSkipOriginalModel;
		}
	}

	return false;
}

static void hkDrawSceneObject(void* pSceneObject, void* pDevice, CMeshData* pData, int nDataSize, void* pView, void* pLayer, void* pArg7, void* pArg8) {
	if (pData != nullptr && Chams(pSceneObject, pDevice, pData, nDataSize, pView, pLayer, pArg7, pArg8)) {
		return;
	}

	fnOriginalDrawSceneObject(pSceneObject, pDevice, pData, nDataSize, pView, pLayer, pArg7, pArg8);
}

DWORD WINAPI MainThread(LPVOID lpReserved)
{

	PatternScan patternScan{};
	//Console::InitConsole();
	if (patternScan.InitPointers())
	{
		void* pCCSGOInput = CCSGOInput::Get();
		void* pfnCreateMove = (*reinterpret_cast<void***>(pCCSGOInput))[5];
		void* pfnValidateInput = (*reinterpret_cast<void***>(pCCSGOInput))[7];
		bool init_hook = false;
		MH_Initialize();
		IEngineCvar::Get()->Setup();
		if (MH_CreateHook(pfnCreateMove, hkCreateMove, reinterpret_cast<void**>(&fnOriginalCreateMove)) != MH_OK || MH_EnableHook(pfnCreateMove) != MH_OK) {
			return FALSE;
		}
	
		if (MH_CreateHook(pfnValidateInput, hkValidateInput, reinterpret_cast<void**>(&fnOriginalValidateInput)) != MH_OK || MH_EnableHook(pfnValidateInput) != MH_OK) {
			return FALSE;
		}

		void* pfnLevelInit = Memory::FindPattern("client.dll", "48 89 5C 24 ? 56 48 83 EC ? 48 8B 0D ? ? ? ? 48 8B F2");
		void* pfnDrawSceneObject = Memory::FindPattern("scenesystem.dll", "48 8B C4 48 89 ?? ?? ?? 41 55 41 56 48 81 EC ?? ?? ?? ?? ?? ?? ?? 49 8B");
		void* pfnOverrideView = Memory::FindPattern("client.dll", "48 89 5C 24 ?? 48 89 6C 24 ?? 48 89 74 24 ?? 57 41 56 41 57 48 83 EC ?? 48 8B FA E8");
		void* pfnLevelShutDown = Memory::FindPattern("client.dll", "48 83 EC ? 48 8B 0D ? ? ? ? 48 8D 15 ? ? ? ? 45 33 C9 45 33 C0 48 8B 01 FF 50 ? 48 85 C0 74 ? 48 8B 0D ? ? ? ? 48 8B D0 4C 8B 01 41 FF 50 ? 48 83 C4");
		if (MH_CreateHook(pfnLevelInit, hkLevelInit, reinterpret_cast<void**>(&fnOriginalLevelInit)) != MH_OK || MH_EnableHook(pfnLevelInit) != MH_OK) {
			return FALSE;
		}

		if (MH_CreateHook(pfnOverrideView, hkOverrideView, reinterpret_cast<void**>(&fnOriginalOverrideView)) != MH_OK || MH_EnableHook(pfnOverrideView) != MH_OK) {
			return FALSE;
		}

		if (MH_CreateHook(pfnLevelShutDown, hkLevelShutDown, reinterpret_cast<void**>(&fnOriginalLevelShutDown)) != MH_OK || MH_EnableHook(pfnLevelShutDown) != MH_OK) {
			return FALSE;
		}

		if (MH_CreateHook(pfnDrawSceneObject, hkDrawSceneObject, reinterpret_cast<void**>(&fnOriginalDrawSceneObject)) != MH_OK || MH_EnableHook(pfnDrawSceneObject) != MH_OK) {
			return FALSE;
		}

		do
		{
			if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
			{
				kiero::bind(8, (void**)&oPresent, hkPresent);
				init_hook = true;
			}
		} while (!init_hook);

	}
	else
	{
		MessageBox(0,L"Update Offsets error !",0,0);
	}

	return TRUE;
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  dwReason,
	LPVOID lpReserved
) {
	if (dwReason == DLL_PROCESS_ATTACH) {
		DisableThreadLibraryCalls(hModule);
		HANDLE hThread = CreateThread(nullptr, 0, MainThread, hModule, 0, nullptr);
		if (hThread && hThread != INVALID_HANDLE_VALUE) {
			CloseHandle(hThread);
		}

	} else if (dwReason == DLL_PROCESS_DETACH) {
		kiero::shutdown();
	}

	return TRUE;
}

