#include "AlchemyLight.h"
#include "AlchemySceneManager.h"

using namespace alchemy;

CLight::CLight(void) :
m_Type(POINT),
m_fRange(0.0f),
m_fFalloff(0.0f),
m_fAttenuation0(0.0f),
m_fAttenuation1(0.0f),
m_fAttenuation2(0.0f),
m_fTheta(0.0f),
m_fPhi(0.0f),
m_Position(0.0f, 0.0f, 0.0f),
m_Direction(0.0f, 0.0f, 0.0f),
m_pShadowMap(ALCHEMY_NULL),
m_pCamera(ALCHEMY_NULL)
{
}

CLight::~CLight(void)
{
	ALCHEMY_DEBUG_DELETE(m_pCamera);
}

const CCamera*   CLight::GetCamera()const
{
	m_pCamera->SetTranslation(m_Position.Float3);
	m_pCamera->SetDirection(m_Direction);

	m_pCamera->Update();

	return m_pCamera;
}

//bool CLight::ActivateCamera()
//{
//	if(m_pShadowMap == ALCHEMY_NULL)
//		return false;
//
//	CSceneManager& SceneManager = static_cast<CSceneManager&>( GetGraphicsResourcePool() );
//
//	CCamera& Camera = SceneManager.GetCamera();
//
//	m_pCamera->SetTranslation(m_Position);
//	m_pCamera->SetDirection(m_Direction);
//
//	m_pCamera->Update();
//
//	SceneManager.SetCamera(*m_pCamera);
//
//	return true;
//}
//
//bool CLight::UnActivateCamera()
//{
//	if(m_pShadowMap == ALCHEMY_NULL)
//		return false;
//
//	CSceneManager& SceneManager = static_cast<CSceneManager&>( GetGraphicsResourcePool() );
//	SceneManager.SetCamera(Camera);
//}

bool CLight::BuildShadowMap(UINT uSize, FLOAT fFOV, FLOAT fNear, FLOAT fFar)
{
	LPTEXTURE pShadowMap = static_cast<CSceneManager&>( GetGraphicsResourcePool() ).GetRenderSystem().CreateTexture(uSize, uSize, ALCHEMY_FLAG(TEXTURE::WRITE_ONLY) | ALCHEMY_FLAG(TEXTURE::RENDER_TARGET), TEXTURE::A8R8G8B8, 1);

	if(pShadowMap)
	{
		if(m_pShadowMap)
			_Release(*m_pShadowMap);

		m_pShadowMap = pShadowMap;

		_AddRef(*pShadowMap);

		if(!m_pCamera)
			ALCHEMY_DEBUG_NEW(m_pCamera, CCamera);

		m_pCamera->SetProjection(fFOV, fNear, fFar, 1);

		m_fTheta = fFOV * 0.5f;
		m_fPhi   = m_fTheta;

		return true;
	}

	return false;
}

bool CLight::AddShadowObject(CSceneObject& Object, CSceneObject::SHADOWTYPE Type)
{
	if( &GetGraphicsResourcePool() == static_cast<CGraphicsResourcePool*>( Object.GetSceneManager() ) )
	{
		SHADOWOBJECT ShadowObject;
		ShadowObject.pSceneObject = &Object;
		ShadowObject.ShadowType   = Type;

		m_ShadowObjects.Add(ShadowObject);

		return true;
	}

	return false;
}

bool CLight::Render()
{
	if(!m_pShadowMap || !m_pCamera)
		return false;

	return true;

	

	//CRenderSystem& RenderSystem = SceneManager.GetRenderSystem();

	//CGraphicsResource::HANDLE RenderTarget = RenderSystem.GetRenderTarget();

	//RenderSystem.SetRenderTarget( m_pShadowMap->GetResourceHandle() );

	////RenderSystem.Clear( 0,  );

	//CVector<SHADOWOBJECT>::CIterator Iterator = m_ShadowObjects.GetBeginIterator();

	//while( CVector<SHADOWOBJECT>::IsValuable(Iterator) )
	//{
	//	if(Iterator->ShadowType == CSceneObject::SHADOW_MAP && Iterator->pSceneObject)
	//		Iterator->pSceneObject->ApplyForRenderShadowMap();

	//	++ Iterator;
	//}

	//SceneManager.RenderNormalScene( ALCHEMY_FLAG(CRenderSystem::TARGET) | ALCHEMY_FLAG(CRenderSystem::ZBUFFER) );

	////SceneManager.GetRenderQueue().Render();

	//RenderSystem.SetRenderTarget(RenderTarget);


	return true;
}

void CLight::Destroy()
{
	if(m_pShadowMap)
	{
		_Release(*m_pShadowMap);

		m_pShadowMap = ALCHEMY_NULL;
	}

	ALCHEMY_DEBUG_DELETE(m_pCamera);
}