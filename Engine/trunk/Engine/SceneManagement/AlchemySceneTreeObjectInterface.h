#pragma once

#include "AlchemyBoundingVolumeInterface.h"

namespace alchemy
{
	ALCHEMY_INTERFACE(ISceneTreeObject)
	{
		virtual const IBoundingVolume& GetBoundingVolume()const = 0;
	};
}