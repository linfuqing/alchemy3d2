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
				FLOAT x, y, z;
			};

			struct
			{
				FLOAT u, v, w;
			};

			struct
			{
				FLOAT r, g, b;
			};

			FLOAT fValue[3];
		};
	}FLOAT3, * LPFLOAT3;

	FLOAT3& Copy(FLOAT3& Destination, const FLOAT3& Source);
	FLOAT3& Negate(FLOAT3& Destination, const FLOAT3& Source);
	FLOAT3& Add(FLOAT3& Output, const FLOAT3& Left, const FLOAT3& Right);
	FLOAT3& Multiply(FLOAT3& Output, FLOAT3& Right, const FLOAT3& Left);

	ALCHEMY_INLINE FLOAT3& Copy(FLOAT3& Destination, const FLOAT3& Source)
	{
		if(&Destination == &Source)
			return Destination;

		Destination.x = Source.x;
		Destination.y = Source.y;
		Destination.z = Source.z;

		return Destination;
	}

	ALCHEMY_INLINE FLOAT3& Negate(FLOAT3& Destination, const FLOAT3& Source)
	{
		Destination.x = - Source.x;
		Destination.y = - Source.y;
		Destination.z = - Source.z;

		return Destination;
	}

	ALCHEMY_INLINE FLOAT3& Add(FLOAT3& Output, const FLOAT3& Left, const FLOAT3& Right)
	{
		Output.x = Left.x + Right.x;
		Output.y = Left.y + Right.y;
		Output.z = Left.z + Right.z;

		return Output;
	}

	ALCHEMY_INLINE FLOAT3& Multiply(FLOAT3& Output, FLOAT3& Right, const FLOAT3& Left)
	{
		Output.x = Left.x + Right.x;
		Output.y = Left.y + Right.y;
		Output.z = Left.z + Right.z;

		return Output;
	}

	class CFloat3 :
		public FLOAT3
	{
	public:
		CFloat3(void) {}
		CFloat3(FLOAT x, FLOAT y, FLOAT z);
		~CFloat3(void);

		CFloat3& operator=(const FLOAT3& Value);

		CFloat3 operator+(const FLOAT3& Value)const;
		CFloat3 operator-(const FLOAT3& Value)const;
		CFloat3 operator*(const FLOAT3& Value)const;
		CFloat3 operator/(const FLOAT3& Value)const;

		CFloat3& operator+=(const FLOAT3& Value);
		CFloat3& operator-=(const FLOAT3& Value);
		CFloat3& operator*=(const FLOAT3& Value);
		CFloat3& operator/=(const FLOAT3& Value);

		CFloat3 operator+(FLOAT fValue)const;
		CFloat3 operator-(FLOAT fValue)const;
		CFloat3 operator*(FLOAT fValue)const;
		CFloat3 operator/(FLOAT fValue)const;

		CFloat3& operator+=(FLOAT fValue);
		CFloat3& operator-=(FLOAT fValue);
		CFloat3& operator*=(FLOAT fValue);
		CFloat3& operator/=(FLOAT fValue);

		operator const FLOAT*()const;
	};

	ALCHEMY_INLINE CFloat3& CFloat3::operator=(const FLOAT3& Value)
	{
		x = Value.x;
		y = Value.y;
		z = Value.z;

		return *this;
	}

	ALCHEMY_INLINE CFloat3 CFloat3::operator+(const FLOAT3& Value)const
	{
		return CFloat3(x + Value.x, y + Value.y, z + Value.z);
	}

	ALCHEMY_INLINE CFloat3 CFloat3::operator-(const FLOAT3& Value)const
	{
		return CFloat3(x - Value.x, y - Value.y, z - Value.z);
	}

	ALCHEMY_INLINE CFloat3 CFloat3::operator*(const FLOAT3& Value)const
	{
		return CFloat3(x * Value.x, y * Value.y, z * Value.z);
	}

	ALCHEMY_INLINE CFloat3 CFloat3::operator/(const FLOAT3& Value)const
	{
		return CFloat3(x / Value.x, y / Value.y, z / Value.z);
	}

	ALCHEMY_INLINE CFloat3& CFloat3::operator+=(const FLOAT3& Value)
	{
		x += Value.x;
		y += Value.y;
		z += Value.z;

		return *this;
	}

	ALCHEMY_INLINE CFloat3& CFloat3::operator-=(const FLOAT3& Value)
	{
		x -= Value.x;
		y -= Value.y;
		z -= Value.z;

		return *this;
	}

	ALCHEMY_INLINE CFloat3& CFloat3::operator*=(const FLOAT3& Value)
	{
		x *= Value.x;
		y *= Value.y;
		z *= Value.z;

		return *this;
	}

	ALCHEMY_INLINE CFloat3& CFloat3::operator/=(const FLOAT3& Value)
	{
		x /= Value.x;
		y /= Value.y;
		z /= Value.z;

		return *this;
	}

	ALCHEMY_INLINE CFloat3 CFloat3::operator+(FLOAT fValue)const
	{
		return CFloat3(x + fValue, y + fValue, z + fValue);
	}

	ALCHEMY_INLINE CFloat3 CFloat3::operator-(FLOAT fValue)const
	{
		return CFloat3(x - fValue, y - fValue, z - fValue);
	}

	ALCHEMY_INLINE CFloat3 CFloat3::operator*(FLOAT fValue)const
	{
		return CFloat3(x * fValue, y * fValue, z * fValue);
	}

	ALCHEMY_INLINE CFloat3 CFloat3::operator/(FLOAT fValue)const
	{
		fValue = 1.0f / fValue;
		return (*this) * fValue;
	}

	ALCHEMY_INLINE CFloat3& CFloat3::operator+=(FLOAT fValue)
	{
		x += fValue;
		y += fValue;
		z += fValue;

		return *this;
	}

	ALCHEMY_INLINE CFloat3& CFloat3::operator-=(FLOAT fValue)
	{
		x -= fValue;
		y -= fValue;
		z -= fValue;

		return *this;
	}

	ALCHEMY_INLINE CFloat3& CFloat3::operator*=(FLOAT fValue)
	{
		x *= fValue;
		y *= fValue;
		z *= fValue;

		return *this;
	}

	ALCHEMY_INLINE CFloat3& CFloat3::operator/=(FLOAT fValue)
	{
		fValue = 1.0f / fValue;

		(*this) *= fValue;

		return *this;
	}

	ALCHEMY_INLINE CFloat3::operator const FLOAT*()const
	{
		return fValue;
	}
}