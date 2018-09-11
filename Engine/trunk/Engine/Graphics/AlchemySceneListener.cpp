#include "AlchemySceneListener.h"
#include "AlchemySceneManager.h"

using namespace alchemy;

CSceneListener::CSceneListener():
CSingleton<CSceneListener>(*this)
{

}

CSceneListener::~CSceneListener()
{
	CMap<CSceneManager*, CVector<IRenderGroup*> >::CIterator iter = m_RenderGroupsMap.GetBeginIterator();

	UINT uEventType, i, uLen;
	CSceneManager* pSceneManger;
	
	while ( CMap<CSceneManager*, CVector<IRenderGroup*> >::IsValuable(iter) )
	{
		pSceneManger = iter.GetKey();

		if( ALCHEMY_GET_EVENT_TYPE(uEventType, RESET_SCENE, CSceneManager) )
			pSceneManger->RemoveEventListener(uEventType, *this);

		CVector<IRenderGroup*>& RenderGroups = iter.GetValue();

		uLen = RenderGroups.GetLength();
		for(i=0; i<uLen; ++i)
			RenderGroups[i]->Release();

		++iter;
	}

	m_RenderGroupsMap.Clear();
}

void CSceneListener::RegisterRenderGroup( CSceneManager& SceneManager, IRenderGroup& RenderGroup )
{
	CVector<IRenderGroup*>* pRenderGroups = m_RenderGroupsMap.Get(&SceneManager);
	if(pRenderGroups)
	{
		UINT uIndex;
		if( !pRenderGroups->GetIndexOf(uIndex, &RenderGroup) )
		{
			RenderGroup.AddRef();
			pRenderGroups->Add(&RenderGroup);
		}
	}
	else
	{
		static CVector<IRenderGroup*>	s_EmptyGroups;
		m_RenderGroupsMap.Insert(&SceneManager, s_EmptyGroups);

		pRenderGroups = m_RenderGroupsMap.Get(&SceneManager);
		RenderGroup.AddRef();
		pRenderGroups->Add(&RenderGroup);

		UINT uEventType;
		if( ALCHEMY_GET_EVENT_TYPE(uEventType, RESET_SCENE, CSceneManager) )
			SceneManager.AddEventListener(uEventType, *this);
	}
}

void CSceneListener::UnResiterRenderGroup( CSceneManager& SceneManager, IRenderGroup& RenderGroup )
{
	CVector<IRenderGroup*>* pRenderGroups = m_RenderGroupsMap.Get(&SceneManager);
	UINT uIndex;
	if( pRenderGroups && pRenderGroups->GetIndexOf(uIndex, &RenderGroup) )
		pRenderGroups->Remove(uIndex, 1);
}

void CSceneListener::Handler(const IEventDispatcher::CEvent& Event)
{
	CVector<IRenderGroup*>*	pRenderGroups = m_RenderGroupsMap.Get(  static_cast<CSceneManager*>( Event.GetTarget() ) );
	
	UINT uLen;
	if( pRenderGroups && ( uLen = pRenderGroups->GetLength() ) )
	{
		for(UINT i=0; i<uLen; ++i)
			;//(*pRenderGroups)[i]->SetDirty(true);
	}
}