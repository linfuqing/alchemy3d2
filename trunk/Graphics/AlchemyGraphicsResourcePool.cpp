#include "AlchemyGraphicsResourcePool.h"
#include "../Core/AlchemyDebug.h"

using namespace alchemy;

bool CGraphicsResourcePool::sm_bIsResourceCanBeCreated = false;

CGraphicsResourcePool::CGraphicsResourcePool(CGraphicsResource::TYPE Begin, CGraphicsResource::TYPE End) :
m_uBegin(Begin + 1),
m_uTotal(End - m_uBegin)
{
	ALCHEMY_DEBUG_NEW(m_pPool, CPool<CGraphicsResource*>[m_uTotal]);
}

CGraphicsResourcePool::~CGraphicsResourcePool(void)
{
	if(m_pPool)
	{
		CPool<CGraphicsResource*>::CIterator Iterator;

		for(UINT i = 0; i < m_uTotal; i ++)
		{
			Iterator = m_pPool[i].GetBeginIterator();

			while( CPool<CGraphicsResource*>::IsValuable(Iterator) )
			{
				(*Iterator)->__Destroy();

				++ Iterator;
			}
		}

		ALCHEMY_DEBUG_DELETE_ARRAY(m_pPool);
	}
}

CGraphicsResource* CGraphicsResourcePool::CreateResource(CGraphicsResource::TYPE Type)
{
	if( !IsValuableType(Type) )
	{
		ALCHEMY_DEBUG_WARNING("CGraphicsResourcePool::CreateResource: Invalid Type.");

		return ALCHEMY_NULL;
	}

	if(m_pPool[Type - m_uBegin].GetLength() >= 0xffff)
	{
		ALCHEMY_DEBUG_WARNING("CGraphicsResourcePool::CreateResource: Resource pool has full.");

		return ALCHEMY_NULL;
	}

	sm_bIsResourceCanBeCreated = true;
	CGraphicsResource* pOutput = _Create(Type);
	sm_bIsResourceCanBeCreated = false;
	if(pOutput)
	{
		pOutput->m_pGraphicsResourcePool = this;
		pOutput->m_Handle                = _AddResource(*pOutput, Type);
		pOutput->m_Type                  = Type;

		pOutput->AddRef();

		return pOutput;
	}

	return ALCHEMY_NULL;
}

bool CGraphicsResourcePool::DestroyResource(CGraphicsResource& Resource)
{
	CGraphicsResource* pResource = GetResource(Resource.m_Handle, Resource.m_Type);

	if(pResource && pResource == &Resource)
	{
		CGraphicsResource::HANDLE Handle = Resource.m_Handle;
		CGraphicsResource::TYPE   Type   = Resource.m_Type;

		if( pResource->__Destroy() )
			return _RemoveResource(Handle, Type);
	}

	return false;
}

void CGraphicsResourcePool::Destroy()
{
	if(m_pPool)
	{
		CPool<CGraphicsResource*>::CIterator Iterator;

		for(UINT i = 0; i < m_uTotal; i ++)
		{
			Iterator = m_pPool[i].GetBeginIterator();

			while( CPool<CGraphicsResource*>::IsValuable(Iterator) )
			{
				//(*Iterator)->Destroy();

				//ALCHEMY_DEBUG_DELETE(*Iterator);

				(*Iterator)->__Destroy();

				++ Iterator;
			}
			//m_pPool[i].Clear();
		}

		ALCHEMY_DEBUG_DELETE_ARRAY(m_pPool);
	}
}

CGraphicsResource::HANDLE CGraphicsResourcePool::_AddResource(CGraphicsResource& Resource        , CGraphicsResource::TYPE Type)
{
	if( !IsValuableType(Type) )
	{
		ALCHEMY_DEBUG_WARNING("CGraphicsResourcePool::_AddResource: Invalid Type.");

		return 0;
	}

	return m_pPool[Type - m_uBegin].Add(&Resource) + 1;
}

bool                      CGraphicsResourcePool::_RemoveResource(CGraphicsResource::HANDLE Handle, CGraphicsResource::TYPE Type)
{
	if( !IsValuableType(Type) )
	{
		ALCHEMY_DEBUG_WARNING("CGraphicsResourcePool::_RemoveResource: Invalid Type.");

		return false;
	}

	if(Handle)
		return m_pPool[Type - m_uBegin].Remove(Handle - 1, 1);

	return false;
}