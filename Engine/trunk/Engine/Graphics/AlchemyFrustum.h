#pragma once

#include "../Core/AlchemySmartClass.h"
#include "../Math/AlchemyVector3.h"
#include "../Math/AlchemyPlane.h"
#include "../SceneManagement/AlchemyClipperInterface.h"

namespace alchemy
{
	class CFrustum :
		public CSmartClass<IClipper>
	{
	public:
		typedef enum
		{
			TOP_SIDE,
			BOTTOM_SIDE,
			LEFT_SIDE,
			RIGHT_SIDE,
			FRONT_SIDE,
			BACK_SIDE,

			TOTAL_SIDES
		}SIDE;
		
		typedef enum
		{
			TOP_LEFT_FRONT_POINT = 0,
			BOTTOM_LEFT_FRONT_POINT, 
			TOP_RIGHT_FRONT_POINT, 
			BOTTOM_RIGHT_FRONT_POINT,
			TOP_LEFT_BACK_POINT,
			BOTTOM_LEFT_BACK_POINT, 
			TOP_RIGHT_BACK_POINT, 
			BOTTOM_RIGHT_BACK_POINT,
			
			TOTAL_POINTS
		}POINT;

	public:
		CFrustum(void);
		~CFrustum(void);

		bool Test(const IBoundingVolume& BoundingVolume, UINT uTestFlag)const;

		bool Test(const IBoundingVolume& BoundingVolume)const;

		void Transform(const MATRIX4X4& Matrix);

		void ExtractFromMatrix(const MATRIX4X4& Matrix, bool bIsHalfDepth);

		void Normalize();
		
		void GetBoundingBox(BOUNDINGBOX& BoundingBox)const;
		void GetBoundingBox(BOUNDINGBOX& BoundingBox, FLOAT fFarRatio, FLOAT fNearRatio)const;
		void GetSphere(SPHERE& Sphere)const;
		void GetSphere(SPHERE& Sphere, FLOAT fFarRatio, FLOAT fNearRatio)const;

	private:
		CPlane m_Planes[TOTAL_SIDES];
		CVector3 m_Points[TOTAL_POINTS];
		
		CMatrix4x4 m_InverceMatrix;
	};
}