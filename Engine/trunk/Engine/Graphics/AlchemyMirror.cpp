#include "AlchemyMirror.h"
#include "AlchemySceneManager.h"
//#include "AlchemyViewportRenderTargetListener.h"
#include "AlchemyPositionEffect.h"
#include "AlchemyCameraDirectionEffect.h"
#include "AlchemyWaterNormalMapEffect.h"
#include "AlchemyTexture2DEffect.h"
#include "AlchemyNormalOffsetForceTextureEffect.h"

using namespace alchemy;

CMirror::CMirror(void) :
m_pPlaneMesh(ALCHEMY_NULL),
//m_pReflectionMap(ALCHEMY_NULL),
m_pMaterial(ALCHEMY_NULL),
m_pEffect(ALCHEMY_NULL),
m_pRenderMethod(ALCHEMY_NULL),
m_pSceneManager(ALCHEMY_NULL),
m_bIsYUp(false),
m_fReflectionScale(0.1f)
{
}

CMirror::~CMirror(void)
{
	
}
//#include <stdio.h>
bool CMirror::Create(
					 FLOAT fWidth,
					 FLOAT fHeight, 
					 UINT uWidthSegment,
					 UINT uHeightSegment,
					 UINT uReflectionMap,
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
	const LPTEXTURE* ppReflectMap = ALCHEMY_NULL;
	LPTEXTURE pTexture;

	if(uNormalMap)
	{
		ppNormalMap = m_pMaterial->GetTextures().Get(uNormalMap - 1);
		if(ppNormalMap)
			-- uNormalMap;
		else
		{
			pTexture = static_cast<LPTEXTURE>( RenderSystem.GetResource(uNormalMap, CGraphicsResource::TEXTURE) );

			if(pTexture)
			{
				ppNormalMap  = &pTexture;
				uNormalMap = m_pMaterial->GetTextures().GetLength();
				m_pMaterial->AddTexture( pTexture->GetResourceHandle() );
			}
		}
	}

	if(uReflectionMap)
	{
		ppReflectMap = m_pMaterial->GetTextures().Get(uReflectionMap - 1);
		if(ppReflectMap)
			-- uReflectionMap;
		else
		{
			pTexture = static_cast<LPTEXTURE>( RenderSystem.GetResource(uReflectionMap, CGraphicsResource::TEXTURE) );

			if(pTexture)
			{
				ppReflectMap  = &pTexture;
				uReflectionMap = m_pMaterial->GetTextures().GetLength();
				m_pMaterial->AddTexture( pTexture->GetResourceHandle() );
			}
		}
	}
	else
	{
		ALCHEMY_DEBUG_WARNING("Error ReflectionMap - CMirror::Create()");

		return false;
	}

	CEffectCode EffectCode( RenderSystem.GetCompiler() );
	CEffectCode::CPassDefiner PassDefiner(EffectCode);
	CPositionEffect PositionEffect;
	CCameraDirectionEffect CameraDirectionEffect;
	CWaterNormalMapEffect WaterNormalMapEffect(m_Translation0, m_Translation1, m_Translation2, m_Translation3, uNormalMap);
	CTexture2DEffect Texture2DEffect( ppNormalMap ? false : true, uReflectionMap );
	CNormalOffsetForceTextureEffect NormalOffsetForceTextureEffect( m_fReflectionScale, 0, uReflectionMap );

	PassDefiner.AddInfo(PositionEffect, ALCHEMY_NULL);

	if(ppNormalMap)
	{
		PassDefiner.AddInfo(CameraDirectionEffect, ALCHEMY_NULL);
		PassDefiner.AddInfo(WaterNormalMapEffect, ALCHEMY_NULL);
	}

	PassDefiner.AddInfo(Texture2DEffect, ALCHEMY_NULL);

	if(ppNormalMap)
		PassDefiner.AddInfo(NormalOffsetForceTextureEffect, ALCHEMY_NULL);

	UINT i, uColorKeyCount = 0;
	for(i = 0; i < uEffectInfoCount; ++ i)
		uColorKeyCount = ALCHEMY_MAXINUM(uColorKeyCount, pEffectInfos[i].uInputColorKey);

	PUINT puColorKey;
	ALCHEMY_DEBUG_NEW(puColorKey, UINT[uColorKeyCount]);
	for(i = 0; i < uColorKeyCount; ++ i)
		puColorKey[i] = PassDefiner.AddColorKey();

	for(i = 0; i < uEffectInfoCount; ++ i)
	{
		if(pEffectInfos->pEffectInfo)
			PassDefiner.AddInfo(*pEffectInfos->pEffectInfo, pEffectInfos->pBlendMethod, pEffectInfos->uInputColorKey ? puColorKey[pEffectInfos->uInputColorKey - 1] : 0, pEffectInfos->uOutputColorKey ? puColorKey[pEffectInfos->uOutputColorKey - 1] : 0);
		else
			PassDefiner.AddInfo(pEffectInfos->uInputColorKey ? puColorKey[pEffectInfos->uInputColorKey - 1] : 0, pEffectInfos->pBlendMethod, pEffectInfos->uSourceColorKey, pEffectInfos->uOutputColorKey ? puColorKey[pEffectInfos->uOutputColorKey - 1] : 0);

		++ pEffectInfos;
	}

	ALCHEMY_DEBUG_DELETE_ARRAY(puColorKey);

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

	m_pEffect = RenderSystem.CreateEffect(EffectCode);

	if(!m_pEffect)
	{
		Destroy();

		ALCHEMY_DEBUG_WARNING("CMirror::Create:  Effect has been created fail.");

		return false;
	}

	m_pRenderMethod = SceneManager.CreateRenderMethod(EffectCode);

	if(!m_pRenderMethod)
	{
		Destroy();

		ALCHEMY_DEBUG_WARNING("CMirror::Create:  RenderMethod has been created fail.");

		return false;
	}

	const CEffect::PASSINFO* pPassInfo = m_pEffect->GetPassInfo(0, 0);
	const CVertexDeclaration::ELEMENT* pElement;

	CVertexDeclaration::LPVERTEXELEMENT pVertexElements, pVertexElement;
	ALCHEMY_DEBUG_NEW(pVertexElements, CVertexDeclaration::VERTEXELEMENT[pPassInfo->uVertexElementCount]);

	UINT uOffset = 0;
	for(UINT i = 0; i < pPassInfo->uVertexElementCount; ++ i)
	{
		pVertexElement = &pVertexElements[i];
		pElement       = &pPassInfo->pVertexElements[i];

		pVertexElement->uStream = 0;
		pVertexElement->uOffset = uOffset;
		pVertexElement->Type    = pElement->Type;
		pVertexElement->Usage   = pElement->Usage;
		pVertexElement->uIndex  = pElement->uIndex;

		uOffset += pElement->Type;
	}

	UINT uUsage = ALCHEMY_FLAG(INDEXBUFFER::WRITE_ONLY);

	UINT uPlaneData = ALCHEMY_MESH_MANAGER.CreatePlane( fWidth, fHeight, uWidthSegment, uHeightSegment, uWidthSegment, uWidthSegment, bIsYUp, pVertexElements, pPassInfo->uVertexElementCount, ALCHEMY_NULL, &uUsage, ALCHEMY_FLAG(INDEXBUFFER::WRITE_ONLY) );

	ALCHEMY_DEBUG_DELETE_ARRAY(pVertexElements);

	const CMeshManager::MESHDATA* pMeshData = ALCHEMY_MESH_MANAGER.GetMeshData(uPlaneData);

	if(!pMeshData || ( m_pPlaneMesh = SceneManager.CreateMesh(uPlaneData) ) == ALCHEMY_NULL)
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

bool CMirror::ApplyForRender(UINT uRenderPass, UINT uRenderList)
{
	if( !__IsValuable() )
		return false;

	CRenderQueue::RENDERENTRY& Entry = m_pSceneManager->GetRenderQueue().LockRenderEntry(*this);

	Entry.uRenderTarget      = 0;
	Entry.uEffect            = m_pEffect->GetResourceHandle();
	Entry.uRenderPass        = 0;
	Entry.uVertexDeclaration = m_pPlaneMesh->GetVertex().GetVertexDeclaration().GetResourceHandle();
	Entry.uModel             = m_pPlaneMesh->GetResourceHandle();
	Entry.uModelParameter    = 0;

	Entry.uUserData          = 0;

	m_pSceneManager->GetRenderQueue().UnlockRenderEntry( CRenderQueue::FINAL, ALCHEMY_FLAG(CRenderQueue::AUTO_SHADER) );

	return true;
}

void CMirror::Render(const CRenderQueue::RENDERENTRY& Entry, const CRenderQueue::RENDERKEY& Key, UINT uFlag)
{
	if( !__IsValuable() )
		return;

	/*if( m_pSceneManager->GetRenderQueue().GetRenderTarget() == m_pReflectionMap->GetResourceHandle() )
		return;*/

	if( ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_RENDER_TARGET) )
		m_pSceneManager->GetRenderQueue().SetRenderTarget();

	if( ALCHEMY_TEST_BIT(uFlag, CRenderQueue::PARENT) || ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_EFFECT) || ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_EFFECT_PASS) )
	{
		CRenderSystem& RenderSystem = m_pSceneManager->GetRenderSystem();
		if(ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_EFFECT) && Entry.uEffect)
			RenderSystem.SetShader(Entry.uEffect);

		if(ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_EFFECT_PASS) && Entry.uRenderPass)
			RenderSystem.SetShader(Entry.uRenderPass);

		RenderSystem.ResetTexture();

		m_pRenderMethod->Activate(Key.uRenderPass, Key.uRenderTargetIndex);

		m_pRenderMethod->CommitChange(&m_WorldMatrix, m_pMaterial);
	}

	m_pPlaneMesh->DrawSubset( 0, ALCHEMY_TEST_BIT(uFlag, CRenderQueue::MODEL), ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_VERTEX_DECLARATION) );
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

	/*if(m_pReflectionMap)
	{
		m_pReflectionMap->Release();

		m_pReflectionMap = ALCHEMY_NULL;
	}*/

	if(m_pMaterial)
	{
		m_pMaterial->Release();

		m_pMaterial = ALCHEMY_NULL;
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

	m_pSceneManager = ALCHEMY_NULL;

	//ALCHEMY_DEBUG_DELETE(m_pReflectionMapInfo);
}