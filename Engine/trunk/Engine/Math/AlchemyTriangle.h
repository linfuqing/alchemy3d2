#pragma once

#include "AlchemyVector3.h"

namespace alchemy
{
	typedef struct  
	{
		FLOAT3 a;
		FLOAT3 b;
		FLOAT3 c;
	}TRIANGLE, * LPTRIANGLE;

	ALCHEMY_INLINE FLOAT3& ComputeNormal(FLOAT3& Output, const FLOAT3& a, const FLOAT3& b, const FLOAT3& c)
	{
		CFloat3 ba, cb;
		Subtract(ba, b, a);
		Subtract(cb, c, b);

		CrossProduct(Output, ba, cb);

		Normalize(Output, Output);

		return Output;
	}

	class CTriangle :
		public TRIANGLE
	{
	public:
		CTriangle(void);
		~CTriangle(void);
	};
}