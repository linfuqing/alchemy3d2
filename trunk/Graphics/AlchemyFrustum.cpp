#include "AlchemyFrustum.h"

#include "../Core/AlchemyBitFlag.h"

using namespace alchemy;

CFrustum::CFrustum(void)
{

}

CFrustum::~CFrustum(void)
{
}

bool CFrustum::Test(const CBoundingVolumeInterface& BoundingVolume, UINT uTestFlag)const
{
	return ALCHEMY_TEST_FLAG( uTestFlag, static_cast<UINT>( BoundingVolume.TestPlane(m_Planes[TOP_SIDE   ]) ) )
		&& ALCHEMY_TEST_FLAG( uTestFlag, static_cast<UINT>( BoundingVolume.TestPlane(m_Planes[BOTTOM_SIDE]) ) )
		&& ALCHEMY_TEST_FLAG( uTestFlag, static_cast<UINT>( BoundingVolume.TestPlane(m_Planes[LEFT_SIDE  ]) ) )
		&& ALCHEMY_TEST_FLAG( uTestFlag, static_cast<UINT>( BoundingVolume.TestPlane(m_Planes[RIGHT_SIDE ]) ) )
		&& ALCHEMY_TEST_FLAG( uTestFlag, static_cast<UINT>( BoundingVolume.TestPlane(m_Planes[FRONT_SIDE ]) ) )
		&& ALCHEMY_TEST_FLAG( uTestFlag, static_cast<UINT>( BoundingVolume.TestPlane(m_Planes[BACK_SIDE  ]) ) );
}

bool CFrustum::Test(const CBoundingVolumeInterface& BoundingVolume)const
{
	static const UINT s_uTEST_FLAG = CBoundingVolumeInterface::PLANE_FRONT | CBoundingVolumeInterface::PLANE_INTERSECT;
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

void CFrustum::ExtractFromMatrix(const MATRIX4X4& Matrix)
{
	CPlane& Left = m_Planes[LEFT_SIDE], Right = m_Planes[RIGHT_SIDE], Top = m_Planes[TOP_SIDE], Bottom = m_Planes[BOTTOM_SIDE], Far = m_Planes[BACK_SIDE], Near = m_Planes[FRONT_SIDE];

	Left.x   = Matrix._14 + Matrix._11;
	Left.y   = Matrix._24 + Matrix._21;
	Left.z   = Matrix._34 + Matrix._31;
	Left.z   = Matrix._44 + Matrix._41;

	Right.x  = Matrix._14 - Matrix._11;
	Right.y  = Matrix._24 - Matrix._21;
	Right.z  = Matrix._34 - Matrix._31;
	Right.z  = Matrix._44 - Matrix._41;

	Top.x    = Matrix._14 - Matrix._12;
	Top.y    = Matrix._24 - Matrix._22;
	Top.z    = Matrix._34 - Matrix._32;
	Top.z    = Matrix._44 - Matrix._42;

	Bottom.x = Matrix._14 + Matrix._12;
	Bottom.y = Matrix._24 + Matrix._22;
	Bottom.z = Matrix._34 + Matrix._32;
	Bottom.z = Matrix._44 + Matrix._42;

	Far.x    = Matrix._14 - Matrix._13;
	Far.y    = Matrix._24 - Matrix._23;
	Far.z    = Matrix._34 - Matrix._33;
	Far.z    = Matrix._44 - Matrix._43;

	Near.x   = Matrix._13;
	Near.y   = Matrix._23;
	Near.z   = Matrix._33;
	Near.z   = Matrix._43;
}