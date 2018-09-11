/*
 *  CCameraEventListener.cpp
 *  IOS
 *
 *  Created by lin fuqing on 12-5-11.
 *  Copyright 2012 AstepGame. All rights reserved.
 *
 */

#include "AlchemyIOSCameraEventListener.h"

#include <math.h>

using namespace alchemy;

CIOSCameraEventListener::CTouchesBeganListener::CTouchesBeganListener(CIOSCameraEventListener& Parent) :
m_Parent(Parent)
{
}

CIOSCameraEventListener::CTouchesBeganListener::~CTouchesBeganListener()
{
}

void CIOSCameraEventListener::CTouchesBeganListener::Handler(const IEventDispatcher::CEvent& Event)
{
	UIEvent* pEvent = &static_cast<const CIOSWindow::CUIEvent&>(Event).GetEvent();
	NSSet* pTouches = [pEvent allTouches];
	NSArray* pTouchArray = [pTouches allObjects];
	
	UITouch* pTouch = [pTouchArray objectAtIndex:0];
	CGPoint Point = [pTouch locationInView:&( m_Parent.m_Window.GetView() )];
	if(pTouchArray.count > 1)
	{
		pTouch = [pTouchArray objectAtIndex:1];
		CGPoint Temp = [pTouch locationInView:&( m_Parent.m_Window.GetView() )];
		Temp.x -= Point.x;
		Temp.y -= Point.y;
		
		Temp.x *= Temp.x;
		Temp.y *= Temp.y;
		
		m_Parent.m_fEndDistance = sqrtf( static_cast<FLOAT>(Temp.x + Temp.y) );
	}
	else
	{
		m_Parent.m_fTouchBeginX = Point.x;
		m_Parent.m_fTouchBeginY = Point.y;
	}
	
	m_Parent.m_fBeginDistance = m_Parent.m_fEndDistance;
	
	m_Parent.m_fTouchEndX   = m_Parent.m_fTouchBeginX;
	m_Parent.m_fTouchEndY   = m_Parent.m_fTouchBeginY;
	
	m_Parent.m_uUpdateCount  = 0;
}

CIOSCameraEventListener::CTouchesMovedListener::CTouchesMovedListener(CIOSCameraEventListener& Parent) :
m_Parent(Parent)
{
}

CIOSCameraEventListener::CTouchesMovedListener::~CTouchesMovedListener()
{
}

void CIOSCameraEventListener::CTouchesMovedListener::Handler(const IEventDispatcher::CEvent& Event)
{
	UIEvent* pEvent = &static_cast<const CIOSWindow::CUIEvent&>(Event).GetEvent();
	NSSet* pTouches = [pEvent allTouches];
	NSArray* pTouchArray = [pTouches allObjects];
	UINT uCount = pTouchArray.count;
	if(m_Parent.m_uUpdateCount > uCount)
		return;
		
	UITouch* pTouch = [pTouchArray objectAtIndex:0];
	CGPoint Point = [pTouch locationInView:&( m_Parent.m_Window.GetView() )];
	if(uCount > 1)
	{
		pTouch = [pTouchArray objectAtIndex:1];
		CGPoint Temp = [pTouch locationInView:&( m_Parent.m_Window.GetView() )];
		Temp.x -= Point.x;
		Temp.y -= Point.y;
		
		Temp.x *= Temp.x;
		Temp.y *= Temp.y;
		
		m_Parent.m_fEndDistance = sqrtf( static_cast<FLOAT>(Temp.x + Temp.y) );
		
		if(m_Parent.m_uUpdateCount < uCount)
		{
			m_Parent.m_fBeginDistance = m_Parent.m_fEndDistance;
			
			m_Parent.m_uUpdateCount = uCount;
		}
	}
	else
	{
		m_Parent.m_fTouchEndX = Point.x;
		m_Parent.m_fTouchEndY = Point.y;
	}
}

CIOSCameraEventListener::CTouchesEndedListener::CTouchesEndedListener(CIOSCameraEventListener& Parent) :
m_Parent(Parent)
{
}

CIOSCameraEventListener::CTouchesEndedListener::~CTouchesEndedListener()
{
}

void CIOSCameraEventListener::CTouchesEndedListener::Handler(const IEventDispatcher::CEvent& Event)
{
}

CIOSCameraEventListener::CIOSCameraEventListener(CCamera& Camera, CIOSWindow& Window) :
m_Camera(Camera), 
m_Window(Window),
m_TouchesBeganListener(*this), 
m_TouchesMovedListener(*this),
m_TouchesEndedListener(*this), 
m_fBeginDistance(0.0f), 
m_fEndDistance(0.0f),
m_fTouchBeginX(0.0f), 
m_fTouchBeginY(0.0f), 
m_fTouchEndX(0.0f), 
m_fTouchEndY(0.0f), 
m_fMove(0.1f),
m_fMoveX(0.01f), 
m_fMoveY(0.01f), 
m_fInterpolation(0.05f),
m_fInterpolationX(0.05f), 
m_fInterpolationY(0.05f),
m_uUpdateCount(0)
{
}

CIOSCameraEventListener::~CIOSCameraEventListener()
{
}

void CIOSCameraEventListener::AddListeners()
{
	UINT uType;
	
	if( ALCHEMY_GET_EVENT_TYPE(uType, TOUCHES_BEGAN, CIOSWindow) )
		m_Window.AddEventListener(uType, m_TouchesBeganListener);
	
	if( ALCHEMY_GET_EVENT_TYPE(uType, TOUCHES_MOVED, CIOSWindow) )
		m_Window.AddEventListener(uType, m_TouchesMovedListener);
	
	if( ALCHEMY_GET_EVENT_TYPE(uType, TOUCHES_ENDED, CIOSWindow) )
		m_Window.AddEventListener(uType, m_TouchesEndedListener);
}

void CIOSCameraEventListener::Update(FLOAT fElapseTime)
{
	FLOAT x = m_fTouchEndX - m_fTouchBeginX, y = m_fTouchEndY - m_fTouchBeginY;
	
	const FLOAT3& Rotation = m_Camera.GetRotation();
	m_Camera.SetRotation(Rotation.x + y * fElapseTime * m_fMoveX, Rotation.y + x * fElapseTime * m_fMoveY, Rotation.z);
	FLOAT fDistance = m_fEndDistance - m_fBeginDistance;
	m_Camera.Forward(fDistance * fElapseTime * m_fMove);
	
	x *= fElapseTime * m_fInterpolationX;
	y *= fElapseTime * m_fInterpolationY;
	
	if(m_fTouchBeginX < m_fTouchEndX)
		m_fTouchBeginX = ALCHEMY_MININUM(m_fTouchBeginX + x, m_fTouchEndX);
	else
		m_fTouchBeginX = ALCHEMY_MAXINUM(m_fTouchBeginX + x, m_fTouchEndX);

	if(m_fTouchBeginY < m_fTouchEndY)
		m_fTouchBeginY = ALCHEMY_MININUM(m_fTouchBeginY + y, m_fTouchEndY);
	else
		m_fTouchBeginY = ALCHEMY_MAXINUM(m_fTouchBeginY + y, m_fTouchEndY);
	
	fDistance *= fElapseTime * m_fInterpolation;
	
	if(m_fBeginDistance < m_fEndDistance)
		m_fBeginDistance = ALCHEMY_MININUM(m_fBeginDistance + fDistance, m_fEndDistance);
	else
		m_fBeginDistance = ALCHEMY_MAXINUM(m_fBeginDistance + fDistance, m_fEndDistance);
}