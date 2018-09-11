#pragma once
/*
 *  AlchemyWindowInterface.h
 *  IOS
 *
 *  Created by lin fuqing on 12-5-1.
 *  Copyright 2012 AstepGame. All rights reserved.
 *
 */

#include "../Core/AlchemyEventDispatcherInterface.h"
#include "../Math/AlchemyFloat2.h"

namespace alchemy
{
	struct IWindow : 
		public IEventDispatcher
	{
		typedef enum
		{
			KEY_A, 
			KEY_B, 
			KEY_C, 
			KEY_D, 
			KEY_E, 
			KEY_F,
			KEY_G, 
			KEY_H, 
			KEY_I, 
			KEY_J, 
			KEY_K, 
			KEY_L, 
			KEY_M, 
			KEY_N, 
			KEY_O, 
			KEY_P, 
			KEY_Q, 
			KEY_R, 
			KEY_S, 
			KEY_T, 
			KEY_U, 
			KEY_V, 
			KEY_W, 
			KEY_X, 
			KEY_Y, 
			KEY_Z, 

			KEY_0, 
			KEY_1, 
			KEY_2, 
			KEY_3, 
			KEY_4, 
			KEY_5, 
			KEY_6, 
			KEY_7, 
			KEY_8, 
			KEY_9, 

			TOTAL_KEY_CODE
		}KEYCODE;

		ALCHEMY_BEGIN_EVENT_ENUM
			POINT_ON,
			POINT_MOVE, 
			POINT_OUT, 
			KEY_DOWN,
			KEY_UP
		ALCHEMY_END_EVENT_ENUM

		ALCHEMY_INTERFACE(IDisplayLink)
		{
			virtual void Run(FLOAT fTime, FLOAT fElapseTime) = 0;
		};

		class CPointEvent : 
			public CEvent
		{
		public:
			CPointEvent(const FLOAT2* pPoints, UINT uPointCount, UINT uType, bool bIsBubbles = true) : CEvent(uType, bIsBubbles), m_pPoints(pPoints), m_uPointCount(uPointCount) {}
			~CPointEvent() {}

			const FLOAT2* GetPoints()const;
			UINT GetPointCount()const;

		private:
			const FLOAT2* m_pPoints;
			UINT m_uPointCount;
		};

		class CKeyEvent : 
			public CEvent
		{
		public:
			CKeyEvent(KEYCODE KeyCode, UINT uType, bool bIsBubbles = true) : CEvent(uType, bIsBubbles), m_KeyCode(KeyCode) {}
			~CKeyEvent() {}

			KEYCODE GetKeyCode()const;

		private:
			KEYCODE m_KeyCode;
		};
		
		virtual bool AttachToScreen(bool bIsAsKeyWindow) = 0;
		
		virtual bool AddRunLoop(IDisplayLink& DisplayLink) = 0;
	};

	ALCHEMY_INLINE const FLOAT2* IWindow::CPointEvent::GetPoints()const
	{
		return m_pPoints;
	}

	ALCHEMY_INLINE UINT IWindow::CPointEvent::GetPointCount()const
	{
		return m_uPointCount;
	}

	ALCHEMY_INLINE IWindow::KEYCODE IWindow::CKeyEvent::GetKeyCode()const
	{
		return m_KeyCode;
	}
}
