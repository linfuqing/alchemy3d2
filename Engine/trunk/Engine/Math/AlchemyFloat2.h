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
				FLOAT a, b;
			};

			struct  
			{
				FLOAT x, y;
			};

			struct  
			{
				FLOAT u, v;
			};

			FLOAT fValue[2];
		};
	}FLOAT2, * LPFLOAT2;

	class CFloat2 :
		public FLOAT2
	{
	public:
		CFloat2(void) {}
		CFloat2(FLOAT x, FLOAT y);
		~CFloat2(void);

		CFloat2& operator =(const FLOAT2& Value);
		CFloat2& operator =(const CFloat2& Value);
		CFloat2& operator+=(const CFloat2& Value);
		CFloat2& operator*=(FLOAT fValue);

		operator const FLOAT*()const; 
	};

	ALCHEMY_INLINE CFloat2& CFloat2::operator =(const FLOAT2& Value)
	{
		x = Value.x;
		y = Value.y;

		return *this;
	}

	ALCHEMY_INLINE CFloat2& CFloat2::operator =(const CFloat2& Value)
	{
		x = Value.x;
		y = Value.y;

		return *this;
	}

	ALCHEMY_INLINE CFloat2& CFloat2::operator+=(const CFloat2& Value)
	{
		x += Value.x;
		y += Value.y;

		return *this;
	}

	ALCHEMY_INLINE CFloat2& CFloat2::operator*=(FLOAT fValue)
	{
		x *= fValue;
		y *= fValue;

		return *this;
	}

	ALCHEMY_INLINE CFloat2::operator const FLOAT*()const
	{
		return fValue;
	}
}