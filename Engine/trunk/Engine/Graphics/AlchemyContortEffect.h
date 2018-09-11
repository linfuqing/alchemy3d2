#pragma once

#include "AlchemyRenderer.h"

namespace alchemy
{
	class CContortEffect : 
		public CRenderer
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
		~CContortEffect();

		bool Create(FLOAT fScale = 10.0f, FLOAT fRadius = 0.01f);

		void Destroy();

		bool ApplyForRender();
	private:
		UINT				m_uEffectParameters[EFFECT_PARAMETER_COUNT];
		FLOAT				m_fParameterValues[EFFECT_PARAMETER_COUNT];
		CEffect*			m_pEffect;
		CRenderMethod*      m_pRenderMethod;
		UINT                m_uUserData;
	};
}