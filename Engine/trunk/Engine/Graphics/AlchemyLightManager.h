#pragma once

#include "AlchemyLight.h"
#include "AlchemyLightDirectionEffect.h"

#include "../Core/AlchemyVector.h"

namespace alchemy
{
	class CLightManager
	{
	public:
		typedef enum
		{
			NONE = 0,
			PHONG,
			BLINN_PHONG,
			BANK_BRDF, 
			COOK_TORRANCE
		}SHADEMODE;

		CLightManager(void);
		~CLightManager(void);

		UINT GetLightCount()const;

		const LIGHT* GetLight(UINT uIndex, SHADEMODE* pShaderMode = ALCHEMY_NULL)const;

		void AddLight(const LIGHT& Light, SHADEMODE LightShadeMode);
		bool SetLight(const LIGHT& Light, SHADEMODE LightShadeMode, UINT uIndex);

		void AddEffectInfoToPass(CEffectCode::CPassDefiner& PassDefiner, IEffectInfoBlendMethod* pEffectInfoBlendMethod, IEffectInfo* pSpecularEffect, IEffectInfo* pNormalMapEffect, UINT uOutputColorKey = 0, UINT uInputColorKey = 0);

		void Destroy();

	private:
		typedef struct
		{
			SHADEMODE ShadeMode;
			const LIGHT* pLight;
			EFFECTINFO** ppEffectInfos;
			UINT uEffectInfoCount;
			EFFECTINFO* pDiffuseEffectInfo;
			EFFECTINFO* pSpecularEffectInfo;
		}LIGHTDECLARATION, * LPLIGHTDECLARATION;

		void __DestroyLightDeclaration(LIGHTDECLARATION& LightDeclaration);

		CVector<LIGHTDECLARATION> m_Lights;
		CVector<CLightDirectionEffect*> m_LightDirectionEffects;
	};

	ALCHEMY_INLINE UINT CLightManager::GetLightCount()const
	{
		return m_Lights.GetLength();
	}

	ALCHEMY_INLINE const LIGHT* CLightManager::GetLight(UINT uIndex, SHADEMODE* pShaderMode)const
	{
		const LIGHTDECLARATION* pLightDeclaration =  m_Lights.Get(uIndex);
		if(pLightDeclaration)
		{
			if(pShaderMode)
				*pShaderMode = pLightDeclaration->ShadeMode;

			return pLightDeclaration->pLight;
		}

		return ALCHEMY_NULL;
	}
}