#include "AlchemyD3D9VertexBuffer.h"
#include "AlchemyD3D9Device.h"

#include <d3d9.h>

#pragma comment(lib,"d3d9.lib")

using namespace alchemy;

CD3D9VertexBuffer::CD3D9VertexBuffer(void) : 
m_pD3D9VertexBuffer(ALCHEMY_NULL), 
m_pData(ALCHEMY_NULL), 
m_pSourceData(ALCHEMY_NULL)
{
}

CD3D9VertexBuffer::~CD3D9VertexBuffer(void)
{
}

void* CD3D9VertexBuffer::Lock(alchemy::UINT uFlag)
{
	if(m_pD3D9VertexBuffer)
	{
		UINT uUsage = GetUsage();
		if( ALCHEMY_TEST_BIT(uUsage, VERTEXBUFFER::WRITE_ONLY) && ALCHEMY_TEST_BIT(uFlag, VERTEXBUFFER::READ_ONLY) )
			return ALCHEMY_NULL;

		void* pData;
		if(m_pD3D9VertexBuffer->Lock( 0, GetByteSize(), &pData, ALCHEMY_TEST_BIT(uFlag, VERTEXBUFFER::READ_ONLY) ? 0 : (ALCHEMY_TEST_BIT(uUsage, VERTEXBUFFER::DYNAMIC) ? D3DLOCK_DISCARD : 0) ) == S_OK)
			return pData;
	}

	return ALCHEMY_NULL;
}

void* CD3D9VertexBuffer::Lock(alchemy::UINT uStart, alchemy::UINT uEnd, alchemy::UINT uFlag)
{
	if(m_pD3D9VertexBuffer)
	{
		UINT uUsage = GetUsage();
		if( ALCHEMY_TEST_BIT(uUsage, VERTEXBUFFER::WRITE_ONLY) && ALCHEMY_TEST_BIT(uFlag, VERTEXBUFFER::READ_ONLY) )
			return ALCHEMY_NULL;

		void* pData;
		if(m_pD3D9VertexBuffer->Lock( 
			uStart, 
			(uEnd + 1 - uStart) * GetByteSize(),
			&pData, 
			ALCHEMY_TEST_BIT(uFlag, VERTEXBUFFER::READ_ONLY) ? D3DLOCK_READONLY : ( ( uStart == 0 && ALCHEMY_TEST_BIT(uUsage, VERTEXBUFFER::DYNAMIC) ) ? D3DLOCK_DISCARD : (ALCHEMY_TEST_BIT(uFlag, VERTEXBUFFER::NO_OVER_WRITE) ? D3DLOCK_NOOVERWRITE : 0) ) ) == S_OK)
			return pData;
	}

	return ALCHEMY_NULL;
}

bool CD3D9VertexBuffer::Unlock()
{
	if(m_pD3D9VertexBuffer)
	{
		if(ALCHEMY_TEST_BIT(GetUsage(), VERTEXBUFFER::DYNAMIC) && m_pSourceData)
		{
			UINT uByteSize = GetByteSize();
			if(!m_pData)
				ALCHEMY_DEBUG_NEW(m_pData, UINT8[uByteSize]);

			ALCHEMY_MEMORY_COPY(m_pData, m_pSourceData, uByteSize);

			m_pSourceData = ALCHEMY_NULL;
		}

		return m_pD3D9VertexBuffer->Unlock() == D3D_OK;
	}

	return false;
}

bool CD3D9VertexBuffer::_Create()
{
	IDirect3DDevice9* pDevice = static_cast<CD3D9Device&>( GetGraphicsResourcePool() ).GetDevice();
	if(pDevice)
	{
		alchemy::UINT uUsage = GetUsage(), uD3D9Usage = 0;
		if( ALCHEMY_TEST_BIT(uUsage, VERTEXBUFFER::WRITE_ONLY) )
			uD3D9Usage |= D3DUSAGE_WRITEONLY;

		ALCHEMY_DEBUG_DELETE_ARRAY(m_pData);

		D3DPOOL D3DPool = D3DPOOL_MANAGED;
		if( ALCHEMY_TEST_BIT(uUsage, VERTEXBUFFER::DYNAMIC) )
		{
			uD3D9Usage |= D3DUSAGE_DYNAMIC;

			D3DPool = D3DPOOL_DEFAULT;
		}

		return pDevice->CreateVertexBuffer(GetByteSize(), uD3D9Usage, 0, D3DPool, &m_pD3D9VertexBuffer, ALCHEMY_NULL) == D3D_OK;
	}

	return false;
}

void CD3D9VertexBuffer::Destroy()
{
	if(m_pD3D9VertexBuffer)
	{
		m_pD3D9VertexBuffer->Release();

		m_pD3D9VertexBuffer = ALCHEMY_NULL;
	}

	ALCHEMY_DEBUG_DELETE_ARRAY(m_pData);

	m_pSourceData = ALCHEMY_NULL;
}