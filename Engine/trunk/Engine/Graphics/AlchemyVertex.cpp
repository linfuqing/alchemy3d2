#include "AlchemyVertex.h"
#include "AlchemySceneManager.h"
#include "../Core/AlchemyUtils.h"
#include "../Core/AlchemyMemory.h"

using namespace alchemy;
CVertex::CVertex(void):
m_pVertexDeclaration(ALCHEMY_NULL),
m_ppVertexBuffer(ALCHEMY_NULL)
{
}

CVertex::~CVertex(void)
{
}

CVertex* CVertex::Clone(const CVertexDeclaration::VERTEXELEMENT* pCloneElements, UINT uCloneElementCount, const UINT* puUsage)
{
	if(!m_pVertexDeclaration || !m_ppVertexBuffer)
	{
		ALCHEMY_DEBUG_WARNING("CVertex::Clone: Vertex has not been created.");

		return ALCHEMY_NULL;
	}

	const CVertexDeclaration::VERTEXELEMENT* pSourceElement = m_pVertexDeclaration->GetVertexElements();
	UINT uSourceElementCount = m_pVertexDeclaration->GetVertexElementCount(), uNumVertices = m_ppVertexBuffer[0]->GetMemberCount();

	CVertex* pOutput = ALCHEMY_NULL;
	
	pOutput = pCloneElements ? 
		static_cast<CSceneManager&>( GetGraphicsResourcePool() ).CreateVertex(uNumVertices, pCloneElements, uCloneElementCount , puUsage):
		static_cast<CSceneManager&>( GetGraphicsResourcePool() ).CreateVertex(uNumVertices, pSourceElement, uSourceElementCount, puUsage);

	if(pOutput)
	{
		const CVertexDeclaration::VERTEXELEMENT* pCloneElement;
		LPVERTEXBUFFER pCloneVertexBuffer, pSourceVertexBuffer;
		UINT uIndex, i, j, uCloneStride, uSourceStride;
		const UINT8* puSourceVertices;
		PUINT8 puCloneVertices;

		for(i = 0; i < uCloneElementCount; ++ i)
		{
			pCloneElement = &pCloneElements[i];

			uIndex = CVertexDeclaration::FindVertexElement(pCloneElement->Usage, pCloneElement->Type, 0, pSourceElement, uSourceElementCount);

			if(uIndex < uSourceElementCount)
			{
				pSourceElement = &pSourceElement[uIndex];

				pSourceVertexBuffer = m_ppVertexBuffer[        pSourceElement->uStream];
				pCloneVertexBuffer  = pOutput->m_ppVertexBuffer[pCloneElement->uStream];

				uSourceStride = pSourceVertexBuffer->GetStride();
				uCloneStride  = pCloneVertexBuffer->GetStride();

				puSourceVertices = static_cast<const UINT8*>( pSourceVertexBuffer->Lock( ALCHEMY_FLAG( VERTEXBUFFER::READ_ONLY ) ) );
				puCloneVertices  = static_cast<PUINT8>( pCloneVertexBuffer ->Lock(0                                        ) );

				if(puSourceVertices && puCloneVertices)
				{
					for(j = 0; j < uNumVertices; ++ j)
					{
						ALCHEMY_MEMORY_COPY(puCloneVertices + pCloneElement->uOffset, puSourceVertices + pSourceElement->uOffset, pCloneElement->Type);

						puSourceVertices += uSourceStride;
						puCloneVertices  += uCloneStride;
					}
					
					pSourceVertexBuffer->Unlock();
					pCloneVertexBuffer->Unlock();
				}
				else 
				{
					ALCHEMY_DEBUG_WARNING("CVertex::Clone: Vertex buffer has lock fail.");
					
					GetGraphicsResourcePool().DestroyResource(*pOutput);
					
					return ALCHEMY_NULL;
				}
			}
		}

		/*UINT i, uStreamCount = m_pVertexDeclaration->GetStreamCount();

		ALCHEMY_DEBUG_NEW(pOutput->m_ppVertexBuffer, LPVERTEXBUFFER[uStreamCount]);

		pOutput->m_pVertexDeclaration = m_pVertexDeclaration;

		m_pVertexDeclaration->AddRef();

		LPVERTEXBUFFER pVertexBuffer;
		for(i = 0; i < uStreamCount; ++ i)
		{
			pVertexBuffer = m_ppVertexBuffer[i];

			pOutput->m_ppVertexBuffer[i] = pVertexBuffer;

			if(pVertexBuffer)
				pVertexBuffer->AddRef();
		}

		const CVertexDeclaration::VERTEXELEMENT* pCloneVertexElement, * pCurrentVertexElement, * pTemp, * pSourceVertexElement = m_pVertexDeclaration->GetVertexElements();
		UINT uIndex, uElementCount = m_pVertexDeclaration->GetElementCount();
		PVOID pVertices;
		for(i = 0; i < uElementCount; ++ i)
		{
			pCloneVertexElement = &pCloneElements[i];

			uIndex = 0;
			while(uIndex < uElementCount)
			{
				pTemp = pCurrentVertexElement = pSourceVertexElement + uIndex;

				if(pTemp->uOffset == pCloneVertexElement->uOffset && pTemp->uStream == pCloneVertexElement->uStream)
					break;

				uIndex = CVertexDeclaration::FindVertexElement(pCloneVertexElement->Usage, pCloneVertexElement->Type, pCurrentVertexElement, uElementCount -= uIndex);
			}

			uElementCount = m_pVertexDeclaration->GetElementCount();

			if(uIndex < uElementCount)
			{
				pVertexBuffer = m_ppVertexBuffer[pTemp->uStream];

				if(pVertexBuffer)
				{
					pVertices = pVertexBuffer->Lock(VERTEXBUFFER::READ_ONLY);
					pOutput->m_ppVertexBuffer[pTemp->uStream] = static_cast<CSceneManager&>( GetGraphicsResourcePool() ).GetRenderSystem().CreateVertexBuffer(
						pVertexBuffer->GetMemberCount(), pVertexBuffer->GetStride(), puUsage ? puUsage[pTemp->uStream] : pVertexBuffer->GetUsage(), pVertices);
					pVertexBuffer->Unlock();

					pVertexBuffer->Release();

					if(!pOutput->m_ppVertexBuffer[pTemp->uStream])
					{
						GetGraphicsResourcePool().DestroyResource(*pOutput);

						return ALCHEMY_NULL;
					}
				}
			}
		}*/

		if( pOutput->_Create() )
			return pOutput;

		GetGraphicsResourcePool().DestroyResource(*pOutput);
	}

	return pOutput;
}

CVertex* CVertex::Clone(const UINT* puCloneStream, UINT uCloneStreamCount, const UINT* puUsage)
{
	if(!m_pVertexDeclaration || !m_ppVertexBuffer)
	{
		ALCHEMY_DEBUG_WARNING("CVertex::Clone::Vertex has not been created.");

		return ALCHEMY_NULL;
	}

	CVertex* pOutput = static_cast<CVertex*>( CHighLevelGraphicsResource::_Clone() );

	if(pOutput)
	{
		pOutput->m_pVertexDeclaration = m_pVertexDeclaration;

		_AddRef(*m_pVertexDeclaration);

		m_pVertexDeclaration->AddRef();

		UINT uStreamCount = m_pVertexDeclaration->GetStreamCount();

		ALCHEMY_DEBUG_NEW(pOutput->m_ppVertexBuffer, LPVERTEXBUFFER[uStreamCount]);

		LPVERTEXBUFFER pVertexBuffer;
		UINT i;
		for(i = 0; i < uStreamCount; ++ i)
		{
			pVertexBuffer = m_ppVertexBuffer[i];

			pOutput->m_ppVertexBuffer[i] = pVertexBuffer;

			if(pVertexBuffer)
			{
				_AddRef(*pVertexBuffer);

				pVertexBuffer->AddRef();
			}
		}

		UINT uStream;
		void* pVertices;
		for(i = 0; i < uCloneStreamCount; ++ i)
		{
			uStream = puCloneStream[i];

			if(uStream < uStreamCount)
			{
				pVertexBuffer = m_ppVertexBuffer[uStream];

				if(pVertexBuffer)
				{
					pVertices = pVertexBuffer->Lock( ALCHEMY_FLAG(VERTEXBUFFER::READ_ONLY) );
					pOutput->m_ppVertexBuffer[uStream] = static_cast<CSceneManager&>( GetGraphicsResourcePool() ).GetRenderSystem().CreateVertexBuffer(
						pVertexBuffer->GetMemberCount(), pVertexBuffer->GetStride(), puUsage ? puUsage[uStream] : pVertexBuffer->GetUsage(), pVertices);
					pVertexBuffer->Unlock();

					_Release(*pVertexBuffer);

					if(!pOutput->m_ppVertexBuffer[uStream])
					{
						ALCHEMY_DEBUG_WARNING("CVertex::Clone: Vertex buffer has been created fail.");

						GetGraphicsResourcePool().DestroyResource(*pOutput);

						return ALCHEMY_NULL;
					}

					_AddRef(*pOutput->m_ppVertexBuffer[uStream]);
				}
			}
		}
	}

	return pOutput;
}

CVertex* CVertex::Clone(FLOAT fCloneInstanceCount, PUINT puNumVertices)
{
	if(fCloneInstanceCount < 0.0f)
		return ALCHEMY_NULL;
	
	LPVERTEXBUFFER pVertexBuffer;
	
	if(!m_ppVertexBuffer || !(pVertexBuffer = m_ppVertexBuffer[0]) || !m_pVertexDeclaration)
	{
		ALCHEMY_DEBUG_WARNING("CVertex::Clone: Error vertex.");
		
		return ALCHEMY_NULL;
	}
	
	UINT uNumVertices = pVertexBuffer->GetMemberCount(), uNumNewVertices = static_cast<UINT>(uNumVertices * fCloneInstanceCount);
	
	if(puNumVertices)
		*puNumVertices = uNumNewVertices;
	
	UINT uStreamCount = m_pVertexDeclaration->GetStreamCount();
	PUINT puUsage;
	ALCHEMY_DEBUG_NEW(puUsage, UINT[uStreamCount]);
	
	UINT i;
	for(i = 0; i < uStreamCount; ++ i)
	{
		pVertexBuffer = m_ppVertexBuffer[i];
		if(pVertexBuffer)
			puUsage[i] = pVertexBuffer->GetUsage();
		else
		{
			ALCHEMY_DEBUG_WARNING("CVertex::Clone: Error Vertex.");
			
			ALCHEMY_DEBUG_DELETE_ARRAY(puUsage);
			
			return ALCHEMY_NULL;
		}
	}
	
	CSceneManager& SceneManager = static_cast<CSceneManager&>( GetGraphicsResourcePool() );
	CVertex* pOutput = SceneManager.CreateVertex(uNumNewVertices, m_pVertexDeclaration->GetResourceHandle(), puUsage);
	ALCHEMY_DEBUG_DELETE_ARRAY(puUsage);
	
	if(pOutput)
	{
		LPVERTEXBUFFER pSourceVertexBuffer, pDestinationVertexBuffer;
		const UINT8* puSourceData;
		PUINT8 puDestinationData;
		UINT uByteSize, j, uInstanceCount = static_cast<UINT>(fCloneInstanceCount);
		for(i = 0; i < uStreamCount; ++ i)
		{
			pSourceVertexBuffer      = m_ppVertexBuffer[i];
			pDestinationVertexBuffer = pOutput->m_ppVertexBuffer[i];
			if(pSourceVertexBuffer && pDestinationVertexBuffer)
			{
				uByteSize         = pSourceVertexBuffer->GetByteSize();
				
				puSourceData      = static_cast<const UINT8*>( pSourceVertexBuffer     ->Lock( ALCHEMY_FLAG(VERTEXBUFFER::READ_ONLY) ) );
				puDestinationData = static_cast<PUINT8      >( pDestinationVertexBuffer->Lock( 0                                     ) );
				
				if(puSourceData && puDestinationData)
				{
					for(j = 0; j < uInstanceCount; ++ j)
					{
						ALCHEMY_MEMORY_COPY(puDestinationData, puSourceData, uByteSize);
						
						puDestinationData += uByteSize;
					}
					
					uByteSize = static_cast<UINT>( (fCloneInstanceCount - uInstanceCount) * pSourceVertexBuffer->GetMemberCount() + 0.5f );
					if(uByteSize)
						ALCHEMY_MEMORY_COPY(puDestinationData, puSourceData, uByteSize);
				}
				
				pSourceVertexBuffer     ->Unlock();
				pDestinationVertexBuffer->Unlock();
			}
		}
		
		if( pOutput->_Create() )
			return pOutput;
		
		SceneManager.DestroyResource(*pOutput);
	}
	
	ALCHEMY_DEBUG_WARNING("CVertex::Clone: Vertex has been created fail.");
	
	return ALCHEMY_NULL;
}

CVertex* CVertex::ConvertToInstancedVertex(const UINT* puInstanceIndexSegments, UINT uInstanceCount, const INSTANCEDATA* pInstanceData, UINT uInstanceDataCount)
{
	LPVERTEXBUFFER pVertexBuffer;
	
	if(!m_ppVertexBuffer || !(pVertexBuffer = m_ppVertexBuffer[0]) || !m_pVertexDeclaration)
	{
		ALCHEMY_DEBUG_WARNING("CVertex::ConvertToBlendedVertex: Error vertex.");
		
		return ALCHEMY_NULL;
	}
	
	UINT uStreamCount = m_pVertexDeclaration->GetStreamCount();
	PUINT puUsage;
	ALCHEMY_DEBUG_NEW(puUsage, UINT[uStreamCount + 1]);
	
	UINT i;
	for(i = 0; i < uStreamCount; ++ i)
	{
		pVertexBuffer = m_ppVertexBuffer[i];
		if(pVertexBuffer)
			puUsage[i] = pVertexBuffer->GetUsage();
		else
		{
			ALCHEMY_DEBUG_WARNING("CVertex::ConvertToInstanceVertex: Error Vertex.");
			
			ALCHEMY_DEBUG_DELETE_ARRAY(puUsage);
			
			return ALCHEMY_NULL;
		}
	}
	
	UINT uVertexElementCount = m_pVertexDeclaration->GetVertexElementCount(), uNewVertexElementCount = uVertexElementCount + uInstanceDataCount + 1, uNewStreamCount = uStreamCount;
	const CVertexDeclaration::VERTEXELEMENT* pVertexElements = m_pVertexDeclaration->GetVertexElements();
	CVertexDeclaration::LPVERTEXELEMENT pNewVertexElements, pVertexElement;
	ALCHEMY_DEBUG_NEW(pNewVertexElements, CVertexDeclaration::VERTEXELEMENT[uNewVertexElementCount]);
	
	for(i = 0; i < uVertexElementCount; ++ i)
		pNewVertexElements[i] = pVertexElements[i];
	
	const INSTANCEDATA* pCurrentInstanceData;
	for(i = 0; i < uInstanceDataCount; ++ i)
	{
		pCurrentInstanceData = &pInstanceData[i];
		
		pVertexElement = &pNewVertexElements[uVertexElementCount];
		
		pVertexElement->uStream = uNewStreamCount;
		pVertexElement->uOffset = 0;
		pVertexElement->Type    = pCurrentInstanceData->ElementType;
		pVertexElement->Usage   = pCurrentInstanceData->ElementUsage;
		pVertexElement->uIndex  = uVertexElementCount;
		
		puUsage[uNewStreamCount]   = ALCHEMY_FLAG(VERTEXBUFFER::WRITE_ONLY);
		
		++ uNewStreamCount;
		++ uVertexElementCount;
	}
	
	pVertexElement = &pNewVertexElements[uVertexElementCount];
	
	pVertexElement->uStream = uNewStreamCount;
	pVertexElement->uOffset = 0;
	pVertexElement->Type    = CVertexDeclaration::FLOAT1;
	pVertexElement->Usage   = CVertexDeclaration::INSTANCEINDEX;
	pVertexElement->uIndex  = uVertexElementCount;
	
	puUsage[uNewStreamCount]   = ALCHEMY_FLAG(VERTEXBUFFER::WRITE_ONLY);
	
	CVertex* pOutput = Clone(pNewVertexElements, uNewVertexElementCount, puUsage);
	ALCHEMY_DEBUG_DELETE_ARRAY(puUsage);
	ALCHEMY_DEBUG_DELETE_ARRAY(pNewVertexElements);
	if(pOutput)
	{
		CSceneManager& SceneManager = static_cast<CSceneManager&>( GetGraphicsResourcePool() );
		
		pVertexBuffer = pOutput->m_ppVertexBuffer[uNewStreamCount];
		if(pVertexBuffer)
		{
			UINT uInstanceIndex = 0, uInstanceIndexSegment = puInstanceIndexSegments[uInstanceIndex], uNewNumVertices = pVertexBuffer->GetMemberCount();
			
			PFLOAT pfInstanceIndices = static_cast<PFLOAT>( pVertexBuffer->Lock(0) );
			if(pfInstanceIndices)
			{
				for(i = 0; i < uNewNumVertices; ++ i)
				{
					if(i == uInstanceIndexSegment)
						uInstanceIndexSegment = puInstanceIndexSegments[(uInstanceIndex + 1) < uInstanceCount ? (++ uInstanceIndex) : uInstanceIndex];
					
					pfInstanceIndices[i] = static_cast<FLOAT> (uInstanceIndex);
				}
				
				pVertexBuffer->Unlock();
			}
			else 
			{
				SceneManager.DestroyResource(*pOutput);
				
				ALCHEMY_DEBUG_WARNING("CVertex::ConvertToInstanceVertex: Vertex buffer has been lock fail.");
				
				return ALCHEMY_NULL;
			}
			
			if(pInstanceData && uInstanceDataCount)
			{
				const UINT8* puSourceData;
				PUINT8 puDestinationData;
				UINT j, uInstanceByteSize;
				for(i = 0; i < uInstanceDataCount; ++ i)
				{
					pVertexBuffer = pOutput->m_ppVertexBuffer[uStreamCount + i];
					
					puDestinationData = static_cast<PUINT8>( pVertexBuffer->Lock(0) );
					if(puDestinationData)
					{
						uInstanceIndex = 0;
						
						pCurrentInstanceData = &pInstanceData[i];
						if(pCurrentInstanceData->bIsElementData)
						{
							uInstanceIndexSegment = puInstanceIndexSegments[uInstanceIndex];
							for(j = 0; j < uNewNumVertices; ++ j)
							{
								if(j == uInstanceIndexSegment)
								{
									uInstanceIndexSegment = puInstanceIndexSegments[(uInstanceIndex + 1) < uInstanceCount ? (++ uInstanceIndex) : uInstanceIndex];
									
									puSourceData = static_cast<const UINT8*>(pCurrentInstanceData->pData) + uInstanceIndex * pCurrentInstanceData->ElementType;
								}
								
								ALCHEMY_MEMORY_COPY(puDestinationData, puSourceData, pCurrentInstanceData->ElementType);
								
								puDestinationData += pCurrentInstanceData->ElementType;
							}
						}
						else
						{
							uInstanceIndexSegment = 0;
							uInstanceByteSize = uInstanceIndexSegment * pCurrentInstanceData->ElementType;
							for(j = 0; j < uNewNumVertices; j += uInstanceIndexSegment)
							{
								puSourceData = static_cast<const UINT8*>(pCurrentInstanceData->pData) + uInstanceIndex * uInstanceByteSize;
								
								uInstanceIndexSegment = puInstanceIndexSegments[uInstanceIndex];
								
								uInstanceByteSize = uInstanceIndexSegment * pCurrentInstanceData->ElementType;
								
								ALCHEMY_MEMORY_COPY(puDestinationData, puSourceData, uInstanceByteSize);
								
								++ uInstanceIndex;
							}
						}
						
						pVertexBuffer->Unlock();
					}
					else 
					{
						SceneManager.DestroyResource(*pOutput);
						
						ALCHEMY_DEBUG_WARNING("CVertex::ConvertToInstanceVertex: Vertex buffer has been lock fail.");
						
						return ALCHEMY_NULL;
					}
				}
			}
			
			return pOutput;
			
			/*if( pOutput->_Create() )
				return pOutput;
			else 
			{
				SceneManager.DestroyResource(*pOutput);
				
				ALCHEMY_DEBUG_WARNING("CVertex::ConvertToInstancedVertex: Vertex has been created fail.");
			}*/
		}
		else 
		{
			SceneManager.DestroyResource(*pOutput);
			
			ALCHEMY_DEBUG_WARNING("CVertex::ConvertToInstanceVertex: Vertex has been created fail.");
			
			return ALCHEMY_NULL;
		}	
	}
	
	return ALCHEMY_NULL;
}

CVertex* CVertex::ConvertToInstancedVertex(FLOAT fInstanceCount, const INSTANCEDATA* pInstanceData, UINT uInstanceDataCount, PUINT puNumVertices)
{
	LPVERTEXBUFFER pVertexBuffer;
	
	if(!m_ppVertexBuffer || !(pVertexBuffer = m_ppVertexBuffer[0]) || !m_pVertexDeclaration)
	{
		ALCHEMY_DEBUG_WARNING("CVertex::ConvertToBlendedVertex: Error vertex.");
		
		return ALCHEMY_NULL;
	}
	
	CVertex* pCloneVertex = Clone(fInstanceCount, puNumVertices);
	if(pCloneVertex)
	{
		UINT uNumVertices = pVertexBuffer->GetMemberCount(), uInstanceIndexSegment = uNumVertices, uInstanceCount = static_cast<UINT> (fInstanceCount);
		PUINT puInstanceIndexSegments;
		ALCHEMY_DEBUG_NEW(puInstanceIndexSegments, UINT[uInstanceCount]);
		for(UINT i = 0; i < uInstanceCount; ++ i)
		{
			puInstanceIndexSegments[i] = uInstanceIndexSegment;
			
			uInstanceIndexSegment += uNumVertices;
		}
		
		CVertex* pOutput = pCloneVertex->ConvertToInstancedVertex(puInstanceIndexSegments, uInstanceCount, pInstanceData, uInstanceDataCount);
		ALCHEMY_DEBUG_DELETE_ARRAY(puInstanceIndexSegments);
		pCloneVertex->Release();
		if(pOutput)
		{
			/*if( pOutput->_Create() )
				return pOutput;
			else 
			{
				GetGraphicsResourcePool().DestroyResource(*pOutput);
				
				ALCHEMY_DEBUG_WARNING("CVertex::ConvertToInstancedVertex: Vertex has been created fail.");
			}*/
			return pOutput;
		}
		else
			ALCHEMY_DEBUG_WARNING("CVertex::ConvertToInstancedVertex: Vertex can not been convert to instanced vertex.");

	}
	else
		ALCHEMY_DEBUG_WARNING("CVertex::ConvertToInstancedVertex: Vertex has been cloned fail.");
	
	/*UINT uInstanceCount = static_cast<UINT>(fInstanceCount), uNumVertices = pVertexBuffer->GetMemberCount(), uNumNewVertices = uNumVertices * uInstanceCount;
	
	if(puNumVertices)
		*puNumVertices = uNumNewVertices;
	
	UINT uStreamCount = m_pVertexDeclaration->GetStreamCount();
	PUINT puUsage;
	ALCHEMY_DEBUG_NEW(puUsage, UINT[uStreamCount + 1]);

	UINT i;
	for(i = 0; i < uStreamCount; ++ i)
	{
		pVertexBuffer = m_ppVertexBuffer[i];
		if(pVertexBuffer)
			puUsage[i] = pVertexBuffer->GetUsage();
		else
		{
			ALCHEMY_DEBUG_WARNING("CVertex::ConvertToInstanceVertex: Error Vertex.");
			
			ALCHEMY_DEBUG_DELETE_ARRAY(puUsage);
			
			return ALCHEMY_NULL;
		}
	}
	
	puUsage[uStreamCount] = ALCHEMY_FLAG(VERTEXBUFFER::WRITE_ONLY);
	
	UINT uVertexElementCount = m_pVertexDeclaration->GetVertexElementCount(), uNewVertexElementCount = uVertexElementCount + 1;
	const CVertexDeclaration::VERTEXELEMENT* pVertexElements = m_pVertexDeclaration->GetVertexElements();
	CVertexDeclaration::LPVERTEXELEMENT pNewVertexElements, pVertexElement;
	ALCHEMY_DEBUG_NEW(pNewVertexElements, CVertexDeclaration::VERTEXELEMENT[uNewVertexElementCount]);
	
	for(i = 0; i < uVertexElementCount; ++ i)
		pNewVertexElements[i] = pVertexElements[i];
	
	pVertexElement = &pNewVertexElements[uVertexElementCount];
	
	pVertexElement->uStream = uStreamCount;
	pVertexElement->uOffset = 0;
	pVertexElement->Type    = CVertexDeclaration::FLOAT1;
	pVertexElement->Usage   = CVertexDeclaration::INSTANCEINDEX;
	pVertexElement->uIndex  = uVertexElementCount;
	
	CSceneManager& SceneManager = static_cast<CSceneManager&>( GetGraphicsResourcePool() );
	CVertex* pOutput = SceneManager.CreateVertex(uNumNewVertices, pNewVertexElements, uNewVertexElementCount, puUsage);
	ALCHEMY_DEBUG_DELETE_ARRAY(puUsage);
	ALCHEMY_DEBUG_DELETE_ARRAY(pNewVertexElements);
	
	if(pOutput)
	{
		UINT j;
		pVertexBuffer = pOutput->m_ppVertexBuffer[uStreamCount];
		if(pVertexBuffer)
		{
			PFLOAT pfInstanceIndices = static_cast<PFLOAT>( pVertexBuffer->Lock(0) );
			if(pfInstanceIndices)
			{
				UINT uIndex = 0;
				for(i = 0; i < uInstanceCount; ++ i)
				{
					for(j = 0; j < uNumVertices; ++ j)
						pfInstanceIndices[uIndex ++] = static_cast<FLOAT> (i);
				}
			}
			else 
			{
				SceneManager.DestroyResource(*pOutput);
				
				ALCHEMY_DEBUG_WARNING("CVertex::ConvertToInstanceVertex: Vertex has been created fail.");
				
				return ALCHEMY_NULL;
			}

			pVertexBuffer->Unlock();
		}
		else 
		{
			SceneManager.DestroyResource(*pOutput);
			
			ALCHEMY_DEBUG_WARNING("CVertex::ConvertToInstanceVertex: Vertex has been created fail.");
			
			return ALCHEMY_NULL;
		}
		
		LPVERTEXBUFFER pSourceVertexBuffer, pDestinationVertexBuffer;
		const UINT8* puSourceData;
		PUINT8 puDestinationData;
		UINT uByteSize;
		for(i = 0; i < uStreamCount; ++ i)
		{
			pSourceVertexBuffer      = m_ppVertexBuffer[i];
			pDestinationVertexBuffer = pOutput->m_ppVertexBuffer[i];
			if(pSourceVertexBuffer && pDestinationVertexBuffer)
			{
				uByteSize         = pSourceVertexBuffer->GetByteSize();
				
				puSourceData      = static_cast<const UINT8*>( pSourceVertexBuffer     ->Lock( ALCHEMY_FLAG(VERTEXBUFFER::READ_ONLY) ) );
				puDestinationData = static_cast<PUINT8      >( pDestinationVertexBuffer->Lock( 0                                     ) );
				
				if(puSourceData && puDestinationData)
				{
					for(j = 0; j < uInstanceCount; ++ j)
					{
						ALCHEMY_MEMORY_COPY(puDestinationData, puSourceData, uByteSize);
						
						puDestinationData += uByteSize;
					}
				}
				
				pSourceVertexBuffer     ->Unlock();
				pDestinationVertexBuffer->Unlock();
			}
		}
		
		if( pOutput->_Create() )
			return pOutput;
		
		SceneManager.DestroyResource(*pOutput);
	}
	
	ALCHEMY_DEBUG_WARNING("CVertex::ConvertToInstanceVertex: Vertex has been created fail.");*/
	
	return ALCHEMY_NULL;
}

CVertex* CVertex::ConvertToBlendedVertex(const CSkinInfo& SkinInfo, 
										 UINT uBlendWeightUsage,
										 UINT uBlendIndexUsage, 
										 UINT uMatrixIndexOffset, 
										 PUINT puOutputMaxVertexInfluenceCount)
{
	LPVERTEXBUFFER pVertexBuffer;

	if(!m_ppVertexBuffer || !(pVertexBuffer = m_ppVertexBuffer[0]) || !m_pVertexDeclaration)
	{
		ALCHEMY_DEBUG_WARNING("CVertex::ConvertToBlendedVertex: Error vertex.");

		return ALCHEMY_NULL;
	}

	UINT uNumVertices = pVertexBuffer->GetMemberCount(), i; 
	PUINT puVertexInfluenceCount;
	ALCHEMY_DEBUG_NEW(puVertexInfluenceCount, UINT[uNumVertices]);

	for(i = 0; i < uNumVertices; ++ i)
		puVertexInfluenceCount[i] = 0;

	UINT uMaxVertexInfluenceCount = 0, uBoneCount = SkinInfo.GetBoneCount(), uInfluenceVertices;
	const UINT* puInfluenceVertices, * puBoneIndices;
	for(i = 0; i < uBoneCount; ++ i)
	{
		puInfluenceVertices = SkinInfo.GetBoneInfluenceCount(i);

		puBoneIndices = SkinInfo.GetIndices(i);
		
		if(puInfluenceVertices)
		{
			uInfluenceVertices = *puInfluenceVertices;
			while(uInfluenceVertices -- > 0)
				uMaxVertexInfluenceCount = ALCHEMY_MAXINUM(uMaxVertexInfluenceCount, ++ puVertexInfluenceCount[puBoneIndices[uInfluenceVertices]]);
		}
	}

	if(puOutputMaxVertexInfluenceCount)
		*puOutputMaxVertexInfluenceCount = uMaxVertexInfluenceCount;

	UINT uBlendCount = uNumVertices * uMaxVertexInfluenceCount;

	//CBuffer& BlendWeightsBuffer = CreateBuffer(sizeof(FLOAT) * uBlendCount);

	PFLOAT pfBlendWeights;// = reinterpret_cast<PFLOAT>( BlendWeightsBuffer.GetBufferPointer() );
	ALCHEMY_DEBUG_NEW(pfBlendWeights, FLOAT[uBlendCount]);
	for(i = 0; i < uBlendCount; ++ i)
		pfBlendWeights[i] = 0.0f;

	//CBuffer& BlendIndicesBuffer = CreateBuffer(sizeof(FLOAT) * uBlendCount);
	PFLOAT pfBlendIndices;// = reinterpret_cast<PFLOAT>( BlendIndicesBuffer.GetBufferPointer() );
	ALCHEMY_DEBUG_NEW(pfBlendIndices, FLOAT[uBlendCount]);
	for(i = 0; i < uBlendCount; ++ i)
		pfBlendIndices[i] = 0;

	for(i = 0; i < uNumVertices; ++ i)
		puVertexInfluenceCount[i] = 0;

	UINT uIndex, uOffset;
	const FLOAT* pfBoneWeights;
	for(i = 0; i < uBoneCount; ++ i)
	{
		puInfluenceVertices = SkinInfo.GetBoneInfluenceCount(i);

		puBoneIndices = SkinInfo.GetIndices(i);

		pfBoneWeights = SkinInfo.GetWeights(i);

		if(puInfluenceVertices)
		{
			uInfluenceVertices = *puInfluenceVertices;
			while(uInfluenceVertices -- > 0)
			{
				uIndex = puBoneIndices[uInfluenceVertices];

				uOffset = puVertexInfluenceCount[uIndex] ++;

				if(uOffset < uMaxVertexInfluenceCount)
				{
					uIndex = uIndex * uMaxVertexInfluenceCount + uOffset;

					pfBlendWeights[uIndex] = pfBoneWeights[uInfluenceVertices];

					pfBlendIndices[uIndex] = static_cast<FLOAT>(i * uMatrixIndexOffset);
				}
			}
		}
	}

	UINT uBlendWeightsOffset = 0,
		uBlendIndicesOffset = 0,
		uVertexElementCount = m_pVertexDeclaration->GetVertexElementCount(), 
		uVertexInfluence, 
		uElementIndex, 
		uCurrentVertexInfluence,
		uVertexElement, 
		uStride, 
		uBasicIndex, 
		j;

	const CVertexDeclaration::VERTEXELEMENT* pVertexElements = m_pVertexDeclaration->GetVertexElements(), * pVertexElement;

	CVertexDeclaration::ELEMENTTYPE ElementType;
	PUINT8 pVertices;
	PFLOAT pfValues;

	uVertexInfluence = 0;
	uElementIndex = 0;
	uCurrentVertexInfluence = uMaxVertexInfluenceCount & 3;
	uCurrentVertexInfluence = uCurrentVertexInfluence ? uCurrentVertexInfluence : 4;
	while(uVertexInfluence < uMaxVertexInfluenceCount)
	{
		ElementType = static_cast<CVertexDeclaration::ELEMENTTYPE>( uCurrentVertexInfluence * sizeof(FLOAT) );

		uVertexElement = CVertexDeclaration::FindVertexElement(CVertexDeclaration::BLENDWEIGHT, ElementType, uElementIndex, pVertexElements, uVertexElementCount);

		if(uVertexElement < uVertexElementCount)
		{
			pVertexElement = &pVertexElements[uVertexElement];

			pVertexBuffer = m_ppVertexBuffer[pVertexElement->uStream];

			if(pVertexBuffer)
			{
				if(pVertexBuffer->GetMemberCount() == uNumVertices)
				{
					pVertices = static_cast<PUINT8>( pVertexBuffer->Lock(0) );

					if(pVertices)
					{
						uStride = pVertexBuffer->GetByteSize();
						uBasicIndex = 0;
						for(i = 0; i < uNumVertices; ++ i)
						{
							uIndex = pVertexElement->uOffset;
							pfValues = reinterpret_cast<PFLOAT>(&pVertices[uIndex]);

							for(j = 0; j < uMaxVertexInfluenceCount; ++ j)
							{
								pfValues[j] = pfBlendWeights[uBasicIndex];

								++ uBasicIndex;
							}

							uIndex += uStride;
						}

						pVertexBuffer->Unlock();
					}
				}
			}
			else
			{
				ALCHEMY_DEBUG_WARNING("CVertex::ConvertToBlendedVertex: Error vertex.");

				ALCHEMY_DEBUG_DELETE_ARRAY(pfBlendWeights);

				ALCHEMY_DEBUG_DELETE_ARRAY(pfBlendIndices);

				ALCHEMY_DEBUG_DELETE_ARRAY(puVertexInfluenceCount);

				return ALCHEMY_NULL;
			}

			uBlendWeightsOffset += uCurrentVertexInfluence;
		}
		else  if(uCurrentVertexInfluence == 4)
			break;

		if(uCurrentVertexInfluence == 4)
			++ uElementIndex;

		uVertexInfluence += uCurrentVertexInfluence;
		uCurrentVertexInfluence = 4;
	}

	uVertexInfluence = 0;
	uElementIndex = 0;
	uCurrentVertexInfluence = uMaxVertexInfluenceCount & 3;
	uCurrentVertexInfluence = uCurrentVertexInfluence ? uCurrentVertexInfluence : 4;
	while(uVertexInfluence < uMaxVertexInfluenceCount)
	{
		uVertexElement = CVertexDeclaration::FindVertexElement(CVertexDeclaration::BLENDINDICES, ElementType, uElementIndex, pVertexElements, uVertexElementCount);

		if(uVertexElement < uVertexElementCount)
		{
			pVertexElement = &pVertexElements[uVertexElement];

			pVertexBuffer = m_ppVertexBuffer[pVertexElement->uStream];

			if(pVertexBuffer)
			{
				if(pVertexBuffer->GetMemberCount() == uNumVertices)
				{
					pVertices = static_cast<PUINT8>( pVertexBuffer->Lock(0) );

					if(pVertices)
					{
						uStride = pVertexBuffer->GetByteSize();
						uBasicIndex = 0;
						for(i = 0; i < uNumVertices; ++ i)
						{
							uIndex = pVertexElement->uOffset;
							pfValues = reinterpret_cast<PFLOAT>(&pVertices[uIndex]);

							for(j = 0; j < uMaxVertexInfluenceCount; ++ j)
							{
								pfValues[j] = pfBlendIndices[uBasicIndex];

								++ uBasicIndex;
							}

							uIndex += uStride;
						}

						pVertexBuffer->Unlock();
					}
				}
			}
			else
			{
				ALCHEMY_DEBUG_WARNING("CVertex::ConvertToBlendedVertex: Error vertex.");

				ALCHEMY_DEBUG_DELETE_ARRAY(pfBlendWeights);

				ALCHEMY_DEBUG_DELETE_ARRAY(pfBlendIndices);

				ALCHEMY_DEBUG_DELETE_ARRAY(puVertexInfluenceCount);

				return ALCHEMY_NULL;
			}

			uBlendIndicesOffset += uCurrentVertexInfluence;
		}
		else if(uCurrentVertexInfluence == 4)
			break;

		if(uCurrentVertexInfluence == 4)
			++ uElementIndex;

		uVertexInfluence += uCurrentVertexInfluence;
		uCurrentVertexInfluence = 4;
	}

	CVertex* pOutput = static_cast<CVertex*>( CHighLevelGraphicsResource::_Clone() );

	if(pOutput)
	{
		UINT uStreamCount = m_pVertexDeclaration->GetStreamCount(), uNewStreamCount = uStreamCount;

		if(uBlendIndicesOffset < uMaxVertexInfluenceCount)
			++ uNewStreamCount;

		if(uBlendWeightsOffset < uMaxVertexInfluenceCount)
			++ uNewStreamCount;

		ALCHEMY_DEBUG_NEW(pOutput->m_ppVertexBuffer, LPVERTEXBUFFER[uNewStreamCount]);

		for(i = 0; i < uStreamCount; ++ i)
		{
			pVertexBuffer = m_ppVertexBuffer[i];

			if(pVertexBuffer)
			{
				_AddRef(*pVertexBuffer);

				pOutput->m_ppVertexBuffer[i] = pVertexBuffer;
			}
			else
				pOutput->m_ppVertexBuffer[i] = ALCHEMY_NULL;
		}

		while(i < uNewStreamCount)
		{
			pOutput->m_ppVertexBuffer[i] = ALCHEMY_NULL;

			++ i;
		}

		if(uNewStreamCount == uStreamCount)
		{
			_AddRef(*m_pVertexDeclaration);

			pOutput->m_pVertexDeclaration = m_pVertexDeclaration;

			if( pOutput->_Create() )
			{
				ALCHEMY_DEBUG_DELETE_ARRAY(pfBlendWeights);

				ALCHEMY_DEBUG_DELETE_ARRAY(pfBlendIndices);

				ALCHEMY_DEBUG_DELETE_ARRAY(puVertexInfluenceCount);

				return pOutput;
			}
		}
		else
		{
			UINT uBlendWeightsStride = uMaxVertexInfluenceCount - uBlendWeightsOffset,
				uBlendIndicesStride = uMaxVertexInfluenceCount - uBlendIndicesOffset,
				uBlendWeightElementCount = (uBlendWeightsStride + 3) / 4,
				uBlendIndicesElementCount = (uBlendIndicesStride + 3) / 4,
				uNewVertexElementCount = uVertexElementCount + uBlendWeightElementCount + uBlendIndicesElementCount;
			//const UINT* puVertexElementOrderMap = m_pVertexDeclaration->GetVertexElementOrderMap();
			CVertexDeclaration::LPVERTEXELEMENT pNewVertexElements;
			ALCHEMY_DEBUG_NEW(pNewVertexElements, CVertexDeclaration::VERTEXELEMENT[uNewVertexElementCount]);

			for(uVertexElement = 0; uVertexElement < uVertexElementCount; ++ uVertexElement)
				pNewVertexElements[uVertexElement/*puVertexElementOrderMap[uVertexElement]*/] = pVertexElements[uVertexElement];

			CVertexDeclaration::LPVERTEXELEMENT pVertexElementTemp;
			if(uBlendWeightElementCount)
			{
				uCurrentVertexInfluence = uBlendWeightsStride & 3;
				uCurrentVertexInfluence = uCurrentVertexInfluence ? uCurrentVertexInfluence : 4;
				uOffset                 = uBlendWeightsOffset * sizeof(FLOAT);
				for(i = 0; i < uBlendWeightElementCount; ++ i)
				{
					ElementType                 = static_cast<CVertexDeclaration::ELEMENTTYPE>( uCurrentVertexInfluence * sizeof(FLOAT) );

					pVertexElementTemp          = &pNewVertexElements[uVertexElement ++];
					pVertexElementTemp->Type    = ElementType;
					pVertexElementTemp->Usage   = CVertexDeclaration::BLENDWEIGHT;
					pVertexElementTemp->uOffset = uOffset;
					pVertexElementTemp->uStream = uStreamCount;

					uOffset                    += ElementType;
					uCurrentVertexInfluence     = 4;
				}

				uStride = uBlendWeightsStride * sizeof(FLOAT);
				pVertexBuffer = static_cast<CSceneManager&>( GetGraphicsResourcePool() ).GetRenderSystem().CreateVertexBuffer(uNumVertices, uStride, uBlendWeightUsage, uBlendWeightsOffset ? ALCHEMY_NULL : pfBlendWeights);
				if( !pVertexBuffer)
				{
					ALCHEMY_DEBUG_WARNING("CVertex::ConvertToBlendedVertex: Blend weight vertex buffer has been created fail.");

					ALCHEMY_DEBUG_DELETE_ARRAY(pfBlendWeights);

					ALCHEMY_DEBUG_DELETE_ARRAY(pfBlendIndices);

					ALCHEMY_DEBUG_DELETE_ARRAY(puVertexInfluenceCount);

					ALCHEMY_DEBUG_DELETE_ARRAY(pNewVertexElements);

					static_cast<CSceneManager&>( GetGraphicsResourcePool() ).DestroyResource(*pOutput);

					return ALCHEMY_NULL;
				}

				if(uBlendWeightsOffset)
				{
					pVertices = reinterpret_cast<PUINT8>( pVertexBuffer->Lock(0) );
					pfValues  = pfBlendWeights + uBlendWeightsOffset;
					for(i = 0; i < uNumVertices; ++ i)
					{
						ALCHEMY_MEMORY_COPY(pVertices, pfValues, uStride);

						pVertices += uStride;
						pfValues  += uMaxVertexInfluenceCount;
					}
				}

				_AddRef(*pVertexBuffer);

				pOutput->m_ppVertexBuffer[uStreamCount] = pVertexBuffer;

				++ uStreamCount;
			}

			if(uBlendIndicesElementCount)
			{
				uCurrentVertexInfluence = uBlendIndicesStride & 3;
				uCurrentVertexInfluence = uCurrentVertexInfluence ? uCurrentVertexInfluence : 4;
				uOffset                 = uBlendIndicesOffset * sizeof(FLOAT);
				for(i = 0; i < uBlendIndicesElementCount; ++ i)
				{
					ElementType                 = static_cast<CVertexDeclaration::ELEMENTTYPE>( uCurrentVertexInfluence * sizeof(FLOAT) );

					pVertexElementTemp          = &pNewVertexElements[uVertexElement ++];
					pVertexElementTemp->Type    = ElementType;
					pVertexElementTemp->Usage   = CVertexDeclaration::BLENDINDICES;
					pVertexElementTemp->uOffset = uOffset;
					pVertexElementTemp->uStream = uStreamCount;

					uOffset                    += ElementType;
					uCurrentVertexInfluence     = 4;
				}

				uStride = uBlendIndicesStride * sizeof(FLOAT);
				pVertexBuffer = static_cast<CSceneManager&>( GetGraphicsResourcePool() ).GetRenderSystem().CreateVertexBuffer(uNumVertices, uStride, uBlendIndexUsage, uBlendIndicesOffset ? ALCHEMY_NULL : pfBlendIndices);
				if( !pVertexBuffer)
				{
					ALCHEMY_DEBUG_WARNING("CVertex::ConvertToBlendedVertex: Blend weight vertex buffer has been created fail.");

					ALCHEMY_DEBUG_DELETE_ARRAY(pfBlendWeights);

					ALCHEMY_DEBUG_DELETE_ARRAY(pfBlendIndices);

					ALCHEMY_DEBUG_DELETE_ARRAY(puVertexInfluenceCount);

					ALCHEMY_DEBUG_DELETE_ARRAY(pNewVertexElements);

					static_cast<CSceneManager&>( GetGraphicsResourcePool() ).DestroyResource(*pOutput);

					return ALCHEMY_NULL;
				}

				if(uBlendIndicesOffset)
				{
					pVertices = reinterpret_cast<PUINT8>( pVertexBuffer->Lock(0) );
					pfValues  = pfBlendIndices + uBlendIndicesOffset;
					for(i = 0; i < uNumVertices; ++ i)
					{
						ALCHEMY_MEMORY_COPY(pVertices, pfValues, uStride);

						pVertices += uStride;
						pfValues  += uMaxVertexInfluenceCount;
					}
				}

				_AddRef(*pVertexBuffer);

				pOutput->m_ppVertexBuffer[uStreamCount] = pVertexBuffer;

				++ uStreamCount;
			}

			if( !( pOutput->m_pVertexDeclaration = static_cast<CSceneManager&>( GetGraphicsResourcePool() ).GetRenderSystem().CreateVertexDeclaration(pNewVertexElements, uNewVertexElementCount) ) )
			{
				ALCHEMY_DEBUG_WARNING("CVertex::ConvertToBlendedVertex: Vertex declaration has been created fail.");

				ALCHEMY_DEBUG_DELETE_ARRAY(pfBlendWeights);

				ALCHEMY_DEBUG_DELETE_ARRAY(pfBlendIndices);

				ALCHEMY_DEBUG_DELETE_ARRAY(puVertexInfluenceCount);

				ALCHEMY_DEBUG_DELETE_ARRAY(pNewVertexElements);

				static_cast<CSceneManager&>( GetGraphicsResourcePool() ).DestroyResource(*pOutput);

				return ALCHEMY_NULL;
			}

			_AddRef(*pOutput->m_pVertexDeclaration);

			ALCHEMY_DEBUG_DELETE_ARRAY(pNewVertexElements);

			if( pOutput->_Create() )
			{
				ALCHEMY_DEBUG_DELETE_ARRAY(pfBlendWeights);

				ALCHEMY_DEBUG_DELETE_ARRAY(pfBlendIndices);

				ALCHEMY_DEBUG_DELETE_ARRAY(puVertexInfluenceCount);

				return pOutput;
			}
		}

		static_cast<CSceneManager&>( GetGraphicsResourcePool() ).DestroyResource(*pOutput);
	}

	ALCHEMY_DEBUG_WARNING("CVertex::ConvertToBlendedVertex: Vertex has been created fail.");

	ALCHEMY_DEBUG_DELETE_ARRAY(pfBlendWeights);

	ALCHEMY_DEBUG_DELETE_ARRAY(pfBlendIndices);

	ALCHEMY_DEBUG_DELETE_ARRAY(puVertexInfluenceCount);

	return ALCHEMY_NULL;
}

void CVertex::Destroy()
{
	if(m_pVertexDeclaration)
	{
		LPVERTEXBUFFER pVertexBuffer;
		UINT uVertexBufferCount = m_pVertexDeclaration->GetStreamCount();
		for(UINT i = 0; i < uVertexBufferCount; ++ i)
		{
			pVertexBuffer = m_ppVertexBuffer[i];

			if(pVertexBuffer)
				_Release(*pVertexBuffer);
		}

		_Release(*m_pVertexDeclaration);

		m_pVertexDeclaration = ALCHEMY_NULL;
	}

	ALCHEMY_DEBUG_DELETE_ARRAY(m_ppVertexBuffer);
}

bool CVertex::Activate(bool bIsActivateVertexDeclaration)
{
	if(m_ppVertexBuffer && m_pVertexDeclaration)
	{
		CRenderSystem& RenderSystem = static_cast<CSceneManager&>( GetGraphicsResourcePool() ).GetRenderSystem();

		if(bIsActivateVertexDeclaration)
			RenderSystem.SetVertexDeclaration( m_pVertexDeclaration->GetResourceHandle() );

		const CVertexDeclaration::VERTEXELEMENT* pVertexElements = m_pVertexDeclaration->GetVertexElements();
		const UINT* puElementEndOfStream = m_pVertexDeclaration->GetElementEndOfStream();
		UINT uVertexBufferCount = m_pVertexDeclaration->GetStreamCount(), uStream;
		LPVERTEXBUFFER pVertexBuffer;
		for(UINT i = 0; i < uVertexBufferCount; ++ i)
		{
			uStream = pVertexElements[puElementEndOfStream[i]].uStream;
			pVertexBuffer = m_ppVertexBuffer[uStream];
			if(pVertexBuffer)
				RenderSystem.SetStreamSource(pVertexBuffer->GetResourceHandle(), uStream);
			else
			{
				ALCHEMY_DEBUG_WARNING("CVertex::Activate: Vertex buffer is null.");

				return false;
			}
		}

		return true;
	}

	ALCHEMY_DEBUG_WARNING("CVertex::Activate: Vertex declaration has not been created.");

	return false;
}