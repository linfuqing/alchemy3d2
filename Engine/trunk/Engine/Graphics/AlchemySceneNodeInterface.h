#pragma once

#include "../Core/AlchemyInterface.h"

namespace alchemy
{
	class CSceneNode;
	ALCHEMY_INTERFACE(ISceneNode)
	{
		virtual bool AddChild(CSceneNode& Child) = 0;
		virtual bool RemoveChild(CSceneNode& Child) = 0;
	};
}