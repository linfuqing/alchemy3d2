#include "AlchemyShadowMapProgram.h"
#include "AlchemyDepthEffect.h"
#include "AlchemyPositionEffect.h"
#include "AlchemyRenderQueue.h"

using namespace alchemy;

CShadowMapProgram::CShadowMapProgram()
:CProgramInstance(CSceneManager::EFFECT_BEFOREHAND),
m_pLight(ALCHEMY_NULL),
m_pPosEffect(ALCHEMY_NULL),
m_pDepthEffect(ALCHEMY_NULL)
{

}

 
CShadowMapProgram::~CShadowMapProgram()
{
	if(m_pPosEffect != ALCHEMY_NULL)
		ALCHEMY_DEBUG_DELETE(m_pPosEffect);

	if(m_pDepthEffect != ALCHEMY_NULL)
		ALCHEMY_DEBUG_DELETE(m_pDepthEffect);
}

CGraphicsResource::HANDLE CShadowMapProgram::GetRenderTarget() const
{
	return m_pLight->GetShadowMap()->GetResourceHandle();
}

bool CShadowMapProgram::Create(UINT uLightIndex, CEffectCode::CPassDefiner& PassDefiner)
{
	m_pSceneManager = CSceneManager::GetInstance();

	m_pLight = m_pSceneManager->GetLight( uLightIndex );
	if(m_pLight == ALCHEMY_NULL)
		return false;

	if(m_pPosEffect == ALCHEMY_NULL)
		ALCHEMY_DEBUG_NEW(m_pPosEffect, CPositionEffect);

	if(m_pDepthEffect == ALCHEMY_NULL)
		ALCHEMY_DEBUG_NEW(m_pDepthEffect, CDepthEffect);

	PassDefiner.AddInfo(*m_pPosEffect, ALCHEMY_NULL);
	PassDefiner.AddInfo(*m_pDepthEffect, ALCHEMY_NULL);

	return	true;//CProgramInstance::Create( PassDefiner );
}

bool CShadowMapProgram::OnPreRenderSceneObject(CSceneObject* pSceneObject, UINT uFlag)
{
	if( ALCHEMY_TEST_BIT(uFlag, CRenderQueue::RENDER_TARGET) )
	{
		CSceneManager& SceneManager = static_cast<CSceneManager&>( m_pLight->GetGraphicsResourcePool() );
		const CCamera* pLightCamera = m_pLight->GetCamera();
		SceneManager.SetCamera( const_cast<CCamera&>(*pLightCamera) );
		SceneManager.GetRenderSystem().Clear( ~0, ALCHEMY_FLAG(CRenderSystem::TARGET) | ALCHEMY_FLAG(CRenderSystem::ZBUFFER) );
		/*else
		{
			SceneManager.SetCamera( *m_pOldCamera );
			m_pOldCamera = ALCHEMY_NULL;
		}*/

		//CRenderSystem& RenderSystem = SceneManager.GetRenderSystem();

		//RenderSystem

		//m_hOldRenderTarget = RenderSystem.GetRenderTarget();


		/*RenderSystem.SetRenderTarget( m_pLight->GetShadowMap()->GetResourceHandle() );*/
	}
	return true;
}

//void CShadowMapProgram::OnEndRenderSceneObject(CSceneObject* pSceneObject, UINT uFlag)
//{
//	if( ALCHEMY_TEST_BIT(uFlag, CRenderQueue::RENDER_TARGET) )
//	{
//		if(m_pOldCamera == ALCHEMY_NULL)
//			return;
//
//		m_pSceneManager->SetCamera(*m_pOldCamera);
//
//		m_pOldCamera = ALCHEMY_NULL;
//	}
//}

void CShadowMapProgram::OnAddedToSceneObject()
{
	if(m_pPosEffect != ALCHEMY_NULL)
		ALCHEMY_DEBUG_DELETE(m_pPosEffect);

	if(m_pDepthEffect != ALCHEMY_NULL)
		ALCHEMY_DEBUG_DELETE(m_pDepthEffect);
}