#include "AlchemyBuffer.h"
#include "AlchemyNull.h"

using namespace alchemy;

CBuffer& alchemy::CreateBuffer(UINT uSize, bool bIsWeakKey)
{
	BUFFER* pBuffer;
	ALCHEMY_DEBUG_NEW( pBuffer, BUFFER(bIsWeakKey) );

	if(uSize)
		ALCHEMY_DEBUG_NEW(pBuffer->m_pBuffer, UINT8[uSize]);

	pBuffer->m_uSize = uSize;

	return *pBuffer;
}

CBuffer::CBuffer() :
m_pBuffer(ALCHEMY_NULL),
m_uSize(0)
{
}

CBuffer::~CBuffer()
{
	ALCHEMY_DEBUG_DELETE_ARRAY(m_pBuffer);
}

bool CBuffer::CopyFrom(const void* pBuffer, UINT uFrom, UINT uSize)
{
	if(m_pBuffer && pBuffer)
	{
		uSize = uSize ? uSize : (m_uSize - uFrom);

		UINT uCurrentSize = uFrom + uSize;
		if(uCurrentSize > m_uSize)
		{
			PVOID pTemp;
			ALCHEMY_DEBUG_NEW(pTemp, UINT8[uCurrentSize]);

			ALCHEMY_MEMORY_COPY( pTemp, m_pBuffer, ALCHEMY_MININUM(uFrom, m_uSize) );

			ALCHEMY_DEBUG_DELETE_ARRAY(m_pBuffer);

			m_pBuffer = pTemp;
			m_uSize   = uCurrentSize;
		}

		ALCHEMY_MEMORY_COPY( reinterpret_cast<UINT8*>(m_pBuffer) + uFrom, pBuffer, uSize);

		return true;
	}

	return false;
}