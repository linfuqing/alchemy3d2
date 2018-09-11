#pragma once

#include "../Core/AlchemyEventDispatcher.h"

namespace alchemy
{
	class CInput : 
		public EVENTDISPATCHER
	{
	public:
		ALCHEMY_BEGIN_EVENT_ENUM
			INPUT_EVENT
		ALCHEMY_END_EVENT_ENUM

		typedef enum
		{
			//Local
			CITY_CHANGE_TOWER, 
			TOWER_CHANGE_FLOOR, 

			//Network
			CITY_BUILD_TOWER, 
			TOWER_BUILD_FLOOR
		}INPUTTYPE;

		class CInputEvent : 
			public IEventDispatcher::CEvent
		{
		public:
			CInputEvent(INPUTTYPE InputType, FLOAT fValue, UINT uType, bool bIsBubbles = true);
			~CInputEvent();

			INPUTTYPE GetInputType()const;
			FLOAT GetValue()const;
		private:
			INPUTTYPE m_Type;
			FLOAT m_fValue;
		};

		CInput(void);
		~CInput(void);

		void DispatchEvent(INPUTTYPE InputType, FLOAT fValue);
	};

	ALCHEMY_INLINE CInput::INPUTTYPE CInput::CInputEvent::GetInputType()const
	{
		return m_Type;
	}

	ALCHEMY_INLINE FLOAT CInput::CInputEvent::GetValue()const
	{
		return m_fValue;
	}
}