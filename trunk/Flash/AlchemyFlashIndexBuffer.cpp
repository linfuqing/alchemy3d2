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

PVOID CFlashIndexBuffer::Lock(UINT uFlag)
{
	if( !ALCHEMY_TEST_BIT(uFlag, CGraphicsBuffer<CIndexBuffer>::READ_ONLY) )
	{
		if( CFlashGraphicsBuffer<CIndexBuffer>::IsActivate() && m_uStartDirty < m_uEndDirty)
			static_cast<CFlashContext3D&>( GetGraphicsResourcePool() ).SubmitRenderDataQueueToRender();

		m_uStartDirty = 0;
		m_uEndDirty   = CGraphicsBuffer<CIndexBuffer>::GetMemberCount() - 1;
	}

	if( CFlashGraphicsBuffer<CVertexBuffer>::IsNoData( CGraphicsBuffer<CIndexBuffer>::GetUsage() ) && !m_pData )
		ALCHEMY_DEBUG_NEW(m_pData, UINT8[CGraphicsBuffer<CIndexBuffer>::GetByteSize()]);

	return m_pData;
}

PVOID CFlashIndexBuffer::Lock(UINT uStart, UINT uEnd, UINT uFlag)
{
	if( !ALCHEMY_TEST_BIT(uFlag, CGraphicsBuffer<CIndexBuffer>::READ_ONLY) )
	{
		if(CFlashGraphicsBuffer<CIndexBuffer>::IsActivate() && m_uStartDirty < m_uEndDirty)
			static_cast<CFlashContext3D&>( GetGraphicsResourcePool() ).SubmitRenderDataQueueToRender();

		m_uStartDirty = ALCHEMY_MININUM(uStart, m_uStartDirty);
		m_uEndDirty   = ALCHEMY_MAXINUM(uEnd  , m_uEndDirty);
	}

	if( CFlashGraphicsBuffer<CIndexBuffer>::IsNoData( CGraphicsBuffer<CIndexBuffer>::GetUsage() ) && !m_pData )
		ALCHEMY_DEBUG_NEW(m_pData, UINT8[CGraphicsBuffer<CIndexBuffer>::GetByteSize()]);

	return m_pData + uStart * CGraphicsBuffer<CIndexBuffer>::GetStride();
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
	if( !CFlashGraphicsBuffer<CIndexBuffer>::IsNoData( CGraphicsBuffer<CIndexBuffer>::GetUsage() ) )
		ALCHEMY_DEBUG_NEW(m_pData, UINT8[CGraphicsBuffer<CIndexBuffer>::GetByteSize()]);

	static_cast<CFlashContext3D&>( GetGraphicsResourcePool() ).CallBackToCreate( this, static_cast<const CFlashObjectInterface*>(this), GetType() );

	return true;
}

#ifdef AS3_H
AS3TYPE CFlashIndexBuffer::Init()
{
	return AS3_Array( "PtrType, PtrType, PtrType, IntType", &m_pData, &m_uStartDirty, &m_uEndDirty, GetMemberCount() );
}
#endif