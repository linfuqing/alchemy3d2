#include "AlchemyRenderer.h"

using namespace alchemy;

CRenderer::CRenderListener::CRenderListener(CRenderer& Parent) :
m_Parent(Parent)
{

}

CRenderer::CRenderListener::~CRenderListener(void)
{

}

void CRenderer::CRenderListener::Handler(const IEventDispatcher::CEvent& Event)
{
	m_Parent.ApplyForRender();
}

CRenderer::CRenderer(void) : 
m_pSceneManager(ALCHEMY_NULL),
m_Listener(*this),
m_bIsVisible(false)
{
}

CRenderer::~CRenderer(void)
{
}

void CRenderer::SetVisible(bool bValue)
{
	if(m_pSceneManager && m_bIsVisible != bValue)
	{
		UINT uEventType;
		if( ALCHEMY_GET_EVENT_TYPE(uEventType, RESET_SCENE, CSceneManager) )
		{
			if(bValue)
				m_pSceneManager->AddEventListener(uEventType, m_Listener);
			else
				m_pSceneManager->RemoveEventListener(uEventType, m_Listener);
		}

		m_pSceneManager->ResetScene();
	}

	m_bIsVisible = bValue;
}