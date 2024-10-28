#include "mem Aimbot.h"
#include "imgui/imgui.h"
#include <unordered_set>
#include <iostream>

bool weaponcheck()
{
    Player LocalPlayer{};
    LocalPlayer.control = Address::GetLocalPlayerControl();
    LocalPlayer.pawn = Get::PlayerPawnAddress(LocalPlayer.control);

    std::string weaponName = Get::GetWeaponName(LocalPlayer.pawn);

    std::cout << weaponName << std::endl;

    // 使用集合存储不允许射击的武器
    static const std::unordered_set<std::string> restrictedWeapons = {
        "molotov", "incgrenade", "decoy",
        "flashbang", "hegrenade", "smokegrenade",
        "knife", "c4"
    };

    // 检查当前武器是否在限制列表中
    if (restrictedWeapons.find(weaponName) != restrictedWeapons.end())
    {
        std::cout << "can't shot" << std::endl;
        return false; // 不允许射击
    }

    return true; // 允许射击
}
