#include "AlchemyFlashStdafx.h"
#include "AlchemyFlashSceneNodeFactory.h"

#include "AlchemyFlashSceneNode.h"
#include "AlchemyFlashCamera.h"
#include "AlchemyFlashSceneObject.h"
#include "AlchemyFlashBone.h"

#include "AlchemyFlashSkyPlane.h"

using namespace alchemy;

CFlashSceneNodeFactory::CFlashSceneNodeFactory(void) :
CSingleton<CFlashSceneNodeFactory>(*this)
{
}

CFlashSceneNodeFactory::~CFlashSceneNodeFactory(void)
{
}

CSceneNode  & CFlashSceneNodeFactory::CreateSceneNode(  void)
{
	return static_cast<CSceneNode&>( CreateSceneNode(SCENE_NODE) );
}

CCamera     & CFlashSceneNodeFactory::CreateCamera(     void)
{
	return static_cast<CCamera&>( CreateSceneNode(CAMERA) );
}

CSceneObject& CFlashSceneNodeFactory::CreateSceneObject(void)
{
	return static_cast<CSceneObject&>( CreateSceneNode(SCENE_OBJECT) );
}

CBone	    & CFlashSceneNodeFactory::CreateBone(		void)
{
	return static_cast<CBone&>( CreateSceneNode(BONE) );
}

CSceneNode& CFlashSceneNodeFactory::CreateSceneNode(CFlashSceneNodeFactory::SCENENODETYPE Type)
{
	CSceneNode* pSceneNode;
	switch(Type)
	{
	case CAMERA:
		ALCHEMY_DEBUG_NEW(pSceneNode, CFlashCamera);
#ifdef AS3_H
		CFlashCamera* pFlashCamera = static_cast<CFlashCamera*>(pSceneNode);
		AS3_CallT(m_CreateSceneNodeMethod, m_Object, "PtrType, PtrType, PtrType, IntType", pFlashCamera, static_cast<CFlashObjectInterface*>(pFlashCamera), static_cast<CSceneNode*>(pFlashCamera), Type);
#endif
		break;
	case SCENE_OBJECT:
		ALCHEMY_DEBUG_NEW(pSceneNode, CFlashSceneObject);
#ifdef AS3_H
		CFlashSceneObject* pFlashSceneObject = static_cast<CFlashSceneObject*>(pSceneNode);
		AS3_CallT(m_CreateSceneNodeMethod, m_Object, "PtrType, PtrType, PtrType, IntType", pFlashSceneObject, static_cast<CFlashObjectInterface*>(pFlashSceneObject), static_cast<CSceneNode*>(pFlashSceneObject), Type);
#endif
		break;
	case BONE:
		ALCHEMY_DEBUG_NEW(pSceneNode, CFlashBone);
#ifdef AS3_H
		CFlashBone* pFlashBone = static_cast<CFlashBone*>(pSceneNode);
		AS3_CallT(m_CreateSceneNodeMethod, m_Object, "PtrType, PtrType, PtrType, IntType", pFlashBone, static_cast<CFlashObjectInterface*>(pFlashBone), static_cast<CSceneNode*>(pFlashBone), Type);
#endif
		break;
	default:
		ALCHEMY_DEBUG_NEW(pSceneNode, CFlashSceneNode);
#ifdef AS3_H
		CFlashSceneNode* pFlashSceneNode = static_cast<CFlashSceneNode*>(pSceneNode);
		AS3_CallT(m_CreateSceneNodeMethod, m_Object, "PtrType, PtrType, PtrType, IntType", pFlashSceneNode, static_cast<CFlashObjectInterface*>(pFlashSceneNode), static_cast<CSceneNode*>(pFlashSceneNode), Type);
#endif
		break;
	}

	CSceneNodeFactory::m_SceneNodePool.Add(pSceneNode);

	return *pSceneNode;
}

bool CFlashSceneNodeFactory::DestroySceneNode(CSceneNode& SceneNode)
{
	UINT uIndex;

	if( m_SceneNodePool.GetIndexOf< BASICEQUAL<CSceneNode*> >(uIndex, &SceneNode) && m_SceneNodePool.Remove(uIndex, 1) )
	{
#ifdef AS3_H
		AS3_CallT(m_DestroySceneNodeMethod, m_Object, "IntType", uIndex);
#endif

		delete &SceneNode;

		return true;
	}

	ALCHEMY_DEBUG_WARNING("Can not fine the scene node.");

	return false;
}

#ifdef AS3_H
AS3TYPE CFlashSceneNodeFactory::Init()
{
	return 0;
}

void CFlashSceneNodeFactory::Fill(AS3TYPE Object, AS3TYPE MethodArray)
{
	m_Object = Object;
	AS3_ArrayValue(MethodArray, "AS3ValType, AS3ValType", &m_CreateSceneNodeMethod, &m_DestroySceneNodeMethod);
}
#endif