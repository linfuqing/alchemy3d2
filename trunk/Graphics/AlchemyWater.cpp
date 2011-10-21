#include "AlchemyWater.h"
#include "AlchemySceneManager.h"

#include "AlchemyAttributeWorldSpaceEffect.h"
#include "AlchemyNormalOffsetTextureEffect.h"
#include "AlchemyFacingEffect.h"

using namespace alchemy;

CWater::CWater(void) :
m_pRefractionMap(ALCHEMY_NULL),
m_pRefractionMapInfo(ALCHEMY_NULL),
m_fRefractionScale(0.02f),
m_NormalMapTranslationScale(0.01f, 0.0f)
{
}

CWater::~CWater(void)
{
	ALCHEMY_DEBUG_DELETE(m_pRefractionMapInfo);
}

bool CWater::Create(
					FLOAT fWidth, 
					FLOAT fHeight, 
					UINT uWidthSegment /* = 1 */, 
					UINT uHeightSegment /* = 1 */, 
					INT nReflectionMapShift /* = 0 */, 
					INT nRefractionMapShift /* = 0 */, 
					UINT uNormalMap,
					CGraphicsResource::HANDLE Material /* = 0 */, 
					const CEffectCode::CPassDefiner::EFFECTINFO* pEffectInfos /* = ALCHEMY_NULL */, 
					UINT uEffectInfoCount)
{
	__Destroy();

	if( !CSceneManager::GetInstance() )
	{
		ALCHEMY_DEBUG_WARNING("CWater::Create:  Water can not be created without a activate scene manager.");

		return false;
	}

	CSceneManager& SceneManager = *CSceneManager::GetInstance();

	CMaterial* pMaterial = Material ? static_cast<CMaterial*>( SceneManager.GetResource(Material, CGraphicsResource::MATERIAL) ) : SceneManager.CreateMaterial();

	if(!pMaterial)
	{
		__Destroy();

		ALCHEMY_DEBUG_WARNING("CWater::Create:  Material has been created fail.");

		return false;
	}

	if(Material)
		pMaterial->AddRef();

	CRenderSystem& RenderSystem = SceneManager.GetRenderSystem();

	const LPTEXTURE* ppNormalMap = ALCHEMY_NULL;
	
	if(uNormalMap)
	{
		ppNormalMap = pMaterial->GetTextures().Get(uNormalMap - 1);
		if(!ppNormalMap)
		{
			LPTEXTURE pTexture = static_cast<LPTEXTURE>( RenderSystem.GetResource(uNormalMap, CGraphicsResource::TEXTURE) );

			if(pTexture)
			{
				ppNormalMap  = &pTexture;
				pMaterial->AddTexture( pTexture->GetResourceHandle() );
				uNormalMap = pMaterial->GetTextures().GetLength();
			}
		}
	}

	ALCHEMY_DEBUG_NEW( m_pRefractionMapInfo, CViewportTextureInfo(RenderSystem) );

	m_pRefractionMapInfo->SetShift(nRefractionMapShift);

	m_pRefractionMap = SceneManager.GetRenderSystem().CreateTexture(*m_pRefractionMapInfo, ALCHEMY_FLAG(TEXTURE::WRITE_ONLY) | ALCHEMY_FLAG(CTexture::RENDER_TARGET), CTexture::A8R8G8B8, 1);

	if(!m_pRefractionMap)
	{
		__Destroy();

		ALCHEMY_DEBUG_WARNING("CWater::Create:  Refraction map has been created fail.");

		return false;
	}

	CNormalOffsetTextureEffect NormalOffsetTextureEffect( m_fRefractionScale, 0, pMaterial->GetTextures().GetLength() );

	pMaterial->AddTexture( m_pRefractionMap->GetResourceHandle() );

	CEffectCode::CPassDefiner::LPEFFECTINFO pCurrentEffectInfos;

	if(ppNormalMap)
		uEffectInfoCount += 2;
	else
		++ uEffectInfoCount;

	ALCHEMY_DEBUG_NEW(pCurrentEffectInfos, CEffectCode::CPassDefiner::EFFECTINFO[uEffectInfoCount]);

	CEffectCode::CPassDefiner::LPEFFECTINFO pEffectInfo = pCurrentEffectInfos;
	UINT uCurrentEffectInfoCount = uEffectInfoCount - 1;

	CFacingEffect FacingEffect(false);
	if(ppNormalMap)
	{
		pEffectInfo->pEffectInfo  = &FacingEffect;
		pEffectInfo->pBlendMethod = ALCHEMY_NULL;

		++ pEffectInfo;

		-- uCurrentEffectInfoCount;
	}

	pEffectInfo->pEffectInfo  = &NormalOffsetTextureEffect;
	pEffectInfo->pBlendMethod = &m_FresnelBlendMethod;

	++ pEffectInfo;

	for(UINT i = 0; i < uCurrentEffectInfoCount; ++ i)
		pEffectInfo[i] = pEffectInfos[i];

	bool bResult = CMirror::Create(fWidth, fHeight, uWidthSegment, uHeightSegment, nReflectionMapShift, uNormalMap, pMaterial->GetResourceHandle(), pCurrentEffectInfos, uEffectInfoCount, true);

	ALCHEMY_DEBUG_DELETE_ARRAY(pCurrentEffectInfos);

	return bResult;
}

void CWater::Update()
{
	CSceneManager* pSceneManager = CMirror::GetSceneManager();
	if(pSceneManager)
	{
		FLOAT fTime = pSceneManager->GetTime();

		m_Translation3.x = m_NormalMapTranslationScale.x * fTime;
		m_Translation3.y = m_NormalMapTranslationScale.y * fTime;

		m_Translation2  = m_Translation3;
		m_Translation2 *= 2.0f;
		m_Translation1  = m_Translation3;
		m_Translation1 *= 4.0f;
		m_Translation0  = m_Translation2;
	}

	CSceneNode::Update();
}

void CWater::Render(const CRenderQueue::RENDERENTRY& Entry, UINT uFlag)
{
	CSceneManager* pSceneManager = CMirror::GetSceneManager();
	if(pSceneManager && m_pRefractionMap)
	{
		CGraphicsResource::HANDLE RenderTarget = pSceneManager->GetSceneRenderTarget();

		pSceneManager->SetSceneRenderTarget( m_pRefractionMap->GetResourceHandle() );

		pSceneManager->RenderNormalScene( ALCHEMY_FLAG(CRenderSystem::TARGET) | ALCHEMY_FLAG(CRenderSystem::ZBUFFER) );

		pSceneManager->SetSceneRenderTarget(RenderTarget);
	}

	CMirror::Render(Entry, uFlag);
}

void CWater::Destroy()
{
	__Destroy();

	CMirror::Destroy();
}

void CWater::__Destroy()
{
	if(m_pRefractionMap)
	{
		m_pRefractionMap->Release();

		m_pRefractionMap = ALCHEMY_NULL;
	}

	ALCHEMY_DEBUG_DELETE(m_pRefractionMapInfo);
}