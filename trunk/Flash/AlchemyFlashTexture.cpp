#include "AlchemyFlashStdafx.h"
#include "AlchemyFlashTexture.h"
#include "AlchemyFlashContext3D.h"

using namespace alchemy;

CFlashTexture::CFlashTexture(void) : 
m_pData(ALCHEMY_NULL),
m_uMipLevelDirtyFlag(0),
m_RenderTarget(0),
m_bIsDirty(false),
m_Filter(CEffectCode::NONE)
{
	for(UINT i = 0; i < TOTAL_MIP_MAP_LEVEL; ++ i)
		m_uCubeMapFaces[i] = 0;
}

CFlashTexture::~CFlashTexture(void)
{
}

void CFlashTexture::Reset()
{
	if( CFlashGraphicsBuffer<CTexture>::IsNoData( CGraphicsBuffer<CTexture>::GetUsage() ) )
		ALCHEMY_DEBUG_DELETE_ARRAY(m_pData);

	if(m_uMipLevelDirtyFlag)
	{
		UINT uEnd = MaxinumBit(m_uMipLevelDirtyFlag);
		for(UINT i = MininumBit(m_uMipLevelDirtyFlag) - 1; i < uEnd; ++ i)
			m_uCubeMapFaces[i] = 0;

		m_uMipLevelDirtyFlag = 0;
	}

	m_bIsDirty = false;

	CFlashGraphicsBuffer<CTexture>::Reset();
}

PVOID CFlashTexture::Lock(UINT uSurface, UINT uFlag, PUINT puRowPitch, PUINT puSlicePitch)
{
	if( ALCHEMY_TEST_BIT(GetUsage(), CTexture::RENDER_TARGET) )
		return false;

	UINT uMipLevel = uSurface & CTexture::MIP_LEVEL_MASK;
	if( uMipLevel >= GetMipLevel() )
		return ALCHEMY_NULL;

	UINT uOffset = 0, uWidth, uHeight, uDepth;

	for(UINT i = 0; i < uMipLevel; ++ i)
	{
		GetMipMapSize(i, &uWidth, &uHeight, &uDepth);

		uOffset += uWidth * uHeight * uDepth * CGraphicsBuffer<CTexture>::GetStride();
	}

	GetMipMapSize(uMipLevel, &uWidth, &uHeight);

	if(CTexture::GetTextureType() == CTexture::CUBE)
	{
		CUBEMAPFACE CubeMapFace = static_cast<CUBEMAPFACE>(uSurface >> CTexture::CUBE_TEXTURE_SURFACE_SHIFT);

		uOffset += CubeMapFace * CGraphicsBuffer<CTexture>::GetByteSize() / 6;

		if( !ALCHEMY_TEST_BIT(uFlag, CGraphicsBuffer<CTexture>::READ_ONLY) )
			ALCHEMY_SET_BIT(m_uCubeMapFaces[uMipLevel], CubeMapFace);
	}
	else if(puSlicePitch)
		*puSlicePitch = uWidth * uHeight;

	if(puRowPitch)
		*puRowPitch = uWidth;

	if( !ALCHEMY_TEST_BIT(uFlag, CGraphicsBuffer<CTexture>::READ_ONLY) )
	{
		if(IsActivate() && m_uMipLevelDirtyFlag)
			static_cast<CFlashContext3D&>( GetGraphicsResourcePool() ).SubmitRenderDataQueueToRender();

		ALCHEMY_SET_BIT(m_uMipLevelDirtyFlag, uMipLevel);
	}

	if( CFlashGraphicsBuffer<CTexture>::IsNoData( CGraphicsBuffer<CTexture>::GetUsage() ) && !m_pData )
		ALCHEMY_DEBUG_NEW(m_pData, UINT8[CGraphicsBuffer<CTexture>::GetByteSize()]);

	return m_pData + uOffset;
}

bool  CFlashTexture::Unlock()
{
	return true;
}

void CFlashTexture::Destroy()
{
	ALCHEMY_DEBUG_DELETE_ARRAY(m_pData);
}

bool CFlashTexture::_Create()
{
	if( !CFlashGraphicsBuffer<CTexture>::IsNoData( CGraphicsBuffer<CTexture>::GetUsage() ) )
		ALCHEMY_DEBUG_NEW(m_pData, UINT8[CGraphicsBuffer<CTexture>::GetByteSize()]);

	static_cast<CFlashContext3D&>( GetGraphicsResourcePool() ).CallBackToCreate( this, static_cast<const CFlashObjectInterface*>(this), GetType() );

	return true;
}

#ifdef AS3_H
AS3TYPE CFlashTexture::Init()
{
	return AS3_Array("IntType, IntType, IntType, IntType, IntType, IntType, IntType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType", GetTextureType(), GetWidthShift(), GetHeightShift(), GetDepthShift(), GetForamt(), GetUsage(), GetMipLevel(), &m_uMipLevelDirtyFlag, &m_pData, m_uCubeMapFaces, &m_bIsDirty, &m_RenderTarget, &m_Filter);
}
#endif