#include "AlchemyMaterial.h"
#include "AlchemySceneManager.h"

using namespace alchemy;

CMaterial::CMaterial(void) : 
m_Ambient(1.0f, 1.0f, 1.0f, 1.0f),
m_Diffuse(1.0f, 1.0f, 1.0f),
m_Specular(1.0f, 1.0f, 1.0f),
m_Emissive(0.0f, 0.0f, 0.0f),
m_fPower(1.0f)
{
}

CMaterial::~CMaterial(void)
{
}

bool CMaterial::AddLight(CGraphicsResource::HANDLE Light, CLightManager::SHADEMODE LightShadeMode)
{
	CLight* pLight = static_cast<CLight*>( static_cast<CSceneManager&>( GetGraphicsResourcePool() ).GetResource(Light, CGraphicsResource::LIGHT) );
	if(pLight)
	{
		_AddRef(*pLight);

		pLight->AddRef();

		m_LightManager.AddLight(*pLight, LightShadeMode);

		return true;
	}

	return false;
}

bool CMaterial::SetLight(CGraphicsResource::HANDLE Light, CLightManager::SHADEMODE LightShadeMode, UINT uIndex)
{
	CLight* pLight = static_cast<CLight*>( static_cast<CSceneManager&>( GetGraphicsResourcePool() ).GetResource(Light, CGraphicsResource::LIGHT) );
	if(pLight)
	{
		_AddRef(*pLight);

		pLight->AddRef();

		return m_LightManager.SetLight(*pLight, LightShadeMode, uIndex);
	}

	return false;
}

void CMaterial::AddLightEffectInfoToPass(CEffectCode::CPassDefiner& PassDefiner, IEffectInfoBlendMethod* pEffectInfoBlendMethod, IEffectInfo* pSpecularEffect, IEffectInfo* pNormalMapEffect, UINT uOutputColorKey, UINT uInputColorKey)
{
	m_LightManager.AddEffectInfoToPass(PassDefiner, pEffectInfoBlendMethod, pSpecularEffect, pNormalMapEffect, uOutputColorKey, uInputColorKey);
}

bool CMaterial::AddTexture(CGraphicsResource::HANDLE Texture)
{
	LPTEXTURE pTexture = static_cast<LPTEXTURE>( static_cast<CSceneManager&>( GetGraphicsResourcePool() ).GetRenderSystem().GetResource(Texture, CGraphicsResource::TEXTURE) );

	if(pTexture)
	{
		_AddRef(*pTexture);

		pTexture->AddRef();

		m_Textures.Add(pTexture);

		return true;
	}

	return false;
}

bool CMaterial::SetTexture(CGraphicsResource::HANDLE Texture, UINT uIndex)
{
	LPTEXTURE pTexture = static_cast<LPTEXTURE>( static_cast<CSceneManager&>( GetGraphicsResourcePool() ).GetRenderSystem().GetResource(Texture, CGraphicsResource::TEXTURE) );

	if(pTexture)
	{
		LPTEXTURE* ppSourceTexture = m_Textures.Get(uIndex);
		LPTEXTURE pSourceTexture = ALCHEMY_NULL;
		if(ppSourceTexture)
			pSourceTexture = *ppSourceTexture;

		if( m_Textures.Set(pTexture, uIndex) )
		{
			_AddRef(*pTexture);

			pTexture->AddRef();

			if(pSourceTexture)
				_Release(*pSourceTexture);

			return true;
		}
	}

	return false;
}

bool CMaterial::RemoveTexture(UINT uIndex)
{
	LPTEXTURE* ppTexture = m_Textures.Get(uIndex);

	if(ppTexture)
	{
		if(*ppTexture)
			_Release(**ppTexture);
		
		return m_Textures.Remove(uIndex, 1);
	}

	return false;
}

UINT CMaterial::AddCamera(const CCamera& Camera)
{
	return m_Cameras.Add(&Camera);
}

bool CMaterial::SetCamera(const CCamera* pCamera, UINT uIndex)
{
	return m_Cameras.Set(pCamera, uIndex);
}

CMaterial* CMaterial::Clone()
{
	CMaterial* pOutput = static_cast<CMaterial*>( _Clone() );
	if(pOutput)
	{
		pOutput->m_Ambient = m_Ambient;

		pOutput->m_Diffuse = m_Diffuse;
		pOutput->m_Specular = m_Specular;
		pOutput->m_Emissive = m_Emissive;
		pOutput->m_fPower = m_fPower;

		CLightManager::SHADEMODE LightShadeMode;
		UINT uLightCount = m_LightManager.GetLightCount();
		const alchemy::LIGHT* pLight;
		for(UINT i =0; i < uLightCount; ++ i)
		{
			pLight = m_LightManager.GetLight(i, &LightShadeMode);
			if(pLight)
				pOutput->m_LightManager.AddLight(*pLight, LightShadeMode);
		}

		pOutput->m_Textures = m_Textures;
		pOutput->m_Cameras = m_Cameras;
	}

	return pOutput;
}

void CMaterial::Destroy()
{
	const CLight* pSourceLight;
	CLight* pDestinationLight;
	UINT uLightCount = m_LightManager.GetLightCount();
	CSceneManager& SceneManager = static_cast<CSceneManager&>( GetGraphicsResourcePool() );
	for(UINT i = 0; i < uLightCount; ++ i)
	{
		pSourceLight = static_cast<const CLight*>( m_LightManager.GetLight(i) );

		if(pSourceLight)
		{
			pDestinationLight = static_cast<CLight*>( SceneManager.GetResource(pSourceLight->GetResourceHandle(), CGraphicsResource::LIGHT) );

			if(pDestinationLight)
				_Release(*pDestinationLight);
		}
	}

	m_LightManager.Destroy();

	LPTEXTURE pTexture;
	CVector<LPTEXTURE>::CConstantIterator TextureIterator = m_Textures.GetIterator(0);
	while( CVector<LPTEXTURE>::IsValuable(TextureIterator) )
	{
		pTexture = *TextureIterator; 

		if(pTexture)
			_Release(*pTexture);

		++ TextureIterator;
	}

	m_Textures.Clear();

	m_Cameras.Clear();
}