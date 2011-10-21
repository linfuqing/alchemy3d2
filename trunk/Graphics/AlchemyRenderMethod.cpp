#include "AlchemyRenderMethod.h"
#include "../Core/AlchemyUtils.h"
#include "AlchemySceneManager.h"

using namespace alchemy;

CRenderMethod::CRenderMethod(void) :
m_pParameterList(ALCHEMY_NULL),
m_pParameterListBuffer(ALCHEMY_NULL),
m_pEffectParameterHandler(ALCHEMY_NULL),
m_pEffect(ALCHEMY_NULL),
m_uPass(0)
{
}

CRenderMethod::~CRenderMethod(void)
{
	if(m_pParameterListBuffer)
	{
		m_pParameterListBuffer->Release();

		m_pParameterListBuffer = ALCHEMY_NULL;

		m_pParameterList = ALCHEMY_NULL;
	}

	if(m_pParameterList)
	{
		UINT uPassCount = m_pEffect->GetTotalPass();
		DestroyParameterList(m_pParameterList, uPassCount);
	}

	if(m_pEffectParameterHandler)
	{
		m_pEffectParameterHandler->Release();

		m_pEffectParameterHandler = ALCHEMY_NULL;
	}

	if(m_pEffect)
	{
		_Release(*m_pEffect);

		m_pEffect = ALCHEMY_NULL;
	}
}

CBuffer& CRenderMethod::CreateParameterListBuffer(const PARAMETERLIST* pParameterList, UINT uParameterListCount)
{
	UINT uBaseSize = sizeof(PARAMETERLIST) * uParameterListCount, uBufferSize = uBaseSize, i;

	const PARAMETERLIST* pCurrentParameterList;

	for(i = 0; i < uParameterListCount; ++ i)
	{
		pCurrentParameterList = &pParameterList[i];

		uBufferSize += pCurrentParameterList->uLightCount   * ( sizeof(PARAMETERINFO) * TOTAL_LIGHT_PARAMETERS   + sizeof(UINT)                         );
		uBufferSize += pCurrentParameterList->uTextureCount * ( sizeof(PARAMETERINFO) * TOTAL_TEXTURE_PARAMETERS + sizeof(UINT) + sizeof(PARAMETERINFO) );

		uBufferSize += pCurrentParameterList->uVertexElementParameterCount * sizeof(CEffectCode::ELEMENTPARAMETER);
	}

	CBuffer& Buffer = CreateBuffer(uBufferSize);
	PUINT8 puParameterList = reinterpret_cast<PUINT8>( Buffer.GetBufferPointer() );
	LPPARAMETERLIST pDestinationParameterList = reinterpret_cast<LPPARAMETERLIST>(puParameterList);

	UINT uOffset = uBaseSize, j, k;
	for(i = 0; i < uParameterListCount; ++ i)
	{
		pCurrentParameterList = &pParameterList[i];

		*pDestinationParameterList = *pCurrentParameterList;

		pDestinationParameterList->pLightParameters = reinterpret_cast<PARAMETERINFO(*)[TOTAL_LIGHT_PARAMETERS]>(puParameterList + uOffset);

		uOffset += pCurrentParameterList->uLightCount * sizeof(PARAMETERINFO) * TOTAL_LIGHT_PARAMETERS;

		pDestinationParameterList->puLightParameterFlag = reinterpret_cast<PUINT>(puParameterList + uOffset);

		uOffset += pCurrentParameterList->uLightCount * sizeof(UINT);

		for(j = 0; j < pCurrentParameterList->uLightCount; ++ j)
		{
			for(k = 0; k < TOTAL_LIGHT_PARAMETERS; ++ k)
				pDestinationParameterList->pLightParameters[j][k] = pCurrentParameterList->pLightParameters[j][k];

			pDestinationParameterList->puLightParameterFlag[j]    = pCurrentParameterList->puLightParameterFlag[j];
		}

		pDestinationParameterList->pTextureParameters = reinterpret_cast<PARAMETERINFO(*)[TOTAL_TEXTURE_PARAMETERS]>(puParameterList + uOffset);

		uOffset += pCurrentParameterList->uTextureCount * sizeof(PARAMETERINFO) * TOTAL_TEXTURE_PARAMETERS;

		pDestinationParameterList->puTextureParameterFlag = reinterpret_cast<PUINT>(puParameterList + uOffset);

		uOffset += pCurrentParameterList->uTextureCount * sizeof(UINT);

		pDestinationParameterList->pTextures = reinterpret_cast<LPPARAMETERINFO>(puParameterList + uOffset);

		uOffset += pCurrentParameterList->uTextureCount * sizeof(PARAMETERINFO);

		for(j = 0; j < pCurrentParameterList->uTextureCount; ++ j)
		{
			for(k = 0; k < CRenderMethod::TOTAL_TEXTURE_PARAMETERS; ++ k)
				pDestinationParameterList->pTextureParameters[j][k] = pCurrentParameterList->pTextureParameters[j][k];

			pDestinationParameterList->puTextureParameterFlag[j]    = pCurrentParameterList->puTextureParameterFlag[j];

			pDestinationParameterList->pTextures[j] = pCurrentParameterList->pTextures[j];
		}

		pDestinationParameterList->pVertexElementParameters = reinterpret_cast<CEffectCode::LPELEMENTPARAMETER>(puParameterList + uOffset);

		uOffset += pCurrentParameterList->uVertexElementParameterCount * sizeof(CEffectCode::ELEMENTPARAMETER);

		for(j = 0; j < pCurrentParameterList->uVertexElementParameterCount; ++ j)
			pDestinationParameterList->pVertexElementParameters[j] = pCurrentParameterList->pVertexElementParameters[j];

		++ pDestinationParameterList;
	}

	return Buffer;
}

void CRenderMethod::DestroyParameterList(LPPARAMETERLIST pParameterList, UINT uParameterListCount)
{
	if(!pParameterList)
		return;

	LPPARAMETERLIST pCurrentParameterList;
	for(UINT i = 0; i < uParameterListCount; ++ i)
	{
		pCurrentParameterList = &pParameterList[i];

		ALCHEMY_DEBUG_DELETE_ARRAY(pCurrentParameterList->pLightParameters);

		ALCHEMY_DEBUG_DELETE_ARRAY(pCurrentParameterList->pTextureParameters);

		ALCHEMY_DEBUG_DELETE_ARRAY(pCurrentParameterList->pTextures);

		ALCHEMY_DEBUG_DELETE_ARRAY(pCurrentParameterList->puLightParameterFlag);

		ALCHEMY_DEBUG_DELETE_ARRAY(pCurrentParameterList->puTextureParameterFlag);

		ALCHEMY_DEBUG_DELETE_ARRAY(pCurrentParameterList->pVertexElementParameters);
	}

	ALCHEMY_DEBUG_DELETE_ARRAY(pParameterList);
}

void CRenderMethod::Destroy()
{
	if(m_pParameterListBuffer)
	{
		m_pParameterListBuffer->Release();

		m_pParameterListBuffer = ALCHEMY_NULL;

		m_pParameterList = ALCHEMY_NULL;
	}

	if(m_pParameterList)
	{
		UINT uPassCount = m_pEffect->GetTotalPass();
		DestroyParameterList(m_pParameterList, uPassCount);
	}

	if(m_pEffectParameterHandler)
	{
		m_pEffectParameterHandler->Release();

		m_pEffectParameterHandler = ALCHEMY_NULL;
	}

	if(m_pEffect)
	{
		_Release(*m_pEffect);

		m_pEffect = ALCHEMY_NULL;
	}
}

bool CRenderMethod::SetParameter(PARAMETER Index, const FLOAT* pfData)
{
	if(!m_pParameterList || !m_pEffect)
	{
		ALCHEMY_DEBUG_WARNING("Render method has not been created.");

		return false;
	}

	LPPARAMETERLIST pParameterList = &m_pParameterList[m_uPass];
	if(!pParameterList->uParameterFlag)
		return true;

	const static UINT s_uSIZE[CRenderMethodInterface::TOTAL_PARAMETERS] = {3, 2, 1, 1, 4};

	UINT uSize = s_uSIZE[Index];

	if( ALCHEMY_TEST_BIT(pParameterList->uParameterFlag, Index) )
	{
		LPPARAMETERINFO pParameterInfo = &pParameterList->Parameters[Index];

		if(pParameterInfo->Type == CEffectInterface::PROGRAM)
		{
			m_pEffect->SetParameter(CEffectInterface::VERTEX_SHADER  , pParameterInfo->uAlphaIndex, pfData, uSize);
			m_pEffect->SetParameter(CEffectInterface::FRAGMENT_SHADER, pParameterInfo->uBetaIndex , pfData, uSize);
		}
		else
			m_pEffect->SetParameter(pParameterInfo->Type, pParameterInfo->uAlphaIndex, pfData, uSize);
	}

	return true;
}


bool CRenderMethod::SetArray(CRenderMethodInterface::ARRAYPARAMETER Index, const void* pData, UINT uCount, bool bIsPointer)
{
	if(!m_pParameterList || !m_pEffect)
	{
		ALCHEMY_DEBUG_WARNING("Render method has not been created.");

		return false;
	}

	if(!pData || !uCount)
		return false;

	LPPARAMETERLIST pParameterList = &m_pParameterList[m_uPass];
	if(!pParameterList->uArrayParameterFlag)
		return true;

	if( ALCHEMY_TEST_BIT(pParameterList->uArrayParameterFlag, BONE_MATRIX_ARRAY) )
	{
		LPPARAMETERINFO pParameterInfo = &pParameterList->ArrayParameters[BONE_MATRIX_ARRAY];

		if(pParameterInfo->Type == CEffectInterface::PROGRAM)
		{
			if(bIsPointer)
			{
				m_pEffect->SetMatrixPointerArray(CEffectInterface::VERTEX_SHADER  , pParameterInfo->uAlphaIndex, reinterpret_cast<const PFLOAT*>(pData), 3, 4, 4, uCount);
				m_pEffect->SetMatrixPointerArray(CEffectInterface::FRAGMENT_SHADER, pParameterInfo->uBetaIndex , reinterpret_cast<const PFLOAT*>(pData), 3, 4, 4, uCount);
			}
			else
			{
				m_pEffect->SetMatrixArray(CEffectInterface::VERTEX_SHADER, pParameterInfo->uAlphaIndex, reinterpret_cast<const FLOAT*>(pData), 3, 4, 4, 16, uCount);
				m_pEffect->SetMatrixArray(CEffectInterface::FRAGMENT_SHADER, pParameterInfo->uBetaIndex , reinterpret_cast<const FLOAT*>(pData), 3, 4, 4, 16, uCount);
			}
		}
		else
		{
			if(bIsPointer)
				m_pEffect->SetMatrixPointerArray(pParameterInfo->Type, pParameterInfo->uAlphaIndex, reinterpret_cast<const PFLOAT*>(pData), 3, 4, 4, uCount);
			else
				m_pEffect->SetMatrixArray(pParameterInfo->Type, pParameterInfo->uAlphaIndex, reinterpret_cast<const FLOAT*>(pData), 3, 4, 4, 16, uCount);
		}
	}

	return true;
}

bool CRenderMethod::SetLight(const CLight& Light, UINT uIndex)
{
	if(!m_pParameterList || !m_pEffect)
	{
		ALCHEMY_DEBUG_WARNING("Render method has not been created.");

		return false;
	}

	LPPARAMETERLIST pParameterList = &m_pParameterList[m_uPass];

	if(uIndex < pParameterList->uLightCount)
	{
		UINT uFlag = pParameterList->puLightParameterFlag[uIndex];
		if(!uFlag)
			return true;

		LPPARAMETERINFO pParameterInfo = pParameterList->pLightParameters[uIndex], pCurrentParameterInfo;

		static CVector3 Direction;
		static CVector4 Position;

		if( ALCHEMY_TEST_BIT(uFlag, LIGHT_AMBIENT) )
		{
			pCurrentParameterInfo = &pParameterInfo[LIGHT_AMBIENT];

			if(pParameterInfo->Type == CEffectInterface::PROGRAM)
			{
				m_pEffect->SetParameter(CEffectInterface::VERTEX_SHADER  , pCurrentParameterInfo->uAlphaIndex, static_cast<const FLOAT*>( Light.GetAmbient() ), 4);
				m_pEffect->SetParameter(CEffectInterface::FRAGMENT_SHADER, pCurrentParameterInfo->uBetaIndex , static_cast<const FLOAT*>( Light.GetAmbient() ), 4);
			}
			else
				m_pEffect->SetParameter(pCurrentParameterInfo->Type, pCurrentParameterInfo->uAlphaIndex, static_cast<const FLOAT*>( Light.GetAmbient() ), 4);
		}

		if( ALCHEMY_TEST_BIT(uFlag, LIGHT_DIFFUSE) )
		{
			pCurrentParameterInfo = &pParameterInfo[LIGHT_DIFFUSE];

			if(pParameterInfo->Type == CEffectInterface::PROGRAM)
			{
				m_pEffect->SetParameter(CEffectInterface::VERTEX_SHADER  , pCurrentParameterInfo->uAlphaIndex, static_cast<const FLOAT*>( Light.GetDiffuse() ), 4);
				m_pEffect->SetParameter(CEffectInterface::FRAGMENT_SHADER, pCurrentParameterInfo->uBetaIndex , static_cast<const FLOAT*>( Light.GetDiffuse() ), 4);
			}
			else
				m_pEffect->SetParameter(pCurrentParameterInfo->Type, pCurrentParameterInfo->uAlphaIndex, static_cast<const FLOAT*>( Light.GetDiffuse() ), 4);
		}

		if( ALCHEMY_TEST_BIT(uFlag, LIGHT_SPECULAR) )
		{
			pCurrentParameterInfo = &pParameterInfo[LIGHT_SPECULAR];

			if(pParameterInfo->Type == CEffectInterface::PROGRAM)
			{
				m_pEffect->SetParameter(CEffectInterface::VERTEX_SHADER  , pCurrentParameterInfo->uAlphaIndex, static_cast<const FLOAT*>( Light.GetSpecular() ), 4);
				m_pEffect->SetParameter(CEffectInterface::FRAGMENT_SHADER, pCurrentParameterInfo->uBetaIndex , static_cast<const FLOAT*>( Light.GetSpecular() ), 4);
			}
			else
				m_pEffect->SetParameter(pCurrentParameterInfo->Type, pCurrentParameterInfo->uAlphaIndex, static_cast<const FLOAT*>( Light.GetSpecular() ), 4);
		}

		if( ALCHEMY_TEST_BIT(uFlag, LIGHT_POSITION) )
		{
			pCurrentParameterInfo = &pParameterInfo[LIGHT_POSITION];

			if(pParameterInfo->Type == CEffectInterface::PROGRAM)
			{
				m_pEffect->SetParameter(CEffectInterface::VERTEX_SHADER  , pCurrentParameterInfo->uAlphaIndex, static_cast<const FLOAT*>( Light.GetPosition() ), 4);
				m_pEffect->SetParameter(CEffectInterface::FRAGMENT_SHADER, pCurrentParameterInfo->uBetaIndex , static_cast<const FLOAT*>( Light.GetPosition() ), 4);
			}
			else
				m_pEffect->SetParameter(pCurrentParameterInfo->Type, pCurrentParameterInfo->uAlphaIndex, static_cast<const FLOAT*>( Light.GetPosition() ), 4);
		}

		if( ALCHEMY_TEST_BIT(uFlag, LIGHT_DIRECTION) )
		{
			pCurrentParameterInfo = &pParameterInfo[LIGHT_DIRECTION];

			if(pParameterInfo->Type == CEffectInterface::PROGRAM)
			{
				m_pEffect->SetParameter(CEffectInterface::VERTEX_SHADER  , pCurrentParameterInfo->uAlphaIndex, static_cast<const FLOAT*>( Light.GetDirection() ), 3);
				m_pEffect->SetParameter(CEffectInterface::FRAGMENT_SHADER, pCurrentParameterInfo->uBetaIndex , static_cast<const FLOAT*>( Light.GetDirection() ), 3);
			}
			else
				m_pEffect->SetParameter(pCurrentParameterInfo->Type, pCurrentParameterInfo->uAlphaIndex, static_cast<const FLOAT*>( Light.GetDirection() ), 3);
		}

		if( ALCHEMY_TEST_BIT(uFlag, LIGHT_RANGE) )
		{
			pCurrentParameterInfo = &pParameterInfo[LIGHT_RANGE];

			FLOAT fRange = Light.GetRange();

			if(pParameterInfo->Type == CEffectInterface::PROGRAM)
			{
				m_pEffect->SetParameter(CEffectInterface::VERTEX_SHADER  , pCurrentParameterInfo->uAlphaIndex, &fRange, 1);
				m_pEffect->SetParameter(CEffectInterface::FRAGMENT_SHADER, pCurrentParameterInfo->uBetaIndex , &fRange, 1);
			}
			else
				m_pEffect->SetParameter(pCurrentParameterInfo->Type, pCurrentParameterInfo->uAlphaIndex, &fRange, 1);
		}

		if( ALCHEMY_TEST_BIT(uFlag, LIGHT_FALLOFF) )
		{
			pCurrentParameterInfo = &pParameterInfo[LIGHT_FALLOFF];

			FLOAT fFalloff = Light.GetFalloff();

			if(pParameterInfo->Type == CEffectInterface::PROGRAM)
			{
				m_pEffect->SetParameter(CEffectInterface::VERTEX_SHADER  , pCurrentParameterInfo->uAlphaIndex, &fFalloff, 1);
				m_pEffect->SetParameter(CEffectInterface::FRAGMENT_SHADER, pCurrentParameterInfo->uBetaIndex , &fFalloff, 1);
			}
			else
				m_pEffect->SetParameter(pCurrentParameterInfo->Type, pCurrentParameterInfo->uAlphaIndex, &fFalloff, 1);
		}

		if( ALCHEMY_TEST_BIT(uFlag, LIGHT_ATTENUATION0) )
		{
			pCurrentParameterInfo = &pParameterInfo[LIGHT_ATTENUATION0];

			FLOAT fAttenuation0 = Light.GetAttenuation0();

			if(pParameterInfo->Type == CEffectInterface::PROGRAM)
			{
				m_pEffect->SetParameter(CEffectInterface::VERTEX_SHADER  , pCurrentParameterInfo->uAlphaIndex, &fAttenuation0, 1);
				m_pEffect->SetParameter(CEffectInterface::FRAGMENT_SHADER, pCurrentParameterInfo->uBetaIndex , &fAttenuation0, 1);
			}
			else
				m_pEffect->SetParameter(pCurrentParameterInfo->Type, pCurrentParameterInfo->uAlphaIndex, &fAttenuation0, 1);
		}

		if( ALCHEMY_TEST_BIT(uFlag, LIGHT_ATTENUATION1) )
		{
			pCurrentParameterInfo = &pParameterInfo[LIGHT_ATTENUATION1];

			FLOAT fAttenuation1 = Light.GetAttenuation1();

			if(pParameterInfo->Type == CEffectInterface::PROGRAM)
			{
				m_pEffect->SetParameter(CEffectInterface::VERTEX_SHADER  , pCurrentParameterInfo->uAlphaIndex, &fAttenuation1, 1);
				m_pEffect->SetParameter(CEffectInterface::FRAGMENT_SHADER, pCurrentParameterInfo->uBetaIndex , &fAttenuation1, 1);
			}
			else
				m_pEffect->SetParameter(pCurrentParameterInfo->Type, pCurrentParameterInfo->uAlphaIndex, &fAttenuation1, 1);
		}

		if( ALCHEMY_TEST_BIT(uFlag, LIGHT_ATTENUATION2) )
		{
			pCurrentParameterInfo = &pParameterInfo[LIGHT_ATTENUATION2];

			FLOAT fAttenuation2 = Light.GetAttenuation2();

			if(pParameterInfo->Type == CEffectInterface::PROGRAM)
			{
				m_pEffect->SetParameter(CEffectInterface::VERTEX_SHADER  , pCurrentParameterInfo->uAlphaIndex, &fAttenuation2, 1);
				m_pEffect->SetParameter(CEffectInterface::FRAGMENT_SHADER, pCurrentParameterInfo->uBetaIndex , &fAttenuation2, 1);
			}
			else
				m_pEffect->SetParameter(pCurrentParameterInfo->Type, pCurrentParameterInfo->uAlphaIndex, &fAttenuation2, 1);
		}

		if( ALCHEMY_TEST_BIT(uFlag, LIGHT_THETA) )
		{
			pCurrentParameterInfo = &pParameterInfo[LIGHT_THETA];

			FLOAT fTheta = Light.GetTheta();

			if(pParameterInfo->Type == CEffectInterface::PROGRAM)
			{
				m_pEffect->SetParameter(CEffectInterface::VERTEX_SHADER  , pCurrentParameterInfo->uAlphaIndex, &fTheta, 1);
				m_pEffect->SetParameter(CEffectInterface::FRAGMENT_SHADER, pCurrentParameterInfo->uBetaIndex , &fTheta, 1);
			}
			else
				m_pEffect->SetParameter(pCurrentParameterInfo->Type, pCurrentParameterInfo->uAlphaIndex, &fTheta, 1);
		}

		if( ALCHEMY_TEST_BIT(uFlag, LIGHT_PHI) )
		{
			pCurrentParameterInfo = &pParameterInfo[LIGHT_PHI];

			FLOAT fPhi = Light.GetPhi();

			if(pParameterInfo->Type == CEffectInterface::PROGRAM)
			{
				m_pEffect->SetParameter(CEffectInterface::VERTEX_SHADER  , pCurrentParameterInfo->uAlphaIndex, &fPhi, 1);
				m_pEffect->SetParameter(CEffectInterface::FRAGMENT_SHADER, pCurrentParameterInfo->uBetaIndex , &fPhi, 1);
			}
			else
				m_pEffect->SetParameter(pCurrentParameterInfo->Type, pCurrentParameterInfo->uAlphaIndex, &fPhi, 1);
		}

		if( ALCHEMY_TEST_BIT(uFlag, LIGHT_COS_THETA) )
		{
			pCurrentParameterInfo = &pParameterInfo[LIGHT_COS_THETA];

			FLOAT fCosTheta = cosf( Light.GetTheta() );

			if(pParameterInfo->Type == CEffectInterface::PROGRAM)
			{
				m_pEffect->SetParameter(CEffectInterface::VERTEX_SHADER  , pCurrentParameterInfo->uAlphaIndex, &fCosTheta, 1);
				m_pEffect->SetParameter(CEffectInterface::FRAGMENT_SHADER, pCurrentParameterInfo->uBetaIndex , &fCosTheta, 1);
			}
			else
				m_pEffect->SetParameter(pCurrentParameterInfo->Type, pCurrentParameterInfo->uAlphaIndex, &fCosTheta, 1);
		}

		return true;
	}

	return false;
}

bool CRenderMethod::Activate(UINT uPass)
{
	if(!m_pParameterList || !m_pEffect)
	{
		ALCHEMY_DEBUG_WARNING("CRenderMethod::Activate: Render method has not been created.");

		return false;
	}

	if( uPass >= m_pEffect->GetTotalPass() )
	{
		ALCHEMY_DEBUG_WARNING("CRenderMethod::Activate: Pass out of range.");

		return false;
	}

	m_uPass = uPass;

	if(m_pEffectParameterHandler)
		m_pEffectParameterHandler->SetParameters(*m_pEffect, uPass);

	LPPARAMETERLIST pParameterList = &m_pParameterList[uPass];

	CSceneManager& SceneManager = static_cast<CSceneManager&>( CGraphicsResource::GetGraphicsResourcePool() );
	if(pParameterList->uParameterFlag)
	{
		SetParameter( CRenderMethodInterface::AMBIENT_LIGHT, static_cast<const FLOAT*>( SceneManager.GetAmbientLight() ) );
	}

	if(pParameterList->uConstantParameterFlag)
	{
		LPPARAMETERINFO pParameterInfo;
		FLOAT fConstant;

		if( ALCHEMY_TEST_BIT(pParameterList->uConstantParameterFlag, CONSTANT_ZERO) )
		{
			pParameterInfo = &pParameterList->ConstantParameters[CONSTANT_ZERO];
			fConstant = 0.0f;

			if(pParameterInfo->Type == CEffectInterface::PROGRAM)
			{
				m_pEffect->SetParameter(CEffectInterface::VERTEX_SHADER  , pParameterInfo->uAlphaIndex, &fConstant, 1);
				m_pEffect->SetParameter(CEffectInterface::FRAGMENT_SHADER, pParameterInfo->uBetaIndex , &fConstant, 1);
			}
			else
				m_pEffect->SetParameter(pParameterInfo->Type, pParameterInfo->uAlphaIndex, &fConstant, 1);
		}

		if( ALCHEMY_TEST_BIT(pParameterList->uConstantParameterFlag, CONSTANT_ONE) )
		{
			pParameterInfo = &pParameterList->ConstantParameters[CONSTANT_ONE];
			fConstant = 1.0f;

			if(pParameterInfo->Type == CEffectInterface::PROGRAM)
			{
				m_pEffect->SetParameter(CEffectInterface::VERTEX_SHADER  , pParameterInfo->uAlphaIndex, &fConstant, 1);
				m_pEffect->SetParameter(CEffectInterface::FRAGMENT_SHADER, pParameterInfo->uBetaIndex , &fConstant, 1);
			}
			else
				m_pEffect->SetParameter(pParameterInfo->Type, pParameterInfo->uAlphaIndex, &fConstant, 1);
		}

		if( ALCHEMY_TEST_BIT(pParameterList->uConstantParameterFlag, CONSTANT_HALF_ONE) )
		{
			pParameterInfo = &pParameterList->ConstantParameters[CONSTANT_HALF_ONE];
			fConstant = 0.5f;

			if(pParameterInfo->Type == CEffectInterface::PROGRAM)
			{
				m_pEffect->SetParameter(CEffectInterface::VERTEX_SHADER  , pParameterInfo->uAlphaIndex, &fConstant, 1);
				m_pEffect->SetParameter(CEffectInterface::FRAGMENT_SHADER, pParameterInfo->uBetaIndex , &fConstant, 1);
			}
			else
				m_pEffect->SetParameter(pParameterInfo->Type, pParameterInfo->uAlphaIndex, &fConstant, 1);
		}

		if( ALCHEMY_TEST_BIT(pParameterList->uConstantParameterFlag, CONSTANT_TWO) )
		{
			pParameterInfo = &pParameterList->ConstantParameters[CONSTANT_TWO];
			fConstant = 2.0f;

			if(pParameterInfo->Type == CEffectInterface::PROGRAM)
			{
				m_pEffect->SetParameter(CEffectInterface::VERTEX_SHADER  , pParameterInfo->uAlphaIndex, &fConstant, 1);
				m_pEffect->SetParameter(CEffectInterface::FRAGMENT_SHADER, pParameterInfo->uBetaIndex , &fConstant, 1);
			}
			else
				m_pEffect->SetParameter(pParameterInfo->Type, pParameterInfo->uAlphaIndex, &fConstant, 1);
		}

		if( ALCHEMY_TEST_BIT(pParameterList->uConstantParameterFlag, CONSTANT_FOUR) )
		{
			pParameterInfo = &pParameterList->ConstantParameters[CONSTANT_FOUR];
			fConstant = 4.0f;

			if(pParameterInfo->Type == CEffectInterface::PROGRAM)
			{
				m_pEffect->SetParameter(CEffectInterface::VERTEX_SHADER  , pParameterInfo->uAlphaIndex, &fConstant, 1);
				m_pEffect->SetParameter(CEffectInterface::FRAGMENT_SHADER, pParameterInfo->uBetaIndex , &fConstant, 1);
			}
			else
				m_pEffect->SetParameter(pParameterInfo->Type, pParameterInfo->uAlphaIndex, &fConstant, 1);
		}
	}

	UINT uLightCount = SceneManager.GetLightCount();

	CLight* pLight;
	for(UINT i = 0; i < uLightCount && i < pParameterList->uLightCount; ++ i)
	{
		pLight = SceneManager.GetLight(i);
		if(pLight)
			SetLight(*pLight, i);
	}

	return true;
}

bool CRenderMethod::SetParameters(const CMatrix4x4& WorldMatrix, const CCamera& Camera, const CMaterial& Material)
{
	if(!m_pParameterList || !m_pEffect)
	{
		ALCHEMY_DEBUG_WARNING("Render method has not been created.");

		return false;
	}

	static CMatrix4x4 Matrix, InverceWorldMatrix;
	static CVector3 Direction;
	static CVector4 Position;
	bool bIsIverceWorldMatrix = false;

	LPPARAMETERLIST pParameterList = &m_pParameterList[m_uPass];
	LPPARAMETERINFO pParameterInfo;
	if(pParameterList->uMatrixParameterFlag)
	{
		if( ALCHEMY_TEST_BIT(pParameterList->uMatrixParameterFlag, WORLD_MATRIX) )
		{
			pParameterInfo = &pParameterList->MatrixParameters[WORLD_MATRIX];
			if(pParameterInfo->Type == CEffectInterface::PROGRAM)
			{
				m_pEffect->SetMatrix(CEffectInterface::VERTEX_SHADER  , pParameterInfo->uAlphaIndex, WorldMatrix);
				m_pEffect->SetMatrix(CEffectInterface::FRAGMENT_SHADER, pParameterInfo->uBetaIndex , WorldMatrix);
			}
			else
				m_pEffect->SetMatrix(pParameterInfo->Type, pParameterInfo->uAlphaIndex, Matrix);
		}

		if( ALCHEMY_TEST_BIT(pParameterList->uMatrixParameterFlag, VIEW_MATRIX) )
		{
			pParameterInfo = &pParameterList->MatrixParameters[VIEW_MATRIX];

			if(pParameterInfo->Type == CEffectInterface::PROGRAM)
			{
				m_pEffect->SetMatrix( CEffectInterface::VERTEX_SHADER  , pParameterInfo->uAlphaIndex, Camera.GetViewMatrix() );
				m_pEffect->SetMatrix( CEffectInterface::FRAGMENT_SHADER, pParameterInfo->uBetaIndex , Camera.GetViewMatrix() );
			}
			else
				m_pEffect->SetMatrix( pParameterInfo->Type, pParameterInfo->uAlphaIndex, Camera.GetViewMatrix() );
		}

		if( ALCHEMY_TEST_BIT(pParameterList->uMatrixParameterFlag, PROJECTION_MATRIX) )
		{
			pParameterInfo = &pParameterList->MatrixParameters[PROJECTION_MATRIX];

			if(pParameterInfo->Type == CEffectInterface::PROGRAM)
			{
				m_pEffect->SetMatrix( CEffectInterface::VERTEX_SHADER  , pParameterInfo->uAlphaIndex, Camera.GetProjectionMatrix() );
				m_pEffect->SetMatrix( CEffectInterface::FRAGMENT_SHADER, pParameterInfo->uBetaIndex , Camera.GetProjectionMatrix() );
			}
			else
				m_pEffect->SetMatrix( pParameterInfo->Type, pParameterInfo->uAlphaIndex, Camera.GetProjectionMatrix() );
		}

		if( ALCHEMY_TEST_BIT(pParameterList->uMatrixParameterFlag, VIEW_PROJECTION_MATRIX) )
		{
			pParameterInfo = &pParameterList->MatrixParameters[VIEW_PROJECTION_MATRIX];

			if(pParameterInfo->Type == CEffectInterface::PROGRAM)
			{
				m_pEffect->SetMatrix( CEffectInterface::VERTEX_SHADER  , pParameterInfo->uAlphaIndex, Camera.GetViewProjectionMatrix() );
				m_pEffect->SetMatrix( CEffectInterface::FRAGMENT_SHADER, pParameterInfo->uBetaIndex , Camera.GetViewProjectionMatrix() );
			}
			else
				m_pEffect->SetMatrix( pParameterInfo->Type, pParameterInfo->uAlphaIndex, Camera.GetViewProjectionMatrix() );
		}

		if( ALCHEMY_TEST_BIT(pParameterList->uMatrixParameterFlag, WORLD_VIEW_MATRIX) )
		{
			pParameterInfo = &pParameterList->MatrixParameters[WORLD_VIEW_MATRIX];

			Matrix  = WorldMatrix;
			Matrix *= Camera.GetViewMatrix();

			if(pParameterInfo->Type == CEffectInterface::PROGRAM)
			{
				m_pEffect->SetMatrix(CEffectInterface::VERTEX_SHADER  , pParameterInfo->uAlphaIndex, Matrix);
				m_pEffect->SetMatrix(CEffectInterface::FRAGMENT_SHADER, pParameterInfo->uBetaIndex , Matrix);
			}
			else
				m_pEffect->SetMatrix(pParameterInfo->Type, pParameterInfo->uAlphaIndex, Matrix);
		}

		if( ALCHEMY_TEST_BIT(pParameterList->uMatrixParameterFlag, WORLD_VIEW_PROJECTION_MATRIX) )
		{
			pParameterInfo = &pParameterList->MatrixParameters[WORLD_VIEW_PROJECTION_MATRIX];

			Matrix  = WorldMatrix;
			Matrix *= Camera.GetViewProjectionMatrix();

			if(pParameterInfo->Type == CEffectInterface::PROGRAM)
			{
				m_pEffect->SetMatrix(CEffectInterface::VERTEX_SHADER  , pParameterInfo->uAlphaIndex, Matrix);
				m_pEffect->SetMatrix(CEffectInterface::FRAGMENT_SHADER, pParameterInfo->uBetaIndex , Matrix);
			}
			else
				m_pEffect->SetMatrix(pParameterInfo->Type, pParameterInfo->uAlphaIndex, Matrix);
		}

		if( ALCHEMY_TEST_BIT(pParameterList->uMatrixParameterFlag, INVERSE_WORLD_MATRIX) )
		{
			if(!bIsIverceWorldMatrix)
			{
				InverceWorldMatrix   = WorldMatrix;
				bIsIverceWorldMatrix = InverceWorldMatrix.Invert3x4();
			}

			pParameterInfo = &pParameterList->MatrixParameters[INVERSE_WORLD_MATRIX];
			if(pParameterInfo->Type == CEffectInterface::PROGRAM)
			{
				m_pEffect->SetMatrix(CEffectInterface::VERTEX_SHADER  , pParameterInfo->uAlphaIndex, InverceWorldMatrix);
				m_pEffect->SetMatrix(CEffectInterface::FRAGMENT_SHADER, pParameterInfo->uBetaIndex , InverceWorldMatrix);
			}
			else
				m_pEffect->SetMatrix(pParameterInfo->Type, pParameterInfo->uAlphaIndex, InverceWorldMatrix);
		}
	}

	pParameterList = &m_pParameterList[m_uPass];
	if(pParameterList->uCameraParameterFlag)
	{
		if( ALCHEMY_TEST_BIT(pParameterList->uCameraParameterFlag, CAMERA_POSITION) )
		{
			pParameterInfo = &pParameterList->CameraParameters[CAMERA_POSITION];

			if(pParameterInfo->Type == CEffectInterface::PROGRAM)
			{
				m_pEffect->SetParameter(CEffectInterface::VERTEX_SHADER  , pParameterInfo->uAlphaIndex, Camera.GetWorldMatrix().fPosition[3], 4);
				m_pEffect->SetParameter(CEffectInterface::FRAGMENT_SHADER, pParameterInfo->uBetaIndex , Camera.GetWorldMatrix().fPosition[3], 4);
			}
			else
				m_pEffect->SetParameter(pParameterInfo->Type, pParameterInfo->uAlphaIndex, Camera.GetWorldMatrix().fPosition[3], 4);
		}

		if( ALCHEMY_TEST_BIT(pParameterList->uCameraParameterFlag, CAMERA_DIRECTION) )
		{
			pParameterInfo = &pParameterList->CameraParameters[CAMERA_DIRECTION];

			Direction = Camera.GetWorldMatrix().Row[2].Float3;

			Direction.Normalize();

			if(pParameterInfo->Type == CEffectInterface::PROGRAM)
			{
				m_pEffect->SetParameter(CEffectInterface::VERTEX_SHADER  , pParameterInfo->uAlphaIndex, static_cast<const FLOAT*>(Direction), 3);
				m_pEffect->SetParameter(CEffectInterface::FRAGMENT_SHADER, pParameterInfo->uBetaIndex , static_cast<const FLOAT*>(Direction), 3);
			}
			else
				m_pEffect->SetParameter(pParameterInfo->Type, pParameterInfo->uAlphaIndex, static_cast<const FLOAT*>(Direction), 3);
		}

		if( ALCHEMY_TEST_BIT(pParameterList->uCameraParameterFlag, CAMERA_NEGATE_DIRECTION) )
		{
			pParameterInfo = &pParameterList->CameraParameters[CAMERA_NEGATE_DIRECTION];

			Direction = Camera.GetWorldMatrix().Row[2].Float3;

			Direction.Negate();

			Direction.Normalize();

			if(pParameterInfo->Type == CEffectInterface::PROGRAM)
			{
				m_pEffect->SetParameter(CEffectInterface::VERTEX_SHADER  , pParameterInfo->uAlphaIndex, static_cast<const FLOAT*>(Direction), 3);
				m_pEffect->SetParameter(CEffectInterface::FRAGMENT_SHADER, pParameterInfo->uBetaIndex , static_cast<const FLOAT*>(Direction), 3);
			}
			else
				m_pEffect->SetParameter(pParameterInfo->Type, pParameterInfo->uAlphaIndex, static_cast<const FLOAT*>(Direction), 3);
		}

		if( ALCHEMY_TEST_BIT(pParameterList->uCameraParameterFlag, CAMERA_NEAR) )
		{
			pParameterInfo = &pParameterList->CameraParameters[CAMERA_NEAR];

			FLOAT fNear = static_cast<FLOAT>( Camera.GetNearPlane() );

			if(pParameterInfo->Type == CEffectInterface::PROGRAM)
			{
				m_pEffect->SetParameter(CEffectInterface::VERTEX_SHADER  , pParameterInfo->uAlphaIndex, &fNear, 1);
				m_pEffect->SetParameter(CEffectInterface::FRAGMENT_SHADER, pParameterInfo->uBetaIndex , &fNear, 1);
			}
			else
				m_pEffect->SetParameter(pParameterInfo->Type, pParameterInfo->uAlphaIndex, &fNear, 1);
		}

		if( ALCHEMY_TEST_BIT(pParameterList->uCameraParameterFlag, CAMERA_FAR) )
		{
			pParameterInfo = &pParameterList->CameraParameters[CAMERA_FAR];

			FLOAT fFar = static_cast<FLOAT>( Camera.GetFarPlane() );

			if(pParameterInfo->Type == CEffectInterface::PROGRAM)
			{
				m_pEffect->SetParameter(CEffectInterface::VERTEX_SHADER  , pParameterInfo->uAlphaIndex, &fFar, 1);
				m_pEffect->SetParameter(CEffectInterface::FRAGMENT_SHADER, pParameterInfo->uBetaIndex , &fFar, 1);
			}
			else
				m_pEffect->SetParameter(pParameterInfo->Type, pParameterInfo->uAlphaIndex, &fFar, 1);
		}

		if( ALCHEMY_TEST_BIT(pParameterList->uCameraParameterFlag, CAMERA_DISTANCE) )
		{
			pParameterInfo = &pParameterList->CameraParameters[CAMERA_DISTANCE];

			FLOAT fDistance = static_cast<FLOAT>( Camera.GetFarPlane() - Camera.GetNearPlane() );

			if(pParameterInfo->Type == CEffectInterface::PROGRAM)
			{
				m_pEffect->SetParameter(CEffectInterface::VERTEX_SHADER  , pParameterInfo->uAlphaIndex, &fDistance, 1);
				m_pEffect->SetParameter(CEffectInterface::FRAGMENT_SHADER, pParameterInfo->uBetaIndex , &fDistance, 1);
			}
			else
				m_pEffect->SetParameter(pParameterInfo->Type, pParameterInfo->uAlphaIndex, &fDistance, 1);
		}

		if( ALCHEMY_TEST_BIT(pParameterList->uCameraParameterFlag, CAMERA_LOCAL_POSITION) )
		{
			if(!bIsIverceWorldMatrix)
			{
				InverceWorldMatrix   = WorldMatrix;
				bIsIverceWorldMatrix = InverceWorldMatrix.Invert3x4();
			}

			pParameterInfo = &pParameterList->CameraParameters[CAMERA_LOCAL_POSITION];

			Position = Camera.GetWorldMatrix().Row[3];
			Position.Transform(InverceWorldMatrix);

			if(pParameterInfo->Type == CEffectInterface::PROGRAM)
			{
				m_pEffect->SetParameter(CEffectInterface::VERTEX_SHADER  , pParameterInfo->uAlphaIndex, static_cast<const FLOAT*>(Position), 4);
				m_pEffect->SetParameter(CEffectInterface::FRAGMENT_SHADER, pParameterInfo->uBetaIndex , static_cast<const FLOAT*>(Position), 4);
			}
			else
				m_pEffect->SetParameter(pParameterInfo->Type, pParameterInfo->uAlphaIndex, static_cast<const FLOAT*>(Position), 4);
		}

		if( ALCHEMY_TEST_BIT(pParameterList->uCameraParameterFlag, CAMERA_LOCAL_DIRECTION) )
		{
			if(!bIsIverceWorldMatrix)
			{
				InverceWorldMatrix   = WorldMatrix;
				bIsIverceWorldMatrix = InverceWorldMatrix.Invert3x4();
			}

			pParameterInfo = &pParameterList->CameraParameters[CAMERA_LOCAL_DIRECTION];

			Direction = Camera.GetWorldMatrix().Row[2].Float3;

			Direction.TransformNormal(InverceWorldMatrix);
			Direction.Normalize();

			if(pParameterInfo->Type == CEffectInterface::PROGRAM)
			{
				m_pEffect->SetParameter(CEffectInterface::VERTEX_SHADER  , pParameterInfo->uAlphaIndex, static_cast<const FLOAT*>(Direction), 3);
				m_pEffect->SetParameter(CEffectInterface::FRAGMENT_SHADER, pParameterInfo->uBetaIndex , static_cast<const FLOAT*>(Direction), 3);
			}
			else
				m_pEffect->SetParameter(pParameterInfo->Type, pParameterInfo->uAlphaIndex, static_cast<const FLOAT*>(Direction), 3);
		}

		if( ALCHEMY_TEST_BIT(pParameterList->uCameraParameterFlag, CAMERA_LOCAL_DIRECTION_NEGATE) )
		{
			if(!bIsIverceWorldMatrix)
			{
				InverceWorldMatrix   = WorldMatrix;
				bIsIverceWorldMatrix = InverceWorldMatrix.Invert3x4();
			}

			pParameterInfo = &pParameterList->CameraParameters[CAMERA_LOCAL_DIRECTION_NEGATE];

			Direction = Camera.GetWorldMatrix().Row[2].Float3;

			Direction.TransformNormal(InverceWorldMatrix);
			Direction.Normalize();

			Direction.Negate();

			if(pParameterInfo->Type == CEffectInterface::PROGRAM)
			{
				m_pEffect->SetParameter(CEffectInterface::VERTEX_SHADER  , pParameterInfo->uAlphaIndex, static_cast<const FLOAT*>(Direction), 3);
				m_pEffect->SetParameter(CEffectInterface::FRAGMENT_SHADER, pParameterInfo->uBetaIndex , static_cast<const FLOAT*>(Direction), 3);
			}
			else
				m_pEffect->SetParameter(pParameterInfo->Type, pParameterInfo->uAlphaIndex, static_cast<const FLOAT*>(Direction), 3);
		}
	}

	pParameterList = &m_pParameterList[m_uPass];
	if(pParameterList->uMaterialParameterFlag)
	{
		if( ALCHEMY_TEST_BIT(pParameterList->uMaterialParameterFlag, MATERIAL_AMBIENT) )
		{
			pParameterInfo = &pParameterList->MaterialParameters[MATERIAL_AMBIENT];

			if(pParameterInfo->Type == CEffectInterface::PROGRAM)
			{
				m_pEffect->SetParameter(CEffectInterface::VERTEX_SHADER  , pParameterInfo->uAlphaIndex, static_cast<const FLOAT*>( Material.GetAmbient() ), 4);
				m_pEffect->SetParameter(CEffectInterface::FRAGMENT_SHADER, pParameterInfo->uBetaIndex , static_cast<const FLOAT*>( Material.GetAmbient() ), 4);
			}
			else
				m_pEffect->SetParameter(pParameterInfo->Type, pParameterInfo->uAlphaIndex, static_cast<const FLOAT*>( Material.GetAmbient() ), 4);
		}

		if( ALCHEMY_TEST_BIT(pParameterList->uMaterialParameterFlag, MATERIAL_DIFFUSE) )
		{
			pParameterInfo = &pParameterList->MaterialParameters[MATERIAL_DIFFUSE];

			if(pParameterInfo->Type == CEffectInterface::PROGRAM)
			{
				m_pEffect->SetParameter(CEffectInterface::VERTEX_SHADER  , pParameterInfo->uAlphaIndex, static_cast<const FLOAT*>( Material.GetDiffuse() ), 4);
				m_pEffect->SetParameter(CEffectInterface::FRAGMENT_SHADER, pParameterInfo->uBetaIndex , static_cast<const FLOAT*>( Material.GetDiffuse() ), 4);
			}
			else
				m_pEffect->SetParameter(pParameterInfo->Type, pParameterInfo->uAlphaIndex, static_cast<const FLOAT*>( Material.GetDiffuse() ), 4);
		}

		if( ALCHEMY_TEST_BIT(pParameterList->uMaterialParameterFlag, MATERIAL_SPECULAR) )
		{
			pParameterInfo = &pParameterList->MaterialParameters[MATERIAL_SPECULAR];

			if(pParameterInfo->Type == CEffectInterface::PROGRAM)
			{
				m_pEffect->SetParameter(CEffectInterface::VERTEX_SHADER  , pParameterInfo->uAlphaIndex, static_cast<const FLOAT*>( Material.GetSpecular() ), 4);
				m_pEffect->SetParameter(CEffectInterface::FRAGMENT_SHADER, pParameterInfo->uBetaIndex , static_cast<const FLOAT*>( Material.GetSpecular() ), 4);
			}
			else
				m_pEffect->SetParameter(pParameterInfo->Type, pParameterInfo->uAlphaIndex, static_cast<const FLOAT*>( Material.GetSpecular() ), 4);
		}

		if( ALCHEMY_TEST_BIT(pParameterList->uMaterialParameterFlag, MATERIAL_EMISSIVE) )
		{
			pParameterInfo = &pParameterList->MaterialParameters[MATERIAL_EMISSIVE];

			if(pParameterInfo->Type == CEffectInterface::PROGRAM)
			{
				m_pEffect->SetParameter(CEffectInterface::VERTEX_SHADER  , pParameterInfo->uAlphaIndex, static_cast<const FLOAT*>( Material.GetEmissive() ), 4);
				m_pEffect->SetParameter(CEffectInterface::FRAGMENT_SHADER, pParameterInfo->uBetaIndex , static_cast<const FLOAT*>( Material.GetEmissive() ), 4);
			}
			else
				m_pEffect->SetParameter(pParameterInfo->Type, pParameterInfo->uAlphaIndex, static_cast<const FLOAT*>( Material.GetEmissive() ), 4);
		}

		if( ALCHEMY_TEST_BIT(pParameterList->uMaterialParameterFlag, MATERIAL_POWER) )
		{
			pParameterInfo = &pParameterList->MaterialParameters[MATERIAL_POWER];

			FLOAT fPower = Material.GetPower();

			if(pParameterInfo->Type == CEffectInterface::PROGRAM)
			{
				m_pEffect->SetParameter(CEffectInterface::VERTEX_SHADER  , pParameterInfo->uAlphaIndex, &fPower, 1);
				m_pEffect->SetParameter(CEffectInterface::FRAGMENT_SHADER, pParameterInfo->uBetaIndex , &fPower, 1);
			}
			else
				m_pEffect->SetParameter(pParameterInfo->Type, pParameterInfo->uAlphaIndex, &fPower, 1);
		}
	}

	if(pParameterList->pTextures)
	{
		UINT uTextureCount = pParameterList->uTextureCount;
		if(uTextureCount)
		{
			const LPTEXTURE* ppTexture;
			for(UINT j = 0; j < uTextureCount; ++ j)
			{
				pParameterInfo = &pParameterList->pTextures[j];
				ppTexture = Material.GetTextures().Get(pParameterInfo->uAlphaIndex);
				if(ppTexture && *ppTexture)
					m_pEffect->SetTexture( pParameterInfo->Type, pParameterInfo->uAlphaIndex, (*ppTexture)->GetResourceHandle() );
			}
		}
	}

	CSceneManager& SceneManager = static_cast<CSceneManager&>( CGraphicsResource::GetGraphicsResourcePool() );

	UINT uLightCount = SceneManager.GetLightCount();

	CLight* pLight;
	UINT uFlag;
	LPPARAMETERINFO pCurrentParameterInfo;
	for(UINT i = 0; i < uLightCount && i < pParameterList->uLightCount; ++ i)
	{
		pLight = SceneManager.GetLight(i);
		if(pLight)
		{
			uFlag          = pParameterList->puLightParameterFlag[i];
			pParameterInfo = pParameterList->pLightParameters[i];

			if( ALCHEMY_TEST_BIT(uFlag, LIGHT_LOCAL_POSITION) )
			{
				if(!bIsIverceWorldMatrix)
				{
					InverceWorldMatrix   = WorldMatrix;
					bIsIverceWorldMatrix = InverceWorldMatrix.Invert3x4();
				}

				Position = pLight->GetPosition();

				Position.Transform(InverceWorldMatrix);

				pCurrentParameterInfo = &pParameterInfo[LIGHT_LOCAL_POSITION];

				if(pParameterInfo->Type == CEffectInterface::PROGRAM)
				{
					m_pEffect->SetParameter(CEffectInterface::VERTEX_SHADER  , pCurrentParameterInfo->uAlphaIndex, static_cast<const FLOAT*>(Position), 4);
					m_pEffect->SetParameter(CEffectInterface::FRAGMENT_SHADER, pCurrentParameterInfo->uBetaIndex , static_cast<const FLOAT*>(Position), 4);
				}
				else
					m_pEffect->SetParameter(pCurrentParameterInfo->Type, pCurrentParameterInfo->uAlphaIndex, static_cast<const FLOAT*>(Position), 4);
			}

			if( ALCHEMY_TEST_BIT(uFlag, LIGHT_LOCAL_DIRECTION) )
			{
				if(!bIsIverceWorldMatrix)
				{
					InverceWorldMatrix   = WorldMatrix;
					bIsIverceWorldMatrix = InverceWorldMatrix.Invert3x4();
				}

				Direction = pLight->GetDirection();
				Direction.TransformNormal(InverceWorldMatrix);
				Direction.Normalize();

				pCurrentParameterInfo = &pParameterInfo[LIGHT_LOCAL_DIRECTION];

				if(pParameterInfo->Type == CEffectInterface::PROGRAM)
				{
					m_pEffect->SetParameter(CEffectInterface::VERTEX_SHADER  , pCurrentParameterInfo->uAlphaIndex, static_cast<const FLOAT*>(Direction), 3);
					m_pEffect->SetParameter(CEffectInterface::FRAGMENT_SHADER, pCurrentParameterInfo->uBetaIndex , static_cast<const FLOAT*>(Direction), 3);
				}
				else
					m_pEffect->SetParameter(pCurrentParameterInfo->Type, pCurrentParameterInfo->uAlphaIndex, static_cast<const FLOAT*>(Direction), 3);
			}

			if( ALCHEMY_TEST_BIT(uFlag, LIGHT_WORLD_VIEW_PROJECT_MATRIX) )
			{
				const CCamera* pCamera = pLight->GetCamera();
				if(pCamera)
				{
					pCurrentParameterInfo = &pParameterInfo[LIGHT_WORLD_VIEW_PROJECT_MATRIX];

					Matrix = WorldMatrix;
					Matrix *= pCamera->GetViewProjectionMatrix();

					if(pParameterInfo->Type == CEffectInterface::PROGRAM)
					{
						m_pEffect->SetMatrix(CEffectInterface::VERTEX_SHADER  , pCurrentParameterInfo->uAlphaIndex, Matrix);
						m_pEffect->SetMatrix(CEffectInterface::FRAGMENT_SHADER, pCurrentParameterInfo->uBetaIndex , Matrix);
					}
					else
						m_pEffect->SetMatrix(pCurrentParameterInfo->Type, pCurrentParameterInfo->uAlphaIndex, Matrix);
				}
			}
		}
	}

	return true;
}

const CEffectCode::ELEMENTPARAMETER* CRenderMethod::GetVertexElementParameters(UINT uIndex, PUINT puOutCount)const
{
	if(!m_pParameterList || !m_pEffect)
	{
		ALCHEMY_DEBUG_WARNING("CRenderMethod::GetVertexElementParameters: Render method has not been created.");

		return ALCHEMY_NULL;
	}

	if( uIndex >= m_pEffect->GetTotalPass() )
	{
		ALCHEMY_DEBUG_WARNING("CRenderMethod::GetVertexElementParameters: Index out of range.");

		return ALCHEMY_NULL;
	}

	PARAMETERLIST& ParameterList = m_pParameterList[uIndex];

	if(puOutCount)
		*puOutCount = ParameterList.uVertexElementParameterCount;

	return ParameterList.pVertexElementParameters;
}