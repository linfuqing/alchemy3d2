#pragma once

#include "AlchemySimplePostProcessEffect.h"

namespace alchemy
{
	class CContortEffect : 
		public CSimplePostProcessEffect
	{
		typedef enum
		{
			EFFECT_SCALE,
			EFFECT_RADIUS,
			EFFECT_TIME,

			EFFECT_PARAMETER_COUNT
		}EFFECT_PARAMETER;

	public:
		CContortEffect();
		virtual ~CContortEffect();

		bool Create(FLOAT fScale = 10.0f, FLOAT fRadius = 0.01f);

		void Destroy();

	protected:
		virtual CGraphicsResource::HANDLE GetEffectHandle();
		virtual void OnEffectUpdate( UINT uFlag, CGraphicsResource::HANDLE uSceneTarget);

	private:
		UINT				m_uEffectParameters[EFFECT_PARAMETER_COUNT];
		FLOAT				m_fParameterValues[EFFECT_PARAMETER_COUNT];
		CEffect*			m_pEffect;
		CSceneManager*		m_pSceneManager;
	};
}