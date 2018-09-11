#include "AlchemyDeferredLightRenderer.h"

#include "AlchemyPosition2DEffect.h"

#include "AlchemyNormal3Power1MapOutputEffect.h"
#include "AlchemyPositionDepthRGBAMapOutputEffect.h"

#include "AlchemyTextureEffect.h"

#include "AlchemyViewLightnessEffect.h"

#include "AlchemyViewPointLightEffect.h"
#include "AlchemyViewSpotLightEffect.h"

#include "AlchemyDiffseLightEffect.h"
#include "AlchemyViewBlinnPhongEffect.h"

#include "AlchemyMultipleLightBlendMethod.h"

#include "AlchemyEffectInfoAddition.h"
#include "AlchemyEffectInfoModulation.h"

#include "AlchemySSAORGBAMapEffect.h"

#include <math.h>

using namespace alchemy;

CDeferredLightRenderer::CListener::CListener(CDeferredLightRenderer& Parent) :
m_Parent(Parent)
{

}

CDeferredLightRenderer::CListener::~CListener()
{

}

void CDeferredLightRenderer::CListener::Handler(const IEventDispatcher::CEvent& Event)
{
	Run();
}

void CDeferredLightRenderer::CListener::Run()
{
	UINT uWidth, uHeight;
	FLOAT fTanHalfFOV = tanf(m_Parent.m_pSceneManager->GetSceneCamera().GetFOV() * g_fDEG2RAD * 0.5f);
	m_Parent.m_pSceneManager->GetRenderSystem().GetViewport(&uWidth, &uHeight);
	if(m_Parent.m_pAORandomMap)
	{
		m_Parent.m_AORandomMapUVScale.x = uWidth * 1.0f  / m_Parent.m_pAORandomMap->GetWidth();
		m_Parent.m_AORandomMapUVScale.y = uHeight * 1.0f / m_Parent.m_pAORandomMap->GetHeight();
	}

	m_Parent.m_AOInverseFocalLength.x = fTanHalfFOV * m_Parent.m_pSceneManager->GetSceneCamera().GetAspect();
	m_Parent.m_AOInverseFocalLength.y = - fTanHalfFOV;

	m_Parent.m_AOFocalLength.x = 1.0f / m_Parent.m_AOInverseFocalLength.x;
	m_Parent.m_AOFocalLength.y = 1.0f / fTanHalfFOV;

	//m_Parent.m_AOInverseFocalLength.y = - m_Parent.m_AOInverseFocalLength.y;

	m_Parent.SetAORadius(m_Parent.m_fAORadius);
}

CDeferredLightRenderer::CDeferredLightRenderer(void) :
m_pOutputRenderTarget(ALCHEMY_NULL),
m_pBufferRenderTarget(ALCHEMY_NULL),
m_pNormal3Detph1Map(ALCHEMY_NULL),
m_pPositionDepthMap(ALCHEMY_NULL),
m_pAORandomMap(ALCHEMY_NULL), 
m_pMaterial(ALCHEMY_NULL),
m_pAORotationDirection(ALCHEMY_NULL),
m_uAONumDirection(0), 
m_uAOStep(0), 
m_fAORadius(50.0f),
m_fAOScale(1.0f),
m_fAOBias(0.0f),
m_fAOIntensity(1.0f),
m_Listener(*this)
{
}

CDeferredLightRenderer::~CDeferredLightRenderer(void)
{
	ALCHEMY_DEBUG_DELETE_ARRAY(m_pAORotationDirection);
}

bool CDeferredLightRenderer::Create(CGraphicsResource::HANDLE Material, CGraphicsResource::HANDLE Normal3Power1Map, CGraphicsResource::HANDLE PositionDepthMap, CGraphicsResource::HANDLE AORandomMap, UINT uAONumDirection, UINT uAOStep, CGraphicsResource::HANDLE OutputRenderTarget)
{
	Destroy();

	m_pSceneManager = CSceneManager::GetInstance();
	if(!m_pSceneManager)
		return false;

	CSceneManager::IFullScreenDrawer* pFullScreenDrawer = m_pSceneManager->GetFullScreenDrawer();
	if(!pFullScreenDrawer)
		return false;

	/*UINT uLightCount = m_pSceneManager->GetLightCount();
	if(!uLightCount)
		return false;*/

	/////////////////////////////////
	// Texture index”–Œ Ã‚
	/////////////////////////////////
	m_pMaterial = static_cast<CMaterial*>( m_pSceneManager->GetResource(Material, CGraphicsResource::MATERIAL) );
	m_pMaterial = m_pMaterial ? m_pMaterial : m_pSceneManager->CreateMaterial();
	if(!m_pMaterial)
	{
		ALCHEMY_DEBUG_WARNING("CDeferredLightRenderer::Create: Material has been created fail.");

		Destroy();

		return false;
	}

	m_pMaterial->AddRef();

	CRenderSystem& RenderSystem = m_pSceneManager->GetRenderSystem();

	m_pNormal3Detph1Map = static_cast<LPTEXTURE>( RenderSystem.GetResource(Normal3Power1Map, CGraphicsResource::TEXTURE) );
	if(!m_pNormal3Detph1Map)
	{
		ALCHEMY_DEBUG_WARNING("CDeferredLightRenderer::Create: Error normal 3 detpht 1 map handle.");

		Destroy();

		return false;
	}

	m_pNormal3Detph1Map->AddRef();

	m_pPositionDepthMap = static_cast<LPTEXTURE>( RenderSystem.GetResource(PositionDepthMap, CGraphicsResource::TEXTURE) );
	if(!m_pPositionDepthMap)
	{
		ALCHEMY_DEBUG_WARNING("CDeferredLightRenderer::Create: Error position depth map handle.");

		Destroy();

		return false;
	}

	m_pPositionDepthMap->AddRef();

	m_pAORandomMap = uAONumDirection ? static_cast<LPTEXTURE>( RenderSystem.GetResource(AORandomMap, CGraphicsResource::TEXTURE) ) : ALCHEMY_NULL;
	if(m_pAORandomMap)
		m_pAORandomMap->AddRef();

	m_pOutputRenderTarget = static_cast<LPTEXTURE>( RenderSystem.GetResource(OutputRenderTarget, CGraphicsResource::TEXTURE) );
	if(m_pOutputRenderTarget)
	{
		if( !ALCHEMY_TEST_BIT(m_pOutputRenderTarget->GetUsage(), CTexture::RENDER_TARGET) )
		{
			ALCHEMY_DEBUG_WARNING("CDeferredLightRenderer::Create: Error material handle.");

			Destroy();

			return false;
		}

		m_pOutputRenderTarget->AddRef();
	}

	//CRenderQueue& RenderQueue = m_pSceneManager->GetRenderQueue();
	/*if(!m_pOutputRenderTarget)
	{
		m_pOutputRenderTarget = RenderQueue.GetSceneRenderTarget();
		if(!m_pOutputRenderTarget)
			m_pOutputRenderTarget = RenderQueue.CreateSceneRenderTarget();

		if(m_pOutputRenderTarget)
		{
			m_pOutputRenderTarget->AddRef();

			OutputRenderTarget = m_pOutputRenderTarget->GetResourceHandle();
		}
	}*/

	UINT uLightCount = m_pMaterial->GetLightCount();
	if(uLightCount > 1 || m_pAORandomMap)
	{
		if(m_pOutputRenderTarget)
			/*CTexture::ITextureInfo* pTextureInfo = m_pOutputRenderTarget->GetTextureInfo();
			m_pBufferRenderTarget = pTextureInfo ? 
				RenderSystem.CreateTexture(*pTextureInfo, m_pOutputRenderTarget->GetUsage(), m_pOutputRenderTarget->GetForamt(), 1, ALCHEMY_NULL) : 
			RenderSystem.CreateTexture(1 << m_pOutputRenderTarget->GetWidthShift(), 1 << m_pOutputRenderTarget->GetHeightShift(), m_pOutputRenderTarget->GetUsage(), m_pOutputRenderTarget->GetForamt(), 1, ALCHEMY_NULL);*/
			//ALCHEMY_VIEWPORT_RENDER_TARGET_LISTENER.AddEventListener(m_pBufferRenderTarget, *m_pOutputRenderTarget);
			m_pBufferRenderTarget = static_cast<LPTEXTURE>( m_pOutputRenderTarget->Clone() );
		else
			//ALCHEMY_VIEWPORT_RENDER_TARGET_LISTENER.AddEventListener(m_pBufferRenderTarget, RenderQueue.GetRenderSystem(), 0, CTexture::R8G8B8A8);
			//m_pBufferRenderTarget = RenderSystem.CreateTexture(RenderQueue.GetRenderTargetInfo(), ALCHEMY_FLAG(CTexture::RENDER_TARGET), CTexture::R8G8B8A8, 1, ALCHEMY_NULL);
			m_pBufferRenderTarget = RenderSystem.CreateRenderTargetView(CTexture::DEFAULT, 0);

		if(!m_pBufferRenderTarget)
		{
			ALCHEMY_DEBUG_WARNING("CDeferredLightRenderer::Create: Buffer render target has been created fail.");

			Destroy();

			return false;
		}
	}

	CEffectCode EffectCode( RenderSystem.GetCompiler() );
	CEffectCode::CPassDefiner PassDefiner(EffectCode);

	UINT uDiffuseColorKey, uSpecularColorKey, i;
	const CLight* pLight;
	RENDERDATA RenderData;
	CPosition2DEffect Position2D;
	IEffect::SAMPLERDECLARATION SamplerDeclaration;
	SamplerDeclaration.Type      = IEffect::TEXTURE2D;
	SamplerDeclaration.MinFilter = IEffect::LINEAR;
	SamplerDeclaration.MagFilter = IEffect::LINEAR;
	SamplerDeclaration.MipFilter = IEffect::NONE;
	SamplerDeclaration.AddressU  = IEffect::CLAMP;
	SamplerDeclaration.AddressV  = IEffect::CLAMP;
	SamplerDeclaration.AddressW  = IEffect::CLAMP;
	CTextureEffect TextureEffect(0, 0, &SamplerDeclaration);
	CNormal3Power1MapOutputEffect Normal3Power1MapOutputEffect(1, IEffect::LINEAR);
	CPositionDepthRGBAMapOutputEffect PositionDepthRGBAMapOutputEffect(2, IEffect::POINT);
	CMultipleLightBlendMethod MultipleLightBlendMethod;
	CEffectInfoAddition EffectInfoAddition;

	CVector<EFFECTINFO*> EffectInfos;
	EFFECTINFO* pEffectInfo;
	for(i = 0; i < uLightCount; ++ i)
	{
		pLight = m_pMaterial->GetLight(i);
		if(pLight)
		{
			PassDefiner.AddInfo(Position2D, ALCHEMY_NULL);

			PassDefiner.AddInfo(Normal3Power1MapOutputEffect, ALCHEMY_NULL);
			PassDefiner.AddInfo(PositionDepthRGBAMapOutputEffect, ALCHEMY_NULL);

			switch(pLight->Type)
			{
			case POINT_LIGHT:
				ALCHEMY_DEBUG_NEW( pEffectInfo, CViewPointLightEffect(i) );
				PassDefiner.AddInfo(*pEffectInfo, ALCHEMY_NULL);
				EffectInfos.Add(pEffectInfo);
				ALCHEMY_DEBUG_NEW( pEffectInfo, CViewLightnessEffect(i) );
				PassDefiner.AddInfo(*pEffectInfo, ALCHEMY_NULL);
				EffectInfos.Add(pEffectInfo);
				break;
			case SPOT_LIGHT:
				ALCHEMY_DEBUG_NEW( pEffectInfo, CViewPointLightEffect(i) );
				PassDefiner.AddInfo(*pEffectInfo, ALCHEMY_NULL);
				EffectInfos.Add(pEffectInfo);
				ALCHEMY_DEBUG_NEW( pEffectInfo, CViewSpotLightEffect(i) );
				PassDefiner.AddInfo(*pEffectInfo, ALCHEMY_NULL);
				EffectInfos.Add(pEffectInfo);
				ALCHEMY_DEBUG_NEW( pEffectInfo, CViewLightnessEffect(i) );
				PassDefiner.AddInfo(*pEffectInfo, ALCHEMY_NULL);
				EffectInfos.Add(pEffectInfo);
				break;
			case DIRECTIONAL_LIGHT:
				ALCHEMY_DEBUG_NEW( pEffectInfo, CViewLightnessEffect(i) );
				PassDefiner.AddInfo(*pEffectInfo, ALCHEMY_NULL);
				EffectInfos.Add(pEffectInfo);
				break;
			}

			uDiffuseColorKey = PassDefiner.AddColorKey();
			uSpecularColorKey = PassDefiner.AddColorKey();

			ALCHEMY_DEBUG_NEW( pEffectInfo, CDiffseLightEffect(i) );
			PassDefiner.AddInfo(*pEffectInfo, ALCHEMY_NULL, 0, uDiffuseColorKey);
			EffectInfos.Add(pEffectInfo);
			ALCHEMY_DEBUG_NEW( pEffectInfo, CViewBlinnPhongEffect(i) );
			PassDefiner.AddInfo(*pEffectInfo, ALCHEMY_NULL, 0, uSpecularColorKey);
			EffectInfos.Add(pEffectInfo);

			PassDefiner.AddInfo(uSpecularColorKey, &MultipleLightBlendMethod, uDiffuseColorKey/*, uDiffuseColorKey*/);

			PassDefiner.AddInfo(TextureEffect, &EffectInfoAddition);

			//PassDefiner.AddInfo(uDiffuseColorKey, &EffectInfoAddition);
		}

		PassDefiner.EndDefine();

		CVector<EFFECTINFO*>::CIterator Iterator = EffectInfos.GetIterator(0);
		while( CVector<EFFECTINFO*>::IsValuable(Iterator) )
		{
			ALCHEMY_DEBUG_DELETE(*Iterator);

			++ Iterator;
		}

		EffectInfos.Reset();

		RenderData.pEffect       = RenderSystem.CreateEffect(EffectCode);
		if(!RenderData.pEffect)
		{
			ALCHEMY_DEBUG_WARNING("CDeferredLightRenderer::Create: Effect has been created fail.");

			Destroy();

			return false;
		}

		RenderData.pRenderMethod = m_pSceneManager->CreateRenderMethod(EffectCode);
		if(!RenderData.pRenderMethod)
		{
			ALCHEMY_DEBUG_WARNING("CDeferredLightRenderer::Create: Render method has been created fail.");

			RenderData.pEffect->Release();

			Destroy();

			return false;
		}

		if(i)
			RenderData.uUserData = pFullScreenDrawer->AddUserData( (i & 1) ? m_pBufferRenderTarget->GetResourceHandle() : OutputRenderTarget, RenderData.pRenderMethod, m_pMaterial->GetResourceHandle(), 0, ALCHEMY_NULL );
		else
			RenderData.uUserData = pFullScreenDrawer->AddUserData(0, RenderData.pRenderMethod, m_pMaterial->GetResourceHandle(), 0, ALCHEMY_NULL);

		m_RenderData.Add(RenderData);

		EffectCode.Clear();
	}

	if( m_pMaterial->AddTexture(Normal3Power1Map) && m_pMaterial->AddTexture(Normal3Power1Map) && m_pMaterial->AddTexture(PositionDepthMap) )
	{
		if(m_pAORandomMap)
		{
			if( !m_pMaterial->AddTexture(AORandomMap) )
			{
				Destroy();

				return false;
			}

			ALCHEMY_DEBUG_NEW(m_pAORotationDirection, CFloat2[uAONumDirection]);
			FLOAT fAlpha = g_fTWO_PI / uAONumDirection, fAngle = 0.0f;
			for(i = 0; i < uAONumDirection; ++ i)
			{
				m_pAORotationDirection[i] = CFloat2( cosf(fAngle), sinf(fAngle) );

				fAngle += fAlpha;
			}

			CSSAORGBAMapEffect SSAORGBAMapEffect(2, 1, 3, uAONumDirection, uAOStep, &m_AORandomMapUVScale, &m_AOSamplerRadius, &m_AOInverseFocalLength, &m_fAOScale, &m_fAOBias, &m_fAOIntensity, m_pAORotationDirection);
			CEffectInfoModulation Modulation;

			PassDefiner.AddInfo(Position2D, ALCHEMY_NULL);
			PassDefiner.AddInfo(SSAORGBAMapEffect, ALCHEMY_NULL);
			PassDefiner.AddInfo(TextureEffect, &Modulation);
			PassDefiner.EndDefine();

			RenderData.pEffect       = RenderSystem.CreateEffect(EffectCode);
			if(!RenderData.pEffect)
			{
				ALCHEMY_DEBUG_WARNING("CDeferredLightRenderer::Create: Effect has been created fail.");

				Destroy();

				return false;
			}

			RenderData.pRenderMethod = m_pSceneManager->CreateRenderMethod(EffectCode);
			if(!RenderData.pRenderMethod)
			{
				ALCHEMY_DEBUG_WARNING("CDeferredLightRenderer::Create: Render method has been created fail.");

				RenderData.pEffect->Release();

				Destroy();

				return false;
			}

			if(uLightCount)
				RenderData.uUserData = pFullScreenDrawer->AddUserData( (uLightCount & 1) ? m_pBufferRenderTarget->GetResourceHandle() : OutputRenderTarget, RenderData.pRenderMethod, m_pMaterial->GetResourceHandle(), 0, ALCHEMY_NULL );
			else
				RenderData.uUserData = pFullScreenDrawer->AddUserData(0, RenderData.pRenderMethod, m_pMaterial->GetResourceHandle(), 0, ALCHEMY_NULL);

			m_RenderData.Add(RenderData);

			m_uAONumDirection = uAONumDirection;
			m_uAOStep         = uAOStep;

			UINT uEventType;

			if( ALCHEMY_GET_EVENT_TYPE(uEventType, PROJECTION_CHANGE, CCamera) )
				m_pSceneManager->GetSceneCamera().AddEventListener(uEventType, m_Listener);

			m_Listener.Run();

			SetAOIntensity(m_fAOIntensity);
		}

		SetVisible(true);

		return true;
	}

	Destroy();

	return false;
}

bool CDeferredLightRenderer::Create(CGraphicsResource::HANDLE Material, CGraphicsResource::HANDLE AORandomMap, UINT uNumAODirection, UINT uAOStep, CGraphicsResource::HANDLE OutputRenderTarget)
{
	m_pSceneManager = CSceneManager::GetInstance();
	if(!m_pSceneManager)
		return false;

	CDeferredRenderingPipeline& DeferredRenderingPipeline = m_pSceneManager->GetDeferredRenderingPipeline();

	UINT uNormal3Depth1MapIndex = DeferredRenderingPipeline.GetRenderTextureCount(), uPositionDepthMapIndex = uNormal3Depth1MapIndex + 1;
	if( DeferredRenderingPipeline.AddRenderTexture(CTexture::DEFAULT, m_Normal3Depth1MapInputEffect) && DeferredRenderingPipeline.AddRenderTexture(CTexture::DEFAULT, m_PositionDepthMapInputEffect) )
	{
		LPTEXTURE pNormal3Depth1Map = DeferredRenderingPipeline.GetRenderTexture(uNormal3Depth1MapIndex), pPositionDepthMap = DeferredRenderingPipeline.GetRenderTexture(uPositionDepthMapIndex);
		if(pNormal3Depth1Map && pPositionDepthMap)
			return Create(Material, pNormal3Depth1Map->GetResourceHandle(), pPositionDepthMap->GetResourceHandle(), AORandomMap, uNumAODirection, uAOStep, OutputRenderTarget);
	}

	return false;
}

bool CDeferredLightRenderer::ApplyForRender()
{
	if(!m_pSceneManager)
		return false;

	CSceneManager::IFullScreenDrawer* pFullScreenDrawer = m_pSceneManager->GetFullScreenDrawer();
	if(!pFullScreenDrawer)
		return false;

	CRenderQueue::LPRENDERENTRY pEntry;
	LPRENDERDATA pRenderData;
	UINT uMaxRenderEntryIndex = m_RenderData.GetLength();
	CRenderQueue& RenderQueue = m_pSceneManager->GetRenderQueue();
	LPTEXTURE pRenderTarget = m_pOutputRenderTarget ? m_pOutputRenderTarget : RenderQueue.GetSceneRenderTarget();
	CGraphicsResource::HANDLE OutputRenderTarget = pRenderTarget ? pRenderTarget->GetResourceHandle() : 0, ResultRenderTarget = m_pOutputRenderTarget ? m_pOutputRenderTarget->GetResourceHandle() : 0, FinalRenderTarget = OutputRenderTarget;
	uMaxRenderEntryIndex = uMaxRenderEntryIndex > 0 ? (uMaxRenderEntryIndex - 1) : uMaxRenderEntryIndex;
	for(UINT i = 0; i < uMaxRenderEntryIndex; ++ i)
	{
		pRenderData = m_RenderData.Get(i);
		if(pRenderData && pRenderData->pEffect && pRenderData->pRenderMethod)
		{
			pEntry = &RenderQueue.LockRenderEntry(*pFullScreenDrawer);

			pEntry->uRenderTarget	   = (i & 1) ? OutputRenderTarget : m_pBufferRenderTarget->GetResourceHandle();
			pEntry->uEffect            = pRenderData->pEffect->GetResourceHandle();
			pEntry->uRenderPass        = 0;
			pEntry->uVertexDeclaration = 0;
			pEntry->uModel             = 0;
			pEntry->uModelParameter    = 0;

			pEntry->uUserData          = pRenderData->uUserData; 

			FinalRenderTarget          = pEntry->uRenderTarget;

			RenderQueue.UnlockRenderEntry( CRenderQueue::FULL_SCREEN, ALCHEMY_FLAG(CRenderQueue::AUTO_SHADER) );
		}
	}

	pRenderData = m_RenderData.Get(uMaxRenderEntryIndex);
	if(pRenderData && pRenderData->pEffect && pRenderData->pRenderMethod)
	{
		if(FinalRenderTarget == ResultRenderTarget)
		{
			pEntry = &RenderQueue.LockRenderEntry(*pFullScreenDrawer);

			pEntry->uRenderTarget	   = m_pBufferRenderTarget->GetResourceHandle();
			pEntry->uEffect            = pRenderData->pEffect->GetResourceHandle();
			pEntry->uRenderPass        = 0;
			pEntry->uVertexDeclaration = 0;
			pEntry->uModel             = 0;
			pEntry->uModelParameter    = 0;

			pEntry->uUserData          = pRenderData->uUserData; 

			RenderQueue.UnlockRenderEntry( CRenderQueue::FULL_SCREEN, ALCHEMY_FLAG(CRenderQueue::AUTO_SHADER) );

			pEntry = &RenderQueue.LockRenderEntry(*pFullScreenDrawer);

			pEntry->uRenderTarget	   = OutputRenderTarget;
			pEntry->uEffect            = 0;
			pEntry->uRenderPass        = 0;
			pEntry->uVertexDeclaration = 0;
			pEntry->uModel             = 0;
			pEntry->uModelParameter    = 0;

			pEntry->uUserData          = pFullScreenDrawer->AddUserData(m_pBufferRenderTarget->GetResourceHandle(), ALCHEMY_NULL, 0, 0, ALCHEMY_NULL);

			RenderQueue.UnlockRenderEntry( CRenderQueue::FULL_SCREEN, ALCHEMY_FLAG(CRenderQueue::AUTO_SHADER) );
		}
		else
		{
			pEntry = &RenderQueue.LockRenderEntry(*pFullScreenDrawer);

			pEntry->uRenderTarget	   = ResultRenderTarget;
			pEntry->uEffect            = pRenderData->pEffect->GetResourceHandle();
			pEntry->uRenderPass        = 0;
			pEntry->uVertexDeclaration = 0;
			pEntry->uModel             = 0;
			pEntry->uModelParameter    = 0;

			pEntry->uUserData          = pRenderData->uUserData; 

			RenderQueue.UnlockRenderEntry( CRenderQueue::FULL_SCREEN, ALCHEMY_FLAG(CRenderQueue::AUTO_SHADER) );
		}
	}

	return true;
}

void CDeferredLightRenderer::Destroy()
{
	if(m_pOutputRenderTarget)
	{
		m_pOutputRenderTarget->Release();

		m_pOutputRenderTarget = ALCHEMY_NULL;
	}

	if(m_pBufferRenderTarget)
	{
		m_pBufferRenderTarget->Release();

		m_pBufferRenderTarget = ALCHEMY_NULL;
	}

	if(m_pNormal3Detph1Map)
	{
		m_pNormal3Detph1Map->Release();

		m_pNormal3Detph1Map = ALCHEMY_NULL;
	}

	if(m_pAORandomMap)
	{
		m_pAORandomMap->Release();

		m_pAORandomMap = ALCHEMY_NULL;
	}

	if(m_pMaterial)
	{
		m_pMaterial->Release();

		m_pMaterial = ALCHEMY_NULL;
	}

	m_RenderData.Clear();

	m_pSceneManager = ALCHEMY_NULL;

	ALCHEMY_DEBUG_DELETE_ARRAY(m_pAORotationDirection);

	m_uAONumDirection = 0;
}