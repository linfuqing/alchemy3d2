#pragma once

#include "AlchemyBoundingVolumeInterface.h"

namespace alchemy
{
	class CClipperInterface
	{
	public:
		CClipperInterface(void) {}
		~CClipperInterface(void) {}

		virtual bool Test(const CBoundingVolumeInterface& BoundingVolume)const = 0;
	};
}