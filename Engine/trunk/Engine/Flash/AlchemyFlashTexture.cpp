#include "AlchemyFlashStdafx.h"
#include "AlchemyFlashTexture.h"
#include "AlchemyFlashContext3D.h"

using namespace alchemy;

CFlashTexture::CFlashTexture(void) : 
m_pData(ALCHEMY_NULL),
m_uMipLevelDirtyFlag(0)
{
	for(UINT i = 0; i < TOTAL_MIP_MAP_LEVEL; ++ i)
		m_uCubeMapFaces[i] = 0;
}

CFlashTexture::~CFlashTexture(void)
{
}

void CFlashTexture::Reset()
{
	if( IsNoData( GetUsage() ) )
		ALCHEMY_DEBUG_DELETE_ARRAY(m_pData);

	if(m_uMipLevelDirtyFlag)
	{
		UINT uEnd = MaxinumBit(m_uMipLevelDirtyFlag);
		for(UINT i = MininumBit(m_uMipLevelDirtyFlag) - 1; i < uEnd; ++ i)
			m_uCubeMapFaces[i] = 0;

		m_uMipLevelDirtyFlag = 0;
	}

	CFlashGraphicsBuffer<CTexture>::Reset();
}

void* CFlashTexture::Lock(UINT uSurface, UINT uFlag, PUINT puRowPitch, PUINT puSlicePitch)
{
	if( ALCHEMY_TEST_BIT(GetUsage(), CTexture::RENDER_TARGET) )
		return ALCHEMY_NULL;

	UINT uMipLevel = uSurface & CTexture::MIP_LEVEL_MASK;
	if( uMipLevel >= GetMipLevel() )
		return ALCHEMY_NULL;

	UINT uOffset = 0, uWidth, uHeight, uDepth;

	for(UINT i = 0; i < uMipLevel; ++ i)
	{
		GetMipMapSize(i, &uWidth, &uHeight, &uDepth);

		uOffset += uWidth * uHeight * uDepth * GetStride();
	}

	GetMipMapSize(uMipLevel, &uWidth, &uHeight);

	if(CTexture::GetTextureType() == CTexture::CUBE)
	{
		CUBEMAPFACE CubeMapFace = static_cast<CUBEMAPFACE>(uSurface >> CTexture::CUBE_TEXTURE_SURFACE_SHIFT);

		uOffset += CubeMapFace * GetByteSize() / 6;

		if( !ALCHEMY_TEST_BIT(uFlag, CGraphicsBuffer<CTexture>::READ_ONLY) )
			ALCHEMY_SET_BIT(m_uCubeMapFaces[uMipLevel], CubeMapFace);
	}
	else if(puSlicePitch)
		*puSlicePitch = uWidth * uHeight * GetStride();

	if(puRowPitch)
		*puRowPitch = uWidth * GetStride();

	if( !ALCHEMY_TEST_BIT(uFlag, CGraphicsBuffer<CTexture>::READ_ONLY) )
	{
		if(IsActivate() && m_uMipLevelDirtyFlag)
			static_cast<CFlashContext3D&>( GetGraphicsResourcePool() ).SubmitRenderDataQueueToRender(); 

		ALCHEMY_SET_BIT(m_uMipLevelDirtyFlag, uMipLevel);
	}

	if( IsNoData( GetUsage() ) && !m_pData )
		ALCHEMY_DEBUG_NEW(m_pData, UINT8[GetByteSize()]);

	return m_pData + uOffset;
}

bool  CFlashTexture::Unlock()
{
	return true;
}

void CFlashTexture::Destroy()
{
	ALCHEMY_DEBUG_DELETE_ARRAY(m_pData);

	CTexture::Destroy();
}

bool CFlashTexture::_Create()
{
	if( !IsNoData( GetUsage() ) )
		ALCHEMY_DEBUG_NEW(m_pData, UINT8[GetByteSize()]);

	static_cast<CFlashContext3D&>( GetGraphicsResourcePool() ).CallBackToCreate( this, static_cast<const CFlashObjectInterface*>(this), GetType(), GetResourceHandle() );

	return true;
}

#ifdef AS3_H
AS3TYPE CFlashTexture::Init()
{
	return AS3_Array("IntType, IntType, IntType, IntType, IntType, IntType, IntType, PtrType, PtrType, PtrType", GetTextureType(), GetWidthShift(), GetHeightShift(), GetDepthShift(), GetFormat(), GetUsage(), GetMipLevel(), &m_uMipLevelDirtyFlag, &m_pData, m_uCubeMapFaces);
}
#endif