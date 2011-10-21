#pragma once

#include "../Graphics/AlchemySkydome.h"
#include "AlchemyFlashAbstractSceneNode.h"

namespace alchemy
{
	class CFlashSkyDome : 
		public CSkyDome, public CFlashAbstractSceneNode
	{
	public:
		CFlashSkyDome(void);
		~CFlashSkyDome(void);

		bool AddChild(CSceneNode& Child);
		bool RemoveChild(CSceneNode& Child);

#ifdef AS3_H
		AS3TYPE Init();
#endif
	};
}