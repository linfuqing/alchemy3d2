#pragma once

#include "AlchemyRenderer.h"
#include "AlchemyGraphicsBuffer.h"

#include "../Math/AlchemyVector4.h"

#include "../Core/AlchemySmartClass.h"

namespace alchemy
{
	class CSceneManager;
	class CEffect;
	struct IRenderMethod;
	class CSceneNode;
	class CLight;
	class CVolumeLightShow :
		public CRenderer, public CSmartClass<CRenderQueue::ICallBackToRender>
	{
		typedef enum
		{
			DENSITY_INDEX,
			SAMPLES_NUM_INDEX,
			EXPOSURE_INDEX,
			DECAY_INDEX,

			INDEX_NUM
		};
	public:
		CVolumeLightShow();
		virtual ~CVolumeLightShow();

		bool Create(CGraphicsResource::HANDLE OccTexture, CGraphicsResource::HANDLE Light, UINT uNumSamples = 5, 
			FLOAT fDensity = 0.3f, FLOAT fExposure = 1.0f, FLOAT fDecay = 0.8f, FLOAT fScatterWeight = 0.8f );

		void Destroy();

		virtual bool ApplyForRender();

		virtual void Render(const CRenderQueue::RENDERENTRY& Entry, const CRenderQueue::RENDERKEY& Key, UINT uFlag);

	private:

		bool						m_bIsBeginRender;

		CEffect*					m_pEffect;
		IRenderMethod*				m_pRenderMethod;

		const CLight*				m_pLight;
		CMaterial*					m_pMaterial;

		CGraphicsResource::HANDLE	m_OccTexture;
		CGraphicsResource::HANDLE	m_OffScreenTexture;
		UINT						m_uUserData[2];

		CVector4			m_ScreenLightPos;

		CVector<FLOAT>		m_fNumSampleInfoes;

	protected:
		FLOAT				m_ConstantData[INDEX_NUM];
		FLOAT				m_fScatterWeight;
		FLOAT				m_fSkip;
	};
}