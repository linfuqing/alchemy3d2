#pragma once

#include "AlchemyBoundingVolumeInterface.h"

#include "../Core/AlchemySmartClass.h"

namespace alchemy
{
	class CAxisAlignedBoundingBox :
		public CSmartClass<IBoundingVolume>
	{
	public:
		CAxisAlignedBoundingBox(void);
		~CAxisAlignedBoundingBox(void);

		void Union(const FLOAT3& Point);
		void SetTransform(const MATRIX4X4& Matrix);
		bool TestHit(const BOUNDINGBOX& Box)const;

		TESTPLANESTATE TestPlane(const FLOAT4& Plane)const;

		void GetBoundingBox(BOUNDINGBOX& BoundingBox)const;

		IBoundingVolume* Clone(IBoundingVolume* pSource)const;

	private:
		BOUNDINGBOX m_Local;
		BOUNDINGBOX m_World;

		bool m_bIsDirty;
	};

	ALCHEMY_INLINE void CAxisAlignedBoundingBox::Union(const FLOAT3& Point)
	{
		if(m_bIsDirty)
		{
			Copy(m_Local.Max, Point);
			Copy(m_Local.Min, Point);

			m_bIsDirty = false;
		}
		else
			alchemy::Union(m_Local, Point, m_Local);
	}

	ALCHEMY_INLINE bool CAxisAlignedBoundingBox::TestHit(const BOUNDINGBOX& BoundingBox)const
	{
		BOUNDINGBOX Result;

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