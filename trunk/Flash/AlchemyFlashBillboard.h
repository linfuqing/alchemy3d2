#pragma once

#include "../Graphics/AlchemyBillboard.h"
#include "AlchemyFlashAbstractSceneNode.h"

namespace alchemy
{
	class CFlashBillboard : 
		public CBillboard, public CFlashAbstractSceneNode
	{
	public:
		CFlashBillboard(void);
		~CFlashBillboard(void);

		bool AddChild(CSceneNode& Child);
		bool RemoveChild(CSceneNode& Child);

#ifdef AS3_H
		AS3TYPE Init();
#endif
	};
}