#pragma once

#include "AlchemyDataType.h"

namespace alchemy
{
#ifndef ALCHEMY_MEMORY_COPY
#define ALCHEMY_MEMORY_COPY(Memory, Source, Size) CMemory::Copy(Memory, Source, Size)
#endif

#ifndef ALCHEMY_MEMORY_ZERO
#define ALCHEMY_MEMORY_ZERO(Memory, Size) CMemory::Zero(Memory, Size)
#endif

	class CMemory
	{
	public:
		static bool Copy(void* pMemory, const void* pSource, UINT uSize);

		CMemory(void);
		~CMemory(void);
	};
}