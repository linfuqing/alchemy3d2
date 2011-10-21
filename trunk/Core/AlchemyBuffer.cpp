#include "AlchemyBuffer.h"

using namespace alchemy;

CBuffer& alchemy::CreateBuffer(UINT uSize)
{
	CBuffer& Buffer = BUFFER::Create();

	ALCHEMY_DEBUG_NEW(Buffer.m_pBuffer, UINT8[uSize]);
	Buffer.m_uSize = uSize;

	return Buffer;
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