#include "AlchemyScriptMemery.h"

using namespace alchemy;

CScriptMemery::CScriptMemery(void) : 
m_uStackIndexCount(0), 
m_pStackMemery(ALCHEMY_NULL), 
m_uStackMemeryCurrentSize(0), 
m_uStackMemeryTotalSize(0)
{
}

CScriptMemery::~CScriptMemery(void)
{
	CPool<MEMERYINFO>::CIterator Iterator = m_MemeryInfos.GetIterator(0);
	LPMEMERYINFO pMemeryInfo;
	while( CPool<MEMERYINFO>::IsValuable(Iterator) )
	{
		pMemeryInfo = &(*Iterator);
		if(pMemeryInfo->bIsHeap)
			ALCHEMY_DEBUG_DELETE_ARRAY(pMemeryInfo->pData);

		++ Iterator;
	}

	ALCHEMY_DEBUG_DELETE_ARRAY(m_pStackMemery);
}

void* CScriptMemery::Get(UINT uHandle, UINT uOffset)
{
	if(uHandle)
	{
		UINT uIndex = uHandle - 1;
		LPMEMERYINFO pMemeryInfo = m_MemeryInfos.Get(uIndex);
		if(pMemeryInfo && uOffset < pMemeryInfo->uSize)
		{
			if(!pMemeryInfo->bIsHeap && !pMemeryInfo->pData)
			{
				if( (m_uStackMemeryTotalSize - m_uStackMemeryCurrentSize) < pMemeryInfo->uSize )
				{
					UINT uSize = ALCHEMY_FLAG( ALCHEMY_MAXINUM_BIT(pMemeryInfo->uSize) );
					if( (uSize - m_uStackMemeryCurrentSize) < pMemeryInfo->uSize )
						uSize <<= 1;

					PVOID pData;
					ALCHEMY_DEBUG_NEW(pData, UINT8[uSize]);

					if(m_pStackMemery)
					{
						ALCHEMY_MEMORY_COPY(pData, m_pStackMemery, m_uStackMemeryCurrentSize);

						ALCHEMY_DEBUG_DELETE_ARRAY(m_pStackMemery);
					}

					m_pStackMemery             = pData;
					m_uStackMemeryTotalSize    = uSize;
					m_uStackMemeryCurrentSize += pMemeryInfo->uSize;
				}

				pMemeryInfo->pData = static_cast<PUINT8>(m_pStackMemery) + m_uStackMemeryTotalSize;

				if(pMemeryInfo->pSource)
					ALCHEMY_MEMORY_COPY(pMemeryInfo->pData, pMemeryInfo->pSource, pMemeryInfo->uSize);

				m_uStackMemeryTotalSize += pMemeryInfo->uSize;

				if( m_uStackIndexCount < m_StackIndices.GetLength() )
					m_StackIndices[m_uStackIndexCount ++] = uIndex;
				else
					m_StackIndices.Add(uIndex);
			}

			return static_cast<PUINT8>(pMemeryInfo->pData) + uOffset;
		}
	}

	return ALCHEMY_NULL;
}

UINT CScriptMemery::Alloc(UINT uSize, bool bIsHeap, const void* pData)
{
	MEMERYINFO MemeryInfo;

	if(bIsHeap)
	{
		ALCHEMY_DEBUG_NEW(MemeryInfo.pData, UINT8[uSize]);

		ALCHEMY_MEMORY_COPY(MemeryInfo.pData, pData, uSize);
	}
	else
		MemeryInfo.pData = ALCHEMY_NULL;

	MemeryInfo.uSize = uSize;

	MemeryInfo.bIsHeap = bIsHeap;

	MemeryInfo.pSource = pData;

	return m_MemeryInfos.Add(MemeryInfo) + 1;
}

bool CScriptMemery::Free(UINT uHandle)
{
	if(uHandle)
	{
		UINT uIndex = uHandle - 1;

		LPMEMERYINFO pMemeryInfo = m_MemeryInfos.Get(uIndex);
		if(pMemeryInfo)
		{
			if(pMemeryInfo->bIsHeap)
				ALCHEMY_DEBUG_DELETE_ARRAY(pMemeryInfo->pData);

			return m_MemeryInfos.Remove(uIndex, 1);
		}
	}

	return false;
}

bool CScriptMemery::FreeStack(UINT uFrom)
{
	if(uFrom < m_uStackIndexCount)
	{
		LPMEMERYINFO pMemeryInfo;
		UINT uTotalSize = 0;
		for(UINT i = 0; i < m_uStackIndexCount; ++ i)
		{
			pMemeryInfo = m_MemeryInfos.Get(i);
			if(pMemeryInfo && !pMemeryInfo->bIsHeap)
			{
				pMemeryInfo->pData = ALCHEMY_NULL;

				uTotalSize += pMemeryInfo->uSize;
			}
		}

		m_uStackIndexCount = uFrom;
		m_uStackMemeryCurrentSize = uTotalSize < m_uStackMemeryCurrentSize ? (m_uStackMemeryCurrentSize - uTotalSize) : 0;

		return true;
	}

	return false;
}

void CScriptMemery::Destroy()
{
	CPool<MEMERYINFO>::CIterator Iterator = m_MemeryInfos.GetIterator(0);
	LPMEMERYINFO pMemeryInfo;
	while( CPool<MEMERYINFO>::IsValuable(Iterator) )
	{
		pMemeryInfo = &(*Iterator);
		if(pMemeryInfo->bIsHeap)
			ALCHEMY_DEBUG_DELETE_ARRAY(pMemeryInfo->pData);

		++ Iterator;
	}

	m_MemeryInfos.Clear();

	m_StackIndices.Clear();

	m_uStackIndexCount = 0;

	ALCHEMY_DEBUG_DELETE_ARRAY(m_pStackMemery);

	m_uStackMemeryTotalSize = 0;

	m_uStackMemeryCurrentSize = 0;
}