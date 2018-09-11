#pragma once

#include "AlchemyRenderGroupInterface.h"

#include "AlchemyCamera.h"
#include "AlchemyGraphicsResource.h"

namespace alchemy
{
	class CLight;
	struct IRenderMethod;
	class CShadowMapRender :
		public CSmartClass<IRenderGroup>
	{
		typedef struct  
		{
			CSceneNode* pSceneNode;
			UINT		uRenderPass;
		}SHADOWCASTER,*LPSHADOWCASTER;
	public:
		typedef enum
		{
			POINT_LIGHT_SHADOW,
			DIRECTIONAL_LIGHT_SHADOW,
			SPOT_LIGHT_SHADOW
		}SHADOWLIGHTTYPE;

		CShadowMapRender();
		virtual ~CShadowMapRender();

		bool Create(CGraphicsResource::HANDLE Light, CCamera& LightCamera, CGraphicsResource::HANDLE RenderTarget, SHADOWLIGHTTYPE ShadowType = DIRECTIONAL_LIGHT_SHADOW);

		UINT AddShadowCaster( CSceneNode& RenderNode, UINT uRenderPass );

		void RemoveShadowCaster( UINT uCasterHandle );

		void RenderAll();

		void Reset();

		void Destroy();


	private:
		const CLight*			m_pLight;

		CCamera*				m_pLightCamera;
		CPool<SHADOWCASTER>		m_ShadowCasters;

		SHADOWLIGHTTYPE				m_eShadowType;
		CGraphicsResource::HANDLE	m_ShadowMap;

		UINT					m_uRenderList;

		CRenderQueue*			m_pRenderQueue;
		CSceneManager*			m_pSceneManager;
	};
}