#pragma once

#include "AlchemyUpdaterInterface.h"

namespace alchemy
{
	struct ITimerQueue : 
		public IUpdater
	{
		ALCHEMY_INTERFACE(ITimer)
		{
			virtual void Handler(FLOAT fTime, ITimerQueue& TimeQueue) = 0;
		};

		virtual bool AdvanceTime(FLOAT fTime, ITimer& Timer) = 0;
	};
}