#pragma once

#include "AlchemyIndexBuffer.h"
#include "AlchemyVertexBuffer.h"
#include "AlchemyConstantBuffer.h"
#include "AlchemyTexture.h"

namespace alchemy
{
	class CRenderSystem;
	template<typename T>
	class CGraphicsBuffer :
		public T
	{
		friend class CRenderSystem;
	public:
		typedef enum
		{
			WRITE_ONLY = 0,
			DYNAMIC,
			TOTAL_USAGE
		}USAGE;

		typedef enum
		{
			READ_ONLY = 0,
			NO_OVER_WRITE
		}LOCKFLAG;
		
		class CLocker :
			public T
		{
		public:
			CLocker() {}
			~CLocker() {}
			
			virtual void* Lock(UINT uStart, UINT uEnd, UINT uFlag) = 0;
			virtual void* Lock(                        UINT uFlag) = 0;
			virtual bool  Unlock(                                ) = 0;
		};
		
		CGraphicsBuffer(void) : m_uMemberCount(0), m_uUsage(0), m_uStride(0), m_uByteSize(0) {};
		~CGraphicsBuffer(void) {};

		ALCHEMY_INLINE UINT GetMemberCount()const
		{
			return m_uMemberCount;
		}

		ALCHEMY_INLINE UINT GetUsage()const
		{
			return m_uUsage;
		}

		ALCHEMY_INLINE UINT GetStride()const
		{
			return m_uStride;
		}

		ALCHEMY_INLINE UINT GetByteSize()const
		{
			return m_uByteSize;
		}

	private:
		UINT m_uMemberCount;
		UINT m_uUsage;
		UINT m_uStride;
		UINT m_uByteSize;
	};

	typedef CGraphicsBuffer<CGraphicsBuffer<CVertexBuffer>::CLocker>   VERTEXBUFFER  , * LPVERTEXBUFFER  ;
	typedef CGraphicsBuffer<CGraphicsBuffer<CIndexBuffer>::CLocker>    INDEXBUFFER   , * LPINDEXBUFFER   ;
	typedef CGraphicsBuffer<CGraphicsBuffer<CConstantBuffer>::CLocker> CONSTANTBUFFER, * LPCONSTANTBUFFER;
	typedef CGraphicsBuffer<CTexture>                                  TEXTURE       , * LPTEXTURE       ;
}
