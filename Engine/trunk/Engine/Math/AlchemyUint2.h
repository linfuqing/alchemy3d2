#pragma once

#include "../Core/AlchemyDataType.h"

namespace alchemy
{
	typedef struct  
	{
		union
		{
			struct  
			{
				UINT x;
				UINT y;
			};

			struct
			{
				UINT uWidth;
				UINT uHeight;
			};
		};
	}UINT2, * LPUINT2;

	class CUint2 :
		public UINT2
	{
	public:
		CUint2(void);
		~CUint2(void);
	};
}