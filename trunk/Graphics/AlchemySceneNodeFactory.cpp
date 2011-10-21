#include "AlchemySceneNodeFactory.h"

using namespace alchemy;

CSceneNodeFactory::CSceneNodeFactory(void)
{
}

CSceneNodeFactory::~CSceneNodeFactory(void)
{
	Destroy();
}

CSceneNode  & CSceneNodeFactory::CreateSceneNode(  void)
{
	CSceneNode* pSceneNode;

	ALCHEMY_DEBUG_NEW(pSceneNode, CSceneNode);

	m_SceneNodePool.Add(pSceneNode);

	return *pSceneNode;
}

CSceneObject& CSceneNodeFactory::CreateSceneObject(void)
{
	CSceneObject* pSceneObject;

	ALCHEMY_DEBUG_NEW(pSceneObject, CSceneObject);

	m_SceneNodePool.Add(pSceneObject);

	return *pSceneObject;
}

CCamera     & CSceneNodeFactory::CreateCamera(     void)
{
	CCamera* pCamera;

	ALCHEMY_DEBUG_NEW(pCamera, CCamera);

	m_SceneNodePool.Add(pCamera);

	return *pCamera;
}

CBone       & CSceneNodeFactory::CreateBone(       void)
{
	CBone* pBone;

	ALCHEMY_DEBUG_NEW(pBone, CBone);

	m_SceneNodePool.Add(pBone);

	return *pBone;
}

bool CSceneNodeFactory::DestroySceneNode(CSceneNode& SceneNode)
{
	UINT uIndex;

	if( m_SceneNodePool.GetIndexOf< BASICEQUAL<CSceneNode*> >(uIndex, &SceneNode) && m_SceneNodePool.Remove(uIndex, 1) )
	{
		delete &SceneNode;

		return true;
	}

	ALCHEMY_DEBUG_WARNING("Can not fine the scene node.");

	return false;
}

void CSceneNodeFactory::Destroy()
{
	CPool<CSceneNode*>::CIterator Iterator = m_SceneNodePool.GetBeginIterator();

	while( CPool<CSceneNode*>::IsValuable(Iterator) )
	{
		//(*Iterator)->Destroy();

		ALCHEMY_DEBUG_DELETE(*Iterator);

		++ Iterator;
	}

	m_SceneNodePool.Clear();
}