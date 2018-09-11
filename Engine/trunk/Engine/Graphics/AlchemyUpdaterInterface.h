#pragma once

#include "../Core/AlchemyInterface.h"

namespace alchemy
{
	ALCHEMY_INTERFACE(IUpdater)
	{
		virtual void Update( FLOAT Elapse ) = 0;
	};
}