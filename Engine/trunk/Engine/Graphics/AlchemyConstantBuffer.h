/*
 *  AlchemyConstantBuffer.h
 *  IOS
 *
 *  Created by lin fuqing on 12-7-10.
 *  Copyright 2012 AstepGame. All rights reserved.
 *
 */
#include "AlchemyGraphicsResource.h"

namespace alchemy
{
	class CRenderSystem;
	class CConstantBuffer : 
		public CGraphicsResource
	{
		friend class CRenderSystem;
	public:
		typedef struct
		{
			UINT uStride;
			UINT uOffset;
		}CONSTANTELEMENT;
		
		CConstantBuffer();
		~CConstantBuffer();
	};
}