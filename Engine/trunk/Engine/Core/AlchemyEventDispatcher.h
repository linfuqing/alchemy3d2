#pragma once

#include "AlchemyList.h"
#include "AlchemyMap.h"
#include "AlchemyEventDispatcherInterface.h"
#include "AlchemySmartClass.h"

namespace alchemy
{
#ifdef _MSC_VER
#pragma warning(disable : 4355)
#endif

	template<typename T>
	class CEventDispatcher :
		public T
	{
	public:
		CEventDispatcher(IEventDispatcher* pTarget = ALCHEMY_NULL) :
		  m_EventDispatcher( pTarget ? *pTarget : static_cast<IEventDispatcher&>(*this) )
		{

		}

		~CEventDispatcher(void)
		{

		}

		void AddEventListener(UINT uType, IEventDispatcher::IListener& Handler)
		{
			CList<IEventDispatcher::IListener*>* pListeners = m_ListenerMap.Get(uType);
			if(pListeners)
				pListeners->Push(&Handler);
			else
			{
				CList<IEventDispatcher::IListener*> Listeners;
				Listeners.Push(&Handler);
				m_ListenerMap.Insert(uType, Listeners);
			}

			Handler.AddRef();
		}

		bool RemoveEventListener(UINT uType, IEventDispatcher::IListener& Handler)
		{
			CList<IEventDispatcher::IListener*>* pListeners = m_ListenerMap.Get(uType);
			if(pListeners)
			{
				if( pListeners->Remove< BASICEQUAL<IEventDispatcher::IListener*> >(&Handler) )
				{
					if( pListeners->IsEmpty() )
						m_ListenerMap.Remove(uType);

					Handler.Release();

					return true;
				}
			}

			return false;
		}

		bool HasEventListener(UINT uType, IEventDispatcher::IListener& Handler)
		{
			return m_ListenerMap.Get(uType) != ALCHEMY_NULL;
		}

		bool DispatchEvent(IEventDispatcher::CEvent& Event)
		{
			CList<IEventDispatcher::IListener*>* pListeners = m_ListenerMap.Get(Event.m_uType);
			if(pListeners)
			{
				Event._SetTarget(m_EventDispatcher);
				CList<IEventDispatcher::IListener*>::CIterator Iterator = pListeners->GetBeginIterator();
				while( CList<IEventDispatcher::IListener*>::IsValuable(Iterator) )
				{
					(*Iterator)->Handler(Event);

					++ Iterator;
				}

				return true;
			}

			return false;
		}

		void Destroy()
		{
			CList<IEventDispatcher::IListener*>* pListeners;
			CList<IEventDispatcher::IListener*>::CIterator ListIterator;

			CMap< UINT, CList<IEventDispatcher::IListener*> >::CIterator MapIterator = m_ListenerMap.GetBeginIterator();

			while( CMap< UINT, CList<IEventDispatcher::IListener*> >::IsValuable(MapIterator) )
			{
				pListeners = &MapIterator.GetValue();
				if(pListeners)
				{
					ListIterator = pListeners->GetBeginIterator();
					while( CList<IEventDispatcher::IListener*>::IsValuable(ListIterator) )
					{
						(*ListIterator)->Release();

						++ ListIterator;
					}
				}

				++ MapIterator;
			}
		}

	private:
		CMap< UINT, CList<IEventDispatcher::IListener*> > m_ListenerMap;
		IEventDispatcher& m_EventDispatcher;
	};

	typedef CEventDispatcher< CSmartClass<IEventDispatcher> > EVENTDISPATCHER;

	/*ALCHEMY_INLINE UINT CEventDispatcher::GetNewEventType()
	{
		static UINT s_uType = 0;

		return s_uType ++;
	}*/
}