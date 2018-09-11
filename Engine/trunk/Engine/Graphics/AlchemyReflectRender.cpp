#include "AlchemyReflectRender.h"
#include "AlchemySceneManager.h"

using namespace alchemy;

CReflectRender::CReflectRender()
:m_RenderTarget(0),
m_ppSceneNodes(ALCHEMY_NULL),
m_uSceneNodeNum(0),
m_uRenderList(0),
m_pSceneManager(ALCHEMY_NULL),
m_pRenderQueue(ALCHEMY_NULL),
m_pPlane(ALCHEMY_NULL)
{
	
}

CReflectRender::~CReflectRender()
{
	if(m_ppSceneNodes && m_uSceneNodeNum)
	{
		for (UINT i=0; i<m_uSceneNodeNum; ++i)
			m_ppSceneNodes[i]->Release();

		ALCHEMY_DEBUG_DELETE(m_ppSceneNodes);
	}
}

bool CReflectRender::Create(CGraphicsResource::HANDLE RenderTarget, const CPlane& Plane, CSceneNode** ppSceneNodes, UINT uSceneNodeNum )
{
	Destroy();

	m_pSceneManager = CSceneManager::GetInstance();
	if(!m_pSceneManager)
		return false;

	LPTEXTURE pTexture = static_cast<LPTEXTURE>( m_pSceneManager->GetRenderSystem().GetResource(RenderTarget, CGraphicsResource::TEXTURE) );
	if(!pTexture)
		return false;

	pTexture->AddRef();

	m_RenderTarget = RenderTarget;

	m_pPlane = &Plane;

	m_pRenderQueue = &m_pSceneManager->GetRenderQueue();

	if(ppSceneNodes && uSceneNodeNum)
	{
		ALCHEMY_DEBUG_NEW(m_ppSceneNodes, CSceneNode*[uSceneNodeNum]);
		m_uSceneNodeNum = uSceneNodeNum;
		m_uRenderList = m_pRenderQueue->AddRenderList(false);

		for(UINT i=0; i<m_uSceneNodeNum; ++i)
		{
			m_ppSceneNodes[i] = ppSceneNodes[i];
			m_ppSceneNodes[i]->AddRef();

			m_ppSceneNodes[i]->ApplyForRender(~0, m_uRenderList);
		}
	}

	return true;
}

void CReflectRender::Reset()
{
	if(m_uSceneNodeNum && m_ppSceneNodes && m_uRenderList)
	{
		for(UINT i=0; i<m_uSceneNodeNum; ++i)
		{
			m_ppSceneNodes[i] = m_ppSceneNodes[i];
			m_ppSceneNodes[i]->AddRef();

			m_ppSceneNodes[i]->ApplyForRender(~0, m_uRenderList);
		}
	}
}

void CReflectRender::Destroy()
{
	if(m_ppSceneNodes && m_uSceneNodeNum)
	{
		for (UINT i=0; i<m_uSceneNodeNum; ++i)
			m_ppSceneNodes[i]->Release();

		ALCHEMY_DEBUG_DELETE(m_ppSceneNodes);
	}

	if(m_RenderTarget)
	{
		LPTEXTURE pTexture = static_cast<LPTEXTURE>( m_pSceneManager->GetRenderSystem().GetResource(m_RenderTarget, CGraphicsResource::TEXTURE) );
		if(pTexture)
			pTexture->Release();
		
		m_RenderTarget = 0;
	}

	m_pSceneManager = ALCHEMY_NULL;
	m_pRenderQueue  = ALCHEMY_NULL;
	m_pPlane		= ALCHEMY_NULL;
	m_uRenderList   = 0;
}

void CReflectRender::RenderAll()
{
	if(!m_pSceneManager || !m_pRenderQueue || !m_RenderTarget || !m_pPlane)
		return;

	CGraphicsResource::HANDLE RenderTarget = m_pRenderQueue->GetRenderTarget();
	m_pRenderQueue->SetRenderTarget(m_RenderTarget, 0);

	CCamera& Camera = m_pSceneManager->GetSceneCamera();
	m_pSceneManager->SetCamera(&Camera, 0);

	static CMatrix4x4 s_LocalMatrix;

	s_LocalMatrix = Camera.GetLocalMatrix();

	static CMatrix4x4 s_ReflectMatrix;

	s_ReflectMatrix.Reflect(*m_pPlane);

	Multiply4x3(s_ReflectMatrix, s_ReflectMatrix, s_LocalMatrix);

	Camera.SetMatrix(s_ReflectMatrix);
	Camera.SetClipPlane(*m_pPlane);
	Camera.Update();

	CRenderSystem& RenderSystem = m_pSceneManager->GetRenderSystem();

	SetRenderState(RenderSystem, IRenderState::CULL_MODE, IRenderState::CW);

	if(m_ppSceneNodes && m_uSceneNodeNum)
		m_pRenderQueue->RenderScene(m_uRenderList);
	else
		m_pSceneManager->RenderScene( ALCHEMY_FLAG(CRenderSystem::TARGET) | ALCHEMY_FLAG(CRenderSystem::ZBUFFER) | ALCHEMY_FLAG(CRenderSystem::STENCIL) );

	SetRenderState(RenderSystem, IRenderState::CULL_MODE, IRenderState::CCW);

	Camera.SetMatrix(s_LocalMatrix);
	Camera.SetProjection();
	Camera.Update();

	Camera.SetDirtyFlag(0);

	m_pRenderQueue->SetRenderTarget(RenderTarget);
}