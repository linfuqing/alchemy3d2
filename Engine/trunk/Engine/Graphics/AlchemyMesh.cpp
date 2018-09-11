#include "AlchemyMesh.h"
#include "AlchemySceneManager.h"
#include "AlchemyGraphicsResourcePool.h"
#include "AlchemySkinInfo.h"

#include "../Core/AlchemyUtils.h"
#include "../Math/AlchemyFloat2.h"
//#include "../Math/AlchemyVector3.h"
#include "../Math/AlchemyVector4.h"
#include "../Math/AlchemyTriangle.h"

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
				void* pIndices = m_pIndexBuffer->Lock( ALCHEMY_FLAG(INDEXBUFFER::READ_ONLY) );
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

		_AddRef(*pOutput->m_pVertex);

		if(bIsCloneIndices)
		{
			if(m_pIndexBuffer)
			{
				void* pIndices = m_pIndexBuffer->Lock( ALCHEMY_FLAG(INDEXBUFFER::READ_ONLY) );
				pOutput->m_pIndexBuffer = static_cast<CSceneManager&>( GetGraphicsResourcePool() ).GetRenderSystem().CreateIndexBuffer(
					m_pIndexBuffer->GetMemberCount(), IndexUsage, m_pIndexBuffer->GetFormat(), pIndices);
				m_pIndexBuffer->Unlock();

				if(!pOutput->m_pIndexBuffer)
				{
					GetGraphicsResourcePool().DestroyResource(*pOutput);

					return ALCHEMY_NULL;
				}

				_AddRef(*pOutput->m_pIndexBuffer);
			}
		}
		else
		{
			pOutput->m_pIndexBuffer = m_pIndexBuffer;

			_AddRef(*m_pIndexBuffer);

			m_pIndexBuffer->AddRef();
		}

		if( pOutput->_Create() )
			return pOutput;

		GetGraphicsResourcePool().DestroyResource(*pOutput);
	}

	return ALCHEMY_NULL;
}

typedef struct
{
	CVector<UINT> TransformIndices;
	UINT uVertexOffset;
}ATTRIBUTERANGEOFFSET, * LPATTRIBUTERANGEOFFSET;

CMesh* CMesh::Clone(const ATTRIBUTERANGE* pAttributeRanges, UINT uAttributeRangeCount, const VERTEXELEMENTTRANSFORM* pTransform, UINT uTransformCount)
{
	if(!m_pVertex && !m_pIndexBuffer)
	{
		ALCHEMY_DEBUG_WARNING("CMesh::Clone: Error mesh.");

		return ALCHEMY_NULL;
	}
	
	CMesh* pOutput = static_cast<CMesh*>( CHighLevelGraphicsResource::_Clone() );

	if(pOutput)
	{
		CVertexDeclaration& VertexDeclaration = m_pVertex->GetVertexDeclaration();
		UINT i, uStreamCount = VertexDeclaration.GetStreamCount();

		PUINT puUsage;
		ALCHEMY_DEBUG_NEW(puUsage, UINT[uStreamCount]);

		CSceneManager& SceneManager = static_cast<CSceneManager&>( GetGraphicsResourcePool() );

		LPVERTEXBUFFER pVertexBuffer;
		for(i = 0; i < uStreamCount; ++ i)
		{
			pVertexBuffer = m_pVertex->GetVertexBuffer(i);
			if(pVertexBuffer)
				puUsage[i] = pVertexBuffer->GetUsage();
			else
			{
				ALCHEMY_DEBUG_WARNING("CMesh::Clone: Error Vertex.");

				ALCHEMY_DEBUG_DELETE_ARRAY(puUsage);

				SceneManager.DestroyResource(*pOutput);

				return ALCHEMY_NULL;
			}
		}
		
		CMap<UINT, ATTRIBUTERANGEOFFSET> AttributeRangeOffsetMap;
		const VERTEXELEMENTTRANSFORM* pVertexElementTransform;
		LPATTRIBUTERANGEOFFSET pAttributeRangeOffset;
		ATTRIBUTERANGEOFFSET AttributeRangeOffset;
		AttributeRangeOffset.uVertexOffset = 0;
		if(pTransform && uTransformCount)
		{
			AttributeRangeOffset.TransformIndices.Add(0);
			for(i = 0; i < uTransformCount; ++ i)
			{
				pVertexElementTransform = &pTransform[i];
				if(pVertexElementTransform->uAttributeRangeIndex < uAttributeRangeCount)
				{
					pAttributeRangeOffset = AttributeRangeOffsetMap.Get(pVertexElementTransform->uAttributeRangeIndex);
					if(pAttributeRangeOffset)
						pAttributeRangeOffset->TransformIndices.Add(i);
					else
					{
						AttributeRangeOffset.TransformIndices[0] = i;
						AttributeRangeOffsetMap.Insert(pVertexElementTransform->uAttributeRangeIndex, AttributeRangeOffset);
					}
				}
			}
		}
		else
		{
			for(i = 0; i < uAttributeRangeCount; ++ i)
				AttributeRangeOffsetMap.Insert(i, AttributeRangeOffset);
		}
		
		CMap<UINT, UINT> IndexMap;
		CIndexBuffer::FORMAT IndexFormat = m_pIndexBuffer->GetFormat();
		UINT j, uIndex, uCurrentIndexCount, uIndexCount = m_pIndexBuffer->GetMemberCount(), uVertexCount = pVertexBuffer->GetMemberCount(), uNewIndexCount = 0, uNewVertexCount = 0, uVertexOffset = 0;
		const UINT* puIndex;
		const ATTRIBUTERANGE* pAttributeRange;
		CVector<UINT>::CIterator Iterator;
		if(IndexFormat == CIndexBuffer::BIT16)
		{
			const UINT16* puSourceIndices = static_cast<const UINT16*>( m_pIndexBuffer->Lock( ALCHEMY_FLAG(INDEXBUFFER::READ_ONLY) ) );
			if(puSourceIndices)
			{
				for(i = 0; i < uAttributeRangeCount; ++ i)
				{
					pAttributeRange = &pAttributeRanges[i];
					uCurrentIndexCount = pAttributeRange->uStartIndex + pAttributeRange->uIndexCount;
					uCurrentIndexCount = uCurrentIndexCount > uIndexCount ? uIndexCount : uCurrentIndexCount;
					
					pAttributeRangeOffset = AttributeRangeOffsetMap.Get(i);
					if(pAttributeRangeOffset)
					{
						pAttributeRangeOffset->uVertexOffset = uVertexOffset;
						
						for(j = pAttributeRange->uStartIndex; j < uCurrentIndexCount; ++ j)
						{
							uIndex = puSourceIndices[j] + pAttributeRangeOffset->uVertexOffset;
							puIndex = IndexMap.Get(uIndex);
							if(!puIndex)
								IndexMap.Insert(uIndex, uNewVertexCount ++);
						}
					}
					else
					{
						for(j = pAttributeRange->uStartIndex; j < uCurrentIndexCount; ++ j)
						{
							uIndex = puSourceIndices[j];
							puIndex = IndexMap.Get(uIndex);
							if(!puIndex)
								IndexMap.Insert(uIndex, uNewVertexCount ++);
						}
					}
					
					uVertexOffset  += uVertexCount;
					uNewIndexCount += pAttributeRange->uIndexCount;
				}
				
				pOutput->m_pVertex = SceneManager.CreateVertex(uNewVertexCount, VertexDeclaration.GetResourceHandle(), puUsage);
				
				ALCHEMY_DEBUG_DELETE_ARRAY(puUsage);
				
				if(!pOutput->m_pVertex)
				{
					ALCHEMY_DEBUG_WARNING("CMesh::Clone: Vertex has been created fail.");

					static_cast<CSceneManager&>( GetGraphicsResourcePool() ).DestroyResource(*pOutput);

					return ALCHEMY_NULL;
				}
				
				_AddRef(*pOutput->m_pVertex);
				
				CRenderSystem& RenderSystem = SceneManager.GetRenderSystem();
				
				pOutput->m_pIndexBuffer = RenderSystem.CreateIndexBuffer(uNewIndexCount, m_pIndexBuffer->GetUsage(), IndexFormat);
				
				if(!pOutput->m_pIndexBuffer)
				{
					ALCHEMY_DEBUG_WARNING("CMesh::Clone: Index buffer has been created fail.");

					SceneManager.DestroyResource(*pOutput);

					return ALCHEMY_NULL;
				}
				
				_AddRef(*pOutput->m_pIndexBuffer);
				
				PUINT16 puDestinationIndices = static_cast<PUINT16>( pOutput->m_pIndexBuffer->Lock(0) );
				if(!puDestinationIndices)
				{
					ALCHEMY_DEBUG_WARNING("CMesh::Clone: Index buffer lock has been fail.");

					ALCHEMY_DEBUG_DELETE_ARRAY(puUsage);

					SceneManager.DestroyResource(*pOutput);

					return ALCHEMY_NULL;
				}
				
				ATTRIBUTERANGE AttributRange;
				AttributRange.uStartIndex = 0;
				AttributRange.uIndexCount = uNewIndexCount;
				pOutput->m_AttributeTable.Add(AttributRange);
				uIndex = 0;
				for(i = 0; i < uAttributeRangeCount; ++ i)
				{
					pAttributeRange = &pAttributeRanges[i];
					uCurrentIndexCount = pAttributeRange->uStartIndex + pAttributeRange->uIndexCount;
					uCurrentIndexCount = uCurrentIndexCount > uIndexCount ? uIndexCount : uCurrentIndexCount;
					
					AttributRange.uStartIndex = uIndex;
					AttributRange.uIndexCount = pAttributeRange->uIndexCount;
					pOutput->m_AttributeTable.Add(AttributRange);
					
					pAttributeRangeOffset = AttributeRangeOffsetMap.Get(i);
					if(pAttributeRangeOffset)
					{
						for(j = pAttributeRange->uStartIndex; j < uCurrentIndexCount; ++ j)
							puDestinationIndices[uIndex ++] = IndexMap[puSourceIndices[j] + pAttributeRangeOffset->uVertexOffset];
					}
					else
					{
						for(j = pAttributeRange->uStartIndex; j < uCurrentIndexCount; ++ j)
							puDestinationIndices[uIndex ++] = puSourceIndices[j];
					}
				}
				
				UINT uStride, k;
				LPVERTEXBUFFER pSourceVertexBuffer, pDestinationVertexBuffer;
				const UINT8* puSourceVertices;
				PUINT8 puDestinationVertices;
				
				if(pTransform && uTransformCount)
				{
					for(i = 0; i < uStreamCount; ++ i)
					{
						pSourceVertexBuffer      = m_pVertex->GetVertexBuffer(i);
						pDestinationVertexBuffer = pOutput->m_pVertex->GetVertexBuffer(i);
						if(pSourceVertexBuffer && pDestinationVertexBuffer)
						{
							puSourceVertices = static_cast<const UINT8*> ( pSourceVertexBuffer->Lock( ALCHEMY_FLAG(VERTEXBUFFER::READ_ONLY) ) );
							puDestinationVertices = static_cast<PUINT8> ( pDestinationVertexBuffer->Lock(0) );
							if(puSourceVertices && puDestinationVertices)
							{
								uStride = pDestinationVertexBuffer->GetStride();
								
								for(j = 0; j < uAttributeRangeCount; ++ j)
								{
									pAttributeRange = &pAttributeRanges[j];
									uCurrentIndexCount = pAttributeRange->uStartIndex + pAttributeRange->uIndexCount;
									uCurrentIndexCount = uCurrentIndexCount > uIndexCount ? uIndexCount : uCurrentIndexCount;
									
									pAttributeRangeOffset = AttributeRangeOffsetMap.Get(j);
									if(pAttributeRangeOffset)
									{
										for(k = pAttributeRange->uStartIndex; k < uCurrentIndexCount; ++ k)
										{
											uIndex = puSourceIndices[k];
											ALCHEMY_MEMORY_COPY(puDestinationVertices + IndexMap[uIndex + pAttributeRangeOffset->uVertexOffset] * uStride, puSourceVertices + uIndex * uStride, uStride);
										}
										
										Iterator = pAttributeRangeOffset->TransformIndices.GetIterator(0);
										while( CVector<UINT>::IsValuable(Iterator) )
										{
											pVertexElementTransform = &pTransform[*Iterator];
											if(pVertexElementTransform->uStream == i)
											{
												if(pVertexElementTransform->bIsNormal)
												{
													for(k = pAttributeRange->uStartIndex; k < uCurrentIndexCount; ++ k)
													{
														uIndex = puSourceIndices[k];
														TransformNormal( *reinterpret_cast<LPFLOAT3>(puDestinationVertices + IndexMap[uIndex + pAttributeRangeOffset->uVertexOffset] * uStride + pVertexElementTransform->uStreamOffset), pVertexElementTransform->Matrix, *reinterpret_cast<const FLOAT3*>(puSourceVertices + uIndex * uStride + pVertexElementTransform->uStreamOffset) );
													}
												}
												else
												{
													for(k = pAttributeRange->uStartIndex; k < uCurrentIndexCount; ++ k)
													{
														uIndex = puSourceIndices[k];
														Transform( *reinterpret_cast<LPFLOAT4>(puDestinationVertices + IndexMap[uIndex + pAttributeRangeOffset->uVertexOffset] * uStride + pVertexElementTransform->uStreamOffset), pVertexElementTransform->Matrix, *reinterpret_cast<const FLOAT4*>(puSourceVertices + uIndex * uStride + pVertexElementTransform->uStreamOffset) );
													}
												}
											}
											
											++ Iterator;
										}
									}
									else 
									{
										for(k = pAttributeRange->uStartIndex; k < uCurrentIndexCount; ++ k)
										{
											uIndex = puSourceIndices[k] * uStride;
											ALCHEMY_MEMORY_COPY(puDestinationVertices + uIndex, puSourceVertices + uIndex, uStride);
										}
									}

								}
								
								pSourceVertexBuffer->Unlock();
								pDestinationVertexBuffer->Unlock();
							}
							else 
							{
								ALCHEMY_DEBUG_WARNING("CMesh::Clone: Vertex buffer lock has been fail.");

								ALCHEMY_DEBUG_DELETE_ARRAY(puUsage);

								SceneManager.DestroyResource(*pOutput);

								return ALCHEMY_NULL;
							}
						}
					}
				}
				else
				{
					for(i = 0; i < uStreamCount; ++ i)
					{
						pSourceVertexBuffer      = m_pVertex->GetVertexBuffer(i);
						pDestinationVertexBuffer = pOutput->m_pVertex->GetVertexBuffer(i);
						if(pSourceVertexBuffer && pDestinationVertexBuffer)
						{
							puSourceVertices = static_cast<const UINT8*> ( pSourceVertexBuffer->Lock( ALCHEMY_FLAG(VERTEXBUFFER::READ_ONLY) ) );
							puDestinationVertices = static_cast<PUINT8> ( pDestinationVertexBuffer->Lock(0) );
							if(puSourceVertices && puDestinationVertices)
							{
								uStride = pDestinationVertexBuffer->GetStride();
								
								for(j = 0; j < uAttributeRangeCount; ++ j)
								{
									pAttributeRange = &pAttributeRanges[j];
									
									uCurrentIndexCount = pAttributeRange->uStartIndex + pAttributeRange->uIndexCount;

									pAttributeRangeOffset = AttributeRangeOffsetMap.Get(j);
									if(pAttributeRangeOffset)
									{
										for(k = pAttributeRange->uStartIndex; k < uCurrentIndexCount; ++ k)
										{
											uIndex = puSourceIndices[k];
											ALCHEMY_MEMORY_COPY(puDestinationVertices + IndexMap[uIndex + pAttributeRangeOffset->uVertexOffset] * uStride, puSourceVertices + uIndex * uStride, uStride);
										}
									}
									else
									{
										for(k = pAttributeRange->uStartIndex; k < uCurrentIndexCount; ++ k)
										{
											uIndex = puSourceIndices[k] * uStride;
											ALCHEMY_MEMORY_COPY(puDestinationVertices + uIndex, puSourceVertices + uIndex, uStride);
										}
									}
								}
								
								pSourceVertexBuffer->Unlock();
								pDestinationVertexBuffer->Unlock();
							}
							else 
							{
								ALCHEMY_DEBUG_WARNING("CMesh::Clone: Vertex buffer lock has been fail.");

								ALCHEMY_DEBUG_DELETE_ARRAY(puUsage);

								SceneManager.DestroyResource(*pOutput);

								return ALCHEMY_NULL;
							}
						}
					}
				}
			
				m_pIndexBuffer->Unlock();
				pOutput->m_pIndexBuffer->Unlock();
			}
			else
			{
				ALCHEMY_DEBUG_WARNING("CMesh::Clone: Index buffer lock has been fail.");

				ALCHEMY_DEBUG_DELETE_ARRAY(puUsage);

				SceneManager.DestroyResource(*pOutput);

				return ALCHEMY_NULL;
			}
		}
		else 
		{
			const UINT32* puSourceIndices = static_cast<const UINT32*>( m_pIndexBuffer->Lock( ALCHEMY_FLAG(INDEXBUFFER::READ_ONLY) ) );
			if(puSourceIndices)
			{
				for(i = 0; i < uAttributeRangeCount; ++ i)
				{
					pAttributeRange = &pAttributeRanges[i];
					uCurrentIndexCount = pAttributeRange->uStartIndex + pAttributeRange->uIndexCount;
					uCurrentIndexCount = uCurrentIndexCount > uIndexCount ? uIndexCount : uCurrentIndexCount;
					
					pAttributeRangeOffset = AttributeRangeOffsetMap.Get(i);
					if(pAttributeRangeOffset)
					{
						pAttributeRangeOffset->uVertexOffset = uVertexOffset;
						
						for(j = pAttributeRange->uStartIndex; j < uCurrentIndexCount; ++ j)
						{
							uIndex = puSourceIndices[j] + pAttributeRangeOffset->uVertexOffset;
							puIndex = IndexMap.Get(uIndex);
							if(!puIndex)
								IndexMap.Insert(uIndex, uNewVertexCount ++);
						}
						
						uVertexOffset  += uVertexCount;
						uNewIndexCount += pAttributeRange->uIndexCount;
					}
					else
					{
						for(j = pAttributeRange->uStartIndex; j < uCurrentIndexCount; ++ j)
						{
							uIndex = puSourceIndices[j];
							puIndex = IndexMap.Get(uIndex);
							if(!puIndex)
								IndexMap.Insert(uIndex, uNewVertexCount ++);
								
							++ uNewIndexCount;
						}
					}
				}
				
				pOutput->m_pVertex = SceneManager.CreateVertex(uNewVertexCount, VertexDeclaration.GetResourceHandle(), puUsage);
				
				ALCHEMY_DEBUG_DELETE_ARRAY(puUsage);
				
				if(!pOutput->m_pVertex)
				{
					ALCHEMY_DEBUG_WARNING("CMesh::Clone: Vertex has been created fail.");

					static_cast<CSceneManager&>( GetGraphicsResourcePool() ).DestroyResource(*pOutput);

					return ALCHEMY_NULL;
				}
				
				_AddRef(*pOutput->m_pVertex);
				
				CRenderSystem& RenderSystem = SceneManager.GetRenderSystem();
				
				pOutput->m_pIndexBuffer = RenderSystem.CreateIndexBuffer(uNewIndexCount, m_pIndexBuffer->GetUsage(), IndexFormat);
				
				if(!pOutput->m_pIndexBuffer)
				{
					ALCHEMY_DEBUG_WARNING("CMesh::Clone: Index buffer has been created fail.");

					SceneManager.DestroyResource(*pOutput);

					return ALCHEMY_NULL;
				}
				
				_AddRef(*pOutput->m_pIndexBuffer);
				
				PUINT32 puDestinationIndices = static_cast<PUINT32>( pOutput->m_pIndexBuffer->Lock(0) );
				if(!puDestinationIndices)
				{
					ALCHEMY_DEBUG_WARNING("CMesh::Clone: Index buffer lock has been fail.");

					ALCHEMY_DEBUG_DELETE_ARRAY(puUsage);

					SceneManager.DestroyResource(*pOutput);

					return ALCHEMY_NULL;
				}
				
				ATTRIBUTERANGE AttributRange;
				AttributRange.uStartIndex = 0;
				AttributRange.uIndexCount = uNewIndexCount;
				pOutput->m_AttributeTable.Add(AttributRange);
				
				uIndex = 0;
				for(i = 0; i < uAttributeRangeCount; ++ i)
				{
					pAttributeRange = &pAttributeRanges[i];
					uCurrentIndexCount = pAttributeRange->uStartIndex + pAttributeRange->uIndexCount;
					uCurrentIndexCount = uCurrentIndexCount > uIndexCount ? uIndexCount : uCurrentIndexCount;
					
					AttributRange.uStartIndex = uIndex;
					AttributRange.uIndexCount = pAttributeRange->uIndexCount;
					pOutput->m_AttributeTable.Add(AttributRange);
					
					pAttributeRangeOffset = AttributeRangeOffsetMap.Get(i);
					if(pAttributeRangeOffset)
					{
						for(j = pAttributeRange->uStartIndex; j < uCurrentIndexCount; ++ j)
							puDestinationIndices[uIndex ++] = IndexMap[puSourceIndices[j] + pAttributeRangeOffset->uVertexOffset];
					}
					else
					{
						for(j = pAttributeRange->uStartIndex; j < uCurrentIndexCount; ++ j)
							puDestinationIndices[uIndex ++] = puSourceIndices[j];
					}
				}
				
				UINT uStride, k;
				LPVERTEXBUFFER pSourceVertexBuffer, pDestinationVertexBuffer;
				const UINT8* puSourceVertices;
				PUINT8 puDestinationVertices;
				
				if(pTransform && uTransformCount)
				{
					for(i = 0; i < uStreamCount; ++ i)
					{
						pSourceVertexBuffer      = m_pVertex->GetVertexBuffer(i);
						pDestinationVertexBuffer = pOutput->m_pVertex->GetVertexBuffer(i);
						if(pSourceVertexBuffer && pDestinationVertexBuffer)
						{
							puSourceVertices = static_cast<const UINT8*> ( pSourceVertexBuffer->Lock( ALCHEMY_FLAG(VERTEXBUFFER::READ_ONLY) ) );
							puDestinationVertices = static_cast<PUINT8> ( pDestinationVertexBuffer->Lock(0) );
							if(puSourceVertices && puDestinationVertices)
							{
								uStride = pDestinationVertexBuffer->GetStride();
								
								for(j = 0; j < uAttributeRangeCount; ++ j)
								{
									pAttributeRange = &pAttributeRanges[j];
									uCurrentIndexCount = pAttributeRange->uStartIndex + pAttributeRange->uIndexCount;
									uCurrentIndexCount = uCurrentIndexCount > uIndexCount ? uIndexCount : uCurrentIndexCount;
									
									pAttributeRangeOffset = AttributeRangeOffsetMap.Get(j);
									if(pAttributeRangeOffset)
									{
										for(k = pAttributeRange->uStartIndex; k < uCurrentIndexCount; ++ k)
										{
											uIndex = puSourceIndices[k];
											ALCHEMY_MEMORY_COPY(puDestinationVertices + IndexMap[uIndex + pAttributeRangeOffset->uVertexOffset] * uStride, puSourceVertices + uIndex * uStride, uStride);
										}
										
										Iterator = pAttributeRangeOffset->TransformIndices.GetIterator(0);
										while( CVector<UINT>::IsValuable(Iterator) )
										{
											pVertexElementTransform = &pTransform[*Iterator];
											if(pVertexElementTransform->uStream == i)
											{
												if(pVertexElementTransform->bIsNormal)
												{
													for(k = pAttributeRange->uStartIndex; k < uCurrentIndexCount; ++ k)
													{
														uIndex = puSourceIndices[k];
														TransformNormal( *reinterpret_cast<LPFLOAT3>(puDestinationVertices + IndexMap[uIndex + pAttributeRangeOffset->uVertexOffset] * uStride + pVertexElementTransform->uStreamOffset), pVertexElementTransform->Matrix, *reinterpret_cast<const FLOAT3*>(puSourceVertices + uIndex * uStride + pVertexElementTransform->uStreamOffset) );
													}
												}
												else
												{
													for(k = pAttributeRange->uStartIndex; k < uCurrentIndexCount; ++ k)
													{
														uIndex = puSourceIndices[k];
														Transform( *reinterpret_cast<LPFLOAT4>(puDestinationVertices + IndexMap[uIndex + pAttributeRangeOffset->uVertexOffset] * uStride + pVertexElementTransform->uStreamOffset), pVertexElementTransform->Matrix, *reinterpret_cast<const FLOAT4*>(puSourceVertices + uIndex * uStride + pVertexElementTransform->uStreamOffset) );
													}
												}
											}
											
											++ Iterator;
										}
									}
									else 
									{
										for(k = pAttributeRange->uStartIndex; k < uCurrentIndexCount; ++ k)
										{
											uIndex = puSourceIndices[k] * uStride;
											ALCHEMY_MEMORY_COPY(puDestinationVertices + uIndex, puSourceVertices + uIndex, uStride);
										}
									}

								}
								
								pSourceVertexBuffer->Unlock();
								pDestinationVertexBuffer->Unlock();
							}
							else 
							{
								ALCHEMY_DEBUG_WARNING("CMesh::Clone: Vertex buffer lock has been fail.");

								ALCHEMY_DEBUG_DELETE_ARRAY(puUsage);

								SceneManager.DestroyResource(*pOutput);

								return ALCHEMY_NULL;
							}
						}
					}
				}
				else
				{
					for(i = 0; i < uStreamCount; ++ i)
					{
						pSourceVertexBuffer      = m_pVertex->GetVertexBuffer(i);
						pDestinationVertexBuffer = pOutput->m_pVertex->GetVertexBuffer(i);
						if(pSourceVertexBuffer && pDestinationVertexBuffer)
						{
							puSourceVertices = static_cast<const UINT8*> ( pSourceVertexBuffer->Lock( ALCHEMY_FLAG(VERTEXBUFFER::READ_ONLY) ) );
							puDestinationVertices = static_cast<PUINT8> ( pDestinationVertexBuffer->Lock(0) );
							if(puSourceVertices && puDestinationVertices)
							{
								uStride = pDestinationVertexBuffer->GetStride();
								
								for(j = 0; j < uAttributeRangeCount; ++ j)
								{
									pAttributeRange = &pAttributeRanges[j];
									uCurrentIndexCount = pAttributeRange->uStartIndex + pAttributeRange->uIndexCount;
									
									pAttributeRangeOffset = AttributeRangeOffsetMap.Get(j);
									if(pAttributeRangeOffset)
									{
										for(k = pAttributeRange->uStartIndex; k < uCurrentIndexCount; ++ k)
										{
											uIndex = puSourceIndices[k];
											ALCHEMY_MEMORY_COPY(puDestinationVertices + IndexMap[uIndex + pAttributeRangeOffset->uVertexOffset] * uStride, puSourceVertices + uIndex * uStride, uStride);
										}
									}
									else
									{
										for(k = pAttributeRange->uStartIndex; k < uCurrentIndexCount; ++ k)
										{
											uIndex = puSourceIndices[k] * uStride;
											ALCHEMY_MEMORY_COPY(puDestinationVertices + uIndex, puSourceVertices + uIndex, uStride);
										}
									}
								}
								
								pSourceVertexBuffer->Unlock();
								pDestinationVertexBuffer->Unlock();
							}
							else 
							{
								ALCHEMY_DEBUG_WARNING("CMesh::Clone: Vertex buffer lock has been fail.");

								ALCHEMY_DEBUG_DELETE_ARRAY(puUsage);

								SceneManager.DestroyResource(*pOutput);

								return ALCHEMY_NULL;
							}
						}
					}
				}
			
				m_pIndexBuffer->Unlock();
				pOutput->m_pIndexBuffer->Unlock();
			}
			else
			{
				ALCHEMY_DEBUG_WARNING("CMesh::Clone: Index buffer lock has been fail.");

				ALCHEMY_DEBUG_DELETE_ARRAY(puUsage);

				SceneManager.DestroyResource(*pOutput);

				return ALCHEMY_NULL;
			}
		}

		if( pOutput->_Create() )
			return pOutput;

		SceneManager.DestroyResource(*pOutput);
	}

	return ALCHEMY_NULL;
}

CMesh* CMesh::Clone(FLOAT fInstanceCount, const MATRIX4X4* pTransform, const CEffectCode::ELEMENTDECLARATION* pTransformElements, UINT uTransformElementCount)
{
	UINT uInstanceCount = ALCHEMY_CEIL(fInstanceCount), uIndexCount = m_pIndexBuffer->GetMemberCount();
	if(!uInstanceCount)
		return ALCHEMY_NULL;
	
	if(!m_pVertex && !m_pIndexBuffer)
	{
		ALCHEMY_DEBUG_WARNING("CMesh::Clone: Error mesh.");

		return ALCHEMY_NULL;
	}
	
	LPATTRIBUTERANGE pAttributeRanges, pAttributeRange;
	ALCHEMY_DEBUG_NEW(pAttributeRanges, ATTRIBUTERANGE[uInstanceCount]);
	UINT uMaxInstanceIndex = uInstanceCount - 1;
	pAttributeRange = &pAttributeRanges[uMaxInstanceIndex];
	pAttributeRange->uStartIndex = 0;
	pAttributeRange->uIndexCount = static_cast<UINT>(ALCHEMY_FRACT(fInstanceCount) * uIndexCount);
	pAttributeRange->uIndexCount = pAttributeRange->uIndexCount ? pAttributeRange->uIndexCount : uIndexCount;
	for(UINT i = 0; i < uMaxInstanceIndex; ++ i)
	{
		pAttributeRange = &pAttributeRanges[i];
		pAttributeRange->uStartIndex = 0;
		pAttributeRange->uIndexCount = uIndexCount;
	}
	
	CMesh* pMesh = ALCHEMY_NULL;
	if(pTransform)
	{
		CVertexDeclaration& VertexDeclaration = m_pVertex->GetVertexDeclaration();
		const CVertexDeclaration::VERTEXELEMENT* pSourceVertexElement = VertexDeclaration.GetVertexElements();

		UINT uSourceVertexElementCount = VertexDeclaration.GetVertexElementCount(), uCurrentElement;
		if(!pTransformElements || !uTransformElementCount)
		{
			static CEffectCode::ELEMENTDECLARATION Elements[4];
			
			CEffectCode::LPELEMENTDECLARATION pElement;
			
			uTransformElementCount = 0;

			uCurrentElement = CVertexDeclaration::FindVertexElement(CVertexDeclaration::POSITION, CVertexDeclaration::FLOAT4, 0, pSourceVertexElement, uSourceVertexElementCount);
			if(uCurrentElement < uSourceVertexElementCount)
			{
				pElement         = &Elements[uTransformElementCount ++ ];
				pElement->Type   = CVertexDeclaration::FLOAT4;
				pElement->Usage  = CVertexDeclaration::POSITION;
				pElement->uIndex = 0;
			}

			uCurrentElement = CVertexDeclaration::FindVertexElement(CVertexDeclaration::NORMAL, CVertexDeclaration::FLOAT3, 0, pSourceVertexElement, uSourceVertexElementCount);
			if(uCurrentElement < uSourceVertexElementCount)
			{
				pElement         = &Elements[uTransformElementCount ++ ];
				pElement->Type   = CVertexDeclaration::FLOAT3;
				pElement->Usage  = CVertexDeclaration::NORMAL;
				pElement->uIndex = 0;
			}

			uCurrentElement = CVertexDeclaration::FindVertexElement(CVertexDeclaration::TANGENT, CVertexDeclaration::FLOAT3, 0, pSourceVertexElement, uSourceVertexElementCount);
			if(uCurrentElement < uSourceVertexElementCount)
			{
				pElement         = &Elements[uTransformElementCount ++ ];
				pElement->Type   = CVertexDeclaration::FLOAT3;
				pElement->Usage  = CVertexDeclaration::TANGENT;
				pElement->uIndex = 0;
			}

			uCurrentElement = CVertexDeclaration::FindVertexElement(CVertexDeclaration::BINORMAL, CVertexDeclaration::FLOAT3, 0, pSourceVertexElement, uSourceVertexElementCount);
			if(uCurrentElement < uSourceVertexElementCount)
			{
				pElement         = &Elements[uTransformElementCount ++ ];
				pElement->Type   = CVertexDeclaration::FLOAT3;
				pElement->Usage  = CVertexDeclaration::BINORMAL;
				pElement->uIndex = 0;
			}
			
			pTransformElements = Elements;
		}
		
		UINT uVertexElementTransformCount = uInstanceCount * uTransformElementCount, j;
		LPVERTEXELEMENTTRANSFORM pVertexElementTransform, pCurrentVertexElementTransform;
		const CVertexDeclaration::VERTEXELEMENT* pVertexElement;
		const CEffectCode::ELEMENTDECLARATION* pTransformElement;
		bool bIsNormal;
		ALCHEMY_DEBUG_NEW(pVertexElementTransform, VERTEXELEMENTTRANSFORM[uVertexElementTransformCount]);
		
		pCurrentVertexElementTransform = pVertexElementTransform;
		for(UINT i = 0; i < uTransformElementCount; ++ i)
		{
			pTransformElement        = &pTransformElements[i];
			uCurrentElement = CVertexDeclaration::FindVertexElement(pTransformElement->Usage, pTransformElement->Type, pTransformElement->uIndex, pSourceVertexElement, uSourceVertexElementCount);
			if(uCurrentElement < uSourceVertexElementCount)
			{
				pVertexElement = &pSourceVertexElement[uCurrentElement];
				bIsNormal = pVertexElement->Usage != CVertexDeclaration::POSITION;
				for(j = 0; j < uInstanceCount; ++ j)
				{
					pCurrentVertexElementTransform->Matrix               = pTransform[j];
					pCurrentVertexElementTransform->uStream              = pVertexElement->uStream;
					pCurrentVertexElementTransform->uStreamOffset        = pVertexElement->uOffset;
					pCurrentVertexElementTransform->uAttributeRangeIndex = j;
					pCurrentVertexElementTransform->bIsNormal            = bIsNormal;
					
					++ pCurrentVertexElementTransform;
				}
			}
		}
		
		pMesh = Clone(pAttributeRanges, uInstanceCount, pVertexElementTransform, uVertexElementTransformCount);
		
		ALCHEMY_DEBUG_DELETE_ARRAY(pVertexElementTransform);
	}
	else
		pMesh = Clone(pAttributeRanges, uInstanceCount);
	
	ALCHEMY_DEBUG_DELETE_ARRAY(pAttributeRanges);
	
	return pMesh;
}

bool CMesh::ComputeTangent(UINT uTexcoordIndex)
{
	CVertexDeclaration& VertexDeclaration = m_pVertex->GetVertexDeclaration();
	const CVertexDeclaration::VERTEXELEMENT* pSourceVertexElement = VertexDeclaration.GetVertexElements();
	UINT uSourceVertexElementCount = VertexDeclaration.GetVertexElementCount(), uCurrentElement;
	LPVERTEXBUFFER pPosBuffer, pNormalBuffer, pTexcoordBuffer, pTangentBuffer, pBinormalBuffer = ALCHEMY_NULL;

	UINT uPosOffset, uNormalOffset, uTexcoordOffset, uTangentOffset, uBinormalOffset;
	FLOAT3* pTemp;

	uCurrentElement = CVertexDeclaration::FindVertexElement(CVertexDeclaration::POSITION, CVertexDeclaration::FLOAT4, 0, pSourceVertexElement, uSourceVertexElementCount);
	if(uCurrentElement < uSourceVertexElementCount)
	{
		pPosBuffer = m_pVertex->GetVertexBuffer( pSourceVertexElement[uCurrentElement].uStream );
		uPosOffset = pSourceVertexElement[uCurrentElement].uOffset;
	}
	else
	{
		ALCHEMY_DEBUG_WARNING("CMesh::ComputeTangent: No Position stream.");
		return false;
	}

	uCurrentElement = CVertexDeclaration::FindVertexElement(CVertexDeclaration::NORMAL, CVertexDeclaration::FLOAT3, 0, pSourceVertexElement, uSourceVertexElementCount);
	if(uCurrentElement < uSourceVertexElementCount)
	{
		pNormalBuffer = m_pVertex->GetVertexBuffer( pSourceVertexElement[uCurrentElement].uStream );
		uNormalOffset = pSourceVertexElement[uCurrentElement].uOffset;
	}
	else
	{
		ALCHEMY_DEBUG_WARNING("CMesh::ComputeTangent: No NORMAL stream.");
		return false;
	}

	uCurrentElement = CVertexDeclaration::FindVertexElement(CVertexDeclaration::TEXCOORD, CVertexDeclaration::FLOAT2, uTexcoordIndex, pSourceVertexElement, uSourceVertexElementCount);
	if(uCurrentElement < uSourceVertexElementCount)
	{
		pTexcoordBuffer = m_pVertex->GetVertexBuffer( pSourceVertexElement[uCurrentElement].uStream );
		uTexcoordOffset = pSourceVertexElement[uCurrentElement].uOffset;
	}
	else
	{
		ALCHEMY_DEBUG_WARNING("CMesh::ComputeTangent: No TEXCOORD stream.");
		return false;
	}

	uCurrentElement = CVertexDeclaration::FindVertexElement(CVertexDeclaration::TANGENT, CVertexDeclaration::FLOAT3, 0, pSourceVertexElement, uSourceVertexElementCount);
	if(uCurrentElement < uSourceVertexElementCount)
	{
		pTangentBuffer = m_pVertex->GetVertexBuffer( pSourceVertexElement[uCurrentElement].uStream );
		uTangentOffset = pSourceVertexElement[uCurrentElement].uOffset;
	}
	else
	{
		ALCHEMY_DEBUG_WARNING("CMesh::ComputeTangent: No TANGENT stream.");
		return false;
	}

	uCurrentElement = CVertexDeclaration::FindVertexElement(CVertexDeclaration::BINORMAL, CVertexDeclaration::FLOAT3, 0, pSourceVertexElement, uSourceVertexElementCount);
	if(uCurrentElement < uSourceVertexElementCount)
	{
		pBinormalBuffer = m_pVertex->GetVertexBuffer( pSourceVertexElement[uCurrentElement].uStream );
		uBinormalOffset = pSourceVertexElement[uCurrentElement].uOffset;
	}


	PUINT8 pPosData = static_cast<PUINT8>( pPosBuffer->Lock( ALCHEMY_FLAG(VERTEXBUFFER::READ_ONLY) ) );
	if(pPosData == ALCHEMY_NULL)
	{
		ALCHEMY_DEBUG_WARNING("CMesh::ComputeTangent: lock position stream fail.");
		return false;
	}

	PUINT8 pTexCoordData = static_cast<PUINT8>( pTexcoordBuffer->Lock( ALCHEMY_FLAG(VERTEXBUFFER::READ_ONLY) ) );
	if(pTexCoordData == ALCHEMY_NULL)
	{
		ALCHEMY_DEBUG_WARNING("CMesh::ComputeTangent: lock texcoord stream fail.");
		return false;
	}

	PUINT8 pTangentData = static_cast<PUINT8>( pTangentBuffer->Lock( ALCHEMY_FLAG(VERTEXBUFFER::READ_ONLY) ) );
	if(pTangentData == ALCHEMY_NULL)
	{
		ALCHEMY_DEBUG_WARNING("CMesh::ComputeTangent: lock tangent stream fail.");
		return false;
	}

	UINT i=0;
	for (i=0; i<pTangentBuffer->GetMemberCount(); ++i)
	{
		pTemp = reinterpret_cast<FLOAT3*>( pTangentData + i * pTangentBuffer->GetStride() + uTangentOffset);
		pTemp->x = 0.0f;
		pTemp->y = 0.0f;
		pTemp->z = 0.0f;
	}

	UINT uPolyCount = m_pIndexBuffer->GetMemberCount()/3;
	UINT uIndices[3] = {0};
	FLOAT4 position[3];
	FLOAT2 texcoord[3];
	CVector3* pNormalTemp;

	FLOAT x1,x2,y1,y2,z1,z2;
	FLOAT s1,s2,t1,t2;
	FLOAT r;

	FLOAT3 sdir;

	if(m_pIndexBuffer->GetFormat() == INDEXBUFFER::BIT16)
	{
		PUINT16 pIndexData = reinterpret_cast<PUINT16>( m_pIndexBuffer->Lock( ALCHEMY_FLAG(INDEXBUFFER::READ_ONLY) ) );
		if(pIndexData == ALCHEMY_NULL)
		{
			ALCHEMY_DEBUG_WARNING("CMesh::ComputeTangent: lock index stream fail.");
			return false;
		}

		for(i=0; i<uPolyCount; ++i)
		{
			uIndices[0] = pIndexData[i*3];
			uIndices[1] = pIndexData[i*3+1];
			uIndices[2] = pIndexData[i*3+2];

			position[0] = *reinterpret_cast<FLOAT4*>( pPosData + uIndices[0] * pPosBuffer->GetStride() + uPosOffset );
			position[1] = *reinterpret_cast<FLOAT4*>( pPosData + uIndices[1] * pPosBuffer->GetStride() + uPosOffset );
			position[2] = *reinterpret_cast<FLOAT4*>( pPosData + uIndices[2] * pPosBuffer->GetStride() + uPosOffset );

			texcoord[0] = *reinterpret_cast<FLOAT2*>( pTexCoordData + uIndices[0] * pTexcoordBuffer->GetStride() + uTexcoordOffset );
			texcoord[1] = *reinterpret_cast<FLOAT2*>( pTexCoordData + uIndices[1] * pTexcoordBuffer->GetStride() + uTexcoordOffset );
			texcoord[2] = *reinterpret_cast<FLOAT2*>( pTexCoordData + uIndices[2] * pTexcoordBuffer->GetStride() + uTexcoordOffset );

			x1 = position[1].x - position[0].x;
			y1 = position[1].y - position[0].y;
			z1 = position[1].z - position[0].z;
			x2 = position[2].x - position[0].x;
			y2 = position[2].y - position[0].y;
			z2 = position[2].z - position[0].z;

			s1 = texcoord[1].x - texcoord[0].x;
			t1 = texcoord[1].y - texcoord[0].y;
			s2 = texcoord[2].x - texcoord[0].x;
			t2 = texcoord[2].y - texcoord[0].y;

			r = s1 * t2 - s2 * t1;
			if(r)
				r = 1.0f / r;
			else
				continue;

			sdir.x = (t2 * x1 - t1 * x2) * r;
			sdir.y = (t2 * y1 - t1 * y2) * r;
			sdir.z = (t2 * z1 - t1 * z2) * r;

			pNormalTemp = reinterpret_cast<CVector3*>(&sdir);
			pNormalTemp->Normalize();

			pTemp = reinterpret_cast<FLOAT3*>( pTangentData + uIndices[0] * pTangentBuffer->GetStride() + uTangentOffset);

			pTemp->x += sdir.x;
			pTemp->y += sdir.y;
			pTemp->z += sdir.z;

			pTemp = reinterpret_cast<FLOAT3*>( pTangentData + uIndices[1] * pTangentBuffer->GetStride() + uTangentOffset);

			pTemp->x += sdir.x;
			pTemp->y += sdir.y;
			pTemp->z += sdir.z;

			pTemp = reinterpret_cast<FLOAT3*>( pTangentData + uIndices[2] * pTangentBuffer->GetStride() + uTangentOffset);

			pTemp->x += sdir.x;
			pTemp->y += sdir.y;
			pTemp->z += sdir.z;
		}
	}
	else
	{
		PUINT32 pIndexData = reinterpret_cast<PUINT32>( m_pIndexBuffer->Lock( ALCHEMY_FLAG(INDEXBUFFER::READ_ONLY) ) );
		if(pIndexData == ALCHEMY_NULL)
		{
			ALCHEMY_DEBUG_WARNING("CMesh::ComputeTangent: lock index stream fail.");
			return false;
		}

		for(i=0; i<uPolyCount; ++i)
		{
			uIndices[0] = pIndexData[i*3];
			uIndices[1] = pIndexData[i*3+1];
			uIndices[2] = pIndexData[i*3+2];

			position[0] = *reinterpret_cast<FLOAT4*>( pPosData + uIndices[0] * pPosBuffer->GetStride() + uPosOffset );
			position[1] = *reinterpret_cast<FLOAT4*>( pPosData + uIndices[1] * pPosBuffer->GetStride() + uPosOffset );
			position[2] = *reinterpret_cast<FLOAT4*>( pPosData + uIndices[2] * pPosBuffer->GetStride() + uPosOffset );

			texcoord[0] = *reinterpret_cast<FLOAT2*>( pTexCoordData + uIndices[0] * pTexcoordBuffer->GetStride() + uTexcoordOffset );
			texcoord[1] = *reinterpret_cast<FLOAT2*>( pTexCoordData + uIndices[1] * pTexcoordBuffer->GetStride() + uTexcoordOffset );
			texcoord[2] = *reinterpret_cast<FLOAT2*>( pTexCoordData + uIndices[2] * pTexcoordBuffer->GetStride() + uTexcoordOffset );

			x1 = position[1].x - position[0].x;
			x2 = position[2].x - position[0].x;
			y1 = position[1].y - position[0].y;
			y2 = position[2].y - position[0].y;
			z1 = position[1].z - position[0].z;
			z2 = position[2].z - position[0].z;

			s1 = texcoord[1].x - texcoord[0].x;
			s2 = texcoord[2].x - texcoord[0].x;
			t1 = texcoord[1].y - texcoord[0].y;
			t2 = texcoord[2].y - texcoord[0].y;

			r = s1 * t2 - s2 * t1;
			if(r)
				r = 1.0f / r;
			else
				continue;

			sdir.x = (t2 * x1 - t1 * x2) * r;
			sdir.y = (t2 * y1 - t1 * y2) * r;
			sdir.z = (t2 * z1 - t1 * z2) * r;

			pNormalTemp = reinterpret_cast<CVector3*>(&sdir);
			pNormalTemp->Normalize();

			pTemp = reinterpret_cast<FLOAT3*>( pTangentData + uIndices[0] * pTangentBuffer->GetStride() + uTangentOffset);

			pTemp->x += sdir.x;
			pTemp->y += sdir.y;
			pTemp->z += sdir.z;

			pTemp = reinterpret_cast<FLOAT3*>( pTangentData + uIndices[1] * pTangentBuffer->GetStride() + uTangentOffset);

			pTemp->x += sdir.x;
			pTemp->y += sdir.y;
			pTemp->z += sdir.z;

			pTemp = reinterpret_cast<FLOAT3*>( pTangentData + uIndices[2] * pTangentBuffer->GetStride() + uTangentOffset);

			pTemp->x += sdir.x;
			pTemp->y += sdir.y;
			pTemp->z += sdir.z;
		}
	}

	m_pIndexBuffer->Unlock();

	pPosBuffer->Unlock();
	pTexcoordBuffer->Unlock();

	PUINT8 pNormalData = reinterpret_cast<PUINT8>( pNormalBuffer->Lock( ALCHEMY_FLAG( VERTEXBUFFER::READ_ONLY ) ) );
	if(pNormalData == ALCHEMY_NULL)
	{
		ALCHEMY_DEBUG_WARNING("CMesh::ComputeTangent: lock normal stream fail.");
		return false;
	}

	PUINT8 pBinormalData = ALCHEMY_NULL;
	if(pBinormalBuffer)
		pBinormalData = reinterpret_cast<PUINT8>( pBinormalBuffer->Lock( ALCHEMY_FLAG( VERTEXBUFFER::WRITE_ONLY ) ) );

	UINT uVertexCount = pPosBuffer->GetMemberCount();
	CVector3  normal;
	CVector3 tangent;
	CVector3 binormal;
	FLOAT dot;
	for(i=0; i<uVertexCount; ++i)
	{
		tangent = *reinterpret_cast<CVector3*>( pTangentData + i*pTangentBuffer->GetStride() + uTangentOffset );
		normal = *reinterpret_cast<CVector3*>( pNormalData + i*pNormalBuffer->GetStride() + uNormalOffset );

		dot = tangent.DotProduct(normal);
		normal.x = dot * tangent.x;
		normal.y = dot * tangent.y;
		normal.z = dot * tangent.z;

		tangent = tangent - normal;
		tangent.Normalize();

		pTemp = reinterpret_cast<FLOAT3*>( pTangentData + i * pTangentBuffer->GetStride() + uTangentOffset);

		pTemp->x = tangent.x;
		pTemp->y = tangent.y;
		pTemp->z = tangent.z;

		if(pBinormalData)
		{
			pTemp = reinterpret_cast<FLOAT3*>( pBinormalData + i * pBinormalBuffer->GetStride() + uBinormalOffset );
			binormal.CrossProduct( tangent, normal );
			pTemp->x = binormal.x;
			pTemp->y = binormal.y;
			pTemp->z = binormal.z;
		}
	}

	pNormalBuffer->Unlock();
	pTangentBuffer->Unlock();
	if(pBinormalBuffer)
		pBinormalBuffer->Unlock();

	return true;
}

bool CMesh::ComputeNormal()
{
	if(!m_pVertex || !m_pIndexBuffer)
	{
		ALCHEMY_DEBUG_WARNING("CMesh::ComputeNormal(): Error Mesh!");
		return false;
	}

	CVertexDeclaration& VertexDeclaration = m_pVertex->GetVertexDeclaration();
	const CVertexDeclaration::VERTEXELEMENT* pSourceVertexElement = VertexDeclaration.GetVertexElements();
	UINT uSourceVertexElementCount = VertexDeclaration.GetVertexElementCount(), uCurrentElement;
	LPVERTEXBUFFER pPosBuffer, pNormalBuffer;
	UINT uPosOffset, uNormalOffset;

	uCurrentElement = CVertexDeclaration::FindVertexElement(CVertexDeclaration::POSITION, CVertexDeclaration::FLOAT4, 0, pSourceVertexElement, uSourceVertexElementCount);
	if(uCurrentElement < uSourceVertexElementCount)
	{
		pPosBuffer = m_pVertex->GetVertexBuffer( pSourceVertexElement[uCurrentElement].uStream );
		uPosOffset = pSourceVertexElement[uCurrentElement].uOffset;
	}
	else
	{
		ALCHEMY_DEBUG_WARNING("CMesh::ComputeTangent: No POSITION stream.")
		return false;
	}

	uCurrentElement = CVertexDeclaration::FindVertexElement(CVertexDeclaration::NORMAL, CVertexDeclaration::FLOAT3, 0, pSourceVertexElement, uSourceVertexElementCount);
	if(uCurrentElement < uSourceVertexElementCount)
	{
		pNormalBuffer = m_pVertex->GetVertexBuffer( pSourceVertexElement[uCurrentElement].uStream );
		uNormalOffset = pSourceVertexElement[uCurrentElement].uOffset;
	}
	else
	{
		ALCHEMY_DEBUG_WARNING("CMesh::ComputeTangent: No NORMAL stream.")
		return false;
	}

	PUINT8 pPosData, pNormalData;
	pPosData = static_cast<PUINT8>( pPosBuffer->Lock( ALCHEMY_FLAG( VERTEXBUFFER::READ_ONLY ) ) );
	if(!pPosData)
	{
		ALCHEMY_DEBUG_WARNING("CMesh::ComputeTangent: Lock Posistion Stream error.");
		return false;
	}

	pNormalData = static_cast<PUINT8>( pNormalBuffer->Lock( ALCHEMY_FLAG( VERTEXBUFFER::WRITE_ONLY ) ) );
	if(!pNormalData)
	{
		ALCHEMY_DEBUG_WARNING("CMesh::ComputeTangent: Lock Normal Stream error.");
		return false;
	}

	CVector3* pNormalTemp;

	UINT i = 0, uNumNormal = pNormalBuffer->GetMemberCount();
	for (i=0; i<uNumNormal; ++i)
	{
		pNormalTemp = reinterpret_cast<CVector3*>( pNormalData + pNormalBuffer->GetStride()*i + uNormalOffset );
		pNormalTemp->x = pNormalTemp->y = pNormalTemp->z = 0.0f;
	}

	UINT uPolyCount = m_pIndexBuffer->GetMemberCount()/3;
	UINT uIndexOffset = m_pIndexBuffer->GetFormat() == INDEXBUFFER::BIT16 ? 16 : 32;

	PUINT8 pIndexData = static_cast<PUINT8>( m_pIndexBuffer->Lock( ALCHEMY_FLAG(INDEXBUFFER::READ_ONLY) ) );
	if(!pIndexData)
	{
		ALCHEMY_DEBUG_WARNING("CMesh::ComputeTangent: Lock Index Stream error.");
		return false;
	}

	UINT uIndices[3] = {0};
	FLOAT4 PosTemp[3];
	FLOAT3 normalTemp[3];
	CVector3 normalVal;
	
	for (i=0; i<uPolyCount; ++i)
	{
		if(uIndexOffset == 16)
			uIndices[0] = *(UINT16*)( pIndexData + (i*3)*m_pIndexBuffer->GetStride() );
		else
			uIndices[0] = *(UINT*)( pIndexData + (i*3)*m_pIndexBuffer->GetStride() );

		if(uIndexOffset == 16)
			uIndices[1] = *(UINT16*)( pIndexData + (i*3+1)*m_pIndexBuffer->GetStride() );
		else
			uIndices[1] = *(UINT*)( pIndexData + (i*3+1)*m_pIndexBuffer->GetStride() );

		if(uIndexOffset == 16)
			uIndices[2] = *(UINT16*)( pIndexData + (i*3+2)*m_pIndexBuffer->GetStride() );
		else
			uIndices[2] = *(UINT*)( pIndexData + (i*3+2)*m_pIndexBuffer->GetStride() );

		PosTemp[0] = *(FLOAT4*)( pPosData + pPosBuffer->GetStride() * uIndices[0] + uPosOffset );
		PosTemp[1] = *(FLOAT4*)( pPosData + pPosBuffer->GetStride() * uIndices[1] + uPosOffset );
		PosTemp[2] = *(FLOAT4*)( pPosData + pPosBuffer->GetStride() * uIndices[2] + uPosOffset );

		normalTemp[0].x = PosTemp[1].x - PosTemp[0].x;
		normalTemp[0].y = PosTemp[1].y - PosTemp[0].y;
		normalTemp[0].z = PosTemp[1].z - PosTemp[0].z;

		normalTemp[1].x = PosTemp[2].x - PosTemp[0].x;
		normalTemp[1].y = PosTemp[2].y - PosTemp[0].y;
		normalTemp[1].z = PosTemp[2].z - PosTemp[0].z;

		normalVal.CrossProduct(normalTemp[0], normalTemp[1]);
		normalVal.Normalize();

		pNormalTemp = reinterpret_cast<CVector3*>( pNormalData + pNormalBuffer->GetStride()*uIndices[0] + uNormalOffset );
		pNormalTemp->x += normalVal.x;
		pNormalTemp->y += normalVal.y;
		pNormalTemp->z += normalVal.z;

		pNormalTemp = reinterpret_cast<CVector3*>( pNormalData + pNormalBuffer->GetStride()*uIndices[1] + uNormalOffset );
		pNormalTemp->x += normalVal.x;
		pNormalTemp->y += normalVal.y;
		pNormalTemp->z += normalVal.z;

		pNormalTemp = reinterpret_cast<CVector3*>( pNormalData + pNormalBuffer->GetStride()*uIndices[2] + uNormalOffset );
		pNormalTemp->x += normalVal.x;
		pNormalTemp->y += normalVal.y;
		pNormalTemp->z += normalVal.z;
	}

	for (i=0; i<uNumNormal; ++i)
	{
		pNormalTemp = reinterpret_cast<CVector3*>( pNormalData + pNormalBuffer->GetStride()*i + uNormalOffset );
		pNormalTemp->Normalize();
	}

	return true;
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

	pPosData = static_cast<PUINT8>( pPosBuffer->Lock( ALCHEMY_FLAG(VERTEXBUFFER::READ_ONLY) ) );
	pIndexData = static_cast<PUINT8>( m_pIndexBuffer->Lock( ALCHEMY_FLAG(INDEXBUFFER::READ_ONLY) ) );

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

bool CMesh::ComputeBoundingVolume(IBoundingVolume& BoundingVolume, const ATTRIBUTERANGE* pAttributeRange)
{
	if(!m_pVertex)
	{
		ALCHEMY_DEBUG_WARNING("CMesh::ComputeBoundingVolume: Error mesh.");

		return false;
	}

	const CVertexDeclaration& VertexDeclaration = m_pVertex->GetVertexDeclaration();
	const CVertexDeclaration::VERTEXELEMENT* pVertexElements = VertexDeclaration.GetVertexElements();
	UINT uVertexElementCount = VertexDeclaration.GetVertexElementCount(), uPositionElement = CVertexDeclaration::FindVertexElement(CVertexDeclaration::POSITION, CVertexDeclaration::FLOAT3, 0, pVertexElements, uVertexElementCount);
	if(uPositionElement == uVertexElementCount)
		uPositionElement = CVertexDeclaration::FindVertexElement(CVertexDeclaration::POSITION, CVertexDeclaration::FLOAT4, 0, pVertexElements, uVertexElementCount);

	const CVertexDeclaration::VERTEXELEMENT* pPositionElement;
	if(uPositionElement < uVertexElementCount)
	{
		pPositionElement = &pVertexElements[uPositionElement];

		LPVERTEXBUFFER pVertexBuffer = m_pVertex->GetVertexBuffer(pPositionElement->uStream);
		PUINT8 puVertices = pVertexBuffer ? (static_cast<PUINT8>( pVertexBuffer->Lock(VERTEXBUFFER::READ_ONLY) ) + pPositionElement->uOffset) : ALCHEMY_NULL;
		if(puVertices)
		{
			UINT uStride = pVertexBuffer->GetStride();
			void* pIndices = m_pIndexBuffer ? m_pIndexBuffer->Lock(INDEXBUFFER::READ_ONLY) : ALCHEMY_NULL;
			if(pIndices)
			{
				UINT uIndexCount = m_pIndexBuffer->GetMemberCount();
				if(m_pIndexBuffer->GetFormat() == CIndexBuffer::BIT16)
				{
					PUINT16 puIndices = static_cast<PUINT16>(pIndices);
					if(puIndices)
					{
						if(pAttributeRange)
						{
							puIndices += pAttributeRange->uStartIndex;
							uIndexCount = pAttributeRange->uIndexCount;
						}

						for(UINT i = 0; i < uIndexCount; ++ i)
							BoundingVolume.Union( *reinterpret_cast<FLOAT3*>(puVertices + puIndices[i] * uStride) );
					}
				}
				else
				{
					PUINT32 puIndices = static_cast<PUINT32>(pIndices);
					if(puIndices)
					{
						if(pAttributeRange && pAttributeRange->uStartIndex + pAttributeRange->uIndexCount <= uIndexCount)
						{
							puIndices += pAttributeRange->uStartIndex;
							uIndexCount = pAttributeRange->uIndexCount;
						}

						for(UINT i = 0; i < uIndexCount; ++ i)
							BoundingVolume.Union( *reinterpret_cast<FLOAT3*>(puVertices + puIndices[i] * uStride) );
					}
				}

				m_pIndexBuffer->Unlock();
			}
			else
			{
				UINT uNumVertices = pVertexBuffer->GetMemberCount();
				for(UINT i = 0; i < uNumVertices; ++ i)
				{
					BoundingVolume.Union( *reinterpret_cast<FLOAT3*>(puVertices) );

					puVertices += uStride;
				}
			}

			pVertexBuffer->Unlock();
		}
		else
			ALCHEMY_DEBUG_WARNING("CMesh::ComputeBoundingVolume: Error vertex buffer.");
	}
	else
		ALCHEMY_DEBUG_WARNING("CMesh::ComputeBoundingVolume: Position element can not be find.");

	return false;
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

				pDestinationVertices = reinterpret_cast<PUINT8>( pDestinationVertexBuffer->Lock(0) );
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

				pDestinationVertices = reinterpret_cast<PUINT8>( pDestinationVertexBuffer->Lock(0) );
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

CMesh* CMesh::ConvertToInstancedMesh(FLOAT fInstanceCount, 
									 const MATRIX4X4* pTransform, 
									 const CEffectCode::ELEMENTDECLARATION* pTransformElements,
									 UINT uTransformElementCount,
									 const CVertex::INSTANCEDATA* pInstanceData, 
									 UINT uInstanceDataCount)
{
	if(!m_pVertex || !m_pIndexBuffer)
	{
		ALCHEMY_DEBUG_WARNING("CMesh::ConvertToInstanceMesh: Error mesh.");

		return ALCHEMY_NULL;
	}
	
	LPVERTEXBUFFER pVertexBuffer = m_pVertex->GetVertexBuffer(0);
	if(!pVertexBuffer)
	{
		ALCHEMY_DEBUG_WARNING("CMesh::ConvertToInstanceMesh: Error mesh.");

		return ALCHEMY_NULL;
	}

	CMesh* pOutput = Clone(fInstanceCount, pTransform, pTransformElements, uTransformElementCount);
	if(pOutput)
	{
		UINT uNumVertices = pVertexBuffer->GetMemberCount(), uInstanceIndexSegment = uNumVertices, uInstanceCount = static_cast<UINT> (fInstanceCount);
		PUINT puInstanceIndexSegments;
		ALCHEMY_DEBUG_NEW(puInstanceIndexSegments, UINT[uInstanceCount]);
		for(UINT i = 0; i < uInstanceCount; ++ i)
		{
			puInstanceIndexSegments[i] = uInstanceIndexSegment;
			
			uInstanceIndexSegment += uNumVertices;
		}
		
		if(pOutput->m_pVertex)
		{
			CVertex* pVertex = pOutput->m_pVertex->ConvertToInstancedVertex(puInstanceIndexSegments, uInstanceCount, pInstanceData, uInstanceDataCount);
			ALCHEMY_DEBUG_DELETE_ARRAY(puInstanceIndexSegments);

			if(pOutput->m_pVertex)
				pOutput->m_pVertex->Release();
				
			pOutput->m_pVertex = pVertex;
			
			if( pOutput->_Create() )
				return pOutput;

			ALCHEMY_DEBUG_WARNING("CMesh::ConvertToInstancedMesh: Mesh has been created fail.");
				
			GetGraphicsResourcePool().DestroyResource(*pOutput);
		}
		else
			ALCHEMY_DEBUG_WARNING("CMesh::ConvertToInstancedMesh: Mesh has been cloned fail.");
	}
	
	return ALCHEMY_NULL;

	/*CMesh* pOutput = static_cast<CMesh*>( CHighLevelGraphicsResource::_Clone() );

	if(pOutput)
	{
		UINT uNumVertices = 0;
		pOutput->m_pVertex = m_pVertex->ConvertToInstancedVertex(fInstanceCount, pInstanceData, uInstanceDataCount, &uNumVertices);
		
		if(!pOutput->m_pVertex)
		{
			ALCHEMY_DEBUG_WARNING("CMesh::ConvertToInstanceMesh: Vertex has been created fail.");

			static_cast<CSceneManager&>( GetGraphicsResourcePool() ).DestroyResource(*pOutput);

			return ALCHEMY_NULL;
		}
		
		_AddRef(*pOutput->m_pVertex);
		
		CSceneManager& SceneManager = static_cast<CSceneManager&> ( GetGraphicsResourcePool() );
		CRenderSystem& RenderSystem = SceneManager.GetRenderSystem();
		
		CIndexBuffer::FORMAT IndexFormat = m_pIndexBuffer->GetFormat(), NewIndexFormat = IndexFormat;
		if(IndexFormat == CIndexBuffer::BIT16 && uNumVertices > 0xffff)
		{
			if( RenderSystem.CheckFormat(CRenderSystem::INDEX_BUFFER, CIndexBuffer::BIT32) )
				NewIndexFormat = CIndexBuffer::BIT32;
			else 
			{
				ALCHEMY_DEBUG_WARNING("CMesh::ConvertToInstanceMesh: Index out of range.");
				
				SceneManager.DestroyResource(*pOutput);

				return ALCHEMY_NULL;
			}
		}
		
		UINT uIndexCount = m_pIndexBuffer->GetMemberCount(), uNewIndexCount = static_cast<UINT>(uIndexCount * fInstanceCount);
		pOutput->m_pIndexBuffer = RenderSystem.CreateIndexBuffer(uNewIndexCount, m_pIndexBuffer->GetUsage(), NewIndexFormat);
		
		if(!pOutput->m_pIndexBuffer)
		{
			ALCHEMY_DEBUG_WARNING("CMesh::ConvertToInstanceMesh: Index buffer has been created fail.");

			SceneManager.DestroyResource(*pOutput);

			return ALCHEMY_NULL;
		}
		
		_AddRef(*pOutput->m_pIndexBuffer);
		
		uNumVertices = m_pVertex->GetVertexBuffer(0)->GetMemberCount();
		if(NewIndexFormat == IndexFormat)
		{
			if(NewIndexFormat == CIndexBuffer::BIT16)
			{
				const UINT16* puSourceData      = static_cast<const UINT16*>( m_pIndexBuffer         ->Lock( ALCHEMY_FLAG(INDEXBUFFER::READ_ONLY) ) );
				PUINT16       puDestinationData = static_cast<PUINT16      >( pOutput->m_pIndexBuffer->Lock( 0                                    ) );
				
				if(puSourceData && puDestinationData)
				{
					UINT i, j, uIndex = 0, uOffset = 0, uInstanceCount = static_cast<UINT>(fInstanceCount);
					for(i = 0; i < uInstanceCount; ++ i)
					{
						for(j = 0; j < uIndexCount; ++ j)
							puDestinationData[uIndex ++] = puSourceData[j] + uOffset;
							
						uOffset += uNumVertices;
					}
					
					uIndexCount = static_cast<UINT>( uIndexCount * (fInstanceCount - uInstanceCount) );
					for(i = 0; i < uIndexCount; ++ i)
						puDestinationData[uIndex ++] = puSourceData[i] + uOffset;
					
					m_pIndexBuffer         ->Unlock();
					pOutput->m_pIndexBuffer->Unlock();
				}
				else
				{
					ALCHEMY_DEBUG_WARNING("CMesh::ConvertToInstanceMesh: Index buffer has been lock fail.");

					SceneManager.DestroyResource(*pOutput);

					return ALCHEMY_NULL;
				}
			}
			else if(NewIndexFormat == CIndexBuffer::BIT32)
			{
				const UINT32* puSourceData      = static_cast<const UINT32*>( m_pIndexBuffer         ->Lock( ALCHEMY_FLAG(INDEXBUFFER::READ_ONLY) ) );
				PUINT32       puDestinationData = static_cast<PUINT32      >( pOutput->m_pIndexBuffer->Lock( 0                                    ) );
				
				if(puSourceData && puDestinationData)
				{
					UINT i, j, uIndex = 0, uOffset = 0, uInstanceCount = static_cast<UINT>(fInstanceCount);
					for(i = 0; i < uInstanceCount; ++ i)
					{
						for(j = 0; j < uIndexCount; ++ j)
							puDestinationData[uIndex ++] = puSourceData[j] + uOffset;
							
						uOffset += uNumVertices;
					}
					
					uIndexCount = static_cast<UINT>( uIndexCount * (fInstanceCount - uInstanceCount) );
					for(i = 0; i < uIndexCount; ++ i)
						puDestinationData[uIndex ++] = puSourceData[i] + uOffset;
					
					m_pIndexBuffer         ->Unlock();
					pOutput->m_pIndexBuffer->Unlock();
				}
				else
				{
					ALCHEMY_DEBUG_WARNING("CMesh::ConvertToInstanceMesh: Index buffer has been lock fail.");

					SceneManager.DestroyResource(*pOutput);

					return ALCHEMY_NULL;
				}
			}
		}
		else 
		{
			const UINT16* puSourceData      = static_cast<const UINT16*>( m_pIndexBuffer         ->Lock( ALCHEMY_FLAG(INDEXBUFFER::READ_ONLY) ) );
			PUINT32       puDestinationData = static_cast<PUINT32      >( pOutput->m_pIndexBuffer->Lock( 0                                    ) );
			
			if(puSourceData && puDestinationData)
			{
				UINT i, j, uIndex = 0, uOffset = 0, uInstanceCount = static_cast<UINT>(fInstanceCount);
				for(i = 0; i < uInstanceCount; ++ i)
				{
					for(j = 0; j < uIndexCount; ++ j)
						puDestinationData[uIndex ++] = puSourceData[j] + uOffset;
						
					uOffset += uNumVertices;
				}
				
				uIndexCount = static_cast<UINT>( uIndexCount * (fInstanceCount - uInstanceCount) );
				for(i = 0; i < uIndexCount; ++ i)
					puDestinationData[uIndex ++] = puSourceData[i] + uOffset;
				
				m_pIndexBuffer         ->Unlock();
				pOutput->m_pIndexBuffer->Unlock();
			}
			else
			{
				ALCHEMY_DEBUG_WARNING("CMesh::ConvertToInstanceMesh: Index buffer has been lock fail.");

				SceneManager.DestroyResource(*pOutput);

				return ALCHEMY_NULL;
			}
		}
		
		ATTRIBUTERANGE AttributeRange;
		AttributeRange.uStartIndex = 0;
		AttributeRange.uIndexCount = uNewIndexCount;
		pOutput->m_AttributeTable.Add(AttributeRange);
		
		if( pOutput->_Create() )
			return pOutput;
		
		SceneManager.DestroyResource(*pOutput);
	}
	
	ALCHEMY_DEBUG_WARNING("CMesh::ConvertToInstanceMesh: Mesh has been created fail.");
	
	return ALCHEMY_NULL;*/
}

CMesh* CMesh::ConvertToInstancedMesh(const ATTRIBUTERANGE* pAttributeRanges, 
									 UINT uAttributeRangeCount,
									 const VERTEXELEMENTTRANSFORM* pTransform, 
									 UINT uTransformCount, 
									 const CVertex::INSTANCEDATA* pInstanceData, 
									 UINT uInstanceDataCount)
{
	if(!uAttributeRangeCount || !uAttributeRangeCount)
		return ALCHEMY_NULL;
		
	if(!m_pVertex || !m_pIndexBuffer)
	{
		ALCHEMY_DEBUG_WARNING("CMesh::ConvertToBlendedMesh: Error mesh.");

		return ALCHEMY_NULL;
	}

	CMesh* pOutput = Clone(pAttributeRanges, uAttributeRangeCount, pTransform, uTransformCount);
	if(pOutput)
	{
		PUINT puInstanceIndexSegments;
		ALCHEMY_DEBUG_NEW(puInstanceIndexSegments, UINT[uAttributeRangeCount]);
		UINT uInstanceIndexSegmentCount = uAttributeRangeCount - 1, uInstanceIndexSegment = pAttributeRanges[0].uIndexCount;
		for(UINT i = 0; i < uInstanceIndexSegmentCount; ++ i)
		{
			puInstanceIndexSegments[i] = uInstanceIndexSegment;
			
			uInstanceIndexSegment += pAttributeRanges[i + 1].uIndexCount;
		}
		
		if(pOutput->m_pVertex)
		{
			CVertex* pVertex = pOutput->m_pVertex->ConvertToInstancedVertex(puInstanceIndexSegments, uInstanceIndexSegmentCount, pInstanceData, uInstanceDataCount);
			ALCHEMY_DEBUG_DELETE_ARRAY(puInstanceIndexSegments);

			if(pOutput->m_pVertex)
				pOutput->m_pVertex->Release();
				
			pOutput->m_pVertex = pVertex;
			
			if( pOutput->_Create() )
				return pOutput;

			ALCHEMY_DEBUG_WARNING("CMesh::ConvertToInstancedMesh: Mesh has been created fail.");
				
			GetGraphicsResourcePool().DestroyResource(*pOutput);
		}
		else
			ALCHEMY_DEBUG_WARNING("CMesh::ConvertToInstancedMesh: Mesh has been cloned fail.");
	}
	
	return ALCHEMY_NULL;
}

CMesh* CMesh::ConvertToBlendedMesh(const CSkinInfo& SkinInfo, 
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

		pOutput->m_pVertex = m_pVertex->ConvertToBlendedVertex(SkinInfo, 0, 0, uMatrixIndexOffset, &uMaxVertexInfluenceCount);

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

			PUINT8 puIndices = static_cast<PUINT8>( pOutput->m_pIndexBuffer->Lock( ALCHEMY_FLAG(INDEXBUFFER::READ_ONLY) ) );

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

bool CMesh::Intersect(
					  const RAY& Ray,
					  LPINTERSECTINFO pIntersectInfos, 
					  PUINT puIntersectInfoCount, 
					  bool bIsCheckHitOn,
					  bool bHitOnMask,
					  bool bIsCheckFront, 
					  bool bFrontMask, 
					  LPFLOAT3 pHitPoints, 
					  LPFLOAT3 pfFaceNormals)
{
	if(!m_pVertex || !m_pIndexBuffer)
	{
		ALCHEMY_DEBUG_WARNING("CMesh::Intersect: Error mesh.");
		
		return false;
	}
	
	CVertexDeclaration& VertexDeclaration = m_pVertex->GetVertexDeclaration();
	const CVertexDeclaration::VERTEXELEMENT* pVertexElements = VertexDeclaration.GetVertexElements();
	UINT uVertexElementCount = VertexDeclaration.GetVertexElementCount();
	
	UINT uPosition = CVertexDeclaration::FindVertexElement(CVertexDeclaration::POSITION, 0, pVertexElements, uVertexElementCount);
	if(uPosition == uVertexElementCount)
		uPosition = CVertexDeclaration::FindVertexElement(CVertexDeclaration::POSITIONT, 0, pVertexElements, uVertexElementCount);
	
	if(uPosition < uVertexElementCount)
	{
		const CVertexDeclaration::VERTEXELEMENT* pPositionElement = &pVertexElements[uPosition];
		LPVERTEXBUFFER pVertexBuffer = m_pVertex->GetVertexBuffer(pPositionElement->uStream);
		if(pVertexBuffer)
		{
			const UINT8* puVertices = static_cast<const UINT8*>( pVertexBuffer->Lock( ALCHEMY_FLAG(VERTEXBUFFER::READ_ONLY) ) );
			if(puVertices)
			{
				if(m_pIndexBuffer->GetFormat() == CIndexBuffer::BIT16)
				{
					const UINT16* puIndices = static_cast<const UINT16*>( m_pIndexBuffer->Lock( ALCHEMY_FLAG(INDEXBUFFER::READ_ONLY) ) );
					if(puIndices)
					{
						const FLOAT3* pA, * pB, * pC;
						UINT uIntersectInfoCount = 0, uMaxIntersectInfoCount = puIntersectInfoCount ? *puIntersectInfoCount : 0, uTriangleCount = CIndexBuffer::GetTriangleCount(m_pIndexBuffer->GetMemberCount(), CIndexBuffer::TRIANGLE_LIST), uStride = pVertexBuffer->GetStride(), uIndex = 0;
						if(pIntersectInfos && uMaxIntersectInfoCount)
						{
							/*if(uMaxIntersectInfoCount == 1)
							{
								UINT uMinDistance = ~0;
								FLOAT u, v, t;
								for(UINT i = 0; i < uTriangleCount; ++ i)
								{
									if( HitTriangle(Ray, 
													*reinterpret_cast<const FLOAT3*>(puVertices + puIndices[uIndex ++] * uStride + pPositionElement->uOffset), 
													*reinterpret_cast<const FLOAT3*>(puVertices + puIndices[uIndex ++] * uStride + pPositionElement->uOffset), 
													*reinterpret_cast<const FLOAT3*>(puVertices + puIndices[uIndex ++] * uStride + pPositionElement->uOffset), 
													ALCHEMY_NULL, 
													ALCHEMY_NULL, 
													ALCHEMY_NULL, 
													&u, 
													&v, 
													&t) && uMinDistance > t )
									{
										pIntersectInfos->uFaceIndex = i;
										pIntersectInfos->u          = u;
										pIntersectInfos->v          = v;
										pIntersectInfos->t          = t;
										
										uMinDistance                = t;
										
										uIntersectInfoCount         = 1;
									}
								}
							}
							else
							{*/
							if(bIsCheckHitOn && bIsCheckFront)
							{
								for(UINT i = 0; i < uTriangleCount; ++ i)
								{
									pA = reinterpret_cast<const FLOAT3*>(puVertices + puIndices[uIndex ++] * uStride + pPositionElement->uOffset);
									pB = reinterpret_cast<const FLOAT3*>(puVertices + puIndices[uIndex ++] * uStride + pPositionElement->uOffset);
									pC = reinterpret_cast<const FLOAT3*>(puVertices + puIndices[uIndex ++] * uStride + pPositionElement->uOffset);
									if( HitTriangle(Ray, 
										*pA, 
										*pB, 
										*pC,
										pHitPoints ? pHitPoints + uIntersectInfoCount : ALCHEMY_NULL, 
										&bIsCheckHitOn, 
										&bIsCheckFront, 
										&pIntersectInfos->u, 
										&pIntersectInfos->v, 
										&pIntersectInfos->t) && bIsCheckHitOn == bHitOnMask && bIsCheckFront == bFrontMask)
									{
										pIntersectInfos->uFaceIndex = i;

										if(pfFaceNormals)
											alchemy::ComputeNormal(*(pfFaceNormals + uIntersectInfoCount), *pA, *pB, *pC);

										++ pIntersectInfos;

										++ uIntersectInfoCount;

										if(uIntersectInfoCount == uMaxIntersectInfoCount)
											break;
									}
								}
							}
							else if(bIsCheckHitOn)
							{
								for(UINT i = 0; i < uTriangleCount; ++ i)
								{
									pA = reinterpret_cast<const FLOAT3*>(puVertices + puIndices[uIndex ++] * uStride + pPositionElement->uOffset);
									pB = reinterpret_cast<const FLOAT3*>(puVertices + puIndices[uIndex ++] * uStride + pPositionElement->uOffset);
									pC = reinterpret_cast<const FLOAT3*>(puVertices + puIndices[uIndex ++] * uStride + pPositionElement->uOffset);
									if( HitTriangle(Ray, 
										*pA, 
										*pB, 
										*pC, 
										pHitPoints ? pHitPoints + uIntersectInfoCount : ALCHEMY_NULL, 
										&bIsCheckHitOn, 
										ALCHEMY_NULL, 
										&pIntersectInfos->u, 
										&pIntersectInfos->v, 
										&pIntersectInfos->t) && bIsCheckHitOn == bHitOnMask)
									{
										pIntersectInfos->uFaceIndex = i;

										if(pfFaceNormals)
											alchemy::ComputeNormal(*(pfFaceNormals + uIntersectInfoCount), *pA, *pB, *pC);

										++ pIntersectInfos;

										++ uIntersectInfoCount;

										if(uIntersectInfoCount == uMaxIntersectInfoCount)
											break;
									}
								}
							}
							else if(bIsCheckFront)
							{
								for(UINT i = 0; i < uTriangleCount; ++ i)
								{
									pA = reinterpret_cast<const FLOAT3*>(puVertices + puIndices[uIndex ++] * uStride + pPositionElement->uOffset);
									pB = reinterpret_cast<const FLOAT3*>(puVertices + puIndices[uIndex ++] * uStride + pPositionElement->uOffset);
									pC = reinterpret_cast<const FLOAT3*>(puVertices + puIndices[uIndex ++] * uStride + pPositionElement->uOffset);
									if( HitTriangle(Ray, 
										*pA, 
										*pB, 
										*pC, 
										pHitPoints ? pHitPoints + uIntersectInfoCount : ALCHEMY_NULL, 
										ALCHEMY_NULL, 
										&bIsCheckFront, 
										&pIntersectInfos->u, 
										&pIntersectInfos->v, 
										&pIntersectInfos->t) && bIsCheckFront == bFrontMask)
									{
										pIntersectInfos->uFaceIndex = i;

										if(pfFaceNormals)
											alchemy::ComputeNormal(*(pfFaceNormals + uIntersectInfoCount), *pA, *pB, *pC);

										++ pIntersectInfos;

										++ uIntersectInfoCount;

										if(uIntersectInfoCount == uMaxIntersectInfoCount)
											break;
									}
								}
							}
							else
							{
								for(UINT i = 0; i < uTriangleCount; ++ i)
								{
									pA = reinterpret_cast<const FLOAT3*>(puVertices + puIndices[uIndex ++] * uStride + pPositionElement->uOffset);
									pB = reinterpret_cast<const FLOAT3*>(puVertices + puIndices[uIndex ++] * uStride + pPositionElement->uOffset);
									pC = reinterpret_cast<const FLOAT3*>(puVertices + puIndices[uIndex ++] * uStride + pPositionElement->uOffset);
									if( HitTriangle(Ray, 
										*pA, 
										*pB, 
										*pC, 
										pHitPoints ? pHitPoints + uIntersectInfoCount : ALCHEMY_NULL, 
										ALCHEMY_NULL, 
										ALCHEMY_NULL, 
										&pIntersectInfos->u, 
										&pIntersectInfos->v, 
										&pIntersectInfos->t) )
									{
										pIntersectInfos->uFaceIndex = i;

										if(pfFaceNormals)
											alchemy::ComputeNormal(*(pfFaceNormals + uIntersectInfoCount), *pA, *pB, *pC);
										
										++ pIntersectInfos;
										
										++ uIntersectInfoCount;
										
										if(uIntersectInfoCount == uMaxIntersectInfoCount)
											break;
									}
								}
							}
							//}
						}
						else
						{
							uMaxIntersectInfoCount = uMaxIntersectInfoCount ? uMaxIntersectInfoCount : 1;

							if(bIsCheckHitOn && bIsCheckFront)
							{
								for(UINT i = 0; i < uTriangleCount; ++ i)
								{
									pA = reinterpret_cast<const FLOAT3*>(puVertices + puIndices[uIndex ++] * uStride + pPositionElement->uOffset);
									pB = reinterpret_cast<const FLOAT3*>(puVertices + puIndices[uIndex ++] * uStride + pPositionElement->uOffset);
									pC = reinterpret_cast<const FLOAT3*>(puVertices + puIndices[uIndex ++] * uStride + pPositionElement->uOffset);
									if( HitTriangle(Ray, 
										*pA, 
										*pB, 
										*pC, 
										pHitPoints ? pHitPoints + uIntersectInfoCount : ALCHEMY_NULL, 
										&bIsCheckHitOn, 
										&bIsCheckFront) && bIsCheckHitOn == bHitOnMask && bIsCheckFront == bFrontMask)
									{
										if(pfFaceNormals)
											alchemy::ComputeNormal(*(pfFaceNormals + uIntersectInfoCount), *pA, *pB, *pC);

										++ uIntersectInfoCount;

										if(uIntersectInfoCount == uMaxIntersectInfoCount)
											break;
									}
								}
							}
							else if(bIsCheckHitOn)
							{
								for(UINT i = 0; i < uTriangleCount; ++ i)
								{
									pA = reinterpret_cast<const FLOAT3*>(puVertices + puIndices[uIndex ++] * uStride + pPositionElement->uOffset);
									pB = reinterpret_cast<const FLOAT3*>(puVertices + puIndices[uIndex ++] * uStride + pPositionElement->uOffset);
									pC = reinterpret_cast<const FLOAT3*>(puVertices + puIndices[uIndex ++] * uStride + pPositionElement->uOffset);
									if( HitTriangle(Ray, 
										*pA, 
										*pB, 
										*pC, 
										pHitPoints ? pHitPoints + uIntersectInfoCount : ALCHEMY_NULL, 
										&bIsCheckHitOn) && bIsCheckHitOn == bHitOnMask)
									{
										if(pfFaceNormals)
											alchemy::ComputeNormal(*(pfFaceNormals + uIntersectInfoCount), *pA, *pB, *pC);

										++ uIntersectInfoCount;

										if(uIntersectInfoCount == uMaxIntersectInfoCount)
											break;
									}
								}
							}
							else if(bIsCheckFront)
							{
								for(UINT i = 0; i < uTriangleCount; ++ i)
								{
									pA = reinterpret_cast<const FLOAT3*>(puVertices + puIndices[uIndex ++] * uStride + pPositionElement->uOffset);
									pB = reinterpret_cast<const FLOAT3*>(puVertices + puIndices[uIndex ++] * uStride + pPositionElement->uOffset);
									pC = reinterpret_cast<const FLOAT3*>(puVertices + puIndices[uIndex ++] * uStride + pPositionElement->uOffset);
									if( HitTriangle(Ray, 
										*pA, 
										*pB, 
										*pC, 
										pHitPoints ? pHitPoints + uIntersectInfoCount : ALCHEMY_NULL, 
										ALCHEMY_NULL, 
										&bIsCheckFront) && bIsCheckFront == bFrontMask )
									{
										if(pfFaceNormals)
											alchemy::ComputeNormal(*(pfFaceNormals + uIntersectInfoCount), *pA, *pB, *pC);

										++ uIntersectInfoCount;

										if(uIntersectInfoCount == uMaxIntersectInfoCount)
											break;
									}
								}
							}
							else
							{
								for(UINT i = 0; i < uTriangleCount; ++ i)
								{
									pA = reinterpret_cast<const FLOAT3*>(puVertices + puIndices[uIndex ++] * uStride + pPositionElement->uOffset);
									pB = reinterpret_cast<const FLOAT3*>(puVertices + puIndices[uIndex ++] * uStride + pPositionElement->uOffset);
									pC = reinterpret_cast<const FLOAT3*>(puVertices + puIndices[uIndex ++] * uStride + pPositionElement->uOffset);
									if( HitTriangle(Ray, 
										*pA, 
										*pB, 
										*pC, 
										pHitPoints ? pHitPoints + uIntersectInfoCount : ALCHEMY_NULL) )
									{
										if(pfFaceNormals)
											alchemy::ComputeNormal(*(pfFaceNormals + uIntersectInfoCount), *pA, *pB, *pC);

										++ uIntersectInfoCount;
										
										if(uIntersectInfoCount == uMaxIntersectInfoCount)
											break;
									}
								}
							}
						}
						
						m_pIndexBuffer->Unlock();

						pVertexBuffer->Unlock();
						
						if(puIntersectInfoCount)
							*puIntersectInfoCount = uIntersectInfoCount;
							
						return uIntersectInfoCount ? true : false;
					}
					else
						ALCHEMY_DEBUG_WARNING("CMesh::Intersect: Index buffer has been lock fail.");
				}
				
				pVertexBuffer->Unlock();
			}
			else
				ALCHEMY_DEBUG_WARNING("CMesh::Intersect: Vertex buffer has been lock fail.");
		}
		else
			ALCHEMY_DEBUG_WARNING("CMesh::Intersect: Vertex buffer can not be find.");
	}
	else
		ALCHEMY_DEBUG_WARNING("CMesh::Intersect: Error Mesh.");
		
	return false;
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