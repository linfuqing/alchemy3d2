#pragma once

#include <math.h>
#include "AlchemyFloat3.h"
#include "AlchemyMatrix4x4.h"

namespace alchemy
{
	FLOAT3& TransformCoord(FLOAT3& Output, const MATRIX4X4& Matrix, const FLOAT3& Position);
	FLOAT3& TransformNormal(FLOAT3& Output, const MATRIX4X4& Matrix, const FLOAT3& Normal);

	FLOAT& CrossProduct(const FLOAT3& Output, const FLOAT3& v0, const FLOAT3& v1);
	
	FLOAT GetLength(const FLOAT3& Value);
	FLOAT Normalize(FLOAT3& Output, const FLOAT3 Value);
	
	ALCHEMY_INLINE FLOAT3& TransformCoord(FLOAT3& Output, const MATRIX4X4& Matrix, const FLOAT3& Position)
	{
		FLOAT x = Position.x, y = Position.y, z = Position.z, w = 1.0f / (x * Matrix._14 + y * Matrix._24 + z * Matrix._34 + Matrix._44);

		Output.x = (x * Matrix._11 + y * Matrix._21 + z * Matrix._31 + Matrix._41) * w;
		Output.y = (x * Matrix._12 + y * Matrix._22 + z * Matrix._32 + Matrix._42) * w;
		Output.z = (x * Matrix._13 + y * Matrix._23 + z * Matrix._33 + Matrix._43) * w;

		return Output;
	}

	ALCHEMY_INLINE FLOAT3& TransformNormal(FLOAT3& Output, const MATRIX4X4& Matrix, const FLOAT3& Normal)
	{
		FLOAT x = Normal.x, y = Normal.y, z = Normal.z;

		Output.x = x * Matrix._11 + y * Matrix._21 + z * Matrix._31;
		Output.y = x * Matrix._12 + y * Matrix._22 + z * Matrix._32;
		Output.z = x * Matrix._13 + y * Matrix._23 + z * Matrix._33;

		return Output;
	}
	
	ALCHEMY_INLINE FLOAT3& CrossProduct(FLOAT3& Output, const FLOAT3& v0, const FLOAT3& v1)
	{
		FLOAT3* pDestination = &Output;
		if(pDestination == &v0 || pDestination == &v1)
		{
			static FLOAT3 Temp;

			pDestination = &Temp;
		}

		pDestination->x = v0.y * v1.z - v0.z * v1.y;
		pDestination->y = v0.z * v1.x - v0.x * v1.z;
		pDestination->z = v0.x * v1.y - v0.y * v1.x;

		if(pDestination != &Output)
			Copy(Output, *pDestination);

		return Output;
	}

	class CVector3 :
		public CFloat3
	{
	public:
		CVector3(void) {}
		CVector3(FLOAT x, FLOAT y, FLOAT z);
		CVector3(const FLOAT3& Value);
		~CVector3(void);

		CVector3& operator=(const FLOAT3& Value);
		CVector3& operator=(const CVector3& Value);

		static FLOAT Distance(const FLOAT3& lhs, const FLOAT3 rhs);

		void Set(FLOAT x, FLOAT y, FLOAT z);
		void CrossProduct(const FLOAT3& v0, const FLOAT3& v1);

		FLOAT DotProduct(const FLOAT3& Value)const;

		FLOAT GetLengthSquared()const;
		FLOAT GetLength()const;
		CVector3& TransformCoord(const MATRIX4X4& Matrix);
		CVector3& TransformNormal(const MATRIX4X4& Matrix);
		CVector3& Transform(const FLOAT4& Quaternion);

		void Negate();

		FLOAT Normalize();

		void Slerp(const FLOAT3& From, const FLOAT3& To, FLOAT fWeight);
	};

	ALCHEMY_INLINE void CVector3::Set(FLOAT x, FLOAT y, FLOAT z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	ALCHEMY_INLINE FLOAT CVector3::DotProduct(const FLOAT3& Value)const
	{
		return x * Value.x + y * Value.y + z * Value.z;
	}

	ALCHEMY_INLINE CVector3& CVector3::operator=(const FLOAT3& Value)
	{
		x = Value.x;
		y = Value.y;
		z = Value.z;

		return *this;
	}

	ALCHEMY_INLINE CVector3& CVector3::operator=(const CVector3& Value)
	{
		x = Value.x;
		y = Value.y;
		z = Value.z;

		return *this;
	}

	ALCHEMY_INLINE FLOAT CVector3::GetLengthSquared()const
	{
		return x * x + y * y + z * z;
	}

	ALCHEMY_INLINE FLOAT CVector3::GetLength()const
	{
		return sqrtf( GetLengthSquared() );
	}

	ALCHEMY_INLINE CVector3& CVector3::TransformCoord(const MATRIX4X4& Matrix)
	{
		FLOAT x = this->x, y = this->y, z = this->z, w = 1.0f / (x * Matrix._14 + y * Matrix._24 + z * Matrix._34 + Matrix._44);

		this->x = (x * Matrix._11 + y * Matrix._21 + z * Matrix._31 + Matrix._41) * w;
		this->y = (x * Matrix._12 + y * Matrix._22 + z * Matrix._32 + Matrix._42) * w;
		this->z = (x * Matrix._13 + y * Matrix._23 + z * Matrix._33 + Matrix._43) * w;

		return *this;
	}

	ALCHEMY_INLINE CVector3& CVector3::TransformNormal(const MATRIX4X4& Matrix)
	{
		FLOAT x = this->x, y = this->y, z = this->z;

		this->x = x * Matrix._11 + y * Matrix._21 + z * Matrix._31;
		this->y = x * Matrix._12 + y * Matrix._22 + z * Matrix._32;
		this->z = x * Matrix._13 + y * Matrix._23 + z * Matrix._33;

		return *this;
	}

	ALCHEMY_INLINE void CVector3::Negate()
	{
		x = - x;
		y = - y;
		z = - z;
	}
}