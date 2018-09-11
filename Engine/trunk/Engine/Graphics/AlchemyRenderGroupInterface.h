#pragma once

#include "../Core/AlchemyInterface.h"

namespace alchemy
{
	/********************

	Render some scene nodes into renderTargets.
	compute frustum clip.

	**********************/

	ALCHEMY_INTERFACE(IRenderGroup)
	{
		virtual void Reset()		= 0;
		virtual void RenderAll()	= 0;
	};
}