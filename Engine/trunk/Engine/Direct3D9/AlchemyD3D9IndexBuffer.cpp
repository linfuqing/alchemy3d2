#include "AlchemyD3D9IndexBuffer.h"
#include "AlchemyD3D9Device.h"

#include <d3d9.h>

#pragma comment(lib,"d3d9.lib")

using namespace alchemy;

CD3D9IndexBuffer::CD3D9IndexBuffer(void) : 
m_pD3D9IndexBuffer(ALCHEMY_NULL), 
m_pData(ALCHEMY_NULL), 
m_pSourceData(ALCHEMY_NULL)
{
}

CD3D9IndexBuffer::~CD3D9IndexBuffer(void)
{
	if(m_pD3D9IndexBuffer)
	{
		m_pD3D9IndexBuffer->Release();

		m_pD3D9IndexBuffer = ALCHEMY_NULL;
	}

	ALCHEMY_DEBUG_DELETE_ARRAY(m_pData);
}

void* CD3D9IndexBuffer::Lock(alchemy::UINT uFlag)
{
	if(m_pD3D9IndexBuffer)
	{
		UINT uUsage = GetUsage();
		if( ALCHEMY_TEST_BIT(uUsage, INDEXBUFFER::WRITE_ONLY) && ALCHEMY_TEST_BIT(uFlag, INDEXBUFFER::READ_ONLY) )
			return ALCHEMY_NULL;

		void* pData;
		if(m_pD3D9IndexBuffer->Lock( 0, GetByteSize(), &pData, ALCHEMY_TEST_BIT(uFlag, INDEXBUFFER::READ_ONLY) ? 0 : (ALCHEMY_TEST_BIT(uUsage, INDEXBUFFER::DYNAMIC) ? D3DLOCK_DISCARD : 0) ) == S_OK)
			return pData;
	}

	return ALCHEMY_NULL;
}

void* CD3D9IndexBuffer::Lock(alchemy::UINT uStart, alchemy::UINT uEnd, alchemy::UINT uFlag)
{
	if(m_pD3D9IndexBuffer)
	{
		UINT uUsage = GetUsage();
		if( ALCHEMY_TEST_BIT(uUsage, INDEXBUFFER::WRITE_ONLY) && ALCHEMY_TEST_BIT(uFlag, INDEXBUFFER::READ_ONLY) )
			return ALCHEMY_NULL;

		void* pData;
		if(m_pD3D9IndexBuffer->Lock( 
			uStart, 
			(uEnd + 1 - uStart) * GetByteSize(),
			&pData, 
			ALCHEMY_TEST_BIT(uFlag, INDEXBUFFER::READ_ONLY) ? 0 : ( ( uStart == 0 && ALCHEMY_TEST_BIT(uUsage, INDEXBUFFER::DYNAMIC) ) ? D3DLOCK_DISCARD : (ALCHEMY_TEST_BIT(uFlag, INDEXBUFFER::NO_OVER_WRITE) ? D3DLOCK_NOOVERWRITE : 0) ) ) == S_OK)
			return pData;
	}

	return ALCHEMY_NULL;
}

bool CD3D9IndexBuffer::Unlock()
{
	if(m_pD3D9IndexBuffer)
	{
		if(ALCHEMY_TEST_BIT(GetUsage(), INDEXBUFFER::DYNAMIC) && m_pSourceData)
		{
			UINT uByteSize = GetByteSize();
			if(!m_pData)
				ALCHEMY_DEBUG_NEW(m_pData, UINT8[uByteSize]);

			ALCHEMY_MEMORY_COPY(m_pData, m_pSourceData, uByteSize);

			m_pSourceData = ALCHEMY_NULL;
		}

		return m_pD3D9IndexBuffer->Unlock() == D3D_OK;
	}

	return false;
}

bool CD3D9IndexBuffer::_Create()
{
	IDirect3DDevice9* pDevice = static_cast<CD3D9Device&>( GetGraphicsResourcePool() ).GetDevice();
	if(pDevice)
	{
		alchemy::UINT uUsage = GetUsage(), uD3D9Usage = 0;
		if( ALCHEMY_TEST_BIT(uUsage, INDEXBUFFER::WRITE_ONLY) )
			uD3D9Usage |= D3DUSAGE_WRITEONLY;

		ALCHEMY_DEBUG_DELETE_ARRAY(m_pData);

		D3DPOOL D3DPool = D3DPOOL_MANAGED;
		if( ALCHEMY_TEST_BIT(uUsage, INDEXBUFFER::DYNAMIC) )
		{
			uD3D9Usage |= D3DUSAGE_DYNAMIC;

			D3DPool = D3DPOOL_DEFAULT;
		}

		return pDevice->CreateIndexBuffer(GetByteSize(), uD3D9Usage, GetFormat() == INDEXBUFFER::BIT32 ? D3DFMT_INDEX32 : D3DFMT_INDEX16, D3DPool, &m_pD3D9IndexBuffer, ALCHEMY_NULL) == D3D_OK;
	}

	return false;
}

void CD3D9IndexBuffer::Destroy()
{
	if(m_pD3D9IndexBuffer)
	{
		m_pD3D9IndexBuffer->Release();

		m_pD3D9IndexBuffer = ALCHEMY_NULL;
	}

	ALCHEMY_DEBUG_DELETE_ARRAY(m_pData);

	m_pSourceData = ALCHEMY_NULL;
}