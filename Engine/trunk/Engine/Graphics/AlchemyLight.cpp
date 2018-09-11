#include "AlchemyLight.h"
#include "AlchemySceneManager.h"

using namespace alchemy;

CLight::CLight(void)/* :
m_pShadowMap(ALCHEMY_NULL),
m_fFOV(0.0f),
m_fNear(0.0f),
m_fFar(0.0f)*/
{
	Type = DIRECTIONAL_LIGHT;
	fRange = 0.0f;
	fFalloff = 0.0f;
	fAttenuation0 = 0.0f;
	fAttenuation1 = 0.0f;
	fAttenuation2 = 0.0f;
	fTheta = 0.0f;
	fPhi = 0.0f;
	Position.x = Position.y = Position.z = 0.0f;
	Direction.x = Direction.y = Direction.z = 0.0f;
	Position.w = 1.0f;
	Specular.r = Specular.g = Specular.b = 0.0f;
	Specular.a = 1.0f;
}

CLight::~CLight(void)
{

}

/*bool CLight::BuildShadowMap(UINT uSize, FLOAT fFOV, FLOAT fNear, FLOAT fFar)
{
	LPTEXTURE pShadowMap = static_cast<CSceneManager&>( GetGraphicsResourcePool() ).GetRenderSystem().CreateTexture(uSize, uSize, ALCHEMY_FLAG(TEXTURE::WRITE_ONLY) | ALCHEMY_FLAG(TEXTURE::RENDER_TARGET), TEXTURE::R8G8B8A8, 1);

	if(pShadowMap)
	{
		if(m_pShadowMap)
			_Release(*m_pShadowMap);

		m_pShadowMap = pShadowMap;

		_AddRef(*pShadowMap);

		m_fFOV = fFOV;
		m_fNear = fNear;
		m_fFar = fFar;

		fTheta = fFOV * g_fDEG2RAD * 0.5f;
		fPhi   = fTheta;

		return true;
	}

	return false;
}

LPTEXTURE        CLight::GetShadowMap()const
{
	return m_pShadowMap;
}

void CLight::Destroy()
{
	if(m_pShadowMap)
	{
		_Release(*m_pShadowMap);

		m_pShadowMap = ALCHEMY_NULL;
	}
}*/