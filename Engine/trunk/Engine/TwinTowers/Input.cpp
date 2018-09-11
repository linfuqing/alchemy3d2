#include "Input.h"

using namespace alchemy;

CInput::CInputEvent::CInputEvent(CInput::INPUTTYPE InputType, FLOAT fValue, UINT uType, bool bIsBubbles) : 
m_Type(InputType), 
m_fValue(fValue), 
CEvent(uType, bIsBubbles)
{

}

CInput::CInputEvent::~CInputEvent()
{

}

CInput::CInput(void)
{
}

CInput::~CInput(void)
{
}

void CInput::DispatchEvent(INPUTTYPE InputType, FLOAT fValue)
{
	UINT uType;
	if( ALCHEMY_GET_EVENT_TYPE(uType, INPUT_EVENT, CInput) )
	{
		CInputEvent Event(InputType, fValue, uType);
		EVENTDISPATCHER::DispatchEvent(Event);
	}
}