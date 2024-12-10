#pragma once
#include <cstddef>
#include <vector>
#include <Windows.h>
#include <Psapi.h> 
#include <TlHelp32.h>
namespace Memory {
	template <typename Type, std::size_t VtableIndex, class CBaseClass, typename... Args>
	static __forceinline Type CallClassFn(CBaseClass* pClass, Args... ArgList) {
		using VFuncMethod = Type(__thiscall*)(const void*, decltype(ArgList)...);
		return (*reinterpret_cast<VFuncMethod* const*>(reinterpret_cast<std::uintptr_t>(pClass)))[VtableIndex](pClass, ArgList...);
	}

	inline void* GetExport(const char* szModule, const char* szProc) {
		HMODULE pModule = GetModuleHandleA(szModule);
		if (!pModule) {
			return nullptr;
		}

		return GetProcAddress(pModule, szProc);
	}

	inline uint8_t* Relative(std::uint8_t* nAddressBytes, std::uint32_t nRVAOffset, std::uint32_t nRIPOffset)
	{
		std::uint32_t nRVA = *reinterpret_cast<std::uint32_t*>(nAddressBytes + nRVAOffset);
		std::uint64_t nRIP = reinterpret_cast<std::uint64_t>(nAddressBytes) + nRIPOffset;

		return reinterpret_cast<std::uint8_t*>(nRVA + nRIP);
	}

	inline uint8_t* Absolute(std::uint8_t* nAddressBytes, std::uint32_t nPreOffset, std::uint32_t nAddOffset)
	{
		uintptr_t pBase = reinterpret_cast<uintptr_t>(nAddressBytes);
		pBase += nPreOffset;
		pBase += sizeof(int) + *(int*)(pBase);
		pBase += nAddOffset;
		return reinterpret_cast<std::uint8_t*>(pBase);
	}

	inline void* CreateInterface(const char* ModuleName, const char* InterfaceName) {
		HMODULE pModule = GetModuleHandleA(ModuleName);
		if (!pModule) {
			return nullptr;
		}

		void* pCreateInterface = GetProcAddress(pModule, "CreateInterface");
		if (!pCreateInterface) {
			return nullptr;
		}

		using GetInterfaceMethod = void* (*)(const char*, int*);
		const auto CreateInterfaceFn = (GetInterfaceMethod)pCreateInterface;
		return CreateInterfaceFn(InterfaceName, nullptr);
	}

	inline std::vector<std::uint32_t> PatternToBytes(const char* Pattern) {
		std::vector<std::uint32_t> Bytes;
		char* PatternStart = const_cast<char*>(Pattern);
		char* PatternFinal = const_cast<char*>(Pattern) + std::strlen(Pattern);
		for (char* CurrentChar = PatternStart; CurrentChar < PatternFinal; CurrentChar++) {
			if (*CurrentChar == '?') {
				CurrentChar++;
				if (*CurrentChar == '?') {
					CurrentChar++;
				}

				Bytes.push_back(-1);
			}
			else {
				Bytes.push_back(std::strtoul(CurrentChar, &CurrentChar, 16));
			}
		}

		return Bytes;
	}

	inline std::uint8_t* FindPattern(const char* ModuleName, const char* Pattern) {
		HMODULE ModuleBase = GetModuleHandleA(ModuleName);
		if (!ModuleBase) {
			return nullptr;
		}

		MODULEINFO ModuleInfo;
		if (!GetModuleInformation(GetCurrentProcess(), ModuleBase, &ModuleInfo, sizeof(ModuleInfo))) {
			return nullptr;
		}

		std::vector<std::uint32_t> Bytes = PatternToBytes(Pattern);
		size_t ByteSize = Bytes.size();
		std::uint32_t* BytesArray = Bytes.data();
		uint8_t* pBytes = reinterpret_cast<uint8_t*>(ModuleBase);
		for (size_t i = 0; i < ModuleInfo.SizeOfImage - ByteSize; i++) {
			bool bPatternResolved = true;
			for (size_t index = 0; index < ByteSize; index++) {
				if (pBytes[i + index] != BytesArray[index] && BytesArray[index] != -1) {
					bPatternResolved = false;
					break;
				}
			}

			if (bPatternResolved) {
				return (std::uint8_t*)&pBytes[i];
			}
		}

		return nullptr;
	}

}