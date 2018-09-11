#include "AlchemyGraphicsResource.h"

#include "AlchemyGraphicsResourcePool.h"

using namespace alchemy;

CGraphicsResource::CGraphicsResource(void) :
m_Handle(0),
m_uForceReferenceCount(0)
{
	ALCHEMY_DEBUG_ASSERT(CGraphicsResourcePool::sm_bIsResourceCanBeCreated, "Resource must be created by resource pool.");
}

CGraphicsResource::~CGraphicsResource(void)
{
	//m_pGraphicsResourcePool->DestroyResource(*this);
}

void CGraphicsResource::Destroy()
{
}

void CGraphicsResource::_Destroy()
{
	m_pGraphicsResourcePool->DestroyResource(*this);
}

bool CGraphicsResource::__Destroy()
{
	if(m_uForceReferenceCount > 0)
	{
		m_uReferenceCount = m_uForceReferenceCount;

		return false;
	}

	Destroy();

	CResource::_Destroy();

	return true;
}