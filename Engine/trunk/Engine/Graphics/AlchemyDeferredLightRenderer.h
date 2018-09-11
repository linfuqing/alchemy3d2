#pragma once

#include "AlchemyRenderer.h"
#include "AlchemyNormal3Power1MapInputEffect.h"
#include "AlchemyPositionDepthRGBAMapInputEffect.h"

#include "../Math/AlchemyFloat2.h"

namespace alchemy
{
	class CDeferredLightRenderer :
		public CRenderer
	{
		class CListener :
			public CSmartClass<IEventDispatcher::IListener>
		{
		public:
			CListener(CDeferredLightRenderer& Parent);
			~CListener();

			void Handler(const IEventDispatcher::CEvent& Event);
			void Run();

		private:
			CDeferredLightRenderer& m_Parent;
		};
	public:
		/*typedef enum
		{
			NORMAL_MAP =0,
			DEPTH_MAP,
			AMBIENT_MAP,
			DIFFUSE_MAP,
			SPECULAR_MAP,
			AMBIENT_OCCLUSION_MAP,

			TOTAL_DEFERRED_LIGHT_MAP
		}DEFERREDLIGHTMAP;*/

		CDeferredLightRenderer(void);
		~CDeferredLightRenderer(void);

		void SetAORadius(FLOAT fRadius);
		void SetAOScale(FLOAT fScale);
		void SetAOIntensity(FLOAT fIntensity);

		bool Create(CGraphicsResource::HANDLE Material, CGraphicsResource::HANDLE Normal3Power1Map, CGraphicsResource::HANDLE PositionDepthMap, CGraphicsResource::HANDLE AORandomMap, UINT uNumAODirection, UINT uAOStep, CGraphicsResource::HANDLE OutputRenderTarget);
		bool Create(CGraphicsResource::HANDLE Material, CGraphicsResource::HANDLE AORandomMap = 0, UINT uNumAODirection = 0, UINT uAOStep = 0, CGraphicsResource::HANDLE OutputRenderTarget = 0);

		bool ApplyForRender();

		void Destroy();

	private:
		typedef struct
		{
			CEffect* pEffect;
			CRenderMethod* pRenderMethod;
			UINT uUserData;
		}RENDERDATA, * LPRENDERDATA;

		LPTEXTURE m_pOutputRenderTarget;
		LPTEXTURE m_pBufferRenderTarget;
		LPTEXTURE m_pNormal3Detph1Map;
		LPTEXTURE m_pPositionDepthMap;
		LPTEXTURE m_pAORandomMap;

		CMaterial* m_pMaterial;

		CVector<RENDERDATA> m_RenderData;

		CNormal3Power1MapInputEffect m_Normal3Depth1MapInputEffect;
		CPositionDepthRGBAMapInputEffect m_PositionDepthMapInputEffect;

		UINT m_uAONumDirection;
		UINT m_uAOStep;

		CFloat2 m_AOFocalLength;

		CFloat2 m_AORandomMapUVScale;
		CFloat2 m_AOSamplerRadius;
		CFloat2 m_AOInverseFocalLength;

		FLOAT m_fAORadius;
		FLOAT m_fAOScale;
		FLOAT m_fAOBias;
		FLOAT m_fAOIntensity;

		CFloat2* m_pAORotationDirection;

		CListener m_Listener;
	};

	ALCHEMY_INLINE void CDeferredLightRenderer::SetAORadius(FLOAT fRadius)
	{
		m_AOSamplerRadius  = m_AOFocalLength;
		m_AOSamplerRadius *= fRadius * 0.5f / (m_uAOStep + 1.0f);

		m_fAORadius = fRadius;
	}

	ALCHEMY_INLINE void CDeferredLightRenderer::SetAOIntensity(FLOAT fIntensity)
	{
		m_fAOIntensity = m_uAONumDirection ? fIntensity / (m_uAONumDirection * m_uAOStep) : fIntensity;
	}
}