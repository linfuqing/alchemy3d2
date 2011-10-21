#pragma once

#include "AlchemyRenderQueue.h"

namespace alchemy
{
	class CCallBackToRenderInterface :
		public CRenderQueue::CCallBackToRenderInterface
	{
	public:
		CCallBackToRenderInterface(void) {}
		virtual ~CCallBackToRenderInterface(void) {}

		virtual bool IsDynamic()const = 0;
	};
}
