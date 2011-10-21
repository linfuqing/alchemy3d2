#pragma once
//#include "alchemygraphicsbuffer.h"
#include "AlchemyGraphicsResource.h"

namespace alchemy
{
	class CRenderSystem;
	class CVertexBuffer :
		public CGraphicsResource//, public CGraphicsBuffer
	{
		friend class CRenderSystem;
	public:
		CVertexBuffer(void);
		~CVertexBuffer(void);

		virtual PVOID Lock(UINT uStart, UINT uEnd, UINT uFlag) = 0;
		virtual PVOID Lock(                        UINT uFlag) = 0;
		virtual bool  Unlock(                                ) = 0;
	};
}
