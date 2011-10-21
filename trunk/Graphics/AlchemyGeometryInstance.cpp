#include "AlchemyGeometryInstance.h"

using namespace alchemy;

CGeometryInstance::CGeometryInstance(void) :
m_pRenderMethod(ALCHEMY_NULL),
m_pMaterial(ALCHEMY_NULL),
m_pVertexDeclaration(ALCHEMY_NULL),
m_pSceneManager(ALCHEMY_NULL),
m_pCurrentInstance(ALCHEMY_NULL)
{
}

CGeometryInstance::~CGeometryInstance(void)
{
}

bool CGeometryInstance::Create(CRenderMethodInterface& RenderMethod, CGraphicsResource::HANDLE Material, CGraphicsResource::HANDLE VertexDeclaration)
{
	Destroy();

	m_pSceneManager = CSceneManager::GetInstance();

	if(m_pSceneManager)
	{
		m_pRenderMethod = &RenderMethod;

		if(Material)
			m_pMaterial = static_cast<CMaterial*>( m_pSceneManager->GetResource(Material, CGraphicsResource::MATERIAL) );

		CRenderSystem& RenderSystem = m_pSceneManager->GetRenderSystem();
		if(VertexDeclaration)
			m_pVertexDeclaration = static_cast<CVertexDeclaration*>( RenderSystem.GetResource(VertexDeclaration, CGraphicsResource::VERTEX_DECLARATION) );

		if(!m_pVertexDeclaration)
		{
			CVertexDeclaration::LPVERTEXELEMENT pVertexElements, pVertexElement;
			UINT uVertexElementCount, uOffset = 0;
			const CEffectCode::ELEMENTPARAMETER* pElementParameters = RenderMethod.GetVertexElementParameters(0, &uVertexElementCount), * pElementParameter;

			ALCHEMY_DEBUG_NEW(pVertexElements, CVertexDeclaration::VERTEXELEMENT[uVertexElementCount]);
			for(UINT i = 0; i < uVertexElementCount; ++ i)
			{
				pVertexElement    = &pVertexElements[i];
				pElementParameter = & pElementParameters[i];

				pVertexElement->uStream = 0;
				pVertexElement->uOffset = uOffset;
				pVertexElement->Type    = pElementParameter->Type;
				pVertexElement->Usage   = pElementParameter->Usage;

				uOffset += pElementParameter->Type;
			}

			m_pVertexDeclaration = RenderSystem.CreateVertexDeclaration(pVertexElements, uVertexElementCount);

			ALCHEMY_DEBUG_DELETE_ARRAY(pVertexElements);

			if(!m_pVertexDeclaration)
			{
				ALCHEMY_DEBUG_WARNING("Vertex declaration has been created fail.");

				Destroy();

				return false;
			}
		}

		return true;
	}

	return false;
}

void CGeometryInstance::Destroy()
{
	CMesh* pMesh;
	CVector<CMesh*>::CIterator InstanceIterator = m_GeometryInstances.GetBeginIterator();
	while( CVector<CMesh*>::IsValuable(InstanceIterator) )
	{
		pMesh = *InstanceIterator;

		if(pMesh)
			pMesh->Release();

		++ InstanceIterator;
	}

	//CVector<CProgramInstance*>::CIterator Iterator = m_ProgramInstances;
	if(m_pRenderMethod)
	{

	}

	if(m_pMaterial)
		m_pMaterial->Release();

	if(m_pVertexDeclaration)
		m_pVertexDeclaration->Release();

	m_pSceneManager = ALCHEMY_NULL;

	if(m_pCurrentInstance)
		m_pCurrentInstance->Release();
}

bool CGeometryInstance::AddGeometry(CGraphicsResource::HANDLE Mesh, 
									const CMesh::ATTRIBUTERANGE* pAttributeRange, 
									LPMATRIX4X4 pTransform,
									const CEffectCode::ELEMENTPARAMETER* pTransformElementParameters,
									UINT uTransformElementParameterCount)
{
	if( !__IsValuable() )
		return false;

	CMesh* pMesh = static_cast<CMesh*>( m_pSceneManager->GetResource(Mesh, CGraphicsResource::MESH) );

	if(!pMesh)
	{
		ALCHEMY_DEBUG_WARNING("CGeometryInstance::AddGeometry: Mesh has been get fail.");

		return false;
	}

	if(!m_pCurrentInstance)
	{
		UINT uStreamCount = m_pVertexDeclaration->GetStreamCount();
		PUINT puUsage;
		ALCHEMY_DEBUG_NEW(puUsage, UINT[uStreamCount]);
		for(UINT i = 0; i < uStreamCount; ++ i)
			puUsage[i] = ALCHEMY_FLAG(INDEXBUFFER::WRITE_ONLY);

		m_pCurrentInstance = m_pSceneManager->CreateMesh(MAXINUM_INDEX_COUNT, MAXINUM_INDEX_COUNT, ALCHEMY_FLAG(INDEXBUFFER::WRITE_ONLY), INDEXBUFFER::BIT16, m_pVertexDeclaration->GetResourceHandle(), puUsage);

		ALCHEMY_DEBUG_DELETE_ARRAY(puUsage);

		if(!m_pCurrentInstance)
		{
			ALCHEMY_DEBUG_WARNING("CGeometryInstance::AddGeometry: Mesh has been created fail.");

			return false;
		}

		CMesh::ATTRIBUTERANGE AttributeRange;
		AttributeRange.uStartIndex = 0;
		AttributeRange.uIndexCount = 0;

		m_pAttributeRange = m_pCurrentInstance->GetAttributeTable().Get( m_pCurrentInstance->GetAttributeTable().Add(AttributeRange) );
	}

	INDEXBUFFER& IndexBuffer = pMesh->GetIndexBuffer();

	UINT uIndexStart, uIndexCount;
	if(pAttributeRange)
	{
		uIndexStart = pAttributeRange->uStartIndex;
		uIndexCount = pAttributeRange->uIndexCount;
	}
	else
	{
		uIndexStart = 0;
		uIndexCount = IndexBuffer.GetMemberCount();
	}

	CVertex& SourceVertex = pMesh->GetVertex(), DestinationVertex = m_pCurrentInstance->GetVertex();
	CVertexDeclaration& SourceVertexDeclaration = SourceVertex.GetVertexDeclaration();
	const CVertexDeclaration::VERTEXELEMENT* pSourceVertexElement = SourceVertexDeclaration.GetVertexElements(), 
		* pDestinationVertexElement = m_pVertexDeclaration->GetVertexElements(), 
		* pTemporarySourceVertexElement, 
		* pTemporaryDestinationVertexElement;

	INDEXBUFFER& SourceIndexBuffer = pMesh->GetIndexBuffer(), &DestinationIndexBuffer = m_pCurrentInstance->GetIndexBuffer();

	UINT uStart =  m_pAttributeRange->uStartIndex + m_pAttributeRange->uIndexCount, 
		uEnd = uStart + uIndexCount, 
		uCurrentEnd = uEnd < MAXINUM_INDEX_COUNT ? uEnd : MAXINUM_INDEX_COUNT,
		uCurrentIndexCount = uIndexCount - (uEnd - uCurrentEnd),
		uSourceVertexElementCount = SourceVertexDeclaration.GetVertexElementCount(), 
		uDestinationVertexElementCount = m_pVertexDeclaration->GetVertexElementCount(),
		uSourceIndex, 
		uVertexElement, 
		uVertexElementIndex, 
		uSourceStride, 
		uDestinationStride, 
		i, 
		j;

	PUINT16 pDestiantionIndices = reinterpret_cast<PUINT16>( DestinationIndexBuffer.Lock( uStart * sizeof(UINT16), uCurrentEnd * sizeof(UINT16), ALCHEMY_FLAG(INDEXBUFFER::NO_OVER_WRITE) ) );

	PUINT puValue;
	PUINT8 puSourceVertices, puDestinationVertices;
	bool bIsTransform;
	CMap<VERTEXELEMENTKEY, UINT, VERTEXELEMENTKEYLESS, VERTEXELEMENTKEYEQUAL> VertexElementIndexMap;
	VERTEXELEMENTKEY VertexElementKey;
	const CEffectCode::ELEMENTPARAMETER* pElementParameter;
	LPVERTEXBUFFER pSourceVertexBuffer, pDestinationVertexBuffer;

	if(SourceIndexBuffer.GetFormat() == INDEXBUFFER::BIT16)
	{
		PUINT16 pSourceIndices = reinterpret_cast<PUINT16>( SourceIndexBuffer.Lock( uIndexStart, uIndexStart + uIndexCount, ALCHEMY_FLAG(INDEXBUFFER::READ_ONLY) ) );
		for(i = 0; i < uDestinationVertexElementCount; ++ i)
		{
			pTemporaryDestinationVertexElement = &pDestinationVertexElement[i];

			VertexElementKey.Type  = pTemporaryDestinationVertexElement->Type;
			VertexElementKey.Usage = pTemporaryDestinationVertexElement->Usage;

			puValue = VertexElementIndexMap.Get(VertexElementKey);

			if(puValue)
				uVertexElementIndex = *(++ puValue);
			else
			{
				uVertexElementIndex = 0;

				VertexElementIndexMap.Insert(VertexElementKey, 0);
			}

			uVertexElement = CVertexDeclaration::FindVertexElement(VertexElementKey.Usage, VertexElementKey.Type, uVertexElementIndex, pSourceVertexElement, uSourceVertexElementCount);

			if(uVertexElement < uSourceVertexElementCount)
			{
				pTemporarySourceVertexElement = &pSourceVertexElement[uVertexElement];
				pSourceVertexBuffer = SourceVertex.GetVertexBuffer(pTemporarySourceVertexElement->uStream);
				pDestinationVertexBuffer = DestinationVertex.GetVertexBuffer(pTemporaryDestinationVertexElement->uStream);
				uSourceStride = pSourceVertexBuffer->GetStride();
				uDestinationStride = pDestinationVertexBuffer->GetStride();

				if(pSourceVertexBuffer && pDestinationVertexBuffer)
				{
					puSourceVertices = reinterpret_cast<PUINT8>( pSourceVertexBuffer->Lock( ALCHEMY_FLAG(VERTEXBUFFER::READ_ONLY) ) );
					puDestinationVertices = reinterpret_cast<PUINT8>( pDestinationVertexBuffer->Lock( 
						uStart * uDestinationStride + pTemporaryDestinationVertexElement->uOffset, 
						uCurrentEnd * uDestinationStride, 
						ALCHEMY_FLAG(INDEXBUFFER::NO_OVER_WRITE) ) );

					if(puSourceVertices && puDestinationVertices)
					{
						bIsTransform = false;
						if(pTransform)
						{
							for(j = 0; j < uTransformElementParameterCount; ++ j)
							{
								pElementParameter = &pTransformElementParameters[j];
								if(pElementParameter->Type == VertexElementKey.Type && pElementParameter->Usage == VertexElementKey.Usage && pElementParameter->uIndex == uVertexElementIndex)
								{
									bIsTransform = true;

									break;
								}
							}
						}

						if(bIsTransform)
						{
							switch(VertexElementKey.Type)
							{
							case CVertexDeclaration::FLOAT4:
								for(j = 0; j < uCurrentIndexCount; ++ j)
								{
									uSourceIndex = pSourceIndices[i];

									ALCHEMY_MEMORY_COPY(puDestinationVertices, puSourceVertices + uSourceIndex * uSourceStride + pTemporarySourceVertexElement->uOffset, pTemporaryDestinationVertexElement->Type);

									reinterpret_cast<CVector4*>(puDestinationVertices)->Transform(*pTransform);

									puDestinationVertices += uDestinationStride;
								}
								break;
							case CVertexDeclaration::FLOAT3:
								for(j = 0; j < uCurrentIndexCount; ++ j)
								{
									uSourceIndex = pSourceIndices[i];

									ALCHEMY_MEMORY_COPY(puDestinationVertices, puSourceVertices + uSourceIndex * uSourceStride + pTemporarySourceVertexElement->uOffset, pTemporaryDestinationVertexElement->Type);

									reinterpret_cast<CVector3*>(puDestinationVertices)->TransformNormal(*pTransform);

									puDestinationVertices += uDestinationStride;
								}
								break;

							default:
								for(j = 0; j < uCurrentIndexCount; ++ j)
								{
									uSourceIndex = pSourceIndices[i];

									ALCHEMY_MEMORY_COPY(puDestinationVertices, puSourceVertices + uSourceIndex * uSourceStride + pTemporarySourceVertexElement->uOffset, pTemporaryDestinationVertexElement->Type);

									puDestinationVertices += uDestinationStride;
								}
								break;
							}
						}
						else
						{
							for(j = 0; j < uCurrentIndexCount; ++ j)
							{
								uSourceIndex = pSourceIndices[i];

								ALCHEMY_MEMORY_COPY(puDestinationVertices, puSourceVertices + uSourceIndex * uSourceStride + pTemporarySourceVertexElement->uOffset, pTemporaryDestinationVertexElement->Type);

								puDestinationVertices += uDestinationStride;
							}
						}

						pSourceVertexBuffer->Unlock();
						pDestinationVertexBuffer->Unlock();
					}
					else
					{
						ALCHEMY_DEBUG_WARNING("CGeometryInstance::AddGeometry: Vertex buffer has been lock fail.");

						return false;
					}
				}
				else
				{
					ALCHEMY_DEBUG_WARNING("CGeometryInstance::AddGeometry: Vertex buffer has been get fail.");

					return false;
				}
			}
			else
			{
				ALCHEMY_DEBUG_WARNING("CGeometryInstance::AddGeometry: Vertex element has been find fail.");

				return false;
			}
		}

		SourceIndexBuffer.Unlock();
	}

	for(UINT i = uStart; i < uCurrentEnd; ++ i)
		pDestiantionIndices[i] = i;

	m_pAttributeRange->uIndexCount = uCurrentEnd;

	DestinationIndexBuffer.Unlock();

	if(uEnd >= MAXINUM_INDEX_COUNT)
	{
		m_GeometryInstances.Add(m_pCurrentInstance);
		m_pCurrentInstance = ALCHEMY_NULL;

		uIndexCount -= uCurrentIndexCount;
		if(uIndexCount)
		{
			CMesh::ATTRIBUTERANGE AttributeRange;
			AttributeRange.uStartIndex = uIndexStart + uCurrentIndexCount;
			AttributeRange.uIndexCount = uIndexCount;

			return AddGeometry(Mesh, &AttributeRange, pTransform, pTransformElementParameters, uTransformElementParameterCount);
		}
	}

	return true;
}

bool CGeometryInstance::ApplyForRender()
{
	if( !__IsValuable() )
		return false;

	CRenderQueue::LPRENDERENTRY pEntry;
	CProgramInstance* pProgramInstance;

	CEffect& Effect = m_pRenderMethod->GetEffect();
	UINT uProgramLength = m_ProgramInstances.GetLength();
	UINT uTotalPass = Effect.GetTotalPass(), i;
	for(i = 0; i < uTotalPass; ++ i)
	{
		pProgramInstance = (i < uProgramLength ? m_ProgramInstances[i] : ALCHEMY_NULL);

		pEntry = &m_pSceneManager->LockRenderEntry(*this);
		pEntry->uEffect            = Effect.GetResourceHandle();
		pEntry->uRenderPass        = i;
		pEntry->uVertexDeclaration = m_pVertexDeclaration->GetResourceHandle();
		pEntry->uModel             = 0;
		pEntry->uRenderTarget	   = pProgramInstance == ALCHEMY_NULL ? 0 : pProgramInstance->GetRenderTarget();
		pEntry->uModelParameter    = 0;

		pEntry->uUserData          = 0; 

		if(pProgramInstance)
			pProgramInstance->SubmitRenderEntry();
		else
			m_pSceneManager->UnlockRenderEntry(CSceneManager::EFFECT_NORMAL);
	}

	return true;
}

void CGeometryInstance::Render(const CRenderQueue::RENDERENTRY& Entry, UINT uFlag)
{
	if( !__IsValuable() )
		return;

	UINT uProgramLength = m_ProgramInstances.GetLength();
	CProgramInstance* pCurProgramInstance =  Entry.uRenderPass < uProgramLength ? m_ProgramInstances[ Entry.uRenderPass ] : ALCHEMY_NULL;

	if( ALCHEMY_TEST_BIT(uFlag, CRenderQueue::RENDER_TARGET) )
	{
		if(Entry.uRenderTarget)
			m_pSceneManager->GetRenderSystem().SetRenderTarget(Entry.uRenderTarget, 0, 0);
		else
			m_pSceneManager->SetSceneRenderTarget();
	}

	/*if(pCurProgramInstance)
		pCurProgramInstance->OnPreRenderSceneObject(this, uFlag);*/

	CEffect& Effect = m_pRenderMethod->GetEffect();

	if( ALCHEMY_TEST_BIT(uFlag, CRenderQueue::EFFECT) && !Effect.Begin() )
		return;

	if( ALCHEMY_TEST_BIT(uFlag, CRenderQueue::EFFECT_PASS) )
	{
		if( !Effect.BeginPass(Entry.uRenderPass) )
			return;

		m_pRenderMethod->Activate(Entry.uRenderPass);

		Effect.CommitChange();
	}

	/*if(pCurProgramInstance)
		pCurProgramInstance->OnRenderSceneObject(this, uFlag);*/

	if( ALCHEMY_TEST_BIT(uFlag, CRenderQueue::VERTEX_DECLARATION) && ( !Entry.uVertexDeclaration || !m_pSceneManager->GetRenderSystem().SetVertexDeclaration(Entry.uVertexDeclaration) ) )
		return;

	/*if(pCurProgramInstance)
		pCurProgramInstance->UpdatePassLogic(this);*/

	CVector<CMesh*>::CIterator Iterator = m_GeometryInstances.GetBeginIterator();
	CMesh* pMesh;
	while( CVector<CMesh*>::IsValuable(Iterator) )
	{
		pMesh = *Iterator;
		if(pMesh)
			pMesh->DrawSubset(0, true, false);
	}

	if(m_pCurrentInstance)
		m_pCurrentInstance->DrawSubset(0, true, false);

	/*if(pCurProgramInstance)
		pCurProgramInstance->OnEndRenderSceneObject(this, uFlag);*/
}