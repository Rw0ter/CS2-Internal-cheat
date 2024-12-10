#pragma once
#include "Bit Flags.hpp"


void Bhop();
void AntiAim();
void AutoStrafe();
void MovementCorrection();
void StandaloneQuickStop();

class SomeClass {
public:
    short Flags();  // 这里不使用 __declspec 或 extern
};

