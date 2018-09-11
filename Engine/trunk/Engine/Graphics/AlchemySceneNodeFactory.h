#pragma once
#include "alchemyscenenodefactoryinterface.h"

#include "../Core/AlchemySmartClass.h"

namespace alchemy
{
	class CSceneNodeFactory :
		public CSmartClass<ISceneNodeFactory>
	{
	public:
		CSceneNodeFactory(void);
		~CSceneNodeFactory(void);

		CSceneNode  &		CreateSceneNode(  void);
		CSceneObject&		CreateSceneObject(void);
		CCamera     &		CreateCamera(     void);
		CBone       &		CreateBone(       void);
		CSceneNodeUpdater&	CreateSceneNodeUpdater();

		bool DestroySceneNode(CSceneNode& SceneNode);

		void Destroy();
	protected:
		CPool<CSceneNode*> m_SceneNodePool;
	};
}