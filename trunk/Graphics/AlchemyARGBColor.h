#pragma once

#include "../Math/AlchemyMath.h"
#include "../Math/AlchemyFloat4.h"
#include "AlchemyRGBColor.h"

namespace alchemy
{
	typedef UINT32 ARGBCOLOR;

	class CARGBColor :
		public CFloat4
	{
	public:
		CARGBColor(void);
		CARGBColor(FLOAT r, FLOAT g, FLOAT b, FLOAT a);
		~CARGBColor(void);

		CARGBColor& operator=(const CARGBColor& Value);
		CARGBColor& operator=(ARGBCOLOR uValue);

		CARGBColor& operator*=(const CARGBColor& Value);
		CARGBColor& operator*=(const CRGBColor& Value);

		CARGBColor& operator+=(const CARGBColor& Value);
		CARGBColor& operator+=(const CRGBColor& Value);
	};

	ALCHEMY_INLINE CARGBColor& CARGBColor::operator=(const CARGBColor& Value)
	{
		a = Value.a;
		r = Value.r;
		g = Value.g;
		b = Value.b;

		return *this;
	}

	ALCHEMY_INLINE CARGBColor& CARGBColor::operator=(ARGBCOLOR uValue)
	{
		a = (  uValue >> 24         ) * g_fINV_255;
		r = ( (uValue >> 16) & 0xff ) * g_fINV_255;
		g = ( (uValue >> 8)  & 0xff ) * g_fINV_255;
		b = (  uValue        & 0xff ) * g_fINV_255;

		return *this;
	}

	ALCHEMY_INLINE CARGBColor& CARGBColor::operator*=(const CARGBColor& Value)
	{
		a *= Value.a;
		r *= Value.r;
		g *= Value.g;
		b *= Value.b;

		return *this;
	}

	ALCHEMY_INLINE CARGBColor& CARGBColor::operator*=(const CRGBColor& Value)
	{
		r *= Value.r;
		g *= Value.g;
		b *= Value.b;

		return *this;
	}

	ALCHEMY_INLINE CARGBColor& CARGBColor::operator+=(const CARGBColor& Value)
	{
		a += Value.a;
		r += Value.r;
		g += Value.g;
		b += Value.b;

		return *this;
	}

	ALCHEMY_INLINE CARGBColor& CARGBColor::operator+=(const CRGBColor& Value)
	{
		a  = 1.0f;
		r += Value.r;
		g += Value.g;
		b += Value.b;

		return *this;
	}
}
