#pragma once

#include "../Graphics/AlchemySkyBox.h"
#include "AlchemyFlashAbstractSceneNode.h"

namespace alchemy
{
	class CFlashSkyBox : 
		public CSkyBox, public CFlashAbstractSceneNode
	{
	public:
		CFlashSkyBox(void);
		~CFlashSkyBox(void);

		bool AddChild(CSceneNode& Child);
		bool RemoveChild(CSceneNode& Child);

#ifdef AS3_H
		AS3TYPE Init();
#endif
	};
}