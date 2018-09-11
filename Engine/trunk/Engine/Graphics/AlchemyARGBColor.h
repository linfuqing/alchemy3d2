#pragma once

#include "AlchemyRGBColor.h"

#include "../Core/AlchemyUtils.h"

#include "../Math/AlchemyMath.h"
#include "../Math/AlchemyFloat4.h"

namespace alchemy
{
#define  ALCHEMY_ARGBCOLOR_255(a, r, g, b) ( ( ( (a) << 24 ) & 0xff000000 ) | ( ( (r) << 16 ) & 0xff0000 ) | ( ( (g) << 8 ) & 0xff00 ) | ( (b) & 0xff ) )
#define ALCHEMY_ARGBCOLOR(a, r, g, b) ALCHEMY_ARGBCOLOR_255( ALCHEMY_CLAMP(static_cast<INT>(a * 255.0f), 0, 255), ALCHEMY_CLAMP(static_cast<INT>(r * 255.0f), 0, 255), ALCHEMY_CLAMP(static_cast<INT>(g * 255.0f), 0, 255), ALCHEMY_CLAMP(static_cast<INT>(b * 255.0f), 0, 255) )

#define ALCHEMY_ARGBCOLOR_ALPHA_255(ARGBColor) ( (ARGBColor >> 24) & 0xff )
#define ALCHEMY_ARGBCOLOR_RED_255(ARGBColor) ( (ARGBColor >> 16) & 0xff )
#define ALCHEMY_ARGBCOLOR_GREEN_255(ARGBColor) ( (ARGBColor >> 8) & 0xff )
#define ALCHEMY_ARGBCOLOR_BLUE_255(ARGBColor) ( (ARGBColor) & 0xff )
    
#define ALCHEMY_ARGBCOLOR_ALPHA(ARGBColor) (ALCHEMY_ARGBCOLOR_ALPHA_255(ARGBColor) * g_fINV_255)
#define ALCHEMY_ARGBCOLOR_RED(ARGBColor)   (ALCHEMY_ARGBCOLOR_RED_255(ARGBColor) * g_fINV_255)
#define ALCHEMY_ARGBCOLOR_GREEN(ARGBColor) (ALCHEMY_ARGBCOLOR_GREEN_255(ARGBColor) * g_fINV_255)
#define ALCHEMY_ARGBCOLOR_BLUE(ARGBColor)  (ALCHEMY_ARGBCOLOR_BLUE_255(ARGBColor) * g_fINV_255)
    
	typedef UINT32 ARGBCOLOR;

	class CARGBColor :
		public CFloat4
	{
	public:
		CARGBColor(void);
		CARGBColor(FLOAT r, FLOAT g, FLOAT b, FLOAT a);
		~CARGBColor(void);

		CARGBColor& operator=(const CARGBColor& Value);
		CARGBColor& operator=(ARGBCOLOR Value);

		CARGBColor& operator*=(const CARGBColor& Value);
		CARGBColor& operator*=(const CRGBColor& Value);

		CARGBColor& operator+=(const CARGBColor& Value);
		CARGBColor& operator+=(const CRGBColor& Value);
		
		operator ARGBCOLOR()const;
		operator RGBCOLOR()const;
	};

	ALCHEMY_INLINE CARGBColor& CARGBColor::operator=(const CARGBColor& Value)
	{
		a = Value.a;
		r = Value.r;
		g = Value.g;
		b = Value.b;

		return *this;
	}

	ALCHEMY_INLINE CARGBColor& CARGBColor::operator=(ARGBCOLOR Value)
	{
		a = ALCHEMY_ARGBCOLOR_ALPHA(Value);
		r = ALCHEMY_ARGBCOLOR_RED(Value);
		g = ALCHEMY_ARGBCOLOR_GREEN(Value);
		b = ALCHEMY_ARGBCOLOR_BLUE(Value);

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
	
	ALCHEMY_INLINE CARGBColor::operator ARGBCOLOR()const
	{
		return ALCHEMY_ARGBCOLOR(a, r, g, b);
	}
	
	ALCHEMY_INLINE CARGBColor::operator RGBCOLOR()const
	{
		return ALCHEMY_ARGBCOLOR(0.0f, r, g, b);
	}
}
