#include "AlchemyDeferredRenderingPipeline.h"
//#include "AlchemyViewportRenderTargetListener.h"

using namespace alchemy;

CDeferredRenderingPipeline::CDeferredRenderingPassDefiner::CDeferredRenderingPassDefiner(CEffectCode& Parent, CDeferredRenderingPipeline& Pipeline) :
CDeferredRenderingPipeline::CPassDefiner(Parent), m_Pipeline(Pipeline)
{

}

CDeferredRenderingPipeline::CDeferredRenderingPassDefiner::~CDeferredRenderingPassDefiner(void)
{

}

UINT CDeferredRenderingPipeline::CDeferredRenderingPassDefiner::EndDefine()
{
	CVector<RENDERTEXTURE>::CIterator Iterator = m_Pipeline.m_RenderTextures.GetIterator(0);
	while( CVector<RENDERTEXTURE>::IsValuable(Iterator) )
	{
		if(Iterator->pTexture && Iterator->pEffectInfo)
			CEffectCode::CPassDefiner::AddInfo( *Iterator->pEffectInfo, ALCHEMY_NULL, 0, CEffectCode::CPassDefiner::AddColorKey(CVertexDeclaration::FLOAT4, true) );

		++ Iterator;
	}

	return CDeferredRenderingPipeline::CPassDefiner::EndDefine();
}

CDeferredRenderingPipeline::CDeferredRenderingPipeline(CRenderQueue& RenderQueue) : 
m_RenderQueue(RenderQueue),
m_bIsDirty(false)
{
}

CDeferredRenderingPipeline::~CDeferredRenderingPipeline(void)
{
	/*CMap<INT, CViewportTextureInfo*>::CIterator Iterator = m_ViewportTextureInfoMap.GetBeginIterator();
	while( CMap<INT, CViewportTextureInfo*>::IsValuable(Iterator) )
	{
		ALCHEMY_DELETE( Iterator.GetValue() );

		++ Iterator;
	}*/
}

bool CDeferredRenderingPipeline::AddRenderTexture(CTexture::FORMAT TextureFormat, IEffectInfo& EffectInfo, INT nShift)
{
	/*CRenderSystem& RenderSystem = m_RenderQueue.GetRenderSystem();

	CViewportTextureInfo** ppViewportTextureInfo = m_ViewportTextureInfoMap.Get(nShift);
	CViewportTextureInfo* pViewportTextureInfo = ppViewportTextureInfo ? *ppViewportTextureInfo : ALCHEMY_NULL;
	if(!pViewportTextureInfo)
	{
		ALCHEMY_DEBUG_NEW( pViewportTextureInfo, CViewportTextureInfo(RenderSystem) );
		pViewportTextureInfo->SetShift(nShift);
		m_ViewportTextureInfoMap.Insert(nShift, pViewportTextureInfo);
	}

	LPTEXTURE pTexture = RenderSystem.CreateTexture(*pViewportTextureInfo, ALCHEMY_FLAG(CTexture::RENDER_TARGET), TextureFormat, 1, ALCHEMY_NULL);
	if(!pTexture)
	{
		ALCHEMY_DEBUG_WARNING("CDeferredRenderingPipeline::AddRenderTexture: Texture has been created fail.");

		return false;
	}*/

	RENDERTEXTURE RenderTexture;
	RenderTexture.pTexture    = m_RenderQueue.GetRenderSystem().CreateRenderTargetView(TextureFormat, nShift);
	RenderTexture.pEffectInfo = &EffectInfo;

	m_RenderTextures.Add(RenderTexture);

	//ALCHEMY_VIEWPORT_RENDER_TARGET_LISTENER.AddEventListener(m_RenderTextures[m_RenderTextures.Add(RenderTexture)].pTexture, m_RenderQueue.GetRenderSystem(), 0, TextureFormat, nShift);

	m_bIsDirty = true;

	return true;
}

CDeferredRenderingPipeline::CPassDefiner& CDeferredRenderingPipeline::CreatePassDefiner(CEffectCode& Code)
{
	CPassDefiner* pPassDefiner;

	ALCHEMY_DEBUG_NEW( pPassDefiner, CDeferredRenderingPassDefiner(Code, *this) );

	return *pPassDefiner;
}

void CDeferredRenderingPipeline::Activate()
{
	if(m_bIsDirty)
	{
		m_RenderQueue.ClearRenderTargetQueue(CRenderQueue::NORMAL);

		CVector<RENDERTEXTURE>::CIterator Iterator = m_RenderTextures.GetIterator(0);
		while( CVector<RENDERTEXTURE>::IsValuable(Iterator) )
		{
			if(Iterator->pTexture && Iterator->pEffectInfo)
				m_RenderQueue.AddRenderTargetToQueue(Iterator->pTexture->GetResourceHandle(), CRenderQueue::NORMAL);

			++ Iterator;
		}

		m_bIsDirty = false;
	}
}

void CDeferredRenderingPipeline::Destroy()
{
	/*CMap<INT, CViewportTextureInfo*>::CIterator Iterator = m_ViewportTextureInfoMap.GetBeginIterator();
	while( CMap<INT, CViewportTextureInfo*>::IsValuable(Iterator) )
	{
		ALCHEMY_DELETE( Iterator.GetValue() );

		++ Iterator;
	}

	m_ViewportTextureInfoMap.Clear();*/
	
	CVector<RENDERTEXTURE>::CIterator Iterator = m_RenderTextures.GetIterator(0);
	LPTEXTURE pTexture;
	while( CVector<RENDERTEXTURE>::IsValuable(Iterator) )
	{
		pTexture = Iterator->pTexture;
		if(pTexture)
			pTexture->Release();

		++ Iterator;
	}

	m_RenderTextures.Clear();
}