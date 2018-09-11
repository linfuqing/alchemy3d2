#pragma once

#include "../Core/AlchemyEventDispatcher.h"
#include "../Math/AlchemyFloat2.h"

namespace alchemy
{
	class CPointOutput : 
		public EVENTDISPATCHER
	{
	public:
		ALCHEMY_BEGIN_EVENT_ENUM
			TRANSVERSE, 
			VERTICAL, 
			SCALE, 
			CLICK
		ALCHEMY_END_EVENT_ENUM

		class CPointEvent : 
			public IEventDispatcher::CEvent
		{
		public:
			CPointEvent(FLOAT fValue, UINT uType, bool bIsBubbles = true);
			~CPointEvent();

			FLOAT GetValue()const;

		private:
			FLOAT m_fValue;
		};

		class CPointMove : 
			public CSmartClass<IEventDispatcher::IListener>
		{
		public:
			CPointMove(CPointOutput& Parent);
			~CPointMove();

			void Handler(const IEventDispatcher::CEvent& Event);

		private:
			CPointOutput& m_Parent;
		};

		class CPointOn : 
			public CSmartClass<IEventDispatcher::IListener>
		{
		public:
			CPointOn(CPointOutput& Parent);
			~CPointOn();

			void Handler(const IEventDispatcher::CEvent& Event);

		private:
			CPointOutput& m_Parent;
		};

		class CPointOut : 
			public CSmartClass<IEventDispatcher::IListener>
		{
		public:
			CPointOut(CPointOutput& Parent);
			~CPointOut();

			void Handler(const IEventDispatcher::CEvent& Event);

		private:
			CPointOutput& m_Parent;
		};

		CPointOutput();
		~CPointOutput();

		void SetFlag(UINT uFlag);
		
		bool Run();

	private:
		FLOAT2 m_Source;
		FLOAT2 m_Destination;
		UINT m_uPointCount;
		bool m_bIsMove;

		bool m_bIsRun;

		CPointMove m_Move;
		CPointOn m_On;
		CPointOut m_Out;

		UINT m_uFlag;
	};

	ALCHEMY_INLINE FLOAT CPointOutput::CPointEvent::GetValue()const
	{
		return m_fValue;
	}

	ALCHEMY_INLINE void CPointOutput::SetFlag(UINT uFlag)
	{
		m_uFlag = uFlag;
	}
}