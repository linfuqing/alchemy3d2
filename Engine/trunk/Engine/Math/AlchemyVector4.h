#pragma once

#include <math.h>
#include "AlchemyFloat4.h"
#include "AlchemyMatrix4x4.h"

namespace alchemy
{
	FLOAT4& Transform(FLOAT4& Output, const MATRIX4X4& Matrix, const FLOAT4& Position);
	
	ALCHEMY_INLINE FLOAT4& Transform(FLOAT4& Output, const MATRIX4X4& Matrix, const FLOAT4& Position)
	{
		FLOAT x = Position.x, y = Position.y, z = Position.z, w = Position.w;
		
		Output.x = x * Matrix._11 + y * Matrix._21 + z * Matrix._31 + w * Matrix._41;
		Output.y = x * Matrix._12 + y * Matrix._22 + z * Matrix._32 + w * Matrix._42;
		Output.z = x * Matrix._13 + y * Matrix._23 + z * Matrix._33 + w * Matrix._43;
		Output.w = x * Matrix._14 + y * Matrix._24 + z * Matrix._34 + w * Matrix._44;
		
		return Output;
	}
	
	class CVector4 :
		public CFloat4
	{
	public:
		CVector4(void) {}
		CVector4(FLOAT x, FLOAT y, FLOAT z, FLOAT w = 1.0f);
		CVector4(const CFloat4& Float);
		~CVector4(void);

		CVector4& operator=(const FLOAT4& Value);

		FLOAT DotProduct(const FLOAT4& Value)const;
		FLOAT GetLengthSquared()const;
		FLOAT GetLength()const;

		CVector4& Transform(const MATRIX4X4& Matrix);
	};

	ALCHEMY_INLINE CVector4& CVector4::operator=(const FLOAT4& Value)
	{
		x = Value.x;
		y = Value.y;
		z = Value.z;
		w = Value.w;

		return *this;
	}
	
	ALCHEMY_INLINE FLOAT CVector4::DotProduct(const FLOAT4& Value)const
	{
		return x * Value.x + y * Value.y + z * Value.z + w * Value.w;
	}
	
	ALCHEMY_INLINE FLOAT CVector4::GetLengthSquared()const
	{
		return x * x + y * y + z * z;
	}

	ALCHEMY_INLINE FLOAT CVector4::GetLength()const
	{
		return sqrtf( GetLengthSquared() );
	}

	ALCHEMY_INLINE CVector4& CVector4::Transform(const struct MATRIX4X4& Matrix)
	{
		FLOAT x = this->x, y = this->y, z = this->z, w = this->w;

		this->x = x * Matrix._11 + y * Matrix._21 + z * Matrix._31 + w * Matrix._41;
		this->y = x * Matrix._12 + y * Matrix._22 + z * Matrix._32 + w * Matrix._42;
		this->z = x * Matrix._13 + y * Matrix._23 + z * Matrix._33 + w * Matrix._43;
		this->w = x * Matrix._14 + y * Matrix._24 + z * Matrix._34 + w * Matrix._44;

		return *this;
	}
}