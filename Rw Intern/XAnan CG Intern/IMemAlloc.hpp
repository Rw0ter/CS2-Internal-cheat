#pragma once
#include "Tace Memory.hpp"
#pragma warning(push)
#pragma warning(disable : 4191)
class IMemAlloc {
public:
	static IMemAlloc* Get() {
		static IMemAlloc* pMemAlloc = *reinterpret_cast<IMemAlloc**>(Memory::GetExport("tier0.dll", "g_pMemAlloc"));
		return pMemAlloc;
	}

	template <typename Type = void*>
	Type Alloc(std::uint64_t nSize) {
		return reinterpret_cast<Type>(Memory::CallClassFn<void*, 1>(this, nSize));
	}

	void* ReAlloc(const void* pMemory, std::uint64_t nSize) {
		return Memory::CallClassFn<void*, 2>(this, pMemory, nSize);
	}

	void Free(const void* pMemory) {
		return Memory::CallClassFn<void, 3>(this, pMemory);
	}

	std::uint64_t GetSize(const void* pMemory) {
		return Memory::CallClassFn<std::uint64_t, 21>(this, pMemory);
	}
};

#pragma warning(pop)