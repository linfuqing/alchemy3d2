#pragma once

#include "AlchemyHighLevelGraphicsResource.h"
#include "../Core/AlchemyPool.h"
#include "../Core/AlchemyEventDispatcher.h"

namespace alchemy
{
//#define ALCHEMY_GRAPHICS_RESOURCE_POOL CGraphicsResourcePool::GetInstance()

	class CGraphicsResourcePool :
		public CEventDispatcher<IEventDispatcher>
	{ 
		//friend class CHighLevelGraphicsResource;

		friend CGraphicsResource::CGraphicsResource(void);
	public:
		CGraphicsResourcePool(CGraphicsResource::TYPE Begin, CGraphicsResource::TYPE End);
		virtual ~CGraphicsResourcePool(void);

		bool IsValuableType(CGraphicsResource::TYPE Type);

		CGraphicsResource* GetResource(CGraphicsResource::HANDLE Handle, CGraphicsResource::TYPE Type);

		CGraphicsResource* CreateResource(CGraphicsResource::TYPE Type);
		bool               DestroyResource(CGraphicsResource& Resource);

		virtual void Destroy();

	protected:
		virtual CGraphicsResource::HANDLE _AddResource(CGraphicsResource& Resource,         CGraphicsResource::TYPE Type);
		virtual bool                      _RemoveResource(CGraphicsResource::HANDLE Handle, CGraphicsResource::TYPE Type);

		virtual CGraphicsResource* _Create(CGraphicsResource::TYPE Type) = 0;

	private:
		static bool sm_bIsResourceCanBeCreated;

		CPool<CGraphicsResource*>* m_pPool;
		UINT m_uBegin;
		UINT m_uTotal;
	};

	ALCHEMY_INLINE bool CGraphicsResourcePool::IsValuableType(CGraphicsResource::TYPE Type)
	{
		return ( static_cast<UINT>(Type) >= m_uBegin && static_cast<UINT>(Type) < (m_uTotal + m_uBegin) );
	}

	ALCHEMY_INLINE CGraphicsResource* CGraphicsResourcePool::GetResource(CGraphicsResource::HANDLE Handle, CGraphicsResource::TYPE Type)
	{
		if( !IsValuableType(Type) )
		{
			ALCHEMY_DEBUG_WARNING("CGraphicsResourcePool::GetResource: Invalid Type.");

			return ALCHEMY_NULL;
		}

		if(Handle)
		{
			CGraphicsResource** ppResource = m_pPool[Type - m_uBegin].Get(Handle - 1);

			if(ppResource)
				return *ppResource;
		}

		return ALCHEMY_NULL;
	}
}
