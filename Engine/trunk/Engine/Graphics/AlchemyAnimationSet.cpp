#include "AlchemyAnimationSet.h"

#include "../Core/AlchemyDebug.h"
#include "../Core/AlchemyNull.h"

using namespace alchemy;

CAnimationSet::CAnimationSet(void)
:m_pAnimations(ALCHEMY_NULL),
m_uAnimationCount(0)
{
}

CAnimationSet::~CAnimationSet(void)
{
	LPANIMATION pAnimation;
	for (UINT i = 0; i < m_uAnimationCount; ++i)
	{
		pAnimation = &m_pAnimations[i];

		ALCHEMY_DEBUG_DELETE_ARRAY(pAnimation->pcName);
		ALCHEMY_DEBUG_DELETE_ARRAY(pAnimation->pfTime);
		ALCHEMY_DEBUG_DELETE_ARRAY(pAnimation->pTransformMatrices);
	}

	ALCHEMY_DEBUG_DELETE_ARRAY(m_pAnimations);

	m_uAnimationCount = 0;
	m_pAnimations = ALCHEMY_NULL;
}

bool CAnimationSet::Create(const ANIMATION* pAnimations, UINT uAnimationCount)
{
	Destroy();

	m_uAnimationCount = uAnimationCount;
	ALCHEMY_DEBUG_NEW(m_pAnimations, ANIMATION[uAnimationCount]);

	UINT i, j, uFrameCount;
	const ANIMATION* pSourceAnimation;
	LPANIMATION pDestinationAnimation;
	for (i = 0; i < uAnimationCount; ++ i)
	{
		pSourceAnimation = &pAnimations[i];
		pDestinationAnimation = &m_pAnimations[i];

		uFrameCount = pSourceAnimation->uFrameCount;

		pDestinationAnimation->uFrameCount = uFrameCount;

		for(j = 0; pSourceAnimation->pcName[j]; ++ j);

		ALCHEMY_DEBUG_NEW(pDestinationAnimation->pcName, CHAR[j + 1]);

		pDestinationAnimation->pcName[j] = 0;

		for(j = 0; pSourceAnimation->pcName[j]; ++ j)
			pDestinationAnimation->pcName[j] = pSourceAnimation->pcName[j];

		ALCHEMY_DEBUG_NEW(pDestinationAnimation->pfTime, FLOAT[uFrameCount]);
		ALCHEMY_DEBUG_NEW(pDestinationAnimation->pTransformMatrices, MATRIX4X4[uFrameCount]);

		for (j = 0; j< uFrameCount; ++ j)
		{
			pDestinationAnimation->pfTime[j] = pSourceAnimation->pfTime[j];

			Copy(pDestinationAnimation->pTransformMatrices[j], pSourceAnimation->pTransformMatrices[j]);
		}
	}

	return true;
}

void CAnimationSet::Destroy()
{
	LPANIMATION pAnimation;
	for (UINT i = 0; i < m_uAnimationCount; ++i)
	{
		pAnimation = &m_pAnimations[i];

		ALCHEMY_DEBUG_DELETE_ARRAY(pAnimation->pcName);
		ALCHEMY_DEBUG_DELETE_ARRAY(pAnimation->pfTime);
		ALCHEMY_DEBUG_DELETE_ARRAY(pAnimation->pTransformMatrices);
	}

	ALCHEMY_DEBUG_DELETE_ARRAY(m_pAnimations);

	m_uAnimationCount = 0;
	m_pAnimations = ALCHEMY_NULL;
}

UINT CAnimationSet::GetFrameOrder(FLOAT fTime, UINT uAnimation)
{
	if(!m_pAnimations)
	{
		ALCHEMY_DEBUG_WARNING("CAnimationSet::GetFrameIndex: Animation set has not been created.");

		return 0;
	}

	if(uAnimation >= m_uAnimationCount)
	{
		ALCHEMY_DEBUG_WARNING("CAnimationSet::GetFrameIndex: Animation index out of range.");

		return 0;
	}

	LPANIMATION pAnimation = &m_pAnimations[uAnimation];

	UINT uCurrentFrameIndex = pAnimation->uFrameCount - 1, uLength, uOldLength;

	if(fTime < pAnimation->pfTime[0])
		uCurrentFrameIndex = 0;
	else if(fTime < pAnimation->pfTime[uCurrentFrameIndex])
	{
		uLength = uOldLength = pAnimation->uFrameCount - 1;

		while(uLength > 1)
		{
			uOldLength = uLength;
			uLength  >>= 1;

			if (pAnimation->pfTime[uCurrentFrameIndex - uLength] > fTime)
			{
				uCurrentFrameIndex -= uLength;
				uLength             = uOldLength - uLength;
			}
		}
	}

	return uCurrentFrameIndex + 1;
}

bool CAnimationSet::GetSRT(
						   FLOAT fTime,
						   UINT uAnimation,
						   MATRIX4X4 &Transform)
{
	UINT uOrder = GetFrameOrder(fTime, uAnimation);

	if(uOrder)
		Transform = m_pAnimations[uAnimation].pTransformMatrices[uOrder - 1];

	return true;	
}