#include "AlchemyRenderQueue.h"
#include "AlchemyRenderSystem.h"

using namespace alchemy;

CPool<CRenderQueue::RENDERLISTNODE> CRenderQueue::sm_RenderEntryPool;

CRenderQueue::CRenderEntry::CRenderEntry()
{

}

CRenderQueue::CRenderEntry:: ~CRenderEntry()
{

}

void CRenderQueue::CRenderEntry::PushRenderEntryNodeTo(CVector<RENDERLIST>& RenderLists)const
{
	CVector<RENDERENTRYNODEINFO>::CConstantIterator Iterator = m_RenderEntryNodeInfos.GetIterator(0);
	const RENDERENTRYNODEINFO* pRenderEntryNodeInfo;
	LPRENDERLIST pRenderList;

	while( CVector<RENDERENTRYNODEINFO>::IsValuable(Iterator) )
	{
		pRenderEntryNodeInfo = &(*Iterator);
		pRenderList = RenderLists.Get(pRenderEntryNodeInfo->uRenderList);
		if(pRenderList)
		{
			if(pRenderList->pTail)
			{
				if(pRenderList->pHead)
					pRenderList->pTail = pRenderList->pTail->pNext = CRenderQueue::sm_RenderEntryPool.Get(pRenderEntryNodeInfo->uRenderEntryNodeIndex);
				else
					pRenderList->pHead = pRenderList->pTail = pRenderList->pTail->pNext = CRenderQueue::sm_RenderEntryPool.Get(pRenderEntryNodeInfo->uRenderEntryNodeIndex);
			}
			else
			{
				if(pRenderList->pHead)
					pRenderList->pTail = CRenderQueue::sm_RenderEntryPool.Get(pRenderEntryNodeInfo->uRenderEntryNodeIndex);
				else
					pRenderList->pHead = pRenderList->pTail = CRenderQueue::sm_RenderEntryPool.Get(pRenderEntryNodeInfo->uRenderEntryNodeIndex);
			}
		}

		++ Iterator;
	}
}

void CRenderQueue::CRenderEntry::AddRenderEntryNode(UINT uRenderEntryNodeIndex, UINT uRenderList)
{
	RENDERENTRYNODEINFO RenderEntryNodeInfo;

	RenderEntryNodeInfo.uRenderEntryNodeIndex = uRenderEntryNodeIndex;
	RenderEntryNodeInfo.uRenderList           = uRenderList;

	m_RenderEntryNodeInfos.Add(RenderEntryNodeInfo);
}

CRenderQueue::CSceneTreeObject::CSceneTreeObject(const IBoundingVolume& BoundingVolume) :
m_BoundingVolume(BoundingVolume)
{

}

CRenderQueue::CSceneTreeObject::~CSceneTreeObject()
{

}

const IBoundingVolume& CRenderQueue::CSceneTreeObject::GetBoundingVolume()const
{
	return m_BoundingVolume;
}

CRenderQueue::CRenderQueue(CRenderSystem& RenderSystem, const IBoundingVolume* pBoundingVolume, const IClipper* pClipper) :
m_RenderSystem(RenderSystem),
m_RenderTarget(0),
m_CurrentRenderTarget(0),
m_uCurrentRenderEntryIndex(0), 
m_uCurrentRenderList(0), 
m_bIsSort(false),
m_uRenderTargetSurface(0),
m_uCurrentRenderTargetSurface(0),
m_pRenderTarget(ALCHEMY_NULL)
{
	RENDERLIST RenderList;
	RenderList.pHead = ALCHEMY_NULL;
	RenderList.pTail = ALCHEMY_NULL;

	RenderList.pBoundingVolume = pBoundingVolume;
	RenderList.pClipper        = pClipper;

	RenderList.bIsSort = true;

	m_RenderLists.Add(RenderList);
	m_RenderLists.Add(RenderList);
	m_RenderLists.Add(RenderList);

	RenderList.bIsSort = false;
	m_uCurrentRenderList = m_RenderLists.Add(RenderList);

	m_pCurrentRenderEntry = ALCHEMY_NULL;
}

CRenderQueue::~CRenderQueue(void)
{
	CVector<CRenderEntry*>::CIterator RenderEntryIterator =  m_RenderEntris.GetIterator(0);
	CRenderEntry* pRenderEntry;
	while( CVector<CRenderEntry*>::IsValuable(RenderEntryIterator) )
	{
		pRenderEntry = *RenderEntryIterator;
		if(pRenderEntry)
			ALCHEMY_DEBUG_DELETE(pRenderEntry);

		++ RenderEntryIterator;
	}
}

UINT CRenderQueue::BuildRenderFlag(
								   const CRenderQueue::RENDERENTRY& CurrentEntry, 
								   const CRenderQueue::RENDERKEY& CurrentKey, 
								   const CRenderQueue::RENDERENTRY& PreviousEntry, 
								   const CRenderQueue::RENDERKEY& PreviousKey, 
								   bool bIsParentDirty)
{
	UINT uFlag = 0;

	if(PreviousEntry.uRenderTarget != CurrentEntry.uRenderTarget)
		ALCHEMY_SET_BIT(uFlag, ENTRY_RENDER_TARGET);

	if(PreviousEntry.uEffect != CurrentEntry.uEffect)
	{
		ALCHEMY_SET_BIT(uFlag, ENTRY_EFFECT);
		ALCHEMY_SET_BIT(uFlag, ENTRY_EFFECT_PASS);
	}
	else if (PreviousEntry.uRenderPass != CurrentEntry.uRenderPass)
		ALCHEMY_SET_BIT(uFlag, ENTRY_EFFECT_PASS);

	if(PreviousEntry.uVertexDeclaration != CurrentEntry.uVertexDeclaration)
		ALCHEMY_SET_BIT(uFlag, ENTRY_VERTEX_DECLARATION);

	if(PreviousEntry.uModel != CurrentEntry.uModel)
	{
		ALCHEMY_SET_BIT(uFlag, MODEL);
		ALCHEMY_SET_BIT(uFlag, MODEL_PARAMETER);
	}
	else if (PreviousEntry.uModelParameter != CurrentEntry.uModelParameter)
		ALCHEMY_SET_BIT(uFlag, MODEL_PARAMETER);	

	if(bIsParentDirty)
		ALCHEMY_SET_BIT(uFlag, PARENT);

	if(PreviousKey.Effect != CurrentKey.Effect)
	{
		ALCHEMY_SET_BIT(uFlag, KEY_EFFECT);
		ALCHEMY_SET_BIT(uFlag, KEY_EFFECT_PASS);
	}
	else if (PreviousKey.uRenderPass != CurrentKey.uRenderPass)
		ALCHEMY_SET_BIT(uFlag, KEY_EFFECT_PASS);

	if(PreviousKey.VertexDeclaration != CurrentKey.VertexDeclaration)
		ALCHEMY_SET_BIT(uFlag, KEY_VERTEX_DECLARATION);

	if(PreviousKey.uRenderTargetIndex != CurrentKey.uRenderTargetIndex)
		ALCHEMY_SET_BIT(uFlag, KEY_RENDER_TARGET);

	return uFlag;
}

LPTEXTURE CRenderQueue::CreateSceneRenderTarget(INT nShift)
{
	if(m_pRenderTarget)
		m_pRenderTarget->Release();

	m_pRenderTarget = m_RenderSystem.CreateRenderTargetView(CTexture::DEFAULT, nShift);

	return m_pRenderTarget;
}

UINT CRenderQueue::AddRenderList(bool bIsSort, const IBoundingVolume* pBoundingVolume, const IClipper* pClipper)
{
	RENDERLIST RenderList;
	RenderList.pHead = ALCHEMY_NULL;
	RenderList.pTail = ALCHEMY_NULL;

	RenderList.bIsSort = bIsSort;

	RenderList.pBoundingVolume = pBoundingVolume;
	RenderList.pClipper        = pClipper;

	return m_RenderLists.Add(RenderList);
}

UINT CRenderQueue::AddRenderTargetToQueue(CGraphicsResource::HANDLE RenderTarget, UINT uRenderList, ARGBCOLOR ClearColor)
{
	LPRENDERLIST pRenderList = m_RenderLists.Get(uRenderList);
	if(pRenderList)
	{
		RENDERTARGETINFO RenderTargetInfo;
		RenderTargetInfo.RenderTarget = RenderTarget;
		RenderTargetInfo.ClearColor = ClearColor;

		return pRenderList->RenderTargetInfos.Add(RenderTargetInfo) + 1;
	}

	return 0;
}

bool CRenderQueue::SetRenderTargetToQueue(CGraphicsResource::HANDLE RenderTarget, UINT uRenderTargetIndex, UINT uRenderList, ARGBCOLOR ClearColor)
{
	LPRENDERLIST pRenderList = m_RenderLists.Get(uRenderList);
	if(pRenderList)
	{
		RENDERTARGETINFO RenderTargetInfo;
		RenderTargetInfo.RenderTarget = RenderTarget;
		RenderTargetInfo.ClearColor = ClearColor;

		return pRenderList->RenderTargetInfos.Set(RenderTargetInfo, uRenderTargetIndex);
	}

	return false;
}

bool CRenderQueue::ClearRenderTargetQueue(UINT uRenderList)
{
	LPRENDERLIST pRenderList = m_RenderLists.Get(uRenderList);
	if(pRenderList)
	{
		pRenderList->RenderTargetInfos.Reset();

		return true;
	}

	return false;
}

bool CRenderQueue::SetRenderTarget(CGraphicsResource::HANDLE RenderTarget, UINT uRenderTargetSurface, bool bIsSaved)
{
	if( m_RenderSystem.SetRenderTarget(RenderTarget, uRenderTargetSurface, 0) )
	{
		m_CurrentRenderTarget         = RenderTarget;
		m_uCurrentRenderTargetSurface = uRenderTargetSurface;

		if(bIsSaved)
		{
			m_RenderTarget         = RenderTarget;

			m_uRenderTargetSurface = uRenderTargetSurface;
		}

		return true;
	}

	ALCHEMY_DEBUG_WARNING("CRenderQueue::SetRenderTarget: Set render target fail.");

	return false;
}

bool CRenderQueue::SetRenderTarget()
{
	m_CurrentRenderTarget         = m_RenderTarget;
	m_uCurrentRenderTargetSurface = m_uRenderTargetSurface;

	return m_RenderSystem.SetRenderTarget(m_RenderTarget, m_uRenderTargetSurface, 0);
}

CRenderQueue::RENDERENTRY& CRenderQueue::LockRenderEntry(CRenderQueue::ICallBackToRender& Renderer, bool* pIsVisible, IBoundingVolume* pBoundingVolume, UINT uSceneTreeManagerIndex)
{
	if(m_pCurrentRenderEntry)
		return m_pCurrentRenderEntry->Data.RenderEntry;

	RENDERLISTNODE s_RenderEntry;
	m_pCurrentRenderEntry = sm_RenderEntryPool.Get( m_uCurrentRenderEntryIndex = sm_RenderEntryPool.Add(s_RenderEntry) );
	m_pCurrentRenderEntry->Data.pRenderer              = &Renderer;
	m_pCurrentRenderEntry->Data.pBoundingVolume        = pBoundingVolume;
	m_pCurrentRenderEntry->Data.uSceneTreeManagerIndex = uSceneTreeManagerIndex;
	m_pCurrentRenderEntry->Data.pIsVisible			   = pIsVisible;
	m_pCurrentRenderEntry->pNext                       = ALCHEMY_NULL;

	return m_pCurrentRenderEntry->Data.RenderEntry;
}

bool CRenderQueue::UnlockRenderEntry(UINT uRenderList, UINT uFlag)
{
	if(m_pCurrentRenderEntry)
	{
		LPRENDERLIST pRenderList = m_RenderLists.Get(uRenderList);
		if(pRenderList)
		{
			LPRENDERLISTNODE pCurrentRenderEntry = m_pCurrentRenderEntry;
			m_pCurrentRenderEntry = ALCHEMY_NULL;

			LPSCENETREEMANAGER pSceneTreeManager = m_SceneTreeManagers.Get(pCurrentRenderEntry->Data.uSceneTreeManagerIndex);
			if(pSceneTreeManager)
			{
				PUINT puRenderEntryIndex = pSceneTreeManager->RenderEntryMap.Get(pCurrentRenderEntry->Data.pBoundingVolume);
				if(puRenderEntryIndex)
				{
					CRenderEntry** ppRenderEntry = m_RenderEntris.Get(*puRenderEntryIndex);
					if(ppRenderEntry && *ppRenderEntry)
						(*ppRenderEntry)->AddRenderEntryNode(m_uCurrentRenderEntryIndex, uRenderList);
				}
				else if(pCurrentRenderEntry->Data.pBoundingVolume)
				{
					CSceneTreeObject* pSceneTreeObject;

					ALCHEMY_DEBUG_NEW( pSceneTreeObject, CSceneTreeObject(*pCurrentRenderEntry->Data.pBoundingVolume) );

					pSceneTreeManager->RenderEntryMap.Insert( pCurrentRenderEntry->Data.pBoundingVolume, m_RenderEntris.Add(pSceneTreeObject) );

					if(pSceneTreeManager->pSceneTreeManager)
					{
						pSceneTreeManager->pSceneTreeManager->AddObject(*pSceneTreeObject);

						m_bIsSort = true;
					}
				}
			}

			pCurrentRenderEntry->Data.RenderKey.VertexDeclaration  = pCurrentRenderEntry->Data.RenderEntry.uVertexDeclaration;
			pCurrentRenderEntry->Data.RenderKey.Effect             = pCurrentRenderEntry->Data.RenderEntry.uEffect;
			pCurrentRenderEntry->Data.RenderKey.uRenderPass        = pCurrentRenderEntry->Data.RenderEntry.uRenderPass;
			pCurrentRenderEntry->Data.RenderKey.uRenderTargetIndex = 0;
			pCurrentRenderEntry->Data.RenderKey.uFlag              = uFlag;

			RENDERTARGETINFO RenderTargetInfo;
			RenderTargetInfo.RenderTarget = pCurrentRenderEntry->Data.RenderEntry.uRenderTarget;
			RenderTargetInfo.ClearColor   = 0;
			if( pCurrentRenderEntry->Data.RenderEntry.uRenderTarget && pRenderList->RenderTargetInfos.GetIndexOf<RENDERTARGETINFOEQUAL>(pCurrentRenderEntry->Data.RenderKey.uRenderTargetIndex, RenderTargetInfo) )
				++ pCurrentRenderEntry->Data.RenderKey.uRenderTargetIndex;

			if(uFlag)
			{
				if( ALCHEMY_TEST_BIT(uFlag, AUTO_RENDER_TARGET) && !pCurrentRenderEntry->Data.RenderKey.uRenderTargetIndex)
				{
					UINT uCurrentRenderTargetCount = pRenderList->RenderTargetInfos.GetLength(), uCurrentFlag = ALCHEMY_CLEAR_BIT(uFlag, AUTO_RENDER_TARGET), uRenderTargetCount = m_RenderSystem.GetRenderSystemInfo().uRenderTargetCount;
					LPRENDERENTRY pRenderEntry;
					for(UINT i = 0; i < uCurrentRenderTargetCount; i += uRenderTargetCount)
					{
						if(pCurrentRenderEntry->Data.RenderKey.uRenderTargetIndex == i + 1)
							continue;

						pRenderEntry = &LockRenderEntry(*pCurrentRenderEntry->Data.pRenderer);
						pRenderEntry->uSortValueA = pCurrentRenderEntry->Data.RenderEntry.uSortValueA;
						pRenderEntry->uSortValueB = pCurrentRenderEntry->Data.RenderEntry.uSortValueB;
						pRenderEntry->uSortValueC = pCurrentRenderEntry->Data.RenderEntry.uSortValueC;
						
						pRenderEntry->uUserData   = pCurrentRenderEntry->Data.RenderEntry.uUserData;

						pRenderEntry->uRenderTarget = pRenderList->RenderTargetInfos[i].RenderTarget;

						UnlockRenderEntry(uRenderList, uCurrentFlag);
					}
				}

				if( ALCHEMY_TEST_BIT(uFlag, AUTO_SHADER) )
				{
					CEffect* pEffect = static_cast<CEffect*>( m_RenderSystem.GetResource(pCurrentRenderEntry->Data.RenderKey.Effect, CGraphicsResource::EFFECT) );
					if(pEffect)
					{
						CShader* pShader = ALCHEMY_NULL, *pAdditionalShader = ALCHEMY_NULL;

						if( pEffect->GetShader(pCurrentRenderEntry->Data.RenderKey.uRenderPass, pCurrentRenderEntry->Data.RenderKey.uRenderTargetIndex, &pShader, &pAdditionalShader) )
						{
							pCurrentRenderEntry->Data.RenderEntry.uEffect     = pShader ? pShader->GetResourceHandle() : 0;
							pCurrentRenderEntry->Data.RenderEntry.uRenderPass = pAdditionalShader ? pAdditionalShader->GetResourceHandle() : 0;
						}
					}
				}

				if( ALCHEMY_TEST_BIT(uFlag, AUTO_VERTEX_DECLARATION) )
				{
					CGraphicsResource::HANDLE* pVertexDeclaration = m_VertexDeclarationMap.Get(pCurrentRenderEntry->Data.RenderKey);
					if(pVertexDeclaration)
						pCurrentRenderEntry->Data.RenderEntry.uVertexDeclaration = *pVertexDeclaration;
					else
					{
						CVertexDeclaration* pVertexDeclaration = static_cast<CVertexDeclaration*>( m_RenderSystem.GetResource(pCurrentRenderEntry->Data.RenderKey.VertexDeclaration, CGraphicsResource::VERTEX_DECLARATION) );
						if(pVertexDeclaration)
						{
							CEffect* pEffect = static_cast<CEffect*>( m_RenderSystem.GetResource(pCurrentRenderEntry->Data.RenderKey.Effect, CGraphicsResource::EFFECT) );
							if(pEffect)
							{
								const CEffect::PASSINFO* pPassInfo = pEffect->GetPassInfo(pCurrentRenderEntry->Data.RenderKey.uRenderPass, pCurrentRenderEntry->Data.RenderKey.uRenderTargetIndex);
								if(pPassInfo)
								{
									pVertexDeclaration = pVertexDeclaration->Clone(pPassInfo->pVertexElements, pPassInfo->uVertexElementCount);
									if(pVertexDeclaration)
										m_VertexDeclarationMap.Insert( pCurrentRenderEntry->Data.RenderKey, pCurrentRenderEntry->Data.RenderEntry.uVertexDeclaration = pVertexDeclaration->GetResourceHandle() );
								}
							}
						}
					}
				}
			}

			if(pRenderList->bIsSort)
			{
				LPRENDERLISTNODE& pRenderEntryHead = pRenderList->pHead;

				if(pRenderEntryHead == ALCHEMY_NULL)
					pRenderEntryHead = pCurrentRenderEntry;
				else
				{
					pCurrentRenderEntry->pNext = pRenderEntryHead;
					pRenderEntryHead           = pCurrentRenderEntry;
				}

				pRenderList->pTail = ALCHEMY_NULL;
			}
			else
			{
				LPRENDERLISTNODE& pRenderEntry = pRenderList->pTail;
				if(pRenderEntry)
				{
					pCurrentRenderEntry->Data.uFlag = BuildRenderFlag(
						pCurrentRenderEntry->Data.RenderEntry, 
						pCurrentRenderEntry->Data.RenderKey,
						pRenderEntry->Data.RenderEntry, 
						pRenderEntry->Data.RenderKey,
						pCurrentRenderEntry->Data.pRenderer != pRenderEntry->Data.pRenderer);

					pRenderList->pTail = pRenderEntry->pNext = pCurrentRenderEntry;
				}
				else
				{
					pRenderEntry = pRenderList->pHead;
					if(pRenderEntry)
					{
						while(pRenderEntry->pNext)
							pRenderEntry = pRenderEntry->pNext;

						pCurrentRenderEntry->Data.uFlag = BuildRenderFlag(
							pCurrentRenderEntry->Data.RenderEntry, 
							pCurrentRenderEntry->Data.RenderKey,
							pRenderEntry->Data.RenderEntry, 
							pRenderEntry->Data.RenderKey,
							pCurrentRenderEntry->Data.pRenderer != pRenderEntry->Data.pRenderer);

						pRenderList->pTail = pRenderEntry->pNext = pCurrentRenderEntry;
					}
					else
					{
						pRenderList->pHead = pRenderList->pTail = pCurrentRenderEntry;

						pCurrentRenderEntry->Data.uFlag = ~0;
					}
				}
			}

			return true;
		}
	}

	return false;
}

bool CRenderQueue::ActivateMultipleRenderTarget(UINT uFromIndex, UINT uCount)
{
	if( uFromIndex && m_uCurrentRenderList < m_RenderLists.GetLength() )
	{
		UINT uRenderTargetCount = ALCHEMY_MININUM(uCount, m_RenderSystem.GetRenderSystemInfo().uRenderTargetCount);
		CVector<RENDERTARGETINFO>::CIterator Iterator = m_RenderLists[m_uCurrentRenderList].RenderTargetInfos.GetIterator(uFromIndex);
		for(UINT i = 1; i < uRenderTargetCount; ++ i)
		{
			if( !CVector<RENDERTARGETINFO>::IsValuable(Iterator) )
				break;

			m_RenderSystem.SetRenderTarget(Iterator->RenderTarget, 0, i);

			++ Iterator;
		}

		//m_RenderSystem.Clear( 0, ALCHEMY_FLAG(CRenderSystem::TARGET) | ALCHEMY_FLAG(CRenderSystem::STENCIL) | ALCHEMY_FLAG(CRenderSystem::ZBUFFER) );

		return true;
	}

	return false;
}

void CRenderQueue::ResetScene()
{
	UINT uIndex;
	LPRENDERLISTNODE pRenderListNode;
	LPRENDERLIST pRenderList;
	CVector<RENDERLIST>::CIterator RenderListIterator = m_RenderLists.GetIterator(0);
	while( CVector<RENDERLIST>::IsValuable(RenderListIterator) )
	{
		pRenderList = &(*RenderListIterator);
		pRenderListNode = pRenderList->pHead;
		while(pRenderListNode)
		{
			if( sm_RenderEntryPool.GetIndexOf(uIndex, *pRenderListNode) )
				sm_RenderEntryPool.Remove(uIndex, 1);

			pRenderListNode = pRenderListNode->pNext;
		}

		pRenderList->pHead = ALCHEMY_NULL;
		pRenderList->pTail = ALCHEMY_NULL;

		++ RenderListIterator;
	}

	m_pCurrentRenderEntry = ALCHEMY_NULL;

	m_bIsSort = false;

	m_RenderTarget = 0;

	CMap<RENDERKEY, CGraphicsResource::HANDLE, RENDERKEYLESS, RENDERKEYEQUAL>::CIterator VertexDeclarationIterator = m_VertexDeclarationMap.GetBeginIterator();
	CVertexDeclaration* pVertexDeclaration;
	while( CMap<RENDERKEY, CGraphicsResource::HANDLE, RENDERKEYLESS, RENDERKEYEQUAL>::IsValuable(VertexDeclarationIterator) )
	{
		pVertexDeclaration = static_cast<CVertexDeclaration*>( m_RenderSystem.GetResource(VertexDeclarationIterator.GetValue(), CGraphicsResource::VERTEX_DECLARATION) );
		if(pVertexDeclaration)
			pVertexDeclaration->Release();

		++ VertexDeclarationIterator;
	}

	m_VertexDeclarationMap.Clear();

	CVector<CRenderEntry*>::CIterator RenderEntryIterator =  m_RenderEntris.GetIterator(0);
	CRenderEntry* pRenderEntry;
	while( CVector<CRenderEntry*>::IsValuable(RenderEntryIterator) )
	{
		pRenderEntry = *RenderEntryIterator;
		if(pRenderEntry)
			ALCHEMY_DEBUG_DELETE(pRenderEntry);
		
		++ RenderEntryIterator;
	}

	m_RenderEntris.Clear();

	CVector<SCENETREEMANAGER>::CIterator SceneTreeManagerIterator =  m_SceneTreeManagers.GetIterator(0);
	while( CVector<SCENETREEMANAGER>::IsValuable(SceneTreeManagerIterator) )
	{
		SceneTreeManagerIterator->RenderEntryMap.Clear();

		++ SceneTreeManagerIterator;
	}
}

void CRenderQueue::RenderScene(ARGBCOLOR BackgroudColor, UINT uClearFlag)
{
	if(uClearFlag)
		m_RenderSystem.Clear(BackgroudColor, uClearFlag);

	RenderScene(NORMAL);
}

void CRenderQueue::RenderScene(UINT uRenderList)
{
	LPRENDERLIST pRenderList = m_RenderLists.Get(uRenderList);

	if(!pRenderList)
		return;

	LPRENDERLISTNODE pCurrentRenderNode = pRenderList->pHead;
	if(!pCurrentRenderNode)
		return;

	CVector<RENDERTARGETINFO>::CIterator Iterator = pRenderList->RenderTargetInfos.GetIterator(0);
	LPRENDERTARGETINFO pRenderTargetInfo = ALCHEMY_NULL;
	while( CVector<RENDERTARGETINFO>::IsValuable(Iterator) )
	{
		pRenderTargetInfo = &(*Iterator);
		m_RenderSystem.SetRenderTarget(pRenderTargetInfo->RenderTarget, 0, 0);
		m_RenderSystem.Clear( pRenderTargetInfo->ClearColor, ALCHEMY_FLAG(CRenderSystem::TARGET) );

		++ Iterator;
	}

	if(!pRenderTargetInfo)
		m_RenderSystem.SetRenderTarget(m_CurrentRenderTarget, m_uCurrentRenderTargetSurface, 0);

	LPRENDERLISTNODE pFinalRenderNode = pRenderList->pTail;
	if(pRenderList->bIsSort && !pFinalRenderNode)
		pCurrentRenderNode = pRenderList->pHead = ALCHEMY_LIST_SORT(RENDERENTRYDATA, RENDERENTRYDATALESS)(pCurrentRenderNode);

	m_uCurrentRenderList = uRenderList;

	LPRENDERLISTNODE pPreviousRenderNode = ALCHEMY_NULL;
	if(pRenderList->pClipper)
	{
		if(pFinalRenderNode)
		{
			while(pCurrentRenderNode)
			{
				if( pCurrentRenderNode->Data.pRenderer && ( !pCurrentRenderNode->Data.pBoundingVolume || pRenderList->pClipper->Test(*pCurrentRenderNode->Data.pBoundingVolume) ) && ( !pCurrentRenderNode->Data.pIsVisible || *pCurrentRenderNode->Data.pIsVisible ) )
				{
					pCurrentRenderNode->Data.pRenderer->Render(
						pCurrentRenderNode->Data.RenderEntry, 
						pCurrentRenderNode->Data.RenderKey,
						pPreviousRenderNode ? (
						pPreviousRenderNode->pNext == pCurrentRenderNode ? pCurrentRenderNode->Data.uFlag : BuildRenderFlag(
						pCurrentRenderNode->Data.RenderEntry, 
						pCurrentRenderNode->Data.RenderKey,
						pPreviousRenderNode->Data.RenderEntry, 
						pPreviousRenderNode->Data.RenderKey,
						pCurrentRenderNode->Data.pRenderer != pPreviousRenderNode->Data.pRenderer) ) : ~0);

					pPreviousRenderNode = pCurrentRenderNode;
				}

				pCurrentRenderNode  = pCurrentRenderNode->pNext;
			}
		}
		else
		{
			pCurrentRenderNode->Data.uFlag = ~0;

			if( pCurrentRenderNode->Data.pRenderer && ( !pCurrentRenderNode->Data.pBoundingVolume || pRenderList->pClipper->Test(*pCurrentRenderNode->Data.pBoundingVolume) ) && ( !pCurrentRenderNode->Data.pIsVisible || *pCurrentRenderNode->Data.pIsVisible ) )
			{
				pCurrentRenderNode->Data.pRenderer->Render(pCurrentRenderNode->Data.RenderEntry, pCurrentRenderNode->Data.RenderKey, pCurrentRenderNode->Data.uFlag);

				pPreviousRenderNode = pCurrentRenderNode;
			}

			while(pCurrentRenderNode->pNext)
			{
				pCurrentRenderNode->pNext->Data.uFlag = BuildRenderFlag(
					pCurrentRenderNode->pNext->Data.RenderEntry, 
					pCurrentRenderNode->pNext->Data.RenderKey,
					pCurrentRenderNode->Data.RenderEntry, 
					pCurrentRenderNode->Data.RenderKey,
					pCurrentRenderNode->pNext->Data.pRenderer != pCurrentRenderNode->Data.pRenderer);

				pCurrentRenderNode = pCurrentRenderNode->pNext;

				if( pCurrentRenderNode->Data.pRenderer && ( !pCurrentRenderNode->Data.pBoundingVolume || pRenderList->pClipper->Test(*pCurrentRenderNode->Data.pBoundingVolume) ) && ( !pCurrentRenderNode->Data.pIsVisible || *pCurrentRenderNode->Data.pIsVisible ) )
				{
					pCurrentRenderNode->Data.pRenderer->Render(
						pCurrentRenderNode->Data.RenderEntry, 
						pCurrentRenderNode->Data.RenderKey,
						pPreviousRenderNode ? (
						pPreviousRenderNode->pNext == pCurrentRenderNode ? pCurrentRenderNode->Data.uFlag : BuildRenderFlag(
						pCurrentRenderNode->Data.RenderEntry, 
						pCurrentRenderNode->Data.RenderKey,
						pPreviousRenderNode->Data.RenderEntry, 
						pPreviousRenderNode->Data.RenderKey,
						pCurrentRenderNode->Data.pRenderer != pPreviousRenderNode->Data.pRenderer) ) : ~0);

					pPreviousRenderNode = pCurrentRenderNode;
				}
			}

			pRenderList->pTail = pCurrentRenderNode;
		}
	}
	else
	{
		if(pFinalRenderNode)
		{
			while(pCurrentRenderNode)
			{
				if( pCurrentRenderNode->Data.pRenderer && (!pCurrentRenderNode->Data.pIsVisible || *pCurrentRenderNode->Data.pIsVisible) )
				{
					pCurrentRenderNode->Data.pRenderer->Render(
						pCurrentRenderNode->Data.RenderEntry, 
						pCurrentRenderNode->Data.RenderKey,
						pPreviousRenderNode ? ( 
						pPreviousRenderNode->pNext == pCurrentRenderNode ? pCurrentRenderNode->Data.uFlag : BuildRenderFlag(
						pCurrentRenderNode->Data.RenderEntry, 
						pCurrentRenderNode->Data.RenderKey,
						pPreviousRenderNode->Data.RenderEntry, 
						pPreviousRenderNode->Data.RenderKey,
						pCurrentRenderNode->Data.pRenderer != pPreviousRenderNode->Data.pRenderer) ) : ~0);

					pPreviousRenderNode = pCurrentRenderNode;
				}

				pCurrentRenderNode = pCurrentRenderNode->pNext;
			}
		}
		else
		{
			pCurrentRenderNode->Data.uFlag = ~0;

			if( pCurrentRenderNode->Data.pRenderer && (!pCurrentRenderNode->Data.pIsVisible || *pCurrentRenderNode->Data.pIsVisible) )
			{
				pCurrentRenderNode->Data.pRenderer->Render(pCurrentRenderNode->Data.RenderEntry, pCurrentRenderNode->Data.RenderKey, pCurrentRenderNode->Data.uFlag);

				pPreviousRenderNode = pCurrentRenderNode;
			}

			while(pCurrentRenderNode->pNext)
			{
				pCurrentRenderNode->pNext->Data.uFlag = BuildRenderFlag(
					pCurrentRenderNode->pNext->Data.RenderEntry, 
					pCurrentRenderNode->pNext->Data.RenderKey,
					pCurrentRenderNode->Data.RenderEntry, 
					pCurrentRenderNode->Data.RenderKey,
					pCurrentRenderNode->pNext->Data.pRenderer != pCurrentRenderNode->Data.pRenderer);

				pCurrentRenderNode = pCurrentRenderNode->pNext;

				if( pCurrentRenderNode->Data.pRenderer && (!pCurrentRenderNode->Data.pIsVisible || *pCurrentRenderNode->Data.pIsVisible) )
				{
					pCurrentRenderNode->Data.pRenderer->Render(
						pCurrentRenderNode->Data.RenderEntry, 
						pCurrentRenderNode->Data.RenderKey,
						pPreviousRenderNode ? ( 
						pPreviousRenderNode->pNext == pCurrentRenderNode ? pCurrentRenderNode->Data.uFlag : BuildRenderFlag(
						pCurrentRenderNode->Data.RenderEntry, 
						pCurrentRenderNode->Data.RenderKey,
						pPreviousRenderNode->Data.RenderEntry, 
						pPreviousRenderNode->Data.RenderKey,
						pCurrentRenderNode->Data.pRenderer != pPreviousRenderNode->Data.pRenderer) ) : ~0);

					pPreviousRenderNode = pCurrentRenderNode;
				}
			}

			pRenderList->pTail = pCurrentRenderNode;
		}
	}

	m_uCurrentRenderList = m_RenderLists.GetLength();
}

void CRenderQueue::Render(ARGBCOLOR BackgroudColor)
{
	static bool s_bIsRendering = false;

	if(s_bIsRendering)
	{
		ALCHEMY_DEBUG_WARNING("CRenderQueue::Render: Render queue is rendering.");

		return;
	}

	s_bIsRendering = true;

	if(m_bIsSort)
	{
		CVector<RENDERLIST>::CIterator RenderListIterator;

		RenderListIterator = m_RenderLists.GetIterator(0);
		while( CVector<RENDERLIST>::IsValuable(RenderListIterator) )
		{
			RenderListIterator->pTail = ALCHEMY_NULL;

			++ RenderListIterator;
		}

		LPSCENETREEMANAGER pSceneTreeManager;
		ISceneTreeManager::CSceneTreeObjectIterator* pSceneTreeObjectIterator;
		CVector<SCENETREEMANAGER>::CIterator SceneTreeManagerIterator =  m_SceneTreeManagers.GetIterator(0);
		while( CVector<SCENETREEMANAGER>::IsValuable(SceneTreeManagerIterator) )
		{
			pSceneTreeManager = &(*SceneTreeManagerIterator);
			if(pSceneTreeManager->pSceneTreeManager && pSceneTreeManager->pBoundingVolume)
			{
				pSceneTreeObjectIterator = pSceneTreeManager->pSceneTreeManager->SearchObject(*pSceneTreeManager->pBoundingVolume, pSceneTreeManager->pClipper, ALCHEMY_NULL);

				while(pSceneTreeObjectIterator)
				{
					static_cast<CSceneTreeObject&>( pSceneTreeObjectIterator->GetSceneTreeObject() ).PushRenderEntryNodeTo(m_RenderLists);

					pSceneTreeObjectIterator = pSceneTreeObjectIterator->GetNext();
				}
			}

			++ SceneTreeManagerIterator;
		}

		RenderListIterator = m_RenderLists.GetIterator(0);
		while( CVector<RENDERLIST>::IsValuable(RenderListIterator) )
		{
			RenderListIterator->pTail = ALCHEMY_NULL;

			++ RenderListIterator;
		}
	}

	SetRenderState(m_RenderSystem, IRenderState::CULL_MODE, IRenderState::CCW);

	RenderScene(BEFOREHAND);

	if(m_pRenderTarget)
	{
		LPRENDERLIST pFullScreenRenderList = m_RenderLists.Get(FULL_SCREEN);

		m_RenderTarget = (pFullScreenRenderList && pFullScreenRenderList->pHead) ? m_pRenderTarget->GetResourceHandle() : 0;
	}
	else
		m_RenderTarget = 0;

	SetRenderTarget(m_RenderTarget, 0, false);

	m_RenderSystem.Clear( BackgroudColor, ALCHEMY_FLAG(CRenderSystem::TARGET) | ALCHEMY_FLAG(CRenderSystem::ZBUFFER) | ALCHEMY_FLAG(CRenderSystem::STENCIL) );

	RenderScene(NORMAL);

	RenderScene(FINAL);

	if(m_pRenderTarget)
	{
		m_RenderTarget = m_pRenderTarget->GetResourceHandle();

		SetRenderTarget(0, 0, false);

		m_RenderSystem.Clear( BackgroudColor, ALCHEMY_FLAG(CRenderSystem::TARGET) | ALCHEMY_FLAG(CRenderSystem::ZBUFFER) | ALCHEMY_FLAG(CRenderSystem::STENCIL) );
	}

	RenderScene(FULL_SCREEN);

	s_bIsRendering = false;
}

void CRenderQueue::Destroy()
{
	m_RenderLists.Clear();

	if(m_pRenderTarget)
	{
		m_pRenderTarget->Release();

		m_pRenderTarget = ALCHEMY_NULL;
	}

	CMap<RENDERKEY, CGraphicsResource::HANDLE, RENDERKEYLESS, RENDERKEYEQUAL>::CIterator VertexDeclarationIterator = m_VertexDeclarationMap.GetBeginIterator();
	CVertexDeclaration* pVertexDeclaration;
	while( CMap<RENDERKEY, CGraphicsResource::HANDLE, RENDERKEYLESS, RENDERKEYEQUAL>::IsValuable(VertexDeclarationIterator) )
	{
		pVertexDeclaration = static_cast<CVertexDeclaration*>( m_RenderSystem.GetResource(VertexDeclarationIterator.GetValue(), CGraphicsResource::VERTEX_DECLARATION) );
		if(pVertexDeclaration)
			pVertexDeclaration->Release();

		++ VertexDeclarationIterator;
	}

	m_VertexDeclarationMap.Clear();

	CVector<CRenderEntry*>::CIterator RenderEntryIterator =  m_RenderEntris.GetIterator(0);
	CRenderEntry* pRenderEntry;
	while( CVector<CRenderEntry*>::IsValuable(RenderEntryIterator) )
	{
		pRenderEntry = *RenderEntryIterator;
		if(pRenderEntry)
			ALCHEMY_DEBUG_DELETE(pRenderEntry);

		++ RenderEntryIterator;
	}

	m_RenderEntris.Clear();

	m_SceneTreeManagers.Clear();
}