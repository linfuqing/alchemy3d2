#pragma once

#include "AlchemyIndexBuffer.h"
#include "AlchemyVertexBuffer.h"
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
			DISCARD,
			NO_OVER_WRITE
		}LOCKFLAG;

		CGraphicsBuffer() {};
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

	typedef CGraphicsBuffer<CVertexBuffer> VERTEXBUFFER, * LPVERTEXBUFFER;
	typedef CGraphicsBuffer<CIndexBuffer>  INDEXBUFFER , * LPINDEXBUFFER ;
	typedef CGraphicsBuffer<CTexture>      TEXTURE     , * LPTEXTURE     ;
}
