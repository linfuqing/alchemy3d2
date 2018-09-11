#include "AlchemyShadowMapRender.h"
#include "AlchemySceneManager.h"

using namespace alchemy;

CShadowMapRender::CShadowMapRender()
:m_eShadowType(SPOT_LIGHT_SHADOW),
m_pLight(ALCHEMY_NULL),
m_ShadowMap(0),
m_pLightCamera(ALCHEMY_NULL),
m_pSceneManager(ALCHEMY_NULL),
m_uRenderList(0),
m_pRenderQueue(ALCHEMY_NULL)
{
}

CShadowMapRender::~CShadowMapRender()
{
	UINT uLen = m_ShadowCasters.GetLength();
	for (UINT i=0; i<uLen; ++i)
	{
		SHADOWCASTER& ShadowCaster = m_ShadowCasters[i];

		if( ShadowCaster.pSceneNode )
			ShadowCaster.pSceneNode->Release();
	}

	if(m_pLightCamera)
	{
		m_pLightCamera->Release();
		m_pLightCamera = ALCHEMY_NULL;
	}

	if(m_pSceneManager)
	{
		m_pSceneManager = ALCHEMY_NULL;
		m_pRenderQueue = ALCHEMY_NULL;
	}
}

bool CShadowMapRender::Create(CGraphicsResource::HANDLE Light, CCamera& LightCamera, CGraphicsResource::HANDLE RenderTarget, SHADOWLIGHTTYPE ShadowType)
{
	Destroy();

	CSceneManager* pSceneManager = CSceneManager::GetInstance();
	if(!pSceneManager)
		return false;

	if(m_pSceneManager)
	{
		if(pSceneManager != m_pSceneManager)
		{
			m_pRenderQueue = &m_pSceneManager->GetRenderQueue();
			m_pSceneManager = pSceneManager;

			m_uRenderList = m_pRenderQueue->AddRenderList(false);
		}
	}
	else
	{
		m_pRenderQueue = &pSceneManager->GetRenderQueue();
		m_pSceneManager = pSceneManager;
		m_uRenderList = m_pRenderQueue->AddRenderList(false);
	}

	m_pLight = static_cast<CLight*>( m_pSceneManager->GetResource(Light, CGraphicsResource::LIGHT) );//->GetLight(uLightIndex);
	if(!m_pLight)
	{
		ALCHEMY_DEBUG_WARNING("CShadowMapRender::Create() - GetLight() - Error light Index");

		return false;
	}

	m_pLightCamera = &LightCamera;
	m_pLightCamera->AddRef();

	LPTEXTURE pRenderTarget =  static_cast<LPTEXTURE>( m_pSceneManager->GetRenderSystem().GetResource(RenderTarget, CGraphicsResource::TEXTURE) );
	if(!pRenderTarget)
	{
		ALCHEMY_DEBUG_WARNING("CShadowMapRender::Create() - GetRendetTarget() - Error RenderTarget Handle");

		return false;
	}

	pRenderTarget->AddRef();

	m_ShadowMap = RenderTarget;

	CCamera& SceneCamera = m_pSceneManager->GetSceneCamera();

	m_eShadowType = ShadowType;

	return true;
}

UINT CShadowMapRender::AddShadowCaster(CSceneNode& RenderNode, UINT uRenderPass)
{
	if(!m_pSceneManager || !m_pLightCamera || !m_ShadowMap)
		return 0;

	SHADOWCASTER Caster = {0};

	RenderNode.AddRef();

	Caster.pSceneNode		= &RenderNode;
	Caster.uRenderPass		= uRenderPass;

	RenderNode.ApplyForRender(uRenderPass, m_uRenderList);

	return m_ShadowCasters.Add(Caster) + 1;
}

void CShadowMapRender::RemoveShadowCaster( UINT uCasterHandle )
{
	if(uCasterHandle)
	{
		SHADOWCASTER* pCaster = m_ShadowCasters.Get(uCasterHandle);
		if(pCaster)
			pCaster->pSceneNode->Release();

		m_pSceneManager->ResetScene();

		m_ShadowCasters.Remove(uCasterHandle - 1, 1);
	}
}

void CShadowMapRender::RenderAll()
{
	if(!m_pSceneManager || !m_pLightCamera || !m_ShadowMap || !m_pLight)
		return;

	if(m_eShadowType == DIRECTIONAL_LIGHT_SHADOW)
	{
		static CCamera& SceneCamera = m_pSceneManager->GetSceneCamera();
		static const CFrustum& SceneFrustum = SceneCamera.GetFrustum();
		static SPHERE SceneSphere;

		SceneFrustum.GetSphere(SceneSphere);

		m_pLightCamera->ExtractFrom(SceneSphere, m_pLight->Direction, SceneCamera.IsLeftHand(), SceneCamera.IsHalfDepth());
		m_pLightCamera->Update();
	}
	else if(m_eShadowType == SPOT_LIGHT_SHADOW)
	{
		m_pLightCamera->SetTranslation( m_pLight->Position.x,  m_pLight->Position.y,  m_pLight->Position.z );
		m_pLightCamera->SetDirection(m_pLight->Direction);
		m_pLightCamera->Update();
	}

	CGraphicsResource::HANDLE OldRenderTarget = m_pRenderQueue->GetRenderTarget();
	const CCamera* pOldCamera = m_pSceneManager->GetCamera(0);

	m_pRenderQueue->SetRenderTarget(m_ShadowMap);
	m_pSceneManager->GetRenderSystem().Clear( 0xffffffff, CRenderSystem::TARGET );

	m_pSceneManager->SetCamera(m_pLightCamera, 0);

	m_pRenderQueue->RenderScene(m_uRenderList);

	m_pSceneManager->SetCamera(pOldCamera, 0);

	m_pRenderQueue->SetRenderTarget(OldRenderTarget);
}

void CShadowMapRender::Reset()
{
	UINT i, uLen = m_ShadowCasters.GetLength();
	for (i=0; i<uLen; ++i)
	{
		SHADOWCASTER& ShadowCaster = m_ShadowCasters[i];
		if(ShadowCaster.pSceneNode)
			ShadowCaster.pSceneNode->ApplyForRender(ShadowCaster.uRenderPass, m_uRenderList);
	}
}

void CShadowMapRender::Destroy()
{
	if(m_ShadowMap && m_pSceneManager)
	{
		LPTEXTURE pRenderTarget =  static_cast<LPTEXTURE>( m_pSceneManager->GetRenderSystem().GetResource(m_ShadowMap, CGraphicsResource::TEXTURE) );
		if(!pRenderTarget)
		{
			ALCHEMY_DEBUG_WARNING("CShadowMapRender::Create() - GetRendetTarget() - Error RenderTarget Handle");

			return;
		}

		m_ShadowMap = 0;
	}

	if(m_pSceneManager)
	{
		UINT uLen = m_ShadowCasters.GetLength();

		for (UINT i=0; i<uLen; ++i)
		{
			SHADOWCASTER& ShadowCaster = m_ShadowCasters[i];

			if( ShadowCaster.pSceneNode )
				ShadowCaster.pSceneNode->Release();
		}
	}

	m_ShadowCasters.Reset();

	if(m_pLightCamera)
	{
		m_pLightCamera->Release();
		m_pLightCamera = ALCHEMY_NULL;
	}
}