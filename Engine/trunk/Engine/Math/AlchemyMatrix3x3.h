#pragma once

#include "AlchemyFloat3.h"

namespace alchemy
{
	typedef struct  
	{
		union
		{
			struct
			{
				FLOAT _11, _12, _13, 
					_21, _22, _23, 
					_31, _32, _33;
			};

			FLOAT fPosition[3][3];
			FLOAT fValue[9];

			FLOAT3 Row[3];
		};
	}MATRIX3X3;

	class CMatrix3x3
	{
	public:
		CMatrix3x3(void);
		~CMatrix3x3(void);
	};
}