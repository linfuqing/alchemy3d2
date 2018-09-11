#pragma once

#include "../Graphics/AlchemyWindowInterface.h"

#include "../Core/AlchemyEventDispatcher.h"

namespace alchemy
{
	class CFlasccWindow :
		public CSmartClass< CEventDispatcher<IWindow> >
	{
	public:
		CFlasccWindow(FLOAT x, FLOAT y, FLOAT fWidth, FLOAT fHeight);
		~CFlasccWindow(void);

		FLOAT GetViewportX()const;
		FLOAT GetViewportY()const;
		FLOAT GetViewportWidth()const;
		FLOAT GetViewportHeight()const;

		bool AttachToScreen(bool bIsAsKeyWindow);

		bool AddRunLoop(IWindow::IDisplayLink& DisplayLink);

	private:
		FLOAT m_fViewportX;
		FLOAT m_fViewportY;
		FLOAT m_fViewportWidth;
		FLOAT m_fViewportHeight;
	};

	ALCHEMY_INLINE FLOAT CFlasccWindow::GetViewportX()const
	{
		return m_fViewportX;
	}

	ALCHEMY_INLINE FLOAT CFlasccWindow::GetViewportY()const
	{
		return m_fViewportY;
	}

	ALCHEMY_INLINE FLOAT CFlasccWindow::GetViewportWidth()const
	{
		return m_fViewportWidth;
	}

	ALCHEMY_INLINE FLOAT CFlasccWindow::GetViewportHeight()const
	{
		return m_fViewportHeight;
	}
}