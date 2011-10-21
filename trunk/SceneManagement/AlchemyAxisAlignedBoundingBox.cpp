#include "AlchemyAxisAlignedBoundingBox.h"

#include "../Math/AlchemyVector3.h"

using namespace alchemy;

CAxisAlignedBoundingBox::CAxisAlignedBoundingBox(void)
{
	Zero(m_Local);

	Copy(m_World, m_Local);
}

CAxisAlignedBoundingBox::~CAxisAlignedBoundingBox(void)
{
}

void CAxisAlignedBoundingBox::SetTransform(const MATRIX4X4& Matrix)
{
	static CVector3 s_Min, s_LegX, s_LegY, s_LegZ;

	s_Min.Set(m_Local.Min.x, m_Local.Min.y, m_Local.Min.z);
	s_LegX.Set(m_Local.Max.x, m_Local.Min.y, m_Local.Min.z);
	s_LegY.Set(m_Local.Min.x, m_Local.Max.y, m_Local.Min.z);
	s_LegZ.Set(m_Local.Min.x, m_Local.Min.y, m_Local.Max.z);

	s_Min.TransformCoord(Matrix);

	s_LegX.TransformCoord(Matrix);
	s_LegY.TransformCoord(Matrix);
	s_LegZ.TransformCoord(Matrix);

	s_LegX -= s_Min;
	s_LegY -= s_Min;
	s_LegZ -= s_Min;

	static CFloat3 s_Max;
	s_Max  = s_Min;
	s_Max += s_LegX;
	s_Max += s_LegY;
	s_Max += s_LegZ;

	m_Local.Max.x = s_Max.x;
	m_Local.Max.y = s_Max.y;
	m_Local.Max.z = s_Max.z;

	m_Local.Min.x = s_Min.x;
	m_Local.Min.y = s_Min.y;
	m_Local.Min.z = s_Min.z;

	s_Max  = s_Min;
	s_Max += s_LegX;
	Union(s_Max);

	s_Max  = s_Min;
	s_Max += s_LegY;
	Union(s_Max);

	s_Max  = s_Min;
	s_Max += s_LegZ;
	Union(s_Max);

	s_Max  = s_Min;
	s_Max += s_LegX;
	s_Max += s_LegY;
	Union(s_Max);

	s_Max  = s_Min;
	s_Max += s_LegY;
	s_Max += s_LegZ;
	Union(s_Max);

	s_Max  = s_Min;
	s_Max += s_LegZ;
	s_Max += s_LegX;
	Union(s_Max);
}

CBoundingVolumeInterface::TESTPLANESTATE CAxisAlignedBoundingBox::TestPlane(const FLOAT4& Plane)const
{
	static FLOAT3 Max, Min;

	if(Plane.a > 0.0f)
	{
		Max.x = m_World.Max.x;
		Min.x = m_World.Min.x;
	}
	else
	{
		Max.x = m_World.Min.x;
		Min.x = m_World.Max.x;
	}

	if(Plane.b > 0.0f)
	{
		Max.y = m_World.Max.y;
		Min.y = m_World.Min.y;
	}
	else
	{
		Max.y = m_World.Min.y;
		Min.y = m_World.Max.y;
	}

	if(Plane.z > 0.0f)
	{
		Max.z = m_World.Max.z;
		Min.z = m_World.Min.z;
	}
	else
	{
		Max.z = m_World.Min.z;
		Min.z = m_World.Max.z;
	}

	FLOAT fMax = DotProductCoord(Plane, Max), fMin = DotProductCoord(Plane, Min);

	if(fMin == 0.0f || fMax * fMin < 0.0f)
		return PLANE_INTERSECT;
	else if(fMin > 0.0f)
		return PLANE_FRONT;

	return PLANE_BACK;
}