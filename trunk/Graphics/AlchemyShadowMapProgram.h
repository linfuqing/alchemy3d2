#pragma once

#include "AlchemyProgramInstance.h"
#include "AlchemyEffectCode.h"

namespace alchemy
{
	class CLight;
	class CPositionEffect;
	class CDepthEffect;

	class CShadowMapProgram :
		public CProgramInstance
	{
	public:
		CShadowMapProgram();
		virtual ~CShadowMapProgram();

		virtual CGraphicsResource::HANDLE GetRenderTarget() const;

		bool Create(UINT uLightIndex, CEffectCode::CPassDefiner& PassDefiner);

		virtual bool OnPreRenderSceneObject(CSceneObject* pSceneObject, UINT uFlag);
		//virtual void OnEndRenderSceneObject(CSceneObject* pSceneObject, UINT uFlag);

		virtual void OnAddedToSceneObject();

	protected:
		CLight*						m_pLight;
		CPositionEffect*			m_pPosEffect;
		CDepthEffect*				m_pDepthEffect;
		//CCamera*					m_pOldCamera;
		//CGraphicsResource::HANDLE	m_hOldRenderTarget;
	};
}