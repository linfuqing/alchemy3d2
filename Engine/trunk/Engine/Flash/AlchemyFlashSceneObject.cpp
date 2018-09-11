#include "AlchemyFlashStdafx.h"
#include "AlchemyFlashSceneObject.h"

using namespace alchemy;

CFlashSceneObject::CFlashSceneObject(void)
{
}

CFlashSceneObject::~CFlashSceneObject(void)
{
}

bool CFlashSceneObject::AddChild(CSceneNode& Child)
{
	return CSceneObject::AddChild(Child) && CFlashAbstractSceneNode::AddChild(Child);
}

bool CFlashSceneObject::RemoveChild(CSceneNode& Child)
{
	return CSceneObject::RemoveChild(Child) && CFlashAbstractSceneNode::RemoveChild(Child);
}

#ifdef AS3_H
AS3TYPE CFlashSceneObject::Init()
{
	return AS3_Array("PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType",
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
		&m_bIVisible
		);
}
#endif