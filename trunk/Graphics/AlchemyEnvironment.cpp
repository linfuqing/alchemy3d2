#include "AlchemyEnvironment.h"
#include "AlchemyEnvironmentalEffect.h"
#include "AlchemyPositionEffect.h"
#include "AlchemyTextureEffect.h"
#include "AlchemyMeasureColorEffect.h"

using namespace alchemy;

CEnvironment::CEnvironment()
:m_pMesh(ALCHEMY_NULL),
m_pMaterial(ALCHEMY_NULL),
m_pEnvironmentTexture(ALCHEMY_NULL),
m_pRenderMethod(ALCHEMY_NULL),
m_pSceneManager(ALCHEMY_NULL),
m_pVertexDeclartion(ALCHEMY_NULL),
m_bIsRenderring(false)
{

}

CEnvironment::~CEnvironment()
{
	Destroy();
}

bool CEnvironment::Create(CGraphicsResource::HANDLE Mesh, CGraphicsResource::HANDLE Material, UINT uEnvironmentSize, CSceneManager& SceneManager, CCompilerInterface& Compiler, CEffectCode::CPassDefiner::EFFECTINFO * pPassInfo, UINT uPassCount)
{
	m_pSceneManager = &SceneManager;

	m_pMesh = static_cast<CMesh*>( m_pSceneManager->GetResource( Mesh, CGraphicsResource::MESH ) );
	if(!m_pMesh)
	{
		ALCHEMY_DEBUG_WARNING("CEnvironment::Create: invalid mesh handle.");
		Destroy();
		return false;
	}

	m_pMesh->AddRef();

	m_pMaterial = static_cast<CMaterial*>( m_pSceneManager->GetResource( Material, CGraphicsResource::MATERIAL ) );
	if(!m_pMaterial)
	{
		ALCHEMY_DEBUG_WARNING("CEnvironment::Create: invalid material handle.");
		Destroy();
		return false;
	}

	m_pMaterial->AddRef();

	m_pEnvironmentTexture = m_pSceneManager->GetRenderSystem().CreateCubeTexture(uEnvironmentSize, TEXTURE::RENDER_TARGET, TEXTURE::A8R8G8B8);
	if(!m_pEnvironmentTexture)
	{
		ALCHEMY_DEBUG_WARNING("CEnvironment::Create: create cubeTexture error.");
		Destroy();
		return false;
	}


	CEffectCode Code(Compiler);
	CEffectCode::CPassDefiner Pass(Code);

	CPositionEffect PosEffect;
	CEnvironmentalEffect EnvironmentEffect;

	CEffectCode::SAMPLERDECLARATION sampler;
	sampler.Type      = CEffectCode::TEXTURE_CUBE;
	sampler.MinFilter = CEffectCode::LINEAR;
	sampler.MagFilter = CEffectCode::LINEAR;
	sampler.MipFilter = CEffectCode::LINEAR;
	sampler.AddressU  = CEffectCode::CLAMP;
	sampler.AddressV  = CEffectCode::CLAMP;
	sampler.AddressW  = CEffectCode::CLAMP;

	CTextureEffect CubeTextureEffect( m_pMaterial->GetTextures().GetLength() , 0, &sampler);

	m_pMaterial->AddTexture( m_pEnvironmentTexture->GetResourceHandle() );

	CMeasureColorEffect MeasureEffect( static_cast<const FLOAT4*>(& m_pMaterial->GetSpecular() ) );

	Pass.AddInfo(PosEffect, ALCHEMY_NULL);
	Pass.AddInfo(EnvironmentEffect, ALCHEMY_NULL);
	Pass.AddInfo(CubeTextureEffect, ALCHEMY_NULL);
	Pass.AddInfo(MeasureEffect, CEffectCode::CPassDefiner::MODULATE);

	/*if(pPassInfo != ALCHEMY_NULL && uPassCount)
	{
		for (UINT i=0; i<uPassCount; ++i)
			Pass.AddInfo( * ( pPassInfo[i].pEffectInfo ), pPassInfo[i].pfnBlendMethod );
	}*/

	Pass.EndDefine();

	m_pRenderMethod = m_pSceneManager->CreateRenderMethod( Code );
	if(!m_pRenderMethod)
	{
		ALCHEMY_DEBUG_WARNING("CEnvironment::Create: create RenderMethod error.");
		Destroy();
		return false;
	}

	//tmp code
	CVertexDeclaration::VERTEXELEMENT VertexElements[] = { {0, 0, CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION}, {0, 16, CVertexDeclaration::FLOAT3, CVertexDeclaration::NORMAL} };

	m_pVertexDeclartion = m_pSceneManager->GetRenderSystem().CreateVertexDeclaration(VertexElements,2);

	return true;
}

bool CEnvironment::ApplyForRender()
{
	if(m_bIsRenderring)
		return true;

	if( !__IsValuable() )
		return false;

	CRenderQueue::RENDERENTRY& Entry = m_pSceneManager->LockRenderEntry(*this);

	Entry.uRenderTarget      = 0;
	Entry.uEffect            = m_pRenderMethod->GetEffect().GetResourceHandle();
	Entry.uRenderPass        = 0;
	Entry.uVertexDeclaration = m_pVertexDeclartion->GetResourceHandle();
	Entry.uModel             = m_pMesh->GetResourceHandle();
	Entry.uModelParameter    = 0;

	Entry.uUserData          = 0;

	m_pSceneManager->UnlockRenderEntry(CSceneManager::EFFECT_FINAL);

	return true;
}

void CEnvironment::Render(const CRenderQueue::RENDERENTRY& Entry, UINT uFlag)
{
	if( !__IsValuable() )
		return;

	m_bIsRenderring = true;

	CRenderSystem& RenderSystem = m_pSceneManager->GetRenderSystem();
	CCamera& Camera = m_pSceneManager->GetSceneCamera();

	FLOAT3 OldCameraPos = Camera.GetTranslation();
	FLOAT3 OldCameraRotate = Camera.GetRotation();

	const FLOAT* pWorldPos = GetWorldMatrix().fPosition[3];
	FLOAT3 fRotate = OldCameraRotate;

	FLOAT3 fTmp = {pWorldPos[0], pWorldPos[1], pWorldPos[2]};
	Camera.SetTranslation( fTmp );

	CGraphicsResource::HANDLE RenderTarget = m_pSceneManager->GetSceneRenderTarget(), EnvironmentTexture = m_pEnvironmentTexture->GetResourceHandle();

	m_pSceneManager->SetSceneRenderTarget(EnvironmentTexture);

	//back plane
	Camera.SetRotation(fRotate);
	Camera.Update();
	RenderSystem.SetRenderTarget(EnvironmentTexture, CTexture::POSITIVE_Z << CTexture::CUBE_TEXTURE_SURFACE_SHIFT, 0);
	m_pSceneManager->RenderNormalScene( ALCHEMY_FLAG(CRenderSystem::TARGET) );


	//left plane
	fRotate.y += -90.0f;
	Camera.SetRotation(fRotate);
	Camera.Update();
	RenderSystem.SetRenderTarget(EnvironmentTexture, CTexture::NEGATIVE_X << CTexture::CUBE_TEXTURE_SURFACE_SHIFT, 0);
	//RenderSystem.Clear(0, CRenderSystem::TARGET);
	m_pSceneManager->RenderNormalScene( ALCHEMY_FLAG(CRenderSystem::TARGET) );

	//front plane
	fRotate.y = OldCameraRotate.y - 180.0f;
	Camera.SetRotation(fRotate);
	Camera.Update();
	RenderSystem.SetRenderTarget(EnvironmentTexture, CTexture::NEGATIVE_Z << CTexture::CUBE_TEXTURE_SURFACE_SHIFT, 0);
	RenderSystem.Clear(0, CRenderSystem::TARGET);
	m_pSceneManager->RenderNormalScene( ALCHEMY_FLAG(CRenderSystem::TARGET) );

	//right plane
	fRotate.y = OldCameraRotate.y - 270.0f;
	Camera.SetRotation(fRotate);
	Camera.Update();
	RenderSystem.SetRenderTarget(EnvironmentTexture, CTexture::POSITIVE_X << CTexture::CUBE_TEXTURE_SURFACE_SHIFT, 0);
	//RenderSystem.Clear(0, CRenderSystem::TARGET);
	m_pSceneManager->RenderNormalScene( ALCHEMY_FLAG(CRenderSystem::TARGET) );

	//top plane
	fRotate.x = OldCameraRotate.x - 90.0f;
	fRotate.y = 0.0f;
	Camera.SetRotation(fRotate);
	Camera.Update();
	RenderSystem.SetRenderTarget(EnvironmentTexture, CTexture::POSITIVE_Y << CTexture::CUBE_TEXTURE_SURFACE_SHIFT, 0);
	//RenderSystem.Clear(0, CRenderSystem::TARGET);
	m_pSceneManager->RenderNormalScene( ALCHEMY_FLAG(CRenderSystem::TARGET) );

	//bottom plane
	fRotate.x = OldCameraRotate.x + 90.0f;
	Camera.SetRotation(fRotate);
	Camera.Update();
	RenderSystem.SetRenderTarget(EnvironmentTexture, CTexture::NEGATIVE_Y << CTexture::CUBE_TEXTURE_SURFACE_SHIFT, 0);
	//RenderSystem.Clear(0, CRenderSystem::TARGET);
	m_pSceneManager->RenderNormalScene( ALCHEMY_FLAG(CRenderSystem::TARGET) );


	Camera.SetTranslation(OldCameraPos);
	Camera.SetRotation(OldCameraRotate);
	Camera.Update();

	m_pSceneManager->SetSceneRenderTarget(RenderTarget);

	RenderSystem.SetVertexDeclaration( m_pVertexDeclartion->GetResourceHandle() );

	CEffect& Effect = m_pRenderMethod->GetEffect();
	Effect.Begin();
	Effect.BeginPass(0);

	m_pRenderMethod->Activate(0);
	//m_pRenderMethod->SetWorldMatrix( GetWorldMatrix() );
	//m_pRenderMethod->SetCamera(Camera);
	//m_pRenderMethod->SetMaterial(*m_pMaterial);
	//m_pRenderMethod->CommitTransform();
	m_pRenderMethod->SetParameters( GetWorldMatrix(), Camera, *m_pMaterial );
	Effect.CommitChange();

	m_pMesh->DrawSubset(0, true, false);

	Effect.EndPass();
	Effect.End();

	m_bIsRenderring = false;
}

void CEnvironment::Destroy()
{
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

	if(m_pRenderMethod)
	{
		m_pRenderMethod->Release();
		m_pRenderMethod = ALCHEMY_NULL;
	}

	if(m_pEnvironmentTexture)
	{
		m_pEnvironmentTexture->Release();
		m_pEnvironmentTexture = ALCHEMY_NULL;
	}

	if(m_pVertexDeclartion)
	{
		m_pVertexDeclartion->Release();
		m_pVertexDeclartion = ALCHEMY_NULL;
	}

	m_pSceneManager = ALCHEMY_NULL;
	m_bIsRenderring = false;
}