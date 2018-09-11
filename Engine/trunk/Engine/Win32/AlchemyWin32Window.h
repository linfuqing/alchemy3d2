#pragma once

#include "../Platform/AlchemyWindowInterface.h"

#include "../Core/AlchemyEventDispatcher.h"

#include "../Core/AlchemyString.h"

#include <windows.h>

namespace alchemy
{
	class CWin32Window : 
		public CSmartClass< CEventDispatcher<IWindow> >
	{
	public:
		CWin32Window(
			FLOAT fViewportX ,
			FLOAT fViewportY, 
			FLOAT fViewportWidth, 
			FLOAT fViewportHeight);

		~CWin32Window(void);

		HWND GetWindowHandle()const;

		FLOAT GetViewportWidth()const;
		FLOAT GetViewportHeight()const;

		bool IsLoop();

		bool AttachToScreen(bool bIsAsKeyWindow);

		bool AddRunLoop(IDisplayLink& DisplayLink);

		void RunLoop(FLOAT fTime, FLOAT fElapseTime);

	private:
		HWND m_hWindow;

		const CHAR* m_pcName;

		FLOAT m_fViewportX;
		FLOAT m_fViewportY;
		FLOAT m_fViewportWidth;
		FLOAT m_fViewportHeight;

		CVector<IDisplayLink*> m_DisplayLinks;
	};

	ALCHEMY_INLINE HWND CWin32Window::GetWindowHandle()const
	{
		return m_hWindow;
	}

	ALCHEMY_INLINE FLOAT CWin32Window::GetViewportWidth()const
	{
		return m_fViewportWidth;
	}

	ALCHEMY_INLINE FLOAT CWin32Window::GetViewportHeight()const
	{
		return m_fViewportHeight;
	}

	ALCHEMY_INLINE bool CWin32Window::IsLoop()
	{
		return m_DisplayLinks.GetLength() != 0;
	}
}