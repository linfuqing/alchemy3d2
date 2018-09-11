#pragma once

#include "../Graphics/AlchemyCamera.h"
#include "AlchemyFlashAbstractSceneNode.h"

namespace alchemy
{
	class CFlashCamera :
		public CCamera, public CFlashAbstractSceneNode
	{
	public:
		CFlashCamera(void);
		~CFlashCamera(void);

		bool AddChild(CSceneNode& Child);
		bool RemoveChild(CSceneNode& Child);

		ALCHEMY_IMPLEMENT_INTERFACE

#ifdef AS3_H
		AS3TYPE Init();
#endif
	};
}