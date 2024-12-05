#pragma once
#include "Tace Memory.hpp"

// @test: using interfaces in the header | not critical but could blow up someday with thousands of errors or affect to compilation time etc
// used: interface handles
//#include "../../core/interfaces.h"
// used: interface declarations
// @source: master/public/tier1/utlmemory.h

template <class T, class N = int>
class CUtlMemory
{
	enum
	{
		EXTERNAL_BUFFER_MARKER = -1,
		EXTERNAL_CONST_BUFFER_MARKER = -2,
	};

public:
	class Iterator_t
	{
	public:
		Iterator_t(const N nIndex) :
			nIndex(nIndex) { }

		bool operator==(const Iterator_t it) const
		{
			return nIndex == it.nIndex;
		}

		bool operator!=(const Iterator_t it) const
		{
			return nIndex != it.nIndex;
		}

		N nIndex;
	};

	CUtlMemory(T* pMemory, const int nElements) :
		pMemory(pMemory), nAllocationCount(nElements), nGrowSize(EXTERNAL_BUFFER_MARKER) { }

	CUtlMemory(const T* pMemory, const int nElements) :
		pMemory(pMemory), nAllocationCount(nElements), nGrowSize(EXTERNAL_CONST_BUFFER_MARKER) { }

	CUtlMemory(const CUtlMemory&) = delete;

	CUtlMemory(CUtlMemory&& moveFrom) noexcept :
		pMemory(moveFrom.pMemory), nAllocationCount(moveFrom.nAllocationCount), nGrowSize(moveFrom.nGrowSize)
	{
		moveFrom.pMemory = nullptr;
		moveFrom.nAllocationCount = 0;
		moveFrom.nGrowSize = 0;
	}

	CUtlMemory& operator=(const CUtlMemory&) = delete;
	[[nodiscard]] T& operator[](const N nIndex)
	{
		return pMemory[nIndex];
	}

	[[nodiscard]] const T& operator[](const N nIndex) const
	{
		return pMemory[nIndex];
	}

	[[nodiscard]] T& Element(const N nIndex)
	{
		return pMemory[nIndex];
	}

	[[nodiscard]] const T& Element(const N nIndex) const
	{
		return pMemory[nIndex];
	}

	[[nodiscard]] T* Base()
	{
		return pMemory;
	}

	[[nodiscard]] const T* Base() const
	{
		return pMemory;
	}

	[[nodiscard]] int AllocationCount() const
	{
		return nAllocationCount;
	}

	[[nodiscard]] bool IsExternallyAllocated() const
	{
		return nGrowSize <= EXTERNAL_BUFFER_MARKER;
	}

	[[nodiscard]] static N InvalidIndex()
	{
		return static_cast<N>(-1);
	}

	[[nodiscard]] bool IsValidIndex(N nIndex) const
	{
		return (nIndex >= 0) && (nIndex < nAllocationCount);
	}

	[[nodiscard]] Iterator_t First() const
	{
		return Iterator_t(IsValidIndex(0) ? 0 : InvalidIndex());
	}

	[[nodiscard]] Iterator_t Next(const Iterator_t& it) const
	{
		return Iterator_t(IsValidIndex(it.nIndex + 1) ? it.nIndex + 1 : InvalidIndex());
	}

	[[nodiscard]] N GetIndex(const Iterator_t& it) const
	{
		return it.nIndex;
	}

	[[nodiscard]] bool IsIndexAfter(N nIndex, const Iterator_t& it) const
	{
		return nIndex > it.nIndex;
	}

	[[nodiscard]] bool IsValidIterator(const Iterator_t& it) const
	{
		return IsValidIndex(it.index);
	}

	[[nodiscard]] Iterator_t InvalidIterator() const
	{
		return Iterator_t(InvalidIndex());
	}

	void* DetachMemory() {
		if (IsExternallyAllocated())
			return NULL;

		void* pMemory = pMemory;
		pMemory = 0;
		nAllocationCount = 0;
		return pMemory;
	}

	inline T* Detach() {
		return (T*)DetachMemory();
	}

	void Swap(CUtlMemory<T, N>& mem) {
		std::swap(nGrowSize, mem.nGrowSize);
		std::swap(pMemory, mem.pMemory);
		std::swap(nAllocationCount, mem.nAllocationCount);
	}

protected:
	T* pMemory; // 0x00
	int nAllocationCount; // 0x04
	int nGrowSize; // 0x08
};