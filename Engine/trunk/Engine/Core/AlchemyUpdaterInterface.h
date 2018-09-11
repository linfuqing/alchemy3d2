#pragma once

#include "AlchemyInterface.h"

namespace alchemy
{
	ALCHEMY_INTERFACE(IUpdater)
	{
		virtual void Update(FLOAT fElapseTime) = 0;
	};
}