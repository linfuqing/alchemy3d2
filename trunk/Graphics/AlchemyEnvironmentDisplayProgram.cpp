#include "AlchemyEnvironmentDisplayProgram.h"
#include "AlchemySceneManager.h"
#include "AlchemyEnvironmentalEffect.h"
#include "AlchemyPositionEffect.h"
#include "AlchemyTextureEffect.h"
#include "AlchemySceneObject.h"
#include "AlchemyEnvironmentProgram.h"

using namespace alchemy;

CEnvironmentDisplayProgram::CEnvironmentDisplayProgram()
:CProgramInstance(CSceneManager::EFFECT_NORMAL),
m_uSamplerIndex(0),
m_pEnvironmentTexture(ALCHEMY_NULL),
m_pPosEffect(ALCHEMY_NULL),
m_pEnvironmentEffect(ALCHEMY_NULL),
m_pTextureEffect(ALCHEMY_NULL)
{
	m_matCameraWorld.Identity();
}

CEnvironmentDisplayProgram::~CEnvironmentDisplayProgram()
{
	OnAddedToSceneObject();

	if(m_pEnvironmentTexture)
		m_pEnvironmentTexture->Release();

	m_pEnvironmentTexture = ALCHEMY_NULL;
}

bool CEnvironmentDisplayProgram::Create(CEnvironmentProgram& EnvironmentProgram, UINT uSamplerIndex, CEffectCode::CPassDefiner& PassDefiner,CEffectCode::CPassDefiner::BLENDMETHOD blendArgus)
{
	m_pSceneManager = CSceneManager::GetInstance();

	m_pEnvironmentTexture = EnvironmentProgram.GetEnvironmentTexture();

	if(!m_pEnvironmentTexture)
		return false;

	m_pEnvironmentTexture->AddRef();

	CEffectCode::SAMPLERDECLARATION sampler;
	sampler.Type      = CEffectCode::TEXTURE_CUBE;
	sampler.MinFilter = CEffectCode::LINEAR;
	sampler.MagFilter = CEffectCode::LINEAR;
	sampler.MipFilter = CEffectCode::LINEAR;
	sampler.AddressU  = CEffectCode::CLAMP;
	sampler.AddressV  = CEffectCode::CLAMP;
	sampler.AddressW  = CEffectCode::CLAMP;

	if(m_pPosEffect == ALCHEMY_NULL)
	{
		ALCHEMY_DEBUG_NEW(m_pPosEffect, CPositionEffect);
		ALCHEMY_DEBUG_NEW(m_pEnvironmentEffect, CEnvironmentalEffect(&m_matCameraWorld) );
		ALCHEMY_DEBUG_NEW(m_pTextureEffect, CTextureEffect(uSamplerIndex, 0, &sampler) );
	}

	PassDefiner.AddInfo(*m_pPosEffect, ALCHEMY_NULL);
	PassDefiner.AddInfo(*m_pEnvironmentEffect, ALCHEMY_NULL);
	PassDefiner.AddInfo(*m_pTextureEffect, blendArgus);

	m_uSamplerIndex = uSamplerIndex;

	return true;
}

bool CEnvironmentDisplayProgram::OnPreRenderSceneObject(CSceneObject* pSceneObject, UINT uFlag)
{
	if( m_pSceneManager->GetRenderSystem().GetRenderTarget() == m_pEnvironmentTexture->GetResourceHandle() )
		return false;

	m_matCameraWorld = m_pSceneManager->GetCamera().GetWorldMatrix();
	m_matCameraWorld._41 = m_matCameraWorld._42 = m_matCameraWorld._43 = 0.0f;

	return true;
}

bool CEnvironmentDisplayProgram::OnRenderSceneObject(CSceneObject* pSceneObject, UINT uFlag)
{
	if(!pSceneObject || !m_pSceneManager)
		return false;

	if( ALCHEMY_TEST_BIT(uFlag, CRenderQueue::PARENT) || ALCHEMY_TEST_BIT(uFlag, CRenderQueue::EFFECT_PASS) )
	{
		CEffect& Effect = pSceneObject->GetRenderMethod()->GetEffect();
		Effect.SetTexture( CEffect::FRAGMENT_SHADER, m_uSamplerIndex, m_pEnvironmentTexture->GetResourceHandle() );
	}

	return true;
}

void CEnvironmentDisplayProgram::OnAddedToSceneObject()
{
	if(m_pPosEffect != ALCHEMY_NULL)
	{
		ALCHEMY_DEBUG_DELETE(m_pPosEffect);
		ALCHEMY_DEBUG_DELETE(m_pEnvironmentEffect);
		ALCHEMY_DEBUG_DELETE(m_pTextureEffect);
	}
}