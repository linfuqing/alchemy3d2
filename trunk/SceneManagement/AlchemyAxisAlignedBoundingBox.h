#pragma once

#include "AlchemyBoundingVolumeInterface.h"

#include "../Core/AlchemySmartPointer.h"
#include "../Core/AlchemyUtils.h"

namespace alchemy
{
	class CAxisAlignedBoundingBox :
		public CBoundingVolumeInterface
	{
	public:
		CAxisAlignedBoundingBox(void);
		~CAxisAlignedBoundingBox(void);

		void Union(const FLOAT3& Point);
		void SetTransform(const MATRIX4X4& Matrix);
		bool TestHit(const BOUNDINGBOX& Box)const;

		TESTPLANESTATE TestPlane(const FLOAT4& Plane)const;

		void GetBoundingBox(BOUNDINGBOX& BoundingBox)const;

	private:
		BOUNDINGBOX m_Local;
		BOUNDINGBOX m_World;
	};

	typedef CSmartPointer<CAxisAlignedBoundingBox> AABB, * LPAABB;

	ALCHEMY_INLINE void CAxisAlignedBoundingBox::Union(const FLOAT3& Point)
	{
		m_Local.Max.x = ALCHEMY_MAXINUM(Point.x, m_Local.Max.x);
		m_Local.Max.y = ALCHEMY_MAXINUM(Point.y, m_Local.Max.y);
		m_Local.Max.z = ALCHEMY_MAXINUM(Point.z, m_Local.Max.z);

		m_Local.Min.x = ALCHEMY_MININUM(Point.x, m_Local.Min.x);
		m_Local.Min.y = ALCHEMY_MININUM(Point.y, m_Local.Min.y);
		m_Local.Min.z = ALCHEMY_MININUM(Point.z, m_Local.Min.z);
	}

	ALCHEMY_INLINE bool CAxisAlignedBoundingBox::TestHit(const BOUNDINGBOX& BoundingBox)const
	{
		static BOUNDINGBOX Result;

		Result.fLeft   = ALCHEMY_MININUM(BoundingBox.fLeft  , m_World.fLeft  );
		Result.fBottom = ALCHEMY_MININUM(BoundingBox.fBottom, m_World.fBottom);
		Result.fFront  = ALCHEMY_MININUM(BoundingBox.fFront , m_World.fFront );

		Result.fRight = ALCHEMY_MAXINUM(BoundingBox.fRight, m_World.fRight);
		Result.fTop   = ALCHEMY_MAXINUM(BoundingBox.fTop  , m_World.fTop  );
		Result.fBack  = ALCHEMY_MAXINUM(BoundingBox.fBack , m_World.fBack );

		return IsValuable(Result);
	}

	ALCHEMY_INLINE void CAxisAlignedBoundingBox::GetBoundingBox(BOUNDINGBOX& BoundingBox)const
	{
		Copy(BoundingBox, m_World);
	}
}