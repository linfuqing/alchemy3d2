#pragma once

#include "AlchemySceneNode.h"

#include "AlchemyFrustum.h"

namespace alchemy
{
	class CCamera :
		public CSceneNode
	{
	public:
		ALCHEMY_BEGIN_EVENT_ENUM
			PROJECTION_CHANGE = 0
		ALCHEMY_END_EVENT_ENUM

		typedef enum
		{
			HALF_DEPTH = 0,
			LEFT_HANDLE,
			PERSPECTIVE
		}PROJECTIONUSAGE;
		
		CCamera(void);
		~CCamera(void);
		
		FLOAT GetFOV()const;
		FLOAT GetAspect()const;
		FLOAT GetNearPlane()const;
		FLOAT GetFarPlane()const;

		const CMatrix4x4& GetViewMatrix()const;
		const CMatrix4x4& GetProjectionMatrix()const;
		const CMatrix4x4& GetViewProjectionMatrix()const;

		const CFrustum& GetFrustum()const;

		bool IsLeftHand() const;

		bool IsHalfDepth() const;

		void SetProjection(
			FLOAT fFOV, 
			FLOAT fNearPlane,
			FLOAT fFarPlane,
			FLOAT fAspect = 0,
			bool bIsLeftHand = true, 
			bool bIsHalfDepth = true);

		void SetProjection(FLOAT fAspect = 0);

		void SetClipPlane(const CPlane& WorldClipPlane);

		void ExtractFrom(const SPHERE& Sphere, const FLOAT3& Direction, bool bIsLeftHand = true, bool bIsHalfDepth = true, const FLOAT3* pScale = ALCHEMY_NULL);

		void UpdateTransform();

	protected:
		CMatrix4x4 m_ProjectionMatrix;
		CMatrix4x4 m_ViewMatrix;
		CMatrix4x4 m_ViewProjectionMatrix;

	private:
		FLOAT m_fFOV;
		FLOAT m_fAspect;
		FLOAT m_fNearPlane;
		FLOAT m_fFarPlane;
		
		bool m_bIsLeftHand;
		bool m_bIsHalfDepth;

		CFrustum m_Frustum;
	};

	ALCHEMY_INLINE FLOAT CCamera::GetFOV()const
	{
		return m_fFOV;
	}

	ALCHEMY_INLINE FLOAT CCamera::GetAspect()const
	{
		return m_fAspect;
	}

	ALCHEMY_INLINE FLOAT CCamera::GetNearPlane()const
	{
		return m_fNearPlane;
	}

	ALCHEMY_INLINE FLOAT CCamera::GetFarPlane()const
	{
		return m_fFarPlane;
	}

	ALCHEMY_INLINE const CMatrix4x4& CCamera::GetViewMatrix()const
	{
		return m_ViewMatrix;
	}

	ALCHEMY_INLINE const CMatrix4x4& CCamera::GetProjectionMatrix()const
	{
		return m_ProjectionMatrix;
	}

	ALCHEMY_INLINE const CMatrix4x4& CCamera::GetViewProjectionMatrix()const
	{
		return m_ViewProjectionMatrix;
	}

	ALCHEMY_INLINE const CFrustum& CCamera::GetFrustum()const
	{
		return m_Frustum;
	}

	ALCHEMY_INLINE bool CCamera::IsLeftHand() const
	{
		return m_bIsLeftHand;
	}

	ALCHEMY_INLINE bool CCamera::IsHalfDepth() const
	{
		return m_bIsHalfDepth;
	}
}