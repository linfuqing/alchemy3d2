#include "AlchemyFlashStdafx.h"
#include "AlchemyFlashIndexBuffer.h"
#include "AlchemyFlashContext3D.h"
//#include "../Core/AlchemyUtils.h"
//#include "../Core/AlchemyBitFlag.h"

using namespace alchemy;

CFlashIndexBuffer::CFlashIndexBuffer(void) :
m_pData(ALCHEMY_NULL)
{
	Reset();
}

CFlashIndexBuffer::~CFlashIndexBuffer(void)
{
}

void* CFlashIndexBuffer::Lock(UINT uFlag)
{
	if( !ALCHEMY_TEST_BIT(uFlag, READ_ONLY) )
	{
		if( IsActivate() && m_uStartDirty < m_uEndDirty)
			static_cast<CFlashContext3D&>( GetGraphicsResourcePool() ).SubmitRenderDataQueueToRender();

		m_uStartDirty = 0;
		m_uEndDirty   = GetMemberCount() - 1;
	}

	if( IsNoData( GetUsage() ) && !m_pData )
		ALCHEMY_DEBUG_NEW(m_pData, UINT8[GetByteSize()]);

	return m_pData;
}

void* CFlashIndexBuffer::Lock(UINT uStart, UINT uEnd, UINT uFlag)
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

bool CFlashIndexBuffer::Unlock()
{
	return true;
}

void CFlashIndexBuffer::Destroy()
{
	ALCHEMY_DEBUG_DELETE_ARRAY(m_pData);
}

bool CFlashIndexBuffer::_Create()
{
	if( !IsNoData( GetUsage() ) )
		ALCHEMY_DEBUG_NEW(m_pData, UINT8[GetByteSize()]);

	static_cast<CFlashContext3D&>( GetGraphicsResourcePool() ).CallBackToCreate( this, static_cast<const CFlashObjectInterface*>(this), GetType(), GetResourceHandle() );

	return true;
}

#ifdef AS3_H
AS3TYPE CFlashIndexBuffer::Init()
{
	return AS3_Array( "PtrType, PtrType, PtrType, IntType", &m_pData, &m_uStartDirty, &m_uEndDirty, GetMemberCount() );
}
#endif