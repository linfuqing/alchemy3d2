#include "AlchemyAxisAlignedBoundingBox.h"

#include "../Math/AlchemyVector3.h"

using namespace alchemy;

CAxisAlignedBoundingBox::CAxisAlignedBoundingBox(void) : 
m_bIsDirty(true)
{
	Zero(m_Local);

	Copy(m_World, m_Local);
}

CAxisAlignedBoundingBox::~CAxisAlignedBoundingBox(void)
{
}

void CAxisAlignedBoundingBox::SetTransform(const MATRIX4X4& Matrix)
{
	CVector3 Min, LegX, LegY, LegZ;

	Min.Set(m_Local.Min.x, m_Local.Min.y, m_Local.Min.z);
	LegX.Set(m_Local.Max.x, m_Local.Min.y, m_Local.Min.z);
	LegY.Set(m_Local.Min.x, m_Local.Max.y, m_Local.Min.z);
	LegZ.Set(m_Local.Min.x, m_Local.Min.y, m_Local.Max.z);

	Min.TransformCoord(Matrix);

	LegX.TransformCoord(Matrix);
	LegY.TransformCoord(Matrix);
	LegZ.TransformCoord(Matrix);

	LegX -= Min;
	LegY -= Min;
	LegZ -= Min;

	CVector3 Max;
	Max  = Min;
	Max += LegX;
	Max += LegY;
	Max += LegZ;

	m_World.Max.x = Max.x;
	m_World.Max.y = Max.y;
	m_World.Max.z = Max.z;

	m_World.Min.x = Min.x;
	m_World.Min.y = Min.y;
	m_World.Min.z = Min.z;

	Max  = Min;
	Max += LegX;
	alchemy::Union(m_World, Max, m_World);

	Max  = Min;
	Max += LegY;
	alchemy::Union(m_World, Max, m_World);

	Max  = Min;
	Max += LegZ;
	alchemy::Union(m_World, Max, m_World);

	Max  = Min;
	Max += LegX;
	Max += LegY;
	alchemy::Union(m_World, Max, m_World);

	Max  = Min;
	Max += LegY;
	Max += LegZ;
	alchemy::Union(m_World, Max, m_World);

	Max  = Min;
	Max += LegZ;
	Max += LegX;
	alchemy::Union(m_World, Max, m_World);
}

IBoundingVolume::TESTPLANESTATE CAxisAlignedBoundingBox::TestPlane(const FLOAT4& Plane)const
{
	FLOAT3 Max, Min;

	if(Plane.x > 0.0f)
	{
		Max.x = m_World.Max.x;
		Min.x = m_World.Min.x;
	}
	else
	{
		Max.x = m_World.Min.x;
		Min.x = m_World.Max.x;
	}

	if(Plane.y > 0.0f)
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

	if(fMax < 0.0f)
		return PLANE_BACK;
	else if(fMin > 0.0f)
		return PLANE_FRONT;

	return PLANE_INTERSECT;
}

IBoundingVolume* CAxisAlignedBoundingBox::Clone(IBoundingVolume* pSource)const
{
	if(!pSource)
	{
		ALCHEMY_DEBUG_NEW(pSource, CAxisAlignedBoundingBox);
	}

	CAxisAlignedBoundingBox* pAxisAlignedBoundingBox = dynamic_cast<CAxisAlignedBoundingBox*>(pSource);
	if(pAxisAlignedBoundingBox)
	{
		Copy(pAxisAlignedBoundingBox->m_Local, m_Local);
		Copy(pAxisAlignedBoundingBox->m_World, m_World);

		return pAxisAlignedBoundingBox;
	}

	return ALCHEMY_NULL;
}