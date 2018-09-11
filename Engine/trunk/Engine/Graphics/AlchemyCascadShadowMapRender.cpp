#include "AlchemyCascadShadowMapRender.h"
#include "AlchemySceneManager.h"

using namespace alchemy;

CCascadShadowMapRender::CCascadShadowMapRender()
:m_uSplitNum(0),
m_ppSplitCameras(ALCHEMY_NULL),
m_pSplitTextures(ALCHEMY_NULL),
m_pLight(ALCHEMY_NULL),
m_pSceneManager(ALCHEMY_NULL),
m_pDepthSegment(ALCHEMY_NULL),
m_pRenderLists(ALCHEMY_NULL),
m_pRenderQueue(ALCHEMY_NULL),
m_fFrustumScale(1.0f)
{
}

CCascadShadowMapRender::~CCascadShadowMapRender()
{
	UINT uLen = m_ShadowCasters.GetLength();

	for (UINT i=0; i<uLen; ++i)
		if( m_ShadowCasters[i].pSceneNode )
			m_ShadowCasters[i].pSceneNode->Release();

	ALCHEMY_DEBUG_DELETE_ARRAY(m_pSplitTextures);

	ALCHEMY_DEBUG_DELETE_ARRAY(m_pRenderLists);

	if(m_pSceneManager)
	{
		m_pSceneManager = ALCHEMY_NULL;
		m_pRenderQueue = ALCHEMY_NULL;
	}
}

bool CCascadShadowMapRender::Create(CGraphicsResource::HANDLE Light, CCamera** ppSplitCameras, CGraphicsResource::HANDLE* pSplitTextures, FLOAT* pDepthSegment, UINT uSplitNum, FLOAT FrustumScale)
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
		}
	}
	else
	{
		m_pRenderQueue = &pSceneManager->GetRenderQueue();
		m_pSceneManager = pSceneManager;
	}

	m_pLight = static_cast<CLight*>( m_pSceneManager->GetResource(Light, CGraphicsResource::LIGHT) );//->GetLight(uLightIndex);
	if(!m_pLight)
	{
		ALCHEMY_DEBUG_WARNING("CCascadShadowMap::Create() -- Error Light Index");

		return false;
	}

	if(uSplitNum <= 1)
	{
		ALCHEMY_DEBUG_WARNING("CCascadShadowMap::Create() -- Error Split Number");

		return false;
	}

	m_uSplitNum = uSplitNum;

	m_pSceneCamera = &m_pSceneManager->GetSceneCamera();

	ALCHEMY_DEBUG_NEW(m_pRenderLists, UINT[m_uSplitNum]);
	ALCHEMY_DEBUG_NEW(m_ppSplitCameras, CCamera*[m_uSplitNum]);

	for (UINT i=0; i<m_uSplitNum; ++i)
	{
		m_ppSplitCameras[i] = ppSplitCameras[i];
		m_ppSplitCameras[i]->AddRef();

		m_pRenderLists[i] = m_pRenderQueue->AddRenderList(false);
	}

	m_pDepthSegment = pDepthSegment;
	if(!m_pDepthSegment)
		return false;

	ALCHEMY_DEBUG_NEW(m_pSplitTextures, CGraphicsResource::HANDLE[m_uSplitNum]);
	LPTEXTURE pTexture;
	for (UINT i=0; i<m_uSplitNum; ++i)
	{
		m_pSplitTextures[i] = pSplitTextures[i];
		pTexture = static_cast<LPTEXTURE>( m_pSceneManager->GetRenderSystem().GetResource(m_pSplitTextures[i], CGraphicsResource::TEXTURE) );
		if(pTexture)
			pTexture->AddRef();
	}

	m_fFrustumScale = FrustumScale;

	return true;
}

UINT CCascadShadowMapRender::AddShadowCaster(CSceneNode& Node, UINT uRenderPass)
{
	if(!m_pSceneManager || !m_ppSplitCameras || !m_pDepthSegment || !m_pSplitTextures)
		return 0;

	SHADOWCASTER Caster = {0};
	
	Caster.pSceneNode		= &Node;
	Caster.uRenderPass		= uRenderPass;

	Node.AddRef();

	for (UINT i=0; i<m_uSplitNum; ++i)
		Node.ApplyForRender(uRenderPass, m_pRenderLists[i]);

	return m_ShadowCasters.Add(Caster) + 1;
}

void CCascadShadowMapRender::RemoveShadowCaster(UINT uCasterHandle)
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

void CCascadShadowMapRender::Reset()
{
	UINT i, j;
	UINT uLen = m_ShadowCasters.GetLength();
	for (i=0; i<uLen; ++i)
	{
		SHADOWCASTER& ShadowCaster = m_ShadowCasters[i];
		if(ShadowCaster.pSceneNode)
		{
			for (j=0; j<m_uSplitNum; ++j)
				ShadowCaster.pSceneNode->ApplyForRender(ShadowCaster.uRenderPass, m_pRenderLists[j]);
		}
	}
}

void CCascadShadowMapRender::RenderAll()
{
	if(m_pLight && m_pSceneCamera && m_ppSplitCameras)
	{
		CGraphicsResource::HANDLE OldRenderTarget = m_pRenderQueue->GetRenderTarget();
		const CCamera* pOldCamera = m_pSceneManager->GetCamera(0);

		static SPHERE SceneSphere;

		m_pSceneCamera->GetFrustum().GetSphere(SceneSphere);

		FLOAT fDestNearPlane = m_pSceneCamera->GetNearPlane(), fDestFarPlane = m_pSceneCamera->GetFarPlane() * m_fFrustumScale;
		FLOAT distance_scale = fDestFarPlane / fDestNearPlane; 
		FLOAT partisionFactor = 0.0f; 
		FLOAT lerpValue1 = 0.0f, lerpValue2 = 0.0f; 
		FLOAT SplitsZ = 0.0f; 
		FLOAT fCameraDistance = fDestFarPlane - fDestNearPlane;
		CCamera* pSplitCamera;

		if(!fCameraDistance)
			return;

		for (UINT i=1; i<m_uSplitNum; ++i)
		{
			partisionFactor = i / (FLOAT)m_uSplitNum; 

			lerpValue1 = m_pSceneCamera->GetNearPlane() + partisionFactor * fCameraDistance; 

			lerpValue2 = m_pSceneCamera->GetNearPlane() * powf(distance_scale, partisionFactor); 

			SplitsZ =  0.25f * lerpValue1  + 0.75f * lerpValue2;

			fDestFarPlane = SplitsZ;

			m_pDepthSegment[i-1] = ( fDestFarPlane - m_pSceneCamera->GetNearPlane() )/fCameraDistance;

			m_pSceneCamera->GetFrustum().GetSphere( SceneSphere, m_pDepthSegment[i-1], (fDestNearPlane - m_pSceneCamera->GetNearPlane())/fCameraDistance );

			pSplitCamera = m_ppSplitCameras[i-1];

			pSplitCamera->ExtractFrom( SceneSphere, m_pLight->Direction, m_pSceneCamera->IsLeftHand(), m_pSceneCamera->IsHalfDepth() );

			pSplitCamera->Update();

			fDestNearPlane = fDestFarPlane * 1.0002f;

			m_pRenderQueue->SetRenderTarget( m_pSplitTextures[i-1] );

			m_pSceneManager->GetRenderSystem().Clear( 0xffffffff, CRenderSystem::TARGET );

			m_pSceneManager->SetCamera(pSplitCamera, 0);

			m_pRenderQueue->RenderScene( m_pRenderLists[i-1] );
		}

		m_pSceneCamera->GetFrustum().GetSphere(SceneSphere, 1.0f, (fDestFarPlane - m_pSceneCamera->GetNearPlane())/fCameraDistance);

		pSplitCamera = m_ppSplitCameras[m_uSplitNum-1];

		pSplitCamera->ExtractFrom( SceneSphere, m_pLight->Direction, m_pSceneCamera->IsLeftHand(), m_pSceneCamera->IsHalfDepth() );

		pSplitCamera->Update();

		m_pDepthSegment[m_uSplitNum-1] = 1.0f;

		m_pRenderQueue->SetRenderTarget( m_pSplitTextures[m_uSplitNum-1] );

		m_pSceneManager->GetRenderSystem().Clear( 0xffffffff, CRenderSystem::TARGET );

		m_pSceneManager->SetCamera(pSplitCamera, 0);

		m_pRenderQueue->RenderScene( m_pRenderLists[m_uSplitNum-1] );

		m_pSceneManager->SetCamera(pOldCamera, 0);

		m_pRenderQueue->SetRenderTarget(OldRenderTarget);
	}
}

void CCascadShadowMapRender::Destroy()
{
	if(!m_pSceneManager)
		return;

	if(m_pSplitTextures)
	{
		LPTEXTURE pTexture;
		for (UINT i=0; i<m_uSplitNum; ++i)
		{
			pTexture = static_cast<LPTEXTURE>( m_pSceneManager->GetRenderSystem().GetResource(m_pSplitTextures[i], CGraphicsResource::TEXTURE) );
			if(pTexture)
				pTexture->Release();
		}

		ALCHEMY_DEBUG_DELETE_ARRAY(m_pSplitTextures);
	}

	UINT uLen = m_ShadowCasters.GetLength();

	for (UINT i=0; i<uLen; ++i)
	{
		SHADOWCASTER& ShadowCaster = m_ShadowCasters[i];

		if( ShadowCaster.pSceneNode )
			ShadowCaster.pSceneNode->Release();
	}

	m_ShadowCasters.Reset();

	ALCHEMY_DEBUG_DELETE_ARRAY(m_ppSplitCameras);
	ALCHEMY_DEBUG_DELETE_ARRAY(m_pDepthSegment);

	m_pLight = ALCHEMY_NULL;
}