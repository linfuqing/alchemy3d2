#include "AlchemyFlashStdafx.h"
#include "AlchemyFlashSkyPlane.h"

using namespace alchemy;

CFlashSkyPlane::CFlashSkyPlane(void)
{
}

CFlashSkyPlane::~CFlashSkyPlane(void)
{
}

bool CFlashSkyPlane::AddChild(CSceneNode& Child)
{
	return CSceneNode::AddChild(Child) && CFlashAbstractSceneNode::AddChild(Child);
}

bool CFlashSkyPlane::RemoveChild(CSceneNode& Child)
{
	return CSceneNode::RemoveChild(Child) && CFlashAbstractSceneNode::RemoveChild(Child);
}

#ifdef AS3_H
AS3TYPE CFlashSkyPlane::Init()
{
	return AS3_Array("PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType",
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
		&m_bIsTransformDirty,
		&m_UVOffsetSpeed.x,
		&m_UVOffsetSpeed.y,
		&m_Emissive);
}
#endif