#include "AlchemyGraphicsResource.h"

#include "AlchemyGraphicsResourcePool.h"

using namespace alchemy;

void* CGraphicsResource::sm_pPointer = ALCHEMY_NULL;

CGraphicsResource::CGraphicsResource(void) :
m_pPointer(sm_pPointer),
m_Handle(0),
m_nForceReferenceCount(0)
{
	ALCHEMY_DEBUG_ASSERT(CGraphicsResourcePool::sm_bIsResourceCanBeCreated, "Resource must be created by resource pool.");
}

CGraphicsResource::~CGraphicsResource(void)
{
	//m_pGraphicsResourcePool->DestroyResource(*this);
}

INT CGraphicsResource::AddRef()
{
	ALCHEMY_DEBUG_ASSERT(CReferenceCounter::GetReferenceCount() < 0xffff, "Reference count out of range.");

	return CReferenceCounter::AddRef();
}

INT CGraphicsResource::Release()
{
	INT nReferenceCount = CReferenceCounter::Release();
	if(nReferenceCount <= 0)
		m_pGraphicsResourcePool->DestroyResource(*this);

	return nReferenceCount;
}

void CGraphicsResource::Destroy()
{
}

void* CGraphicsResource::operator new(UINT uSize)
{
	sm_pPointer = ::operator new(uSize);

	return sm_pPointer;
}

bool CGraphicsResource::__Destroy()
{
	if(m_nForceReferenceCount > 0)
	{
		m_nReferenceCount = m_nForceReferenceCount;

		return false;
	}

	Destroy();

	::operator delete(m_pPointer);

	return true;
}