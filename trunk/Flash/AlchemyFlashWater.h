#pragma once

#include "AlchemyFlashAbstractSceneNode.h"

#include "../Graphics/AlchemyWater.h"

namespace alchemy
{
	class CFlashWater :
		public CWater, public CFlashAbstractSceneNode
	{
	public:
		CFlashWater(void);
		~CFlashWater(void);

		bool AddChild(CSceneNode& Child);
		bool RemoveChild(CSceneNode& Child);

	#ifdef AS3_H
		AS3TYPE Init();
	#endif
	};
}
