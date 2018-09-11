#pragma once

#include "../Math/AlchemyMatrix4x4.h"

#include "../Core/AlchemyInterface.h"
#include "../Core/AlchemySmartClass.h"

namespace alchemy
{
	class CAnimationSet :
		public CSmartClass<IInterface>
	{
	public:
		typedef struct
		{
			PCHAR pcName;

			LPMATRIX4X4 pTransformMatrices;

			PFLOAT pfTime;

			UINT uFrameCount;
		}ANIMATION, * LPANIMATION;

		CAnimationSet(void);
		~CAnimationSet(void);

		bool Create(const ANIMATION* pAnimations, UINT uAnimationCount);
		void Destroy();

		UINT GetFrameOrder(FLOAT fTime, UINT uAnimation);

		bool GetSRT(
			FLOAT fTime,
			UINT uAnimation,
			MATRIX4X4 &Transform);

		const ANIMATION* GetAnimations()const;
		UINT GetAnimationCount()const;
	private:
		LPANIMATION m_pAnimations;
		UINT m_uAnimationCount;
	};

	ALCHEMY_INLINE const CAnimationSet::ANIMATION* CAnimationSet::GetAnimations()const
	{
		return m_pAnimations;
	}

	ALCHEMY_INLINE UINT CAnimationSet::GetAnimationCount()const
	{
		return m_uAnimationCount;
	}
}