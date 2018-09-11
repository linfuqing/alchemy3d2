#include "AlchemyFlashStdafx.h"
#include "AlchemyFlashCustomSceneNode.h"

using namespace alchemy;

CFlashCustomSceneNode::CFlashCustomSceneNode(void)
{

}

CFlashCustomSceneNode::~CFlashCustomSceneNode(void)
{

}

#ifdef AS3_H
void CFlashCustomSceneNode::Fill(AS3TYPE Object, AS3TYPE MethodArray)
{
	m_Object = Object;

	AS3_ArrayValue(MethodArray, "AS3ValType, AS3ValType", &m_ApplyRenderMethod, &m_RenderMethod);
}
#endif