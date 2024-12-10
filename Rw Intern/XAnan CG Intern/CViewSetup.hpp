#pragma once
#include "Vector.h"
class CViewSetup  {
public:
	char pad_01[0x490];
	float flOrthoLeft;
	float flOrthoTop;
	float flOrthoRight;
	float flOrthoBottom;
	char pad_02[0x38];
	float flFov;
	float flFovViewmodel;
	Vector3 origin;
	char pad_03[0xC];
	Vector3 angles;
	char pad_04[0x14];
	float flAspectRatio;
};