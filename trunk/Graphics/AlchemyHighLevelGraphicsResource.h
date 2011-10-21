#pragma once

#include "AlchemyGraphicsResource.h"

namespace alchemy
{
	class CHighLevelGraphicsResource :
		public CGraphicsResource
	{
	public:
		CHighLevelGraphicsResource(void);
		~CHighLevelGraphicsResource(void);

	protected:
		bool _Create();

		CHighLevelGraphicsResource* _Clone();
	};
}