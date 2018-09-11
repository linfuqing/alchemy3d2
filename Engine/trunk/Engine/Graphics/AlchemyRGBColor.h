#pragma once

#include "../Math/AlchemyMath.h"
#include "../Math/AlchemyFloat4.h"

namespace alchemy
{
	typedef UINT RGBCOLOR;

	class CRGBColor :
		public CFloat4
	{
	public:
		CRGBColor(void);
		CRGBColor(FLOAT r, FLOAT g, FLOAT b);
		~CRGBColor(void);

		CRGBColor& operator=(UINT uValue);
	};

	ALCHEMY_INLINE CRGBColor& CRGBColor::operator=(UINT uValue)
	{
		r = ( (uValue >> 16) & 0xff ) * g_fINV_255;
		g = ( (uValue >> 8 ) & 0xff ) * g_fINV_255;
		b = (  uValue        & 0xff ) * g_fINV_255;
		a = 1.0f;

		return *this;
	}
}