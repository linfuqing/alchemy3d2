#include "AlchemyEnvironmentProgram.h"
#include "AlchemySceneManager.h"
#include "AlchemySceneObject.h"

using namespace alchemy;

CEnvironmentProgram::CEnvironmentProgram()
:CProgramInstance(CSceneManager::EFFECT_BEFOREHAND)
{
	
}

CEnvironmentProgram::~CEnvironmentProgram(void)
{
	if(m_pEnvironmentTexture)
		m_pEnvironmentTexture->Release();
}

bool CEnvironmentProgram::Create(UINT uEnvironmentSize)
{
	m_pSceneManager = CSceneManager::GetInstance();

	m_pEnvironmentTexture = m_pSceneManager->GetRenderSystem().CreateCubeTexture(uEnvironmentSize, ALCHEMY_FLAG( TEXTURE::RENDER_TARGET ) | ALCHEMY_FLAG(TEXTURE::WRITE_ONLY), TEXTURE::A8R8G8B8);
	if(!m_pEnvironmentTexture)
		return false;
	
	return true;
}

bool CEnvironmentProgram::OnPreRenderSceneObject(CSceneObject* pSceneObject, UINT uFlag)
{
	CCamera& SceneCamera = m_pSceneManager->GetSceneCamera();
	static CCamera s_Camera;
	s_Camera.SetProjection(90.0f, SceneCamera.GetNearPlane(), SceneCamera.GetFarPlane(), 1.0f);

	static CMatrix4x4 s_CameraFaceWorldMatrix;
	static CQuaternion s_Rotate;

	const FLOAT3& ScenePos = pSceneObject->GetWorldMatrix().Row[3].Float3;
	CGraphicsResource::HANDLE RenderTarget = m_pSceneManager->GetSceneRenderTarget(), EnvironmentTexture = m_pEnvironmentTexture->GetResourceHandle();

	static FLOAT s_RotateVal = g_fDEG2RAD * 90.0f;
	m_pSceneManager->SetCamera( s_Camera );

	for (UINT i=0; i<6; ++i)
	{
		switch(i)
		{
		case CTexture::POSITIVE_X:
			s_Rotate.RotationYawPitchRoll(s_RotateVal, 0.0f, 0.0f);
			break;
		case CTexture::NEGATIVE_X:
			s_Rotate.RotationYawPitchRoll(-s_RotateVal, 0.0f, 0.0f);
			break;
		case CTexture::POSITIVE_Y:
			s_Rotate.RotationYawPitchRoll(0.0f, -s_RotateVal, 0.0f);
			break;
		case CTexture::NEGATIVE_Y:
			s_Rotate.RotationYawPitchRoll(0.0f, s_RotateVal, 0.0f);
			break;
		case CTexture::POSITIVE_Z:
			s_Rotate.RotationYawPitchRoll(0.0f, 0.0f, 0.0f);
			break;
		case CTexture::NEGATIVE_Z:
			s_Rotate.RotationYawPitchRoll(-s_RotateVal*2.0f, 0.0f, 0.0f);
			break;
		}

		s_CameraFaceWorldMatrix.RotationQuaternion(s_Rotate);
		s_CameraFaceWorldMatrix._41 = ScenePos.x; s_CameraFaceWorldMatrix._42 = ScenePos.y; s_CameraFaceWorldMatrix._43 = ScenePos.z;
		s_Camera.SetMatrix(s_CameraFaceWorldMatrix);
		s_Camera.Update();
		m_pSceneManager->SetSceneRenderTarget(EnvironmentTexture, i << CTexture::CUBE_TEXTURE_SURFACE_SHIFT);
		m_pSceneManager->RenderNormalScene( ALCHEMY_FLAG(CRenderSystem::TARGET) | ALCHEMY_FLAG( CRenderSystem::ZBUFFER ) );
	}

	m_pSceneManager->SetCamera(SceneCamera);
	m_pSceneManager->SetSceneRenderTarget(RenderTarget);

	return true;
}

//bool CEnvironmentProgram::OnPreRenderSceneObject(CSceneObject* pSceneObject, UINT uFlag)
//{
//	CCamera& SceneCamera = m_pSceneManager->GetSceneCamera();
//
//	static CCamera s_Camera;
//	static CMatrix4x4 s_CameraWorldMatrix, s_CameraFaceWorldMatrix, s_FinalMatrix;
//	static CQuaternion s_Rotate;
//
//	s_CameraWorldMatrix = SceneCamera.GetWorldMatrix();
//	s_CameraWorldMatrix._41 = s_CameraWorldMatrix._42 = s_CameraWorldMatrix._43 = 0.0f;
//	s_Camera.SetProjection(90.0f, SceneCamera.GetNearPlane(), SceneCamera.GetFarPlane(), 1.0f);
//
//
//	const FLOAT3& ScenePos = pSceneObject->GetWorldMatrix().Row[3].Float3;
//	CGraphicsResource::HANDLE RenderTarget = m_pSceneManager->GetSceneRenderTarget(), EnvironmentTexture = m_pEnvironmentTexture->GetResourceHandle();
//
//	m_pSceneManager->SetSceneRenderTarget(EnvironmentTexture);
//
//	static FLOAT s_RotateVal = g_fDEG2RAD * 90.0f;
//	m_pSceneManager->SetCamera( s_Camera );
//
//	for (UINT i=0; i<6; ++i)
//	{
//		switch(i)
//		{
//		case CTexture::POSITIVE_X:
//			s_Rotate.RotationYawPitchRoll(s_RotateVal, 0.0f, 0.0f);
//			break;
//		case CTexture::NEGATIVE_X:
//			s_Rotate.RotationYawPitchRoll(-s_RotateVal, 0.0f, 0.0f);
//			break;
//		case CTexture::POSITIVE_Y:
//			s_Rotate.RotationYawPitchRoll(0.0f, -s_RotateVal, 0.0f);
//			break;
//		case CTexture::NEGATIVE_Y:
//			s_Rotate.RotationYawPitchRoll(0.0f, s_RotateVal, 0.0f);
//			break;
//		case CTexture::POSITIVE_Z:
//			s_Rotate.RotationYawPitchRoll(0.0f, 0.0f, 0.0f);
//			break;
//		case CTexture::NEGATIVE_Z:
//			s_Rotate.RotationYawPitchRoll(-s_RotateVal*2.0f, 0.0f, 0.0f);
//			break;
//		}
//
//		s_CameraFaceWorldMatrix.RotationQuaternion(s_Rotate);
//		Multiply(s_FinalMatrix,s_CameraWorldMatrix, s_CameraFaceWorldMatrix);
//		s_FinalMatrix._41 =  ScenePos.x; s_FinalMatrix._42 =  ScenePos.y; s_FinalMatrix._43 =  ScenePos.z;
//		s_Camera.SetMatrix(s_FinalMatrix);
//		s_Camera.Update();
//		m_pSceneManager->SetSceneRenderTarget(EnvironmentTexture, i << CTexture::CUBE_TEXTURE_SURFACE_SHIFT);
//		m_pSceneManager->RenderNormalScene( ALCHEMY_FLAG(CRenderSystem::TARGET) | ALCHEMY_FLAG( CRenderSystem::ZBUFFER ) );
//	}
//
//	m_pSceneManager->SetCamera(SceneCamera);
//	m_pSceneManager->SetSceneRenderTarget(RenderTarget);
//
//	return true;
//}