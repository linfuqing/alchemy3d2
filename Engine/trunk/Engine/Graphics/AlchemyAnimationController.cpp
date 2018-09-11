#include "AlchemyAnimationController.h"

using namespace alchemy;

CAnimationController::CAnimationController(void) :
/*m_pOutput(ALCHEMY_NULL),
m_pAnimationSet(ALCHEMY_NULL),
m_uOutputCount(0),
m_uMaxOutputCount(0),
m_uAnimationSetCount(0),
m_uMaxAnimationSetCount(0),*/
m_fTime(0.0f),
m_fMaxPlayTime(0.0f), 
m_bIsReplay(true),
m_bIsMapDirty(false)
{

}

CAnimationController::~CAnimationController(void)
{
	/*UINT i;
	LPANIMATIONSET pAnimationSet;
	for(i=0; i <m_uMaxAnimationSetCount; ++i)
	{
		pAnimationSet = &m_pAnimationSet[i];
		if(pAnimationSet->puMap)
			ALCHEMY_DEBUG_DELETE_ARRAY(pAnimationSet->puMap);
	}

	ALCHEMY_DEBUG_DELETE_ARRAY(m_pAnimationSet);
	ALCHEMY_DEBUG_DELETE_ARRAY(m_pOutput);*/

	CPool<ANIMATIONSET>::CIterator Iterator = m_AnimationSetPool.GetIterator(0);
	while( CPool<ANIMATIONSET>::IsValuable(Iterator) )
	{
		ALCHEMY_DEBUG_DELETE_ARRAY(Iterator->puMap);

		++ Iterator;
	}
}

/*bool CAnimationController::Create(UINT uMaxOutputCount, UINT uMaxAnimationSetCount, bool isReplay)
{
	Destroy();

	m_uMaxOutputCount = uMaxOutputCount;
	m_uMaxAnimationSetCount = uMaxAnimationSetCount;

	ALCHEMY_DEBUG_NEW(m_pAnimationSet, ANIMATIONSET[uMaxAnimationSetCount]);

	UINT i;
	for (i=0; i<m_uMaxAnimationSetCount; ++i)
	{
		m_pAnimationSet[i].pAnimationSet = ALCHEMY_NULL;
		m_pAnimationSet[i].puMap = ALCHEMY_NULL;
	}

	ALCHEMY_DEBUG_NEW(m_pOutput, OUTPUT[uMaxOutputCount]);

	for (i=0; i<m_uMaxOutputCount; ++i)
		m_pOutput[i].pMatrix = ALCHEMY_NULL;

	m_bIsReplay = isReplay;

	m_fMaxPlayTime = 0.0f;

	return true;
}*/

bool CAnimationController::RegisterAnimationSet(CAnimationSet& AnimationSet, FLOAT fWeight, FLOAT fSpeed)
{
	/*if(m_uAnimationSetCount == m_uMaxAnimationSetCount)
	{
		ALCHEMY_DEBUG_WARNING("CAnimationController::RegisterAnimationSet: Animation set has full.");

		return false;
	}

	UINT i;
	for(i = 0; i < m_uAnimationSetCount; ++ i)
	{
		if(!m_pAnimationSet[i].pAnimationSet)
			break;
	}

	AnimationSet.AddRef();

	LPANIMATIONSET pAnimationSet = &m_pAnimationSet[i == m_uAnimationSetCount ? m_uAnimationSetCount ++ : i];
	pAnimationSet->pAnimationSet = &AnimationSet;
	pAnimationSet->fWeight       = fWeight;
	pAnimationSet->fSpeed        = fSpeed;
	pAnimationSet->fTime         = 0.0f;

	ALCHEMY_DEBUG_DELETE_ARRAY(pAnimationSet->puMap);

	if(m_bIsMapDirty)
		pAnimationSet->puMap = ALCHEMY_NULL;
	else
	{
		ALCHEMY_DEBUG_NEW(pAnimationSet->puMap, UINT[m_uOutputCount]);
		__BuildMap( pAnimationSet->puMap, pAnimationSet->pAnimationSet->GetAnimations(), pAnimationSet->pAnimationSet->GetAnimationCount() );
	}

	const CAnimationSet::ANIMATION* pAnim = m_pAnimationSet[0].pAnimationSet->GetAnimations();

	if(pAnim->pfTime[ pAnim->uFrameCount -1 ] > m_fMaxPlayTime)
		m_fMaxPlayTime = pAnim->pfTime[ pAnim->uFrameCount -1 ];*/

	AnimationSet.AddRef();

	ANIMATIONSET AnimationSetTemp;
	AnimationSetTemp.pAnimationSet = &AnimationSet;
	AnimationSetTemp.fWeight       = fWeight;
	AnimationSetTemp.fSpeed        = fSpeed;
	AnimationSetTemp.fTime         = 0.0f;

	if(m_bIsMapDirty)
		AnimationSetTemp.puMap = ALCHEMY_NULL;
	else
	{
		ALCHEMY_DEBUG_NEW(AnimationSetTemp.puMap, UINT[m_OutputPool.GetLength()]);
		__BuildMap( AnimationSetTemp.puMap, AnimationSet.GetAnimations(), AnimationSet.GetAnimationCount() );
	}

	m_AnimationSetPool.Add(AnimationSetTemp);

	const CAnimationSet::ANIMATION* pAnimation = AnimationSet.GetAnimations();

	if(pAnimation)
		m_fMaxPlayTime = ALCHEMY_MAXINUM(m_fMaxPlayTime, pAnimation->pfTime[pAnimation->uFrameCount - 1]);

	return true;
}

bool CAnimationController::UnregisterAnimationSet(CAnimationSet& AnimationSet)
{
	/*UINT i;
	LPANIMATIONSET pAnimationSet;

	for(i = 0; i < m_uAnimationSetCount; ++ i)
	{
		pAnimationSet = &m_pAnimationSet[i];
		if(pAnimationSet->pAnimationSet == &AnimationSet)
		{
			pAnimationSet->pAnimationSet = ALCHEMY_NULL;

			ALCHEMY_DEBUG_DELETE_ARRAY(pAnimationSet->puMap);

			return true;
		}
	}

	ALCHEMY_DEBUG_WARNING("CAnimationController::UnregisterAnimationSet: No the animation set");

	return false;*/

	m_fMaxPlayTime = 0.0f;

	LPANIMATIONSET pAnimationSetTemp;
	CPool<ANIMATIONSET>::CIterator Iterator = m_AnimationSetPool.GetIterator(0);
	while( CPool<ANIMATIONSET>::IsValuable(Iterator) )
	{
		pAnimationSetTemp = &(*Iterator);
		if(pAnimationSetTemp->pAnimationSet == &AnimationSet)
		{
			ALCHEMY_DEBUG_DELETE_ARRAY(pAnimationSetTemp->puMap);

			m_AnimationSetPool.Remove(Iterator.GetIndex(), 1);
		}
		else
			m_fMaxPlayTime = ALCHEMY_MAXINUM(m_fMaxPlayTime, pAnimationSetTemp->fTime);

		++ Iterator;
	}

	return true;
}

bool CAnimationController::RegisterAnimationOutput(const CHAR* pcName, MATRIX4X4& Output)
{
	/*if(m_uOutputCount == m_uMaxOutputCount)
	{
		ALCHEMY_DEBUG_WARNING("CAnimationController::RegisterAnimationOutput: Output has full.");

		return false;
	}

	UINT i;
	for(i = 0; i < m_uOutputCount; ++ i)
	{
		if(m_pOutput[i].Name == pcName)
			break;
	}

	LPOUTPUT pOutput;
	if(i == m_uOutputCount)
	{
		pOutput       = &m_pOutput[m_uOutputCount ++];
		pOutput->Name = pcName;
		m_bIsMapDirty = true;
	}
	else
		pOutput = &m_pOutput[i];

	pOutput->pMatrix = &Output;*/

	CPool<OUTPUT>::CIterator Iterator = m_OutputPool.GetIterator(0);

	while( CPool<OUTPUT>::IsValuable(Iterator) )
	{
		if(Iterator->Name == pcName)
			break;

		++ Iterator;
	}

	if( CPool<OUTPUT>::IsValuable(Iterator) )
		Iterator->pMatrix = &Output;
	else
	{
		OUTPUT OutputTemp;

		OutputTemp.Name    = pcName;
		OutputTemp.pMatrix = &Output;

		m_OutputPool.Add(OutputTemp);

		m_bIsMapDirty = true;
	}

	return true;
}

bool CAnimationController::UnregisterAnimationOutput(const CHAR* pcName)
{
	/*UINT i;
	LPOUTPUT pOutput;
	for(i = 0; i < m_uOutputCount; ++ i)
	{
		pOutput = &m_pOutput[i];
		if(pOutput->Name == pcName)
		{
			pOutput->pMatrix = ALCHEMY_NULL;
		}
	}

	ALCHEMY_DEBUG_WARNING("CAnimationController::UnregisterAnimationOutput: No the output set");*/

	UINT uIndex = m_OutputPool.GetLength();
	CPool<OUTPUT>::CIterator Iterator = m_OutputPool.GetIterator(0);
	while( CPool<OUTPUT>::IsValuable(Iterator) )
	{
		if(Iterator->Name == pcName)
		{
			m_OutputPool.Remove(Iterator.GetIndex(), 1);

			return true;
		}

		++ Iterator;
	}

	return false;
}

/*void CAnimationController::ResetTime()
{
	m_fTime = 0.0f;

	for(UINT i = 0; i < m_uAnimationSetCount; ++ i)
		m_pAnimationSet[i].fTime = 0.0f;
}*/

void CAnimationController::SetPlayTime(FLOAT fPlayTime)
{
	SetTime(fPlayTime);

	m_fTime = fPlayTime;

	/*for(UINT i = 0; i < m_uAnimationSetCount; ++ i)
		m_pAnimationSet[i].fTime = fPlayTime;*/

	CPool<ANIMATIONSET>::CIterator Iterator = m_AnimationSetPool.GetIterator(0);
	while( CPool<ANIMATIONSET>::IsValuable(Iterator) )
	{
		Iterator->fTime = fPlayTime;

		++ Iterator;
	}

	Update(0.0);
}

void CAnimationController::Update(FLOAT fElapseTime)
{
	LPANIMATIONSET pAnimationSet;
	UINT uOutputCount = m_OutputPool.GetLength();
	CPool<ANIMATIONSET>::CIterator AnimationSetIterator;
	if(m_bIsMapDirty)
	{
		/*for(i = 0; i < m_uAnimationSetCount; ++ i)
		{
			pAnimationSet = &m_pAnimationSet[i];

			if(pAnimationSet->pAnimationSet)
			{
				ALCHEMY_DEBUG_DELETE_ARRAY(pAnimationSet->puMap);
				ALCHEMY_DEBUG_NEW(pAnimationSet->puMap, UINT[m_uOutputCount]);

				__BuildMap( pAnimationSet->puMap, pAnimationSet->pAnimationSet->GetAnimations(), pAnimationSet->pAnimationSet->GetAnimationCount() );
			}
		}*/

		AnimationSetIterator = m_AnimationSetPool.GetIterator(0);
		while( CPool<ANIMATIONSET>::IsValuable(AnimationSetIterator) )
		{
			pAnimationSet = &(*AnimationSetIterator);

			if(pAnimationSet->pAnimationSet)
			{
				ALCHEMY_DEBUG_DELETE_ARRAY(pAnimationSet->puMap);
				ALCHEMY_DEBUG_NEW(pAnimationSet->puMap, UINT[uOutputCount]);

				__BuildMap( pAnimationSet->puMap, pAnimationSet->pAnimationSet->GetAnimations(), pAnimationSet->pAnimationSet->GetAnimationCount() );
			}

			++ AnimationSetIterator;
		}

		m_bIsMapDirty = false;
	}

	AnimationSetIterator = m_AnimationSetPool.GetIterator(0);
	while( CPool<ANIMATIONSET>::IsValuable(AnimationSetIterator) )
	{
		pAnimationSet = &(*AnimationSetIterator);

		pAnimationSet->fTime += fElapseTime * pAnimationSet->fSpeed;

		m_fTime = ALCHEMY_MAXINUM(m_fTime, pAnimationSet->fTime);

		++ AnimationSetIterator;
	}

	if(m_bIsReplay)
	{
		if(m_fTime >= m_fMaxPlayTime)
		{
			m_fTime = 0.0f;

			ResetTime();

			AnimationSetIterator = m_AnimationSetPool.GetIterator(0);
			while( CPool<ANIMATIONSET>::IsValuable(AnimationSetIterator) )
			{
				pAnimationSet = &(*AnimationSetIterator);

				pAnimationSet->fTime = 0.0f;

				++ AnimationSetIterator;
			}
		}
	}

	/*for(i = 0; i < m_uAnimationSetCount; ++ i)
	{
		pAnimationSet = &m_pAnimationSet[i];
		pAnimationSet->fTime += fElapseTime * pAnimationSet->fSpeed;
	}

	if(m_bIsReplay)
	{
		if(m_fTime >= m_fMaxPlayTime)
		{
			m_fTime = 0.0f;

			for(i = 0; i < m_uAnimationSetCount; ++ i)
			{
				pAnimationSet = &m_pAnimationSet[i];
				pAnimationSet->fTime = 0.0f;
			}
		}
	}*/

	UINT uIndex, uMapIndex;
	LPMATRIX4X4 pDestination;
	bool bResult = true;

	static CMatrix4x4 Destination;

	CPool<OUTPUT>::CIterator OutputIterator = m_OutputPool.GetIterator(0);

	while( CPool<OUTPUT>::IsValuable(OutputIterator) )
	{
		pDestination = OutputIterator->pMatrix;

		if(pDestination)
		{
			uIndex = OutputIterator.GetIndex();

			AnimationSetIterator = m_AnimationSetPool.GetIterator(0);
			while( CPool<ANIMATIONSET>::IsValuable(AnimationSetIterator) )
			{
				pAnimationSet = &(*AnimationSetIterator);

				if(pAnimationSet->pAnimationSet)
				{
					uMapIndex = pAnimationSet->puMap[uIndex];

					if( uMapIndex == pAnimationSet->pAnimationSet->GetAnimationCount() )
					{
						++ AnimationSetIterator;

						continue;
					}

					if( pAnimationSet->pAnimationSet->GetSRT(pAnimationSet->fTime, uMapIndex, Destination) )
						Slerp(*pDestination, *pDestination, Destination, pAnimationSet->fWeight);
				}

				++ AnimationSetIterator;
			}
		}

		++ OutputIterator;
	}

	/*for(i = 0; i < m_uOutputCount; ++ i)
	{
		pDestination = m_pOutput[i].pMatrix;

		if(pDestination)
		{
			//Identity(*pDestination);
			
			for(j = 0; j < m_uAnimationSetCount; ++ j)
			{
				pAnimationSet = &m_pAnimationSet[j];

				if(pAnimationSet->pAnimationSet)
				{
					if( pAnimationSet->puMap[i] == pAnimationSet->pAnimationSet->GetAnimationCount() )
						continue;

					if( pAnimationSet->pAnimationSet->GetSRT(pAnimationSet->fTime, pAnimationSet->puMap[i], Destination) )
						Slerp(*pDestination, *pDestination, Destination, pAnimationSet->fWeight);
				}
			}
		}
	}*/

	TIMERQUEUE::Update(fElapseTime);
}

void CAnimationController::Destroy()
{
	/*UINT i;
	LPANIMATIONSET pAnimationSet;
	for(i=0; i <m_uMaxAnimationSetCount; ++i)
	{
		pAnimationSet = &m_pAnimationSet[i];
		if(pAnimationSet->puMap)
			ALCHEMY_DEBUG_DELETE_ARRAY(pAnimationSet->puMap);
	}

	ALCHEMY_DEBUG_DELETE_ARRAY(m_pAnimationSet);
	ALCHEMY_DEBUG_DELETE_ARRAY(m_pOutput);*/

	m_OutputPool.Clear();

	CPool<ANIMATIONSET>::CIterator Iterator = m_AnimationSetPool.GetIterator(0);
	while( CPool<ANIMATIONSET>::IsValuable(Iterator) )
	{
		ALCHEMY_DEBUG_DELETE_ARRAY(Iterator->puMap);

		++ Iterator;
	}

	m_AnimationSetPool.Clear();
}

void CAnimationController::__BuildMap(PUINT puMap, const CAnimationSet::ANIMATION* pAnimation, UINT uAnimationCount)
{
	/*LPOUTPUT pOutput;
	UINT i, j;
	for(i = 0; i < m_uOutputCount; ++ i)
	{
		pOutput = &m_pOutput[i];
		
		for(j = 0; j < uAnimationCount; ++ j)
		{
			if(pOutput->Name == pAnimation[j].pcName)
			{
				puMap[i] = j;
				break;
			}
		}

		if(j == uAnimationCount)
			puMap[i] = uAnimationCount;
	}*/

	LPOUTPUT pOutput;
	UINT i, uIndex;
	CPool<OUTPUT>::CIterator Iterator = m_OutputPool.GetIterator(0);
	while( CPool<OUTPUT>::IsValuable(Iterator) )
	{
		pOutput = &(*Iterator);
		uIndex = Iterator.GetIndex();

		for(i = 0; i < uAnimationCount; ++ i)
		{
			if(pOutput->Name == pAnimation[i].pcName)
			{
				puMap[uIndex] = i;
				break;
			}
		}

		if(i == uAnimationCount)
			puMap[uIndex] = uAnimationCount;

		++ Iterator;
	}
}