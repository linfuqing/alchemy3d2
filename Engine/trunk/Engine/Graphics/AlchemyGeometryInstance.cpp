#include "AlchemyGeometryInstance.h"
#include "AlchemySceneManager.h"
#include "AlchemyMeshProcessorInterface.h"

using namespace alchemy;

CGeometryInstance::CGeometryInstance(void) :
m_fInstanceCount(0.0),
m_pfTransform(ALCHEMY_NULL), 
m_uTransformStride(0), 
m_uTransformCount(0), 
m_pTransformInfos(ALCHEMY_NULL), 
m_uTransformInfoCount(0), 
m_fHeadInstanceCount(0.0), 
m_fTailInstanceCount(0.0), 
m_pSourceMesh(ALCHEMY_NULL), 
m_pHeadMesh(ALCHEMY_NULL), 
m_pTailMesh(ALCHEMY_NULL), 
m_pTailMeshProcessor(ALCHEMY_NULL),
m_puTransformOffsetTable(ALCHEMY_NULL)
{
}

CGeometryInstance::~CGeometryInstance(void)
{
	ALCHEMY_DEBUG_DELETE_ARRAY(m_pfTransform);
	ALCHEMY_DEBUG_DELETE_ARRAY(m_pTransformInfos);
}

bool CGeometryInstance::SetInstances(FLOAT fInstanceCount, UINT uTransformCount, const TRANSFORMINFO* pTransformInfos, UINT uTransformInfoCount)
{
	m_fInstanceCount = fInstanceCount;

	ALCHEMY_DEBUG_DELETE_ARRAY(m_pfTransform);

	m_uTransformCount = uTransformCount;

	ALCHEMY_DEBUG_DELETE_ARRAY(m_puTransformOffsetTable);
	ALCHEMY_DEBUG_DELETE_ARRAY(m_pTransformInfos);
	if(pTransformInfos && uTransformInfoCount)
	{
		ALCHEMY_DEBUG_NEW(m_pTransformInfos, TRANSFORMINFO[uTransformInfoCount]);
		ALCHEMY_MEMORY_COPY( m_pTransformInfos, pTransformInfos, uTransformInfoCount * sizeof(TRANSFORMINFO) );

		m_uTransformInfoCount = uTransformInfoCount;

		__BuildTransformOffsetTable();

		UINT uInstanceCount = ALCHEMY_CEIL(fInstanceCount);
		ALCHEMY_DEBUG_NEW(m_pfTransform, FLOAT[uInstanceCount * m_uTransformStride]);

		UINT uOffset = 0, i, j;
		for(j = 0; j < uInstanceCount; ++ j)
		{
			for(i = 0; i < uTransformInfoCount; ++ i)
			{
				switch(pTransformInfos[i].Usage)
				{
				case POSITION_X:
				case POSITION_Y:
				case POSITION_Z:
					m_pfTransform[uOffset ++] = 0.0;
					break;

				case SCALE_X:
				case SCALE_Y:
				case SCALE_Z:
					m_pfTransform[uOffset ++] = 1.0;
					break;

				case ROTATION_X:
				case ROTATION_Y:
				case ROTATION_Z:
					m_pfTransform[uOffset ++] = 1.0;
					break;

				case QUATERNION:
					m_pfTransform[uOffset ++] = 0.0;
					m_pfTransform[uOffset ++] = 0.0;
					m_pfTransform[uOffset ++] = 0.0;
					m_pfTransform[uOffset ++] = 0.0;
					break;

				case MATRIX:
					m_pfTransform[uOffset ++] = 1.0;
					m_pfTransform[uOffset ++] = 0.0;
					m_pfTransform[uOffset ++] = 0.0;
					m_pfTransform[uOffset ++] = 0.0;

					m_pfTransform[uOffset ++] = 0.0;
					m_pfTransform[uOffset ++] = 1.0;
					m_pfTransform[uOffset ++] = 0.0;
					m_pfTransform[uOffset ++] = 0.0;

					m_pfTransform[uOffset ++] = 0.0;
					m_pfTransform[uOffset ++] = 0.0;
					m_pfTransform[uOffset ++] = 1.0;
					m_pfTransform[uOffset ++] = 0.0;
					break;

				default:
					break;
				}
			}
		}
	}
	else
		m_uTransformInfoCount = 0;

	if(m_pSourceMesh)
		return SetMesh( m_pSourceMesh->GetResourceHandle() );

	m_KeptValues.Clear();

	return true;
}

bool CGeometryInstance::SetTransform(UINT uTransformIndex, UINT uTransformInfoIndex, const FLOAT* pfValues, bool bIsKeep)
{
	if(m_pfTransform && uTransformIndex < static_cast<UINT>( ALCHEMY_CEIL(m_fInstanceCount) ) && m_pTransformInfos && uTransformInfoIndex < m_uTransformInfoCount)
	{
		if(bIsKeep)
		{
			KEPTVALUE KeptValue;
			KeptValue.uTransformIndex = uTransformIndex;
			KeptValue.uTransformInfoIndex = uTransformInfoIndex;
			KeptValue.pfValues = pfValues;

			m_KeptValues.Add(KeptValue);
		}
		else
		{
			if(!m_puTransformOffsetTable)
				__BuildTransformOffsetTable();

			UINT uIndex = uTransformIndex * m_uTransformStride + m_puTransformOffsetTable[uTransformInfoIndex];

			switch(m_pTransformInfos[uTransformInfoIndex].Usage)
			{
			case POSITION_X:
			case POSITION_Y:
			case POSITION_Z:
			case SCALE_X:
			case SCALE_Y:
			case SCALE_Z:
			case ROTATION_X:
			case ROTATION_Y:
			case ROTATION_Z:
				m_pfTransform[uIndex] = *pfValues;
				break;

			case QUATERNION:
				m_pfTransform[uIndex ++] = pfValues[0];
				m_pfTransform[uIndex ++] = pfValues[1];
				m_pfTransform[uIndex ++] = pfValues[2];
				m_pfTransform[uIndex ++] = pfValues[3];
				break;

			case MATRIX:
				m_pfTransform[uIndex ++] = pfValues[0];
				m_pfTransform[uIndex ++] = pfValues[1];
				m_pfTransform[uIndex ++] = pfValues[2];
				m_pfTransform[uIndex ++] = pfValues[3];

				m_pfTransform[uIndex ++] = pfValues[4];
				m_pfTransform[uIndex ++] = pfValues[5];
				m_pfTransform[uIndex ++] = pfValues[6];
				m_pfTransform[uIndex ++] = pfValues[7];

				m_pfTransform[uIndex ++] = pfValues[8];
				m_pfTransform[uIndex ++] = pfValues[9];
				m_pfTransform[uIndex ++] = pfValues[10];
				m_pfTransform[uIndex ++] = pfValues[11];
				break;

			default:
				break;
			}
		}

		return true;
	}

	return false;
}


bool CGeometryInstance::ApplyForRender(UINT uRenderPass, UINT uRenderList)
{
	CSceneManager* pSceneManager = GetSceneManager();
	CEffect* pEffect = GetEffect();
	if(!pSceneManager || !pEffect || !m_pHeadMesh)
		return false;

	CRenderQueue& RenderQueue = pSceneManager->GetRenderQueue();
	CRenderQueue::RENDERENTRY& RenderEntry = RenderQueue.LockRenderEntry( *this, &m_bIVisible, GetBoundingVolume() );

	RenderEntry.uRenderTarget	   = 0;
	RenderEntry.uEffect            = pEffect->GetResourceHandle();
	RenderEntry.uRenderPass        = uRenderPass == ~0 ? GetRenderPass() : uRenderPass;
	RenderEntry.uVertexDeclaration = m_pHeadMesh->GetVertex().GetVertexDeclaration().GetResourceHandle();
	RenderEntry.uModel             = m_pHeadMesh->GetResourceHandle();
	RenderEntry.uModelParameter    = 0;
	RenderEntry.uUserData          = 0;

	pSceneManager->GetRenderQueue().UnlockRenderEntry(uRenderList);

	return true;
}

void CGeometryInstance::Render(const CRenderQueue::RENDERENTRY& Entry, const CRenderQueue::RENDERKEY& Key, UINT uFlag)
{
	CEffect* pEffect = GetEffect();
	IRenderMethod* pRenderMethod = GetRenderMethod();
	CSceneManager* pSceneManager = GetSceneManager();
	if(!pSceneManager || !pEffect || !pRenderMethod || !m_pHeadMesh)
		return;

	CRenderQueue& RenderQueue = pSceneManager->GetRenderQueue();
	CRenderSystem& RenderSystem = pSceneManager->GetRenderSystem();

	CMaterial* pMaterial = GetMaterial();
	if(pMaterial)
	{
		UINT uTextureCount;
		const UINT* pTextureAlphaIndexes = pRenderMethod->GetTextureInfo(Key.uRenderPass, Key.uRenderTargetIndex, uTextureCount);
		if(pTextureAlphaIndexes)
		{
			const LPTEXTURE* ppTexture;
			for(UINT i = 0; i < uTextureCount; ++ i)
			{
				ppTexture = pMaterial->GetTextures().Get( pTextureAlphaIndexes[i] );

				if( ppTexture && *ppTexture && ALCHEMY_TEST_BIT( (*ppTexture)->GetUsage(), CTexture::RENDER_TARGET ) && (*ppTexture)->GetResourceHandle() == RenderQueue.GetRenderTarget() )
					return;
			}
		}
	}

	if( ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_RENDER_TARGET) )
	{
		if(Entry.uRenderTarget)
			pSceneManager->GetRenderQueue().SetRenderTarget(Entry.uRenderTarget, 0, false);
		else
			pSceneManager->GetRenderQueue().SetRenderTarget();

		RenderQueue.ActivateMultipleRenderTarget( Key.uRenderTargetIndex, pEffect->GetRenderTargetCount(Key.uRenderPass) );
	}

	if(ALCHEMY_TEST_BIT(uFlag, CRenderQueue::PARENT) || ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_EFFECT) || ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_EFFECT_PASS) )
	{
		if( ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_EFFECT) && Entry.uEffect )
			RenderSystem.SetShader(Entry.uEffect);

		if( ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_EFFECT_PASS) && Entry.uRenderPass )
			RenderSystem.SetShader(Entry.uRenderPass);
	}

	if(!pRenderMethod->Activate(Key.uRenderPass, Key.uRenderTargetIndex) )
		return;

	RenderSystem.ResetTexture();

	RenderSystem.RegisterRenderState();

	if( ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_VERTEX_DECLARATION) && ( !Entry.uVertexDeclaration || !RenderSystem.SetVertexDeclaration(Entry.uVertexDeclaration) ) )
		return;

	if(!m_puTransformOffsetTable)
		__BuildTransformOffsetTable();

	CVector<KEPTVALUE>::CIterator Iterator = m_KeptValues.GetIterator(0);
	LPKEPTVALUE pKeptValue;
	UINT uOffset;
	while( CVector<KEPTVALUE>::IsValuable(Iterator) )
	{
		pKeptValue = &(*Iterator);
		if(pKeptValue->uTransformIndex < m_uTransformCount && pKeptValue->uTransformInfoIndex < m_uTransformInfoCount)
		{
			uOffset = pKeptValue->uTransformIndex * m_uTransformStride + m_puTransformOffsetTable[pKeptValue->uTransformInfoIndex];
			switch(m_pTransformInfos[pKeptValue->uTransformInfoIndex].Usage)
			{
			case POSITION_X:
			case POSITION_Y:
			case POSITION_Z:
			case SCALE_X:
			case SCALE_Y:
			case SCALE_Z:
			case ROTATION_X:
			case ROTATION_Y:
			case ROTATION_Z:
				*(m_pfTransform + uOffset) = *pKeptValue->pfValues;
				break;

			case QUATERNION:
				*( m_pfTransform + (uOffset ++) ) = pKeptValue->pfValues[0];
				*( m_pfTransform + (uOffset ++) ) = pKeptValue->pfValues[1];
				*( m_pfTransform + (uOffset ++) ) = pKeptValue->pfValues[2];
				*( m_pfTransform + (uOffset ++) ) = pKeptValue->pfValues[3];
				break;

			case MATRIX:
				*( m_pfTransform + (uOffset ++) ) = pKeptValue->pfValues[0];
				*( m_pfTransform + (uOffset ++) ) = pKeptValue->pfValues[1];
				*( m_pfTransform + (uOffset ++) ) = pKeptValue->pfValues[2];
				*( m_pfTransform + (uOffset ++) ) = pKeptValue->pfValues[3];

				*( m_pfTransform + (uOffset ++) ) = pKeptValue->pfValues[4];
				*( m_pfTransform + (uOffset ++) ) = pKeptValue->pfValues[5];
				*( m_pfTransform + (uOffset ++) ) = pKeptValue->pfValues[6];
				*( m_pfTransform + (uOffset ++) ) = pKeptValue->pfValues[7];

				*( m_pfTransform + (uOffset ++) ) = pKeptValue->pfValues[8];
				*( m_pfTransform + (uOffset ++) ) = pKeptValue->pfValues[9];
				*( m_pfTransform + (uOffset ++) ) = pKeptValue->pfValues[10];
				*( m_pfTransform + (uOffset ++) ) = pKeptValue->pfValues[11];

				break;

			default:
				break;
			}
		}

		++ Iterator;
	}

	UINT uTransformCount = ALCHEMY_CEIL(m_fHeadInstanceCount), uTransformArrayCount = uTransformCount * m_uTransformStride, uTransformArrayOffset = 0;

	pRenderMethod->CommitChange( &GetWorldMatrix(), pMaterial);
	pRenderMethod->SetArray(IRenderMethod::INSTANCE_TRANSFORM_ARRAY, m_pfTransform, m_uTransformStride, 1, m_uTransformStride, m_uTransformStride, uTransformCount, false);

	uTransformArrayOffset += uTransformArrayCount;

	UINT uRenderFlag = ALCHEMY_CLEAR_BIT(uFlag, CRenderQueue::ENTRY_VERTEX_DECLARATION);

	IMeshProcessor* pMeshProcessor = GetMeshProcessor();
	if(pMeshProcessor)
		pMeshProcessor->OnProcess(*pRenderMethod, Entry.uVertexDeclaration ? uRenderFlag : uFlag);
	else
		m_pHeadMesh->DrawSubset(Entry.uModelParameter, ALCHEMY_TEST_BIT(uFlag, CRenderQueue::MODEL), ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_VERTEX_DECLARATION) && !Entry.uVertexDeclaration);

	FLOAT i;

	ALCHEMY_SET_BIT(uRenderFlag, CRenderQueue::MODEL);
	ALCHEMY_SET_BIT(uRenderFlag, CRenderQueue::MODEL_PARAMETER);

	if(pMeshProcessor)
	{
		for(i = m_fHeadInstanceCount; i < m_fInstanceCount; i += m_fHeadInstanceCount)
		{
			pRenderMethod->SetArray(IRenderMethod::INSTANCE_TRANSFORM_ARRAY, m_pfTransform + uTransformArrayOffset, m_uTransformStride, 1, m_uTransformStride, m_uTransformStride, uTransformCount, false);

			pMeshProcessor->OnProcess(*pRenderMethod, uRenderFlag);

			uTransformArrayOffset += uTransformArrayCount;
		}
	}
	else
	{
		for(i = m_fHeadInstanceCount; i < m_fInstanceCount; i += m_fHeadInstanceCount)
		{
			pRenderMethod->SetArray(IRenderMethod::INSTANCE_TRANSFORM_ARRAY, m_pfTransform + uTransformArrayOffset, m_uTransformStride, 1, m_uTransformStride, m_uTransformStride, uTransformCount, false);

			m_pHeadMesh->DrawSubset(0, true, false);

			uTransformArrayOffset += uTransformArrayCount;
		}
	}

	if(m_pTailMeshProcessor)
	{
		pRenderMethod->SetArray(IRenderMethod::INSTANCE_TRANSFORM_ARRAY, m_pfTransform + uTransformArrayOffset, m_uTransformStride, 1, m_uTransformStride, m_uTransformStride, ALCHEMY_CEIL(m_fTailInstanceCount), false);

		m_pTailMeshProcessor->OnProcess(*pRenderMethod, uRenderFlag);
	}
	else if(m_pTailMesh)
	{
		pRenderMethod->SetArray(IRenderMethod::INSTANCE_TRANSFORM_ARRAY, m_pfTransform + uTransformArrayOffset, m_uTransformStride, 1, m_uTransformStride, m_uTransformStride, ALCHEMY_CEIL(m_fTailInstanceCount), false);

		m_pTailMesh->DrawSubset(0, true, false);
	}

	RenderSystem.RestoreRenderState();
}

CMesh* CGeometryInstance::_ConvertMesh(CMesh* pSourceMesh)
{
	m_pSourceMesh = pSourceMesh;
	if(m_fInstanceCount && pSourceMesh)
	{
		LPVERTEXBUFFER pVertexBuffer = pSourceMesh->GetVertex().GetVertexBuffer(0);
		if(pVertexBuffer)
		{
			const CRenderSystem::RENDERSYSTEMINFO& RenderSystemInfo = static_cast<CSceneManager&>( pSourceMesh->GetGraphicsResourcePool() ).GetRenderSystem().GetRenderSystemInfo();

			FLOAT fInstanceCount = ALCHEMY_MININUM( m_fInstanceCount, RenderSystemInfo.uMaxVertexCount * 1.0f / pVertexBuffer->GetMemberCount() );

			INDEXBUFFER& IndexBuffer = pSourceMesh->GetIndexBuffer();

			fInstanceCount = ALCHEMY_MININUM( fInstanceCount, RenderSystemInfo.uMaxVertexIndexCount * 1.0f / IndexBuffer.GetMemberCount() );
			fInstanceCount = ALCHEMY_MININUM( fInstanceCount, static_cast<FLOAT>(m_uTransformCount) );

			if(fInstanceCount)
			{
				m_fHeadInstanceCount = fInstanceCount;

				const CMesh::ATTRIBUTERANGE* pAttributeRange = pSourceMesh->GetAttributeTable().Get( GetMeshSubset() );
				if(!pAttributeRange)
					return ALCHEMY_NULL;

				CMesh* pMesh = ALCHEMY_NULL;
				if(pAttributeRange->uStartIndex == 0 && pSourceMesh->GetIndexBuffer().GetMemberCount() == pAttributeRange->uIndexCount)
					pMesh = pSourceMesh->ConvertToInstancedMesh(fInstanceCount);
				else
				{
					pSourceMesh = pSourceMesh->Clone(pAttributeRange, 1);

					if(pSourceMesh)
					{
						pMesh = pSourceMesh->ConvertToInstancedMesh(fInstanceCount);

						pSourceMesh->Release();
					}
				}

				if(m_pHeadMesh)
					m_pHeadMesh->Release();

				IMeshProcessor* pMeshProcessor = GetMeshProcessor();
				if(pMesh && pMeshProcessor)
				{
					CMesh* pDestinationMesh = pMeshProcessor->ConvertMesh(*pMesh, 0);

					if(pDestinationMesh)
					{
						pDestinationMesh->AddRef();
						pMesh->Release();

						pMesh = pDestinationMesh;
					}
				}

				m_pHeadMesh = pMesh;

				FLOAT i;
				for(i = m_fInstanceCount; i >= fInstanceCount; i -= fInstanceCount);

				m_fTailInstanceCount = i;
				if(m_fTailInstanceCount)
				{
					if(m_pTailMesh)
						m_pTailMesh->Release();

					m_pTailMesh = pSourceMesh->ConvertToInstancedMesh(m_fTailInstanceCount);

					if(m_pTailMeshProcessor)
						m_pTailMeshProcessor->Release();

					if(pMeshProcessor)
					{
						m_pTailMeshProcessor = pMeshProcessor->Clone();
						if(m_pTailMeshProcessor)
						{
							CMesh* pDestinationMesh = m_pTailMeshProcessor->ConvertMesh(*m_pTailMesh, 0);

							if(pDestinationMesh)
							{
								pDestinationMesh->AddRef();
								m_pTailMesh->Release();

								m_pTailMesh = pDestinationMesh;
							}
						}
					}
					else
						m_pTailMeshProcessor = ALCHEMY_NULL;
				}
			}
		}
	}

	return pSourceMesh;
}

void CGeometryInstance::__BuildTransformOffsetTable()
{
	ALCHEMY_DEBUG_NEW(m_puTransformOffsetTable, UINT[m_uTransformInfoCount]);

	m_uTransformStride = 0;

	UINT i;
	for(i = 0; i < m_uTransformInfoCount; ++ i)
	{
		m_puTransformOffsetTable[i] = m_uTransformStride;

		switch(m_pTransformInfos[i].Usage)
		{
		case POSITION_X:
		case POSITION_Y:
		case POSITION_Z:
		case SCALE_X:
		case SCALE_Y:
		case SCALE_Z:
		case ROTATION_X:
		case ROTATION_Y:
		case ROTATION_Z:
			++ m_uTransformStride;
			break;

		case QUATERNION:
			m_uTransformStride += 4;
			break;

		case MATRIX:
			m_uTransformStride += 12;
			break;

		default:
			break;
		}
	}
}