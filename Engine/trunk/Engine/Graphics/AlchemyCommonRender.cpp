#include "AlchemyCommonRender.h"
#include "AlchemySceneManager.h"

using namespace alchemy;

CCommonRender::CCommonRender()
:m_pSceneManager(ALCHEMY_NULL),
m_pCamera(ALCHEMY_NULL)
{

}

CCommonRender::~CCommonRender()
{

}

bool CCommonRender::Create(CCamera* pCamera)
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

	m_pCamera = pCamera;
	if(m_pCamera)
		m_pCamera->AddRef();

	return true;
}

UINT CCommonRender::ApplyRenderTargetGroup( ARGBCOLOR ClearColor, UINT uClearFlag, CGraphicsResource::HANDLE RenderTarget )
{
	if(!m_pSceneManager)
		return 0;

	LPTEXTURE pRenderTarget = static_cast<LPTEXTURE>( m_pSceneManager->GetRenderSystem().GetResource(RenderTarget, CGraphicsResource::TEXTURE) );
	if(!pRenderTarget)
	{
		ALCHEMY_DEBUG_WARNING("CCommonRender::ApplyRenderTargetGroup() - Error RenderTarget Handle");
		return 0;
	}

	RENDERTARGETGROUP Group;
	Group.ClearColor	= ClearColor;
	Group.ClearFlag		= uClearFlag;
	Group.RenderTarget	= RenderTarget;
	Group.RenderList	= m_pRenderQueue->AddRenderList(false);

	return m_GroupObjects.Add( Group ) + 1;
}

void CCommonRender::RemoveRenderTargetGroup( UINT uGroupHandle )
{
	if(uGroupHandle)
	{
		RENDERTARGETGROUP* pRenderGroup = m_GroupObjects.Get(uGroupHandle - 1);
		if(pRenderGroup)
		{
			CPool<RENDEROBJECT>::CIterator iter = pRenderGroup->RenderObjects.GetIterator(0);

			while ( CPool<RENDEROBJECT>::IsValuable(iter) )
			{
				if(iter->pSceneNode)
					iter->pSceneNode->Release();

				++iter;
			}

			m_GroupObjects.Remove(uGroupHandle - 1, 1);

			m_pSceneManager->ResetScene();
		}
	}
}

UINT CCommonRender::AddRenderObject(UINT uGroupHandle, CSceneNode& SceneNode, UINT uRenderPass)
{
	if(!m_pSceneManager || !uGroupHandle)
		return 0;

	RENDERTARGETGROUP* pRenderTargetGroup = m_GroupObjects.Get( uGroupHandle - 1 );
	if(!pRenderTargetGroup)
		return 0;

	SceneNode.AddRef();

	RENDEROBJECT RenderObject;

	RenderObject.pSceneNode = &SceneNode;
	RenderObject.RenderPass = uRenderPass;

	SceneNode.ApplyForRender(uRenderPass, pRenderTargetGroup->RenderList);

	return pRenderTargetGroup->RenderObjects.Add(RenderObject);
}

void CCommonRender::RemoveRenderObject( UINT uGroupHandle, UINT uRenderObjectHandle )
{
	if(!m_pSceneManager || !uGroupHandle || !uRenderObjectHandle)
		return;

	RENDERTARGETGROUP* pRenderTargetGroup = m_GroupObjects.Get( uGroupHandle - 1 );
	if(!pRenderTargetGroup)
		return;

	RENDEROBJECT* pRenderObject = pRenderTargetGroup->RenderObjects.Get( uRenderObjectHandle - 1 );
	if(pRenderObject && pRenderObject->pSceneNode)
		pRenderObject->pSceneNode->Release();

	pRenderTargetGroup->RenderObjects.Remove( uRenderObjectHandle -1 , 1 );

	m_pSceneManager->ResetScene();
}

void CCommonRender::Destroy()
{

}

void CCommonRender::Reset()
{
	CPool<RENDERTARGETGROUP>::CIterator iter = m_GroupObjects.GetIterator(0);
	CPool<RENDEROBJECT>::CIterator stIter;

	while ( CPool<RENDERTARGETGROUP>::IsValuable(iter) )
	{
		stIter = iter->RenderObjects.GetIterator(0);
		while ( CPool<RENDEROBJECT>::IsValuable(stIter) )
		{
			if(stIter->pSceneNode)
				stIter->pSceneNode->ApplyForRender( stIter->RenderPass, iter->RenderList );

			++stIter;
		}

		++iter;
	}
}

void CCommonRender::RenderAll()
{
	if(!m_pSceneManager)
		return;

	CGraphicsResource::HANDLE OldRenderTarget = m_pRenderQueue->GetRenderTarget();
	const CCamera* pOldCamera = ALCHEMY_NULL;

	if(m_pCamera)
		pOldCamera = m_pSceneManager->GetCamera(0);

	CPool<RENDERTARGETGROUP>::CIterator iter = m_GroupObjects.GetIterator(0);
	while ( CPool<RENDERTARGETGROUP>::IsValuable(iter) )
	{
		m_pRenderQueue->SetRenderTarget( iter->RenderTarget );

		m_pSceneManager->GetRenderSystem().Clear( iter->ClearColor, iter->ClearFlag );

		m_pRenderQueue->RenderScene( iter->RenderList );

		++iter;
	}

	if(pOldCamera)
		m_pSceneManager->SetCamera(pOldCamera, 0);

	m_pRenderQueue->SetRenderTarget(OldRenderTarget);
}