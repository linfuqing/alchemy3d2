/*
 *  AlchemyInstanceMeshProcessor.cpp
 *  IOS
 *
 *  Created by lin fuqing on 12-5-21.
 *  Copyright 2012 AstepGame. All rights reserved.
 *
 */

#include "AlchemyInstanceMeshProcessor.h"
#include "AlchemySceneManager.h"

using namespace alchemy;

CInstanceMeshProcessor::CInstanceMeshProcessor(FLOAT fInstanceCount, const MATRIX4X4* pMatrices) :
m_pDestinationMatrices(ALCHEMY_NULL), 
m_pMesh(ALCHEMY_NULL),
m_uSubset(0),
m_fInstanceCount(fInstanceCount)
{
	m_pDestinationMatrices = pMatrices;
}

CInstanceMeshProcessor::~CInstanceMeshProcessor()
{
	ALCHEMY_DEBUG_DELETE_ARRAY(m_pDestinationMatrices);
}

IMeshProcessor* CInstanceMeshProcessor::Clone()
{
	IMeshProcessor* pMeshProcessor;

	ALCHEMY_DEBUG_NEW( pMeshProcessor, CInstanceMeshProcessor(m_fInstanceCount, m_pDestinationMatrices) );

	return pMeshProcessor;
}

CMesh* CInstanceMeshProcessor::GetMesh()
{
	return m_pMesh;
}

CMesh* CInstanceMeshProcessor::ConvertMesh(CMesh& Mesh, UINT uSubset)
{	
	FLOAT fInstanceCount = m_fInstanceCount;
	if( !static_cast<CSceneManager&>( Mesh.GetGraphicsResourcePool() ).GetRenderSystem().CheckFormat(CRenderSystem::INDEX_BUFFER, CIndexBuffer::BIT32) )
	{
		LPVERTEXBUFFER pVertexBuffer = Mesh.GetVertex().GetVertexBuffer(0);
		if(pVertexBuffer)
		{
			fInstanceCount = static_cast<FLOAT>(0xffff) / pVertexBuffer->GetMemberCount();
			if(m_fInstanceCount < fInstanceCount)
				fInstanceCount = m_fInstanceCount;
		}
	}
	
	UINT uInstanceCount = ALCHEMY_CEIL(fInstanceCount);
	
	const CMesh::ATTRIBUTERANGE* pAttributeRange = Mesh.GetAttributeTable().Get(uSubset);
	if(!pAttributeRange)
		return ALCHEMY_NULL;
	
	CMesh* pMesh = ALCHEMY_NULL;
	if(pAttributeRange->uStartIndex == 0 && Mesh.GetIndexBuffer().GetMemberCount() == pAttributeRange->uIndexCount)
	{
		if(m_pDestinationMatrices)
			pMesh = Mesh.Clone(fInstanceCount, m_pDestinationMatrices);
		else
			pMesh = Mesh.ConvertToInstancedMesh(fInstanceCount);
	}
	else
	{
		pMesh = Mesh.Clone(pAttributeRange, 1);
		
		if(pMesh)
		{
			CMesh* pInstancedMesh = ALCHEMY_NULL;
			if(m_pDestinationMatrices)
				pInstancedMesh = pMesh->Clone(fInstanceCount, m_pDestinationMatrices);
			else
				pInstancedMesh = pMesh->ConvertToInstancedMesh(fInstanceCount);

			if(pInstancedMesh)
			{
				pMesh->Release();
				
				pMesh = pInstancedMesh;
			}
		}
	}
	
	if(m_pMesh)
		m_pMesh->Release();
	
	m_pMesh = pMesh;
	
	m_uSubset = 0;
	
	return m_pMesh;
}

void CInstanceMeshProcessor::Update(FLOAT fElapse)
{
}

bool CInstanceMeshProcessor::OnProcess(IRenderMethod& RenderMethod, UINT uRenderFlag)
{
	if(m_pMesh)
		return m_pMesh->DrawSubset( 0, ALCHEMY_TEST_BIT(uRenderFlag, CRenderQueue::MODEL), ALCHEMY_TEST_BIT(uRenderFlag, CRenderQueue::ENTRY_VERTEX_DECLARATION) );
	
	return false;
}