#pragma once

#include "../Graphics/AlchemyBillboardSystem.h"
#include "../Graphics/AlchemyBillboardSet.h"
#include "AlchemyFlashAbstractSceneNode.h"

namespace alchemy
{
	class CFlashBillboardSystem : 
		public CBillboardSystem, public CFlashAbstractSceneNode
	{
	public:
		CFlashBillboardSystem(void);
		~CFlashBillboardSystem(void);

		bool AddChild(CSceneNode& Child);
		bool RemoveChild(CSceneNode& Child);

		ALCHEMY_IMPLEMENT_INTERFACE

#ifdef AS3_H
		AS3TYPE Init();
#endif
	};
}