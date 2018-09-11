#pragma once

#include "AlchemyInterface.h"

namespace alchemy
{
#define ALCHEMY_BEGIN_EVENT_ENUM \
public: \
	typedef enum \
	{

#define ALCHEMY_END_EVENT_ENUM \
	, \
	TOTAL_EVENT \
	}EVENT; \
	\
	static bool GetEventType(UINT& uOutput, EVENT Event) \
	{ \
		static UINT s_uEventType[TOTAL_EVENT]; \
		static bool s_bIsInitEventType = false; \
		if(!s_bIsInitEventType) \
		{ \
			for(UINT i = 0; i < TOTAL_EVENT; ++ i) \
				s_uEventType[i] = GetNewEventType(); \
			\
			s_bIsInitEventType = true; \
		} \
		\
		if(Event >= 0 && Event < TOTAL_EVENT) \
		{ \
			uOutput = s_uEventType[Event]; \
			\
			return true; \
		} \
		\
		return false; \
	}

#define ALCHEMY_GET_EVENT_TYPE(Output, EventEnum, Type) Type::GetEventType(Output, Type::EventEnum)

	ALCHEMY_INLINE static UINT GetNewEventType()
	{
		static UINT s_uType = 0;

		return s_uType ++;
	}

	template<typename T>
	class CEventDispatcher;

	ALCHEMY_INTERFACE(IEventDispatcher)
	{
		class CEvent
		{
			template <typename T> 
			friend class CEventDispatcher;
		public:
			CEvent(UINT uType, bool bIsBubbles = true) : m_uType(uType), m_bIsBubbles(bIsBubbles), m_pTarget(ALCHEMY_NULL) {}
			virtual ~CEvent() {}

			UINT GetType()const;
			bool IsBubbles()const;
			IEventDispatcher* GetTarget()const;

		protected:
			virtual void _SetTarget(IEventDispatcher& EventDispatcher);

		private:
			UINT m_uType;
			bool m_bIsBubbles;
			IEventDispatcher* m_pTarget;
		};

		ALCHEMY_INTERFACE(IListener)
		{
			virtual void Handler(const CEvent& Event) = 0;
		};

		virtual void AddEventListener(UINT uType, IListener& Handler) = 0;
		virtual bool RemoveEventListener(UINT uType, IListener& Handler) = 0;
		virtual bool HasEventListener(UINT uType, IListener& Handler) = 0;
		virtual bool DispatchEvent(CEvent& Event) = 0;
	};

	ALCHEMY_INLINE UINT IEventDispatcher::CEvent::GetType()const
	{
		return m_uType;
	}

	ALCHEMY_INLINE bool IEventDispatcher::CEvent::IsBubbles()const
	{
		return m_bIsBubbles;
	}

	ALCHEMY_INLINE IEventDispatcher* IEventDispatcher::CEvent::GetTarget()const
	{
		return m_pTarget;
	}

	ALCHEMY_INLINE void IEventDispatcher::CEvent::_SetTarget(IEventDispatcher& EventDispatcher)
	{
		m_pTarget = &EventDispatcher;
	}
}