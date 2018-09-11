#include "AlchemyEnvironmentRender.h"
#include "AlchemySceneManager.h"

using namespace alchemy;

CEnvironmentRender::CEnvironmentRender()
:m_pCamera(ALCHEMY_NULL),
m_pWorldMatrix(ALCHEMY_NULL),
m_RenderTarget(0),
m_uSceneNodeNum(0),
m_ppSceneNodes(ALCHEMY_NULL),
m_pSceneManager(ALCHEMY_NULL),
m_uRenderList(0)
{

}

CEnvironmentRender::~CEnvironmentRender()
{
	ALCHEMY_DEBUG_DELETE(m_pCamera);
	m_pWorldMatrix = ALCHEMY_NULL;

	for (UINT i=0; i<m_uSceneNodeNum; ++i)
		m_ppSceneNodes[i]->Release();

	ALCHEMY_DEBUG_DELETE_ARRAY(m_ppSceneNodes);
}

bool CEnvironmentRender::Create(CGraphicsResource::HANDLE RenderTarget, const CMatrix4x4& WorldMatrix, CSceneNode** ppNodes, UINT uSceneNodeNum )
{
	m_pSceneManager = CSceneManager::GetInstance();
	if(!m_pSceneManager)
		return false;

	LPTEXTURE pSourceRenderTarget = ALCHEMY_NULL;

	if(m_RenderTarget)
		pSourceRenderTarget = static_cast<LPTEXTURE>( m_pSceneManager->GetRenderSystem().GetResource(m_RenderTarget, CGraphicsResource::TEXTURE) );

	LPTEXTURE pRenderTarget = static_cast<LPTEXTURE>( m_pSceneManager->GetRenderSystem().GetResource(RenderTarget, CGraphicsResource::TEXTURE) );

	if(!pRenderTarget || pRenderTarget->GetTextureType() != CTexture::CUBE)
	{
		ALCHEMY_DEBUG_WARNING("Error Render Target handle - CEnvironmentRender::Create");
		return false;
	}

	pRenderTarget->AddRef();

	if(pSourceRenderTarget)
		pSourceRenderTarget->Release();

	m_RenderTarget = RenderTarget;

	m_pWorldMatrix = &WorldMatrix;

	UINT i;
	for (i=0; i<m_uSceneNodeNum; ++i)
		m_ppSceneNodes[i]->Release();

	ALCHEMY_DEBUG_DELETE_ARRAY(m_ppSceneNodes);
	m_uSceneNodeNum = 0;

	if(ppNodes && uSceneNodeNum)
	{
		m_uSceneNodeNum = uSceneNodeNum;
		if(m_uSceneNodeNum)
		{
			ALCHEMY_DEBUG_NEW(m_ppSceneNodes, CSceneNode*[m_uSceneNodeNum]);

			CRenderQueue& RenderQueue = m_pSceneManager->GetRenderQueue();
			if(m_uRenderList == 0)
				m_uRenderList = RenderQueue.AddRenderList(false);
			else
				RenderQueue.ClearRenderTargetQueue(m_uRenderList);

			for (i=0; i<m_uSceneNodeNum; ++i)
			{
				m_ppSceneNodes[i] = ppNodes[i];
				m_ppSceneNodes[i]->AddRef();
				m_ppSceneNodes[i]->ApplyForRender(~0, m_uRenderList);
			}
		}
	}

	if(!m_pCamera)
	{
		ALCHEMY_DEBUG_NEW(m_pCamera, CCamera);
	}

	CCamera& SceneCamera = m_pSceneManager->GetSceneCamera();
	m_pCamera->SetProjection(90.0f, SceneCamera.GetNearPlane(), SceneCamera.GetFarPlane(), 1.0f);

	return true;
}

void CEnvironmentRender::Reset()
{
	if(m_ppSceneNodes && m_uRenderList && m_uSceneNodeNum)
	{
		for (UINT i=0; i<m_uSceneNodeNum; ++i)
		{
			m_ppSceneNodes[i] = m_ppSceneNodes[i];
			m_ppSceneNodes[i]->AddRef();
			m_ppSceneNodes[i]->ApplyForRender(~0, m_uRenderList);
		}
	}
}

void CEnvironmentRender::RenderAll()
{
	if(!m_pWorldMatrix || !m_pSceneManager || !m_RenderTarget || !m_pCamera)
		return;

	static CMatrix4x4 s_CameraFaceWorldMatrix;
	static CQuaternion s_Rotate;

	const CCamera* pCamera = m_pSceneManager->GetCamera(0);
	static CRenderQueue& RenderQueue = m_pSceneManager->GetRenderQueue();
	CGraphicsResource::HANDLE RenderTarget = RenderQueue.GetRenderTarget();

	FLOAT3 ScenePos;
	m_pWorldMatrix->GetPosition(ScenePos);

	static FLOAT s_RotateVal = g_fDEG2RAD * 90.0f;

	m_pSceneManager->SetCamera(m_pCamera, 0);

	for (UINT i=0; i<6; ++i)
	{
		switch(i)
		{
		case CTexture::POSITIVE_X:
			s_Rotate.RotationYawPitchRoll(s_RotateVal, 0.0f, 0.0f);
			break;
		case CTexture::NEGATIVE_X:
			s_Rotate.RotationYawPitchRoll(-s_RotateVal, 0.0f, 0.0f);
			break;
		case CTexture::POSITIVE_Y:
			s_Rotate.RotationYawPitchRoll(0.0f, -s_RotateVal, 0.0f);
			break;
		case CTexture::NEGATIVE_Y:
			s_Rotate.RotationYawPitchRoll(0.0f, s_RotateVal, 0.0f);
			break;
		case CTexture::POSITIVE_Z:
			s_Rotate.RotationYawPitchRoll(0.0f, 0.0f, 0.0f);
			break;
		case CTexture::NEGATIVE_Z:
			s_Rotate.RotationYawPitchRoll(-s_RotateVal*2.0f, 0.0f, 0.0f);
			break;
		}

		s_CameraFaceWorldMatrix.RotationQuaternion(s_Rotate);
		s_CameraFaceWorldMatrix._41 = ScenePos.x; s_CameraFaceWorldMatrix._42 = ScenePos.y; s_CameraFaceWorldMatrix._43 = ScenePos.z;
		m_pCamera->SetMatrix(s_CameraFaceWorldMatrix);
		m_pCamera->Update();
		RenderQueue.SetRenderTarget(m_RenderTarget, i << CTexture::CUBE_TEXTURE_SURFACE_SHIFT);

		if(m_ppSceneNodes && m_uRenderList)
			RenderQueue.RenderScene(m_uRenderList);
		else
			m_pSceneManager->RenderScene( ALCHEMY_FLAG(CRenderSystem::TARGET) | ALCHEMY_FLAG(CRenderSystem::ZBUFFER) | ALCHEMY_FLAG(CRenderSystem::STENCIL) );
	}

	m_pSceneManager->SetCamera(pCamera, 0);
	m_pSceneManager->GetRenderQueue().SetRenderTarget(RenderTarget);
}