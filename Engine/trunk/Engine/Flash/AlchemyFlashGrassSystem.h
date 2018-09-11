/*
 *  AlchemyFlashGrassSystem.h
 *  IOS
 *
 *  Created by lin fuqing on 12-7-9.
 *  Copyright 2012 AstepGame. All rights reserved.
 *
 */

#pragma once

#include "../Graphics/AlchemyGrassSystem.h"
#include "AlchemyFlashAbstractSceneNode.h"

namespace alchemy
{
	class CFlashGrassSystem :
	public CGrassSystem, public CFlashAbstractSceneNode
	{
	public:
		CFlashGrassSystem(void);
		~CFlashGrassSystem(void);
		
		bool AddChild(CSceneNode& Child);
		bool RemoveChild(CSceneNode& Child);
		
		ALCHEMY_IMPLEMENT_INTERFACE
		
#ifdef AS3_H
		AS3TYPE Init();
#endif
	};
}