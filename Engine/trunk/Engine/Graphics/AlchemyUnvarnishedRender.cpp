#include "AlchemyUnvarnishedRender.h"
#include "AlchemySceneManager.h"

using namespace alchemy;

CUnvarnishedRender::CUnvarnishedRender()
:m_RenderTarget(0),
m_pSceneManager(ALCHEMY_NULL),
m_pCamera(ALCHEMY_NULL),
m_uRenderList(0),
m_pRenderQueue(ALCHEMY_NULL)
{

}

CUnvarnishedRender::~CUnvarnishedRender()
{
	UINT uLen = m_RenderObjects.GetLength();
	for (UINT i=0; i<uLen; ++i)
		if( m_RenderObjects[i].pSceneNode )
			m_RenderObjects[i].pSceneNode->Release();

	if(m_pSceneManager)
	{
		m_pSceneManager = ALCHEMY_NULL;
		m_pRenderQueue = ALCHEMY_NULL;
	}
}

bool CUnvarnishedRender::Create( ARGBCOLOR ClearColor, UINT uClearFlag, CGraphicsResource::HANDLE hRenderTarget, const CCamera* pCamera)
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

	m_ClearColor = ClearColor;
	m_uClearFlag = uClearFlag;

	m_pCamera = pCamera;

	LPTEXTURE pRenderTarget = static_cast<LPTEXTURE>( m_pSceneManager->GetRenderSystem().GetResource(hRenderTarget, CGraphicsResource::TEXTURE) );
	if(pRenderTarget)
		pRenderTarget->AddRef();
	else
		return false;

	m_RenderTarget = hRenderTarget;

	return true;
}

UINT CUnvarnishedRender::AddRenderObject(CSceneNode& SceneNode, UINT uRenderPass)
{
	if(!m_pSceneManager || !m_RenderTarget)
		return 0;

	SceneNode.AddRef();

	RENDEROBJECT RenderObject;
	RenderObject.pSceneNode = &SceneNode;
	RenderObject.RenderPass = uRenderPass;

	SceneNode.ApplyForRender(uRenderPass, m_uRenderList);
	
	return m_RenderObjects.Add(RenderObject) + 1;
}

void CUnvarnishedRender::RemoveRenderObject( UINT uRenderObjectHandle )
{
	if(uRenderObjectHandle)
	{
		RENDEROBJECT* pRenderObject = m_RenderObjects.Get(uRenderObjectHandle);
		if(pRenderObject)
			pRenderObject->pSceneNode->Release();

		m_pSceneManager->ResetScene();

		m_RenderObjects.Remove(uRenderObjectHandle - 1, 1);
	}
}

void CUnvarnishedRender::Reset()
{
	UINT i, uLen = m_RenderObjects.GetLength();
	for (i=0; i<uLen; ++i)
	{
		RENDEROBJECT& RenderObject = m_RenderObjects[i];
		if(RenderObject.pSceneNode)
			RenderObject.pSceneNode->ApplyForRender(RenderObject.RenderPass, m_uRenderList);
	}
}

void CUnvarnishedRender::RenderAll()
{
	if(!m_pSceneManager || !m_RenderTarget)
		return;

	CGraphicsResource::HANDLE OldRenderTarget = m_pRenderQueue->GetRenderTarget();
	const CCamera* pOldCamera = ALCHEMY_NULL;
	
	m_pRenderQueue->SetRenderTarget(m_RenderTarget);
	m_pSceneManager->GetRenderSystem().Clear( m_ClearColor, m_uClearFlag );

	if(m_pCamera)
	{
		pOldCamera = m_pSceneManager->GetCamera(0);
		m_pSceneManager->SetCamera(m_pCamera, 0);
	}

	m_pRenderQueue->RenderScene(m_uRenderList);

	if(pOldCamera)
		m_pSceneManager->SetCamera(pOldCamera, 0);

	m_pRenderQueue->SetRenderTarget(OldRenderTarget);
}

void CUnvarnishedRender::Destroy()
{
	if(m_pSceneManager && m_RenderTarget)
	{
		LPTEXTURE pRenderTarget = static_cast<LPTEXTURE>( m_pSceneManager->GetRenderSystem().GetResource(m_RenderTarget, CGraphicsResource::TEXTURE) );
		if(pRenderTarget)
			pRenderTarget->Release();

		m_RenderTarget = 0;
	}

	UINT uLen = m_RenderObjects.GetLength();
	for (UINT i=0; i<uLen; ++i)
		if( m_RenderObjects[i].pSceneNode )
			m_RenderObjects[i].pSceneNode->Release();

	m_RenderObjects.Reset();
}