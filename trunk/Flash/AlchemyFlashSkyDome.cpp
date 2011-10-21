#include "AlchemyFlashStdafx.h"
#include "AlchemyFlashSkyDome.h"

using namespace alchemy;

CFlashSkyDome::CFlashSkyDome(void)
{

}

CFlashSkyDome::~CFlashSkyDome(void)
{

}

bool CFlashSkyDome::AddChild(CSceneNode& Child)
{
	return CSceneNode::AddChild(Child) && CFlashAbstractSceneNode::AddChild(Child);
}

bool CFlashSkyDome::RemoveChild(CSceneNode& Child)
{
	return CSceneNode::RemoveChild(Child) && CFlashAbstractSceneNode::RemoveChild(Child);
}

#ifdef AS3_H

AS3TYPE CFlashSkyDome::Init()
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
		&m_bIsTransformDirty);
}

#endif