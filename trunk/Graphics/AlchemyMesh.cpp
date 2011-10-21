#include "AlchemyMesh.h"
#include "AlchemySceneManager.h"
#include "AlchemyGraphicsResourcePool.h"
#include "AlchemySkinInfo.h"

#include "../Core/AlchemyUtils.h"

using namespace alchemy;
CMesh::CMesh(void) :
m_pVertex(ALCHEMY_NULL),
m_pIndexBuffer(ALCHEMY_NULL)
{
}

CMesh::~CMesh(void)
{
}

void CMesh::Destroy()
{
	m_AttributeTable.Clear();

	if(m_pIndexBuffer)
	{
		_Release(*m_pIndexBuffer);

		m_pIndexBuffer = ALCHEMY_NULL;
	}

	if(m_pVertex)
	{
		_Release(*m_pVertex);

		m_pVertex = ALCHEMY_NULL;
	}
}

//bool CMesh::Create(UINT uNumVertices, UINT uNumIndices, UINT uIndexUsage, CIndexBuffer::FORMAT IndexFormat, const CVertexDeclaration::VERTEXELEMENT* pVertexElement, UINT uElementCount)
//{
//	if( !m_Vertex.Create(uNumVertices, pVertexElement, uElementCount) )
//		return false;
//
//	m_pIndexBuffer = ALCHEMY_RENDER_SYSTEM.CreateIndexBuffer(uNumIndices, uIndexUsage, IndexFormat);
//
//	if(m_pIndexBuffer)
//		return true;
//
//	m_Vertex.Destroy();
//
//	return false;
//}

CMesh* CMesh::Clone(const CVertexDeclaration::VERTEXELEMENT* pCloneElements, UINT uCloneElementCount, const UINT* puVertexUsage, bool bIsCloneIndices, UINT IndexUsage)
{
	if(!m_pVertex)
	{
		ALCHEMY_DEBUG_WARNING("CMesh::ConvertToBlendedMesh: Error mesh.");

		return ALCHEMY_NULL;
	}

	CMesh* pOutput = static_cast<CMesh*>( CHighLevelGraphicsResource::_Clone() );

	if(pOutput)
	{
		pOutput->m_AttributeTable = m_AttributeTable;

		pOutput->m_pVertex = m_pVertex->Clone(pCloneElements, uCloneElementCount, puVertexUsage);

		if(!pOutput->m_pVertex)
		{
			GetGraphicsResourcePool().DestroyResource(*pOutput);

			ALCHEMY_DEBUG_WARNING("CMesh::Clone: Vertex is created fail.");

			return ALCHEMY_NULL;
		}

		if(bIsCloneIndices)
		{
			if(m_pIndexBuffer)
			{
				PVOID pIndices = m_pIndexBuffer->Lock( ALCHEMY_FLAG(INDEXBUFFER::READ_ONLY) );
				pOutput->m_pIndexBuffer = static_cast<CSceneManager&>( GetGraphicsResourcePool() ).GetRenderSystem().CreateIndexBuffer(
					m_pIndexBuffer->GetMemberCount(), IndexUsage, m_pIndexBuffer->GetFormat(), pIndices);
				m_pIndexBuffer->Unlock();

				if(!pOutput->m_pIndexBuffer)
				{
					GetGraphicsResourcePool().DestroyResource(*pOutput);

					return ALCHEMY_NULL;
				}
			}
		}
		else
		{
			pOutput->m_pIndexBuffer = m_pIndexBuffer;

			m_pIndexBuffer->AddRef();
		}

		if( pOutput->_Create() )
			return pOutput;

		GetGraphicsResourcePool().DestroyResource(*pOutput);
	}

	return ALCHEMY_NULL;
}

CMesh* CMesh::Clone(const UINT* puCloneStream, UINT uCloneStreamCount, const UINT* puVertexUsage, bool bIsCloneIndices, UINT IndexUsage)
{
	if(!m_pVertex)
	{
		ALCHEMY_DEBUG_WARNING("CMesh::ConvertToBlendedMesh: Error mesh.");

		return ALCHEMY_NULL;
	}

	CMesh* pOutput = static_cast<CMesh*>( CHighLevelGraphicsResource::_Clone() );

	if(pOutput)
	{
		pOutput->m_AttributeTable = m_AttributeTable;

		pOutput->m_pVertex = m_pVertex->Clone(puCloneStream, uCloneStreamCount, puVertexUsage);

		if(!pOutput->m_pVertex)
		{
			GetGraphicsResourcePool().DestroyResource(*pOutput);

			ALCHEMY_DEBUG_WARNING("CMesh::Clone: Vertex is created fail.");

			return ALCHEMY_NULL;
		}

		if(bIsCloneIndices)
		{
			if(m_pIndexBuffer)
			{
				PVOID pIndices = m_pIndexBuffer->Lock( ALCHEMY_FLAG(INDEXBUFFER::READ_ONLY) );
				pOutput->m_pIndexBuffer = static_cast<CSceneManager&>( GetGraphicsResourcePool() ).GetRenderSystem().CreateIndexBuffer(
					m_pIndexBuffer->GetMemberCount(), IndexUsage, m_pIndexBuffer->GetFormat(), pIndices);
				m_pIndexBuffer->Unlock();

				if(!pOutput->m_pIndexBuffer)
				{
					GetGraphicsResourcePool().DestroyResource(*pOutput);

					return ALCHEMY_NULL;
				}
			}
		}
		else
		{
			pOutput->m_pIndexBuffer = m_pIndexBuffer;

			m_pIndexBuffer->AddRef();
		}

		if( pOutput->_Create() )
			return pOutput;

		GetGraphicsResourcePool().DestroyResource(*pOutput);
	}

	return ALCHEMY_NULL;
}

bool CMesh::GenerateAdjacency(FLOAT Epsilon, UINT* pAdjacency)
{
	if(pAdjacency == ALCHEMY_NULL)
		return false;

	CVertexDeclaration& VertexDeclaration = m_pVertex->GetVertexDeclaration();
	const CVertexDeclaration::VERTEXELEMENT* pSourceVertexElement = VertexDeclaration.GetVertexElements();
	UINT uSourceVertexElementCount = VertexDeclaration.GetVertexElementCount(), uCurrentElement;
	LPVERTEXBUFFER pPosBuffer;
	

	uCurrentElement = CVertexDeclaration::FindVertexElement(CVertexDeclaration::POSITION, CVertexDeclaration::FLOAT4, 0, pSourceVertexElement, uSourceVertexElementCount);
	if(uCurrentElement < uSourceVertexElementCount)
		pPosBuffer = m_pVertex->GetVertexBuffer( pSourceVertexElement[uCurrentElement].uStream );
	else
		return false;

	if(pPosBuffer == ALCHEMY_NULL)
		return false;

	UINT uFormat = m_pIndexBuffer->GetFormat();
	UINT uPosOffset = pSourceVertexElement[ uCurrentElement ].uOffset;
	UINT uIndexDataStride = uFormat == INDEXBUFFER::BIT16 ? 2 : 4;
	UINT uPosDataStride = pPosBuffer->GetStride();

	UINT uIndicesCount = m_pIndexBuffer->GetMemberCount();
	PUINT8 pIndexData, pPosData;

	for(UINT i=0; i<uIndicesCount; ++i)
		pAdjacency[i] = -1;

	pPosData = pPosBuffer->Lock( ALCHEMY_FLAG(VERTEXBUFFER::READ_ONLY) );
	pIndexData = m_pIndexBuffer->Lock( ALCHEMY_FLAG(INDEXBUFFER::READ_ONLY) );

	if(!pPosData || !pIndexData)
		return false;

	UINT uFaceCount = uIndicesCount/3;

	UINT uCurrentIndices[3];
	UINT uCompareIndices[3];
	LPFLOAT3 pCurrentPosition[3], pComparePosition[3];
	FLOAT fDistance;
	int nSucess = 0, nSucessIndex = 0;;
	UINT uEdgeIndexArray[2], uCompareEdgeIndexArray[2];

	for(UINT i=0; i<uFaceCount-1; ++i)
	{
		for (UINT j=0; j<3; ++j)
		{
			uCurrentIndices[j] = (uIndexDataStride == 2 ?  static_cast<UINT>(* reinterpret_cast<PUINT16>( pIndexData + (i*3 + j)*uIndexDataStride ) ) :  static_cast<UINT>(* reinterpret_cast<PUINT>( pIndexData + (i*3 + j)*uIndexDataStride ) ) );
			pCurrentPosition[j] = (LPFLOAT3) ( pPosData + uCurrentIndices[j]*uPosDataStride + uPosOffset );
		}
		for (UINT uCurEdgeIndex = 0; uCurEdgeIndex < 3; ++uCurEdgeIndex)
		{
			if ( pAdjacency[ i*3 + uCurEdgeIndex ] != -1 )
				continue;

			switch(uCurEdgeIndex)
			{
			case 0:
				uEdgeIndexArray[0] = 0;
				uEdgeIndexArray[1] = 1;
				break;
			case 1:
				uEdgeIndexArray[0] = 1;
				uEdgeIndexArray[1] = 2;
				break;
			case 2:
				uEdgeIndexArray[0] = 2;
				uEdgeIndexArray[1] = 0;
				break;
			}

			for (UINT j=i+1; j<uFaceCount; ++j)
			{
				for (UINT c=0; c<3; ++c)
				{
					uCompareIndices[c] = (uIndexDataStride == 2 ?  static_cast<UINT>(* reinterpret_cast<PUINT16>( pIndexData + (j*3 + c)*uIndexDataStride ) ) :  static_cast<UINT>(* reinterpret_cast<PUINT>( pIndexData + (j*3 + c)*uIndexDataStride ) ) );
					pComparePosition[c] = (LPFLOAT3) ( pPosData + uCompareIndices[c]*uPosDataStride + uPosOffset );
				}

				for (UINT c=0; c<3; ++c)
				{
					nSucess = 0;
					nSucessIndex = -1;
					switch(c)
					{
					case 0:
						uCompareEdgeIndexArray[0] = 1;
						uCompareEdgeIndexArray[1] = 0;
						break;
					case 1:
						uCompareEdgeIndexArray[0] = 2;
						uCompareEdgeIndexArray[1] = 1;
						break;
					case 2:
						uCompareEdgeIndexArray[0] = 0;
						uCompareEdgeIndexArray[1] = 2;
						break;
					}

					for (UINT uEdgeIndex = 0; uEdgeIndex < 2; ++uEdgeIndex)
					{
						fDistance = CVector3::Distance( *pCurrentPosition[ uEdgeIndexArray[uEdgeIndex] ], *pComparePosition[ uCompareEdgeIndexArray[uEdgeIndex] ] );

						if(fDistance <= Epsilon)
							++nSucess;
					}

					if(nSucess == 2)
					{
						nSucessIndex = c;
						break;
					}
				}
				
				if(nSucess == 2)
				{
					pAdjacency[ i*3 + uCurEdgeIndex ] = j;
					pAdjacency[ j*3 + nSucessIndex] = i;
					break;
				}
			}
		}
	}

	return true;
}

CMesh* CMesh::ConvertToIndexedMesh()
{
	if(!m_pVertex || !m_pIndexBuffer)
	{
		ALCHEMY_DEBUG_WARNING("CMesh::ConvertToBlendedMesh: Error mesh.");

		return ALCHEMY_NULL;
	}

	CMesh* pOutput = static_cast<CMesh*>( CHighLevelGraphicsResource::_Clone() );

	if(pOutput)
	{
		CVertexDeclaration& VertexDeclaration = m_pVertex->GetVertexDeclaration();
		UINT uStreamCount = VertexDeclaration.GetStreamCount();

		PUINT puUsage;
		ALCHEMY_DEBUG_NEW(puUsage, UINT[uStreamCount]);

		CSceneManager& SceneManager = static_cast<CSceneManager&>( GetGraphicsResourcePool() );

		UINT i;
		LPVERTEXBUFFER pVertexBuffer;
		for(i = 0; i < uStreamCount; ++ i)
		{
			pVertexBuffer = m_pVertex->GetVertexBuffer(i);
			if(pVertexBuffer)
				puUsage[i] = pVertexBuffer->GetUsage();
			else
			{
				ALCHEMY_DEBUG_WARNING("CMesh::ConvertToIndexedMesh: Error Vertex.");

				ALCHEMY_DEBUG_DELETE_ARRAY(puUsage);

				SceneManager.DestroyResource(*pOutput);

				return ALCHEMY_NULL;
			}
		}

		UINT uNumIndices = m_pIndexBuffer->GetMemberCount();
		pOutput->m_pVertex = SceneManager.CreateVertex(uNumIndices, VertexDeclaration.GetResourceHandle(), puUsage);
		ALCHEMY_DEBUG_DELETE_ARRAY(puUsage);

		if(!pOutput->m_pVertex)
		{
			ALCHEMY_DEBUG_WARNING("CMesh::ConvertToIndexedMesh: Vertex has been converted fail.");

			static_cast<CSceneManager&>( GetGraphicsResourcePool() ).DestroyResource(*pOutput);

			return ALCHEMY_NULL;
		}

		CRenderSystem& RenderSystem = SceneManager.GetRenderSystem();

		pOutput->m_pIndexBuffer = RenderSystem.CreateIndexBuffer( m_pIndexBuffer->GetMemberCount(), m_pIndexBuffer->GetUsage(), m_pIndexBuffer->GetFormat() );

		if(!pOutput->m_pIndexBuffer)
		{
			ALCHEMY_DEBUG_WARNING("CMesh::ConvertToIndexedMesh: Index buffer has been created fail.");

			static_cast<CSceneManager&>( GetGraphicsResourcePool() ).DestroyResource(*pOutput);

			return ALCHEMY_NULL;
		}

		_AddRef(*pOutput->m_pIndexBuffer);

		void* pIndices = m_pIndexBuffer->Lock( ALCHEMY_FLAG(INDEXBUFFER::READ_ONLY) );
		LPVERTEXBUFFER pDestinationVertexBuffer, pSourceVertexBuffer;
		PUINT8 pDestinationVertices, pSourceVertices;
		UINT uStride, uIndex, j;
		if(m_pIndexBuffer->GetFormat() == CIndexBuffer::BIT16)
		{
			PUINT16 puIndices = reinterpret_cast<PUINT16>(pIndices);

			for(i = 0; i < uStreamCount; ++ i)
			{
				pSourceVertexBuffer = m_pVertex->GetVertexBuffer(i);

				if(!pSourceVertexBuffer)
				{
					ALCHEMY_DEBUG_WARNING("CMesh::ConvertToIndexedMesh: Error source vertex.");

					SceneManager.DestroyResource(*pOutput);

					return ALCHEMY_NULL;
				}

				uStride = pSourceVertexBuffer->GetStride();

				pDestinationVertexBuffer = pOutput->m_pVertex->GetVertexBuffer(i);

				if(!pDestinationVertexBuffer)
				{
					ALCHEMY_DEBUG_WARNING("CMesh::ConvertToIndexedMesh: Error desination vertex.");

					SceneManager.DestroyResource(*pOutput);

					return ALCHEMY_NULL;
				}

				pDestinationVertices = reinterpret_cast<PUINT8>( pDestinationVertexBuffer->Lock( ALCHEMY_FLAG(VERTEXBUFFER::DISCARD  ) ) );
				pSourceVertices      = reinterpret_cast<PUINT8> ( pSourceVertexBuffer     ->Lock( ALCHEMY_FLAG(VERTEXBUFFER::READ_ONLY) ) );

				uIndex = 0;
				for(j = 0; j < uNumIndices; ++ j)
				{
					ALCHEMY_MEMORY_COPY(&pDestinationVertices[uIndex], &pSourceVertices[puIndices[j] * uStride], uStride);

					uIndex += uStride;
				}

				pDestinationVertexBuffer->Unlock();
				pSourceVertexBuffer     ->Unlock();
			}

			puIndices = reinterpret_cast<PUINT16>( pOutput->m_pIndexBuffer->Lock(0) );
			for(UINT i = 0; i < uNumIndices; ++ i)
				puIndices[i] = i;
			pOutput->m_pIndexBuffer->Unlock();
		}
		else
		{
			PUINT32 puIndices = reinterpret_cast<PUINT32>(pIndices);

			for(i = 0; i < uStreamCount; ++ i)
			{
				pSourceVertexBuffer = m_pVertex->GetVertexBuffer(i);

				if(!pSourceVertexBuffer)
				{
					ALCHEMY_DEBUG_WARNING("CMesh::ConvertToIndexedMesh: Error source vertex.");

					SceneManager.DestroyResource(*pOutput);

					return ALCHEMY_NULL;
				}

				uStride = pSourceVertexBuffer->GetStride();

				pDestinationVertexBuffer = pOutput->m_pVertex->GetVertexBuffer(i);

				if(!pDestinationVertexBuffer)
				{
					ALCHEMY_DEBUG_WARNING("CMesh::ConvertToIndexedMesh: Error desination vertex.");

					SceneManager.DestroyResource(*pOutput);

					return ALCHEMY_NULL;
				}

				pDestinationVertices = reinterpret_cast<PUINT8>( pDestinationVertexBuffer->Lock( ALCHEMY_FLAG(VERTEXBUFFER::DISCARD  ) ) );
				pSourceVertices      = reinterpret_cast<PUINT8> ( pSourceVertexBuffer    ->Lock( ALCHEMY_FLAG(VERTEXBUFFER::READ_ONLY) ) );

				uIndex = 0;
				for(j = 0; j < uNumIndices; ++ j)
				{
					ALCHEMY_MEMORY_COPY(&pDestinationVertices[uIndex], &pSourceVertices[puIndices[j] * uStride], uStride);

					uIndex += uStride;
				}

				pDestinationVertexBuffer->Unlock();
				pSourceVertexBuffer     ->Unlock();
			}

			puIndices = reinterpret_cast<PUINT32>( pOutput->m_pIndexBuffer->Lock(0) );
			for(UINT i = 0; i < uNumIndices; ++ i)
				puIndices[i] = i;
			pOutput->m_pIndexBuffer->Unlock();
		}

		m_pIndexBuffer->Unlock();

		pOutput->m_AttributeTable = m_AttributeTable;

		if( pOutput->_Create() )
			return pOutput;
		else
		{
			ALCHEMY_DEBUG_WARNING("CMesh::ConvertToIndexedMesh: Mesh has been created fail.");

			static_cast<CSceneManager&>( GetGraphicsResourcePool() ).DestroyResource(*pOutput);

			return ALCHEMY_NULL;
		}
	}

	ALCHEMY_DEBUG_WARNING("CMesh::ConvertToIndexedMesh: Mesh has been cloned fail.");

	return ALCHEMY_NULL;
}

CMesh* CMesh::ConvertToBlendedMesh(const CSkinInfo& SkinInfo, 
								   UINT uBlendWeightUsage, 
								   UINT uBlendIndexUsage, 
								   UINT uMatrixIndexOffset, 
								   UINT uMaxBoneCount, 
								   PUINT puOutputMaxVertexInfluenceCount, 
								   CBuffer** ppBoneCombination,
								   PUINT puBoneCombinationCount)
{
	if(!m_pVertex || !m_pIndexBuffer)
	{
		ALCHEMY_DEBUG_WARNING("CMesh::ConvertToBlendedMesh: Error mesh.");

		return ALCHEMY_NULL;
	}

	CMesh* pOutput = static_cast<CMesh*>( CHighLevelGraphicsResource::_Clone() );

	if(pOutput)
	{
		UINT uMaxVertexInfluenceCount = 0;

		pOutput->m_pVertex = m_pVertex->ConvertToBlendedVertex(SkinInfo, uBlendWeightUsage, uBlendIndexUsage, uMatrixIndexOffset, &uMaxVertexInfluenceCount);

		if(!pOutput->m_pVertex)
		{
			ALCHEMY_DEBUG_WARNING("CMesh::ConvertToBlendedMesh: Vertex has been created fail.");

			static_cast<CSceneManager&>( GetGraphicsResourcePool() ).DestroyResource(*pOutput);

			return ALCHEMY_NULL;
		}

		_AddRef(*pOutput->m_pVertex);

		if(puOutputMaxVertexInfluenceCount)
			*puOutputMaxVertexInfluenceCount = uMaxVertexInfluenceCount;

		UINT uBoneCount = SkinInfo.GetBoneCount();
		if(uMaxBoneCount && uMaxBoneCount < uBoneCount)
		{
			_AddRef(*m_pIndexBuffer);

			pOutput->m_pIndexBuffer = m_pIndexBuffer;

			if( pOutput->_Create() )
			{
				CMesh* pMesh = pOutput->ConvertToIndexedMesh();

				if(!pMesh)
				{
					ALCHEMY_DEBUG_WARNING("CMesh::ConvertToBlendedMesh: Indexed mesh has been converted fail.");

					static_cast<CSceneManager&>( GetGraphicsResourcePool() ).DestroyResource(*pOutput);

					return ALCHEMY_NULL;
				}

				pMesh->m_AttributeTable = m_AttributeTable;

				pOutput->Release();

				pOutput = pMesh;
			}
			else
			{
				ALCHEMY_DEBUG_WARNING("CMesh::ConvertToBlendedMesh: Output mesh has been created fail.");

				static_cast<CSceneManager&>( GetGraphicsResourcePool() ).DestroyResource(*pOutput);

				return ALCHEMY_NULL;
			}

			UINT uBlendIndicesCount = (uMaxVertexInfluenceCount + 3) >> 2;

			PUINT8* ppuSourceBlendIndices;
			ALCHEMY_DEBUG_NEW(ppuSourceBlendIndices, PUINT8[uBlendIndicesCount]);

			PUINT puSourceBlendIndicesStreamStride;
			ALCHEMY_DEBUG_NEW(puSourceBlendIndicesStreamStride, UINT[uBlendIndicesCount]);

			CVertexDeclaration& VertexDeclaration = pOutput->m_pVertex->GetVertexDeclaration();
			UINT uStreamCount = VertexDeclaration.GetStreamCount(), i;

			PUINT8* ppuBlendIndicesMap;
			ALCHEMY_DEBUG_NEW(ppuBlendIndicesMap, PUINT8[uStreamCount]);
			for(i = 0; i < uStreamCount; ++ i)
				ppuBlendIndicesMap[i] = ALCHEMY_NULL;

			PUINT puBlendIndicesStreamStrideMap;
			ALCHEMY_DEBUG_NEW(puBlendIndicesStreamStrideMap, UINT[uStreamCount]);

			UINT uVertexElementCount = VertexDeclaration.GetVertexElementCount(), uCurrentSourceBlendIndices = 0, uBlend4IndicesCount = uMaxVertexInfluenceCount >> 2;
			const CVertexDeclaration::VERTEXELEMENT* pVertexElements = VertexDeclaration.GetVertexElements(), * pVertexElement;
			PUINT8 puBlendIndices;
			LPVERTEXBUFFER pBlendIndicesStream;

			if(uCurrentSourceBlendIndices < uBlend4IndicesCount)
			{
				for(i = 0; i < uVertexElementCount; ++ i)
				{
					pVertexElement = &pVertexElements[i];

					if(pVertexElement->Type == CVertexDeclaration::FLOAT4 && pVertexElement->Usage == CVertexDeclaration::BLENDINDICES)
					{
						puBlendIndices = ppuBlendIndicesMap[pVertexElement->uStream];

						if(!puBlendIndices)
						{
							pBlendIndicesStream = pOutput->m_pVertex->GetVertexBuffer(pVertexElement->uStream);

							if(!pBlendIndicesStream)
							{
								ALCHEMY_DEBUG_WARNING("CMesh::ConvertToBlendedMesh: Vertex can not find the blend indices stream.");

								ALCHEMY_DEBUG_DELETE_ARRAY(ppuSourceBlendIndices);

								ALCHEMY_DEBUG_DELETE_ARRAY(puSourceBlendIndicesStreamStride);

								ALCHEMY_DEBUG_DELETE_ARRAY(ppuBlendIndicesMap);

								ALCHEMY_DEBUG_DELETE_ARRAY(puBlendIndicesStreamStrideMap);

								static_cast<CSceneManager&>( GetGraphicsResourcePool() ).DestroyResource(*pOutput);

								return ALCHEMY_NULL;
							}

							puBlendIndicesStreamStrideMap[pVertexElement->uStream] = pBlendIndicesStream->GetStride();

							puBlendIndices = ppuBlendIndicesMap[pVertexElement->uStream] = reinterpret_cast<PUINT8>( pBlendIndicesStream->Lock(0) );

							if(!puBlendIndices)
							{
								ALCHEMY_DEBUG_WARNING("CMesh::ConvertToBlendedMesh: Blend indices has been locked fail.");

								ALCHEMY_DEBUG_DELETE_ARRAY(ppuSourceBlendIndices);

								ALCHEMY_DEBUG_DELETE_ARRAY(puSourceBlendIndicesStreamStride);

								ALCHEMY_DEBUG_DELETE_ARRAY(ppuBlendIndicesMap);

								ALCHEMY_DEBUG_DELETE_ARRAY(puBlendIndicesStreamStrideMap);

								static_cast<CSceneManager&>( GetGraphicsResourcePool() ).DestroyResource(*pOutput);

								return ALCHEMY_NULL;
							}
						}

						puSourceBlendIndicesStreamStride[uCurrentSourceBlendIndices] = puBlendIndicesStreamStrideMap[pVertexElement->uStream];

						ppuSourceBlendIndices[uCurrentSourceBlendIndices ++] = puBlendIndices + pVertexElement->uOffset;

						if(uCurrentSourceBlendIndices >= uBlend4IndicesCount)
							break;
					}
				}
			}

			UINT uVertexInfluenceCount = uMaxVertexInfluenceCount & 3;
			if(uVertexInfluenceCount)
			{
				UINT uVertexElement = CVertexDeclaration::FindVertexElement(CVertexDeclaration::BLENDINDICES, static_cast<CVertexDeclaration::ELEMENTTYPE>( uVertexInfluenceCount * sizeof(FLOAT) ), 0, pVertexElements, uVertexElementCount);

				if(uVertexElement < uVertexElementCount)
				{
					pVertexElement = &pVertexElements[uVertexElement];

					puBlendIndices = ppuBlendIndicesMap[pVertexElement->uStream];

					if(!puBlendIndices)
					{
						pBlendIndicesStream = pOutput->m_pVertex->GetVertexBuffer(pVertexElement->uStream);

						if(!pBlendIndicesStream)
						{
							ALCHEMY_DEBUG_WARNING("CMesh::ConvertToBlendedMesh: Vertex can not find the blend indices stream.");

							ALCHEMY_DEBUG_DELETE_ARRAY(ppuSourceBlendIndices);

							ALCHEMY_DEBUG_DELETE_ARRAY(puSourceBlendIndicesStreamStride);

							ALCHEMY_DEBUG_DELETE_ARRAY(ppuBlendIndicesMap);

							ALCHEMY_DEBUG_DELETE_ARRAY(puBlendIndicesStreamStrideMap);

							static_cast<CSceneManager&>( GetGraphicsResourcePool() ).DestroyResource(*pOutput);

							return ALCHEMY_NULL;
						}

						puBlendIndicesStreamStrideMap[pVertexElement->uStream] = pBlendIndicesStream->GetStride();

						puBlendIndices = ppuBlendIndicesMap[pVertexElement->uStream] = reinterpret_cast<PUINT8>( pBlendIndicesStream->Lock(0) );

						if(!puBlendIndices)
						{
							ALCHEMY_DEBUG_WARNING("CMesh::ConvertToBlendedMesh: Blend indices has been locked fail.");

							ALCHEMY_DEBUG_DELETE_ARRAY(ppuSourceBlendIndices);

							ALCHEMY_DEBUG_DELETE_ARRAY(puSourceBlendIndicesStreamStride);

							ALCHEMY_DEBUG_DELETE_ARRAY(ppuBlendIndicesMap);

							ALCHEMY_DEBUG_DELETE_ARRAY(puBlendIndicesStreamStrideMap);

							static_cast<CSceneManager&>( GetGraphicsResourcePool() ).DestroyResource(*pOutput);

							return ALCHEMY_NULL;
						}
					}

					puSourceBlendIndicesStreamStride[uCurrentSourceBlendIndices] = puBlendIndicesStreamStrideMap[pVertexElement->uStream];

					ppuSourceBlendIndices[uCurrentSourceBlendIndices] = puBlendIndices + pVertexElement->uOffset;
				}
				else
				{
					ALCHEMY_DEBUG_WARNING("CMesh::ConvertToBlendedMesh: Blend indices can not be find from the vertex elements.");

					ALCHEMY_DEBUG_DELETE_ARRAY(ppuSourceBlendIndices);

					ALCHEMY_DEBUG_DELETE_ARRAY(puSourceBlendIndicesStreamStride);

					ALCHEMY_DEBUG_DELETE_ARRAY(ppuBlendIndicesMap);

					ALCHEMY_DEBUG_DELETE_ARRAY(puBlendIndicesStreamStrideMap);

					static_cast<CSceneManager&>( GetGraphicsResourcePool() ).DestroyResource(*pOutput);

					return ALCHEMY_NULL;
				}
			}

			ALCHEMY_DEBUG_DELETE_ARRAY(puBlendIndicesStreamStrideMap);

			PUINT8 puIndices = pOutput->m_pIndexBuffer->Lock( ALCHEMY_FLAG(INDEXBUFFER::READ_ONLY) );

			if(!puIndices)
			{
				ALCHEMY_DEBUG_WARNING("CMesh::ConvertToBlendedMesh: Indices has been locked fail.");

				ALCHEMY_DEBUG_DELETE_ARRAY(ppuSourceBlendIndices);

				ALCHEMY_DEBUG_DELETE_ARRAY(puSourceBlendIndicesStreamStride);

				ALCHEMY_DEBUG_DELETE_ARRAY(ppuBlendIndicesMap);

				static_cast<CSceneManager&>( GetGraphicsResourcePool() ).DestroyResource(*pOutput);

				return ALCHEMY_NULL;
			}

			PFLOAT pfBoneIndexMap;
			ALCHEMY_DEBUG_NEW(pfBoneIndexMap, FLOAT[uBoneCount]);

			const FLOAT fMAX_BONE_INDEX = static_cast<FLOAT>(uMaxBoneCount * uMatrixIndexOffset);
			for(i = 0; i < uBoneCount; ++ i)
				pfBoneIndexMap[i] = fMAX_BONE_INDEX;

			PUINT8 puBlendIndicesBuffer;

			ALCHEMY_DEBUG_NEW(puBlendIndicesBuffer, UINT8[uMaxVertexInfluenceCount * pOutput->m_pIndexBuffer->GetMemberCount() * sizeof(FLOAT)]);

			PFLOAT pfDestinationBlendIndices = reinterpret_cast<PFLOAT>(puBlendIndicesBuffer), pfSourceBlendIndices;

			UINT uIndexCount = pOutput->m_pIndexBuffer->GetMemberCount(), 
				uIndexStride = pOutput->m_pIndexBuffer->GetStride(), 
				uBoneCombinationCount = 0, 
				uBoneCombinationSize = 0, 
				uCurrentBoneCount = 0,
				uBoneCountOffset = 0, 
				uIndexOffset = 0,
				uIndex, 
				uSourceBlendIndex,
				uDesinationBlendIndex, 
				uSourceBlendStreamIndex,
				uBoneIndex,
				j;

			CIndexBuffer::FORMAT Format = pOutput->m_pIndexBuffer->GetFormat();

			if(Format == CIndexBuffer::BIT16)
			{
				for(i = 0; i < uIndexCount; ++ i)
				{
					if(uIndexOffset == 3)
					{
						uBoneCountOffset = 0;
						uIndexOffset     = 0;
					}

					uIndex = *reinterpret_cast<PUINT16>(puIndices + i * uIndexStride);

					uDesinationBlendIndex = uIndex * uMaxVertexInfluenceCount;

					for(j = 0; j < uMaxVertexInfluenceCount; ++ j)
					{
						if( !(j & 3) )
						{
							uSourceBlendStreamIndex = j >> 2;
							pfSourceBlendIndices    = reinterpret_cast<PFLOAT>(ppuSourceBlendIndices[uSourceBlendStreamIndex] + puSourceBlendIndicesStreamStride[uSourceBlendStreamIndex] * uIndex);
							uSourceBlendIndex       = 0;
						}

						uBoneIndex = static_cast<UINT>(pfSourceBlendIndices[uSourceBlendIndex]) / uMatrixIndexOffset;

						if(pfBoneIndexMap[uBoneIndex] == fMAX_BONE_INDEX)
						{
							pfBoneIndexMap[uBoneIndex] = static_cast<FLOAT>(uCurrentBoneCount * uMatrixIndexOffset);

							++ uCurrentBoneCount;

							++ uBoneCountOffset;

							if(uCurrentBoneCount == uMaxBoneCount)
							{
								if( uIndexOffset != 2 || j != (uMaxVertexInfluenceCount - 1) )
								{
									i -= uIndexOffset + 1;

									uCurrentBoneCount -= uBoneCountOffset;
								}
								else
									pfDestinationBlendIndices[uDesinationBlendIndex] = static_cast<FLOAT>( (uCurrentBoneCount - 1) * uMatrixIndexOffset );

								uBoneCombinationSize += uCurrentBoneCount * sizeof(UINT);

								uBoneCombinationSize += sizeof(BONECOMBINATION);

								++ uBoneCombinationCount;

								uCurrentBoneCount = 0;

								uBoneCountOffset = 0;

								uIndexOffset = 2;

								for(j = 0; j < uBoneCount; ++ j)
									pfBoneIndexMap[j] = fMAX_BONE_INDEX;

								break;
							}
						}

						pfDestinationBlendIndices[uDesinationBlendIndex] = pfBoneIndexMap[uBoneIndex];

						++ uSourceBlendIndex;
						++ uDesinationBlendIndex;
					}

					++ uIndexOffset;
				}
			}
			else
			{
				for(i = 0; i < uIndexCount; ++ i)
				{
					if(uIndexOffset == 3)
					{
						uBoneCountOffset = 0;
						uIndexOffset     = 0;
					}

					uIndex = *reinterpret_cast<PUINT32>(puIndices + i * uIndexStride);

					uDesinationBlendIndex = uIndex * uMaxVertexInfluenceCount;

					for(j = 0; j < uMaxVertexInfluenceCount; ++ j)
					{
						if( !(j & 3) )
						{
							uSourceBlendStreamIndex = j >> 2;
							pfSourceBlendIndices    = reinterpret_cast<PFLOAT>(ppuSourceBlendIndices[uSourceBlendStreamIndex] + puSourceBlendIndicesStreamStride[uSourceBlendStreamIndex] * uIndex);
							uSourceBlendIndex       = 0;
						}

						uBoneIndex = static_cast<UINT>(pfSourceBlendIndices[uSourceBlendIndex]) / uMatrixIndexOffset;

						if(pfBoneIndexMap[uBoneIndex] == fMAX_BONE_INDEX)
						{
							pfBoneIndexMap[uBoneIndex] = static_cast<FLOAT>(uCurrentBoneCount * uMatrixIndexOffset);

							++ uCurrentBoneCount;

							++ uBoneCountOffset;

							if(uCurrentBoneCount == uMaxBoneCount)
							{
								if( uIndexOffset != 2 || j != (uMaxVertexInfluenceCount - 1) )
								{
									i -= uIndexOffset + 1;

									uCurrentBoneCount -= uBoneCountOffset;
								}
								else
									pfDestinationBlendIndices[uDesinationBlendIndex] = static_cast<FLOAT>( (uCurrentBoneCount - 1) * uMatrixIndexOffset );

								uBoneCombinationSize += uCurrentBoneCount * sizeof(UINT);

								uBoneCombinationSize += sizeof(BONECOMBINATION);

								++ uBoneCombinationCount;

								uCurrentBoneCount = 0;

								uIndexOffset = 2;

								for(j = 0; j < uBoneCount; ++ j)
									pfBoneIndexMap[j] = fMAX_BONE_INDEX;

								break;
							}
						}

						pfDestinationBlendIndices[uDesinationBlendIndex] = pfBoneIndexMap[uBoneIndex];

						++ uSourceBlendIndex;
						++ uDesinationBlendIndex;
					}

					++ uIndexOffset;
				}
			}

			if(uCurrentBoneCount)
			{
				uBoneCombinationSize += uCurrentBoneCount * sizeof(UINT);

				uBoneCombinationSize += sizeof(BONECOMBINATION);

				++ uBoneCombinationCount;
			}

			if(ppBoneCombination)
			{
				CBuffer& BoneCombinationBuffer = CreateBuffer(uBoneCombinationSize);
				LPBONECOMBINATION pBoneCombination = reinterpret_cast<LPBONECOMBINATION>( BoneCombinationBuffer.GetBufferPointer() );
				PUINT puBoneIndices = reinterpret_cast<PUINT>(pBoneCombination + uBoneCombinationCount);

				UINT uStart = 0;

				for(i = 0; i < uBoneCount; ++ i)
					pfBoneIndexMap[i] = fMAX_BONE_INDEX;

				uBoneCountOffset  = 0;
				uIndexOffset      = 0;
				uCurrentBoneCount = 0;
				if(Format == CIndexBuffer::BIT16)
				{
					for(i = 0; i < uIndexCount; ++ i)
					{
						if(uIndexOffset == 3)
						{
							uBoneCountOffset = 0;
							uIndexOffset     = 0;
						}

						uIndex = *reinterpret_cast<PUINT16>(puIndices + i * uIndexStride);

						uDesinationBlendIndex = uIndex * uMaxVertexInfluenceCount;

						for(j = 0; j < uMaxVertexInfluenceCount; ++ j)
						{
							if( !(j & 3) )
							{
								uSourceBlendStreamIndex = j >> 2;
								pfSourceBlendIndices    = reinterpret_cast<PFLOAT>(ppuSourceBlendIndices[uSourceBlendStreamIndex] + puSourceBlendIndicesStreamStride[uSourceBlendStreamIndex] * uIndex);
								uSourceBlendIndex       = 0;
							}

							uBoneIndex = static_cast<UINT>(pfSourceBlendIndices[uSourceBlendIndex]) / uMatrixIndexOffset;

							if(pfBoneIndexMap[uBoneIndex] == fMAX_BONE_INDEX)
							{
								pfBoneIndexMap[uBoneIndex] = 0.0f;

								puBoneIndices[uCurrentBoneCount] = uBoneIndex;

								++ uCurrentBoneCount;

								++ uBoneCountOffset;

								if(uCurrentBoneCount == uMaxBoneCount)
								{
									if( uIndexOffset != 2 || j != (uMaxVertexInfluenceCount - 1) )
									{
										i -= uIndexOffset + 1;

										uCurrentBoneCount -= uBoneCountOffset;
									}

									pBoneCombination->uStartIndex = uStart;

									uStart = i + 1;

									pBoneCombination->uIndexCount = uStart - pBoneCombination->uStartIndex;

									pBoneCombination->puBoneIndices = puBoneIndices;

									pBoneCombination->uBoneCount = uCurrentBoneCount;

									++ pBoneCombination;

									puBoneIndices += uCurrentBoneCount;

									uCurrentBoneCount = 0;

									uBoneCountOffset = 0;

									uIndexOffset = 2;

									for(j = 0; j < uBoneCount; ++ j)
										pfBoneIndexMap[j] = fMAX_BONE_INDEX;

									break;
								}
							}

							++ uSourceBlendIndex;
							++ uDesinationBlendIndex;
						}

						++ uIndexOffset;
					}
				}
				else
				{
					for(i = 0; i < uIndexCount; ++ i)
					{
						if(uIndexOffset == 3)
						{
							uBoneCountOffset = 0;
							uIndexOffset     = 0;
						}

						uIndex = *reinterpret_cast<PUINT32>(puIndices + i * uIndexStride);

						uDesinationBlendIndex = uIndex * uMaxVertexInfluenceCount;

						for(j = 0; j < uMaxVertexInfluenceCount; ++ j)
						{
							if( !(j & 3) )
							{
								uSourceBlendStreamIndex = j >> 2;
								pfSourceBlendIndices    = reinterpret_cast<PFLOAT>(ppuSourceBlendIndices[uSourceBlendStreamIndex] + puSourceBlendIndicesStreamStride[uSourceBlendStreamIndex] * uIndex);
								uSourceBlendIndex       = 0;
							}

							uBoneIndex = static_cast<UINT>(pfSourceBlendIndices[uSourceBlendIndex]) / uMatrixIndexOffset;

							if(pfBoneIndexMap[uBoneIndex] == fMAX_BONE_INDEX)
							{
								pfBoneIndexMap[uBoneIndex] = 0.0f;

								puBoneIndices[uCurrentBoneCount] = uBoneIndex;

								++ uCurrentBoneCount;

								++ uBoneCountOffset;

								if(uCurrentBoneCount == uMaxBoneCount)
								{
									if( uIndexOffset != 2 || j != (uMaxVertexInfluenceCount - 1) )
									{
										i -= uIndexOffset + 1;

										uCurrentBoneCount -= uBoneCountOffset;
									}

									pBoneCombination->uStartIndex = uStart;

									uStart = i + 1;

									pBoneCombination->uIndexCount = uStart - pBoneCombination->uStartIndex;

									pBoneCombination->puBoneIndices = puBoneIndices;

									pBoneCombination->uBoneCount = uCurrentBoneCount;

									++ pBoneCombination;

									puBoneIndices += uCurrentBoneCount;

									uCurrentBoneCount = 0;

									uBoneCountOffset = 0;

									uIndexOffset = 2;

									for(j = 0; j < uBoneCount; ++ j)
										pfBoneIndexMap[j] = fMAX_BONE_INDEX;

									break;
								}
							}

							++ uSourceBlendIndex;
							++ uDesinationBlendIndex;
						}

						++ uIndexOffset;
					}
				}

				if(uCurrentBoneCount)
				{
					pBoneCombination->uStartIndex = uStart;

					pBoneCombination->uIndexCount = uIndexCount - uStart;

					pBoneCombination->puBoneIndices = puBoneIndices;

					pBoneCombination->uBoneCount = uCurrentBoneCount;
				}

				*ppBoneCombination = &BoneCombinationBuffer;

				if(puBoneCombinationCount)
					*puBoneCombinationCount = uBoneCombinationCount;
			}

			ALCHEMY_DEBUG_DELETE_ARRAY(pfBoneIndexMap);

			pOutput->m_pIndexBuffer->Unlock();

			uCurrentSourceBlendIndices = 0;

			UINT uNumVertices ,uStride, uOffset = 0;
			if(uCurrentSourceBlendIndices < uBlend4IndicesCount)
			{
				for(i = 0; i < uVertexElementCount; ++ i)
				{
					pVertexElement = &pVertexElements[i];

					if(pVertexElement->Type == CVertexDeclaration::FLOAT4 && pVertexElement->Usage == CVertexDeclaration::BLENDINDICES)
					{
						pBlendIndicesStream = pOutput->m_pVertex->GetVertexBuffer(pVertexElement->uStream);

						if(!pBlendIndicesStream)
						{
							ALCHEMY_DEBUG_WARNING("CMesh::ConvertToBlendedMesh: Vertex can not find the blend indices stream.");

							ALCHEMY_DEBUG_DELETE_ARRAY(ppuSourceBlendIndices);

							ALCHEMY_DEBUG_DELETE_ARRAY(puSourceBlendIndicesStreamStride);

							ALCHEMY_DEBUG_DELETE_ARRAY(ppuBlendIndicesMap);

							ALCHEMY_DEBUG_DELETE_ARRAY(puBlendIndicesBuffer);

							static_cast<CSceneManager&>( GetGraphicsResourcePool() ).DestroyResource(*pOutput);

							return ALCHEMY_NULL;
						}

						uNumVertices = pBlendIndicesStream->GetMemberCount();
						uStride = puSourceBlendIndicesStreamStride[uCurrentSourceBlendIndices];
						puBlendIndices = ppuSourceBlendIndices[uCurrentSourceBlendIndices ++];
						pfDestinationBlendIndices = reinterpret_cast<PFLOAT>(puBlendIndicesBuffer + uOffset);
						uOffset += pVertexElement->Type;
						for(j = 0; j < uNumVertices; ++ j)
						{
							ALCHEMY_MEMORY_COPY(puBlendIndices, pfDestinationBlendIndices, pVertexElement->Type);

							puBlendIndices            += uStride;
							pfDestinationBlendIndices += uMaxVertexInfluenceCount;
						}

						if(uCurrentSourceBlendIndices >= uBlend4IndicesCount)
							break;
					}
				}
			}

			if(uVertexInfluenceCount)
			{
				UINT uVertexElement = CVertexDeclaration::FindVertexElement(CVertexDeclaration::BLENDINDICES, static_cast<CVertexDeclaration::ELEMENTTYPE>( uVertexInfluenceCount * sizeof(FLOAT) ), 0, pVertexElements, uVertexElementCount);

				if(uVertexElement < uVertexElementCount)
				{
					pVertexElement = &pVertexElements[uVertexElement];

					pBlendIndicesStream = pOutput->m_pVertex->GetVertexBuffer(pVertexElement->uStream);

					if(!pBlendIndicesStream)
					{
						ALCHEMY_DEBUG_WARNING("CMesh::ConvertToBlendedMesh: Vertex can not find the blend indices stream.");

						ALCHEMY_DEBUG_DELETE_ARRAY(ppuSourceBlendIndices);

						ALCHEMY_DEBUG_DELETE_ARRAY(puSourceBlendIndicesStreamStride);

						ALCHEMY_DEBUG_DELETE_ARRAY(ppuBlendIndicesMap);

						ALCHEMY_DEBUG_DELETE_ARRAY(puBlendIndicesBuffer);

						static_cast<CSceneManager&>( GetGraphicsResourcePool() ).DestroyResource(*pOutput);

						return ALCHEMY_NULL;
					}

					uNumVertices = pBlendIndicesStream->GetMemberCount();
					uStride = puSourceBlendIndicesStreamStride[uCurrentSourceBlendIndices];
					puBlendIndices = ppuSourceBlendIndices[uCurrentSourceBlendIndices];
					pfDestinationBlendIndices = reinterpret_cast<PFLOAT>(puBlendIndicesBuffer + uOffset);
					for(j = 0; j < uNumVertices; ++ j)
					{
						ALCHEMY_MEMORY_COPY(puBlendIndices, pfDestinationBlendIndices, pVertexElement->Type);

						puBlendIndices            += uStride;
						pfDestinationBlendIndices += uMaxVertexInfluenceCount;
					}
				}
				else
				{
					ALCHEMY_DEBUG_WARNING("CMesh::ConvertToBlendedMesh: Blend indices can not be find from the vertex elements.");

					ALCHEMY_DEBUG_DELETE_ARRAY(ppuSourceBlendIndices);

					ALCHEMY_DEBUG_DELETE_ARRAY(puSourceBlendIndicesStreamStride);

					ALCHEMY_DEBUG_DELETE_ARRAY(ppuBlendIndicesMap);

					ALCHEMY_DEBUG_DELETE_ARRAY(puBlendIndicesBuffer);

					static_cast<CSceneManager&>( GetGraphicsResourcePool() ).DestroyResource(*pOutput);

					return ALCHEMY_NULL;
				}
			}

			for(i = 0; i < uStreamCount; ++ i)
			{
				if(ppuBlendIndicesMap[i])
				{
					pBlendIndicesStream = pOutput->m_pVertex->GetVertexBuffer(i);

					if(!pBlendIndicesStream)
					{
						ALCHEMY_DEBUG_WARNING("CMesh::ConvertToBlendedMesh: Vertex can not find the blend indices stream.");

						ALCHEMY_DEBUG_DELETE_ARRAY(ppuSourceBlendIndices);

						ALCHEMY_DEBUG_DELETE_ARRAY(puSourceBlendIndicesStreamStride);

						ALCHEMY_DEBUG_DELETE_ARRAY(ppuBlendIndicesMap);

						ALCHEMY_DEBUG_DELETE_ARRAY(puBlendIndicesBuffer);

						static_cast<CSceneManager&>( GetGraphicsResourcePool() ).DestroyResource(*pOutput);

						return ALCHEMY_NULL;
					}

					pBlendIndicesStream->Unlock();
				}
			}

			ALCHEMY_DEBUG_DELETE_ARRAY(ppuSourceBlendIndices);
			ALCHEMY_DEBUG_DELETE_ARRAY(puSourceBlendIndicesStreamStride);
			ALCHEMY_DEBUG_DELETE_ARRAY(ppuBlendIndicesMap);
			ALCHEMY_DEBUG_DELETE_ARRAY(puBlendIndicesBuffer);

			return pOutput;
		}

		_AddRef(*m_pIndexBuffer);

		pOutput->m_pIndexBuffer = m_pIndexBuffer;

		pOutput->m_AttributeTable = m_AttributeTable;

		if( pOutput->_Create() )
			return pOutput;

		static_cast<CSceneManager&>( GetGraphicsResourcePool() ).DestroyResource(*pOutput);
	}

	ALCHEMY_DEBUG_WARNING("CMesh::ConvertToBlendedMesh: Clone mesh fail.");

	return ALCHEMY_NULL;
}

bool CMesh::DrawSubset(const ATTRIBUTERANGE& AttributeRange, bool bIsActivate, bool bIsActivateVertexDeclaration)
{
	CRenderSystem& RenderSystem = static_cast<CSceneManager&>( GetGraphicsResourcePool() ).GetRenderSystem();

	if(bIsActivate) 
	{
		if(m_pVertex)
			m_pVertex->Activate(bIsActivateVertexDeclaration);
		else
			return false;

		if(m_pIndexBuffer)
			RenderSystem.SetIndices( m_pIndexBuffer->GetResourceHandle() );
	}

	return RenderSystem.DrawIndexedTriangles(
		CIndexBuffer::TRIANGLE_LIST,
		AttributeRange.uStartIndex, 
		AttributeRange.uIndexCount / 3);
}

bool CMesh::DrawSubset(UINT AttributeID                   , bool bIsActivate, bool bIsActivateVertexDeclaration)
{
	LPATTRIBUTERANGE pAttributeRange = m_AttributeTable.Get(AttributeID);

	if(!pAttributeRange)
	{
		ALCHEMY_DEBUG_WARNING("CMesh::DrawSubset: Attribute id out of range.");

		return false;
	}

	/*CRenderSystem& RenderSystem = static_cast<CSceneManager&>( GetGraphicsResourcePool() ).GetRenderSystem();

	if(bIsActivate) 
	{
		if(m_pVertex)
			m_pVertex->Activate(bIsActivateVertexDeclaration);
		else
			return false;

		if(m_pIndexBuffer)
			RenderSystem.SetIndices(*m_pIndexBuffer);
	}

	return RenderSystem.DrawIndexedTriangles(
		pAttributeRange->Type, 
		pAttributeRange->uStartIndex, 
		CIndexBuffer::GetTriangleCount(pAttributeRange->uIndexCount, pAttributeRange->Type) );*/
	return DrawSubset(*pAttributeRange, bIsActivate, bIsActivateVertexDeclaration);
}