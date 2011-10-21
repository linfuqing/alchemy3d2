#include "AlchemyShadowMapDisplayEffectInfo.h"
#include "AlchemySceneObject.h"
#include "AlchemySceneManager.h"

using namespace alchemy;

CShadowMapDisplayEffectInfo::CShadowMapDisplayEffectInfo()
:m_pEffect(ALCHEMY_NULL),
m_pShadowDisplayEffect(ALCHEMY_NULL)
{

}

CShadowMapDisplayEffectInfo::~CShadowMapDisplayEffectInfo()
{
	if(m_pShadowDisplayEffect != ALCHEMY_NULL)
		ALCHEMY_DEBUG_DELETE(m_pShadowDisplayEffect);
}

bool CShadowMapDisplayEffectInfo::Create(UINT uLightIndex, UINT uSamplerIndex, CEffectCode::CPassDefiner& PassDefiner)
{
	m_pSceneManager = CSceneManager::GetInstance();

	m_hShadowMap = m_pSceneManager->GetLight(uLightIndex)->GetShadowMap()->GetResourceHandle();
	m_uSamplerIndex = uSamplerIndex;

	m_fEpsilon = 0.005f;

	if(m_pShadowDisplayEffect == ALCHEMY_NULL)
		ALCHEMY_DEBUG_NEW(m_pShadowDisplayEffect, CShadowMapDisplayEffect(uSamplerIndex, uLightIndex, m_fEpsilon) );

	PassDefiner.AddInfo(*m_pShadowDisplayEffect, ALCHEMY_NULL);

	return true;
}

void CShadowMapDisplayEffectInfo::OnPassLogic(CSceneObject* pSceneObject)
{
	if(pSceneObject != ALCHEMY_NULL && m_hShadowMap != 0)
	{
		m_pEffect = &(pSceneObject->GetRenderMethod()->GetEffect());
		m_pEffect->SetTexture(CEffect::FRAGMENT_SHADER, m_uSamplerIndex, m_hShadowMap);
	}
}

void CShadowMapDisplayEffectInfo::OnAddToProgram()
{
	if(m_pShadowDisplayEffect != ALCHEMY_NULL)
		ALCHEMY_DEBUG_DELETE(m_pShadowDisplayEffect);
}