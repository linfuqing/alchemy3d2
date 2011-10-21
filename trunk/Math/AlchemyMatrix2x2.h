#pragma once

#include "AlchemyFloat2.h"

namespace alchemy
{
	typedef struct  
	{
		union
		{
			struct
			{
				FLOAT _11, _12,
					_21, _22;
			};

			FLOAT fPosition[2][2];
			FLOAT fValue[4];

			FLOAT2 Row[2];
		};
	}MATRIX2X2;

	class CMatrix2x2 :
		public MATRIX2X2
	{
	public:
		CMatrix2x2(void);
		~CMatrix2x2(void);
	};
}