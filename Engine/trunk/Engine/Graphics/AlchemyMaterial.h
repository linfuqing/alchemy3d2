#pragma once

#include "AlchemyRGBColor.h"
#include "AlchemyARGBColor.h"
//#include "AlchemyHighLevelGraphicsResource.h"
//#include "AlchemyGraphicsBuffer.h"

#include "AlchemyLightManager.h"

#include "../Core/AlchemyBitFlag.h"
//#include "../Core/AlchemyVector.h"

namespace alchemy
{
	class CSceneManager;
	class CMaterial :
		public CHighLevelGraphicsResource
	{
		friend class CSceneManager;

		typedef enum
		{
			TOTAL_TEXTURE_COUNT = ALCHEMY_TOTAL_BITS(UINT)
		}CONTAINS;
	public:
		CMaterial(void);
		~CMaterial(void);

		void SetAmbient(FLOAT r, FLOAT g, FLOAT b, FLOAT a);

		void SetDiffuse(FLOAT r, FLOAT g, FLOAT b);
		void SetSpecular(FLOAT r, FLOAT g, FLOAT b);
		void SetEmissive(FLOAT r, FLOAT g, FLOAT b);

		void SetPower(FLOAT fPower);

		const CARGBColor& GetAmbient()const;

		const CRGBColor& GetDiffuse()const;
		const CRGBColor& GetSpecular()const;
		const CRGBColor& GetEmissive()const;

		FLOAT GetPower()const;

		UINT GetLightCount()const;
		UINT GetCameraCount()const;

		const CVector<LPTEXTURE>& GetTextures()const;

		const CLight* GetLight(UINT uIndex, CLightManager::SHADEMODE* pShaderMode = ALCHEMY_NULL)const;

		bool AddLight(CGraphicsResource::HANDLE Light, CLightManager::SHADEMODE LightShadeMode);
		bool SetLight(CGraphicsResource::HANDLE Light, CLightManager::SHADEMODE LightShadeMode, UINT uIndex);

		void AddLightEffectInfoToPass(CEffectCode::CPassDefiner& PassDefiner, IEffectInfoBlendMethod* pEffectInfoBlendMethod, IEffectInfo* pSpecularEffect, IEffectInfo* pNormalMapEffect, UINT uOutputColorKey = 0, UINT uInputColorKey = 0);

		bool AddTexture(CGraphicsResource::HANDLE Texture);
		bool SetTexture(CGraphicsResource::HANDLE Texture, UINT uIndex);
		bool RemoveTexture(UINT uIndex);

		const CCamera* GetCamera(UINT uIndex)const;

		UINT AddCamera(const CCamera& Camera);
		bool SetCamera(const CCamera* pCamera, UINT uIndex);

		CMaterial* Clone();

		void Destroy();
	protected:
		CARGBColor m_Ambient;

		CRGBColor m_Diffuse;
		CRGBColor m_Specular;
		CRGBColor m_Emissive;
		FLOAT m_fPower;

	private:
		CLightManager m_LightManager;

		CVector<LPTEXTURE> m_Textures;

		CVector<const CCamera*> m_Cameras;
	};

	ALCHEMY_INLINE void CMaterial::SetAmbient(FLOAT r, FLOAT g, FLOAT b, FLOAT a)
	{
		m_Ambient.r = r;
		m_Ambient.g = g;
		m_Ambient.b = b;
		m_Ambient.a = a;
	}

	ALCHEMY_INLINE void CMaterial::SetDiffuse(FLOAT r, FLOAT g, FLOAT b)
	{
		m_Diffuse.r = r;
		m_Diffuse.g = g;
		m_Diffuse.b = b;
	}

	ALCHEMY_INLINE void CMaterial::SetSpecular(FLOAT r, FLOAT g, FLOAT b)
	{
		m_Specular.r = r;
		m_Specular.g = g;
		m_Specular.b = b;
	}

	ALCHEMY_INLINE void CMaterial::SetEmissive(FLOAT r, FLOAT g, FLOAT b)
	{
		m_Emissive.r = r;
		m_Emissive.g = g;
		m_Emissive.b = b;
	}

	ALCHEMY_INLINE void CMaterial::SetPower(FLOAT fPower)
	{
		m_fPower = fPower;
	}

	ALCHEMY_INLINE const CARGBColor& CMaterial::GetAmbient()const
	{
		return m_Ambient;
	}

	ALCHEMY_INLINE const CRGBColor& CMaterial::GetDiffuse()const
	{
		return m_Diffuse;
	}

	ALCHEMY_INLINE const CRGBColor& CMaterial::GetSpecular()const
	{
		return m_Specular;
	}

	ALCHEMY_INLINE const CRGBColor& CMaterial::GetEmissive()const
	{
		return m_Emissive;
	}

	ALCHEMY_INLINE FLOAT CMaterial::GetPower()const
	{
		return m_fPower;
	}

	ALCHEMY_INLINE UINT CMaterial::GetLightCount()const
	{
		return m_LightManager.GetLightCount();
	}

	ALCHEMY_INLINE UINT CMaterial::GetCameraCount()const
	{
		return m_Cameras.GetLength();
	}

	ALCHEMY_INLINE const CVector<LPTEXTURE>& CMaterial::GetTextures()const
	{
		return m_Textures;
	}

	ALCHEMY_INLINE const CLight* CMaterial::GetLight(UINT uIndex, CLightManager::SHADEMODE* pShaderMode)const
	{
		const alchemy::LIGHT* pLight = m_LightManager.GetLight(uIndex, pShaderMode);
		if(pLight)
			return static_cast<const CLight*>(pLight);

		return ALCHEMY_NULL;
	}

	ALCHEMY_INLINE const CCamera* CMaterial::GetCamera(UINT uIndex)const
	{
		if( uIndex < m_Cameras.GetLength() )
			return m_Cameras[uIndex];

		return ALCHEMY_NULL;
	}
}