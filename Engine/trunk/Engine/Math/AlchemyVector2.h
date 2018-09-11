#pragma once
/*
 *  AlchemyVector2.h
 *  IOS
 *
 *  Created by lin fuqing on 12-5-30.
 *  Copyright 2012 AstepGame. All rights reserved.
 *
 */

#include "AlchemyFloat2.h"
#include "AlchemyFloat4.h"
#include "AlchemyMatrix3x3.h"
#include <math.h>

namespace alchemy
{
	FLOAT2& Transform(FLOAT2& Output, const FLOAT4& Matrix, const FLOAT2& Input);
	FLOAT2& TransformCoord(FLOAT2& Output, const MATRIX3X3& Matrix, const FLOAT2& Input);
	FLOAT2& TransformNormal(FLOAT2& Output, const MATRIX3X3& Matrix, const FLOAT2& Input);

	ALCHEMY_INLINE FLOAT2& Transform(FLOAT2& Output, const FLOAT4& Matrix, const FLOAT2& Input)
	{
		FLOAT x = Input.x, y = Input.y;

		Output.x = Matrix.x * x + Matrix.y * y;
		Output.y = Matrix.z * x + Matrix.w * y;

		return Output;
	}

	ALCHEMY_INLINE FLOAT2& TransformCoord(FLOAT2& Output, const MATRIX3X3& Matrix, const FLOAT2& Input)
	{
		FLOAT x = Input.x, y = Input.y;

		Output.x = Matrix._11 * x + Matrix._21 * y + Matrix._31;
		Output.y = Matrix._12 * x + Matrix._22 * y + Matrix._32;

		return Output;
	}

	ALCHEMY_INLINE FLOAT2& TransformNormal(FLOAT2& Output, const MATRIX3X3& Matrix, const FLOAT2& Input)
	{
		FLOAT x = Input.x, y = Input.y;

		Output.x = Matrix._11 * x + Matrix._21 * y;
		Output.y = Matrix._12 * x + Matrix._22 * y;

		return Output;
	}

	class CVector2 : 
	public CFloat2 
	{
	public:
		CVector2();
		CVector2(FLOAT x, FLOAT y);
		~CVector2();
		
		FLOAT DotProduct(const FLOAT2& Value)const;
		FLOAT GetLengthSquared()const;
		FLOAT GetLength()const;
		
		void Normalize();
	};
	
	
	ALCHEMY_INLINE FLOAT CVector2::DotProduct(const FLOAT2& Value)const
	{
		return x * Value.x + y * Value.y;
	}
	
	ALCHEMY_INLINE FLOAT CVector2::GetLengthSquared()const
	{
		return x * x + y * y;
	}
	
	ALCHEMY_INLINE FLOAT CVector2::GetLength()const
	{
		return sqrtf( GetLengthSquared() );
	}
}