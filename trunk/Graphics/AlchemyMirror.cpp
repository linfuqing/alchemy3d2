#include "AlchemyMirror.h"
#include "AlchemySceneManager.h"

#include "AlchemyPositionEffect.h"
#include "AlchemyWaterNormalMapEffect.h"
#include "AlchemyTexture2DEffect.h"
#include "AlchemyNormalOffsetTextureEffect.h"

using namespace alchemy;

CMirror::CMirror(void) :
m_pPlaneMesh(ALCHEMY_NULL),
m_pReflectionMap(ALCHEMY_NULL),
m_pMaterial(ALCHEMY_NULL),
m_pRenderMethod(ALCHEMY_NULL),
m_pSceneManager(ALCHEMY_NULL),
m_bIsYUp(false),
m_pReflectionMapInfo(ALCHEMY_NULL),
m_fReflectionScale(0.1f)
{
}

CMirror::~CMirror(void)
{
	ALCHEMY_DEBUG_DELETE(m_pReflectionMapInfo);
}
//#include <stdio.h>
bool CMirror::Create(
					 FLOAT fWidth,
					 FLOAT fHeight, 
					 UINT uWidthSegment,
					 UINT uHeightSegment,
					 INT nReflectionMapShift, 
					 UINT uNormalMap,
					 CGraphicsResource::HANDLE Material,
					 const CEffectCode::CPassDefiner::EFFECTINFO* pEffectInfos, 
					 UINT uEffectInfoCount,
					 bool bIsYUp)
{
	__Destroy();

	if( !CSceneManager::GetInstance() )
	{
		ALCHEMY_DEBUG_WARNING("CMirror::Create:  Mirror can not be created without a activate scene manager.");

		return false;
	}

	CSceneManager& SceneManager = *CSceneManager::GetInstance();

	m_pMaterial = Material ? static_cast<CMaterial*>( SceneManager.GetResource(Material, CGraphicsResource::MATERIAL) ) : SceneManager.CreateMaterial();

	if(!m_pMaterial)
	{
		__Destroy();

		ALCHEMY_DEBUG_WARNING("CMirror::Create:  Material has been created fail.");

		return false;
	}

	if(Material)
		m_pMaterial->AddRef();

	CRenderSystem& RenderSystem = SceneManager.GetRenderSystem();

	const LPTEXTURE* ppNormalMap = ALCHEMY_NULL;

	if(uNormalMap)
	{
		ppNormalMap = m_pMaterial->GetTextures().Get(uNormalMap - 1);
		if(ppNormalMap)
			-- uNormalMap;
		else
		{
			LPTEXTURE pTexture = static_cast<LPTEXTURE>( RenderSystem.GetResource(uNormalMap, CGraphicsResource::TEXTURE) );

			if(pTexture)
			{
				ppNormalMap  = &pTexture;
				uNormalMap = m_pMaterial->GetTextures().GetLength();
				m_pMaterial->AddTexture( pTexture->GetResourceHandle() );
			}
		}
	}

	ALCHEMY_DEBUG_NEW( m_pReflectionMapInfo, CViewportTextureInfo(RenderSystem) );

	m_pReflectionMapInfo->SetShift(nReflectionMapShift);

	m_pReflectionMap = SceneManager.GetRenderSystem().CreateTexture(*m_pReflectionMapInfo, ALCHEMY_FLAG(TEXTURE::WRITE_ONLY) | ALCHEMY_FLAG(CTexture::RENDER_TARGET), CTexture::A8R8G8B8, 1);

	if(!m_pReflectionMap)
	{
		__Destroy();

		ALCHEMY_DEBUG_WARNING("CMirror::Create:  Reflection map has been created fail.");

		return false;
	}

	CEffectCode EffectCode( RenderSystem.GetComplier() );
	CEffectCode::CPassDefiner PassDefiner(EffectCode);
	CPositionEffect PositionEffect;
	CWaterNormalMapEffect WaterNormalMapEffect(m_Translation0, m_Translation1, m_Translation2, m_Translation3, uNormalMap);
	CTexture2DEffect Texture2DEffect( ppNormalMap ? false : true, m_pMaterial->GetTextures().GetLength() );
	CNormalOffsetTextureEffect NormalOffsetTextureEffect( m_fReflectionScale, 0, m_pMaterial->GetTextures().GetLength() );

	m_pMaterial->AddTexture( m_pReflectionMap->GetResourceHandle() );

	PassDefiner.AddInfo(PositionEffect, ALCHEMY_NULL);

	if(ppNormalMap)
		PassDefiner.AddInfo(WaterNormalMapEffect, ALCHEMY_NULL);

	PassDefiner.AddInfo(Texture2DEffect, ALCHEMY_NULL);

	if(ppNormalMap)
		PassDefiner.AddInfo(NormalOffsetTextureEffect, ALCHEMY_NULL);

	for(UINT i = 0; i < uEffectInfoCount; ++ i)
	{
		if(pEffectInfos->pEffectInfo)
			PassDefiner.AddInfo(*pEffectInfos->pEffectInfo, pEffectInfos->pBlendMethod);

		++ pEffectInfos;
	}

	/*UINT uPass = */PassDefiner.EndDefine();

	//const CEffectCode::PASS* pPass = EffectCode.GetPass(uPass);

	//STRING Out;

	////生成代码到Out中
	//EffectCode.BuildShaderFunctionCode(Out, pPass->uVertexShader);
	//wprintf(ALCHEMY_TEXT("Vertex Shader:\n"));
	//wprintf(static_cast<const CHAR*>(Out));

	//EffectCode.BuildShaderFunctionCode(Out, pPass->uFragmentShader);
	//wprintf(ALCHEMY_TEXT("\nFragment Shader:\n"));
	//wprintf(static_cast<const CHAR*>(Out));

	m_pRenderMethod = SceneManager.CreateRenderMethod(EffectCode);

	if(!m_pRenderMethod)
	{
		Destroy();

		ALCHEMY_DEBUG_WARNING("CMirror::Create:  RenderMethod has been created fail.");

		return false;
	}

	UINT uVertexElementCount = 0;
	const CEffectCode::ELEMENTPARAMETER* pVertexElementParameters = m_pRenderMethod->GetVertexElementParameters(0, &uVertexElementCount), *pVertexElementParameter;

	CVertexDeclaration::LPVERTEXELEMENT pVertexElements, pVertexElement;
	ALCHEMY_DEBUG_NEW(pVertexElements, CVertexDeclaration::VERTEXELEMENT[uVertexElementCount]);

	PUINT puUsage;
	ALCHEMY_DEBUG_NEW(puUsage, UINT[uVertexElementCount]);

	UINT uOffset = 0;
	for(UINT i = 0; i < uVertexElementCount; ++ i)
	{
		pVertexElement          = &pVertexElements[i];
		pVertexElementParameter = &pVertexElementParameters[i];

		pVertexElement->uStream = 0;
		pVertexElement->uOffset = uOffset;
		pVertexElement->Type    = pVertexElementParameter->Type;
		pVertexElement->Usage   = pVertexElementParameter->Usage;

		uOffset += pVertexElementParameter->Type;

		puUsage[i] = ALCHEMY_FLAG(INDEXBUFFER::WRITE_ONLY);
	}

	UINT uPlaneData = ALCHEMY_MESH_MANAGER.CreatePlane( fWidth, fHeight, uWidthSegment, uHeightSegment, uWidthSegment, uWidthSegment, bIsYUp, pVertexElements, uVertexElementCount, ALCHEMY_NULL, puUsage, ALCHEMY_FLAG(INDEXBUFFER::WRITE_ONLY) );

	ALCHEMY_DEBUG_DELETE_ARRAY(pVertexElements);
	ALCHEMY_DEBUG_DELETE_ARRAY(puUsage);

	const CMeshManager::MESHDATA* pMeshData = ALCHEMY_MESH_MANAGER.GetMeshData(uPlaneData);

	if(!pMeshData || ( m_pPlaneMesh = SceneManager.CreateMesh(*pMeshData) ) == ALCHEMY_NULL)
	{
		__Destroy();

		ALCHEMY_DEBUG_WARNING("CMirror::Create:  Mesh has been created fail.");

		return false;
	}

	ALCHEMY_MESH_MANAGER.DestroyMeshData(uPlaneData);

	if(bIsYUp)
	{
		m_Plane.x =   0.0f;
		m_Plane.y =   1.0f;
		m_Plane.z =   0.0f;
		m_Plane.w = - 1.0f;
	}
	else
	{
		m_Plane.x =   0.0f;
		m_Plane.y =   0.0f;
		m_Plane.z =   1.0f;
		m_Plane.w = - 1.0f;
	}

	m_bIsYUp = bIsYUp;

	m_pSceneManager = &SceneManager;

	return true;
}

void CMirror::UpdateTransform()
{
	CSceneNode::UpdateTransform();

	if(m_bIsYUp)
	{
		m_Plane.x = 0.0f;
		m_Plane.y = 1.0f;
		m_Plane.z = 0.0f;
		m_Plane.w = 0.0f;
	}
	else
	{
		m_Plane.x = 0.0f;
		m_Plane.y = 0.0f;
		m_Plane.z = 1.0f;
		m_Plane.w = 0.0f;
	}

	m_Plane.Transform(m_WorldMatrix);
}

bool CMirror::ApplyForRender()
{
	if( !__IsValuable() )
		return false;

	/*m_RenderEntry.uEffect            = m_pRenderMethod->GetEffect().GetResourceHandle();
	m_RenderEntry.uRenderPass        = 0;
	m_RenderEntry.uVertexDeclaration = m_pPlaneMesh->GetVertex().GetVertexDeclaration().GetResourceHandle();
	m_RenderEntry.uModel             = m_pPlaneMesh->GetResourceHandle();
	m_RenderEntry.uDepth             = uDepth;
	m_RenderEntry.uModelType         = CRenderQueue::UNKNOWN_TYPE;
	m_RenderEntry.uModelParameter    = 0;

	m_RenderEntry.pParent            = this;
	m_RenderEntry.uUserData          = 0;

	m_pSceneManager->AddRenderEntryCache(m_RenderEntry);*/

	CRenderQueue::RENDERENTRY& Entry = m_pSceneManager->LockRenderEntry(*this);

	Entry.uRenderTarget      = 0;
	Entry.uEffect            = m_pRenderMethod->GetEffect().GetResourceHandle();
	Entry.uRenderPass        = 0;
	Entry.uVertexDeclaration = m_pPlaneMesh->GetVertex().GetVertexDeclaration().GetResourceHandle();
	Entry.uModel             = m_pPlaneMesh->GetResourceHandle();
	Entry.uModelParameter    = 0;

	Entry.uUserData          = 0;

	m_pSceneManager->UnlockRenderEntry(CSceneManager::EFFECT_FINAL);

	return true;
}

void CMirror::Render(const CRenderQueue::RENDERENTRY& Entry, UINT uFlag)
{
	if( !__IsValuable() )
		return;

	CRenderSystem& RenderSystem = m_pSceneManager->GetRenderSystem();

	CGraphicsResource::HANDLE RenderTarget = m_pSceneManager->GetSceneRenderTarget();

	m_pSceneManager->SetSceneRenderTarget( m_pReflectionMap->GetResourceHandle() );
	//RenderSystem.Clear(0, CRenderSystem::TARGET | CRenderSystem::ZBUFFER | CRenderSystem::STENCIL);

	CCamera& Camera = m_pSceneManager->GetSceneCamera();
	m_pSceneManager->SetCamera(Camera);

	static CMatrix4x4 s_LocalMatrix;

	s_LocalMatrix = Camera.GetLocalMatrix();

	static CMatrix4x4 s_ReflectMatrix;

	s_ReflectMatrix.Reflect(m_Plane);

	Multiply4x3(s_ReflectMatrix, s_ReflectMatrix, s_LocalMatrix);

	Camera.SetMatrix(s_ReflectMatrix);
	Camera.SetClipPlane(m_Plane);
	Camera.Update();

	RenderSystem.SetRenderState(CRenderSystem::CULL_MODE, CRenderSystem::CW);
	m_pSceneManager->RenderNormalScene( ALCHEMY_FLAG(CRenderSystem::TARGET) | ALCHEMY_FLAG(CRenderSystem::ZBUFFER) );
	RenderSystem.SetRenderState(CRenderSystem::CULL_MODE, CRenderSystem::CCW);

	Camera.SetMatrix(s_LocalMatrix);
	Camera.SetProjection();
	Camera.Update();

	Camera.SetDirtyFlag(0);

	m_pSceneManager->SetSceneRenderTarget(RenderTarget);

	CEffect& Effect = m_pRenderMethod->GetEffect();

	Effect.Begin();

	Effect.BeginPass(Entry.uRenderPass);

	m_pRenderMethod->Activate(0);
	//m_pRenderMethod->SetWorldMatrix(m_WorldMatrix);
	//m_pRenderMethod->SetCamera(Camera);
	//m_pRenderMethod->SetMaterial(*m_pMaterial);
	//m_pRenderMethod->CommitTransform();
	m_pRenderMethod->SetParameters(m_WorldMatrix, Camera, *m_pMaterial);

	Effect.CommitChange();

	m_pPlaneMesh->DrawSubset(0, true, true);
}

void CMirror::Destroy()
{
	__Destroy();
}

void CMirror::__Destroy()
{
	if(m_pPlaneMesh)
	{
		m_pPlaneMesh->Release();

		m_pPlaneMesh = ALCHEMY_NULL;
	}

	if(m_pReflectionMap)
	{
		m_pReflectionMap->Release();

		m_pReflectionMap = ALCHEMY_NULL;
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

	m_pSceneManager = ALCHEMY_NULL;

	ALCHEMY_DEBUG_DELETE(m_pReflectionMapInfo);
}