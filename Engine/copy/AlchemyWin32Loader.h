#pragma once

#include "../Loader/AlchemyLoader.h"

namespace alchemy
{
	class CWin32Loader : 
		public CLoader
	{
	public:
		CWin32Loader(void);
		~CWin32Loader(void);

		void Load(const CHAR* pcPath);
	};
}