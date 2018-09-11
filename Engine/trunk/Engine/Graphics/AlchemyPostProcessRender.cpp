#include "AlchemyPostProcessRender.h"

using namespace  alchemy;

CPostProcessRender::CPostProcessRender()
:m_pEffect(ALCHEMY_NULL),
m_pRenderMethod(ALCHEMY_NULL),
m_pMesh(ALCHEMY_NULL),
m_OutputRenderTarget(0),
m_uRenderPass(0),
m_uRenderData(0),
m_pMaterial(ALCHEMY_NULL),
m_eRenderType(CRenderQueue::FULL_SCREEN)
{

}

CPostProcessRender::~CPostProcessRender()
{
}

bool CPostProcessRender::Create(CGraphicsResource::HANDLE Material, IRenderMethod& RenderMethod, CGraphicsResource::HANDLE Effect, UINT uRenderPass,
								CGraphicsResource::HANDLE OutputRenderTarget, CGraphicsResource::HANDLE ScreenMesh, CRenderQueue::RENDERENTRYTYPE RenderType)
{
	Destroy();

	m_eRenderType = RenderType;

	m_pSceneManager = CSceneManager::GetInstance();
	if(!m_pSceneManager)
	{
		ALCHEMY_DEBUG_WARNING("CPostProcessRender::Create() - SceneManager error");
		return false;
	}

	CRenderSystem& RenderSystem = m_pSceneManager->GetRenderSystem();

	m_pMaterial = static_cast<CMaterial*>( m_pSceneManager->GetResource(Material, CGraphicsResource::MATERIAL) );
	if(!m_pMaterial)
	{
		ALCHEMY_DEBUG_WARNING("CPostProcessRender::Create() - Material error");
		return false;
	}

	if(m_pMaterial->GetTextures().GetLength() == 0)
	{
		ALCHEMY_DEBUG_WARNING("CPostProcessRender::Create() - Material no texture");
		return false;
	}

	m_OutputRenderTarget = OutputRenderTarget;
	if(m_OutputRenderTarget)
	{
		LPTEXTURE pRenderTarget = static_cast<LPTEXTURE>( RenderSystem.GetResource(m_OutputRenderTarget, CGraphicsResource::TEXTURE) );
		if(pRenderTarget)
			pRenderTarget->AddRef();
		else
			m_OutputRenderTarget = 0;
	}

	if(ScreenMesh)
	{
		m_pMesh = static_cast<CMesh*>( m_pSceneManager->GetResource(ScreenMesh, CMesh::MESH) );
		if(m_pMesh)
			m_pMesh->AddRef();
	}

	m_pEffect = static_cast<CEffect*>( m_pSceneManager->GetRenderSystem().GetResource(Effect, CGraphicsResource::EFFECT) );
	if(m_pEffect)
		m_pEffect->AddRef();
	else
	{
		ALCHEMY_DEBUG_WARNING("CPostProcessRender::Create() - Effect error");
		return false;
	}

	m_pRenderMethod = &RenderMethod;
	m_pRenderMethod->AddRef();

	CSceneManager::IFullScreenDrawer* pFullScreenDrawer = m_pSceneManager->GetFullScreenDrawer();

	m_uRenderData = pFullScreenDrawer->AddUserData( m_pMaterial->GetTextures()[0]->GetResourceHandle(), m_pRenderMethod, m_pMaterial->GetResourceHandle(), 0, ALCHEMY_NULL );

	return true;
}

bool CPostProcessRender::Create(CGraphicsResource::HANDLE Material, UINT uRenderTargetIndex, IRenderMethod& RenderMethod, CGraphicsResource::HANDLE Effect, UINT uRenderPass,
			CGraphicsResource::HANDLE OutputRenderTarget, CGraphicsResource::HANDLE ScreenMesh, CRenderQueue::RENDERENTRYTYPE RenderType)
{
	Destroy();

	m_eRenderType = RenderType;

	m_pSceneManager = CSceneManager::GetInstance();
	if(!m_pSceneManager)
	{
		ALCHEMY_DEBUG_WARNING("CPostProcessRender::Create() - SceneManager error");
		return false;
	}

	CRenderSystem& RenderSystem = m_pSceneManager->GetRenderSystem();

	m_pMaterial = static_cast<CMaterial*>( m_pSceneManager->GetResource(Material, CGraphicsResource::MATERIAL) );
	if(!m_pMaterial)
	{
		ALCHEMY_DEBUG_WARNING("CPostProcessRender::Create() - Material error");
		return false;
	}

	m_OutputRenderTarget = OutputRenderTarget;
	if(m_OutputRenderTarget)
	{
		LPTEXTURE pRenderTarget = static_cast<LPTEXTURE>( RenderSystem.GetResource(m_OutputRenderTarget, CGraphicsResource::TEXTURE) );
		if(pRenderTarget)
			pRenderTarget->AddRef();
		else
			m_OutputRenderTarget = 0;
	}

	if(ScreenMesh)
	{
		m_pMesh = static_cast<CMesh*>( m_pSceneManager->GetResource(ScreenMesh, CMesh::MESH) );
		if(m_pMesh)
			m_pMesh->AddRef();
	}

	m_pEffect = static_cast<CEffect*>( m_pSceneManager->GetRenderSystem().GetResource(Effect, CGraphicsResource::EFFECT) );
	if(m_pEffect)
		m_pEffect->AddRef();
	else
	{
		ALCHEMY_DEBUG_WARNING("CPostProcessRender::Create() - Effect error");
		return false;
	}

	m_pRenderMethod = &RenderMethod;
	m_pRenderMethod->AddRef();

	CSceneManager::IFullScreenDrawer* pFullScreenDrawer = m_pSceneManager->GetFullScreenDrawer();

	m_uRenderData = pFullScreenDrawer->AddUserData(0, m_pRenderMethod, m_pMaterial->GetResourceHandle(), uRenderTargetIndex, ALCHEMY_NULL);

	return true;
}

//void CPostProcessRender::SetEffect( CGraphicsResource::HANDLE Effect )
//{
//	if(!m_pSceneManager)
//		return;
//
//	CEffect* pEffect = static_cast<LPTEXTURE>( m_pSceneManager->GetRenderSystem().GetResource(m_pInputRenderTargets[i], CGraphicsResource::TEXTURE) );
//	if(pEffect)
//		pEffect->AddRef();
//
//	if(m_pEffect)
//		m_pEffect->Release();
//	
//	if(m_pEffect != pEffect)
//	{
//		m_pEffect = pEffect;
//		m_pSceneManager->ResetScene();
//	}
//}
//
//void CPostProcessRender::SetRenderPass( UINT uRenderPass )
//{
//	if(m_uRenderPass != uRenderPass)
//	{
//		m_uRenderPass = uRenderPass;
//		if(m_pSceneManager)
//			m_pSceneManager->ResetScene();
//	}
//}
//
//void CPostProcessRender::SetRenderMethod( IRenderMethod* pRenderMethod )
//{
//	if(!m_pSceneManager)
//		return;
//
//	if(pRenderMethod)
//		pRenderMethod->AddRef();
//
//	if(m_pRenderMethod)
//		m_pRenderMethod->Release();
//
//	if(m_pRenderMethod != pRenderMethod)
//	{
//		m_pRenderMethod = pRenderMethod;
//		m_pSceneManager->ResetScene();
//	}
//}

void CPostProcessRender::Destroy()
{
	if(!m_pSceneManager)
		return;

	CRenderSystem& RenderSystem = m_pSceneManager->GetRenderSystem();

	if(m_OutputRenderTarget)
	{
		LPTEXTURE pRenderTarget = static_cast<LPTEXTURE>( RenderSystem.GetResource(m_OutputRenderTarget, CGraphicsResource::TEXTURE) );
		if(pRenderTarget)
			pRenderTarget->Release();

		m_OutputRenderTarget = 0;
	}

	if(m_pEffect)
	{
		m_pEffect->Release();
		m_pEffect = ALCHEMY_NULL;
	}

	if(m_pRenderMethod)
	{
		m_pRenderMethod->Release();
		m_pRenderMethod = ALCHEMY_NULL;
	}

	if(m_pMesh)
	{
		m_pMesh->Release();
		m_pMesh = ALCHEMY_NULL;
	}

	if(m_pMaterial)
	{
		m_pMaterial->Release();
		m_pMaterial = ALCHEMY_NULL;
	}

	m_pSceneManager = ALCHEMY_NULL;
}

bool CPostProcessRender::ApplyForRender()
{
	if(m_pSceneManager && m_pMaterial && m_pEffect && m_pRenderMethod)
	{
		CRenderQueue& RenderQueue = m_pSceneManager->GetRenderQueue();
		CSceneManager::IFullScreenDrawer* pFullScreenDrawer = m_pSceneManager->GetFullScreenDrawer();

		CRenderQueue::RENDERENTRY& RenderEntry = RenderQueue.LockRenderEntry(*pFullScreenDrawer);
		RenderEntry.uEffect            = m_pEffect->GetResourceHandle();
		RenderEntry.uRenderPass        = m_uRenderPass;
		RenderEntry.uVertexDeclaration = m_pMesh ? m_pMesh->GetVertex().GetVertexDeclaration().GetResourceHandle() : 0;
		RenderEntry.uModel             = m_pMesh ? m_pMesh->GetResourceHandle() : 0;
		RenderEntry.uRenderTarget	   = m_OutputRenderTarget;
		RenderEntry.uModelParameter    = 0;
		RenderEntry.uUserData          = m_uRenderData;
		RenderQueue.UnlockRenderEntry( m_eRenderType );

		return true;
	}

	return false;
}