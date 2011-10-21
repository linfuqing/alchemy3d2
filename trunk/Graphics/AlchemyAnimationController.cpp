#include "AlchemyAnimationController.h"

using namespace alchemy;

CAnimationController::CAnimationController(void):
m_pOutput(ALCHEMY_NULL),
m_pAnimationSet(ALCHEMY_NULL),
m_uOutputCount(0),
m_uMaxOutputCount(0),
m_uAnimationSetCount(0),
m_uMaxAnimationSetCount(0),
m_bIsMapDirty(false),
m_fTime(0.0f)
{

}

CAnimationController::~CAnimationController(void)
{
	Destroy();
}

bool CAnimationController::Create(UINT uMaxOutputCount, UINT uMaxAnimationSetCount)
{
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

	for (i=0; i<m_uMaxAnimationSetCount; ++i)
		m_pOutput[i].pMatrix = ALCHEMY_NULL;

	return true;
}

void CAnimationController::Destroy()
{
	UINT i;
	LPANIMATIONSET pAnimationSet;
	for(i=0; i <m_uMaxAnimationSetCount; ++i)
	{
		pAnimationSet = &m_pAnimationSet[i];
		if(pAnimationSet->puMap)
			ALCHEMY_DEBUG_DELETE_ARRAY(pAnimationSet->puMap);
	}

	ALCHEMY_DEBUG_DELETE_ARRAY(m_pAnimationSet);
	ALCHEMY_DEBUG_DELETE_ARRAY(m_pOutput);

}

bool CAnimationController::RegisterAnimationSet(CAnimationSet& AnimationSet, FLOAT fWeight, FLOAT fSpeed)
{
	if(m_uAnimationSetCount == m_uMaxAnimationSetCount)
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

	LPANIMATIONSET pAnimationSet = &m_pAnimationSet[i == m_uAnimationSetCount ? m_uAnimationSetCount ++ : i];
	pAnimationSet->pAnimationSet = &AnimationSet;
	pAnimationSet->fWeight       = fWeight;
	pAnimationSet->fSpeed        = fSpeed;

	ALCHEMY_DEBUG_DELETE_ARRAY(pAnimationSet->puMap);

	if(m_bIsMapDirty)
		pAnimationSet->puMap = ALCHEMY_NULL;
	else
	{
		ALCHEMY_DEBUG_NEW(pAnimationSet->puMap, UINT[m_uOutputCount]);
		__BuildMap( pAnimationSet->puMap, pAnimationSet->pAnimationSet->GetAnimations(), pAnimationSet->pAnimationSet->GetAnimationCount() );
	}

	return true;
}

bool CAnimationController::UnregisterAnimationSet(CAnimationSet& AnimationSet)
{
	UINT i;
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

	return false;
}

bool CAnimationController::RegisterAnimationOutput(const CHAR* pcName, MATRIX4X4& Output)
{
	if(m_uOutputCount == m_uMaxOutputCount)
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

	pOutput->pMatrix = &Output;

	return true;
}

bool CAnimationController::UnregisterAnimationOutput(const CHAR* pcName)
{
	UINT i;
	LPOUTPUT pOutput;
	for(i = 0; i < m_uOutputCount; ++ i)
	{
		pOutput = &m_pOutput[i];
		if(pOutput->Name == pcName)
		{
			pOutput->pMatrix = ALCHEMY_NULL;
		}
	}

	ALCHEMY_DEBUG_WARNING("CAnimationController::UnregisterAnimationOutput: No the output set");

	return false;
}

void CAnimationController::ResetTime()
{
	m_fTime = 0.0f;

	for(UINT i = 0; i < m_uAnimationSetCount; ++ i)
		m_pAnimationSet[i].fTime = 0.0f;
}

bool CAnimationController::Update(FLOAT fElapseTime)
{
	m_fTime += fElapseTime;

	LPANIMATIONSET pAnimationSet;
	UINT i;
	if(m_bIsMapDirty)
	{
		for(i = 0; i < m_uAnimationSetCount; ++ i)
		{
			pAnimationSet = &m_pAnimationSet[i];

			if(pAnimationSet->pAnimationSet)
			{
				ALCHEMY_DEBUG_DELETE_ARRAY(pAnimationSet->puMap);
				ALCHEMY_DEBUG_NEW(pAnimationSet->puMap, UINT[m_uOutputCount]);

				__BuildMap( pAnimationSet->puMap, pAnimationSet->pAnimationSet->GetAnimations(), pAnimationSet->pAnimationSet->GetAnimationCount() );
			}
		}

		m_bIsMapDirty = false;
	}

	for(i = 0; i < m_uAnimationSetCount; ++ i)
	{
		pAnimationSet = &m_pAnimationSet[i];
		pAnimationSet->fTime += fElapseTime * pAnimationSet->fSpeed;
	}

	UINT j;
	LPMATRIX4X4 pDestination;
	bool bResult = true;

	static CMatrix4x4 Destination;

	for(i = 0; i < m_uOutputCount; ++ i)
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
	}

	return bResult;
}

void CAnimationController::__BuildMap(PUINT puMap, const CAnimationSet::ANIMATION* pAnimation, UINT uAnimationCount)
{
	LPOUTPUT pOutput;
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
	}
}