#pragma once

#include "AlchemyBoundingVolumeInterface.h"

namespace alchemy
{
	class CSceneTreeObjectInterface
	{
	public:

		CSceneTreeObjectInterface(void) {}
		virtual ~CSceneTreeObjectInterface(void) {}

		virtual const CBoundingVolumeInterface& GetBoundingVolume()const = 0;
	};
}