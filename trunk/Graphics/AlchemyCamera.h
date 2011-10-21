#pragma once

#include "AlchemySceneNode.h"

#include "AlchemyFrustum.h"

namespace alchemy
{
	class CCamera :
		public CSceneNode
	{
	public:
		CCamera(void);
		~CCamera(void);

		FLOAT GetFOV()const;
		FLOAT GetAspect()const;
		FLOAT GetNearPlane()const;
		FLOAT GetFarPlane()const;

		const CMatrix4x4& GetViewMatrix()const;
		const CMatrix4x4& GetProjectionMatrix()const;
		const CMatrix4x4& GetViewProjectionMatrix()const;

		void SetProjection(
			FLOAT fFOV, 
			FLOAT fNearPlane,
			FLOAT fFarPlane,
			FLOAT fAspect = 0);

		void SetProjection();

		void SetClipPlane(const CPlane& WorldClipPlane);

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
}