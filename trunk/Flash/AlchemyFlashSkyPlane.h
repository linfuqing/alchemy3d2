#pragma once

#include "../Graphics/AlchemySkyPlane.h"
#include "AlchemyFlashAbstractSceneNode.h"

namespace alchemy
{
	class CFlashSkyPlane :
		public CSkyPlane, public CFlashAbstractSceneNode
	{
	public:
		CFlashSkyPlane(void);
		~CFlashSkyPlane(void);

		bool AddChild(CSceneNode& Child);
		bool RemoveChild(CSceneNode& Child);

#ifdef AS3_H
		AS3TYPE Init();
#endif
	};
}