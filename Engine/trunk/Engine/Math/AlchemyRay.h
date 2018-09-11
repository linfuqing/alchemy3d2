#pragma once
/*
 *  AlchemyRay.h
 *  IOS
 *
 *  Created by lin fuqing on 12-7-2.
 *  Copyright 2012 AstepGame. All rights reserved.
 *
 */

#include "AlchemyMatrix4x4.h"

#include "../Core/AlchemyNull.h"

namespace alchemy
{
	typedef struct
	{
		FLOAT3 Origin;
		FLOAT3 Direction;
	}RAY, * LPRAY;
	
	bool HitTriangle(const RAY& Ray,
					 const FLOAT3& a, 
					 const FLOAT3& b, 
					 const FLOAT3& c, 
					 LPFLOAT3 pHitPoint = ALCHEMY_NULL, 
					 bool* pbIsHitOn = ALCHEMY_NULL, 
					 bool* pbIsFront = ALCHEMY_NULL, 
					 PFLOAT pU = ALCHEMY_NULL, 
					 PFLOAT pV = ALCHEMY_NULL, 
					 PFLOAT pT = ALCHEMY_NULL);
	
	class CRay :
	public RAY
	{
	public:
		CRay();
		CRay(const RAY& Ray);
		CRay(FLOAT3 Origin, FLOAT3 Direction);
		CRay(FLOAT x, FLOAT y, FLOAT z, FLOAT fToX, FLOAT fToY, FLOAT fToZ);
		~CRay();
		
		bool HitTriangle(const FLOAT3& a, 
						 const FLOAT3& b, 
						 const FLOAT3& c, 
						 LPFLOAT3 pHitPoint = ALCHEMY_NULL, 
						 bool* pbIsHitOn = ALCHEMY_NULL, 
						 bool* pbIsFront = ALCHEMY_NULL, 
						 PFLOAT pU = ALCHEMY_NULL, 
						 PFLOAT pV = ALCHEMY_NULL, 
						 PFLOAT pT = ALCHEMY_NULL)const;
						 
		void Transform(const MATRIX4X4& Matrix, bool bIsInverceTranspose = false);
	};
}