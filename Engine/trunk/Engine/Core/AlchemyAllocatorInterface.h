#pragma once
/*
 *  AlchemyAllocatorInterface.h
 *  IOS
 *
 *  Created by lin fuqing on 12-5-31.
 *  Copyright 2012 AstepGame. All rights reserved.
 *
 */

#include "AlchemyInterface.h"

namespace alchemy
{
	ALCHEMY_INTERFACE(IAllocator)
	{
		virtual void* Allocate()                = 0;
		virtual void  Dellocate(void* pPointer) = 0;
	};
}