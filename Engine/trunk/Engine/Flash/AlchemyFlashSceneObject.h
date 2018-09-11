#pragma once

#include "../Graphics/AlchemySceneObject.h"
#include "AlchemyFlashAbstractSceneNode.h"

namespace alchemy
{
	class CFlashSceneObject :
		public CSceneObject, public CFlashAbstractSceneNode
	{
	public:
		CFlashSceneObject(void);
		~CFlashSceneObject(void);

		bool AddChild(CSceneNode& Child);
		bool RemoveChild(CSceneNode& Child);

		ALCHEMY_IMPLEMENT_INTERFACE

#ifdef AS3_H
		AS3TYPE Init();
#endif
	};
}