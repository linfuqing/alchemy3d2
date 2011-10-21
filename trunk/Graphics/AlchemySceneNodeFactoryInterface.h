#pragma once

#include "AlchemySceneObject.h"
#include "AlchemyCamera.h"
#include "AlchemyBone.h"

namespace alchemy
{
	class CSceneNodeFactoryInterface
	{
	public:

		CSceneNodeFactoryInterface(void) {}
		virtual ~CSceneNodeFactoryInterface(void) {}

		virtual CSceneNode  & CreateSceneNode(  void) = 0;
		virtual CSceneObject& CreateSceneObject(void) = 0;
		virtual CCamera     & CreateCamera(     void) = 0;
		virtual CBone       & CreateBone(       void) = 0;

		virtual bool DestroySceneNode(CSceneNode& SceneNode) = 0;
	};
}