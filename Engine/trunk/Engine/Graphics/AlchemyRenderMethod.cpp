#include "AlchemyRenderMethod.h"
#include "AlchemySceneManager.h"
#include "AlchemyMaterial.h"

#include "../Core/AlchemyUtils.h"
#include "../Math/AlchemyVector3.h"
#include "../Math/AlchemyVector4.h"

using namespace alchemy;

CVector3 g_Direction;
CVector4 g_Position;
CMatrix4x4 g_Matrix;

CRenderMethod::CRenderStateHandler::CRenderStateHandler() :
m_pRenderStateInfos(ALCHEMY_NULL), 
m_uRenderStateInfoCount(0)
{
}

CRenderMethod::CRenderStateHandler::~CRenderStateHandler()
{
	ALCHEMY_DEBUG_DELETE_ARRAY(m_pRenderStateInfos);
}
			
bool CRenderMethod::CRenderStateHandler::Create(const IRenderState::RENDERSTATEINFO* pRenderStateInfos, UINT uCount)
{
	ALCHEMY_DEBUG_DELETE_ARRAY(m_pRenderStateInfos);
	
	ALCHEMY_DEBUG_NEW(m_pRenderStateInfos, IRenderState::RENDERSTATEINFO[uCount]);
	
	for(UINT i = 0 ; i < uCount; ++ i)
		m_pRenderStateInfos[i] = pRenderStateInfos[i];
		
	m_uRenderStateInfoCount = uCount;
	
	return true;
}
			
bool CRenderMethod::CRenderStateHandler::SetRenderState(IRenderState& RenderState)
{
	return RenderState.SetRenderState(m_pRenderStateInfos, m_uRenderStateInfoCount, false);
}

CRenderMethod::CRenderMethod(void) :
m_pParameterList(ALCHEMY_NULL),
m_pParameterListBuffer(ALCHEMY_NULL),
m_puRenderTargetIndexCount(ALCHEMY_NULL),
m_puRenderTargetIndexOffset(ALCHEMY_NULL),
m_puTextureCount(ALCHEMY_NULL),
m_uParameterListIndex(0),
m_uPassCount(0),
m_pShaderParametersHandler(ALCHEMY_NULL),
m_pRenderStateHandler(ALCHEMY_NULL), 
m_ppTextureAlphaIndexes(ALCHEMY_NULL),
m_bIsInverceWorldMatrix(true),
m_bIsTransposeWorldMatrix(true), 
m_bIsActivate(true)
{
	m_WorldMatrix.Identity();
	m_InverceWorldMatrix.Identity();
	m_TransposeWorldMatrix.Identity();
}

CRenderMethod::~CRenderMethod(void)
{
	if(!m_pParameterListBuffer)
		DestroyParameterList( m_pParameterList, GetParameterListCount() );

	ALCHEMY_DEBUG_DELETE_ARRAY(m_puRenderTargetIndexCount);
	ALCHEMY_DEBUG_DELETE_ARRAY(m_puRenderTargetIndexOffset);
	ALCHEMY_DEBUG_DELETE_ARRAY(m_puTextureCount);

	UINT uParameterListCount = 0, i;
	for(i = 0; i < m_uPassCount; ++ i)
		uParameterListCount += m_puRenderTargetIndexCount[i];

	for (UINT i=0; i<uParameterListCount; ++i)
		ALCHEMY_DEBUG_DELETE_ARRAY(m_ppTextureAlphaIndexes[i]);

	ALCHEMY_DEBUG_DELETE_ARRAY(m_ppTextureAlphaIndexes);
}

CBuffer& CRenderMethod::CreateParameterListBuffer(const PARAMETERLIST* pParameterList, UINT uParameterListCount)
{
	UINT uBaseSize = sizeof(PARAMETERLIST) * uParameterListCount, uBufferSize = uBaseSize, i;

	const PARAMETERLIST* pCurrentParameterList;

	for(i = 0; i < uParameterListCount; ++ i)
	{
		pCurrentParameterList = &pParameterList[i];

		uBufferSize += pCurrentParameterList->uCameraCount  * 
		( sizeof(PARAMETERINFO) * TOTAL_CAMERA_PARAMETERS  + sizeof(UINT) );
		uBufferSize += pCurrentParameterList->uLightCount   * 
		( sizeof(PARAMETERINFO) * TOTAL_LIGHT_PARAMETERS   + sizeof(UINT) );
		uBufferSize += pCurrentParameterList->uTextureCount * 
		( sizeof(PARAMETERINFO) * TOTAL_TEXTURE_PARAMETERS + sizeof(UINT) + sizeof(PARAMETERINFO) + 
		(pCurrentParameterList->pSamplerDeclaration ? sizeof(IEffect::SAMPLERDECLARATION) : 0) );

		//uBufferSize += pCurrentParameterList->uVertexElementCount * sizeof(CEffectCode::ELEMENTDECLARATION);
	}

	CBuffer& Buffer = CreateBuffer(uBufferSize);
	PUINT8 puParameterList = reinterpret_cast<PUINT8>( Buffer.GetBufferPointer() );
	LPPARAMETERLIST pDestinationParameterList = reinterpret_cast<LPPARAMETERLIST>(puParameterList);

	UINT uOffset = uBaseSize, j, k;
	for(i = 0; i < uParameterListCount; ++ i)
	{
		pCurrentParameterList = &pParameterList[i];

		*pDestinationParameterList = *pCurrentParameterList;

		pDestinationParameterList->pCameraParameters = reinterpret_cast<PARAMETERINFO(*)[TOTAL_CAMERA_PARAMETERS]>(puParameterList + uOffset);

		uOffset += pCurrentParameterList->uCameraCount * sizeof(PARAMETERINFO) * TOTAL_CAMERA_PARAMETERS;

		pDestinationParameterList->puCameraParameterFlag = reinterpret_cast<PUINT>(puParameterList + uOffset);

		uOffset += pCurrentParameterList->uCameraCount * sizeof(UINT);

		for(j = 0; j < pCurrentParameterList->uCameraCount; ++ j)
		{
			for(k = 0; k < TOTAL_CAMERA_PARAMETERS; ++ k)
				pDestinationParameterList->pCameraParameters[j][k] = pCurrentParameterList->pCameraParameters[j][k];

			pDestinationParameterList->puCameraParameterFlag[j]    = pCurrentParameterList->puCameraParameterFlag[j];
		}

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
		
		if(pCurrentParameterList->pSamplerDeclaration)
		{
			pDestinationParameterList->pSamplerDeclaration = reinterpret_cast<IEffect::LPSAMPLERDECLARATION>(puParameterList + uOffset);

			uOffset += pCurrentParameterList->uTextureCount * sizeof(IEffect::SAMPLERDECLARATION);
			
			for(j = 0; j < pCurrentParameterList->uTextureCount; ++ j)
				pDestinationParameterList->pSamplerDeclaration[j] = pCurrentParameterList->pSamplerDeclaration[j];
		}

		/*pDestinationParameterList->pVertexElements = reinterpret_cast<CEffectCode::LPELEMENTDECLARATION>(puParameterList + uOffset);

		uOffset += pCurrentParameterList->uVertexElementCount * sizeof(CEffectCode::ELEMENTDECLARATION);

		for(j = 0; j < pCurrentParameterList->uVertexElementCount; ++ j)
			pDestinationParameterList->pVertexElements[j] = pCurrentParameterList->pVertexElements[j];*/

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

		ALCHEMY_DEBUG_DELETE_ARRAY(pCurrentParameterList->pCameraParameters);

		ALCHEMY_DEBUG_DELETE_ARRAY(pCurrentParameterList->pLightParameters);

		ALCHEMY_DEBUG_DELETE_ARRAY(pCurrentParameterList->pTextureParameters);

		ALCHEMY_DEBUG_DELETE_ARRAY(pCurrentParameterList->pTextures);
		
		ALCHEMY_DEBUG_DELETE_ARRAY(pCurrentParameterList->pSamplerDeclaration);

		ALCHEMY_DEBUG_DELETE_ARRAY(pCurrentParameterList->puCameraParameterFlag);

		ALCHEMY_DEBUG_DELETE_ARRAY(pCurrentParameterList->puLightParameterFlag);

		ALCHEMY_DEBUG_DELETE_ARRAY(pCurrentParameterList->puTextureParameterFlag);

		//ALCHEMY_DEBUG_DELETE_ARRAY(pCurrentParameterList->pVertexElements);
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
		DestroyParameterList( m_pParameterList, GetParameterListCount() );

		m_pParameterList = ALCHEMY_NULL;
	}

	if(m_pShaderParametersHandler)
	{
		m_pShaderParametersHandler->Release();

		m_pShaderParametersHandler = ALCHEMY_NULL;
	}

	UINT uParameterListCount = GetParameterListCount();
	for (UINT i = 0; i < uParameterListCount; ++ i)
		ALCHEMY_DEBUG_DELETE_ARRAY(m_ppTextureAlphaIndexes[i]);

	ALCHEMY_DEBUG_DELETE_ARRAY(m_puRenderTargetIndexCount);
	ALCHEMY_DEBUG_DELETE_ARRAY(m_puRenderTargetIndexOffset);
	ALCHEMY_DEBUG_DELETE_ARRAY(m_puTextureCount);

	ALCHEMY_DEBUG_DELETE_ARRAY(m_ppTextureAlphaIndexes);
}

const UINT* CRenderMethod::GetTextureInfo(UINT uPass, UINT uRenderTargetIndex, UINT& uCount)
{
	if(m_ppTextureAlphaIndexes && m_puRenderTargetIndexOffset && m_puRenderTargetIndexCount && uPass < m_uPassCount && m_puRenderTargetIndexCount[uPass] > uRenderTargetIndex)
	{
		uCount = m_puTextureCount[ m_puRenderTargetIndexOffset[uPass] + uRenderTargetIndex ];
		return m_ppTextureAlphaIndexes[ m_puRenderTargetIndexOffset[uPass] + uRenderTargetIndex ];
	}

	uCount = 0;
	return ALCHEMY_NULL;
}

bool CRenderMethod::SetParameter(PARAMETER Index, const FLOAT* pfData)
{
	if(!m_pParameterList)
	{
		ALCHEMY_DEBUG_WARNING("CRenderMethod::SetParameter: Render method has not been created.");

		return false;
	}

	const PARAMETERLIST* pParameterList = &m_pParameterList[m_uParameterListIndex];
	if(!pParameterList->uParameterFlag)
		return true;

	const static UINT s_uSIZE[IRenderMethod::TOTAL_PARAMETERS] = {3, 2, 1, 1, 4, 16, 16};

	UINT uSize = s_uSIZE[Index];

	if( ALCHEMY_TEST_BIT(pParameterList->uParameterFlag, Index) )
	{
		CRenderSystem& RenderSystem = static_cast<CSceneManager&>( GetGraphicsResourcePool() ).GetRenderSystem();

		const PARAMETERINFO* pParameterInfo = &pParameterList->Parameters[Index];

		if(pParameterInfo->Type == IEffect::PROGRAM)
		{
			RenderSystem.SetShaderParameters(IEffect::VERTEX_SHADER  , pParameterInfo->uAlphaIndex, pfData, uSize);
			RenderSystem.SetShaderParameters(IEffect::FRAGMENT_SHADER, pParameterInfo->uBetaIndex , pfData, uSize);
		}
		else
			RenderSystem.SetShaderParameters(pParameterInfo->Type, pParameterInfo->uAlphaIndex, pfData, uSize);
	}

	if(m_pShaderParametersHandler)
	{
		uSize *= sizeof(FLOAT);
		pParameterList = m_pShaderParametersHandler->GetParameterlist(m_uParameterListIndex);
		if(pParameterList)
		{
			if( ALCHEMY_TEST_BIT(pParameterList->uParameterFlag, Index) )
			{
				const PARAMETERINFO* pParameterInfo = &pParameterList->Parameters[Index];

				if(pParameterInfo->Type == IEffect::PROGRAM)
				{
					m_pShaderParametersHandler->SetParameters(pfData, pParameterInfo->uAlphaIndex, uSize);
					m_pShaderParametersHandler->SetParameters(pfData, pParameterInfo->uBetaIndex , uSize);
				}
				else
					m_pShaderParametersHandler->SetParameters(pfData, pParameterInfo->uAlphaIndex, uSize);
			}
		}
	}

	return true;
}


bool CRenderMethod::SetArray(IRenderMethod::ARRAYPARAMETER Index, const void* pData, UINT uRow, UINT uColumn, UINT uRowOffset, UINT uOffset, UINT uCount, bool bIsPointer)
{
	if(!m_pParameterList)
	{
		ALCHEMY_DEBUG_WARNING("CRenderMethod::SetParameter: Render method has not been created.");

		return false;
	}

	if(!pData || !uCount)
		return false;

	const PARAMETERLIST* pParameterList = &m_pParameterList[m_uParameterListIndex];
	if(!pParameterList->uArrayParameterFlag)
		return true;

	CRenderSystem& RenderSystem = static_cast<CSceneManager&>( GetGraphicsResourcePool() ).GetRenderSystem();
	const PARAMETERINFO* pParameterInfo;

	if( ALCHEMY_TEST_BIT(pParameterList->uArrayParameterFlag, BONE_MATRIX_ARRAY) )
	{
		pParameterInfo = &pParameterList->ArrayParameters[BONE_MATRIX_ARRAY];

		if(pParameterInfo->Type == IEffect::PROGRAM)
		{
			if(bIsPointer)
			{
				RenderSystem.SetShaderMatrixPointerArray(IEffect::VERTEX_SHADER  , pParameterInfo->uAlphaIndex, reinterpret_cast<const PFLOAT*>(pData), uRow, uColumn, uRowOffset, uCount);
				RenderSystem.SetShaderMatrixPointerArray(IEffect::FRAGMENT_SHADER, pParameterInfo->uBetaIndex , reinterpret_cast<const PFLOAT*>(pData), uRow, uColumn, uRowOffset, uCount);
			}
			else
			{
				RenderSystem.SetShaderMatrixArray(IEffect::VERTEX_SHADER, pParameterInfo->uAlphaIndex, reinterpret_cast<const FLOAT*>(pData), uRow, uColumn, uRowOffset, uOffset, uCount);
				RenderSystem.SetShaderMatrixArray(IEffect::FRAGMENT_SHADER, pParameterInfo->uBetaIndex , reinterpret_cast<const FLOAT*>(pData), uRow, uColumn, uRowOffset, uOffset, uCount);
			}
		}
		else
		{
			if(bIsPointer)
				RenderSystem.SetShaderMatrixPointerArray(pParameterInfo->Type, pParameterInfo->uAlphaIndex, reinterpret_cast<const PFLOAT*>(pData), uRow, uColumn, uRowOffset, uCount);
			else
				RenderSystem.SetShaderMatrixArray(pParameterInfo->Type, pParameterInfo->uAlphaIndex, reinterpret_cast<const FLOAT*>(pData), uRow, uColumn, uRowOffset, uOffset, uCount);
		}
	}

	if( ALCHEMY_TEST_BIT(pParameterList->uArrayParameterFlag, INSTANCE_TRANSFORM_ARRAY) )
	{
		pParameterInfo = &pParameterList->ArrayParameters[INSTANCE_TRANSFORM_ARRAY];

		if(pParameterInfo->Type == IEffect::PROGRAM)
		{
			if(bIsPointer)
			{
				RenderSystem.SetShaderMatrixPointerArray(IEffect::VERTEX_SHADER  , pParameterInfo->uAlphaIndex, reinterpret_cast<const PFLOAT*>(pData), uRow, uColumn, uRowOffset, uCount);
				RenderSystem.SetShaderMatrixPointerArray(IEffect::FRAGMENT_SHADER, pParameterInfo->uBetaIndex , reinterpret_cast<const PFLOAT*>(pData), uRow, uColumn, uRowOffset, uCount);
			}
			else
			{
				RenderSystem.SetShaderMatrixArray(IEffect::VERTEX_SHADER, pParameterInfo->uAlphaIndex, reinterpret_cast<const FLOAT*>(pData), uRow, uColumn, uRowOffset, uOffset, uCount);
				RenderSystem.SetShaderMatrixArray(IEffect::FRAGMENT_SHADER, pParameterInfo->uBetaIndex , reinterpret_cast<const FLOAT*>(pData), uRow, uColumn, uRowOffset, uOffset, uCount);
			}
		}
		else
		{
			if(bIsPointer)
				RenderSystem.SetShaderMatrixPointerArray(pParameterInfo->Type, pParameterInfo->uAlphaIndex, reinterpret_cast<const PFLOAT*>(pData), uRow, uColumn, uRowOffset, uCount);
			else
				RenderSystem.SetShaderMatrixArray(pParameterInfo->Type, pParameterInfo->uAlphaIndex, reinterpret_cast<const FLOAT*>(pData), uRow, uColumn, uRowOffset, uOffset, uCount);
		}
	}

	if(m_pShaderParametersHandler)
	{
		pParameterList = m_pShaderParametersHandler->GetParameterlist(m_uParameterListIndex);
		if(pParameterList)
		{
			if( ALCHEMY_TEST_BIT(pParameterList->uArrayParameterFlag, BONE_MATRIX_ARRAY) )
			{
				pParameterInfo = &pParameterList->ArrayParameters[BONE_MATRIX_ARRAY];

				if(pParameterInfo->Type == IEffect::PROGRAM)
				{
					if(bIsPointer)
					{
						m_pShaderParametersHandler->SetParameters(reinterpret_cast<const PFLOAT*>(pData), pParameterInfo->uAlphaIndex, uRow, uColumn, uRowOffset, uCount);
						m_pShaderParametersHandler->SetParameters(reinterpret_cast<const PFLOAT*>(pData), pParameterInfo->uBetaIndex , uRow, uColumn, uRowOffset, uCount);
					}
					else
					{
						m_pShaderParametersHandler->SetParameters(reinterpret_cast<const FLOAT*>(pData), pParameterInfo->uAlphaIndex, uRow, uColumn, uRowOffset, uOffset, uCount);
						m_pShaderParametersHandler->SetParameters(reinterpret_cast<const FLOAT*>(pData), pParameterInfo->uBetaIndex , uRow, uColumn, uRowOffset, uOffset, uCount);
					}
				}
				else
				{
					if(bIsPointer)
						m_pShaderParametersHandler->SetParameters(reinterpret_cast<const PFLOAT*>(pData), pParameterInfo->uAlphaIndex, uRow, uColumn, uRowOffset, uCount);
					else
						m_pShaderParametersHandler->SetParameters(reinterpret_cast<const FLOAT*>(pData), pParameterInfo->uAlphaIndex, uRow, uColumn, uRowOffset, uOffset, uCount);
				}
			}

			if( ALCHEMY_TEST_BIT(pParameterList->uArrayParameterFlag, INSTANCE_TRANSFORM_ARRAY) )
			{
				pParameterInfo = &pParameterList->ArrayParameters[INSTANCE_TRANSFORM_ARRAY];

				if(pParameterInfo->Type == IEffect::PROGRAM)
				{
					if(bIsPointer)
					{
						m_pShaderParametersHandler->SetParameters(reinterpret_cast<const PFLOAT*>(pData), pParameterInfo->uAlphaIndex, uRow, uColumn, uRowOffset, uCount);
						m_pShaderParametersHandler->SetParameters(reinterpret_cast<const PFLOAT*>(pData), pParameterInfo->uBetaIndex , uRow, uColumn, uRowOffset, uCount);
					}
					else
					{
						m_pShaderParametersHandler->SetParameters(reinterpret_cast<const FLOAT*>(pData), pParameterInfo->uAlphaIndex, uRow, uColumn, uRowOffset, uOffset, uCount);
						m_pShaderParametersHandler->SetParameters(reinterpret_cast<const FLOAT*>(pData), pParameterInfo->uBetaIndex , uRow, uColumn, uRowOffset, uOffset, uCount);
					}
				}
				else
				{
					if(bIsPointer)
						m_pShaderParametersHandler->SetParameters(reinterpret_cast<const PFLOAT*>(pData), pParameterInfo->uAlphaIndex, uRow, uColumn, uRowOffset, uCount);
					else
						m_pShaderParametersHandler->SetParameters(reinterpret_cast<const FLOAT*>(pData), pParameterInfo->uAlphaIndex, uRow, uColumn, uRowOffset, uOffset, uCount);
				}
			}
		}
	}

	return true;
}

bool CRenderMethod::SetCamera(const CCamera& Camera, UINT uIndex, bool bIsSetWorldMatrixOnly)
{
	if(!m_pParameterList)
	{
		ALCHEMY_DEBUG_WARNING("Render method has not been created.");

		return false;
	}

	const PARAMETERLIST* pParameterList = &m_pParameterList[m_uParameterListIndex], * pShaderParameterList = m_pShaderParametersHandler ? m_pShaderParametersHandler->GetParameterlist(m_uParameterListIndex) : ALCHEMY_NULL;

	if( uIndex < pParameterList->uCameraCount || (pShaderParameterList && uIndex < pShaderParameterList->uCameraCount) )
	{
		UINT uFlag = uIndex < pParameterList->uCameraCount ? pParameterList->puCameraParameterFlag[uIndex] : 0, uShaderFlag = (pShaderParameterList && uIndex < pShaderParameterList->uCameraCount) ? pShaderParameterList->puCameraParameterFlag[uIndex] : 0;
		if(!uFlag && !uShaderFlag)
			return true;

		const PARAMETERINFO* pParameterInfo = uFlag ? pParameterList->pCameraParameters[uIndex] : static_cast<const PARAMETERINFO*>(ALCHEMY_NULL), *pShaderParameterInfo = uShaderFlag ? pShaderParameterList->pCameraParameters[uIndex] : static_cast<const PARAMETERINFO*>(ALCHEMY_NULL), *pCurrentParameterInfo;
		CRenderSystem& RenderSystem = static_cast<CSceneManager&>( GetGraphicsResourcePool() ).GetRenderSystem();

		if(!bIsSetWorldMatrixOnly)
		{
			if( ALCHEMY_TEST_BIT(uFlag, CAMERA_POSITION) || ALCHEMY_TEST_BIT(uShaderFlag, CAMERA_POSITION) )
			{
				Camera.GetWorldMatrix().GetPosition(g_Position);
				if( ALCHEMY_TEST_BIT(uFlag, CAMERA_POSITION) )
				{
					pCurrentParameterInfo = &pParameterInfo[CAMERA_POSITION];

					if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
					{
						RenderSystem.SetShaderParameters(IEffect::VERTEX_SHADER  , pCurrentParameterInfo->uAlphaIndex, static_cast<const FLOAT*>(g_Position), 4);
						RenderSystem.SetShaderParameters(IEffect::FRAGMENT_SHADER, pCurrentParameterInfo->uBetaIndex , static_cast<const FLOAT*>(g_Position), 4);
					}
					else
						RenderSystem.SetShaderParameters(pCurrentParameterInfo->Type, pCurrentParameterInfo->uAlphaIndex, static_cast<const FLOAT*>(g_Position), 4);
				}

				if( ALCHEMY_TEST_BIT(uShaderFlag, CAMERA_POSITION) )
				{
					pCurrentParameterInfo = &pShaderParameterInfo[CAMERA_POSITION];

					if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
					{
						m_pShaderParametersHandler->SetParameters( static_cast<const FLOAT*>(g_Position), pCurrentParameterInfo->uAlphaIndex, 4 * sizeof(FLOAT) );
						m_pShaderParametersHandler->SetParameters( static_cast<const FLOAT*>(g_Position), pCurrentParameterInfo->uBetaIndex , 4 * sizeof(FLOAT) );
					}
					else
						m_pShaderParametersHandler->SetParameters( static_cast<const FLOAT*>(g_Position), pCurrentParameterInfo->uAlphaIndex, 4 * sizeof(FLOAT) );
				}
			}

			if( ALCHEMY_TEST_BIT(uFlag, CAMERA_DIRECTION) || ALCHEMY_TEST_BIT(uShaderFlag, CAMERA_DIRECTION) || ALCHEMY_TEST_BIT(uFlag, CAMERA_NEGATE_DIRECTION) || ALCHEMY_TEST_BIT(uShaderFlag, CAMERA_NEGATE_DIRECTION) )
			{
				//g_Direction = Camera.GetWorldMatrix().Row[2].Float3;
				Camera.GetWorldMatrix().GetDirection(g_Direction);

				g_Direction.Normalize();

				if( ALCHEMY_TEST_BIT(uFlag, CAMERA_DIRECTION) )
				{
					pCurrentParameterInfo = &pParameterInfo[CAMERA_DIRECTION];

					if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
					{
						RenderSystem.SetShaderParameters(IEffect::VERTEX_SHADER  , pCurrentParameterInfo->uAlphaIndex, static_cast<const FLOAT*>(g_Direction), 3);
						RenderSystem.SetShaderParameters(IEffect::FRAGMENT_SHADER, pCurrentParameterInfo->uBetaIndex , static_cast<const FLOAT*>(g_Direction), 3);
					}
					else
						RenderSystem.SetShaderParameters(pCurrentParameterInfo->Type, pCurrentParameterInfo->uAlphaIndex, static_cast<const FLOAT*>(g_Direction), 3);
				}

				if( ALCHEMY_TEST_BIT(uShaderFlag, CAMERA_DIRECTION) )
				{
					pCurrentParameterInfo = &pShaderParameterInfo[CAMERA_DIRECTION];

					if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
					{
						m_pShaderParametersHandler->SetParameters( static_cast<const FLOAT*>(g_Direction), pCurrentParameterInfo->uAlphaIndex, 3 * sizeof(FLOAT) );
						m_pShaderParametersHandler->SetParameters( static_cast<const FLOAT*>(g_Direction), pCurrentParameterInfo->uBetaIndex , 3 * sizeof(FLOAT) );
					}
					else
						m_pShaderParametersHandler->SetParameters( static_cast<const FLOAT*>(g_Direction), pCurrentParameterInfo->uAlphaIndex, 3 * sizeof(FLOAT) );
				}

				if( ALCHEMY_TEST_BIT(uFlag, CAMERA_NEGATE_DIRECTION) || ALCHEMY_TEST_BIT(uShaderFlag, CAMERA_NEGATE_DIRECTION) )
				{
					g_Direction.Negate();

					if( ALCHEMY_TEST_BIT(uFlag, CAMERA_NEGATE_DIRECTION) )
					{
						pCurrentParameterInfo = &pParameterInfo[CAMERA_NEGATE_DIRECTION];

						if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
						{
							RenderSystem.SetShaderParameters(IEffect::VERTEX_SHADER  , pCurrentParameterInfo->uAlphaIndex, static_cast<const FLOAT*>(g_Direction), 3);
							RenderSystem.SetShaderParameters(IEffect::FRAGMENT_SHADER, pCurrentParameterInfo->uBetaIndex , static_cast<const FLOAT*>(g_Direction), 3);
						}
						else
							RenderSystem.SetShaderParameters(pCurrentParameterInfo->Type, pCurrentParameterInfo->uAlphaIndex, static_cast<const FLOAT*>(g_Direction), 3);
					}

					if( ALCHEMY_TEST_BIT(uShaderFlag, CAMERA_NEGATE_DIRECTION) )
					{
						pCurrentParameterInfo = &pShaderParameterInfo[CAMERA_NEGATE_DIRECTION];

						if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
						{
							m_pShaderParametersHandler->SetParameters( static_cast<const FLOAT*>(g_Direction), pCurrentParameterInfo->uAlphaIndex, 3 * sizeof(FLOAT) );
							m_pShaderParametersHandler->SetParameters( static_cast<const FLOAT*>(g_Direction), pCurrentParameterInfo->uBetaIndex , 3 * sizeof(FLOAT) );
						}
						else
							m_pShaderParametersHandler->SetParameters( static_cast<const FLOAT*>(g_Direction), pCurrentParameterInfo->uAlphaIndex, 3 * sizeof(FLOAT) );
					}
				}
			}

			if( ALCHEMY_TEST_BIT(uFlag, CAMERA_NEAR) || ALCHEMY_TEST_BIT(uShaderFlag, CAMERA_NEAR) )
			{
				FLOAT fNear = static_cast<FLOAT>( Camera.GetNearPlane() );

				if( ALCHEMY_TEST_BIT(uFlag, CAMERA_NEAR) )
				{
					pCurrentParameterInfo = &pParameterInfo[CAMERA_NEAR];

					if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
					{
						RenderSystem.SetShaderParameters(IEffect::VERTEX_SHADER  , pCurrentParameterInfo->uAlphaIndex, &fNear, 1);
						RenderSystem.SetShaderParameters(IEffect::FRAGMENT_SHADER, pCurrentParameterInfo->uBetaIndex , &fNear, 1);
					}
					else
						RenderSystem.SetShaderParameters(pCurrentParameterInfo->Type, pCurrentParameterInfo->uAlphaIndex, &fNear, 1);
				}

				if( ALCHEMY_TEST_BIT(uShaderFlag, CAMERA_NEAR) )
				{
					pCurrentParameterInfo = &pShaderParameterInfo[CAMERA_NEAR];

					if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
					{
						m_pShaderParametersHandler->SetParameters( &fNear, pCurrentParameterInfo->uAlphaIndex, sizeof(FLOAT) );
						m_pShaderParametersHandler->SetParameters( &fNear, pCurrentParameterInfo->uBetaIndex , sizeof(FLOAT) );
					}
					else
						m_pShaderParametersHandler->SetParameters( &fNear, pCurrentParameterInfo->uAlphaIndex, sizeof(FLOAT) );
				}
			}

			if( ALCHEMY_TEST_BIT(uFlag, CAMERA_FAR) || ALCHEMY_TEST_BIT(uShaderFlag, CAMERA_FAR) )
			{
				FLOAT fFar = static_cast<FLOAT>( Camera.GetFarPlane() );

				if( ALCHEMY_TEST_BIT(uFlag, CAMERA_FAR) )
				{
					pCurrentParameterInfo = &pParameterInfo[CAMERA_FAR];

					if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
					{
						RenderSystem.SetShaderParameters(IEffect::VERTEX_SHADER  , pCurrentParameterInfo->uAlphaIndex, &fFar, 1);
						RenderSystem.SetShaderParameters(IEffect::FRAGMENT_SHADER, pCurrentParameterInfo->uBetaIndex , &fFar, 1);
					}
					else
						RenderSystem.SetShaderParameters(pCurrentParameterInfo->Type, pCurrentParameterInfo->uAlphaIndex, &fFar, 1);
				}

				if( ALCHEMY_TEST_BIT(uShaderFlag, CAMERA_FAR) )
				{
					pCurrentParameterInfo = &pShaderParameterInfo[CAMERA_FAR];

					if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
					{
						m_pShaderParametersHandler->SetParameters( &fFar, pCurrentParameterInfo->uAlphaIndex, sizeof(FLOAT) );
						m_pShaderParametersHandler->SetParameters( &fFar, pCurrentParameterInfo->uBetaIndex , sizeof(FLOAT) );
					}
					else
						m_pShaderParametersHandler->SetParameters( &fFar, pCurrentParameterInfo->uAlphaIndex, sizeof(FLOAT) );
				}
			}

			if( ALCHEMY_TEST_BIT(uFlag, CAMERA_DISTANCE) || ALCHEMY_TEST_BIT(uShaderFlag, CAMERA_DISTANCE) )
			{
				FLOAT fDistance = static_cast<FLOAT>( Camera.GetFarPlane() - Camera.GetNearPlane() );

				if( ALCHEMY_TEST_BIT(uFlag, CAMERA_DISTANCE) )
				{
					pCurrentParameterInfo = &pParameterInfo[CAMERA_DISTANCE];

					if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
					{
						RenderSystem.SetShaderParameters(IEffect::VERTEX_SHADER  , pCurrentParameterInfo->uAlphaIndex, &fDistance, 1);
						RenderSystem.SetShaderParameters(IEffect::FRAGMENT_SHADER, pCurrentParameterInfo->uBetaIndex , &fDistance, 1);
					}
					else
						RenderSystem.SetShaderParameters(pCurrentParameterInfo->Type, pCurrentParameterInfo->uAlphaIndex, &fDistance, 1);
				}

				if( ALCHEMY_TEST_BIT(uShaderFlag, CAMERA_DISTANCE) )
				{
					pCurrentParameterInfo = &pShaderParameterInfo[CAMERA_DISTANCE];

					if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
					{
						m_pShaderParametersHandler->SetParameters( &fDistance, pCurrentParameterInfo->uAlphaIndex, sizeof(FLOAT) );
						m_pShaderParametersHandler->SetParameters( &fDistance, pCurrentParameterInfo->uBetaIndex , sizeof(FLOAT) );
					}
					else
						m_pShaderParametersHandler->SetParameters( &fDistance, pCurrentParameterInfo->uAlphaIndex, sizeof(FLOAT) );
				}
			}

			if( ALCHEMY_TEST_BIT(uFlag, CAMERA_VIEW_MATRIX) )
			{
				pCurrentParameterInfo = &pParameterInfo[CAMERA_VIEW_MATRIX];

				if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
				{
					SetShaderMatrix(RenderSystem, IEffect::VERTEX_SHADER  , Camera.GetViewMatrix(), pCurrentParameterInfo->uAlphaIndex);
					SetShaderMatrix(RenderSystem, IEffect::FRAGMENT_SHADER, Camera.GetViewMatrix(), pCurrentParameterInfo->uBetaIndex );
				}
				else
					SetShaderMatrix(RenderSystem, pCurrentParameterInfo->Type, Camera.GetViewMatrix(), pCurrentParameterInfo->uAlphaIndex);
			}

			if( ALCHEMY_TEST_BIT(uShaderFlag, CAMERA_VIEW_MATRIX) )
			{
				pCurrentParameterInfo = &pShaderParameterInfo[CAMERA_VIEW_MATRIX];

				if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
				{
					m_pShaderParametersHandler->SetParameters( Camera.GetViewMatrix().fValue, pCurrentParameterInfo->uAlphaIndex, 16 * sizeof(FLOAT) );
					m_pShaderParametersHandler->SetParameters( Camera.GetViewMatrix().fValue, pCurrentParameterInfo->uBetaIndex , 16 * sizeof(FLOAT) );
				}
				else
					m_pShaderParametersHandler->SetParameters( Camera.GetViewMatrix().fValue, pCurrentParameterInfo->uAlphaIndex, 16 * sizeof(FLOAT) );
			}

			if( ALCHEMY_TEST_BIT(uFlag, CAMERA_PROJECTION_MATRIX) )
			{
				pCurrentParameterInfo = &pParameterInfo[CAMERA_PROJECTION_MATRIX];

				if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
				{
					SetShaderMatrix(RenderSystem, IEffect::VERTEX_SHADER  , Camera.GetProjectionMatrix(), pCurrentParameterInfo->uAlphaIndex);
					SetShaderMatrix(RenderSystem, IEffect::FRAGMENT_SHADER, Camera.GetProjectionMatrix(), pCurrentParameterInfo->uBetaIndex );
				}
				else
					SetShaderMatrix(RenderSystem, pCurrentParameterInfo->Type, Camera.GetProjectionMatrix(), pCurrentParameterInfo->uAlphaIndex);
			}

			if( ALCHEMY_TEST_BIT(uShaderFlag, CAMERA_PROJECTION_MATRIX) )
			{
				pCurrentParameterInfo = &pShaderParameterInfo[CAMERA_PROJECTION_MATRIX];

				if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
				{
					m_pShaderParametersHandler->SetParameters( Camera.GetProjectionMatrix().fValue, pCurrentParameterInfo->uAlphaIndex, 16 * sizeof(FLOAT) );
					m_pShaderParametersHandler->SetParameters( Camera.GetProjectionMatrix().fValue, pCurrentParameterInfo->uBetaIndex , 16 * sizeof(FLOAT) );
				}
				else
					m_pShaderParametersHandler->SetParameters( Camera.GetProjectionMatrix().fValue, pCurrentParameterInfo->uAlphaIndex, 16 * sizeof(FLOAT) );
			}

			if( ALCHEMY_TEST_BIT(uFlag, CAMERA_VIEW_PROJECTION_MATRIX) )
			{
				pCurrentParameterInfo = &pParameterInfo[CAMERA_VIEW_PROJECTION_MATRIX];

				if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
				{
					SetShaderMatrix(RenderSystem, IEffect::VERTEX_SHADER  , Camera.GetViewProjectionMatrix(), pCurrentParameterInfo->uAlphaIndex);
					SetShaderMatrix(RenderSystem, IEffect::FRAGMENT_SHADER, Camera.GetViewProjectionMatrix(), pCurrentParameterInfo->uBetaIndex );
				}
				else
					SetShaderMatrix(RenderSystem, pCurrentParameterInfo->Type, Camera.GetViewProjectionMatrix(), pCurrentParameterInfo->uAlphaIndex);
			}

			if( ALCHEMY_TEST_BIT(uShaderFlag, CAMERA_VIEW_PROJECTION_MATRIX) )
			{
				pCurrentParameterInfo = &pShaderParameterInfo[CAMERA_VIEW_PROJECTION_MATRIX];

				if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
				{
					m_pShaderParametersHandler->SetParameters( Camera.GetViewProjectionMatrix().fValue, pCurrentParameterInfo->uAlphaIndex, 16 * sizeof(FLOAT) );
					m_pShaderParametersHandler->SetParameters( Camera.GetViewProjectionMatrix().fValue, pCurrentParameterInfo->uBetaIndex , 16 * sizeof(FLOAT) );
				}
				else
					m_pShaderParametersHandler->SetParameters( Camera.GetViewProjectionMatrix().fValue, pCurrentParameterInfo->uAlphaIndex, 16 * sizeof(FLOAT) );
			}

			if( ALCHEMY_TEST_BIT(uFlag, CAMERA_INVERSE_VIEW_MATRIX) )
			{
				pCurrentParameterInfo = &pParameterInfo[CAMERA_INVERSE_VIEW_MATRIX];

				if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
				{
					SetShaderMatrix(RenderSystem, IEffect::VERTEX_SHADER  , Camera.GetWorldMatrix(), pCurrentParameterInfo->uAlphaIndex);
					SetShaderMatrix(RenderSystem, IEffect::FRAGMENT_SHADER, Camera.GetWorldMatrix(), pCurrentParameterInfo->uBetaIndex );
				}
				else
					SetShaderMatrix(RenderSystem, pCurrentParameterInfo->Type, Camera.GetWorldMatrix(), pCurrentParameterInfo->uAlphaIndex);
			}

			if( ALCHEMY_TEST_BIT(uShaderFlag, CAMERA_INVERSE_VIEW_MATRIX) )
			{
				pCurrentParameterInfo = &pShaderParameterInfo[CAMERA_INVERSE_VIEW_MATRIX];

				if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
				{
					m_pShaderParametersHandler->SetParameters( Camera.GetWorldMatrix().fValue, pCurrentParameterInfo->uAlphaIndex, 16 * sizeof(FLOAT) );
					m_pShaderParametersHandler->SetParameters( Camera.GetWorldMatrix().fValue, pCurrentParameterInfo->uBetaIndex , 16 * sizeof(FLOAT) );
				}
				else
					m_pShaderParametersHandler->SetParameters( Camera.GetWorldMatrix().fValue, pCurrentParameterInfo->uAlphaIndex, 16 * sizeof(FLOAT) );
			}

			if( ALCHEMY_TEST_BIT(uFlag, CAMERA_INVERSE_PROJECTION_MATRIX) || ALCHEMY_TEST_BIT(uShaderFlag, CAMERA_INVERSE_PROJECTION_MATRIX) )
			{
				g_Matrix  = Camera.GetProjectionMatrix();
				g_Matrix.Invert4x4();

				if( ALCHEMY_TEST_BIT(uFlag, CAMERA_INVERSE_PROJECTION_MATRIX) )
				{
					pCurrentParameterInfo = &pParameterInfo[CAMERA_INVERSE_PROJECTION_MATRIX];

					if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
					{
						SetShaderMatrix(RenderSystem, IEffect::VERTEX_SHADER  , g_Matrix, pCurrentParameterInfo->uAlphaIndex);
						SetShaderMatrix(RenderSystem, IEffect::FRAGMENT_SHADER, g_Matrix, pCurrentParameterInfo->uBetaIndex );
					}
					else
						SetShaderMatrix(RenderSystem, pCurrentParameterInfo->Type, g_Matrix, pCurrentParameterInfo->uAlphaIndex);
				}

				if( ALCHEMY_TEST_BIT(uShaderFlag, CAMERA_INVERSE_PROJECTION_MATRIX) )
				{
					pCurrentParameterInfo = &pShaderParameterInfo[CAMERA_INVERSE_PROJECTION_MATRIX];

					if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
					{
						m_pShaderParametersHandler->SetParameters( g_Matrix.fValue, pCurrentParameterInfo->uAlphaIndex, 16 * sizeof(FLOAT) );
						m_pShaderParametersHandler->SetParameters( g_Matrix.fValue, pCurrentParameterInfo->uBetaIndex , 16 * sizeof(FLOAT) );
					}
					else
						m_pShaderParametersHandler->SetParameters( g_Matrix.fValue, pCurrentParameterInfo->uAlphaIndex, 16 * sizeof(FLOAT) );
				}
			}
		}

		if( ALCHEMY_TEST_BIT(uFlag, CAMERA_LOCAL_POSITION) || ALCHEMY_TEST_BIT(uShaderFlag, CAMERA_LOCAL_POSITION) )
		{
			if(!m_bIsInverceWorldMatrix)
			{
				m_InverceWorldMatrix    = m_WorldMatrix;
				m_bIsInverceWorldMatrix = m_InverceWorldMatrix.Invert3x4();
			}

			//g_Position = Camera.GetWorldMatrix().Row[3];
			Camera.GetWorldMatrix().GetPosition(g_Position);
			g_Position.Transform(m_InverceWorldMatrix);

			if( ALCHEMY_TEST_BIT(uFlag, CAMERA_LOCAL_POSITION) )
			{
				pCurrentParameterInfo = &pParameterInfo[CAMERA_LOCAL_POSITION];

				if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
				{
					RenderSystem.SetShaderParameters(IEffect::VERTEX_SHADER  , pCurrentParameterInfo->uAlphaIndex, static_cast<const FLOAT*>(g_Position), 4);
					RenderSystem.SetShaderParameters(IEffect::FRAGMENT_SHADER, pCurrentParameterInfo->uBetaIndex , static_cast<const FLOAT*>(g_Position), 4);
				}
				else
					RenderSystem.SetShaderParameters(pCurrentParameterInfo->Type, pCurrentParameterInfo->uAlphaIndex, static_cast<const FLOAT*>(g_Position), 4);
			}

			if( ALCHEMY_TEST_BIT(uShaderFlag, CAMERA_LOCAL_POSITION) )
			{
				pCurrentParameterInfo = &pShaderParameterInfo[CAMERA_LOCAL_POSITION];

				if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
				{
					m_pShaderParametersHandler->SetParameters( static_cast<const FLOAT*>(g_Position), pCurrentParameterInfo->uAlphaIndex, 4 * sizeof(FLOAT) );
					m_pShaderParametersHandler->SetParameters( static_cast<const FLOAT*>(g_Position), pCurrentParameterInfo->uBetaIndex , 4 * sizeof(FLOAT) );
				}
				else
					m_pShaderParametersHandler->SetParameters( static_cast<const FLOAT*>(g_Position), pCurrentParameterInfo->uAlphaIndex, 4 * sizeof(FLOAT) );
			}
		}

		if( ALCHEMY_TEST_BIT(uFlag      , CAMERA_LOCAL_DIRECTION) || 
			ALCHEMY_TEST_BIT(uShaderFlag, CAMERA_LOCAL_DIRECTION) || 
			ALCHEMY_TEST_BIT(uFlag      , CAMERA_LOCAL_DIRECTION_NEGATE) || 
			ALCHEMY_TEST_BIT(uShaderFlag, CAMERA_LOCAL_DIRECTION_NEGATE) )
		{
			if(!m_bIsTransposeWorldMatrix)
			{
				m_TransposeWorldMatrix = m_WorldMatrix;
				
				m_TransposeWorldMatrix.Transpose();
				
				m_bIsTransposeWorldMatrix = true;
			}

			//g_Direction = Camera.GetWorldMatrix().Row[2].Float3;
			Camera.GetWorldMatrix().GetDirection(g_Direction);

			g_Direction.TransformNormal(m_TransposeWorldMatrix);
			g_Direction.Normalize();

			if( ALCHEMY_TEST_BIT(uFlag, CAMERA_LOCAL_DIRECTION) )
			{
				pCurrentParameterInfo = &pParameterInfo[CAMERA_LOCAL_DIRECTION];

				if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
				{
					RenderSystem.SetShaderParameters(IEffect::VERTEX_SHADER  , pParameterInfo->uAlphaIndex, static_cast<const FLOAT*>(g_Direction), 3);
					RenderSystem.SetShaderParameters(IEffect::FRAGMENT_SHADER, pParameterInfo->uBetaIndex , static_cast<const FLOAT*>(g_Direction), 3);
				}
				else
					RenderSystem.SetShaderParameters(pParameterInfo->Type, pParameterInfo->uAlphaIndex, static_cast<const FLOAT*>(g_Direction), 3);
			}

			if( ALCHEMY_TEST_BIT(uShaderFlag, CAMERA_LOCAL_DIRECTION) )
			{
				pCurrentParameterInfo = &pShaderParameterInfo[CAMERA_LOCAL_DIRECTION];

				if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
				{
					m_pShaderParametersHandler->SetParameters( static_cast<const FLOAT*>(g_Direction), pCurrentParameterInfo->uAlphaIndex, 3 * sizeof(FLOAT) );
					m_pShaderParametersHandler->SetParameters( static_cast<const FLOAT*>(g_Direction), pCurrentParameterInfo->uBetaIndex , 3 * sizeof(FLOAT) );
				}
				else
					m_pShaderParametersHandler->SetParameters( static_cast<const FLOAT*>(g_Direction), pCurrentParameterInfo->uAlphaIndex, 3 * sizeof(FLOAT) );
			}

			if( ALCHEMY_TEST_BIT(uFlag, CAMERA_LOCAL_DIRECTION_NEGATE) || ALCHEMY_TEST_BIT(uShaderFlag, CAMERA_LOCAL_DIRECTION_NEGATE) )
			{
				g_Direction.Negate();

				if( ALCHEMY_TEST_BIT(uFlag, CAMERA_LOCAL_DIRECTION_NEGATE) )
				{
					pCurrentParameterInfo = &pParameterInfo[CAMERA_LOCAL_DIRECTION_NEGATE];

					if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
					{
						RenderSystem.SetShaderParameters(IEffect::VERTEX_SHADER  , pCurrentParameterInfo->uAlphaIndex, static_cast<const FLOAT*>(g_Direction), 3);
						RenderSystem.SetShaderParameters(IEffect::FRAGMENT_SHADER, pCurrentParameterInfo->uBetaIndex , static_cast<const FLOAT*>(g_Direction), 3);
					}
					else
						RenderSystem.SetShaderParameters(pCurrentParameterInfo->Type, pCurrentParameterInfo->uAlphaIndex, static_cast<const FLOAT*>(g_Direction), 3);
				}

				if( ALCHEMY_TEST_BIT(uShaderFlag, CAMERA_LOCAL_DIRECTION) )
				{
					pCurrentParameterInfo = &pShaderParameterInfo[CAMERA_LOCAL_DIRECTION];

					if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
					{
						m_pShaderParametersHandler->SetParameters( static_cast<const FLOAT*>(g_Direction), pCurrentParameterInfo->uAlphaIndex, 3 * sizeof(FLOAT) );
						m_pShaderParametersHandler->SetParameters( static_cast<const FLOAT*>(g_Direction), pCurrentParameterInfo->uBetaIndex , 3 * sizeof(FLOAT) );
					}
					else
						m_pShaderParametersHandler->SetParameters( static_cast<const FLOAT*>(g_Direction), pCurrentParameterInfo->uAlphaIndex, 3 * sizeof(FLOAT) );
				}
			}
		}

		if( ALCHEMY_TEST_BIT(uFlag, CAMERA_WORLD_VIEW_MATRIX) || ALCHEMY_TEST_BIT(uShaderFlag, CAMERA_WORLD_VIEW_MATRIX) )
		{
			g_Matrix  = m_WorldMatrix;
			g_Matrix *= Camera.GetViewMatrix();

			if( ALCHEMY_TEST_BIT(uFlag, CAMERA_WORLD_VIEW_MATRIX) )
			{
				pCurrentParameterInfo = &pParameterInfo[CAMERA_WORLD_VIEW_MATRIX];

				if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
				{
					SetShaderMatrix(RenderSystem, IEffect::VERTEX_SHADER  , g_Matrix, pCurrentParameterInfo->uAlphaIndex);
					SetShaderMatrix(RenderSystem, IEffect::FRAGMENT_SHADER, g_Matrix, pCurrentParameterInfo->uBetaIndex );
				}
				else
					SetShaderMatrix(RenderSystem, pCurrentParameterInfo->Type, g_Matrix, pCurrentParameterInfo->uAlphaIndex);
			}

			if( ALCHEMY_TEST_BIT(uShaderFlag, CAMERA_WORLD_VIEW_MATRIX) )
			{
				pCurrentParameterInfo = &pShaderParameterInfo[CAMERA_WORLD_VIEW_MATRIX];

				if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
				{
					m_pShaderParametersHandler->SetParameters( g_Matrix.fValue, pCurrentParameterInfo->uAlphaIndex, 16 * sizeof(FLOAT) );
					m_pShaderParametersHandler->SetParameters( g_Matrix.fValue, pCurrentParameterInfo->uBetaIndex , 16 * sizeof(FLOAT) );
				}
				else
					m_pShaderParametersHandler->SetParameters( g_Matrix.fValue, pCurrentParameterInfo->uAlphaIndex, 16 * sizeof(FLOAT) );
			}
		}

		if( ALCHEMY_TEST_BIT(uFlag, CAMERA_WORLD_VIEW_PROJECTION_MATRIX) || ALCHEMY_TEST_BIT(uShaderFlag, CAMERA_WORLD_VIEW_PROJECTION_MATRIX) )
		{
			g_Matrix  = m_WorldMatrix;
			g_Matrix *= Camera.GetViewProjectionMatrix();

			if( ALCHEMY_TEST_BIT(uFlag, CAMERA_WORLD_VIEW_PROJECTION_MATRIX) )
			{
				pCurrentParameterInfo = &pParameterInfo[CAMERA_WORLD_VIEW_PROJECTION_MATRIX];

				if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
				{
					SetShaderMatrix(RenderSystem, IEffect::VERTEX_SHADER  , g_Matrix, pCurrentParameterInfo->uAlphaIndex);
					SetShaderMatrix(RenderSystem, IEffect::FRAGMENT_SHADER, g_Matrix, pCurrentParameterInfo->uBetaIndex );
				}
				else
					SetShaderMatrix(RenderSystem, pCurrentParameterInfo->Type, g_Matrix, pCurrentParameterInfo->uAlphaIndex);
			}

			if( ALCHEMY_TEST_BIT(uShaderFlag, CAMERA_WORLD_VIEW_PROJECTION_MATRIX) )
			{
				pCurrentParameterInfo = &pShaderParameterInfo[CAMERA_WORLD_VIEW_PROJECTION_MATRIX];

				if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
				{
					m_pShaderParametersHandler->SetParameters( g_Matrix.fValue, pCurrentParameterInfo->uAlphaIndex, 16 * sizeof(FLOAT) );
					m_pShaderParametersHandler->SetParameters( g_Matrix.fValue, pCurrentParameterInfo->uBetaIndex , 16 * sizeof(FLOAT) );
				}
				else
					m_pShaderParametersHandler->SetParameters( g_Matrix.fValue, pCurrentParameterInfo->uAlphaIndex, 16 * sizeof(FLOAT) );
			}
		}

		return true;
	}

	return false;
}

bool CRenderMethod::SetLight(const CLight& Light, UINT uIndex, bool bIsSetWorldMatrixOnly)
{
	if(!m_pParameterList)
	{
		ALCHEMY_DEBUG_WARNING("Render method has not been created.");

		return false;
	}

	const PARAMETERLIST* pParameterList = &m_pParameterList[m_uParameterListIndex], * pShaderParameterList = m_pShaderParametersHandler ? m_pShaderParametersHandler->GetParameterlist(m_uParameterListIndex) : ALCHEMY_NULL;

	if( uIndex < pParameterList->uLightCount || (pShaderParameterList && uIndex < pShaderParameterList->uLightCount) )
	{
		UINT uFlag = uIndex < pParameterList->uLightCount ? pParameterList->puLightParameterFlag[uIndex] : 0, uShaderFlag = (pShaderParameterList && uIndex < pShaderParameterList->uLightCount) ? pShaderParameterList->puLightParameterFlag[uIndex] : 0;
		if(!uFlag && !uShaderFlag)
			return true;

		const PARAMETERINFO* pParameterInfo = uFlag ? pParameterList->pLightParameters[uIndex] : static_cast<const PARAMETERINFO*>(ALCHEMY_NULL), * pShaderParameterInfo = uShaderFlag ? pShaderParameterList->pLightParameters[uIndex] : static_cast<const PARAMETERINFO*>(ALCHEMY_NULL), * pCurrentParameterInfo;
		CRenderSystem& RenderSystem = static_cast<CSceneManager&>( GetGraphicsResourcePool() ).GetRenderSystem();

		if(!bIsSetWorldMatrixOnly)
		{
			if( ALCHEMY_TEST_BIT(uFlag, LIGHT_AMBIENT) )
			{
				pCurrentParameterInfo = &pParameterInfo[LIGHT_AMBIENT];

				if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
				{
					RenderSystem.SetShaderParameters(IEffect::VERTEX_SHADER  , pCurrentParameterInfo->uAlphaIndex, Light.Ambient.fValue, 4);
					RenderSystem.SetShaderParameters(IEffect::FRAGMENT_SHADER, pCurrentParameterInfo->uBetaIndex , Light.Ambient.fValue, 4);
				}
				else
					RenderSystem.SetShaderParameters(pCurrentParameterInfo->Type, pCurrentParameterInfo->uAlphaIndex, Light.Ambient.fValue, 4);
			}

			if( ALCHEMY_TEST_BIT(uShaderFlag, LIGHT_AMBIENT) )
			{
				pCurrentParameterInfo = &pShaderParameterInfo[LIGHT_AMBIENT];

				if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
				{
					m_pShaderParametersHandler->SetParameters( Light.Ambient.fValue, pCurrentParameterInfo->uAlphaIndex, 4 * sizeof(FLOAT) );
					m_pShaderParametersHandler->SetParameters( Light.Ambient.fValue, pCurrentParameterInfo->uBetaIndex , 4 * sizeof(FLOAT) );
				}
				else
					m_pShaderParametersHandler->SetParameters( Light.Ambient.fValue, pCurrentParameterInfo->uAlphaIndex, 4 * sizeof(FLOAT) );
			}

			if( ALCHEMY_TEST_BIT(uFlag, LIGHT_DIFFUSE) )
			{
				pCurrentParameterInfo = &pParameterInfo[LIGHT_DIFFUSE];

				if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
				{
					RenderSystem.SetShaderParameters(IEffect::VERTEX_SHADER  , pCurrentParameterInfo->uAlphaIndex, Light.Diffuse.fValue, 4);
					RenderSystem.SetShaderParameters(IEffect::FRAGMENT_SHADER, pCurrentParameterInfo->uBetaIndex , Light.Diffuse.fValue, 4);
				}
				else
					RenderSystem.SetShaderParameters(pCurrentParameterInfo->Type, pCurrentParameterInfo->uAlphaIndex, Light.Diffuse.fValue, 4);
			}

			if( ALCHEMY_TEST_BIT(uShaderFlag, LIGHT_DIFFUSE) )
			{
				pCurrentParameterInfo = &pShaderParameterInfo[LIGHT_DIFFUSE];

				if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
				{
					m_pShaderParametersHandler->SetParameters( Light.Diffuse.fValue, pCurrentParameterInfo->uAlphaIndex, 4 * sizeof(FLOAT) );
					m_pShaderParametersHandler->SetParameters( Light.Diffuse.fValue, pCurrentParameterInfo->uBetaIndex , 4 * sizeof(FLOAT) );
				}
				else
					m_pShaderParametersHandler->SetParameters( Light.Diffuse.fValue, pCurrentParameterInfo->uAlphaIndex, 4 * sizeof(FLOAT) );
			}

			if( ALCHEMY_TEST_BIT(uFlag, LIGHT_SPECULAR) )
			{
				pCurrentParameterInfo = &pParameterInfo[LIGHT_SPECULAR];

				if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
				{
					RenderSystem.SetShaderParameters(IEffect::VERTEX_SHADER  , pCurrentParameterInfo->uAlphaIndex, Light.Specular.fValue, 4);
					RenderSystem.SetShaderParameters(IEffect::FRAGMENT_SHADER, pCurrentParameterInfo->uBetaIndex , Light.Specular.fValue, 4);
				}
				else
					RenderSystem.SetShaderParameters(pCurrentParameterInfo->Type, pCurrentParameterInfo->uAlphaIndex, Light.Specular.fValue, 4);
			}

			if( ALCHEMY_TEST_BIT(uShaderFlag, LIGHT_SPECULAR) )
			{
				pCurrentParameterInfo = &pShaderParameterInfo[LIGHT_SPECULAR];

				if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
				{
					m_pShaderParametersHandler->SetParameters( Light.Specular.fValue, pCurrentParameterInfo->uAlphaIndex, 4 * sizeof(FLOAT) );
					m_pShaderParametersHandler->SetParameters( Light.Specular.fValue, pCurrentParameterInfo->uBetaIndex , 4 * sizeof(FLOAT) );
				}
				else
					m_pShaderParametersHandler->SetParameters( Light.Specular.fValue, pCurrentParameterInfo->uAlphaIndex, 4 * sizeof(FLOAT) );
			}

			if( ALCHEMY_TEST_BIT(uFlag, LIGHT_POSITION) )
			{
				pCurrentParameterInfo = &pParameterInfo[LIGHT_POSITION];

				if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
				{
					RenderSystem.SetShaderParameters(IEffect::VERTEX_SHADER  , pCurrentParameterInfo->uAlphaIndex, Light.Position.fValue, 4);
					RenderSystem.SetShaderParameters(IEffect::FRAGMENT_SHADER, pCurrentParameterInfo->uBetaIndex , Light.Position.fValue, 4);
				}
				else
					RenderSystem.SetShaderParameters(pCurrentParameterInfo->Type, pCurrentParameterInfo->uAlphaIndex, Light.Position.fValue, 4);
			}

			if( ALCHEMY_TEST_BIT(uShaderFlag, LIGHT_POSITION) )
			{
				pCurrentParameterInfo = &pShaderParameterInfo[LIGHT_POSITION];

				if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
				{
					m_pShaderParametersHandler->SetParameters( Light.Position.fValue, pCurrentParameterInfo->uAlphaIndex, 4 * sizeof(FLOAT) );
					m_pShaderParametersHandler->SetParameters( Light.Position.fValue, pCurrentParameterInfo->uBetaIndex , 4 * sizeof(FLOAT) );
				}
				else
					m_pShaderParametersHandler->SetParameters( Light.Position.fValue, pCurrentParameterInfo->uAlphaIndex, 4 * sizeof(FLOAT) );
			}

			if( ALCHEMY_TEST_BIT(uFlag, LIGHT_DIRECTION) )
			{
				pCurrentParameterInfo = &pParameterInfo[LIGHT_DIRECTION];

				if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
				{
					RenderSystem.SetShaderParameters(IEffect::VERTEX_SHADER  , pCurrentParameterInfo->uAlphaIndex, Light.Direction.fValue, 3);
					RenderSystem.SetShaderParameters(IEffect::FRAGMENT_SHADER, pCurrentParameterInfo->uBetaIndex , Light.Direction.fValue, 3);
				}
				else
					RenderSystem.SetShaderParameters(pCurrentParameterInfo->Type, pCurrentParameterInfo->uAlphaIndex, Light.Direction.fValue, 3);
			}

			if( ALCHEMY_TEST_BIT(uShaderFlag, LIGHT_DIRECTION) )
			{
				pCurrentParameterInfo = &pShaderParameterInfo[LIGHT_DIRECTION];

				if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
				{
					m_pShaderParametersHandler->SetParameters( Light.Direction.fValue, pCurrentParameterInfo->uAlphaIndex, 3 * sizeof(FLOAT) );
					m_pShaderParametersHandler->SetParameters( Light.Direction.fValue, pCurrentParameterInfo->uBetaIndex , 3 * sizeof(FLOAT) );
				}
				else
					m_pShaderParametersHandler->SetParameters( Light.Direction.fValue, pCurrentParameterInfo->uAlphaIndex, 3 * sizeof(FLOAT) );
			}

			if( ALCHEMY_TEST_BIT(uFlag, LIGHT_RANGE) )
			{
				pCurrentParameterInfo = &pParameterInfo[LIGHT_RANGE];

				if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
				{
					RenderSystem.SetShaderParameters(IEffect::VERTEX_SHADER  , pCurrentParameterInfo->uAlphaIndex, &Light.fRange, 1);
					RenderSystem.SetShaderParameters(IEffect::FRAGMENT_SHADER, pCurrentParameterInfo->uBetaIndex , &Light.fRange, 1);
				}
				else
					RenderSystem.SetShaderParameters(pCurrentParameterInfo->Type, pCurrentParameterInfo->uAlphaIndex, &Light.fRange, 1);
			}

			if( ALCHEMY_TEST_BIT(uShaderFlag, LIGHT_RANGE) )
			{
				pCurrentParameterInfo = &pShaderParameterInfo[LIGHT_RANGE];

				if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
				{
					m_pShaderParametersHandler->SetParameters( &Light.fRange, pCurrentParameterInfo->uAlphaIndex, sizeof(FLOAT) );
					m_pShaderParametersHandler->SetParameters( &Light.fRange, pCurrentParameterInfo->uBetaIndex , sizeof(FLOAT) );
				}
				else
					m_pShaderParametersHandler->SetParameters( &Light.fRange, pCurrentParameterInfo->uAlphaIndex, sizeof(FLOAT) );
			}

			if( ALCHEMY_TEST_BIT(uFlag, LIGHT_FALLOFF) )
			{
				pCurrentParameterInfo = &pParameterInfo[LIGHT_FALLOFF];

				if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
				{
					RenderSystem.SetShaderParameters(IEffect::VERTEX_SHADER  , pCurrentParameterInfo->uAlphaIndex, &Light.fFalloff, 1);
					RenderSystem.SetShaderParameters(IEffect::FRAGMENT_SHADER, pCurrentParameterInfo->uBetaIndex , &Light.fFalloff, 1);
				}
				else
					RenderSystem.SetShaderParameters(pCurrentParameterInfo->Type, pCurrentParameterInfo->uAlphaIndex, &Light.fFalloff, 1);
			}

			if( ALCHEMY_TEST_BIT(uShaderFlag, LIGHT_FALLOFF) )
			{
				pCurrentParameterInfo = &pShaderParameterInfo[LIGHT_FALLOFF];

				if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
				{
					m_pShaderParametersHandler->SetParameters( &Light.fFalloff, pCurrentParameterInfo->uAlphaIndex, sizeof(FLOAT) );
					m_pShaderParametersHandler->SetParameters( &Light.fFalloff, pCurrentParameterInfo->uBetaIndex , sizeof(FLOAT) );
				}
				else
					m_pShaderParametersHandler->SetParameters( &Light.fFalloff, pCurrentParameterInfo->uAlphaIndex, sizeof(FLOAT) );
			}

			if( ALCHEMY_TEST_BIT(uFlag, LIGHT_ATTENUATION0) )
			{
				pCurrentParameterInfo = &pParameterInfo[LIGHT_ATTENUATION0];

				if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
				{
					RenderSystem.SetShaderParameters(IEffect::VERTEX_SHADER  , pCurrentParameterInfo->uAlphaIndex, &Light.fAttenuation0, 1);
					RenderSystem.SetShaderParameters(IEffect::FRAGMENT_SHADER, pCurrentParameterInfo->uBetaIndex , &Light.fAttenuation0, 1);
				}
				else
					RenderSystem.SetShaderParameters(pCurrentParameterInfo->Type, pCurrentParameterInfo->uAlphaIndex, &Light.fAttenuation0, 1);
			}

			if( ALCHEMY_TEST_BIT(uShaderFlag, LIGHT_ATTENUATION0) )
			{
				pCurrentParameterInfo = &pShaderParameterInfo[LIGHT_ATTENUATION0];

				if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
				{
					m_pShaderParametersHandler->SetParameters( &Light.fAttenuation0, pCurrentParameterInfo->uAlphaIndex, sizeof(FLOAT) );
					m_pShaderParametersHandler->SetParameters( &Light.fAttenuation0, pCurrentParameterInfo->uBetaIndex , sizeof(FLOAT) );
				}
				else
					m_pShaderParametersHandler->SetParameters( &Light.fAttenuation0, pCurrentParameterInfo->uAlphaIndex, sizeof(FLOAT) );
			}

			if( ALCHEMY_TEST_BIT(uFlag, LIGHT_ATTENUATION1) )
			{
				pCurrentParameterInfo = &pParameterInfo[LIGHT_ATTENUATION1];

				if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
				{
					RenderSystem.SetShaderParameters(IEffect::VERTEX_SHADER  , pCurrentParameterInfo->uAlphaIndex, &Light.fAttenuation1, 1);
					RenderSystem.SetShaderParameters(IEffect::FRAGMENT_SHADER, pCurrentParameterInfo->uBetaIndex , &Light.fAttenuation1, 1);
				}
				else
					RenderSystem.SetShaderParameters(pCurrentParameterInfo->Type, pCurrentParameterInfo->uAlphaIndex, &Light.fAttenuation1, 1);
			}

			if( ALCHEMY_TEST_BIT(uShaderFlag, LIGHT_ATTENUATION1) )
			{
				pCurrentParameterInfo = &pShaderParameterInfo[LIGHT_ATTENUATION1];

				if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
				{
					m_pShaderParametersHandler->SetParameters( &Light.fAttenuation1, pCurrentParameterInfo->uAlphaIndex, sizeof(FLOAT) );
					m_pShaderParametersHandler->SetParameters( &Light.fAttenuation1, pCurrentParameterInfo->uBetaIndex , sizeof(FLOAT) );
				}
				else
					m_pShaderParametersHandler->SetParameters( &Light.fAttenuation1, pCurrentParameterInfo->uAlphaIndex, sizeof(FLOAT) );
			}

			if( ALCHEMY_TEST_BIT(uFlag, LIGHT_ATTENUATION2) )
			{
				pCurrentParameterInfo = &pParameterInfo[LIGHT_ATTENUATION2];

				if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
				{
					RenderSystem.SetShaderParameters(IEffect::VERTEX_SHADER  , pCurrentParameterInfo->uAlphaIndex, &Light.fAttenuation2, 1);
					RenderSystem.SetShaderParameters(IEffect::FRAGMENT_SHADER, pCurrentParameterInfo->uBetaIndex , &Light.fAttenuation2, 1);
				}
				else
					RenderSystem.SetShaderParameters(pCurrentParameterInfo->Type, pCurrentParameterInfo->uAlphaIndex, &Light.fAttenuation2, 1);
			}

			if( ALCHEMY_TEST_BIT(uShaderFlag, LIGHT_ATTENUATION2) )
			{
				pCurrentParameterInfo = &pShaderParameterInfo[LIGHT_ATTENUATION2];

				if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
				{
					m_pShaderParametersHandler->SetParameters( &Light.fAttenuation2, pCurrentParameterInfo->uAlphaIndex, sizeof(FLOAT) );
					m_pShaderParametersHandler->SetParameters( &Light.fAttenuation2, pCurrentParameterInfo->uBetaIndex , sizeof(FLOAT) );
				}
				else
					m_pShaderParametersHandler->SetParameters( &Light.fAttenuation2, pCurrentParameterInfo->uAlphaIndex, sizeof(FLOAT) );
			}

			if( ALCHEMY_TEST_BIT(uFlag, LIGHT_THETA) )
			{
				pCurrentParameterInfo = &pParameterInfo[LIGHT_THETA];

				if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
				{
					RenderSystem.SetShaderParameters(IEffect::VERTEX_SHADER  , pCurrentParameterInfo->uAlphaIndex, &Light.fTheta, 1);
					RenderSystem.SetShaderParameters(IEffect::FRAGMENT_SHADER, pCurrentParameterInfo->uBetaIndex , &Light.fTheta, 1);
				}
				else
					RenderSystem.SetShaderParameters(pCurrentParameterInfo->Type, pCurrentParameterInfo->uAlphaIndex, &Light.fTheta, 1);
			}

			if( ALCHEMY_TEST_BIT(uShaderFlag, LIGHT_THETA) )
			{
				pCurrentParameterInfo = &pShaderParameterInfo[LIGHT_THETA];

				if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
				{
					m_pShaderParametersHandler->SetParameters( &Light.fTheta, pCurrentParameterInfo->uAlphaIndex, sizeof(FLOAT) );
					m_pShaderParametersHandler->SetParameters( &Light.fTheta, pCurrentParameterInfo->uBetaIndex , sizeof(FLOAT) );
				}
				else
					m_pShaderParametersHandler->SetParameters( &Light.fTheta, pCurrentParameterInfo->uAlphaIndex, sizeof(FLOAT) );
			}

			if( ALCHEMY_TEST_BIT(uFlag, LIGHT_PHI) )
			{
				pCurrentParameterInfo = &pParameterInfo[LIGHT_PHI];

				if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
				{
					RenderSystem.SetShaderParameters(IEffect::VERTEX_SHADER  , pCurrentParameterInfo->uAlphaIndex, &Light.fPhi, 1);
					RenderSystem.SetShaderParameters(IEffect::FRAGMENT_SHADER, pCurrentParameterInfo->uBetaIndex , &Light.fPhi, 1);
				}
				else
					RenderSystem.SetShaderParameters(pCurrentParameterInfo->Type, pCurrentParameterInfo->uAlphaIndex, &Light.fPhi, 1);
			}

			if( ALCHEMY_TEST_BIT(uShaderFlag, LIGHT_PHI) )
			{
				pCurrentParameterInfo = &pShaderParameterInfo[LIGHT_PHI];

				if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
				{
					m_pShaderParametersHandler->SetParameters( &Light.fPhi, pCurrentParameterInfo->uAlphaIndex, sizeof(FLOAT) );
					m_pShaderParametersHandler->SetParameters( &Light.fPhi, pCurrentParameterInfo->uBetaIndex , sizeof(FLOAT) );
				}
				else
					m_pShaderParametersHandler->SetParameters( &Light.fPhi, pCurrentParameterInfo->uAlphaIndex, sizeof(FLOAT) );
			}

			if( ALCHEMY_TEST_BIT(uFlag      , LIGHT_COS_THETA) || 
				ALCHEMY_TEST_BIT(uShaderFlag, LIGHT_PHI) || 
				ALCHEMY_TEST_BIT(uFlag      , LIGHT_INVERSE_COS_PHI_SUBSTRUCT_COS_THETA) || 
				ALCHEMY_TEST_BIT(uShaderFlag, LIGHT_INVERSE_COS_PHI_SUBSTRUCT_COS_THETA))
			{
				FLOAT fCosTheta;
				fCosTheta = cosf(Light.fTheta);

				if( ALCHEMY_TEST_BIT(uFlag, LIGHT_COS_THETA) )
				{
					pCurrentParameterInfo = &pParameterInfo[LIGHT_COS_THETA];

					if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
					{
						RenderSystem.SetShaderParameters(IEffect::VERTEX_SHADER  , pCurrentParameterInfo->uAlphaIndex, &fCosTheta, 1);
						RenderSystem.SetShaderParameters(IEffect::FRAGMENT_SHADER, pCurrentParameterInfo->uBetaIndex , &fCosTheta, 1);
					}
					else
						RenderSystem.SetShaderParameters(pCurrentParameterInfo->Type, pCurrentParameterInfo->uAlphaIndex, &fCosTheta, 1);
				}

				if( ALCHEMY_TEST_BIT(uShaderFlag, LIGHT_COS_THETA) )
				{
					pCurrentParameterInfo = &pShaderParameterInfo[LIGHT_COS_THETA];

					if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
					{
						m_pShaderParametersHandler->SetParameters( &fCosTheta, pCurrentParameterInfo->uAlphaIndex, sizeof(FLOAT) );
						m_pShaderParametersHandler->SetParameters( &fCosTheta, pCurrentParameterInfo->uBetaIndex , sizeof(FLOAT) );
					}
					else
						m_pShaderParametersHandler->SetParameters( &fCosTheta, pCurrentParameterInfo->uAlphaIndex, sizeof(FLOAT) );
				}

				if( ALCHEMY_TEST_BIT(uFlag, LIGHT_INVERSE_COS_PHI_SUBSTRUCT_COS_THETA) || ALCHEMY_TEST_BIT(uShaderFlag, LIGHT_INVERSE_COS_PHI_SUBSTRUCT_COS_THETA) )
				{
					FLOAT fCosThetaSubstructCosPhi = cosf(Light.fPhi) - fCosTheta;
					fCosThetaSubstructCosPhi = fCosThetaSubstructCosPhi ? (1.0f / fCosThetaSubstructCosPhi) : g_fINFINITY;

					if( ALCHEMY_TEST_BIT(uFlag, LIGHT_INVERSE_COS_PHI_SUBSTRUCT_COS_THETA) )
					{
						pCurrentParameterInfo = &pParameterInfo[LIGHT_INVERSE_COS_PHI_SUBSTRUCT_COS_THETA];

						if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
						{
							RenderSystem.SetShaderParameters(IEffect::VERTEX_SHADER  , pCurrentParameterInfo->uAlphaIndex, &fCosThetaSubstructCosPhi, 1);
							RenderSystem.SetShaderParameters(IEffect::FRAGMENT_SHADER, pCurrentParameterInfo->uBetaIndex , &fCosThetaSubstructCosPhi, 1);
						}
						else
							RenderSystem.SetShaderParameters(pCurrentParameterInfo->Type, pCurrentParameterInfo->uAlphaIndex, &fCosThetaSubstructCosPhi, 1);
					}

					if( ALCHEMY_TEST_BIT(uShaderFlag, LIGHT_INVERSE_COS_PHI_SUBSTRUCT_COS_THETA) )
					{
						pCurrentParameterInfo = &pShaderParameterInfo[LIGHT_INVERSE_COS_PHI_SUBSTRUCT_COS_THETA];

						if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
						{
							m_pShaderParametersHandler->SetParameters( &fCosThetaSubstructCosPhi, pCurrentParameterInfo->uAlphaIndex, sizeof(FLOAT) );
							m_pShaderParametersHandler->SetParameters( &fCosThetaSubstructCosPhi, pCurrentParameterInfo->uBetaIndex , sizeof(FLOAT) );
						}
						else
							m_pShaderParametersHandler->SetParameters( &fCosThetaSubstructCosPhi, pCurrentParameterInfo->uAlphaIndex, sizeof(FLOAT) );
					}
				}
			}

			const CCamera* pCamera = static_cast<CSceneManager&>( GetGraphicsResourcePool() ).GetCamera(0);
			if(pCamera)
			{
				if( ALCHEMY_TEST_BIT(uFlag, LIGHT_VIEW_POSITION) || ALCHEMY_TEST_BIT(uShaderFlag, LIGHT_VIEW_POSITION) )
				{
					g_Position = Light.Position;

					g_Position.Transform( pCamera->GetViewMatrix() );

					if( ALCHEMY_TEST_BIT(uFlag, LIGHT_VIEW_POSITION) )
					{
						pCurrentParameterInfo = &pParameterInfo[LIGHT_VIEW_POSITION];

						if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
						{
							RenderSystem.SetShaderParameters(IEffect::VERTEX_SHADER  , pCurrentParameterInfo->uAlphaIndex, static_cast<const FLOAT*>(g_Position), 4);
							RenderSystem.SetShaderParameters(IEffect::FRAGMENT_SHADER, pCurrentParameterInfo->uBetaIndex , static_cast<const FLOAT*>(g_Position), 4);
						}
						else
							RenderSystem.SetShaderParameters(pCurrentParameterInfo->Type, pCurrentParameterInfo->uAlphaIndex, static_cast<const FLOAT*>(g_Position), 4);
					}

					if( ALCHEMY_TEST_BIT(uShaderFlag, LIGHT_VIEW_POSITION) )
					{
						pCurrentParameterInfo = &pShaderParameterInfo[LIGHT_VIEW_POSITION];

						if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
						{
							m_pShaderParametersHandler->SetParameters( static_cast<const FLOAT*>(g_Position), pCurrentParameterInfo->uAlphaIndex, 4 * sizeof(FLOAT) );
							m_pShaderParametersHandler->SetParameters( static_cast<const FLOAT*>(g_Position), pCurrentParameterInfo->uBetaIndex , 4 * sizeof(FLOAT) );
						}
						else
							m_pShaderParametersHandler->SetParameters( static_cast<const FLOAT*>(g_Position), pCurrentParameterInfo->uAlphaIndex, 4 * sizeof(FLOAT) );
					}
				}

				if( ALCHEMY_TEST_BIT(uFlag      , LIGHT_VIEW_DIRECTION) || 
					ALCHEMY_TEST_BIT(uShaderFlag, LIGHT_VIEW_DIRECTION) || 
					ALCHEMY_TEST_BIT(uFlag      , LIGHT_VIEW_DIRECTION_NEGATE) || 
					ALCHEMY_TEST_BIT(uShaderFlag, LIGHT_VIEW_DIRECTION_NEGATE))
				{
					g_Matrix = pCamera->GetWorldMatrix();
					g_Matrix.Transpose();
					
					g_Direction = Light.Direction;
					g_Direction.TransformNormal(g_Matrix);
					g_Direction.Normalize();

					if( ALCHEMY_TEST_BIT(uFlag, LIGHT_VIEW_DIRECTION) )
					{
						pCurrentParameterInfo = &pParameterInfo[LIGHT_VIEW_DIRECTION];

						if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
						{
							RenderSystem.SetShaderParameters(IEffect::VERTEX_SHADER  , pCurrentParameterInfo->uAlphaIndex, static_cast<const FLOAT*>(g_Direction), 3);
							RenderSystem.SetShaderParameters(IEffect::FRAGMENT_SHADER, pCurrentParameterInfo->uBetaIndex , static_cast<const FLOAT*>(g_Direction), 3);
						}
						else
							RenderSystem.SetShaderParameters(pCurrentParameterInfo->Type, pCurrentParameterInfo->uAlphaIndex, static_cast<const FLOAT*>(g_Direction), 3);
					}

					if( ALCHEMY_TEST_BIT(uShaderFlag, LIGHT_VIEW_DIRECTION) )
					{
						pCurrentParameterInfo = &pShaderParameterInfo[LIGHT_VIEW_DIRECTION];

						if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
						{
							m_pShaderParametersHandler->SetParameters( static_cast<const FLOAT*>(g_Direction), pCurrentParameterInfo->uAlphaIndex, 3 * sizeof(FLOAT) );
							m_pShaderParametersHandler->SetParameters( static_cast<const FLOAT*>(g_Direction), pCurrentParameterInfo->uBetaIndex , 3 * sizeof(FLOAT) );
						}
						else
							m_pShaderParametersHandler->SetParameters( static_cast<const FLOAT*>(g_Direction), pCurrentParameterInfo->uAlphaIndex, 3 * sizeof(FLOAT) );
					}

					if( ALCHEMY_TEST_BIT(uFlag, LIGHT_VIEW_DIRECTION_NEGATE) || ALCHEMY_TEST_BIT(uShaderFlag, LIGHT_VIEW_DIRECTION_NEGATE) )
					{
						g_Direction.Negate();

						if( ALCHEMY_TEST_BIT(uFlag, LIGHT_VIEW_DIRECTION_NEGATE) )
						{
							pCurrentParameterInfo = &pParameterInfo[LIGHT_VIEW_DIRECTION_NEGATE];

							if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
							{
								RenderSystem.SetShaderParameters(IEffect::VERTEX_SHADER  , pCurrentParameterInfo->uAlphaIndex, static_cast<const FLOAT*>(g_Direction), 3);
								RenderSystem.SetShaderParameters(IEffect::FRAGMENT_SHADER, pCurrentParameterInfo->uBetaIndex , static_cast<const FLOAT*>(g_Direction), 3);
							}
							else
								RenderSystem.SetShaderParameters(pCurrentParameterInfo->Type, pCurrentParameterInfo->uAlphaIndex, static_cast<const FLOAT*>(g_Direction), 3);
						}

						if( ALCHEMY_TEST_BIT(uShaderFlag, LIGHT_VIEW_DIRECTION_NEGATE) )
						{
							pCurrentParameterInfo = &pShaderParameterInfo[LIGHT_VIEW_DIRECTION_NEGATE];

							if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
							{
								m_pShaderParametersHandler->SetParameters( static_cast<const FLOAT*>(g_Direction), pCurrentParameterInfo->uAlphaIndex, 3 * sizeof(FLOAT) );
								m_pShaderParametersHandler->SetParameters( static_cast<const FLOAT*>(g_Direction), pCurrentParameterInfo->uBetaIndex , 3 * sizeof(FLOAT) );
							}
							else
								m_pShaderParametersHandler->SetParameters( static_cast<const FLOAT*>(g_Direction), pCurrentParameterInfo->uAlphaIndex, 3 * sizeof(FLOAT) );
						}
					}
				}
			}
		}

		if( ALCHEMY_TEST_BIT(uFlag, LIGHT_LOCAL_POSITION) || ALCHEMY_TEST_BIT(uShaderFlag, LIGHT_LOCAL_POSITION) )
		{
			if(!m_bIsInverceWorldMatrix)
			{
				m_InverceWorldMatrix   = m_WorldMatrix;
				m_bIsInverceWorldMatrix = m_InverceWorldMatrix.Invert3x4();
			}

			g_Position = Light.Position;

			g_Position.Transform(m_InverceWorldMatrix);

			if( ALCHEMY_TEST_BIT(uFlag, LIGHT_LOCAL_POSITION) )
			{
				pCurrentParameterInfo = &pParameterInfo[LIGHT_LOCAL_POSITION];

				if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
				{
					RenderSystem.SetShaderParameters(IEffect::VERTEX_SHADER  , pCurrentParameterInfo->uAlphaIndex, static_cast<const FLOAT*>(g_Position), 4);
					RenderSystem.SetShaderParameters(IEffect::FRAGMENT_SHADER, pCurrentParameterInfo->uBetaIndex , static_cast<const FLOAT*>(g_Position), 4);
				}
				else
					RenderSystem.SetShaderParameters(pCurrentParameterInfo->Type, pCurrentParameterInfo->uAlphaIndex, static_cast<const FLOAT*>(g_Position), 4);
			}

			if( ALCHEMY_TEST_BIT(uShaderFlag, LIGHT_LOCAL_POSITION) )
			{
				pCurrentParameterInfo = &pShaderParameterInfo[LIGHT_LOCAL_POSITION];

				if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
				{
					m_pShaderParametersHandler->SetParameters( static_cast<const FLOAT*>(g_Position), pCurrentParameterInfo->uAlphaIndex, 4 * sizeof(FLOAT) );
					m_pShaderParametersHandler->SetParameters( static_cast<const FLOAT*>(g_Position), pCurrentParameterInfo->uBetaIndex , 4 * sizeof(FLOAT) );
				}
				else
					m_pShaderParametersHandler->SetParameters( static_cast<const FLOAT*>(g_Position), pCurrentParameterInfo->uAlphaIndex, 4 * sizeof(FLOAT) );
			}
		}

		if( ALCHEMY_TEST_BIT(uFlag      , LIGHT_LOCAL_DIRECTION) || 
			ALCHEMY_TEST_BIT(uShaderFlag, LIGHT_LOCAL_DIRECTION) || 
			ALCHEMY_TEST_BIT(uFlag      , LIGHT_LOCAL_DIRECTION_NEGATE) || 
			ALCHEMY_TEST_BIT(uShaderFlag, LIGHT_LOCAL_DIRECTION_NEGATE))
		{
			if(!m_bIsTransposeWorldMatrix)
			{
				m_TransposeWorldMatrix   = m_WorldMatrix;
				m_TransposeWorldMatrix.Transpose();
				
				m_bIsTransposeWorldMatrix = true;
			}
			
			g_Direction = Light.Direction;
			g_Direction.TransformNormal(m_TransposeWorldMatrix);
			g_Direction.Normalize();

			if( ALCHEMY_TEST_BIT(uFlag, LIGHT_LOCAL_DIRECTION) )
			{
				pCurrentParameterInfo = &pParameterInfo[LIGHT_LOCAL_DIRECTION];

				if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
				{
					RenderSystem.SetShaderParameters(IEffect::VERTEX_SHADER  , pCurrentParameterInfo->uAlphaIndex, static_cast<const FLOAT*>(g_Direction), 3);
					RenderSystem.SetShaderParameters(IEffect::FRAGMENT_SHADER, pCurrentParameterInfo->uBetaIndex , static_cast<const FLOAT*>(g_Direction), 3);
				}
				else
					RenderSystem.SetShaderParameters(pCurrentParameterInfo->Type, pCurrentParameterInfo->uAlphaIndex, static_cast<const FLOAT*>(g_Direction), 3);
			}

			if( ALCHEMY_TEST_BIT(uShaderFlag, LIGHT_LOCAL_DIRECTION) )
			{
				pCurrentParameterInfo = &pShaderParameterInfo[LIGHT_LOCAL_DIRECTION];

				if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
				{
					m_pShaderParametersHandler->SetParameters( static_cast<const FLOAT*>(g_Direction), pCurrentParameterInfo->uAlphaIndex, 3 * sizeof(FLOAT) );
					m_pShaderParametersHandler->SetParameters( static_cast<const FLOAT*>(g_Direction), pCurrentParameterInfo->uBetaIndex , 3 * sizeof(FLOAT) );
				}
				else
					m_pShaderParametersHandler->SetParameters( static_cast<const FLOAT*>(g_Direction), pCurrentParameterInfo->uAlphaIndex, 3 * sizeof(FLOAT) );
			}

			if( ALCHEMY_TEST_BIT(uFlag, LIGHT_LOCAL_DIRECTION_NEGATE) || ALCHEMY_TEST_BIT(uShaderFlag, LIGHT_LOCAL_DIRECTION_NEGATE) )
			{
				g_Direction.Negate();

				if( ALCHEMY_TEST_BIT(uFlag, LIGHT_LOCAL_DIRECTION_NEGATE) )
				{
					pCurrentParameterInfo = &pParameterInfo[LIGHT_LOCAL_DIRECTION_NEGATE];

					if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
					{
						RenderSystem.SetShaderParameters(IEffect::VERTEX_SHADER  , pCurrentParameterInfo->uAlphaIndex, static_cast<const FLOAT*>(g_Direction), 3);
						RenderSystem.SetShaderParameters(IEffect::FRAGMENT_SHADER, pCurrentParameterInfo->uBetaIndex , static_cast<const FLOAT*>(g_Direction), 3);
					}
					else
						RenderSystem.SetShaderParameters(pCurrentParameterInfo->Type, pCurrentParameterInfo->uAlphaIndex, static_cast<const FLOAT*>(g_Direction), 3);
				}

				if( ALCHEMY_TEST_BIT(uShaderFlag, LIGHT_LOCAL_DIRECTION_NEGATE) )
				{
					pCurrentParameterInfo = &pShaderParameterInfo[LIGHT_LOCAL_DIRECTION_NEGATE];

					if(pCurrentParameterInfo->Type == IEffect::PROGRAM)
					{
						m_pShaderParametersHandler->SetParameters( static_cast<const FLOAT*>(g_Direction), pCurrentParameterInfo->uAlphaIndex, 3 * sizeof(FLOAT) );
						m_pShaderParametersHandler->SetParameters( static_cast<const FLOAT*>(g_Direction), pCurrentParameterInfo->uBetaIndex , 3 * sizeof(FLOAT) );
					}
					else
						m_pShaderParametersHandler->SetParameters( static_cast<const FLOAT*>(g_Direction), pCurrentParameterInfo->uAlphaIndex, 3 * sizeof(FLOAT) );
				}
			}
		}

		return true;
	}

	return false;
}

bool CRenderMethod::Activate(UINT uPass, UINT uRenderTargetIndex)
{
	if(!m_pParameterList || !m_puRenderTargetIndexCount || !m_puRenderTargetIndexOffset)
	{
		ALCHEMY_DEBUG_WARNING("CRenderMethod::Activate: Render method has not been created.");

		return false;
	}

	if(uPass >= m_uPassCount)
	{
		ALCHEMY_DEBUG_WARNING("CRenderMethod::Activate: Pass out of range.");

		return false;
	}

	if(uRenderTargetIndex >= m_puRenderTargetIndexCount[uPass])
	{
		ALCHEMY_DEBUG_WARNING("CRenderMethod::Activate: Render target index out of range.");

		return false;
	}

	m_uParameterListIndex = m_puRenderTargetIndexOffset[uPass] + uRenderTargetIndex;

	CRenderSystem& RenderSystem = static_cast<CSceneManager&>( GetGraphicsResourcePool() ).GetRenderSystem();

	const PARAMETERLIST* pParameterList = &m_pParameterList[m_uParameterListIndex], * pShaderParameterList = m_pShaderParametersHandler ? m_pShaderParametersHandler->GetParameterlist(m_uParameterListIndex) : ALCHEMY_NULL;

	const PARAMETERINFO* pParameterInfo;

	CSceneManager& SceneManager = static_cast<CSceneManager&>( CGraphicsResource::GetGraphicsResourcePool() );

	if( pParameterList->uParameterFlag || (pShaderParameterList && pShaderParameterList->uParameterFlag) )
	{
		if( ALCHEMY_TEST_BIT(pParameterList->uParameterFlag, IRenderMethod::AMBIENT_LIGHT) )
		{
			pParameterInfo = &pParameterList->Parameters[IRenderMethod::AMBIENT_LIGHT];
			if(pParameterInfo->Type == IEffect::PROGRAM)
			{
				RenderSystem.SetShaderParameters(IEffect::VERTEX_SHADER  , pParameterInfo->uAlphaIndex, static_cast<const FLOAT*>( SceneManager.GetAmbientLight() ), 1);
				RenderSystem.SetShaderParameters(IEffect::FRAGMENT_SHADER, pParameterInfo->uBetaIndex , static_cast<const FLOAT*>( SceneManager.GetAmbientLight() ), 1);
			}
			else
				RenderSystem.SetShaderParameters(pParameterInfo->Type, pParameterInfo->uAlphaIndex, static_cast<const FLOAT*>( SceneManager.GetAmbientLight() ), 1);

			if( pShaderParameterList && ALCHEMY_TEST_BIT(pShaderParameterList->uParameterFlag, AMBIENT_LIGHT) )
			{
				pParameterInfo = &pShaderParameterList->Parameters[AMBIENT_LIGHT];

				if(pParameterInfo->Type == IEffect::PROGRAM)
				{
					const UINT uSIZE = sizeof(FLOAT) * 4;
					m_pShaderParametersHandler->SetParameters(static_cast<const FLOAT*>( SceneManager.GetAmbientLight() ), pParameterInfo->uAlphaIndex, uSIZE);
					m_pShaderParametersHandler->SetParameters(static_cast<const FLOAT*>( SceneManager.GetAmbientLight() ), pParameterInfo->uBetaIndex , uSIZE);
				}
				else
					m_pShaderParametersHandler->SetParameters(static_cast<const FLOAT*>( SceneManager.GetAmbientLight() ), pParameterInfo->uAlphaIndex, sizeof(FLOAT) * 4);
			}
		}

		if( ALCHEMY_TEST_BIT(pParameterList->uParameterFlag, IRenderMethod::TOTAL_TIME) || ( pShaderParameterList && ALCHEMY_TEST_BIT(pShaderParameterList->uParameterFlag, TOTAL_TIME) ) )
		{
			FLOAT fTime = SceneManager.GetTime();

			if( ALCHEMY_TEST_BIT(pParameterList->uParameterFlag, IRenderMethod::TOTAL_TIME) )
			{
				pParameterInfo = &pParameterList->Parameters[IRenderMethod::TOTAL_TIME];

				if(pParameterInfo->Type == IEffect::PROGRAM)
				{
					RenderSystem.SetShaderParameters(IEffect::VERTEX_SHADER  , pParameterInfo->uAlphaIndex, &fTime, 1);
					RenderSystem.SetShaderParameters(IEffect::FRAGMENT_SHADER, pParameterInfo->uBetaIndex , &fTime, 1);
				}
				else
					RenderSystem.SetShaderParameters(pParameterInfo->Type, pParameterInfo->uAlphaIndex, &fTime, 1);
			}

			if( pShaderParameterList && ALCHEMY_TEST_BIT(pShaderParameterList->uParameterFlag, TOTAL_TIME) )
			{
				pParameterInfo = &pShaderParameterList->Parameters[TOTAL_TIME];

				if(pParameterInfo->Type == IEffect::PROGRAM)
				{
					m_pShaderParametersHandler->SetParameters( &fTime, pParameterInfo->uAlphaIndex, sizeof(FLOAT) );
					m_pShaderParametersHandler->SetParameters( &fTime, pParameterInfo->uBetaIndex , sizeof(FLOAT) );
				}
				else
					m_pShaderParametersHandler->SetParameters( &fTime, pParameterInfo->uAlphaIndex, sizeof(FLOAT) );
			}
		}

		if( ALCHEMY_TEST_BIT(pParameterList->uParameterFlag, IRenderMethod::ELAPSED_TIME) || ( pShaderParameterList && ALCHEMY_TEST_BIT(pShaderParameterList->uParameterFlag, ELAPSED_TIME) ) )
		{
			FLOAT fElapsedTime = SceneManager.GetElapsedTime();

			if( ALCHEMY_TEST_BIT(pParameterList->uParameterFlag, IRenderMethod::ELAPSED_TIME) )
			{
				pParameterInfo = &pParameterList->Parameters[IRenderMethod::ELAPSED_TIME];

				if(pParameterInfo->Type == IEffect::PROGRAM)
				{
					RenderSystem.SetShaderParameters(IEffect::VERTEX_SHADER  , pParameterInfo->uAlphaIndex, &fElapsedTime, 1);
					RenderSystem.SetShaderParameters(IEffect::FRAGMENT_SHADER, pParameterInfo->uBetaIndex , &fElapsedTime, 1);
				}
				else
					RenderSystem.SetShaderParameters(pParameterInfo->Type, pParameterInfo->uAlphaIndex, &fElapsedTime, 1);
			}

			if( pShaderParameterList && ALCHEMY_TEST_BIT(pShaderParameterList->uParameterFlag, ELAPSED_TIME) )
			{
				pParameterInfo = &pShaderParameterList->Parameters[ELAPSED_TIME];

				if(pParameterInfo->Type == IEffect::PROGRAM)
				{
					m_pShaderParametersHandler->SetParameters( &fElapsedTime, pParameterInfo->uAlphaIndex, sizeof(FLOAT) );
					m_pShaderParametersHandler->SetParameters( &fElapsedTime, pParameterInfo->uBetaIndex , sizeof(FLOAT) );
				}
				else
					m_pShaderParametersHandler->SetParameters( &fElapsedTime, pParameterInfo->uAlphaIndex, sizeof(FLOAT) );
			}
		}
	}

	if( pParameterList->uConstantParameterFlag || (pShaderParameterList && pShaderParameterList->uConstantParameterFlag) )
	{
		FLOAT fConstant;

		if( ALCHEMY_TEST_BIT(pParameterList->uConstantParameterFlag, CONSTANT_ZERO) || ( pShaderParameterList && ALCHEMY_TEST_BIT(pShaderParameterList->uConstantParameterFlag, CONSTANT_ZERO) ) )
		{
			fConstant = 0.0f;

			if( ALCHEMY_TEST_BIT(pParameterList->uConstantParameterFlag, CONSTANT_ZERO) )
			{
				pParameterInfo = &pParameterList->ConstantParameters[CONSTANT_ZERO];

				if(pParameterInfo->Type == IEffect::PROGRAM)
				{
					RenderSystem.SetShaderParameters(IEffect::VERTEX_SHADER  , pParameterInfo->uAlphaIndex, &fConstant, 1);
					RenderSystem.SetShaderParameters(IEffect::FRAGMENT_SHADER, pParameterInfo->uBetaIndex , &fConstant, 1);
				}
				else
					RenderSystem.SetShaderParameters(pParameterInfo->Type, pParameterInfo->uAlphaIndex, &fConstant, 1);
			}

			if( pShaderParameterList && ALCHEMY_TEST_BIT(pShaderParameterList->uConstantParameterFlag, CONSTANT_ZERO) )
			{
				pParameterInfo = &pShaderParameterList->ConstantParameters[CONSTANT_ZERO];

				if(pParameterInfo->Type == IEffect::PROGRAM)
				{
					m_pShaderParametersHandler->SetParameters( &fConstant, pParameterInfo->uAlphaIndex, sizeof(FLOAT) );
					m_pShaderParametersHandler->SetParameters( &fConstant, pParameterInfo->uBetaIndex , sizeof(FLOAT) );
				}
				else
					m_pShaderParametersHandler->SetParameters( &fConstant, pParameterInfo->uAlphaIndex, sizeof(FLOAT) );
			}
		}

		if( ALCHEMY_TEST_BIT(pParameterList->uConstantParameterFlag, CONSTANT_ONE) || ( pShaderParameterList && ALCHEMY_TEST_BIT(pShaderParameterList->uConstantParameterFlag, CONSTANT_ONE) ) )
		{
			fConstant = 1.0f;

			if( ALCHEMY_TEST_BIT(pParameterList->uConstantParameterFlag, CONSTANT_ONE) )
			{
				pParameterInfo = &pParameterList->ConstantParameters[CONSTANT_ONE];

				if(pParameterInfo->Type == IEffect::PROGRAM)
				{
					RenderSystem.SetShaderParameters(IEffect::VERTEX_SHADER  , pParameterInfo->uAlphaIndex, &fConstant, 1);
					RenderSystem.SetShaderParameters(IEffect::FRAGMENT_SHADER, pParameterInfo->uBetaIndex , &fConstant, 1);
				}
				else
					RenderSystem.SetShaderParameters(pParameterInfo->Type, pParameterInfo->uAlphaIndex, &fConstant, 1);
			}

			if( pShaderParameterList && ALCHEMY_TEST_BIT(pShaderParameterList->uConstantParameterFlag, CONSTANT_ONE) )
			{
				pParameterInfo = &pShaderParameterList->ConstantParameters[CONSTANT_ONE];

				if(pParameterInfo->Type == IEffect::PROGRAM)
				{
					m_pShaderParametersHandler->SetParameters( &fConstant, pParameterInfo->uAlphaIndex, sizeof(FLOAT) );
					m_pShaderParametersHandler->SetParameters( &fConstant, pParameterInfo->uBetaIndex , sizeof(FLOAT) );
				}
				else
					m_pShaderParametersHandler->SetParameters( &fConstant, pParameterInfo->uAlphaIndex, sizeof(FLOAT) );
			}
		}

		if( ALCHEMY_TEST_BIT(pParameterList->uConstantParameterFlag, CONSTANT_HALF_ONE) || ( pShaderParameterList && ALCHEMY_TEST_BIT(pShaderParameterList->uConstantParameterFlag, CONSTANT_HALF_ONE) ) )
		{
			fConstant = 0.5f;

			if( ALCHEMY_TEST_BIT(pParameterList->uConstantParameterFlag, CONSTANT_HALF_ONE) )
			{
				pParameterInfo = &pParameterList->ConstantParameters[CONSTANT_HALF_ONE];

				if(pParameterInfo->Type == IEffect::PROGRAM)
				{
					RenderSystem.SetShaderParameters(IEffect::VERTEX_SHADER  , pParameterInfo->uAlphaIndex, &fConstant, 1);
					RenderSystem.SetShaderParameters(IEffect::FRAGMENT_SHADER, pParameterInfo->uBetaIndex , &fConstant, 1);
				}
				else
					RenderSystem.SetShaderParameters(pParameterInfo->Type, pParameterInfo->uAlphaIndex, &fConstant, 1);
			}

			if( pShaderParameterList && ALCHEMY_TEST_BIT(pShaderParameterList->uConstantParameterFlag, CONSTANT_HALF_ONE) )
			{
				pParameterInfo = &pShaderParameterList->ConstantParameters[CONSTANT_HALF_ONE];

				if(pParameterInfo->Type == IEffect::PROGRAM)
				{
					m_pShaderParametersHandler->SetParameters( &fConstant, pParameterInfo->uAlphaIndex, sizeof(FLOAT) );
					m_pShaderParametersHandler->SetParameters( &fConstant, pParameterInfo->uBetaIndex , sizeof(FLOAT) );
				}
				else
					m_pShaderParametersHandler->SetParameters( &fConstant, pParameterInfo->uAlphaIndex, sizeof(FLOAT) );
			}
		}

		if( ALCHEMY_TEST_BIT(pParameterList->uConstantParameterFlag, CONSTANT_TWO) || ( pShaderParameterList && ALCHEMY_TEST_BIT(pShaderParameterList->uConstantParameterFlag, CONSTANT_TWO) ) )
		{
			fConstant = 2.0f;

			if( ALCHEMY_TEST_BIT(pParameterList->uConstantParameterFlag, CONSTANT_TWO) )
			{
				pParameterInfo = &pParameterList->ConstantParameters[CONSTANT_TWO];

				if(pParameterInfo->Type == IEffect::PROGRAM)
				{
					RenderSystem.SetShaderParameters(IEffect::VERTEX_SHADER  , pParameterInfo->uAlphaIndex, &fConstant, 1);
					RenderSystem.SetShaderParameters(IEffect::FRAGMENT_SHADER, pParameterInfo->uBetaIndex , &fConstant, 1);
				}
				else
					RenderSystem.SetShaderParameters(pParameterInfo->Type, pParameterInfo->uAlphaIndex, &fConstant, 1);
			}

			if( pShaderParameterList && ALCHEMY_TEST_BIT(pShaderParameterList->uConstantParameterFlag, CONSTANT_TWO) )
			{
				pParameterInfo = &pShaderParameterList->ConstantParameters[CONSTANT_TWO];

				if(pParameterInfo->Type == IEffect::PROGRAM)
				{
					m_pShaderParametersHandler->SetParameters( &fConstant, pParameterInfo->uAlphaIndex, sizeof(FLOAT) );
					m_pShaderParametersHandler->SetParameters( &fConstant, pParameterInfo->uBetaIndex , sizeof(FLOAT) );
				}
				else
					m_pShaderParametersHandler->SetParameters( &fConstant, pParameterInfo->uAlphaIndex, sizeof(FLOAT) );
			}
		}

		if( ALCHEMY_TEST_BIT(pParameterList->uConstantParameterFlag, CONSTANT_FOUR) || ( pShaderParameterList && ALCHEMY_TEST_BIT(pShaderParameterList->uConstantParameterFlag, CONSTANT_FOUR) ) )
		{
			fConstant = 4.0f;

			if( ALCHEMY_TEST_BIT(pParameterList->uConstantParameterFlag, CONSTANT_FOUR) )
			{
				pParameterInfo = &pParameterList->ConstantParameters[CONSTANT_FOUR];

				if(pParameterInfo->Type == IEffect::PROGRAM)
				{
					RenderSystem.SetShaderParameters(IEffect::VERTEX_SHADER  , pParameterInfo->uAlphaIndex, &fConstant, 1);
					RenderSystem.SetShaderParameters(IEffect::FRAGMENT_SHADER, pParameterInfo->uBetaIndex , &fConstant, 1);
				}
				else
					RenderSystem.SetShaderParameters(pParameterInfo->Type, pParameterInfo->uAlphaIndex, &fConstant, 1);
			}

			if( pShaderParameterList && ALCHEMY_TEST_BIT(pShaderParameterList->uConstantParameterFlag, CONSTANT_FOUR) )
			{
				pParameterInfo = &pShaderParameterList->ConstantParameters[CONSTANT_FOUR];

				if(pParameterInfo->Type == IEffect::PROGRAM)
				{
					m_pShaderParametersHandler->SetParameters( &fConstant, pParameterInfo->uAlphaIndex, sizeof(FLOAT) );
					m_pShaderParametersHandler->SetParameters( &fConstant, pParameterInfo->uBetaIndex , sizeof(FLOAT) );
				}
				else
					m_pShaderParametersHandler->SetParameters( &fConstant, pParameterInfo->uAlphaIndex, sizeof(FLOAT) );
			}
		}

		if( ALCHEMY_TEST_BIT(pParameterList->uConstantParameterFlag, CONSTANT_255) || ( pShaderParameterList && ALCHEMY_TEST_BIT(pShaderParameterList->uConstantParameterFlag, CONSTANT_255) ) )
		{
			fConstant = 255.0f;

			if( ALCHEMY_TEST_BIT(pParameterList->uConstantParameterFlag, CONSTANT_255) )
			{
				pParameterInfo = &pParameterList->ConstantParameters[CONSTANT_255];

				if(pParameterInfo->Type == IEffect::PROGRAM)
				{
					RenderSystem.SetShaderParameters(IEffect::VERTEX_SHADER  , pParameterInfo->uAlphaIndex, &fConstant, 1);
					RenderSystem.SetShaderParameters(IEffect::FRAGMENT_SHADER, pParameterInfo->uBetaIndex , &fConstant, 1);
				}
				else
					RenderSystem.SetShaderParameters(pParameterInfo->Type, pParameterInfo->uAlphaIndex, &fConstant, 1);
			}

			if( pShaderParameterList && ALCHEMY_TEST_BIT(pShaderParameterList->uConstantParameterFlag, CONSTANT_255) )
			{
				pParameterInfo = &pShaderParameterList->ConstantParameters[CONSTANT_255];

				if(pParameterInfo->Type == IEffect::PROGRAM)
				{
					m_pShaderParametersHandler->SetParameters( &fConstant, pParameterInfo->uAlphaIndex, sizeof(FLOAT) );
					m_pShaderParametersHandler->SetParameters( &fConstant, pParameterInfo->uBetaIndex , sizeof(FLOAT) );
				}
				else
					m_pShaderParametersHandler->SetParameters( &fConstant, pParameterInfo->uAlphaIndex, sizeof(FLOAT) );
			}
		}

		if( ALCHEMY_TEST_BIT(pParameterList->uConstantParameterFlag, CONSTANT_INVERSE_255) || ( pShaderParameterList && ALCHEMY_TEST_BIT(pShaderParameterList->uConstantParameterFlag, CONSTANT_INVERSE_255) ) )
		{
			if( ALCHEMY_TEST_BIT(pParameterList->uConstantParameterFlag, CONSTANT_INVERSE_255) )
			{
				pParameterInfo = &pParameterList->ConstantParameters[CONSTANT_INVERSE_255];

				if(pParameterInfo->Type == IEffect::PROGRAM)
				{
					RenderSystem.SetShaderParameters(IEffect::VERTEX_SHADER  , pParameterInfo->uAlphaIndex, &g_fINV_255, 1);
					RenderSystem.SetShaderParameters(IEffect::FRAGMENT_SHADER, pParameterInfo->uBetaIndex , &g_fINV_255, 1);
				}
				else
					RenderSystem.SetShaderParameters(pParameterInfo->Type, pParameterInfo->uAlphaIndex, &g_fINV_255, 1);
			}

			if( pShaderParameterList && ALCHEMY_TEST_BIT(pShaderParameterList->uConstantParameterFlag, CONSTANT_INVERSE_255) )
			{
				pParameterInfo = &pShaderParameterList->ConstantParameters[CONSTANT_INVERSE_255];

				if(pParameterInfo->Type == IEffect::PROGRAM)
				{
					m_pShaderParametersHandler->SetParameters( &g_fINV_255, pParameterInfo->uAlphaIndex, sizeof(FLOAT) );
					m_pShaderParametersHandler->SetParameters( &g_fINV_255, pParameterInfo->uBetaIndex , sizeof(FLOAT) );
				}
				else
					m_pShaderParametersHandler->SetParameters( &g_fINV_255, pParameterInfo->uAlphaIndex, sizeof(FLOAT) );
			}
		}

		if( ALCHEMY_TEST_BIT(pParameterList->uConstantParameterFlag, CONSTANT_FLOAT_TO_RGBA_CODE) || ( pShaderParameterList && ALCHEMY_TEST_BIT(pShaderParameterList->uConstantParameterFlag, CONSTANT_FLOAT_TO_RGBA_CODE) ) )
		{
			static const FLOAT s_FLOAT_TO_RGBA_CODE[] = {1.0f, 255.0f, 65025.0f, 160581375.0f};

			if( ALCHEMY_TEST_BIT(pParameterList->uConstantParameterFlag, CONSTANT_FLOAT_TO_RGBA_CODE) )
			{
				pParameterInfo = &pParameterList->ConstantParameters[CONSTANT_FLOAT_TO_RGBA_CODE];

				if(pParameterInfo->Type == IEffect::PROGRAM)
				{
					RenderSystem.SetShaderParameters(IEffect::VERTEX_SHADER  , pParameterInfo->uAlphaIndex, s_FLOAT_TO_RGBA_CODE, 4);
					RenderSystem.SetShaderParameters(IEffect::FRAGMENT_SHADER, pParameterInfo->uBetaIndex , s_FLOAT_TO_RGBA_CODE, 4);
				}
				else
					RenderSystem.SetShaderParameters(pParameterInfo->Type, pParameterInfo->uAlphaIndex, s_FLOAT_TO_RGBA_CODE, 4);
			}

			if( pShaderParameterList && ALCHEMY_TEST_BIT(pShaderParameterList->uConstantParameterFlag, CONSTANT_FLOAT_TO_RGBA_CODE) )
			{
				pParameterInfo = &pShaderParameterList->ConstantParameters[CONSTANT_FLOAT_TO_RGBA_CODE];

				if(pParameterInfo->Type == IEffect::PROGRAM)
				{
					m_pShaderParametersHandler->SetParameters(s_FLOAT_TO_RGBA_CODE, pParameterInfo->uAlphaIndex, sizeof(FLOAT) * 4);
					m_pShaderParametersHandler->SetParameters(s_FLOAT_TO_RGBA_CODE, pParameterInfo->uBetaIndex , sizeof(FLOAT) * 4);
				}
				else
					m_pShaderParametersHandler->SetParameters(s_FLOAT_TO_RGBA_CODE, pParameterInfo->uAlphaIndex, sizeof(FLOAT) * 4);
			}
		}

		if( ALCHEMY_TEST_BIT(pParameterList->uConstantParameterFlag, CONSTANT_RGBA_TO_FLOAT_CODE) || ( pShaderParameterList && ALCHEMY_TEST_BIT(pShaderParameterList->uConstantParameterFlag, CONSTANT_RGBA_TO_FLOAT_CODE) ) )
		{
			static const FLOAT s_RGBA_TO_FLOAT_CODE[] = {1.0f, 1.0f / 255.0f, 1.0f / 65025.0f, 1.0f / 160581375.0f};

			if( ALCHEMY_TEST_BIT(pParameterList->uConstantParameterFlag, CONSTANT_RGBA_TO_FLOAT_CODE) )
			{
				pParameterInfo = &pParameterList->ConstantParameters[CONSTANT_RGBA_TO_FLOAT_CODE];

				if(pParameterInfo->Type == IEffect::PROGRAM)
				{
					RenderSystem.SetShaderParameters(IEffect::VERTEX_SHADER  , pParameterInfo->uAlphaIndex, s_RGBA_TO_FLOAT_CODE, 4);
					RenderSystem.SetShaderParameters(IEffect::FRAGMENT_SHADER, pParameterInfo->uBetaIndex , s_RGBA_TO_FLOAT_CODE, 4);
				}
				else
					RenderSystem.SetShaderParameters(pParameterInfo->Type, pParameterInfo->uAlphaIndex, s_RGBA_TO_FLOAT_CODE, 4);
			}

			if( pShaderParameterList && ALCHEMY_TEST_BIT(pShaderParameterList->uConstantParameterFlag, CONSTANT_RGBA_TO_FLOAT_CODE) )
			{
				pParameterInfo = &pShaderParameterList->ConstantParameters[CONSTANT_RGBA_TO_FLOAT_CODE];

				if(pParameterInfo->Type == IEffect::PROGRAM)
				{
					m_pShaderParametersHandler->SetParameters(s_RGBA_TO_FLOAT_CODE, pParameterInfo->uAlphaIndex, sizeof(FLOAT) * 4);
					m_pShaderParametersHandler->SetParameters(s_RGBA_TO_FLOAT_CODE, pParameterInfo->uBetaIndex , sizeof(FLOAT) * 4);
				}
				else
					m_pShaderParametersHandler->SetParameters(s_RGBA_TO_FLOAT_CODE, pParameterInfo->uAlphaIndex, sizeof(FLOAT) * 4);
			}
		}
	}

	/*UINT uLightCount = SceneManager.GetLightCount();

	const CLight* pLight;
	for(UINT i = 0; i < uLightCount && i < pParameterList->uLightCount; ++ i)
	{
		pLight = SceneManager.GetLight(i);
		if(pLight)
			SetLight(*pLight, i);
	}*/
	m_bIsActivate = true;

	return true;
}

bool CRenderMethod::CommitChange(const MATRIX4X4* pWorldMatrix, const CMaterial* pMaterial)
{
	if(!m_pParameterList)
	{
		ALCHEMY_DEBUG_WARNING("Render method has not been created.");

		return false;
	}

	if(pWorldMatrix)
		SetWorldMatrix(*pWorldMatrix);
	else if(m_bIsActivate)
	{
		m_WorldMatrix.Identity();

		m_bIsInverceWorldMatrix = false;
		
		m_bIsTransposeWorldMatrix = false;
	}

	CSceneManager& SceneManager = static_cast<CSceneManager&>( CGraphicsResource::GetGraphicsResourcePool() );

	const CCamera* pCamera = SceneManager.GetCamera(0);
	if(pCamera)
		SetCamera(*pCamera, 0, !m_bIsActivate);

	const PARAMETERLIST* pParameterList = &m_pParameterList[m_uParameterListIndex];

	UINT i, uCount;

	if(pMaterial)
	{
		SetMaterial(*pMaterial);

		if(pParameterList->uCameraCount)
		{
			uCount = ALCHEMY_MININUM(pMaterial->GetCameraCount() + 1, pParameterList->uCameraCount);
			for(i = 1; i < uCount; ++ i)
			{
				pCamera = pMaterial->GetCamera(i - 1);
				if(pCamera)
					SetCamera(*pCamera, i, !m_bIsActivate);
			}
		}

		if(pParameterList->uLightCount)
		{
			const CLight* pLight;
			uCount = ALCHEMY_MININUM(pMaterial->GetLightCount(), pParameterList->uLightCount);
			for(i = 0; i < uCount; ++ i)
			{
				pLight = pMaterial->GetLight(i);
				if(pLight)
					SetLight(*pLight, i, !m_bIsActivate);
			}
		}
	}

	/*const CCamera* pCamera;
	uCount = ALCHEMY_MININUM(SceneManager.GetCameraCount(), pParameterList->uCameraCount);
	for(i = 0; i < uCount; ++ i)
	{
		pCamera = SceneManager.GetCamera(i);
		if(pCamera)
			SetCamera(*pCamera, i, !m_bIsActivate);
	}

	const CLight* pLight;
	uCount = ALCHEMY_MININUM(SceneManager.GetLightCount(), pParameterList->uLightCount);
	for(i = 0; i < uCount; ++ i)
	{
		pLight = SceneManager.GetLight(i);
		if(pLight)
			SetLight(*pLight, i, !m_bIsActivate);
	}*/

	CRenderSystem& RenderSystem = SceneManager.GetRenderSystem();
	if(m_pShaderParametersHandler)
		m_pShaderParametersHandler->SetParameters(RenderSystem, m_uParameterListIndex, m_bIsActivate);
		
	if(m_pRenderStateHandler)
		m_pRenderStateHandler->SetRenderState(RenderSystem);

	m_bIsActivate = false;

	return true;
}

bool CRenderMethod::SetWorldMatrix(const MATRIX4X4& Matrix)
{
	if(!m_pParameterList)
	{
		ALCHEMY_DEBUG_WARNING("Render method has not been created.");

		return false;
	}

	m_WorldMatrix          = Matrix;
	m_bIsInverceWorldMatrix = false;
	m_bIsTransposeWorldMatrix = false;

	CSceneManager& SceneManager = static_cast<CSceneManager&>( CGraphicsResource::GetGraphicsResourcePool() );
	CRenderSystem& RenderSystem = SceneManager.GetRenderSystem();

	const PARAMETERLIST* pParameterList = &m_pParameterList[m_uParameterListIndex], * pShaderParameterList = m_pShaderParametersHandler ? m_pShaderParametersHandler->GetParameterlist(m_uParameterListIndex) : ALCHEMY_NULL;
	const PARAMETERINFO* pParameterInfo;

	if( ALCHEMY_TEST_BIT(pParameterList->uParameterFlag, WORLD_MATRIX) )
	{
		pParameterInfo = &pParameterList->Parameters[WORLD_MATRIX];

		if(pParameterInfo->Type == IEffect::PROGRAM)
		{
			SetShaderMatrix(RenderSystem, IEffect::VERTEX_SHADER  , m_WorldMatrix, pParameterInfo->uAlphaIndex);
			SetShaderMatrix(RenderSystem, IEffect::FRAGMENT_SHADER, m_WorldMatrix, pParameterInfo->uBetaIndex );
		}
		else
			SetShaderMatrix(RenderSystem, pParameterInfo->Type, m_InverceWorldMatrix, pParameterInfo->uAlphaIndex);
	}

	if( pShaderParameterList && ALCHEMY_TEST_BIT(pShaderParameterList->uParameterFlag, WORLD_MATRIX) )
	{
		pParameterInfo = &pShaderParameterList->Parameters[WORLD_MATRIX];

		if(pParameterInfo->Type == IEffect::PROGRAM)
		{
			m_pShaderParametersHandler->SetParameters( m_WorldMatrix.fValue, pParameterInfo->uAlphaIndex, 16 * sizeof(FLOAT) );
			m_pShaderParametersHandler->SetParameters( m_WorldMatrix.fValue, pParameterInfo->uBetaIndex , 16 * sizeof(FLOAT) );
		}
		else
			m_pShaderParametersHandler->SetParameters( m_WorldMatrix.fValue, pParameterInfo->uAlphaIndex, 16 * sizeof(FLOAT) );
	}

	if( ALCHEMY_TEST_BIT(pParameterList->uParameterFlag, INVERSE_WORLD_MATRIX) || ( pShaderParameterList && ALCHEMY_TEST_BIT(pShaderParameterList->uParameterFlag, INVERSE_WORLD_MATRIX) ) )
	{
		if(!m_bIsInverceWorldMatrix)
		{
			m_InverceWorldMatrix   = m_WorldMatrix;
			m_bIsInverceWorldMatrix = m_InverceWorldMatrix.Invert3x4();
		}

		if( ALCHEMY_TEST_BIT(pParameterList->uParameterFlag, INVERSE_WORLD_MATRIX) )
		{
			pParameterInfo = &pParameterList->Parameters[INVERSE_WORLD_MATRIX];

			if(pParameterInfo->Type == IEffect::PROGRAM)
			{
				SetShaderMatrix(RenderSystem, IEffect::VERTEX_SHADER  , m_InverceWorldMatrix, pParameterInfo->uAlphaIndex);
				SetShaderMatrix(RenderSystem, IEffect::FRAGMENT_SHADER, m_InverceWorldMatrix, pParameterInfo->uBetaIndex );
			}
			else
				SetShaderMatrix(RenderSystem, pParameterInfo->Type, m_InverceWorldMatrix, pParameterInfo->uAlphaIndex);
		}

		if( pShaderParameterList && ALCHEMY_TEST_BIT(pShaderParameterList->uParameterFlag, INVERSE_WORLD_MATRIX) )
		{
			pParameterInfo = &pShaderParameterList->Parameters[INVERSE_WORLD_MATRIX];

			if(pParameterInfo->Type == IEffect::PROGRAM)
			{
				m_pShaderParametersHandler->SetParameters( m_InverceWorldMatrix.fValue, pParameterInfo->uAlphaIndex, 16 * sizeof(FLOAT) );
				m_pShaderParametersHandler->SetParameters( m_InverceWorldMatrix.fValue, pParameterInfo->uBetaIndex , 16 * sizeof(FLOAT) );
			}
			else
				m_pShaderParametersHandler->SetParameters( m_InverceWorldMatrix.fValue, pParameterInfo->uAlphaIndex, 16 * sizeof(FLOAT) );
		}
	}

	return true;
}

bool CRenderMethod::SetMaterial(const CMaterial& Material)
{
	if(!m_pParameterList)
	{
		ALCHEMY_DEBUG_WARNING("Render method has not been created.");

		return false;
	}

	CSceneManager& SceneManager = static_cast<CSceneManager&>( CGraphicsResource::GetGraphicsResourcePool() );
	CRenderSystem& RenderSystem = SceneManager.GetRenderSystem();

	const PARAMETERLIST* pParameterList = &m_pParameterList[m_uParameterListIndex];
	const PARAMETERINFO* pParameterInfo;

	if(pParameterList->uMaterialParameterFlag)
	{
		if( ALCHEMY_TEST_BIT(pParameterList->uMaterialParameterFlag, MATERIAL_AMBIENT) )
		{
			pParameterInfo = &pParameterList->MaterialParameters[MATERIAL_AMBIENT];

			if(pParameterInfo->Type == IEffect::PROGRAM)
			{
				RenderSystem.SetShaderParameters(IEffect::VERTEX_SHADER  , pParameterInfo->uAlphaIndex, static_cast<const FLOAT*>( Material.GetAmbient() ), 4);
				RenderSystem.SetShaderParameters(IEffect::FRAGMENT_SHADER, pParameterInfo->uBetaIndex , static_cast<const FLOAT*>( Material.GetAmbient() ), 4);
			}
			else
				RenderSystem.SetShaderParameters(pParameterInfo->Type, pParameterInfo->uAlphaIndex, static_cast<const FLOAT*>( Material.GetAmbient() ), 4);
		}

		if( ALCHEMY_TEST_BIT(pParameterList->uMaterialParameterFlag, MATERIAL_DIFFUSE) )
		{
			pParameterInfo = &pParameterList->MaterialParameters[MATERIAL_DIFFUSE];

			if(pParameterInfo->Type == IEffect::PROGRAM)
			{
				RenderSystem.SetShaderParameters(IEffect::VERTEX_SHADER  , pParameterInfo->uAlphaIndex, static_cast<const FLOAT*>( Material.GetDiffuse() ), 4);
				RenderSystem.SetShaderParameters(IEffect::FRAGMENT_SHADER, pParameterInfo->uBetaIndex , static_cast<const FLOAT*>( Material.GetDiffuse() ), 4);
			}
			else
				RenderSystem.SetShaderParameters(pParameterInfo->Type, pParameterInfo->uAlphaIndex, static_cast<const FLOAT*>( Material.GetDiffuse() ), 4);
		}

		if( ALCHEMY_TEST_BIT(pParameterList->uMaterialParameterFlag, MATERIAL_SPECULAR) )
		{
			pParameterInfo = &pParameterList->MaterialParameters[MATERIAL_SPECULAR];

			if(pParameterInfo->Type == IEffect::PROGRAM)
			{
				RenderSystem.SetShaderParameters(IEffect::VERTEX_SHADER  , pParameterInfo->uAlphaIndex, static_cast<const FLOAT*>( Material.GetSpecular() ), 4);
				RenderSystem.SetShaderParameters(IEffect::FRAGMENT_SHADER, pParameterInfo->uBetaIndex , static_cast<const FLOAT*>( Material.GetSpecular() ), 4);
			}
			else
				RenderSystem.SetShaderParameters(pParameterInfo->Type, pParameterInfo->uAlphaIndex, static_cast<const FLOAT*>( Material.GetSpecular() ), 4);
		}

		if( ALCHEMY_TEST_BIT(pParameterList->uMaterialParameterFlag, MATERIAL_EMISSIVE) )
		{
			pParameterInfo = &pParameterList->MaterialParameters[MATERIAL_EMISSIVE];

			if(pParameterInfo->Type == IEffect::PROGRAM)
			{
				RenderSystem.SetShaderParameters(IEffect::VERTEX_SHADER  , pParameterInfo->uAlphaIndex, static_cast<const FLOAT*>( Material.GetEmissive() ), 4);
				RenderSystem.SetShaderParameters(IEffect::FRAGMENT_SHADER, pParameterInfo->uBetaIndex , static_cast<const FLOAT*>( Material.GetEmissive() ), 4);
			}
			else
				RenderSystem.SetShaderParameters(pParameterInfo->Type, pParameterInfo->uAlphaIndex, static_cast<const FLOAT*>( Material.GetEmissive() ), 4);
		}

		if( ALCHEMY_TEST_BIT(pParameterList->uMaterialParameterFlag, MATERIAL_POWER) )
		{
			pParameterInfo = &pParameterList->MaterialParameters[MATERIAL_POWER];

			FLOAT fPower = Material.GetPower();

			if(pParameterInfo->Type == IEffect::PROGRAM)
			{
				RenderSystem.SetShaderParameters(IEffect::VERTEX_SHADER  , pParameterInfo->uAlphaIndex, &fPower, 1);
				RenderSystem.SetShaderParameters(IEffect::FRAGMENT_SHADER, pParameterInfo->uBetaIndex , &fPower, 1);
			}
			else
				RenderSystem.SetShaderParameters(pParameterInfo->Type, pParameterInfo->uAlphaIndex, &fPower, 1);
		}

		if( ALCHEMY_TEST_BIT(pParameterList->uMaterialParameterFlag, MATERIAL_INVERSE_255_POWER) )
		{
			pParameterInfo = &pParameterList->MaterialParameters[MATERIAL_INVERSE_255_POWER];

			FLOAT fPower = ALCHEMY_BOUND(Material.GetPower(), 0.0f, 255.0f) * g_fINV_255;

			if(pParameterInfo->Type == IEffect::PROGRAM)
			{
				RenderSystem.SetShaderParameters(IEffect::VERTEX_SHADER  , pParameterInfo->uAlphaIndex, &fPower, 1);
				RenderSystem.SetShaderParameters(IEffect::FRAGMENT_SHADER, pParameterInfo->uBetaIndex , &fPower, 1);
			}
			else
				RenderSystem.SetShaderParameters(pParameterInfo->Type, pParameterInfo->uAlphaIndex, &fPower, 1);
		}
	}

	if(pParameterList->pTextures)
	{
		UINT uTextureCount = pParameterList->uTextureCount;
		if(uTextureCount)
		{
			const LPTEXTURE* ppTexture;
			if(pParameterList->pSamplerDeclaration)
			{
				const IEffect::SAMPLERDECLARATION* pSamplerDeclaration;
				
				for(UINT i = 0; i < uTextureCount; ++ i)
				{
					pParameterInfo = &pParameterList->pTextures[i];
					pSamplerDeclaration = &pParameterList->pSamplerDeclaration[i];

					if(pParameterInfo->Type == IEffect::PROGRAM)
					{
						ppTexture = Material.GetTextures().Get(pParameterInfo->uAlphaIndex);
						if(ppTexture && *ppTexture)
							RenderSystem.SetTexture(IEffect::VERTEX_SHADER  , (*ppTexture)->GetResourceHandle(), pParameterInfo->uAlphaIndex, pSamplerDeclaration);

						ppTexture = Material.GetTextures().Get(pParameterInfo->uAlphaIndex + pParameterInfo->uBetaIndex);
						if(ppTexture && *ppTexture)
							RenderSystem.SetTexture(IEffect::FRAGMENT_SHADER, (*ppTexture)->GetResourceHandle(), pParameterInfo->uBetaIndex, pSamplerDeclaration);
					}
					else
					{
						ppTexture = Material.GetTextures().Get(pParameterInfo->uAlphaIndex);
						if(ppTexture && *ppTexture)
							RenderSystem.SetTexture(pParameterInfo->Type, (*ppTexture)->GetResourceHandle(), pParameterInfo->uAlphaIndex, pSamplerDeclaration);
					}
				}
			}
			else
			{
				for(UINT i = 0; i < uTextureCount; ++ i)
				{
					pParameterInfo = &pParameterList->pTextures[i];

					if(pParameterInfo->Type == IEffect::PROGRAM)
					{
						ppTexture = Material.GetTextures().Get(pParameterInfo->uAlphaIndex);
						if(ppTexture && *ppTexture)
							RenderSystem.SetTexture(IEffect::VERTEX_SHADER  , (*ppTexture)->GetResourceHandle(), pParameterInfo->uAlphaIndex, ALCHEMY_NULL);

						ppTexture = Material.GetTextures().Get(pParameterInfo->uAlphaIndex + pParameterInfo->uBetaIndex);
						if(ppTexture && *ppTexture)
							RenderSystem.SetTexture(IEffect::FRAGMENT_SHADER, (*ppTexture)->GetResourceHandle(), pParameterInfo->uBetaIndex, ALCHEMY_NULL);
					}
					else
					{
						ppTexture = Material.GetTextures().Get(pParameterInfo->uAlphaIndex);
						if(ppTexture && *ppTexture)
							RenderSystem.SetTexture(pParameterInfo->Type, (*ppTexture)->GetResourceHandle(), pParameterInfo->uAlphaIndex, ALCHEMY_NULL);
					}
				}
			}
		}
	}

	pParameterList = m_pShaderParametersHandler ? m_pShaderParametersHandler->GetParameterlist(m_uParameterListIndex) : ALCHEMY_NULL;
	if(pParameterList)
	{
		if(pParameterList->uMaterialParameterFlag)
		{
			if( ALCHEMY_TEST_BIT(pParameterList->uMaterialParameterFlag, MATERIAL_AMBIENT) )
			{
				pParameterInfo = &pParameterList->MaterialParameters[MATERIAL_AMBIENT];

				if(pParameterInfo->Type == IEffect::PROGRAM)
				{
					m_pShaderParametersHandler->SetParameters( static_cast<const FLOAT*>( Material.GetAmbient() ), pParameterInfo->uAlphaIndex, 4 * sizeof(FLOAT) );
					m_pShaderParametersHandler->SetParameters( static_cast<const FLOAT*>( Material.GetAmbient() ), pParameterInfo->uBetaIndex , 4 * sizeof(FLOAT) );
				}
				else
					m_pShaderParametersHandler->SetParameters( static_cast<const FLOAT*>( Material.GetAmbient() ), pParameterInfo->uAlphaIndex, 4 * sizeof(FLOAT) );
			}

			if( ALCHEMY_TEST_BIT(pParameterList->uMaterialParameterFlag, MATERIAL_DIFFUSE) )
			{
				pParameterInfo = &pParameterList->MaterialParameters[MATERIAL_DIFFUSE];

				if(pParameterInfo->Type == IEffect::PROGRAM)
				{
					m_pShaderParametersHandler->SetParameters(static_cast<const FLOAT*>( Material.GetDiffuse() ), pParameterInfo->uAlphaIndex, 4 * sizeof(FLOAT) );
					m_pShaderParametersHandler->SetParameters(static_cast<const FLOAT*>( Material.GetDiffuse() ), pParameterInfo->uBetaIndex , 4 * sizeof(FLOAT) );
				}
				else
					m_pShaderParametersHandler->SetParameters(static_cast<const FLOAT*>( Material.GetDiffuse() ), pParameterInfo->uAlphaIndex, 4 * sizeof(FLOAT) );
			}

			if( ALCHEMY_TEST_BIT(pParameterList->uMaterialParameterFlag, MATERIAL_SPECULAR) )
			{
				pParameterInfo = &pParameterList->MaterialParameters[MATERIAL_SPECULAR];

				if(pParameterInfo->Type == IEffect::PROGRAM)
				{
					m_pShaderParametersHandler->SetParameters(static_cast<const FLOAT*>( Material.GetSpecular() ), pParameterInfo->uAlphaIndex, 4 * sizeof(FLOAT) );
					m_pShaderParametersHandler->SetParameters(static_cast<const FLOAT*>( Material.GetSpecular() ), pParameterInfo->uBetaIndex , 4 * sizeof(FLOAT) );
				}
				else
					m_pShaderParametersHandler->SetParameters(static_cast<const FLOAT*>( Material.GetSpecular() ), pParameterInfo->uAlphaIndex, 4 * sizeof(FLOAT) );
			}

			if( ALCHEMY_TEST_BIT(pParameterList->uMaterialParameterFlag, MATERIAL_EMISSIVE) )
			{
				pParameterInfo = &pParameterList->MaterialParameters[MATERIAL_EMISSIVE];

				if(pParameterInfo->Type == IEffect::PROGRAM)
				{
					m_pShaderParametersHandler->SetParameters( static_cast<const FLOAT*>( Material.GetEmissive() ), pParameterInfo->uAlphaIndex, 4 * sizeof(FLOAT) );
					m_pShaderParametersHandler->SetParameters( static_cast<const FLOAT*>( Material.GetEmissive() ), pParameterInfo->uBetaIndex , 4 * sizeof(FLOAT) );
				}
				else
					m_pShaderParametersHandler->SetParameters( static_cast<const FLOAT*>( Material.GetEmissive() ), pParameterInfo->uAlphaIndex, 4 * sizeof(FLOAT) );
			}

			if( ALCHEMY_TEST_BIT(pParameterList->uMaterialParameterFlag, MATERIAL_POWER) )
			{
				pParameterInfo = &pParameterList->MaterialParameters[MATERIAL_POWER];

				FLOAT fPower = Material.GetPower();

				if(pParameterInfo->Type == IEffect::PROGRAM)
				{
					m_pShaderParametersHandler->SetParameters( &fPower, pParameterInfo->uAlphaIndex, sizeof(FLOAT) );
					m_pShaderParametersHandler->SetParameters( &fPower, pParameterInfo->uBetaIndex , sizeof(FLOAT) );
				}
				else
					m_pShaderParametersHandler->SetParameters( &fPower, pParameterInfo->uAlphaIndex, sizeof(FLOAT) );
			}

			if( ALCHEMY_TEST_BIT(pParameterList->uMaterialParameterFlag, MATERIAL_INVERSE_255_POWER) )
			{
				pParameterInfo = &pParameterList->MaterialParameters[MATERIAL_INVERSE_255_POWER];

				FLOAT fPower = ALCHEMY_BOUND(Material.GetPower(), 0.0f, 255.0f) * g_fINV_255;

				if(pParameterInfo->Type == IEffect::PROGRAM)
				{
					m_pShaderParametersHandler->SetParameters( &fPower, pParameterInfo->uAlphaIndex, sizeof(FLOAT) );
					m_pShaderParametersHandler->SetParameters( &fPower, pParameterInfo->uBetaIndex , sizeof(FLOAT) );
				}
				else
					m_pShaderParametersHandler->SetParameters( &fPower, pParameterInfo->uAlphaIndex, sizeof(FLOAT) );
			}
		}
	}

	return true;
}