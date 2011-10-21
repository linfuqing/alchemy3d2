#pragma once

#include "AlchemyEffectInfoInstance.h"
#include "AlchemyEffectCode.h"

namespace alchemy
{
	class CEffect;
	class CSceneObject;
	class CSceneManager;
	class CShadowMapDisplayEffect;

	class CShadowMapDisplayEffectInfo :
		public CEffectInfoInstance
	{
	public:
		CShadowMapDisplayEffectInfo();
		virtual ~CShadowMapDisplayEffectInfo();

		bool Create(UINT uLightIndex, UINT uSamplerIndex, CEffectCode::CPassDefiner& PassDefiner);

		virtual void OnPassLogic(CSceneObject* pSceneObject);

		virtual void OnAddToProgram();

	protected:
		CEffect*					m_pEffect;
		UINT						m_uSamplerIndex;
		FLOAT						m_fEpsilon;
		CGraphicsResource::HANDLE	m_hShadowMap;
		CSceneManager*				m_pSceneManager;

		CShadowMapDisplayEffect*	m_pShadowDisplayEffect;
	};
}