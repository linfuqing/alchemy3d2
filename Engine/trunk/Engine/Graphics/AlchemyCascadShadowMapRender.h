#pragma once

#include "AlchemyRenderGroupInterface.h"
#include "AlchemyCamera.h"

#include "AlchemyGraphicsResource.h"

#include "../Core/AlchemyVector.h"

namespace alchemy
{
	class CLight;
	class CSceneManager;

	struct IRenderMethod;

	class CCascadShadowMapRender : 
		public CSmartClass<IRenderGroup>
	{
		typedef struct  
		{
			CSceneNode*					pSceneNode;
			UINT						uRenderPass;
		}SHADOWCASTER,*LPSHADOWCASTER;

	public:
		CCascadShadowMapRender();
		virtual ~CCascadShadowMapRender();

		bool Create(CGraphicsResource::HANDLE Light, CCamera** ppSplitCameras, CGraphicsResource::HANDLE* pSplitTextures, FLOAT* pDepthSegment, UINT uSplitNum = 4, FLOAT FrustumScale = 1.0f);

		UINT AddShadowCaster(CSceneNode& Node, UINT uRenderPass);

		void RemoveShadowCaster(UINT uCasterHandle);

		const FLOAT* GetDepthSegment(UINT* pSplitNum = ALCHEMY_NULL) const;

		const CGraphicsResource::HANDLE* GetSplitTextures(UINT* pSplitNum) const;

		void Reset();

		void RenderAll();

		void Destroy();
	private:
		UINT						m_uSplitNum;

		FLOAT						m_fFrustumScale;
		CCamera**					m_ppSplitCameras;
		FLOAT*						m_pDepthSegment;
		CGraphicsResource::HANDLE*	m_pSplitTextures;

		CVector<SHADOWCASTER>		m_ShadowCasters;

		const CLight*				m_pLight;
		CCamera*					m_pSceneCamera;

		UINT*						m_pRenderLists;
		CRenderQueue*				m_pRenderQueue;
		CSceneManager*				m_pSceneManager;
	};

	ALCHEMY_INLINE const FLOAT* CCascadShadowMapRender::GetDepthSegment(UINT* pSplitNum) const
	{
		if(pSplitNum)
			*pSplitNum = m_uSplitNum;

		return m_pDepthSegment;
	}

	ALCHEMY_INLINE const CGraphicsResource::HANDLE* CCascadShadowMapRender::GetSplitTextures(UINT* pSplitNum) const
	{
		if(pSplitNum)
			*pSplitNum = m_uSplitNum;

		return m_pSplitTextures;
	}
}