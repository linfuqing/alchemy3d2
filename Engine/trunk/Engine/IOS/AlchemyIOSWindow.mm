//
//  AlchemyIOSWindow.m
//  IOS
//
//  Created by liulin on 1/7/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//


#import "AlchemyIOSWindow.h"
#import <QuartzCore/QuartzCore.h>

#include "../Core/AlchemyDebug.h"
//#include <OpenGLES/ES2/gl.h>

//@interface AlchemyIOSWindow()
//
//+ (Class)layerClass;
//- (id)initWithFrame:(CGRect)frame;
//
//@end

using namespace alchemy;

@interface IOSView : UIView
@end

@implementation IOSView

+ (Class)layerClass {
    return [CAEAGLLayer class];
}

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code
    }
    return self;
}

@end

@implementation IOSEventDispatcher

- (void)initWithWindow:(CEventDispatcher*)iosEventDispatcher withDisplayLinks:(CVector<IWindow::IDisplayLink*>*)windowDisplayLinks
{
	eventDispatcher = iosEventDispatcher;
	displayLinks    = windowDisplayLinks;
	
	if( displayLinks && displayLinks->GetLength() )
	{
		CADisplayLink* link = [CADisplayLink displayLinkWithTarget:self selector:@selector(render:)];
		[link addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode]; 
	}
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	if(eventDispatcher)
	{
		UINT uType;
		if( ALCHEMY_GET_EVENT_TYPE(uType, TOUCHES_BEGAN, CIOSWindow) )
		{
			CIOSWindow::CUIEvent Event(uType, *event);
			eventDispatcher->DispatchEvent(Event);
		}
	}
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
	if(eventDispatcher)
	{
		UINT uType;
		if( ALCHEMY_GET_EVENT_TYPE(uType, TOUCHES_MOVED, CIOSWindow) )
		{
			CIOSWindow::CUIEvent Event(uType, *event);
			eventDispatcher->DispatchEvent(Event);
		}
	}
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	if(eventDispatcher)
	{
		UINT uType;
		if( ALCHEMY_GET_EVENT_TYPE(uType, TOUCHES_ENDED, CIOSWindow) )
		{
			CIOSWindow::CUIEvent Event(uType, *event);
			eventDispatcher->DispatchEvent(Event);
		}
	}
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
	if(eventDispatcher)
	{
		UINT uType;
		if( ALCHEMY_GET_EVENT_TYPE(uType, TOUCHES_CANCELLED, CIOSWindow) )
		{
			CIOSWindow::CUIEvent Event(uType, *event);
			eventDispatcher->DispatchEvent(Event);
		}
	}
}

- (void)motionBegan:(UIEventSubtype)motion withEvent:(UIEvent *)event
{
	if(eventDispatcher)
	{
		UINT uType;
		if( ALCHEMY_GET_EVENT_TYPE(uType, MOTION_BEGAN, CIOSWindow) )
		{
			CIOSWindow::CUIEvent Event(uType, *event);
			eventDispatcher->DispatchEvent(Event);
		}
	}
}

- (void)motionEnded:(UIEventSubtype)motion withEvent:(UIEvent *)event
{
	if(eventDispatcher)
	{
		UINT uType;
		if( ALCHEMY_GET_EVENT_TYPE(uType, MOTION_ENDED, CIOSWindow) )
		{
			CIOSWindow::CUIEvent Event(uType, *event);
			eventDispatcher->DispatchEvent(Event);
		}
	}
}

- (void)motionCancelled:(UIEventSubtype)motion withEvent:(UIEvent *)event
{
	if(eventDispatcher)
	{
		UINT uType;
		if( ALCHEMY_GET_EVENT_TYPE(uType, MOTION_CANCELLED, CIOSWindow) )
		{
			CIOSWindow::CUIEvent Event(uType, *event);
			eventDispatcher->DispatchEvent(Event);
		}
	}
}

- (void)render:(CADisplayLink*)displayLink
{
	if(displayLinks)
	{
		static CFTimeInterval s_Time = 0;
		CFTimeInterval ElapseTime = displayLink.timestamp - s_Time;
		
		CVector<IWindow::IDisplayLink*>::CIterator Iteraotr = displayLinks->GetIterator(0);
		
		while( CVector<IWindow::IDisplayLink*>::IsValuable(Iteraotr) )
		{
			(*Iteraotr)->Run( static_cast<FLOAT>(s_Time), static_cast<FLOAT>(ElapseTime) );
			
			++ Iteraotr;
		}
	}
}

//- (void)remoteControlReceivedWithEvent:(UIEvent *)event
//{
//}

/*
// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect
{
    // Drawing code
}
*/

@end

CIOSWindow::CUIEvent::CUIEvent(UINT uType, UIEvent& Event, bool bIsBubbles) : 
CEvent(uType, bIsBubbles),
m_UIEvent(Event)
{
}

CIOSWindow::CUIEvent::~CUIEvent()
{
}

//using namespace alchemy;

CIOSWindow::CIOSWindow() :
m_pView(nil), 
m_pEventDispatcher(nil)
{
}

CIOSWindow::~CIOSWindow()
{
	if(m_pView)
		[m_pView release];
		
	if(m_pEventDispatcher)
		[m_pEventDispatcher release];
}

bool CIOSWindow::Create(const CGRect& Rect)
{
	if(!m_pView)
	{
		m_pView = _CreateView(Rect);
		if(!m_pView)
		{
			ALCHEMY_DEBUG_WARNING("CIOSWindow::Create: View has been init error.");
			
			return false;
		}
		
		if(!m_pEventDispatcher)
		{
			m_pEventDispatcher = [[IOSEventDispatcher alloc] initWithNibName:nil bundle:nil];
			
			if(!m_pEventDispatcher)
			{
				ALCHEMY_DEBUG_WARNING("CIOSWindow::Create: View controller has been init error.");
				
				[m_pView release];
				
				m_pView = nil;
				
				return false;
			}
		}
		
		[m_pEventDispatcher initWithWindow:this withDisplayLinks:&m_DisplayLinks];
	
		m_pEventDispatcher.view = m_pView;
	}
	
	return true;
}

bool CIOSWindow::AttachToScreen(bool bIsAsKeyWindow)
{
	if(!m_pView)
	{
		ALCHEMY_DEBUG_WARNING("CIOSWindow::AttachToScreen: IOS window has not been created.");
			
		return false;
	}
	
	UIApplication* pApplication = [UIApplication sharedApplication];
	
	if(pApplication)
	{
		UIWindow* pWindow = [pApplication keyWindow];
		if(pWindow)
		{
			[pWindow addSubview:m_pView];
			if(bIsAsKeyWindow)
				[pWindow bringSubviewToFront:m_pView];
		}
		else
		{
			//...
		}
		
		return true;
	}
	
	ALCHEMY_DEBUG_WARNING("CIOSWindow::AttachToScreen: Application has not been set.");
	
	return false;
}

bool CIOSWindow::AddRunLoop(IWindow::IDisplayLink& DisplayLink)
{
	if(!m_DisplayLinks.GetLength() && m_pEventDispatcher)
	{
		CADisplayLink* displayLink = [CADisplayLink displayLinkWithTarget:m_pEventDispatcher selector:@selector(render:)];
		[displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode]; 
	}
	
	m_DisplayLinks.Add(&DisplayLink);
	
	return true;
}

UIView* CIOSWindow::_CreateView(const CGRect& Rect)
{
	UIView* pView = [[IOSView alloc] initWithFrame:Rect];
	
	return pView;
}
