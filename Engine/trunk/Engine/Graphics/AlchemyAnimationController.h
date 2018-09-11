#pragma once

#include "AlchemyAnimationSet.h"

//#include "AlchemyUpdaterInterface.h"

#include "../Core/AlchemyString.h"
#include "../Core/AlchemyTimerQueue.h"

namespace alchemy
{
	class CAnimationController :
		public TIMERQUEUE
	{
		typedef struct
		{
			CAnimationSet* pAnimationSet;
			PUINT puMap;
			FLOAT fWeight;
			FLOAT fSpeed;
			FLOAT fTime;
		}ANIMATIONSET, * LPANIMATIONSET;

		typedef struct
		{
			STRING Name;

			LPMATRIX4X4 pMatrix;
		}OUTPUT, * LPOUTPUT;
	public:
		CAnimationController(void);
		~CAnimationController(void);

		FLOAT GetPlayTime()const;

		FLOAT GetMaxPlayTime()const;

		CAnimationSet* GetAnimationSetByIndex(UINT uIndex);

		void SetIsReplay(bool bIsReplay);

		void SetPlayTime(FLOAT fPlayTime);

		//bool Create(UINT uMaxOutputCount, UINT uMaxAnimationSetCount, bool isReplay = true);

		bool RegisterAnimationSet(CAnimationSet& AnimationSet, FLOAT fWeight = 1.0f, FLOAT fSpeed = 1.0f);
		bool UnregisterAnimationSet(CAnimationSet& AnimationSet);

		bool RegisterAnimationOutput(const CHAR* pcName, MATRIX4X4& Output);
		bool UnregisterAnimationOutput(const CHAR* pcName);

		//void ResetTime();

		void Update(FLOAT fElapseTime);

		void Destroy();

	private:
		void __BuildMap(PUINT puMap, const CAnimationSet::ANIMATION* pAnimation, UINT uAnimationCount);

		FLOAT m_fTime;
		FLOAT m_fMaxPlayTime;
		//LPOUTPUT m_pOutput;

		CPool<OUTPUT> m_OutputPool;
		CPool<ANIMATIONSET> m_AnimationSetPool;

		bool m_bIsReplay;

		//LPANIMATIONSET m_pAnimationSet;

		/*UINT m_uOutputCount;
		UINT m_uAnimationSetCount;

		UINT m_uMaxOutputCount;
		UINT m_uMaxAnimationSetCount;*/

		bool m_bIsMapDirty;
	};

	ALCHEMY_INLINE FLOAT CAnimationController::GetMaxPlayTime() const
	{
		return m_fMaxPlayTime;
	}

	ALCHEMY_INLINE FLOAT CAnimationController::GetPlayTime() const
	{
		return m_fTime;
	}

	ALCHEMY_INLINE CAnimationSet* CAnimationController::GetAnimationSetByIndex(UINT uIndex)
	{
		LPANIMATIONSET pAnimationSet = m_AnimationSetPool.Get(uIndex);
		
		return pAnimationSet ? pAnimationSet->pAnimationSet : ALCHEMY_NULL;//m_pAnimationSet ? ( uIndex > m_uAnimationSetCount ? ALCHEMY_NULL : m_pAnimationSet[uIndex].pAnimationSet ) : ALCHEMY_NULL;
	}

	ALCHEMY_INLINE void CAnimationController::SetIsReplay(bool bIsReplay)
	{
		m_bIsReplay = bIsReplay;
	}
}