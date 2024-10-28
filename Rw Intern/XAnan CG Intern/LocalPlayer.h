#pragma once
#include "Address.h"
#include "Entity.h"
#include "Vector.h"
#include "mem Aimbot.h"

namespace LocalPlayer
{
	bool SetViewAngles(const Vector3& targetAngle);
	bool SetSmoothViewAngles(Vector3 pTargetAngle, const int pSmoothValue);
	Vector3 GetViewAngles();
};