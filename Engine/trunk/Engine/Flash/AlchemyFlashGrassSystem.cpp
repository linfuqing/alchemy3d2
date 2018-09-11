/*
 *  AlchemyFlashGrassSystem.cpp
 *  IOS
 *
 *  Created by lin fuqing on 12-7-9.
 *  Copyright 2012 AstepGame. All rights reserved.
 *
 */

#include "AlchemyFlashStdafx.h"

#include "AlchemyFlashGrassSystem.h"

using namespace alchemy;

CFlashGrassSystem::CFlashGrassSystem(void)
{
}

CFlashGrassSystem::~CFlashGrassSystem(void)
{
}

bool CFlashGrassSystem::AddChild(CSceneNode& Child)
{
	return CGrassSystem::AddChild(Child) && CFlashAbstractSceneNode::AddChild(Child);
}

bool CFlashGrassSystem::RemoveChild(CSceneNode& Child)
{
	return CGrassSystem::RemoveChild(Child) && CFlashAbstractSceneNode::RemoveChild(Child);
}

#ifdef AS3_H
AS3TYPE CFlashGrassSystem::Init()
{
	return AS3_Array("PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType",
					 &Translation.x, 
					 &Translation.y,
					 &Translation.z,
					 &Scale.x,
					 &Scale.y,
					 &Scale.z,
					 &Rotation.x, 
					 &Rotation.y, 
					 &Rotation.z, 
					 &m_LocalMatrix,
					 &m_WorldMatrix,
					 &m_uDirtyFlag,
					 &m_bIsTransformDirty
					 );
}
#endif