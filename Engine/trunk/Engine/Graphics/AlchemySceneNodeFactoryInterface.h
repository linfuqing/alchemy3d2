#pragma once

#include "../Core/AlchemyInterface.h"

namespace alchemy
{
	class CSceneNode;
	class CSceneObject;
	class CCamera;
	class CBone;
	class CSceneNodeUpdater;

	ALCHEMY_INTERFACE(ISceneNodeFactory)
	{
		virtual CSceneNode  &		CreateSceneNode(  void)  = 0;
		virtual CSceneObject&		CreateSceneObject(void)  = 0;
		virtual CCamera     &		CreateCamera(     void)  = 0;
		virtual CBone       &		CreateBone(       void)	 = 0;
		virtual CSceneNodeUpdater&	CreateSceneNodeUpdater() = 0;

		virtual bool DestroySceneNode(CSceneNode& SceneNode) = 0;
	};
}