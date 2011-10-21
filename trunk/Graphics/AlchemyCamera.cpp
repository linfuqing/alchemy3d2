#include "AlchemyCamera.h"
#include "../Math/AlchemyMath.h"

using namespace alchemy;

CCamera::CCamera(void) :
m_fFOV(0.0f),
m_fAspect(0.0f),
m_fNearPlane(0.0f),
m_fFarPlane(0.0f)
{
	m_ProjectionMatrix.Identity();
	m_ViewMatrix.Identity();
	m_ViewProjectionMatrix.Identity();
}

CCamera::~CCamera(void)
{
}

void CCamera::SetProjection(
			FLOAT fFOV, 
			FLOAT fNearPlane,
			FLOAT fFarPlane,
			FLOAT fAspect)
{
	m_ProjectionMatrix.PerspectiveFieldOfViewLeftHand(fFOV*g_fDEG2RAD, fAspect, fNearPlane, fFarPlane);

	m_fFOV       = fFOV;
	m_fNearPlane = fNearPlane;
	m_fFarPlane  = fFarPlane;
	m_fAspect    = fAspect;

	m_bIsTransformDirty = true;
}

void CCamera::SetProjection()
{
	SetProjection(m_fFOV, m_fNearPlane, m_fFarPlane, m_fAspect);
}

void CCamera::SetClipPlane(const CPlane& WorldClipPlane)
{
	CMatrix4x4 ClipProjectionMatrix(m_ViewProjectionMatrix);
    
	CPlane Plane(WorldClipPlane);

    Plane.Normalize();

    ClipProjectionMatrix.Invert4x4();
    ClipProjectionMatrix.Transpose();

    CPlane ClipPlane(Plane);

	ClipPlane *= ClipProjectionMatrix;

    if(ClipPlane.w == 0)
        return;

    if (ClipPlane.w > 0)
    {
		ClipPlane.x = - Plane.x;
		ClipPlane.y = - Plane.y;
		ClipPlane.z = - Plane.z;
		ClipPlane.w = - Plane.w;

        ClipPlane *= ClipProjectionMatrix;
    }

	ClipProjectionMatrix.Identity();

    ClipProjectionMatrix._13 = ClipPlane.x;
    ClipProjectionMatrix._23 = ClipPlane.y;
    ClipProjectionMatrix._33 = ClipPlane.z;
    ClipProjectionMatrix._43 = ClipPlane.w;

    m_ProjectionMatrix *= ClipProjectionMatrix;

	m_bIsTransformDirty = true;
}

void CCamera::UpdateTransform()
{
	CSceneNode::UpdateTransform();

	m_ViewMatrix = CSceneNode::GetWorldMatrix();

	m_ViewMatrix.Invert3x4();

	m_ViewProjectionMatrix = m_ViewMatrix;

	m_ViewProjectionMatrix *= m_ProjectionMatrix;

	m_Frustum.ExtractFromMatrix(m_ViewProjectionMatrix);
}