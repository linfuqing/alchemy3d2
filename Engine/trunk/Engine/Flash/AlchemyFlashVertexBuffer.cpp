#include "AlchemyFlashStdafx.h"
#include "AlchemyFlashVertexBuffer.h"
#include "AlchemyFlashContext3D.h"

using namespace alchemy;
CFlashVertexBuffer::CFlashVertexBuffer(void) :
m_pData(ALCHEMY_NULL)
{
	Reset();
}

CFlashVertexBuffer::~CFlashVertexBuffer(void)
{
}

void* CFlashVertexBuffer::Lock(UINT uFlag)
{
	if( !ALCHEMY_TEST_BIT(uFlag, READ_ONLY) )
	{
		if(IsActivate() && m_uStartDirty < m_uEndDirty)
			static_cast<CFlashContext3D&>( GetGraphicsResourcePool() ).SubmitRenderDataQueueToRender();

		m_uStartDirty = 0;
		m_uEndDirty   = GetMemberCount() - 1;
	}

	if(IsNoData( GetUsage() ) && !m_pData)
		ALCHEMY_DEBUG_NEW(m_pData, UINT8[GetByteSize()]);

	return m_pData;
}

void* CFlashVertexBuffer::Lock(UINT uStart, UINT uEnd, UINT uFlag)
{
	if( !ALCHEMY_TEST_BIT(uFlag, READ_ONLY) )
	{
		if(IsActivate() && m_uStartDirty < m_uEndDirty)
			static_cast<CFlashContext3D&>( GetGraphicsResourcePool() ).SubmitRenderDataQueueToRender();

		m_uStartDirty = ALCHEMY_MININUM(uStart, m_uStartDirty);
		m_uEndDirty   = ALCHEMY_MAXINUM(uEnd  , m_uEndDirty);
	}

	if( IsNoData( GetUsage() ) && !m_pData )
		ALCHEMY_DEBUG_NEW(m_pData, UINT8[GetByteSize()]);

	return m_pData + uStart * GetStride();
}

bool CFlashVertexBuffer::Unlock()
{
	return true;
}

void CFlashVertexBuffer::Destroy()
{
	ALCHEMY_DEBUG_DELETE_ARRAY(m_pData);
}

bool CFlashVertexBuffer::_Create()
{
	if( !IsNoData( GetUsage() ) )
		ALCHEMY_DEBUG_NEW(m_pData, UINT8[GetByteSize()]);

	static_cast<CFlashContext3D&>( GetGraphicsResourcePool() ).CallBackToCreate( this, static_cast<const CFlashObjectInterface*>(this), GetType(), GetResourceHandle() );

	return true;
}

#ifdef AS3_H
AS3TYPE CFlashVertexBuffer::Init()
{
	return AS3_Array("PtrType, PtrType, PtrType, IntType, IntType", &m_pData, &m_uStartDirty, &m_uEndDirty, GetMemberCount(), GetStride() >> 2);
}
#endif