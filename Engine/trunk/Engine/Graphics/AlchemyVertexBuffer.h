#pragma once
//#include "alchemygraphicsbuffer.h"
#include "AlchemyGraphicsResource.h"

namespace alchemy
{
	class CRenderSystem;
	class CVertexBuffer :
		public CGraphicsResource
	{
		friend class CRenderSystem;
	public:
		CVertexBuffer(void);
		~CVertexBuffer(void);
	};
}
