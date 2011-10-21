#pragma once
#include "alchemyscenenodefactoryinterface.h"

namespace alchemy
{
	class CSceneNodeFactory :
		public CSceneNodeFactoryInterface
	{
	public:
		CSceneNodeFactory(void);
		~CSceneNodeFactory(void);

		CSceneNode  & CreateSceneNode(  void);
		CSceneObject& CreateSceneObject(void);
		CCamera     & CreateCamera(     void);
		CBone       & CreateBone(       void);

		bool DestroySceneNode(CSceneNode& SceneNode);

		void Destroy();
	protected:
		CPool<CSceneNode*> m_SceneNodePool;
	};
}