#include "CKeyValues3.hpp"
std::uint64_t CUtlStringToken::MurMurHash(const std::string_view strString) {
	static auto fnMurMurHash64 = reinterpret_cast<uint64_t(__fastcall*)(const char*, uint32_t, int)>(Memory::FindPattern("particles.dll", "48 89 5C 24 ?? 57 48 81 EC ?? ?? ?? ?? 33 C0 8B DA"));
	return fnMurMurHash64(strString.data(), strString.length(), KeyVarsHash);
}

CKeyValues3* CKeyValues3::SetType(KeyVarBasicType BasicType, KeyVarBasicType FieldType) {
	static auto fnSetKeyValues3Type = reinterpret_cast<CKeyValues3*(__fastcall*)(void*, uint8_t, uint8_t)>(Memory::FindPattern("client.dll", "40 53 48 83 EC 20 48 8B 01 48 8B D9 44"));
	return fnSetKeyValues3Type(this, BasicType, FieldType);
}

void CKeyValues3::SetMaterialFunction(const char* szFunctionName, const int iValue) {
	CUtlStringToken strToken(szFunctionName);
	static auto fnSetMaterialFunction = reinterpret_cast<void(__fastcall*)(void*, CUtlStringToken*, int, uint8_t)>(Memory::FindPattern("particles.dll", "48 89 5C 24 ?? 48 89 6C 24 ?? 56 57 41 54 41 56 41 57 48 83 EC ?? 0F B6 01 45 0F B6 F9 8B 2A 48 8B F9"));
	return fnSetMaterialFunction(this, &strToken, iValue, 0x18);
}

bool CKeyValues3::LoadFromKv3(const char* szBuffer, const char* szShader) {
	static auto fnLoadFromKv3 = reinterpret_cast<bool(__fastcall*)(CKeyValues3*, void*, const char*, void*, char*)>(Memory::GetExport("tier0.dll", "?LoadKV3@@YA_NPEAVKeyValues3@@PEAVCUtlString@@PEBDAEBUKV3ID_t@@2@Z"));
	CKeyValue3ID tokenKeyValueID = CKeyValue3ID(szShader, 0x469806E97412167C, 0xE73790B53EE6F2AF);
	return fnLoadFromKv3(this, nullptr, szBuffer, &tokenKeyValueID, nullptr);
}

void CKeyValues3::SetShaderType(const char* szShaderName) {
	CUtlStringToken strToken("shader");
	static auto fnSetShaderType = reinterpret_cast<void(__fastcall*)(void*, CUtlStringToken&, const char*, uint8_t)>(Memory::Absolute(Memory::FindPattern("particles.dll", "E8 ?? ?? ?? ?? 41 B8 06"), 1, 0));
	return fnSetShaderType(this, strToken, szShaderName, 0x18);
};