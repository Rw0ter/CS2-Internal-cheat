#pragma once
#include <array>
#include "color.hpp"
#include <Windows.h>
#include "vector.h"
class CMaterialParam {
public:
	const char* Name;
	Vector4 vecValue;
	const char* StringValues;
	std::uint8_t $Align0[8u];
	void* TexValue;
	std::uint8_t $Align1[16u];
};

class CMaterial2 {
public:
	virtual const char* GetName() = 0;
	virtual const char* GetShareNameWithMod() = 0;
	virtual void* GetMode(void* pToken) = 0;
	virtual void* GetAttributes() = 0;
	virtual void vf4() = 0;
	virtual void vf5() = 0;
	virtual void vf6() = 0;
	virtual void vf7() = 0;
	virtual void vf8() = 0;
	virtual void vf9() = 0;
	virtual void vf10() = 0;
	virtual void vf11() = 0;
	virtual void vf12() = 0;
	virtual void vf13() = 0;
	virtual void vf14() = 0;
	virtual void vf15() = 0;
	virtual void vf16() = 0;
	virtual void vf17() = 0;
	virtual void* GetVertexShaderInputSignature() = 0;
	virtual void vf19() = 0;
	virtual void vf20() = 0;
	virtual void vf21() = 0;
	virtual void vf22() = 0;
	virtual bool FindParameter(const char* Parameter, CMaterialParam** MaterialParamOut) = 0;
	virtual void vf24() = 0;
	virtual void vf25() = 0;
	virtual void vf26() = 0;
	virtual void vf27() = 0;
	virtual void vf28() = 0;
	virtual void vf29() = 0;
	virtual void vf30() = 0;
	virtual void vf31() = 0;
	virtual void vf32() = 0;
	virtual void vf33() = 0;
};