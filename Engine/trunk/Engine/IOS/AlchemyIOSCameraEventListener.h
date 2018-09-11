#pragma once
/*
 *  CCameraEventListener.h
 *  IOS
 *
 *  Created by lin fuqing on 12-5-11.
 *  Copyright 2012 AstepGame. All rights reserved.
 *
 */
#include "AlchemyIOSWindow.h"

#include "../Graphics/AlchemyCamera.h"

namespace alchemy
{
	class CIOSCameraEventListener
	{
	public:
		class CTouchesBeganListener :
		public CSmartClass<IEventDispatcher::IListener>
		{
		public:
			CTouchesBeganListener(CIOSCameraEventListener& Parent);
			~CTouchesBeganListener();
			
			void Handler(const IEventDispatcher::CEvent& Event);
			
		private:
			CIOSCameraEventListener& m_Parent;
		};
		
		class CTouchesMovedListener :
		public CSmartClass<IEventDispatcher::IListener>
		{
		public:
			CTouchesMovedListener(CIOSCameraEventListener& Parent);
			~CTouchesMovedListener();
			
			void Handler(const IEventDispatcher::CEvent& Event);
			
		private:
			CIOSCameraEventListener& m_Parent;
		};
		
		class CTouchesEndedListener :
		public CSmartClass<IEventDispatcher::IListener>
		{
		public:
			CTouchesEndedListener(CIOSCameraEventListener& Parent);
			~CTouchesEndedListener();
			
			void Handler(const IEventDispatcher::CEvent& Event);
			
		private:
			CIOSCameraEventListener& m_Parent;
		};
		
		CIOSCameraEventListener(CCamera& Camera, CIOSWindow& Window);
		~CIOSCameraEventListener();
		
		void AddListeners();
		
		void Update(FLOAT fElapseTime);
		
	private:
		CCamera& m_Camera;
		CIOSWindow& m_Window;
		
		CTouchesBeganListener m_TouchesBeganListener;
		CTouchesMovedListener m_TouchesMovedListener;
		CTouchesEndedListener m_TouchesEndedListener;
		
		FLOAT m_fBeginDistance;
		FLOAT m_fEndDistance;
		
		FLOAT m_fTouchBeginX;
		FLOAT m_fTouchBeginY;
		
		FLOAT m_fTouchEndX;
		FLOAT m_fTouchEndY;
		
		FLOAT m_fMove;
		FLOAT m_fMoveX;
		FLOAT m_fMoveY;
		
		FLOAT m_fInterpolation;
		FLOAT m_fInterpolationX;
		FLOAT m_fInterpolationY;
		
		UINT m_uUpdateCount;
	};
}