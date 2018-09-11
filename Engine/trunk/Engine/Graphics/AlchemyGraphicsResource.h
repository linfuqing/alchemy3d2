#pragma once

#include "../Core/AlchemyResource.h"

namespace alchemy
{
	class CGraphicsResourcePool;
	//class CHighLevelGraphicsResource;
	class CGraphicsResource :
		public CResource
	{
		friend class CGraphicsResourcePool;
		//friend class CHighLevelGraphicsResource;
	public:
		typedef UINT16 HANDLE;

		typedef enum
		{
			BEGIN_RENDER_RESOURCE = 0,

			VERTEX_DECLARATION,
			VERTEX_BUFFER,
			INDEX_BUFFER,
			CONSTANT_BUFFER,
			SHADER,
			EFFECT,
			TEXTURE,

			END_RENDER_RESOURCE,

			BEGIN_SCENE_RESOURCE,

			VERTEX,
			MESH,
			MATERIAL,
			LIGHT,
			RENDER_METHOD,

			END_SCENE_RESOURCE
		}TYPE;

		CGraphicsResource(void);
		~CGraphicsResource(void);

		HANDLE GetResourceHandle()const;
		TYPE GetType()const;
		CGraphicsResourcePool& GetGraphicsResourcePool();

		virtual void Destroy();

	protected:
		static void _AddRef(CGraphicsResource& Resource);
		static void _Release(CGraphicsResource& Resource);

		virtual bool _Create() = 0;

		void _Destroy();

	private:
		bool __Destroy();

		UINT m_uForceReferenceCount;

		HANDLE m_Handle;
		TYPE m_Type;
		CGraphicsResourcePool* m_pGraphicsResourcePool;
	};

	ALCHEMY_INLINE CGraphicsResource::HANDLE CGraphicsResource::GetResourceHandle() const
	{
		return m_Handle;
	}

	ALCHEMY_INLINE CGraphicsResource::TYPE CGraphicsResource::GetType() const
	{
		return m_Type;
	}

	ALCHEMY_INLINE CGraphicsResourcePool& CGraphicsResource::GetGraphicsResourcePool()
	{
		return *m_pGraphicsResourcePool;
	}

	ALCHEMY_INLINE void CGraphicsResource::_AddRef(CGraphicsResource& Resource)
	{
		++ Resource.m_uForceReferenceCount;

		//Resource.AddRef();
	}

	ALCHEMY_INLINE void CGraphicsResource::_Release(CGraphicsResource& Resource)
	{
		-- Resource.m_uForceReferenceCount;

		Resource.Release();
	}
}