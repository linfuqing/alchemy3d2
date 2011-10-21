#pragma once

#include "AlchemyBoundingBox.h"

#include "../Math/AlchemyPlane.h"

namespace alchemy
{
	class CBoundingVolumeInterface
	{
	public:
		typedef enum
		{
			PLANE_NONE      = 0,

			PLANE_FRONT     = 0x01,
			PLANE_BACK      = 0x02,
			PLANE_INTERSECT = 0x04
		}TESTPLANESTATE;

		CBoundingVolumeInterface(void) {}
		virtual ~CBoundingVolumeInterface(void) {}

		virtual void Union(const FLOAT3& Point) = 0;
		virtual void SetTransform(const MATRIX4X4& Matrix) = 0;

		virtual TESTPLANESTATE TestPlane(const FLOAT4& Plane)const = 0;
		virtual bool TestHit(const BOUNDINGBOX& Box)const = 0;

		virtual void GetBoundingBox(BOUNDINGBOX& BoundingBox)const = 0;
	};
}