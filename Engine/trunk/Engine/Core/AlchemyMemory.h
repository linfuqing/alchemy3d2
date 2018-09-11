#pragma once

#include "AlchemyDataType.h"

#ifdef _MSC_VER
#ifdef _DEBUG
#include <memory>
#endif
#endif

namespace alchemy
{
#ifndef ALCHEMY_MEMORY_COPY
#ifdef _MSC_VER
#define ALCHEMY_MEMORY_COPY(Memory, Source, Size) memcpy(Memory, Source, Size)
#else
#define ALCHEMY_MEMORY_COPY(Memory, Source, Size) MemoryCopy(Memory, Source, Size)
#endif
#endif

#ifndef ALCHEMY_MEMORY_SET
#ifdef _MSC_VER
#define ALCHEMY_MEMORY_SET(Memory, Value, Size) memset(Memory,Value,Size)
#else
#define ALCHEMY_MEMORY_SET(Memory, Value, Size) MemorySet(Memory, Value, Size)
#endif
#endif

#ifndef ALCHEMY_MEMORY_ZERO
#ifdef _MSC_VER
#define ALCHEMY_MEMORY_ZERO(Memory,  Size) memset(Memory,0,Size)
#else
#define ALCHEMY_MEMORY_ZERO(Memory, Size) MemorySet(Memory, 0, Size)
#endif
#endif

#ifndef ALCHEMY_MEMORY_COMPARE
#ifdef _MSC_VER
#define ALCHEMY_MEMORY_COMPARE(Memory, Source, Size) memcmp(Memory, Source, Size)
#else
#define ALCHEMY_MEMORY_COMPARE(Memory, Source, Size) MemoryCompare(Memory, Source, Size)
#endif
#endif
	
	template<typename T>
	void MemoryCopy(T* pMemory, const T* pSource, UINT uTotalBytes)
	{
		if(pMemory && pSource)
		{
			UINT uSize = uTotalBytes / sizeof(T);

			for(UINT i = 0; i < uSize; ++ i)
				pMemory[i] = pSource[i];
		}
	}

	template<typename T>
	void MemorySet(T* pMemory, T Value, UINT uTotalBytes)
	{
		if(pMemory)
		{
			UINT uSize = uTotalBytes / sizeof(T);

			for(UINT i = 0; i < uSize; ++ i)
				pMemory[i] = Value;
		}
	}
	
	template<typename T>
	bool MemoryCompare(const T* pMemory, const T* pSource, UINT uTotalBytes)
	{
		if(pMemory && pSource)
		{
			UINT uSize = uTotalBytes / sizeof(T);
			
			for(UINT i = 0; i < uSize; ++ i)
			{
				if(pMemory[i] != pSource[i])
					return false;
			}
			
			return true;
		}
		
		return false;
	}

	void MemoryCopy(void* pMemory, const void* pSource, UINT uSize);
	void MemorySet(void* pMemory, INT nVal,  UINT uSize);
	bool MemoryCompare(const void* pMemory, const void* pSource, UINT uSize);
}