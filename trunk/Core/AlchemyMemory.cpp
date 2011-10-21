#include "AlchemyMemory.h"
#include "AlchemyDebug.h"

using namespace alchemy;

CMemory::CMemory(void)
{
}

CMemory::~CMemory(void)
{
}

bool CMemory::Copy(void* pMemory, const void* pSource, UINT uSize)
{
	if(!pMemory)
	{
		ALCHEMY_DEBUG_WARNING("No memory.");

		return false;
	}

	if(!pSource)
	{
		ALCHEMY_DEBUG_WARNING("No source.");

		return false;
	}
		
	PUINT8       puMemory = static_cast<PUINT8>(pMemory);
	
	const UINT8* puSource = static_cast<const UINT8*>(pSource);

	//uSize >>= 1;

	for(UINT i = 0; i < uSize; ++ i)
		puMemory[i] = puSource[i];

	return true;
}