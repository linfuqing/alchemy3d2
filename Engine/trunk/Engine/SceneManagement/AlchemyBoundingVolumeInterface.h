#pragma once

#include "AlchemyBoundingBox.h"

#include "../Core/AlchemyInterface.h"
#include "../Math/AlchemyPlane.h"
#include "../Math/AlchemySphere.h"

namespace alchemy
{
	ALCHEMY_INTERFACE(IBoundingVolume)
	{
		typedef enum
		{
			PLANE_NONE      = 0,

			PLANE_FRONT     = 0x01,
			PLANE_BACK      = 0x02,
			PLANE_INTERSECT = 0x04
		}TESTPLANESTATE;

		virtual void Union(const FLOAT3& Point) = 0;
		virtual void SetTransform(const MATRIX4X4& Matrix) = 0;

		virtual TESTPLANESTATE TestPlane(const FLOAT4& Plane)const = 0;
		virtual bool TestHit(const BOUNDINGBOX& Box)const = 0;

		virtual void GetBoundingBox(BOUNDINGBOX& BoundingBox)const = 0;

		virtual IBoundingVolume* Clone(IBoundingVolume* pSource)const  = 0;
	};
}