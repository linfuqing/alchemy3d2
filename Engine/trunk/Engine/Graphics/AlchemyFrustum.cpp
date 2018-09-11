#include "AlchemyFrustum.h"

#include "../Core/AlchemyBitFlag.h"

using namespace alchemy;

CFrustum::CFrustum(void)
{

}

CFrustum::~CFrustum(void)
{
}

bool CFrustum::Test(const IBoundingVolume& BoundingVolume, UINT uTestFlag)const
{
	return ALCHEMY_TEST_FLAG( uTestFlag, static_cast<UINT>( BoundingVolume.TestPlane(m_Planes[TOP_SIDE   ]) ) )
		&& ALCHEMY_TEST_FLAG( uTestFlag, static_cast<UINT>( BoundingVolume.TestPlane(m_Planes[BOTTOM_SIDE]) ) )
		&& ALCHEMY_TEST_FLAG( uTestFlag, static_cast<UINT>( BoundingVolume.TestPlane(m_Planes[LEFT_SIDE  ]) ) )
		&& ALCHEMY_TEST_FLAG( uTestFlag, static_cast<UINT>( BoundingVolume.TestPlane(m_Planes[RIGHT_SIDE ]) ) )
		&& ALCHEMY_TEST_FLAG( uTestFlag, static_cast<UINT>( BoundingVolume.TestPlane(m_Planes[FRONT_SIDE ]) ) )
		&& ALCHEMY_TEST_FLAG( uTestFlag, static_cast<UINT>( BoundingVolume.TestPlane(m_Planes[BACK_SIDE  ]) ) );
}

bool CFrustum::Test(const IBoundingVolume& BoundingVolume)const
{
	static const UINT s_uTEST_FLAG = IBoundingVolume::PLANE_FRONT | IBoundingVolume::PLANE_INTERSECT;
	return Test(BoundingVolume, s_uTEST_FLAG);
}

void CFrustum::Transform(const MATRIX4X4& Matrix)
{
	m_Planes[TOP_SIDE   ].Transform(Matrix);
	m_Planes[BOTTOM_SIDE].Transform(Matrix);
	m_Planes[LEFT_SIDE  ].Transform(Matrix);
	m_Planes[RIGHT_SIDE ].Transform(Matrix);
	m_Planes[FRONT_SIDE ].Transform(Matrix);
	m_Planes[BACK_SIDE  ].Transform(Matrix);
}

void CFrustum::Normalize()
{
	m_Planes[TOP_SIDE   ].Normalize();
	m_Planes[BOTTOM_SIDE].Normalize();
	m_Planes[LEFT_SIDE  ].Normalize();
	m_Planes[RIGHT_SIDE ].Normalize();
	m_Planes[FRONT_SIDE ].Normalize();
	m_Planes[BACK_SIDE  ].Normalize();
}

void CFrustum::ExtractFromMatrix(const MATRIX4X4& Matrix, bool bIsHalfDepth)
{
	CPlane &Left = m_Planes[LEFT_SIDE], &Right = m_Planes[RIGHT_SIDE], &Top = m_Planes[TOP_SIDE], &Bottom = m_Planes[BOTTOM_SIDE], &Far = m_Planes[BACK_SIDE], &Near = m_Planes[FRONT_SIDE];

	Left.x   = Matrix._14 + Matrix._11;
	Left.y   = Matrix._24 + Matrix._21;
	Left.z   = Matrix._34 + Matrix._31;
	Left.w   = Matrix._44 + Matrix._41;

	Right.x  = Matrix._14 - Matrix._11;
	Right.y  = Matrix._24 - Matrix._21;
	Right.z  = Matrix._34 - Matrix._31;
	Right.w  = Matrix._44 - Matrix._41;

	Top.x    = Matrix._14 - Matrix._12;
	Top.y    = Matrix._24 - Matrix._22;
	Top.z    = Matrix._34 - Matrix._32;
	Top.w    = Matrix._44 - Matrix._42;

	Bottom.x = Matrix._14 + Matrix._12;
	Bottom.y = Matrix._24 + Matrix._22;
	Bottom.z = Matrix._34 + Matrix._32;
	Bottom.w = Matrix._44 + Matrix._42;

	Far.x    = Matrix._14 - Matrix._13;
	Far.y    = Matrix._24 - Matrix._23;
	Far.z    = Matrix._34 - Matrix._33;
	Far.w    = Matrix._44 - Matrix._43;

	if(bIsHalfDepth)
	{
		Near.x   = Matrix._13;
		Near.y   = Matrix._23;
		Near.z   = Matrix._33;
		Near.w   = Matrix._43;
	}
	else
	{
		Near.x   = Matrix._14 + Matrix._13;
		Near.y   = Matrix._24 + Matrix._23;
		Near.z   = Matrix._34 + Matrix._33;
		Near.w   = Matrix._44 + Matrix._43;
	}

	m_InverceMatrix = Matrix;
	
	if( m_InverceMatrix.Invert4x4() )
	{
		if(bIsHalfDepth)
		{
			static const CVector3 s_TOP_LEFT_FRONT_POINT(- 1.0f, 1.0f, 0.0f);
			static const CVector3 s_BOTTOM_LEFT_FRONT_POINT(- 1.0f, - 1.0f, 0.0f);
			static const CVector3 s_TOP_RIGHT_FRONT_POINT(1.0f, 1.0f, 0.0f);
			static const CVector3 s_BOTTOM_RIGHT_FRONT_POINT(1.0f, - 1.0f, 0.0f);
			
			TransformCoord(m_Points[TOP_LEFT_FRONT_POINT], m_InverceMatrix, s_TOP_LEFT_FRONT_POINT);
			TransformCoord(m_Points[BOTTOM_LEFT_FRONT_POINT], m_InverceMatrix, s_BOTTOM_LEFT_FRONT_POINT);
			TransformCoord(m_Points[TOP_RIGHT_FRONT_POINT], m_InverceMatrix, s_TOP_RIGHT_FRONT_POINT);
			TransformCoord(m_Points[BOTTOM_RIGHT_FRONT_POINT], m_InverceMatrix, s_BOTTOM_RIGHT_FRONT_POINT);
		}
		else
		{
			static const CVector3 s_TOP_LEFT_FRONT_POINT(- 1.0f, 1.0f, - 1.0f);
			static const CVector3 s_BOTTOM_LEFT_FRONT_POINT(- 1.0f, - 1.0f, - 1.0f);
			static const CVector3 s_TOP_RIGHT_FRONT_POINT(1.0f, 1.0f, - 1.0f);
			static const CVector3 s_BOTTOM_RIGHT_FRONT_POINT(1.0f, - 1.0f, - 1.0f);

			TransformCoord(m_Points[TOP_LEFT_FRONT_POINT], m_InverceMatrix, s_TOP_LEFT_FRONT_POINT);
			TransformCoord(m_Points[BOTTOM_LEFT_FRONT_POINT], m_InverceMatrix, s_BOTTOM_LEFT_FRONT_POINT);
			TransformCoord(m_Points[TOP_RIGHT_FRONT_POINT], m_InverceMatrix, s_TOP_RIGHT_FRONT_POINT);
			TransformCoord(m_Points[BOTTOM_RIGHT_FRONT_POINT], m_InverceMatrix, s_BOTTOM_RIGHT_FRONT_POINT);
		}

		static const CVector3 s_TOP_LEFT_BACK_POINT(- 1.0f, 1.0f, 1.0f);
		static const CVector3 s_BOTTOM_LEFT_BACK_POINT(- 1.0f, - 1.0f, 1.0f);
		static const CVector3 s_TOP_RIGHT_BACK_POINT(1.0f, 1.0f, 1.0f);
		static const CVector3 s_BOTTOM_RIGHT_BACK_POINT(1.0f, - 1.0f, 1.0f);

		TransformCoord(m_Points[TOP_LEFT_BACK_POINT], m_InverceMatrix, s_TOP_LEFT_BACK_POINT);
		TransformCoord(m_Points[BOTTOM_LEFT_BACK_POINT], m_InverceMatrix, s_BOTTOM_LEFT_BACK_POINT);
		TransformCoord(m_Points[TOP_RIGHT_BACK_POINT], m_InverceMatrix, s_TOP_RIGHT_BACK_POINT);
		TransformCoord(m_Points[BOTTOM_RIGHT_BACK_POINT], m_InverceMatrix, s_BOTTOM_RIGHT_BACK_POINT);
	}
}

void CFrustum::GetBoundingBox(BOUNDINGBOX& BoundingBox)const
{
	Copy(BoundingBox.Min, m_Points[TOP_LEFT_FRONT_POINT]);
	Copy(BoundingBox.Max, BoundingBox.Min);
	
	Union(BoundingBox, m_Points[BOTTOM_LEFT_FRONT_POINT], BoundingBox);
	Union(BoundingBox, m_Points[TOP_RIGHT_FRONT_POINT], BoundingBox);
	Union(BoundingBox, m_Points[BOTTOM_RIGHT_FRONT_POINT], BoundingBox);
	Union(BoundingBox, m_Points[TOP_LEFT_BACK_POINT], BoundingBox);
	Union(BoundingBox, m_Points[BOTTOM_LEFT_BACK_POINT], BoundingBox);
	Union(BoundingBox, m_Points[TOP_RIGHT_BACK_POINT], BoundingBox);
	Union(BoundingBox, m_Points[BOTTOM_RIGHT_BACK_POINT], BoundingBox);
}

void CFrustum::GetBoundingBox(BOUNDINGBOX& BoundingBox, FLOAT fFarRatio, FLOAT fNearRatio)const
{
	const CVector3& TopLeft = m_Points[TOP_LEFT_FRONT_POINT], 
				 & BottomLeft = m_Points[BOTTOM_LEFT_FRONT_POINT], 
				 & TopRight = m_Points[TOP_RIGHT_FRONT_POINT], 
				 & BottomRight = m_Points[BOTTOM_RIGHT_FRONT_POINT];
	
	static CVector3 Temp;
	if(fNearRatio)
	{
		static CVector3 Point;
		
		FLOAT fDistanceRatio = fFarRatio / fNearRatio;
		
		Temp   = m_Points[TOP_LEFT_BACK_POINT];
		Temp  -= TopLeft;
		
		Temp  *= fNearRatio;
		Point  = TopLeft;
		Point += Temp;
		Copy(BoundingBox.Min, Point);
		Copy(BoundingBox.Max, Point);
		
		Temp *= fDistanceRatio;
		Point  = TopLeft;
		Point += Temp;
		Union(BoundingBox, Point, BoundingBox);
		
		Temp  = m_Points[BOTTOM_LEFT_BACK_POINT];
		Temp -= BottomLeft;
		
		Temp  *= fNearRatio;
		Point  = BottomLeft;
		Point += Temp;
		Union(BoundingBox, Point, BoundingBox);
		
		Temp *= fDistanceRatio;
		Point  = BottomLeft;
		Point += Temp;
		Union(BoundingBox, Point, BoundingBox);
		
		Temp  = m_Points[TOP_RIGHT_BACK_POINT];
		Temp -= TopRight;
		
		Temp  *= fNearRatio;
		Point  = TopRight;
		Point += Temp;
		Union(BoundingBox, Point, BoundingBox);
		
		Temp *= fDistanceRatio;
		Point  = TopRight;
		Point += Temp;
		Union(BoundingBox, Point, BoundingBox);
		
		Temp  = m_Points[BOTTOM_RIGHT_BACK_POINT];
		Temp -= BottomRight;
		
		Temp  *= fNearRatio;
		Point  = BottomRight;
		Point += Temp;
		Union(BoundingBox, Point, BoundingBox);
		
		Temp *= fDistanceRatio;
		Point  = BottomRight;
		Point += Temp;
		Union(BoundingBox, Point, BoundingBox);
	}
	else
	{
		Copy(BoundingBox.Min, TopLeft);
		Copy(BoundingBox.Max, TopLeft);
		
		Temp  = m_Points[TOP_LEFT_BACK_POINT];
		Temp -= TopLeft;
		Temp *= fFarRatio;
		Temp += TopLeft;
		Union(BoundingBox, Temp, BoundingBox);
		
		
		Union(BoundingBox, BottomLeft, BoundingBox);
		
		Temp  = m_Points[BOTTOM_LEFT_BACK_POINT];
		Temp -= BottomLeft;
		Temp *= fFarRatio;
		Temp += BottomLeft;
		Union(BoundingBox, Temp, BoundingBox);
		
		Union(BoundingBox, TopRight, BoundingBox);
		
		Temp  = m_Points[TOP_RIGHT_BACK_POINT];
		Temp -= TopRight;
		Temp *= fFarRatio;
		Temp += TopRight;
		Union(BoundingBox, Temp, BoundingBox);
		
		Union(BoundingBox, BottomRight, BoundingBox);
		
		Temp  = m_Points[BOTTOM_RIGHT_BACK_POINT];
		Temp -= BottomRight;
		Temp *= fFarRatio;
		Temp += BottomRight;
		Union(BoundingBox, Temp, BoundingBox);
	}
}

void CFrustum::GetSphere(SPHERE& Sphere)const
{
	CBoundingBox BoundingBox;
	GetBoundingBox(BoundingBox);
	
	CVector3 Point( Center(Sphere.Origin, BoundingBox) );
	Point -= BoundingBox.Min;
	Sphere.fRadius = Point.GetLength();
}

void CFrustum::GetSphere(SPHERE& Sphere, FLOAT fFarRatio, FLOAT fNearRatio)const
{
	CBoundingBox BoundingBox;
	GetBoundingBox(BoundingBox, fFarRatio, fNearRatio);
	
	CVector3 Point( Center(Sphere.Origin, BoundingBox) );
	Point -= BoundingBox.Min;
	Sphere.fRadius = Point.GetLength();
}