#pragma once

#include "AlchemyWindowInterface.h"

#include "../Core/AlchemySmartClass.h"

namespace alchemy
{
	class CKeyListener
	{
		class CKeyDownListener : 
			public CSmartClass<IEventDispatcher::IListener>
		{
		public:
			CKeyDownListener(CKeyListener& Parent);
			~CKeyDownListener();

			void Handler(const IEventDispatcher::CEvent& Event);

		private:
			CKeyListener& m_Parent;
		};

		class CKeyUpListener : 
			public CSmartClass<IEventDispatcher::IListener>
		{
		public:
			CKeyUpListener(CKeyListener& Parent);
			~CKeyUpListener();

			void Handler(const IEventDispatcher::CEvent& Event);

		private:
			CKeyListener& m_Parent;
		};
	public:
		CKeyListener(void);
		~CKeyListener(void);

		bool CheckKeyDown(IWindow::KEYCODE KeyCode)const;

		void AddEventListenerTo(IEventDispatcher& Window);
		void RemoveEventListenerFrom(IEventDispatcher& Window);

	private:
		CKeyDownListener m_KeyDownListener;
		CKeyUpListener m_KeyUpListener;

		bool m_bIsKeyDown[IWindow::TOTAL_KEY_CODE];
	};

	ALCHEMY_INLINE bool CKeyListener::CheckKeyDown(IWindow::KEYCODE KeyCode)const
	{
		return KeyCode < IWindow::TOTAL_KEY_CODE ? m_bIsKeyDown[KeyCode] : false;
	}
}