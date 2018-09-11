/*
 *  AlchemyRay.cpp
 *  IOS
 *
 *  Created by lin fuqing on 12-7-2.
 *  Copyright 2012 AstepGame. All rights reserved.
 *
 */

#include "AlchemyRay.h"
#include "AlchemyVector3.h"

using namespace alchemy;

bool alchemy::HitTriangle(const RAY& Ray,
						  const FLOAT3& a, 
						  const FLOAT3& b, 
						  const FLOAT3& c, 
						  LPFLOAT3 pHitPoint, 
						  bool* pbIsHitOn, 
						  bool* pbIsFront, 
						  PFLOAT pU, 
						  PFLOAT pV, 
						  PFLOAT pT)
{
	CVector3 ba; //(b.x - a.x, b.y - a.y, b.z - a.z);
	ba  = b;
	ba -= a;

	CVector3 ca;//(c.x - a.x, c.y - a.y, c.z - a.z);
	ca  = c;
	ca -= a;

	CVector3 Temp;
	Temp.CrossProduct(Ray.Direction, ca);

	FLOAT fDeterminant = ba.DotProduct(Temp);
	
	if(!fDeterminant)
		return false;
	
	CVector3 Tangent;
	if(fDeterminant > 0)
	{
		Tangent  = Ray.Origin;
		Tangent -= a;
		
		if(pbIsFront)
			*pbIsFront = true;
	}
	else
	{
		Tangent      =   a;
		Tangent     -=   Ray.Origin;
		fDeterminant = - fDeterminant;
		
		if(pbIsFront)
			*pbIsFront = false;
	}

	FLOAT u = Tangent.DotProduct(Temp);
	if(u < 0.0f || u > fDeterminant)
		return false;

	Temp.CrossProduct(Tangent, ba);
	
	FLOAT v = Temp.DotProduct(Ray.Direction);
	if(v < 0.0f || u + v > fDeterminant)
		return false;
	
	if(pbIsHitOn)
		*pbIsHitOn = u == 0.0f || v == 0.0f || v + u == fDeterminant;
	
	FLOAT fInvercefDeterminant = 1.0f / fDeterminant;
	if(pU)
		*pU = u * fInvercefDeterminant;
	
	if(pV)
		*pV = v * fInvercefDeterminant;
	
	if(pT || pHitPoint)
	{
		FLOAT t = ca.DotProduct(Temp) * fInvercefDeterminant;
		
		if(pT)
			*pT = t;
		
		if(pHitPoint)
		{
			Temp  = Ray.Direction;
			Temp *= t;
			Temp += Ray.Origin;
			
			*pHitPoint = Temp;
		}
	}
	
	return true;
}

CRay::CRay()
{
}

CRay::CRay(const RAY& Ray)
{
	Copy(Origin   , Ray.Origin   );
	Copy(Direction, Ray.Direction);
}

CRay::CRay(FLOAT3 Origin, FLOAT3 Direction)
{
	Copy(this->Origin   , Origin   );
	Copy(this->Direction, Direction);
}

CRay::CRay(FLOAT x, FLOAT y, FLOAT z, FLOAT fToX, FLOAT fToY, FLOAT fToZ)
{
	Origin.x = x;
	Origin.y = y;
	Origin.z = z;
	
	Direction.x = fToX - x;
	Direction.y = fToY - y;
	Direction.z = fToZ - z;
	
	Normalize(Direction, Direction);
}

CRay::~CRay()
{
}

bool CRay::HitTriangle(const FLOAT3& a, 
					   const FLOAT3& b, 
					   const FLOAT3& c, 
					   LPFLOAT3 pHitPoint, 
					   bool* pbIsHitOn, 
					   bool* pbIsFront, 
					   PFLOAT pU, 
					   PFLOAT pV,
					   PFLOAT pT)const
{
	CVector3 ba(b.x - a.x, b.y - a.y, b.z - a.z);
	CVector3 ca(c.x - a.x, c.y - a.y, c.z - a.z);
	CVector3 Temp;
	Temp.CrossProduct(Direction, ca);
	FLOAT fDeterminant = ba.DotProduct(Temp);
	
	if(!fDeterminant)
		return false;
	
	CVector3 Tangent;
	if(fDeterminant > 0)
	{
		Tangent  = Origin;
		Tangent -= a;
		
		if(pbIsFront)
			*pbIsFront = true;
	}
	else
	{
		Tangent      =   a;
		Tangent     -=   Origin;
		fDeterminant = - fDeterminant;
		
		if(pbIsFront)
			*pbIsFront = false;
	}

	FLOAT u = Tangent.DotProduct(Temp);
	if(u < 0.0f || u > fDeterminant)
		return false;

	Temp.CrossProduct(Tangent, ba);
	
	FLOAT v = Temp.DotProduct(Direction);
	if(v < 0.0f || u + v > fDeterminant)
		return false;
	
	if(pbIsHitOn)
		*pbIsHitOn = u == 0.0f || v == 0.0f || v + u == fDeterminant;
	
	FLOAT fInvercefDeterminant = 1.0f / fDeterminant;
	if(pU)
		*pU = u * fInvercefDeterminant;
	
	if(pV)
		*pV = v * fInvercefDeterminant;
	
	if(pT || pHitPoint)
	{
		FLOAT t = ca.DotProduct(Temp) * fInvercefDeterminant;
		
		if(pT)
			*pT = t;
		
		if(pHitPoint)
		{
			Temp  = Direction;
			Temp *= t;
			Temp += Origin;
			
			*pHitPoint = Temp;
		}
	}
	
	return true;
}

void CRay::Transform(const MATRIX4X4& Matrix, bool bIsInverceTranspose)
{
	TransformCoord(Origin, Matrix, Origin);
	if(bIsInverceTranspose)
	{
		CMatrix4x4 Temp(Matrix);
		Temp.Invert3x4();
		Temp.Transpose();
		TransformNormal(Direction, Temp, Direction);
	}
	else
		TransformNormal(Direction, Matrix, Direction);
}