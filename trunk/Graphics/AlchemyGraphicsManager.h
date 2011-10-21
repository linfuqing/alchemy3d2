#pragma once

#include "../Core/AlchemySingleton.h"

namespace alchemy
{
	class CGraphicsManager :
		public CSingleton<CGraphicsManager>
	{
	public:

		CGraphicsManager(void);
		~CGraphicsManager(void);
	};
}