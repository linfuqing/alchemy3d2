#include "AlchemyLightManager.h"

#include "AlchemyLightnessEffect.h"

#include "AlchemyPointLightEffect.h"
#include "AlchemySpotLightEffect.h"

#include "AlchemyDiffseLightEffect.h"
#include "AlchemyPhongEffect.h"
#include "AlchemyBlinnPhongEffect.h"
#include "AlchemyBankBRDFEffect.h"
#include "AlchemyCookTorranceEffect.h"

#include "AlchemyMultipleLightBlendMethod.h"

#include "AlchemyCameraDirectionEffect.h"
#include "AlchemyEffectInfoAddition.h"

#include "AlchemyPixelColorEffect.h"

using namespace alchemy;

CLightManager::CLightManager(void)
{
}

CLightManager::~CLightManager(void)
{
	CVector<LIGHTDECLARATION>::CIterator LightIterator = m_Lights.GetIterator(0);
	while( CVector<LIGHTDECLARATION>::IsValuable(LightIterator) )
	{
		__DestroyLightDeclaration(*LightIterator);

		++ LightIterator;
	}
	
	CVector<CLightDirectionEffect*>::CIterator LightDirectionEffectIterator = m_LightDirectionEffects.GetIterator(0);
	while ( CVector<CLightDirectionEffect*>::IsValuable(LightDirectionEffectIterator) ) 
	{
		ALCHEMY_DEBUG_DELETE(*LightDirectionEffectIterator);
		
		++ LightDirectionEffectIterator;
	}
}

void CLightManager::AddLight(const LIGHT& Light, CLightManager::SHADEMODE LightShadeMode)
{
	LIGHTDECLARATION LightDeclaration;
	LightDeclaration.pLight = &Light;
	LightDeclaration.ShadeMode = LightShadeMode;
	LightDeclaration.uEffectInfoCount = 0;

	switch(Light.Type)
	{
	case POINT_LIGHT:
		LightDeclaration.uEffectInfoCount = 2;//3;
		break;
	case SPOT_LIGHT:
		LightDeclaration.uEffectInfoCount = 3;//4;
		break;
	case DIRECTIONAL_LIGHT:
		LightDeclaration.uEffectInfoCount = 1;//2;
		break;
	}

	ALCHEMY_DEBUG_NEW(LightDeclaration.ppEffectInfos, LPEFFECTINFO[LightDeclaration.uEffectInfoCount]);

	UINT uLightIndex = m_Lights.GetLength();
	switch(Light.Type)
	{
	case POINT_LIGHT:
		ALCHEMY_DEBUG_NEW( LightDeclaration.ppEffectInfos[0], CPointLightEffect(uLightIndex) );
		ALCHEMY_DEBUG_NEW( LightDeclaration.ppEffectInfos[1], CLightnessEffect(uLightIndex) );
		//ALCHEMY_DEBUG_NEW( LightDeclaration.ppEffectInfos[2], CCameraDirectionEffect(IEffect::FRAGMENT_SHADER) );
		break;
	case SPOT_LIGHT:
		ALCHEMY_DEBUG_NEW( LightDeclaration.ppEffectInfos[0], CPointLightEffect(uLightIndex) );
		ALCHEMY_DEBUG_NEW( LightDeclaration.ppEffectInfos[1], CSpotLightEffect(uLightIndex) );
		ALCHEMY_DEBUG_NEW( LightDeclaration.ppEffectInfos[2], CLightnessEffect(uLightIndex) );
		//ALCHEMY_DEBUG_NEW( LightDeclaration.ppEffectInfos[3], CCameraDirectionEffect(IEffect::FRAGMENT_SHADER) );
		break;
	case DIRECTIONAL_LIGHT:
		ALCHEMY_DEBUG_NEW( LightDeclaration.ppEffectInfos[0], CLightnessEffect(uLightIndex) );
		//ALCHEMY_DEBUG_NEW( LightDeclaration.ppEffectInfos[1], CCameraDirectionEffect(IEffect::FRAGMENT_SHADER) );
		break;
	}

	ALCHEMY_DEBUG_NEW( LightDeclaration.pDiffuseEffectInfo , CDiffseLightEffect(uLightIndex) );
	
	switch (LightShadeMode) 
	{
		case PHONG:
			ALCHEMY_DEBUG_NEW( LightDeclaration.pSpecularEffectInfo, CPhongEffect(uLightIndex) );
			break;
		case BLINN_PHONG:
			ALCHEMY_DEBUG_NEW( LightDeclaration.pSpecularEffectInfo, CBlinnPhongEffect(uLightIndex) );
			break;
		case BANK_BRDF:
			ALCHEMY_DEBUG_NEW( LightDeclaration.pSpecularEffectInfo, CBankBRDFEffect(uLightIndex) );
			break;
		case COOK_TORRANCE:
			ALCHEMY_DEBUG_NEW( LightDeclaration.pSpecularEffectInfo, CCookTorranceEffect(uLightIndex) );
			break;

		default:
			LightDeclaration.pSpecularEffectInfo = ALCHEMY_NULL;
			break;
	}

	m_Lights.Add(LightDeclaration);
}

bool CLightManager::SetLight(const LIGHT& Light, CLightManager::SHADEMODE LightShadeMode, UINT uIndex)
{
	LIGHTDECLARATION LightDeclaration;
	LightDeclaration.pLight = &Light;
	LightDeclaration.ShadeMode = LightShadeMode;
	LightDeclaration.uEffectInfoCount = 0;

	switch(Light.Type)
	{
	case POINT_LIGHT:
		LightDeclaration.uEffectInfoCount = 2;//3;
		break;
	case SPOT_LIGHT:
		LightDeclaration.uEffectInfoCount = 3;//4;
		break;
	case DIRECTIONAL_LIGHT:
		LightDeclaration.uEffectInfoCount = 1;//2;
		break;
	}

	ALCHEMY_DEBUG_NEW(LightDeclaration.ppEffectInfos, LPEFFECTINFO[LightDeclaration.uEffectInfoCount]);

	UINT uLightIndex = m_Lights.GetLength();
	switch(Light.Type)
	{
	case POINT_LIGHT:
		ALCHEMY_DEBUG_NEW( LightDeclaration.ppEffectInfos[0], CPointLightEffect(uLightIndex) );
		//ALCHEMY_DEBUG_NEW( LightDeclaration.ppEffectInfos[2], CCameraDirectionEffect() );
		ALCHEMY_DEBUG_NEW( LightDeclaration.ppEffectInfos[1], CLightnessEffect(uLightIndex) );
		break;
	case SPOT_LIGHT:
		ALCHEMY_DEBUG_NEW( LightDeclaration.ppEffectInfos[0], CPointLightEffect(uLightIndex) );
		ALCHEMY_DEBUG_NEW( LightDeclaration.ppEffectInfos[1], CSpotLightEffect(uLightIndex) );
		//ALCHEMY_DEBUG_NEW( LightDeclaration.ppEffectInfos[3], CCameraDirectionEffect() );
		ALCHEMY_DEBUG_NEW( LightDeclaration.ppEffectInfos[2], CLightnessEffect(uLightIndex) );
		break;
	case DIRECTIONAL_LIGHT:
		//ALCHEMY_DEBUG_NEW( LightDeclaration.ppEffectInfos[1], CCameraDirectionEffect() );
		ALCHEMY_DEBUG_NEW( LightDeclaration.ppEffectInfos[0], CLightnessEffect(uLightIndex) );
		break;
	}

	ALCHEMY_DEBUG_NEW( LightDeclaration.pDiffuseEffectInfo , CDiffseLightEffect(uLightIndex) );
	
	switch (LightShadeMode) 
	{
		case PHONG:
			ALCHEMY_DEBUG_NEW( LightDeclaration.pSpecularEffectInfo, CPhongEffect(uLightIndex) );
			break;
		case BLINN_PHONG:
			ALCHEMY_DEBUG_NEW( LightDeclaration.pSpecularEffectInfo, CBlinnPhongEffect(uLightIndex) );
			break;
		case BANK_BRDF:
			ALCHEMY_DEBUG_NEW( LightDeclaration.pSpecularEffectInfo, CBankBRDFEffect(uLightIndex) );
			break;
		case COOK_TORRANCE:
			ALCHEMY_DEBUG_NEW( LightDeclaration.pSpecularEffectInfo, CCookTorranceEffect(uLightIndex) );
			break;
		default:
			LightDeclaration.pSpecularEffectInfo = ALCHEMY_NULL;
			break;
	}

	LPLIGHTDECLARATION pLightDeclaration = m_Lights.Get(uIndex);
	if(pLightDeclaration)
		__DestroyLightDeclaration(*pLightDeclaration);

	return m_Lights.Set(LightDeclaration, uIndex);
}

void CLightManager::AddEffectInfoToPass(CEffectCode::CPassDefiner& PassDefiner, IEffectInfoBlendMethod* pEffectInfoBlendMethod,IEffectInfo* pSpecularEffect, IEffectInfo* pNormalMapEffect, UINT uOutputColorKey, UINT uInputColorKey)
{
	static CEffectInfoAddition s_Addition;
	static CMultipleLightBlendMethod s_MultipleLightBlendMethod;

	UINT uOutputDiffuseColorKey = PassDefiner.AddColorKey(), uOutputSpecularColorKey = PassDefiner.AddColorKey(), uInputDiffuseColorKey = 0, uInputSpecularColorKey = 0, i;
	LPLIGHTDECLARATION pLightDeclaration;
	IEffectInfo* pEffectInfo;

	if(pSpecularEffect)
		PassDefiner.AddInfo(*pSpecularEffect, ALCHEMY_NULL);

	CVector<LIGHTDECLARATION>::CIterator Iterator = m_Lights.GetIterator(0);

	if(pNormalMapEffect)
	{
		static CCameraDirectionEffect s_CameraDirectionEffect;
		PassDefiner.AddInfo(s_CameraDirectionEffect, ALCHEMY_NULL);
		CLightDirectionEffect** ppLightDirectionEffect;
		CLightDirectionEffect* pLightDirectionEffect;
		UINT uIndex;
		const LIGHT* pLight;
		while( CVector<LIGHTDECLARATION>::IsValuable(Iterator) )
		{
			pLight = Iterator->pLight;
			if(pLight && pLight->Type != DIRECTIONAL_LIGHT)
			{
				ppLightDirectionEffect = m_LightDirectionEffects.Get( uIndex = Iterator.GetIndex() );
				if(ppLightDirectionEffect)
					pLightDirectionEffect = *ppLightDirectionEffect;
				else
				{
					ALCHEMY_DEBUG_NEW(pLightDirectionEffect, CLightDirectionEffect(uIndex));
					
					m_LightDirectionEffects.Add(pLightDirectionEffect);
				}
				
				if(!pLightDirectionEffect)
				{
					ALCHEMY_DEBUG_NEW( pLightDirectionEffect, CLightDirectionEffect(uIndex) );
					
					m_LightDirectionEffects.Set(pLightDirectionEffect, uIndex);
				}
				
				PassDefiner.AddInfo(*pLightDirectionEffect, ALCHEMY_NULL);
			}
			
			++ Iterator;
		}
		Iterator = m_Lights.GetIterator(0);
		
		PassDefiner.AddInfo(*pNormalMapEffect, ALCHEMY_NULL);
	}
	else
	{
		static CCameraDirectionEffect s_CameraDirectionEffect(IEffect::FRAGMENT_SHADER);
		
		PassDefiner.AddInfo(s_CameraDirectionEffect, ALCHEMY_NULL);
	}
	
	while( CVector<LIGHTDECLARATION>::IsValuable(Iterator) )
	{
		pLightDeclaration = &(*Iterator);
		for(i = 0; i < pLightDeclaration->uEffectInfoCount; ++ i)
		{
			pEffectInfo = pLightDeclaration->ppEffectInfos[i];
			if(pEffectInfo)
				PassDefiner.AddInfo(*pEffectInfo, ALCHEMY_NULL);
		}

		if(pLightDeclaration->pDiffuseEffectInfo)
		{
			PassDefiner.AddInfo(*pLightDeclaration->pDiffuseEffectInfo, uInputDiffuseColorKey ? &s_Addition: ALCHEMY_NULL, uInputDiffuseColorKey, uOutputDiffuseColorKey);

			uInputDiffuseColorKey = uOutputDiffuseColorKey;
		}

		if(pLightDeclaration->pSpecularEffectInfo)
		{
			PassDefiner.AddInfo(*pLightDeclaration->pSpecularEffectInfo, uInputSpecularColorKey ? &s_Addition : ALCHEMY_NULL, uInputSpecularColorKey, uOutputSpecularColorKey);

			uInputSpecularColorKey = uOutputSpecularColorKey;
		}

		++ Iterator;
	}
	
	if(!uInputSpecularColorKey)
	{
		static CPixelColorEffect s_PixelColorEffect;
		PassDefiner.AddInfo(s_PixelColorEffect, ALCHEMY_NULL, 0, uOutputSpecularColorKey);
		
		uInputSpecularColorKey = uOutputSpecularColorKey;
	}

	if(uInputDiffuseColorKey && uInputSpecularColorKey)
	{
		if(pEffectInfoBlendMethod)
		{
			PassDefiner.AddInfo(uInputSpecularColorKey, &s_MultipleLightBlendMethod, uInputDiffuseColorKey, uOutputDiffuseColorKey);

			PassDefiner.AddInfo(uInputDiffuseColorKey, pEffectInfoBlendMethod, uInputColorKey, uOutputColorKey);
		}
		else
			PassDefiner.AddInfo(uInputSpecularColorKey, &s_MultipleLightBlendMethod, uInputDiffuseColorKey, uOutputColorKey);
	}
}

void CLightManager::Destroy()
{
	CVector<LIGHTDECLARATION>::CIterator LightIterator = m_Lights.GetIterator(0);
	while( CVector<LIGHTDECLARATION>::IsValuable(LightIterator) )
	{
		__DestroyLightDeclaration(*LightIterator);

		++ LightIterator;
	}

	m_Lights.Clear();

	CVector<CLightDirectionEffect*>::CIterator LightDirectionEffectIterator = m_LightDirectionEffects.GetIterator(0);
	while ( CVector<CLightDirectionEffect*>::IsValuable(LightDirectionEffectIterator) ) 
	{
		ALCHEMY_DEBUG_DELETE(*LightDirectionEffectIterator);

		++ LightDirectionEffectIterator;
	}

	m_LightDirectionEffects.Clear();
}

void CLightManager::__DestroyLightDeclaration(LIGHTDECLARATION& LightDeclaration)
{
	if(LightDeclaration.ppEffectInfos)
	{
		for(UINT i = 0; i < LightDeclaration.uEffectInfoCount; ++ i)
			ALCHEMY_DEBUG_DELETE(LightDeclaration.ppEffectInfos[i]);

		ALCHEMY_DEBUG_DELETE_ARRAY(LightDeclaration.ppEffectInfos);
	}

	ALCHEMY_DEBUG_DELETE(LightDeclaration.pDiffuseEffectInfo);
	ALCHEMY_DEBUG_DELETE(LightDeclaration.pSpecularEffectInfo);
}