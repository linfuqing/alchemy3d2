#include "AlchemyRenderQueue.h"
#include "AlchemySceneNode.h"
#include "AlchemyEffect.h"
#include "../Core/AlchemyDebug.h"
#include "../Core/AlchemyUtils.h"
#include "../Core/AlchemyBitFlag.h"
#include "AlchemyRenderSystem.h"

using namespace alchemy;

CRenderQueue::CRenderQueue(alchemy::UINT uMaxRenderEntries, CRenderSystem& RenderSystem) :
m_uMaxRenderEntries(uMaxRenderEntries),
m_RenderSystem(RenderSystem),
m_uActiveEntries(0)
{
	ALCHEMY_DEBUG_NEW(m_pEntryPool, RENDERENTRY[uMaxRenderEntries]);
	ALCHEMY_DEBUG_NEW(m_ppEntryList, LPRENDERENTRY[uMaxRenderEntries]);

	ALCHEMY_DEBUG_NEW(m_ppRenderers, CCallBackToRenderInterface*[uMaxRenderEntries]);
}

CRenderQueue::~CRenderQueue(void)
{
	Destroy();
}

UINT CRenderQueue::BuildRenderFlag(const CRenderQueue::RENDERENTRY& Current, const CRenderQueue::RENDERENTRY& Previous, bool bIsParentDirty)
{
	UINT uFlag = 0;

	if(Previous.uRenderTarget != Current.uRenderTarget)
		ALCHEMY_SET_BIT(uFlag, RENDER_TARGET);

	if(Previous.uEffect != Current.uEffect)
	{
		ALCHEMY_SET_BIT(uFlag, EFFECT);
		ALCHEMY_SET_BIT(uFlag, EFFECT_PASS);
	}
	else if (Previous.uRenderPass != Current.uRenderPass)
		ALCHEMY_SET_BIT(uFlag, EFFECT_PASS);

	if(Previous.uVertexDeclaration != Current.uVertexDeclaration)
		ALCHEMY_SET_BIT(uFlag, VERTEX_DECLARATION);

	if(Previous.uModel != Current.uModel)
	{
		ALCHEMY_SET_BIT(uFlag, MODEL);
		ALCHEMY_SET_BIT(uFlag, MODEL_PARAMETER);
	}
	else if (Previous.uModelParameter != Current.uModelParameter)
		ALCHEMY_SET_BIT(uFlag, MODEL_PARAMETER);	

	if(bIsParentDirty)
		ALCHEMY_SET_BIT(uFlag, PARENT);

	return uFlag;
}

void CRenderQueue::Destroy()
{
	ALCHEMY_DEBUG_DELETE_ARRAY(m_pEntryPool);
	ALCHEMY_DEBUG_DELETE_ARRAY(m_ppEntryList);

	ALCHEMY_DEBUG_DELETE_ARRAY(m_ppRenderers);
}

CRenderQueue::RENDERENTRY& CRenderQueue::LockRenderEntry()
{
	LPRENDERENTRY pEntry = &m_pEntryPool[m_uActiveEntries];

	pEntry->uSortValueA = 0;
	pEntry->uSortValueB = 0;
	pEntry->uSortValueC = 0;

	return *pEntry;
}

void CRenderQueue::UnlockRenderEntry(CRenderQueue::RENDERENTRY& Entry, CCallBackToRenderInterface& Renderderer)
{
	if (m_uActiveEntries + 1 >= m_uMaxRenderEntries)
		Render();

	m_ppEntryList[m_uActiveEntries] = &Entry;

	m_ppRenderers[m_uActiveEntries] = &Renderderer;

	++ m_uActiveEntries;
}

void CRenderQueue::Render(const RENDERENTRY* pPreviousEntry, bool bIsParentDirty)
{
	if(m_uActiveEntries)
	{	
		Sort<LPRENDERENTRY, LPRENDERENTRYLESS>(m_ppEntryList, m_ppEntryList, m_uActiveEntries);

		const RENDERENTRY* pCurrentEntry = m_ppEntryList[0];

		CCallBackToRenderInterface* pRenderer = m_ppRenderers[0];

		if(pRenderer)
			pRenderer->Render( *pCurrentEntry, pPreviousEntry ? BuildRenderFlag(*pCurrentEntry, *pPreviousEntry, bIsParentDirty) : ( ~( bIsParentDirty ? 0 : ALCHEMY_FLAG(PARENT) ) ) );

		for (UINT i = 1; i < m_uActiveEntries; i ++)
		{
			pRenderer = m_ppRenderers[i];

			if(pRenderer)
			{
				pCurrentEntry = m_ppEntryList[i];
				pRenderer->Render( *pCurrentEntry, BuildRenderFlag(*pCurrentEntry, *m_ppEntryList[i - 1], pRenderer != m_ppRenderers[i - 1]) );
			}
		}
	}
	
	m_uActiveEntries = 0;
}
