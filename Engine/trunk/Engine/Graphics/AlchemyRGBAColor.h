#pragma once

#include "AlchemyRGBColor.h"

#include "../Core/AlchemyUtils.h"

#include "../Math/AlchemyMath.h"
#include "../Math/AlchemyFloat4.h"

namespace alchemy
{
#define ALCHEMY_RGBACOLOR_255(r, g, b, a) ( ( ( (r) << 24 ) & 0xff ) | ( ( (g) << 16 ) & 0xff ) | ( ( (b) << 8 ) & 0xff ) | ( (a) & 0xff ) )
#define ALCHEMY_RGBACOLOR(r, g, b, a) ALCHEMY_RGBACOLOR_255( ALCHEMY_CLAMP(static_cast<INT>(r * 255.0f), 0, 255), ALCHEMY_CLAMP(static_cast<INT>(g * 255.0f), 0, 255), ALCHEMY_CLAMP(static_cast<INT>(b * 255.0f), 0, 255), ALCHEMY_CLAMP(static_cast<INT>(a * 255.0f), 0, 255) )

#define ALCHEMY_RGBACOLOR_RED_255(RGBAColor) ( (RGBAColor >> 24) & 0xff )
#define ALCHEMY_RGBACOLOR_GREEN_255(RGBAColor) ( (RGBAColor >> 16) & 0xff )
#define ALCHEMY_RGBACOLOR_BLUE_255(RGBAColor) ( (RGBAColor >> 8) & 0xff )
#define ALCHEMY_RGBACOLOR_ALPHA_255(RGBAColor) ( (RGBAColor) & 0xff )

#define ALCHEMY_RGBACOLOR_RED(RGBAColor)   (ALCHEMY_RGBACOLOR_RED_255(RGBAColor) * g_fINV_255)
#define ALCHEMY_RGBACOLOR_GREEN(RGBAColor) (ALCHEMY_RGBACOLOR_GREEN_255(RGBAColor) * g_fINV_255)
#define ALCHEMY_RGBACOLOR_BLUE(RGBAColor)  (ALCHEMY_RGBACOLOR_BLUE_255(RGBAColor) * g_fINV_255)
#define ALCHEMY_RGBACOLOR_ALPHA(RGBAColor) (ALCHEMY_RGBACOLOR_ALPHA_255(RGBAColor) * g_fINV_255)

	typedef UINT32 RGBACOLOR;

	class CRGBAColor :
		public CFloat4
	{
	public:
		CRGBAColor(void);
		CRGBAColor(FLOAT r, FLOAT g, FLOAT b, FLOAT a);
		~CRGBAColor(void);

		CRGBAColor& operator=(const CRGBAColor& Value);
		CRGBAColor& operator=(RGBACOLOR Value);

		CRGBAColor& operator*=(const CRGBAColor& Value);
		CRGBAColor& operator*=(const CRGBColor& Value);

		CRGBAColor& operator+=(const CRGBAColor& Value);
		CRGBAColor& operator+=(const CRGBColor& Value);

		operator RGBACOLOR()const;
		operator RGBCOLOR()const;
	};

	ALCHEMY_INLINE CRGBAColor& CRGBAColor::operator=(const CRGBAColor& Value)
	{
		a = Value.a;
		r = Value.r;
		g = Value.g;
		b = Value.b;

		return *this;
	}

	ALCHEMY_INLINE CRGBAColor& CRGBAColor::operator=(RGBACOLOR Value)
	{
		a = ALCHEMY_RGBACOLOR_ALPHA(Value);
		r = ALCHEMY_RGBACOLOR_RED(Value);
		g = ALCHEMY_RGBACOLOR_GREEN(Value);
		b = ALCHEMY_RGBACOLOR_BLUE(Value);

		return *this;
	}

	ALCHEMY_INLINE CRGBAColor& CRGBAColor::operator*=(const CRGBAColor& Value)
	{
		a *= Value.a;
		r *= Value.r;
		g *= Value.g;
		b *= Value.b;

		return *this;
	}

	ALCHEMY_INLINE CRGBAColor& CRGBAColor::operator*=(const CRGBColor& Value)
	{
		r *= Value.r;
		g *= Value.g;
		b *= Value.b;

		return *this;
	}

	ALCHEMY_INLINE CRGBAColor& CRGBAColor::operator+=(const CRGBAColor& Value)
	{
		a += Value.a;
		r += Value.r;
		g += Value.g;
		b += Value.b;

		return *this;
	}

	ALCHEMY_INLINE CRGBAColor& CRGBAColor::operator+=(const CRGBColor& Value)
	{
		a  = 1.0f;
		r += Value.r;
		g += Value.g;
		b += Value.b;

		return *this;
	}

	ALCHEMY_INLINE CRGBAColor::operator RGBACOLOR()const
	{
		return ALCHEMY_RGBACOLOR(r, g, b, a);
	}

	ALCHEMY_INLINE CRGBAColor::operator RGBCOLOR()const
	{
		return (ALCHEMY_CLAMP(static_cast<INT>(r * 255.0f), 0, 255) << 16) |
			(ALCHEMY_CLAMP(static_cast<INT>(g * 255.0f), 0, 255) << 8 ) |
			(ALCHEMY_CLAMP(static_cast<INT>(b * 255.0f), 0, 255)      );
	}
}
