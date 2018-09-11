#pragma once

#ifdef _MSC_VER
#pragma warning(disable : 4355)
#endif

#include "AlchemyTimerQueueInterface.h"
#include "AlchemySmartClass.h"

namespace alchemy
{
	template<typename T>
	class CTimerQueue :
		public T
	{
		typedef struct 
		{
			FLOAT fTime;
			ITimerQueue::ITimer* pTimer;
		}TIMER, * LPTIMER;

		struct TIMERLESS
		{
			bool operator()(const TIMER& Source, const TIMER& Destination);
		};
	public:
		CTimerQueue(ITimerQueue* pTimerQueue = ALCHEMY_NULL) : m_TimerQueue( pTimerQueue ? *pTimerQueue : static_cast<ITimerQueue&>(*this) ), m_fTime(0.0f), m_uNextTimer(0), m_bIsTimerDirty(false) {}
		~CTimerQueue(void) {}

		ALCHEMY_INLINE void ResetTime()
		{
			m_fTime = 0.0f;
			m_uNextTimer = 0;
		}

		void SetTime(FLOAT fTime)
		{
			if(fTime)
			{
				m_fTime = fTime;

				if(!m_bIsTimerDirty)
				{
					LPTIMER pTimer;
					UINT uLength = m_Timers.GetLength();
					for(m_uNextTimer = 0; m_uNextTimer < uLength; ++ m_uNextTimer)
					{
						pTimer = m_Timers.Get(m_uNextTimer);
						if(pTimer)
						{
							if(pTimer->fTime >= m_fTime)
								break;
						}
					}
				}
			}
			else
				ResetTime();
		}

		bool AdvanceTime(FLOAT fTime, ITimerQueue::ITimer& Timer)
		{
			TIMER TimerTemp;
			TimerTemp.fTime  = fTime;
			TimerTemp.pTimer = &Timer;

			m_Timers.Add(TimerTemp);

			m_bIsTimerDirty = true;

			return true;
		}

		void Update(FLOAT fElapseTime)
		{
			LPTIMER pTimer;
			UINT uLength = m_Timers.GetLength();
			if(m_bIsTimerDirty)
			{
				m_Timers.Sort<TIMERLESS>();

				for(m_uNextTimer = 0; m_uNextTimer < uLength; ++ m_uNextTimer)
				{
					pTimer = m_Timers.Get(m_uNextTimer);
					if(pTimer)
					{
						if(pTimer->fTime >= m_fTime)
							break;
					}
				}

				m_bIsTimerDirty = false;
			}

			m_fTime += fElapseTime;

			for(; m_uNextTimer < uLength; ++ m_uNextTimer)
			{
				pTimer = m_Timers.Get(m_uNextTimer);
				if(pTimer)
				{
					if(pTimer->fTime <= m_fTime)
					{
						if(pTimer->pTimer)
							pTimer->pTimer->Handler(m_fTime, m_TimerQueue);
					}
					else
						break;
				}
			}
		}

	private:
		FLOAT m_fTime;
		CVector<TIMER> m_Timers;
		UINT m_uNextTimer;
		bool m_bIsTimerDirty;

		ITimerQueue& m_TimerQueue;
	};

	typedef CTimerQueue< CSmartClass<ITimerQueue> > TIMERQUEUE;

	template<typename T>
	ALCHEMY_INLINE bool CTimerQueue<T>::TIMERLESS::operator()(const TIMER& Source, const TIMER& Destination)
	{
		return Source.fTime < Destination.fTime;
	}
}