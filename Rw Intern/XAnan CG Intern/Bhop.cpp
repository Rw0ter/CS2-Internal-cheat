#include <cstdint>
#include <iostream>

#include "Bhop.hpp"
#include "Search.h"
#include "mem Aimbot.h"
#include "imgui/imgui.h"
#include "GameTace.hpp"
#include "mem.hpp"
#include "CUserCmd.h"


void Bhop()
{
    Player LocalPlayer{};
    LocalPlayer.control = Address::GetLocalPlayerControl();
    LocalPlayer.pawn = Get::PlayerPawnAddress(LocalPlayer.control);

    uintptr_t baseAddress = static_cast<uintptr_t>(LocalPlayer.pawn);
    short* flagsPointer = reinterpret_cast<short*>(baseAddress + cs2_dumper::schemas::client_dll::C_BaseEntity::m_fFlags);

    // 读取当前玩家的flags
    short flags = *flagsPointer;
    CUserCMD* pUserCmd = CUserCMD::Get();
    if (!pUserCmd) {
        return; 
    }
    if (flags & (1 << 0) && pUserCmd->nButtons.nValue & IN_JUMP && Menu::Misc::Bhop) {
        printf("On ground\n");
        pUserCmd->nButtons.nValue &= ~IN_JUMP; 
    }
}
