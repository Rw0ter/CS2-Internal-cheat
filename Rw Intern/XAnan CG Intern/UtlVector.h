#pragma once
#include "UtlMemory.h"

// used: memorymove

// @source: master/public/tier1/utlvector.h

/*
 * a growable array class which doubles in size by default.
 * it will always keep all elements consecutive in memory, and may move the
 * elements around in memory (via a realloc) when elements are inserted or removed.
 * clients should therefore refer to the elements of the vector by index and not pointers
 *
 * @note: if variable that uses it intend to call any method that needs to allocate/deallocate should have overloaded constructor/destructor and/or new/delete operators respectively
 */
template <class T, class A = CUtlMemory<T>>
class CUtlVector
{
	using CAllocator = A;

public:
	explicit CUtlVector(const int nGrowSize = 0, const int nInitialCapacity = 0) :
		memory(nGrowSize, nInitialCapacity), nSize(0), pElements(memory.Base()) { }

	CUtlVector(T* pMemory, const int nInitialCapacity, const int nInitialCount = 0) :
		memory(pMemory, nInitialCapacity), nSize(nInitialCount), pElements(memory.Base()) { }

	CUtlVector(const CUtlVector&) = delete;

	CUtlVector& operator=(const CUtlVector& vecOther)
	{

		const int nSourceCount = vecOther.Count();
		SetCount(nSourceCount);

		for (int i = 0; i < nSourceCount; i++)
			(*this)[i] = vecOther[i];

		return *this;
	}

	[[nodiscard]] T& operator[](const int nIndex)
	{
		return memory[nIndex];
	}

	[[nodiscard]] const T& operator[](const int nIndex) const
	{
		return memory[nIndex];
	}

	[[nodiscard]] T& Element(const int nIndex)
	{
		return memory[nIndex];
	}

	[[nodiscard]] T* ElementPointer(const int nIndex)
	{
		return &memory[nIndex];
	}

	[[nodiscard]] const T& Element(const int nIndex) const
	{
		return memory[nIndex];
	}

	[[nodiscard]] const T* ElementPointer(const int nIndex) const
	{
		return &memory[nIndex];
	}

	[[nodiscard]] T* Base()
	{
		return memory.Base();
	}

	[[nodiscard]] const T* Base() const
	{
		return memory.Base();
	}

	[[nodiscard]] int Count() const
	{
		return nSize;
	}

	[[nodiscard]] int& Size()
	{
		return nSize;
	}

	[[nodiscard]] bool IsValidIndex(const int nIndex) const
	{
		return (nIndex >= 0) && (nIndex < nSize);
	}

	void GrowVector(const int nCount = 1)
	{
		if (nSize + nCount > memory.AllocationCount())
			memory.Grow(nSize + nCount - memory.AllocationCount());

		nSize += nCount;
		pElements = memory.Base();
	}

	[[nodiscard]] int Find(const T& source) const
	{
		for (int i = 0; i < nSize; ++i)
		{
			if (Element(i) == source)
				return i;
		}

		return -1;
	}

	[[nodiscard]] auto begin() noexcept
	{
		return memory.Base();
	}

	[[nodiscard]] auto end() noexcept
	{
		return memory.Base() + nSize;
	}

	[[nodiscard]] auto begin() const noexcept
	{
		return memory.Base();
	}

	[[nodiscard]] auto end() const noexcept
	{
		return memory.Base() + nSize;
	}

protected:
	int nSize;
	CAllocator memory;
	T* pElements;
};

template <typename T>
class C_NetworkUtlVectorBase
{
public:
	std::uint32_t nSize;
	T* pElements;
};