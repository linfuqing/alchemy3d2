#include "AlchemySkinMeshProcessor.h"
#include "AlchemyRenderQueue.h"
#include "AlchemyBone.h"
#include "AlchemySceneManager.h"

using namespace alchemy;

//CSkinMeshProcessor::CSkinMeshProcessor( CSkinMeshController& Controller, UINT uSubset, CAnimationController* pAnimationController )
//:m_uSubset(uSubset),
//m_pDestMesh(ALCHEMY_NULL),
//m_pSkinMeshController(ALCHEMY_NULL),
//m_pBoneMatrices(ALCHEMY_NULL),
//m_uBoneCount(0),
//m_pBoneCombination(ALCHEMY_NULL),
//m_pRenderDataNode(ALCHEMY_NULL),
//m_pAnimationController(ALCHEMY_NULL)
//{
//	SetSkinController( Controller );
//
//	if(pAnimationController)
//		SetAnimationController(*pAnimationController);
//}
CSkinMeshProcessor::CSkinMeshProcessor(
		CSkinMeshController::SKINTYPE SkinType,
		CSkinInfo& SkinInfo, 
		CBone& Root, 
		const CEffectCode::ELEMENTDECLARATION* pSkinElementParameters, 
		UINT uElementCount,
		UINT uMaxBoneInfluence) :
		m_SkinType(SkinType),
		m_SkinInfo(SkinInfo),
		m_Root(Root),
		m_pSkinElementParameters(pSkinElementParameters), 
		m_uElementCount(uElementCount),
		m_uMaxBoneInfluence(uMaxBoneInfluence),
		m_uSubset(0), 
		m_pDestMesh(ALCHEMY_NULL),
		m_pBoneMatrices(ALCHEMY_NULL),
		m_uBoneCount(0),
		m_pBoneCombination(ALCHEMY_NULL),
		m_pRenderDataNode(ALCHEMY_NULL)/*,
		m_pAnimationController(ALCHEMY_NULL)*/
{
	m_SkinInfo.AddRef();
}

CSkinMeshProcessor::~CSkinMeshProcessor()
{
	//if(m_pDestMesh)
	//	m_pDestMesh->Release();

	m_pDestMesh = ALCHEMY_NULL;

	//m_pSkinMeshController = ALCHEMY_NULL;

	RENDERDATA* pRenderData;
	while (m_pRenderDataNode != ALCHEMY_NULL)
	{
		pRenderData = m_pRenderDataNode->pNext;

		ALCHEMY_DEBUG_DELETE(m_pRenderDataNode->pAttribute);
		ALCHEMY_DEBUG_DELETE_ARRAY(m_pRenderDataNode->ppBoneMatrix);
		ALCHEMY_DEBUG_DELETE(m_pRenderDataNode);

		m_pRenderDataNode = pRenderData;
	}

	/*if(m_pAnimationController)
	{
		m_pAnimationController->Release();
		m_pAnimationController = ALCHEMY_NULL;
	}*/

	m_SkinInfo.Release();
}

IMeshProcessor* CSkinMeshProcessor::Clone()
{
	IMeshProcessor* pMeshProcessor;

	ALCHEMY_DEBUG_NEW( pMeshProcessor, CSkinMeshProcessor(
		m_SkinType,
		m_SkinInfo,
		m_Root,
		m_pSkinElementParameters, 
		m_uElementCount,
		m_uMaxBoneInfluence) );

	return pMeshProcessor;
}

CMesh* CSkinMeshProcessor::GetMesh()
{
	return m_pDestMesh;
}

//void CSkinMeshProcessor::SetAnimationController(CAnimationController& AnimController)
//{
//	CBone* pRootBone = m_SkinMeshController.GetRootBone();
//	if(pRootBone)
//		pRootBone->RegisterAnimationController(AnimController);
//
//	AnimController.AddRef();
//	if(m_pAnimationController)
//		m_pAnimationController->Release();
//
//	m_pAnimationController = &AnimController;
//}

//bool CSkinMeshProcessor::SetSkinController(CSkinMeshController& Controller )
//{
//	RENDERDATA* pRenderData;
//	while (m_pRenderDataNode != ALCHEMY_NULL)
//	{
//		pRenderData = m_pRenderDataNode->pNext;
//
//		ALCHEMY_DEBUG_DELETE(m_pRenderDataNode->pAttribute);
//		ALCHEMY_DEBUG_DELETE_ARRAY(m_pRenderDataNode->ppBoneMatrix);
//		ALCHEMY_DEBUG_DELETE(m_pRenderDataNode);
//
//		m_pRenderDataNode = pRenderData;
//	}
//
//	m_pSkinMeshController = &Controller;
//	m_pDestMesh = m_pSkinMeshController->GetDesinationMesh();
//	m_pDestMesh->AddRef();
//
//	if(m_pSkinMeshController && m_pSkinMeshController->GetSkinType() == CSkinMeshController::SKIN_HARDWARE)
//	{
//		m_pBoneMatrices = m_pSkinMeshController->GetBoneMatrix( m_uBoneCount );
//		m_pBoneCombination = m_pSkinMeshController->GetBoneCombination();
//		if(m_pBoneCombination == ALCHEMY_NULL)
//			return true;
//
//		ALCHEMY_DEBUG_NEW(m_pRenderDataNode, RENDERDATA);
//		CMesh::ATTRIBUTERANGE* pAttribute = m_pDestMesh->GetAttributeTable().Get( m_uSubset );
//		__UpdateSingleRenderData(pAttribute, *m_pRenderDataNode);
//	}
//
//	return true;
//}

CMesh* CSkinMeshProcessor::ConvertMesh(CMesh& Mesh, UINT uSubset)
{
	if( !m_pSkinElementParameters || !m_uElementCount )
	{
		if( ! m_SkinMeshController.Create(
			Mesh, 
			m_SkinType, 
			m_SkinInfo, 
			m_Root, 
			m_uMaxBoneInfluence) 
			)
			return ALCHEMY_NULL;
	}
	else
	{
		if( ! m_SkinMeshController.Create(
			Mesh, 
			m_SkinType,
			m_SkinInfo, 
			m_Root, 
			m_pSkinElementParameters, 
			m_uElementCount,
			m_uMaxBoneInfluence) )
			return ALCHEMY_NULL;
	}
	
	m_pDestMesh = m_SkinMeshController.GetDesinationMesh();
	if(m_pDestMesh)
		m_pDestMesh->AddRef();
		
	m_uSubset = uSubset;
		
	if(m_SkinType == CSkinMeshController::SKIN_HARDWARE)
	{
		m_pBoneMatrices    = m_SkinMeshController.GetBoneMatrix(m_uBoneCount);
		m_pBoneCombination = m_SkinMeshController.GetBoneCombination();
		if(m_pBoneCombination == ALCHEMY_NULL || !m_pDestMesh)
			return m_pDestMesh;

		ALCHEMY_DEBUG_NEW(m_pRenderDataNode, RENDERDATA);
		CMesh::ATTRIBUTERANGE* pAttribute = m_pDestMesh->GetAttributeTable().Get( m_uSubset );
		__UpdateSingleRenderData(pAttribute, *m_pRenderDataNode);
	}
	
	return m_pDestMesh;
}

//UINT CSkinMeshProcessor::GetSubset()
//{
//	return m_uSubset;
//}

void CSkinMeshProcessor::Destroy()
{
	if(m_pDestMesh)
		m_pDestMesh->Release();

	m_pDestMesh = ALCHEMY_NULL;
	//m_pSkinMeshController = ALCHEMY_NULL;

	RENDERDATA* pRenderData;
	while (m_pRenderDataNode != ALCHEMY_NULL)
	{
		pRenderData = m_pRenderDataNode->pNext;

		ALCHEMY_DEBUG_DELETE(m_pRenderDataNode->pAttribute);
		ALCHEMY_DEBUG_DELETE_ARRAY(m_pRenderDataNode->ppBoneMatrix);
		ALCHEMY_DEBUG_DELETE(m_pRenderDataNode);

		m_pRenderDataNode = pRenderData;
	}
}

void CSkinMeshProcessor::Update(FLOAT fElapse)
{
	/*if(m_pAnimationController)
		m_pAnimationController->Update(fElapse);*/

	//if(m_pSkinMeshController)
	//	m_pSkinMeshController->Update();
	m_SkinMeshController.Update();
}

bool CSkinMeshProcessor::OnProcess(IRenderMethod& RenderMethod, UINT uRenderFlag)
{
	if(!m_pDestMesh)
		return false;

	if(m_pRenderDataNode && m_pBoneCombination)
	{
		RENDERDATA* pRenderData = m_pRenderDataNode;
		while(pRenderData)
		{
			RenderMethod.SetArray(IRenderMethod::BONE_MATRIX_ARRAY, pRenderData->ppBoneMatrix, 4, 3, 4, 16, pRenderData->uBoneCount, true);
			m_pDestMesh->DrawSubset(*(pRenderData->pAttribute) , true, false);
			pRenderData = pRenderData->pNext;
		}
	}
	else
		m_pDestMesh->DrawSubset( m_uSubset, ALCHEMY_TEST_BIT(uRenderFlag, CRenderQueue::MODEL), ALCHEMY_TEST_BIT(uRenderFlag, CRenderQueue::ENTRY_VERTEX_DECLARATION) );

	return true;
}

void CSkinMeshProcessor::__UpdateSingleRenderData( CMesh::ATTRIBUTERANGE* pAttributeRange, RENDERDATA& RenderData )
{
	UINT uStartIndex, uEndIndex;

	RenderData.pNext = ALCHEMY_NULL;
	RenderData.ppBoneMatrix = ALCHEMY_NULL;
	RenderData.uBoneCount = 0;
	RenderData.pAttribute = ALCHEMY_NULL;

//	if(!m_pSkinMeshController)
//		return;

	m_SkinMeshController.GetBoneCombinationRange(&uStartIndex, &uEndIndex, *pAttributeRange);

	if(uStartIndex > uEndIndex)
		return;

	RENDERDATA*  pRenderData = ALCHEMY_NULL, *pLastRenderData = ALCHEMY_NULL;

	const CMesh::BONECOMBINATION* pBoneCombination = m_pBoneCombination + uStartIndex;

	for (UINT i = uStartIndex; i <= uEndIndex; ++i)
	{
		if(pRenderData == ALCHEMY_NULL)
			pRenderData = &RenderData;
		else
			ALCHEMY_DEBUG_NEW(pRenderData, RENDERDATA);

		ALCHEMY_DEBUG_NEW(pRenderData->pAttribute, CMesh::ATTRIBUTERANGE);

		pRenderData->pAttribute->uStartIndex = Maxinum( pAttributeRange->uStartIndex, pBoneCombination->uStartIndex );
		pRenderData->pAttribute->uIndexCount = Mininum( pAttributeRange->uStartIndex + pAttributeRange->uIndexCount, pBoneCombination->uStartIndex + pBoneCombination->uIndexCount ) - pRenderData->pAttribute->uStartIndex;

		ALCHEMY_DEBUG_NEW(pRenderData->ppBoneMatrix, CMatrix4x4*[pBoneCombination->uBoneCount]);
		pRenderData->uBoneCount = pBoneCombination->uBoneCount;

		for(UINT j = 0; j < pBoneCombination->uBoneCount; ++ j)
			pRenderData->ppBoneMatrix[j] = &m_pBoneMatrices[ pBoneCombination->puBoneIndices[j] ];

		pRenderData->pNext = ALCHEMY_NULL;
		if(pLastRenderData)
			pLastRenderData->pNext = pRenderData;

		pLastRenderData = pRenderData;

		++pBoneCombination;
	}
}