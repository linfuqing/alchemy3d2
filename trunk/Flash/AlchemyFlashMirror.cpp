#include "AlchemyFlashStdafx.h"

#include "AlchemyFlashMirror.h"

using namespace alchemy;

CFlashMirror::CFlashMirror(void)
{
}

CFlashMirror::~CFlashMirror(void)
{
}

bool CFlashMirror::AddChild(CSceneNode& Child)
{
	return CSceneNode::AddChild(Child) && CFlashAbstractSceneNode::AddChild(Child);
}

bool CFlashMirror::RemoveChild(CSceneNode& Child)
{
	return CSceneNode::RemoveChild(Child) && CFlashAbstractSceneNode::RemoveChild(Child);
}

#ifdef AS3_H
AS3TYPE CFlashMirror::Init()
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