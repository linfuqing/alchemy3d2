#include "AlchemyKeyListener.h"

using namespace alchemy;

CKeyListener::CKeyDownListener::CKeyDownListener(CKeyListener& Parent) :
m_Parent(Parent)
{

}

CKeyListener::CKeyDownListener::~CKeyDownListener()
{

}

void CKeyListener::CKeyDownListener::Handler(const IEventDispatcher::CEvent& Event)
{
	IWindow::KEYCODE KeyCode = static_cast<const IWindow::CKeyEvent&>(Event).GetKeyCode();
	if(KeyCode < IWindow::TOTAL_KEY_CODE)
	{
		m_Parent.m_bIsKeyDown[KeyCode] = true;
	}
}

CKeyListener::CKeyUpListener::CKeyUpListener(CKeyListener& Parent) :
m_Parent(Parent)
{

}

CKeyListener::CKeyUpListener::~CKeyUpListener()
{

}

void CKeyListener::CKeyUpListener::Handler(const IEventDispatcher::CEvent& Event)
{
	IWindow::KEYCODE KeyCode = static_cast<const IWindow::CKeyEvent&>(Event).GetKeyCode();
	if(KeyCode < IWindow::TOTAL_KEY_CODE)
	{
		m_Parent.m_bIsKeyDown[KeyCode] = false;
	}
}

CKeyListener::CKeyListener(void) : 
m_KeyDownListener(*this), 
m_KeyUpListener(*this)
{
	for(UINT i = 0; i < IWindow::TOTAL_KEY_CODE; ++ i)
		m_bIsKeyDown[i] = false;
}

CKeyListener::~CKeyListener(void)
{
}

void CKeyListener::AddEventListenerTo(IEventDispatcher& Window)
{
	UINT uType;

	if( ALCHEMY_GET_EVENT_TYPE(uType, KEY_DOWN, IWindow) )
		Window.AddEventListener(uType, m_KeyDownListener);

	if( ALCHEMY_GET_EVENT_TYPE(uType, KEY_UP, IWindow) )
		Window.AddEventListener(uType, m_KeyUpListener);
}

void CKeyListener::RemoveEventListenerFrom(IEventDispatcher& Window)
{
	UINT uType;

	if( ALCHEMY_GET_EVENT_TYPE(uType, KEY_DOWN, IWindow) )
		Window.RemoveEventListener(uType, m_KeyDownListener);

	if( ALCHEMY_GET_EVENT_TYPE(uType, KEY_UP, IWindow) )
		Window.RemoveEventListener(uType, m_KeyUpListener);
}