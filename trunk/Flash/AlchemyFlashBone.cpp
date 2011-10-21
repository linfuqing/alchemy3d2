#include "AlchemyFlashStdafx.h"
#include "AlchemyFlashBone.h"

using namespace alchemy;

CFlashBone::CFlashBone(void)
{

}

CFlashBone::~CFlashBone(void)
{

}

bool CFlashBone::AddChild(CSceneNode& Child)
{
	return CBone::AddChild(Child) && CFlashAbstractSceneNode::AddChild(Child);
}

bool CFlashBone::RemoveChild(CSceneNode& Child)
{
	return CBone::RemoveChild(Child) && CFlashAbstractSceneNode::RemoveChild(Child);
}

#ifdef AS3_H
AS3TYPE CFlashBone::Init()
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