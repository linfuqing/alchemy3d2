#pragma once
//
//  AlchemyIOSWindow.h
//  IOS
//
//  Created by liulin on 1/7/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "../Core/AlchemyEventDispatcher.h"
#include "../Core/AlchemySmartClass.h"

#include "../Graphics/AlchemyWindowInterface.h"

#import <UIKit/UIKit.h>

//@interface IOSView : UIView
//@end

@interface IOSEventDispatcher : UIViewController
{
	alchemy::CEventDispatcher* eventDispatcher;
	alchemy::CVector<alchemy::IWindow::IDisplayLink*>* displayLinks;
}

@end

namespace alchemy
{
	class CIOSWindow :
		public CEventDispatcher, public IWindow
	{
	public:
		ALCHEMY_BEGIN_EVENT_ENUM
		TOUCHES_BEGAN, 
		TOUCHES_MOVED, 
		TOUCHES_ENDED, 
		TOUCHES_CANCELLED, 
		
		MOTION_BEGAN,
		MOTION_ENDED,
		MOTION_CANCELLED
		ALCHEMY_END_EVENT_ENUM
		
		class CUIEvent :
		public IEventDispatcher::CEvent
		{
		public:
			CUIEvent(UINT uType, UIEvent& Event, bool bIsBubbles = true);
			~CUIEvent();
			
			const UIEvent& GetEvent()const;
			
		private:
			UIEvent& m_UIEvent;
		};
		
		CIOSWindow();
		~CIOSWindow();
		
		bool Create(const CGRect& Rect);
		
		bool AttachToScreen(bool bIsAsKeyWindow);
		
		bool AddRunLoop(IWindow::IDisplayLink& DisplayLink);
		
		UIView& GetView()const;
		
		ALCHEMY_IMPLEMENT_INTERFACE
		
	protected:
		virtual UIView* _CreateView(const CGRect& Rect);
		
	private:
		UIView* m_pView;
		IOSEventDispatcher* m_pEventDispatcher;
		CVector<IWindow::IDisplayLink*> m_DisplayLinks;
	};
	
	ALCHEMY_INLINE const UIEvent& CIOSWindow::CUIEvent::GetEvent()const
	{
		return m_UIEvent;
	}
	
	ALCHEMY_INLINE UIView& CIOSWindow::GetView()const
	{
		return *m_pView;
	}
}
