#pragma once

#include "../Core/AlchemyString.h"
#include "AlchemyAnimationSet.h"

namespace alchemy
{
	class CAnimationController
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

		bool Create(UINT uMaxOutputCount, UINT uMaxAnimationSetCount);

		void Destroy();

		bool RegisterAnimationSet(CAnimationSet& AnimationSet, FLOAT fWeight = 1.0f, FLOAT fSpeed = 1.0f);
		bool UnregisterAnimationSet(CAnimationSet& AnimationSet);

		bool RegisterAnimationOutput(const CHAR* pcName, MATRIX4X4& Output);
		bool UnregisterAnimationOutput(const CHAR* pcName);

		void ResetTime();

		bool Update(FLOAT fElapseTime);

	private:
		void __BuildMap(PUINT puMap, const CAnimationSet::ANIMATION* pAnimation, UINT uAnimationCount);

		FLOAT	m_fTime;
		LPOUTPUT m_pOutput;

		LPANIMATIONSET m_pAnimationSet;

		UINT m_uOutputCount;
		UINT m_uAnimationSetCount;

		UINT m_uMaxOutputCount;
		UINT m_uMaxAnimationSetCount;

		bool m_bIsMapDirty;
	};
}