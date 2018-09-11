#pragma once

#include "AlchemyRenderer.h"
#include "AlchemyRenderQueue.h"

namespace alchemy
{
	class CPostProcessRender :
		public CRenderer
	{
	public:
		CPostProcessRender();
		~CPostProcessRender();

		//need next render target
		bool Create(CGraphicsResource::HANDLE Material, IRenderMethod& RenderMethod, CGraphicsResource::HANDLE Effect, UINT uRenderPass = 0,
			CGraphicsResource::HANDLE OutputRenderTarget = 0, CGraphicsResource::HANDLE ScreenMesh = 0, CRenderQueue::RENDERENTRYTYPE RenderType = CRenderQueue::FULL_SCREEN);

		//first render target - need scene render target
		bool Create(CGraphicsResource::HANDLE Material, UINT uRenderTargetIndex, IRenderMethod& RenderMethod, CGraphicsResource::HANDLE Effect, UINT uRenderPass = 0,
			CGraphicsResource::HANDLE OutputRenderTarget = 0, CGraphicsResource::HANDLE ScreenMesh = 0, CRenderQueue::RENDERENTRYTYPE RenderType = CRenderQueue::FULL_SCREEN);

		void Destroy();

		virtual bool ApplyForRender();

	private:
		UINT								m_uRenderData;
		CGraphicsResource::HANDLE			m_OutputRenderTarget;

		UINT								m_uRenderPass;
		CEffect*							m_pEffect;
		IRenderMethod*						m_pRenderMethod;
		CMesh*								m_pMesh;
		CMaterial*							m_pMaterial;
		CRenderQueue::RENDERENTRYTYPE		m_eRenderType;
	};
}