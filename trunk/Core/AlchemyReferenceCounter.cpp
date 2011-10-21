#include "AlchemyReferenceCounter.h"

using namespace alchemy;

CReferenceCounter::CReferenceCounter(void)
{
	m_nReferenceCount = 0;
}

CReferenceCounter::~CReferenceCounter(void)
{
	m_nReferenceCount = 0;
}

INT CReferenceCounter::AddRef()
{
	return ++ m_nReferenceCount;
}

INT CReferenceCounter::Release()
{
	return -- m_nReferenceCount;
}