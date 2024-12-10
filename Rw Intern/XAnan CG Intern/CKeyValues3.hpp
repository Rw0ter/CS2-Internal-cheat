#pragma once
#include <iostream>
#include "color.hpp"
#include "IMemAlloc.hpp"
#include "UtlStrongHandle.h"
class CMaterial2;
enum KeyVarBasicType : uint32_t {
	kv_basic_invalid,
	kv_basic_null,
	kv_basic_bool,
	kv_basic_int,
	kv_basic_uint,
	kv_basic_double,
	kv_basic_string,
	kv_basic_binary_blob,
	kv_basic_array,
	kv_basic_table
};

enum KeyVarFieldType : uint32_t {
	kv_field_invalid,
	kv_field_resource,
	kv_field_resource_name,
	kv_field_panorama,
	kv_field_soundevent,
	kv_field_subclass,
	kv_field_unspecified,
	kv_field_null,
	kv_field_binary_blob,
	kv_field_array,
	kv_field_table,
	kv_field_bool8,
	kv_field_char8,
	kv_field_uchar32,
	kv_field_int8,
	kv_field_uint8,
	kv_field_int16,
	kv_field_uint16,
	kv_field_int32,
	kv_field_uint32,
	kv_field_int64,
	kv_field_uint64,
	kv_field_float32,
	kv_field_float64,
	kv_field_string,
	kv_field_pointer,
	kv_field_color32,
	kv_field_vector,
	kv_field_vector2d,
	kv_field_vector4d,
	kv_field_rotation_vector,
	kv_field_quaternion,
	kv_field_qangle,
	kv_field_matrix3x4,
	kv_field_transform,
	kv_field_string_token,
	kv_field_ehandle
};

inline static constexpr int KeyVarsHash = 0x31415926;

class CUtlStringToken {
	std::uint64_t nKey;
	const char* szString;
public:
	std::uint64_t MurMurHash(const std::string_view strString);
	CUtlStringToken(const std::string_view strString) : nKey(MurMurHash(strString)), szString(strString.data()) {};
};

class CKeyValue3ID {
public:
	const char* szName;
	uint64_t nMurmurHashKey1;
	uint64_t nMurmurHashKey2;
	CKeyValue3ID() = default;
	CKeyValue3ID(const char* cName, uint64_t uUnk0, uint64_t uUnk1) : szName(cName), nMurmurHashKey1(uUnk0), nMurmurHashKey2(uUnk1) {}
};

class CKeyValues3 {
	char pad_01[0x100];
	uint64_t nKey;
	void* pData;
public:
	void SetShaderType(const char* szShaderName);
	void SetMaterialFunction(const char* szFunctionName, const int iValue);
	bool LoadFromKv3(const char* szBuffer, const char* szShader = "generic");
	CKeyValues3* SetType(KeyVarBasicType BasicType, KeyVarBasicType FieldType);
	void Release() const {
		IMemAlloc::Get()->Free(this);
	}

	static CKeyValues3* Create() {
		CKeyValues3* pKeyValues = IMemAlloc::Get()->Alloc<CKeyValues3*>(sizeof(CKeyValues3));
		if (!pKeyValues) {
			return nullptr;
		}

		return pKeyValues->SetType(kv_basic_null, kv_basic_string);
	}
};