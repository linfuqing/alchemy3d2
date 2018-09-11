#pragma once

#include "AlchemyFloat3.h"

#include "AlchemyMatrix4x4.h"

namespace alchemy
{
	class CPlane :
		public CFloat4
	{
	public:
		CPlane(void) {};
		CPlane(const FLOAT4& Plane);
		CPlane(FLOAT a, FLOAT b, FLOAT c, FLOAT d);
		CPlane(const FLOAT3& Point1, const FLOAT3& Point2, const FLOAT3& Point3);
		CPlane(const FLOAT3& Point,const FLOAT3& Normal);

		~CPlane(void);

		CPlane& operator=(const CPlane& Plane);
		CPlane& operator*=(FLOAT fValue);
		CPlane& operator*=(const MATRIX4X4& Matrix);

		bool Transform(const MATRIX4X4& Matrix);

		FLOAT Normalize();
	};

	ALCHEMY_INLINE CPlane& CPlane::operator=(const CPlane& Plane)
	{
		x = Plane.x;
		y = Plane.y;
		z = Plane.z;
		w = Plane.w;

		return *this;
	}
	
	ALCHEMY_INLINE CPlane& CPlane::operator*=(FLOAT fValue)
	{
		x *= fValue;
		y *= fValue;
		z *= fValue;
		w *= fValue;
		
		return *this;
	}
}