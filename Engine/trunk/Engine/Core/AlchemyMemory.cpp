#include "AlchemyMemory.h"
#include "AlchemyDebug.h"

using namespace alchemy;

void alchemy::MemoryCopy(void* pMemory, const void* pSource, UINT uSize)
{
	if(uSize & 0x1)
		MemoryCopy(reinterpret_cast<PINT8>(pMemory), reinterpret_cast<const INT8*>(pSource), uSize);
	else if(uSize & 0x2)
		MemoryCopy(reinterpret_cast<PINT16>(pMemory), reinterpret_cast<const INT16*>(pSource), uSize);
	else if(uSize & 0x4)
		MemoryCopy(reinterpret_cast<PINT32>(pMemory), reinterpret_cast<const INT32*>(pSource), uSize);
	else if(uSize & 0x8)
		MemoryCopy(reinterpret_cast<PINT64>(pMemory), reinterpret_cast<const INT64*>(pSource), uSize);
	else
		MemoryCopy(reinterpret_cast<PINT>(pMemory), reinterpret_cast<const INT*>(pSource), uSize);
}

void alchemy::MemorySet(void* pMemory, INT nVal, UINT uSize)
{
	if(uSize & 0x1)
		MemorySet(reinterpret_cast<PINT8>(pMemory), (INT8)nVal, uSize);
	else if(uSize & 0x2)
		MemorySet(reinterpret_cast<PINT16>(pMemory), (INT16)nVal, uSize);
	else if(uSize & 0x4)
		MemorySet(reinterpret_cast<PINT32>(pMemory), (INT32)nVal, uSize);
	else if(uSize & 0x8)
		MemorySet(reinterpret_cast<PINT64>(pMemory), (INT64)nVal, uSize);
	else
		MemorySet(reinterpret_cast<PINT>(pMemory), nVal, uSize);
}

bool alchemy::MemoryCompare(const void* pMemory, const void* pSource, UINT uSize)
{
	if(uSize & 0x1)
		return MemoryCompare(reinterpret_cast<const INT8*>(pMemory), reinterpret_cast<const INT8*>(pSource), uSize);
	else if(uSize & 0x2)
		return MemoryCompare(reinterpret_cast<const INT16*>(pMemory), reinterpret_cast<const INT16*>(pSource), uSize);
	else if(uSize & 0x4)
		return MemoryCompare(reinterpret_cast<const INT32*>(pMemory), reinterpret_cast<const INT32*>(pSource), uSize);
	else if(uSize & 0x8)
		return MemoryCompare(reinterpret_cast<const INT64*>(pMemory), reinterpret_cast<const INT64*>(pSource), uSize);
	else
		return MemoryCompare(reinterpret_cast<const INT*>(pMemory), reinterpret_cast<const INT*>(pSource), uSize);
}