#pragma once
#include "../Graphics/AlchemySceneNodeFactory.h"
#include "../Core/AlchemySingleton.h"
#include "AlchemyFlashObject.h"

namespace alchemy
{
#define ALCHEMY_FLASH_SCENE_NODE_FACTORY CFlashSceneNodeFactory::GetInstance()

	class CFlashSceneNodeFactory :
		public CFlashObject, public CSceneNodeFactory, public CSingleton<CFlashSceneNodeFactory>
	{
	public:
		typedef enum
		{
			SCENE_NODE = 0,
			CAMERA,
			SCENE_OBJECT,
			BONE,

			TOTAL_TYPES
		}SCENENODETYPE;

		CFlashSceneNodeFactory(void);
		~CFlashSceneNodeFactory(void);

		CBone		& CreateBone(		void);
		CSceneNode  & CreateSceneNode(  void);
		CCamera     & CreateCamera(     void);
		CSceneObject& CreateSceneObject(void);

		CSceneNode& CreateSceneNode(SCENENODETYPE Type);

		bool DestroySceneNode(CSceneNode& SceneNode);

#ifdef AS3_H
		AS3TYPE Init();
		void Fill(AS3TYPE Object, AS3TYPE MethodArray);
#endif
	private:

#ifdef AS3_H
		AS3TYPE m_CreateSceneNodeMethod;
		AS3TYPE m_DestroySceneNodeMethod;
#endif
	};
}