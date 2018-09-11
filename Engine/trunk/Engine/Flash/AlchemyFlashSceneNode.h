#pragma once

#include "../Graphics/AlchemySceneNode.h"
#include "AlchemyFlashAbstractSceneNode.h"

namespace alchemy
{
	class CFlashSceneNode :
		public CSceneNode, public CFlashAbstractSceneNode
	{
	public:
		CFlashSceneNode(void);
		~CFlashSceneNode(void);

		bool AddChild(CSceneNode& Child);
		bool RemoveChild(CSceneNode& Child);

		ALCHEMY_IMPLEMENT_INTERFACE

#ifdef AS3_H
		AS3TYPE Init();
#endif
	};
}