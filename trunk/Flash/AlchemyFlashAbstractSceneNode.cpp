#include "AlchemyFlashStdafx.h"
#include "AlchemyFlashAbstractSceneNode.h"

using namespace alchemy;

CFlashAbstractSceneNode::CFlashAbstractSceneNode(void)
{
}

CFlashAbstractSceneNode::~CFlashAbstractSceneNode(void)
{
}

bool CFlashAbstractSceneNode::AddChild(CSceneNode& Child)
{
#ifdef AS3_H
	CSceneNodeInterface& Interface = static_cast<CSceneNodeInterface&>(Child);
	CFlashAbstractSceneNode* pSceneNode = dynamic_cast<CFlashAbstractSceneNode*>(&Interface);
	if(pSceneNode)
		AS3_CallT(m_AddSceneNodeMethod, m_Object, "AS3ValType", pSceneNode->m_Object);
#endif

	return true;
}

bool CFlashAbstractSceneNode::RemoveChild(CSceneNode& Child)
{
#ifdef AS3_H
	CSceneNodeInterface& Interface = static_cast<CSceneNodeInterface&>(Child);
	CFlashAbstractSceneNode* pSceneNode = dynamic_cast<CFlashAbstractSceneNode*>(&Interface);
	if(pSceneNode)
		AS3_CallT(m_RemoveSceneNodeMethod, m_Object, "AS3ValType", pSceneNode->m_Object);
#endif

	return true;
}

#ifdef AS3_H
void CFlashAbstractSceneNode::Fill(AS3TYPE Object, AS3TYPE MehtodArray)
{
	m_Object = Object;

	AS3_ArrayValue(MehtodArray, "AS3ValType, AS3ValType", &m_AddSceneNodeMethod, &m_RemoveSceneNodeMethod);
}
#endif