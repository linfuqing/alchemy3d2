#pragma once
/*
 *  AlchemyGLESVertexDeclaration.h
 *  IOS
 *
 *  Created by lin fuqing on 12-5-1.
 *  Copyright 2012 AstepGame. All rights reserved.
 *
 */

#include "../Graphics/AlchemyVertexDeclaration.h"

namespace alchemy
{
	class CGLESContext;
	class CGLESVertexDeclaration :
	public CVertexDeclaration
	{
		friend class CGLESContext;
	public:
		CGLESVertexDeclaration();
		~CGLESVertexDeclaration();
		
	protected:
		bool _Create();
	};
}