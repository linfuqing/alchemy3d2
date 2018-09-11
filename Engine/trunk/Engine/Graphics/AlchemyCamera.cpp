#include "AlchemyCamera.h"
#include "../Math/AlchemyMath.h"
#include "../Math/AlchemyVector4.h"

using namespace alchemy;

CCamera::CCamera(void) :
m_fFOV(0.0f),
m_fAspect(0.0f),
m_fNearPlane(0.0f),
m_fFarPlane(0.0f),
m_bIsLeftHand(false), 
m_bIsHalfDepth(false)
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
			FLOAT fAspect,
			bool bIsLeftHand, 
			bool bIsHalfDepth)
{
	m_ProjectionMatrix.Projection( fFOV * g_fDEG2RAD, fAspect, fNearPlane, fFarPlane, bIsLeftHand, bIsHalfDepth);

	m_fFOV            = fFOV;
	m_fNearPlane      = fNearPlane;
	m_fFarPlane       = fFarPlane;
	m_fAspect         = fAspect;
	m_bIsLeftHand     = bIsLeftHand;
	m_bIsHalfDepth    = bIsHalfDepth;

	m_bIsTransformDirty = true;

	UINT uEventType;
	if( ALCHEMY_GET_EVENT_TYPE(uEventType, PROJECTION_CHANGE, CCamera) )
	{
		CEvent Event(uEventType);
		DispatchEvent(Event);
	}
}

void CCamera::SetProjection(FLOAT fAspect)
{
	m_fAspect = fAspect ? fAspect : m_fAspect;

	m_ProjectionMatrix.Projection(m_fFOV * g_fDEG2RAD, m_fAspect, m_fNearPlane, m_fFarPlane, m_bIsLeftHand, m_bIsHalfDepth);
	
	m_bIsTransformDirty = true;
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
	
	if(!m_bIsHalfDepth)
	{
		ClipPlane   *= 1.0f / ClipPlane.z;
		ClipPlane.w -= 1.0f;
	}

	ClipProjectionMatrix.Identity();

    ClipProjectionMatrix._13 = ClipPlane.x;
    ClipProjectionMatrix._23 = ClipPlane.y;
    ClipProjectionMatrix._33 = ClipPlane.z;
    ClipProjectionMatrix._43 = ClipPlane.w;

    m_ProjectionMatrix *= ClipProjectionMatrix;

	m_bIsTransformDirty = true;
}

void CCamera::ExtractFrom(const SPHERE& Sphere, const FLOAT3& Direction, bool bIsLeftHand, bool bIsHalfDepth, const FLOAT3* pScale)
{
	if(pScale)
		SetScale(*pScale);
	
	SetDirection(Direction);
	
	CVector3 Position(Direction);
	Position *= Sphere.fRadius;
	
	Position -= Sphere.Origin;
	Position.Negate();
	
	SetTranslation(Position);

	m_fFOV            = 0;
	m_fNearPlane      = 0.0f;
	m_fFarPlane       = Sphere.fRadius * 2.0f;
	m_fAspect         = 1.0f;
	m_bIsLeftHand     = bIsLeftHand;
	m_bIsHalfDepth    = bIsHalfDepth;

	if(m_bIsHalfDepth)
		m_ProjectionMatrix.OrthoOffCenterLeftHandHalfDepth(Sphere.fRadius, - Sphere.fRadius, - Sphere.fRadius, Sphere.fRadius, 0, m_fFarPlane);
	else
		m_ProjectionMatrix.OrthoOffCenterLeftHandFullDepth(Sphere.fRadius, - Sphere.fRadius, - Sphere.fRadius, Sphere.fRadius, 0, m_fFarPlane);
}

void CCamera::UpdateTransform()
{
	CSceneNode::UpdateTransform();

	m_ViewMatrix = CSceneNode::GetWorldMatrix();

	m_ViewMatrix.Invert3x4();

	m_ViewProjectionMatrix = m_ViewMatrix;

	m_ViewProjectionMatrix *= m_ProjectionMatrix;

	m_Frustum.ExtractFromMatrix(m_ViewProjectionMatrix, m_bIsHalfDepth);
}