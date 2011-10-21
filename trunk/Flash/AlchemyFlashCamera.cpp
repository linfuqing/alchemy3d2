#include "AlchemyFlashStdafx.h"
#include "AlchemyFlashCamera.h"

using namespace alchemy;

CFlashCamera::CFlashCamera(void)
{
}

CFlashCamera::~CFlashCamera(void)
{
}

bool CFlashCamera::AddChild(CSceneNode& Child)
{
	return CCamera::AddChild(Child) && CFlashAbstractSceneNode::AddChild(Child);
}

bool CFlashCamera::RemoveChild(CSceneNode& Child)
{
	return CCamera::RemoveChild(Child) && CFlashAbstractSceneNode::RemoveChild(Child);
}

#ifdef AS3_H
AS3TYPE CFlashCamera::Init()
{
	return AS3_Array("PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType",
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
		&m_ViewMatrix,
		&m_ProjectionMatrix
		);
}
#endif