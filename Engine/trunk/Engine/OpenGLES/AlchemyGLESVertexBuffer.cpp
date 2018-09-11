#include "AlchemyGLESVertexBuffer.h"

using namespace alchemy;

CGLESVertexBuffer::CGLESVertexBuffer() :
m_uBufferHandle(0), 
m_puData(ALCHEMY_NULL),
m_uLockStart(0),
m_uLockEnd(0),
m_uLockFlag(0)
{
}

CGLESVertexBuffer::~CGLESVertexBuffer()
{
	ALCHEMY_DEBUG_DELETE_ARRAY(m_puData);
}

void CGLESVertexBuffer::Activate()
{
	UINT uUsage = GetUsage();
	if( ALCHEMY_TEST_BIT(uUsage, VERTEXBUFFER::WRITE_ONLY) && !ALCHEMY_TEST_BIT(uUsage, VERTEXBUFFER::DYNAMIC) )
		ALCHEMY_DEBUG_DELETE_ARRAY(m_puData);
			
	glBindBuffer(GL_ARRAY_BUFFER, m_uBufferHandle);
}

void* CGLESVertexBuffer::Lock(UINT uStart, UINT uEnd, UINT uFlag)
{
	uEnd = ALCHEMY_MININUM( uEnd, GetMemberCount() );
    if (uStart < uEnd) 
        return ALCHEMY_NULL;
    
    if( ALCHEMY_TEST_BIT(m_uLockFlag, VERTEXBUFFER::NO_OVER_WRITE) && ALCHEMY_TEST_BIT(uFlag, VERTEXBUFFER::NO_OVER_WRITE) )
    {
        if(m_uLockStart == uEnd)
            m_uLockStart = uStart; 
        else if(m_uLockEnd == uStart)
            m_uLockEnd = uEnd;
        else
            return ALCHEMY_NULL;
    }
    else
    {
        m_uLockStart = uStart;
        m_uLockEnd   = uEnd;
    }
    
    m_uLockFlag  = uFlag;
    
    if ( ALCHEMY_TEST_BIT(GetUsage(), VERTEXBUFFER::WRITE_ONLY) ) 
    {
        if ( ALCHEMY_TEST_BIT(uFlag, VERTEXBUFFER::READ_ONLY) ) 
            return ALCHEMY_NULL;
    }
    
    if( ALCHEMY_TEST_BIT(uFlag, VERTEXBUFFER::DISCARD) )
    {
        ALCHEMY_DEBUG_DELETE_ARRAY(m_puData);
        ALCHEMY_DEBUG_NEW(m_puData, UINT8[GetByteSize()]);
    }
    else if(!m_puData)
    {
        if( ALCHEMY_TEST_BIT(m_uLockFlag, VERTEXBUFFER::NO_OVER_WRITE) || !ALCHEMY_TEST_BIT(GetUsage(), VERTEXBUFFER::WRITE_ONLY) )
        {
            ALCHEMY_DEBUG_NEW(m_puData, UINT8[GetByteSize()]);
        }
        else
        {
            ALCHEMY_DEBUG_NEW(m_puData, UINT8[(uEnd - uStart + 1) * GetStride()]);
            
            return m_puData;
        }
    }
    
    return m_puData + uStart * GetStride();
}

void* CGLESVertexBuffer::Lock(                        UINT uFlag)
{
	m_uLockStart = 0;
    m_uLockEnd   = GetMemberCount() - 1;
    m_uLockFlag  = uFlag;
    
    if ( ALCHEMY_TEST_BIT(GetUsage(), VERTEXBUFFER::WRITE_ONLY) ) 
    {
        if ( ALCHEMY_TEST_BIT(uFlag, VERTEXBUFFER::READ_ONLY) ) 
            return ALCHEMY_NULL;
    }
    
    if( ALCHEMY_TEST_BIT(uFlag, VERTEXBUFFER::DISCARD) )
    {
        ALCHEMY_DEBUG_DELETE_ARRAY(m_puData);
        ALCHEMY_DEBUG_NEW(m_puData, UINT8[GetByteSize()]);
    }
    else if(!m_puData)
        ALCHEMY_DEBUG_NEW(m_puData, UINT8[GetByteSize()]);
	
    return m_puData;
}

bool  CGLESVertexBuffer::Unlock(                                )
{
	if( !ALCHEMY_TEST_BIT(m_uLockFlag, VERTEXBUFFER::READ_ONLY) )
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_uBufferHandle);
        if(m_uLockStart == 0 && m_uLockEnd == GetMemberCount() - 1)
        {
            glBufferData(GL_ARRAY_BUFFER, GetByteSize(), m_puData, ALCHEMY_TEST_BIT(GetUsage(), VERTEXBUFFER::DYNAMIC) ? GL_STREAM_DRAW : GL_STATIC_DRAW);
            
            m_uLockStart = m_uLockEnd = 0;
            
            /*if( ALCHEMY_TEST_BIT(GetUsage(), VERTEXBUFFER::WRITE_ONLY) )
                ALCHEMY_DEBUG_DELETE_ARRAY(m_puData);*/
        }
        else if( !ALCHEMY_TEST_BIT(m_uLockFlag, VERTEXBUFFER::NO_OVER_WRITE) )
        {
            UINT uStride = GetStride(), uStart = ALCHEMY_TEST_BIT(GetUsage(), VERTEXBUFFER::WRITE_ONLY), uCount = (m_uLockEnd - m_uLockStart + 1) * uStride;
            if( ALCHEMY_TEST_BIT(GetUsage(), VERTEXBUFFER::WRITE_ONLY) )
            {
                glBufferSubData(GL_ARRAY_BUFFER, uStart, uCount, m_puData);
                
                //ALCHEMY_DEBUG_DELETE_ARRAY(m_puData);
            }
            else
                glBufferSubData(GL_ARRAY_BUFFER, uStart , uCount, m_puData + uStart);
            
            m_uLockStart = m_uLockEnd = 0;
        }
    }
    
    return true;	
}

void CGLESVertexBuffer::Destroy()
{
	ALCHEMY_DEBUG_DELETE_ARRAY(m_puData);
	
	if(m_uBufferHandle)
		glDeleteBuffers(1, &m_uBufferHandle);
}

bool CGLESVertexBuffer::_Create()
{
	glGenBuffers(1, &m_uBufferHandle);
	
	return true;
}