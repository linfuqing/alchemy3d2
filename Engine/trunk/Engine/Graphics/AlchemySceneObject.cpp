#include "AlchemySceneObject.h"
#include "AlchemySceneManager.h"
#include "AlchemyMeshProcessorInterface.h"

using namespace alchemy;

CSceneObject::CSceneObject() :
m_pMeshProcessor(ALCHEMY_NULL),
m_pSourceMesh(ALCHEMY_NULL)
{
	
}

CSceneObject::~CSceneObject(void)
{
}

void CSceneObject::SetMeshProcessor(IMeshProcessor* pMeshProcessor)
{
	if(!m_pSourceMesh)
	{
		m_pSourceMesh = GetMesh();

		m_pSourceMesh->AddRef();
	}

	if(pMeshProcessor)
		pMeshProcessor->AddRef();

	if(m_pMeshProcessor)
		m_pMeshProcessor->Release();

	m_pMeshProcessor = pMeshProcessor;

	if(m_pSourceMesh)
		SetMesh( m_pSourceMesh->GetResourceHandle() );
}

CSceneNode& CSceneObject::Clone(ISceneNodeFactory& SceneNodeFactory)const
{
	CSceneObject& SceneObject = SceneNodeFactory.CreateSceneObject();

	SceneObject.SetName( GetName() );

	CTransform::Clone(SceneObject);

	CMesh* pMesh = GetMesh();
	CEffect* pEffect = GetEffect();
	CMaterial* pMaterial = GetMaterial();
	CSceneManager* pSceneManager = GetSceneManager();
	SceneObject.Create(pMesh ? pMesh->GetResourceHandle() : 0, GetMeshSubset(), pEffect ? pEffect->GetResourceHandle() : 0, GetRenderMethod(), GetRenderPass(), pMaterial ? pMaterial->GetResourceHandle() : 0, pSceneManager);

	IBoundingVolume* pBoundingVolume = GetBoundingVolume();

	if(pBoundingVolume)
	{
		pBoundingVolume = pBoundingVolume->Clone(ALCHEMY_NULL);

		SceneObject.SetBoundingVolume(pBoundingVolume);

		pBoundingVolume->Release();
	}

	if(m_pMeshProcessor)
	{
		m_pMeshProcessor->AddRef();

		SceneObject.m_pMeshProcessor = m_pMeshProcessor;
	}

	return SceneObject;
}

void CSceneObject::Destroy()
{
	CStaticObject::Destroy();

	if(m_pMeshProcessor)
	{
		m_pMeshProcessor->Release();

		m_pMeshProcessor = ALCHEMY_NULL;
	}

	if(m_pSourceMesh)
	{
		m_pSourceMesh->Release();

		m_pSourceMesh = ALCHEMY_NULL;
	}
}

void CSceneObject::Render(const CRenderQueue::RENDERENTRY& Entry, const CRenderQueue::RENDERKEY& Key, UINT uFlag)
{
	CMesh* pMesh = GetMesh();
	CEffect* pEffect = GetEffect();
	CSceneManager* pSceneManager = GetSceneManager();
	if(!pSceneManager || !pEffect || !pMesh)
		return;

	CRenderQueue& RenderQueue = pSceneManager->GetRenderQueue();
	CRenderSystem& RenderSystem = pSceneManager->GetRenderSystem();

	CMaterial* pMaterial = GetMaterial();
	IRenderMethod* pRenderMethod = GetRenderMethod();
	if(pMaterial && pRenderMethod)
	{
		UINT uTextureCount;
		const UINT* pTextureAlphaIndexes = pRenderMethod->GetTextureInfo(Key.uRenderPass, Key.uRenderTargetIndex, uTextureCount);
		if(pTextureAlphaIndexes)
		{
			const LPTEXTURE* ppTexture;
			for(UINT i = 0; i < uTextureCount; ++ i)
			{
				ppTexture = pMaterial->GetTextures().Get( pTextureAlphaIndexes[i] );

				if( ppTexture && *ppTexture && ALCHEMY_TEST_BIT( (*ppTexture)->GetUsage(), CTexture::RENDER_TARGET ) && (*ppTexture)->GetResourceHandle() == RenderQueue.GetRenderTarget() )
					return;
			}
		}
	}

	if( ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_RENDER_TARGET) )
	{
		if(Entry.uRenderTarget)
			pSceneManager->GetRenderQueue().SetRenderTarget(Entry.uRenderTarget, 0, false);
		else
			pSceneManager->GetRenderQueue().SetRenderTarget();

		RenderQueue.ActivateMultipleRenderTarget( Key.uRenderTargetIndex, pEffect->GetRenderTargetCount(Key.uRenderPass) );
	}

	if(ALCHEMY_TEST_BIT(uFlag, CRenderQueue::PARENT) || ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_EFFECT) || ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_EFFECT_PASS) )
	{
		if( ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_EFFECT) && Entry.uEffect )
			RenderSystem.SetShader(Entry.uEffect);

		if( ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_EFFECT_PASS) && Entry.uRenderPass )
			RenderSystem.SetShader(Entry.uRenderPass);
	}

	if(pRenderMethod && !pRenderMethod->Activate(Key.uRenderPass, Key.uRenderTargetIndex) )
		return;

	RenderSystem.ResetTexture();

	RenderSystem.RegisterRenderState();

	if(pRenderMethod)
		pRenderMethod->CommitChange( &GetWorldMatrix(), pMaterial);

	if( ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_VERTEX_DECLARATION) && ( !Entry.uVertexDeclaration || !RenderSystem.SetVertexDeclaration(Entry.uVertexDeclaration) ) )
		return;

	if(m_pMeshProcessor)
		m_pMeshProcessor->OnProcess(*pRenderMethod, Entry.uVertexDeclaration ? ALCHEMY_CLEAR_BIT(uFlag, CRenderQueue::ENTRY_VERTEX_DECLARATION) : uFlag);
	else
		pMesh->DrawSubset( Entry.uModelParameter, ALCHEMY_TEST_BIT(uFlag, CRenderQueue::MODEL), ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_VERTEX_DECLARATION) && !Entry.uVertexDeclaration);

	RenderSystem.RestoreRenderState();
}

CMesh* CSceneObject::_ConvertMesh(CMesh* pSourceMesh)
{
	m_pSourceMesh = pSourceMesh;

	CMesh* pMesh = m_pSourceMesh;

	if(m_pMeshProcessor && m_pSourceMesh)
	{
		m_pSourceMesh->AddRef();

		pMesh = m_pMeshProcessor->ConvertMesh( *m_pSourceMesh, GetMeshSubset() );
	}

	return CStaticObject::_ConvertMesh(pMesh);
}