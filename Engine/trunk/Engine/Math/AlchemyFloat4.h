#pragma once

#include "AlchemyFloat3.h"

namespace alchemy
{
	typedef struct
	{
//#pragma pack(1)
		union
		{
			struct
			{
				FLOAT x, y, z, w;
			};

			struct
			{
				FLOAT r, g, b, a;
			};

			struct
			{
				FLOAT3 Float3;
				
				FLOAT  fFloat;
			};

			FLOAT fValue[4];
		};
//#pragma pack()
	}FLOAT4, * LPFLOAT4;

	FLOAT4& Copy(FLOAT4& Destination, const FLOAT4& Source);
	FLOAT DotProduct(const FLOAT4& Source1, const FLOAT4& Source2);
	FLOAT DotProductCoord(const FLOAT4& Source1, const FLOAT3& Source2);
	FLOAT DotProductNormal(const FLOAT4& Source1, const FLOAT3& Source2);

	ALCHEMY_INLINE FLOAT4& Copy(FLOAT4& Destination, const FLOAT4& Source)
	{
		if(&Destination == &Source)
			return Destination;

		Destination.x = Source.x;
		Destination.y = Source.y;
		Destination.z = Source.z;
		Destination.w = Source.w;

		return Destination;
	}

	ALCHEMY_INLINE FLOAT DotProduct(const FLOAT4& Source1, const FLOAT4& Source2)
	{
		return Source1.x * Source2.x + Source1.y * Source2.y + Source1.z * Source2.z + Source1.w * Source2.w;
	}

	ALCHEMY_INLINE FLOAT DotProductCoord(const FLOAT4& Source1, const FLOAT3& Source2)
	{
		return Source1.x * Source2.x + Source1.y * Source2.y + Source1.z * Source2.z + Source1.w;
	}

	ALCHEMY_INLINE FLOAT DotProductNormal(const FLOAT4& Source1, const FLOAT3& Source2)
	{
		return Source1.x * Source2.x + Source1.y * Source2.y + Source1.z * Source2.z;
	}

	class CFloat4 :
		public FLOAT4
	{
	public:
		CFloat4(void) {}
		CFloat4(FLOAT x, FLOAT y, FLOAT z, FLOAT w = 1.0f);
		~CFloat4(void);

		CFloat4& operator*=(const FLOAT Float);
		CFloat4& operator-=(const FLOAT4& Float);

		operator const FLOAT*()const;
	};
	
	ALCHEMY_INLINE CFloat4& CFloat4::operator*=(const FLOAT Float)
	{
		x -= Float;
		y -= Float;
		z -= Float;
		w -= Float;
		
		return *this;
	}

	ALCHEMY_INLINE CFloat4& CFloat4::operator-=(const FLOAT4& Float)
	{
		x -= Float.x;
		y -= Float.y;
		z -= Float.z;
		w -= Float.w;

		return *this;
	}

	ALCHEMY_INLINE CFloat4::operator const FLOAT*()const
	{
		return fValue;
	}
}