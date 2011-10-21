#include "AlchemyDistanceInfoProgram.h"
#include "AlchemyDepthFieldEffect.h"
#include "AlchemyDistanceInfoEffect.h"
#include "AlchemyPositionEffect.h"
#include "AlchemySceneObject.h"

using namespace alchemy;

CDistanceInfoProgram::CDistanceInfoProgram()
:CProgramInstance( CSceneManager::EFFECT_BEFOREHAND ),
m_hRenderTarget(0),
m_pDistanceInfoEffect(ALCHEMY_NULL),
m_pPositionEffect(ALCHEMY_NULL)
{
}

CDistanceInfoProgram::~CDistanceInfoProgram()
{
	Destroy();
}

bool CDistanceInfoProgram::Create(const CDepthFieldEffect& DepthEffect,CEffectCode::CPassDefiner& PassDefiner)
{
	m_fFocusDistance = -( DepthEffect.GetDistance() );
	m_hRenderTarget = DepthEffect.GetAlphaTexture();

	if(!m_pDistanceInfoEffect)
	{
		ALCHEMY_DEBUG_NEW(m_pDistanceInfoEffect,CDistanceInfoEffect);
		if( !m_pDistanceInfoEffect->Create(m_fFocusDistance) )
			return false;
	}

	if(!m_pPositionEffect)
		ALCHEMY_DEBUG_NEW(m_pPositionEffect,CPositionEffect);

	PassDefiner.AddInfo(*m_pPositionEffect, 0);
	PassDefiner.AddInfo(*m_pDistanceInfoEffect, 0);


	m_pSceneManager = CSceneManager::GetInstance();

	return true;
}

void CDistanceInfoProgram::Destroy()
{
	OnAddedToSceneObject();
}

CGraphicsResource::HANDLE CDistanceInfoProgram::GetRenderTarget() const
{
	return m_hRenderTarget;
}

bool CDistanceInfoProgram::OnPreRenderSceneObject(CSceneObject* pSceneObject, UINT uFlag)
{
	CSceneManager* pSceneManager = pSceneObject->GetSceneManager();
	if( ALCHEMY_TEST_BIT(uFlag, CRenderQueue::RENDER_TARGET) )
		pSceneManager->GetRenderSystem().Clear( 0, ALCHEMY_FLAG(CRenderSystem::TARGET) | ALCHEMY_FLAG(CRenderSystem::ZBUFFER) );

	return true;
}

void CDistanceInfoProgram::OnAddedToSceneObject()
{
	ALCHEMY_DEBUG_DELETE(m_pPositionEffect);
	ALCHEMY_DEBUG_DELETE(m_pDistanceInfoEffect);
}