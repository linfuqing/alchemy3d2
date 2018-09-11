#pragma once

#include "../Graphics/AlchemyBone.h"
#include "AlchemyFlashAbstractSceneNode.h"

namespace alchemy
{
	class CFlashBone :
		public CBone, public CFlashAbstractSceneNode
	{
	public:
		CFlashBone(void);
		~CFlashBone(void);

		bool AddChild(CSceneNode& Child);
		bool RemoveChild(CSceneNode& Child);

		ALCHEMY_IMPLEMENT_INTERFACE

#ifdef AS3_H
		AS3TYPE Init();
#endif
	};
}