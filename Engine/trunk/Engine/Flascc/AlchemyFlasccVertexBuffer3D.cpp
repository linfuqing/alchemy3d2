#include "AlchemyFlasccStdafx.h"
#include "AlchemyFlasccVertexBuffer3D.h"
#include "AlchemyFlasccContext3D.h"

using namespace alchemy;

CFlasccVertexBuffer3D::CFlasccVertexBuffer3D(void) :
m_pData(ALCHEMY_NULL), 
m_uDataSize(0), 
m_uLockStart(0), 
m_uLockCount(0),
m_uLockFlag(0)
{
}

CFlasccVertexBuffer3D::~CFlasccVertexBuffer3D(void)
{
}

void* CFlasccVertexBuffer3D::Lock(UINT uFlag)
{
	if( ALCHEMY_TEST_BIT(uFlag, VERTEXBUFFER::READ_ONLY) )
		return ALCHEMY_TEST_BIT(GetUsage(), VERTEXBUFFER::WRITE_ONLY) ? ALCHEMY_NULL : m_pData;
	
	m_uLockStart = 0;
	m_uLockCount = GetMemberCount();
	m_uLockFlag  = uFlag;

	UINT uLockSize = m_uLockCount * GetStride();

	if(m_uDataSize < uLockSize)
		ALCHEMY_DEBUG_DELETE_ARRAY(m_pData);

	if(!m_pData)
		ALCHEMY_DEBUG_NEW(m_pData, UINT8[m_uDataSize = GetByteSize()]);

	return m_pData;
}

void* CFlasccVertexBuffer3D::Lock(UINT uStart, UINT uEnd, UINT uFlag)
{
	if(uStart < uEnd)
	{
		UINT uUsage = GetUsage();
		if( ALCHEMY_TEST_BIT(uFlag, VERTEXBUFFER::READ_ONLY) )
			return ALCHEMY_TEST_BIT(uUsage, VERTEXBUFFER::WRITE_ONLY) ? ALCHEMY_NULL : reinterpret_cast<PUINT8>( m_pData + uStart * GetStride() );

		m_uLockStart = uStart;
		m_uLockCount   = uEnd - uStart + 1;
		m_uLockFlag  = uFlag;

		UINT uLockSize = m_uLockCount * GetStride();

		if(m_uDataSize < uLockSize)
			ALCHEMY_DEBUG_DELETE_ARRAY(m_pData);

		if( !ALCHEMY_TEST_BIT(uUsage, VERTEXBUFFER::WRITE_ONLY) || ALCHEMY_TEST_BIT(uUsage, VERTEXBUFFER::DYNAMIC) )
		{
			if(!m_pData)
				ALCHEMY_DEBUG_NEW(m_pData, UINT8[m_uDataSize = GetByteSize()]);

			return reinterpret_cast<PUINT8>(m_pData) + uStart;
		}

		if(!m_pData)
			ALCHEMY_DEBUG_NEW(m_pData, UINT8[m_uDataSize = uLockSize]);

		return m_pData;
	}

	return ALCHEMY_NULL;
}

bool CFlasccVertexBuffer3D::Unlock()
{
	if( ALCHEMY_TEST_BIT(m_uLockFlag, VERTEXBUFFER::READ_ONLY) )
		return true;

	if(!m_uLockCount)
		return false;

	UINT uUsage = GetUsage();
#ifdef AS3_H
	inline_as3(
		"import alchemy.core.AlchemyGraphicsManager;\n"
		"import alchemy.core.AlchemyRenderSystem;\n"
		"var renderSystem = AlchemyGraphicsManager.instance.getRenderSystem(%0);\n"
		"if(renderSystem) renderSystem.uploadVertexBuffer3D(%1, ram, %2, %3, %4);\n"
		: : "r"( static_cast<CFlasccContext3D&>( GetGraphicsResourcePool() ).GetRenderSystemIndex() ),
		"r"( static_cast<unsigned int>( GetResourceHandle() ) ), 
		"r"( ( !ALCHEMY_TEST_BIT(uUsage, VERTEXBUFFER::WRITE_ONLY) || ALCHEMY_TEST_BIT(uUsage, VERTEXBUFFER::DYNAMIC) ) ? reinterpret_cast<PUINT8>(m_pData) + ( m_uLockStart * GetStride() ) : m_pData ), 
		"r"(m_uLockStart), 
		"r"(m_uLockCount)
		);
#endif

	m_uLockStart = 0;
	m_uLockCount = 0;
	m_uLockFlag = 0;

	if( ALCHEMY_TEST_BIT(uUsage, VERTEXBUFFER::WRITE_ONLY) && !ALCHEMY_TEST_BIT(uUsage, VERTEXBUFFER::DYNAMIC) )
		ALCHEMY_DEBUG_DELETE_ARRAY(m_pData);

	return true;
}

void CFlasccVertexBuffer3D::Destroy()
{
	ALCHEMY_DEBUG_DELETE_ARRAY(m_pData);
}

bool CFlasccVertexBuffer3D::_Create()
{
#ifdef AS3_H
	inline_as3(
		"import alchemy.core.AlchemyGraphicsManager;\n"
		"import alchemy.core.AlchemyRenderSystem;\n"
		"var renderSystem = AlchemyGraphicsManager.instance.getRenderSystem(%0);\n"
		"if(renderSystem) renderSystem.createVertexBuffer3D(%1, %2);\n"
		: : "r"( static_cast<CFlasccContext3D&>( GetGraphicsResourcePool() ).GetRenderSystemIndex() ), "r"( GetMemberCount() ), "r"(GetStride() >> 2)
		);
#endif

	return true;
}