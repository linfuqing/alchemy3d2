#include "AlchemyFlashStdafx.h"
#include "AlchemyFlashGraphicsResourcePool.h"

using namespace alchemy;

CFlashGraphicsResourcePool::CFlashGraphicsResourcePool(void)
{
}

CFlashGraphicsResourcePool::~CFlashGraphicsResourcePool(void)
{
}

void CFlashGraphicsResourcePool::CallBackToCreate(const void* pObject, const void* pInterface, CGraphicsResource::TYPE Type, CGraphicsResource::HANDLE Handle)
{
#ifdef AS3_H
	int nType = Type, nHandle = Handle;
	AS3_CallT(m_CreateResourceMethod, m_Object, "IntType, IntType, IntType, IntType", pObject, pInterface, nType, nHandle);
#endif
}