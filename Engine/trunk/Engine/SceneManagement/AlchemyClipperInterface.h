#pragma once

#include "AlchemyBoundingVolumeInterface.h"

namespace alchemy
{
	ALCHEMY_INTERFACE(IClipper)
	{
		virtual bool Test(const IBoundingVolume& BoundingVolume)const = 0;
	};
}