#include "AlchemyFlasccStdafx.h"
#include "AlchemyFlasccTexture.h"
#include "AlchemyFlasccContext3D.h"

using namespace alchemy;

CFlasccTexture::CFlasccTexture(void) :
m_pData(ALCHEMY_NULL), 
m_uLockFlag(0)
{
}

CFlasccTexture::~CFlasccTexture(void)
{
}

void* CFlasccTexture::Lock(UINT uSurface, UINT uFlag, PUINT puRowPitch, PUINT puSlicePitch)
{
	if( ALCHEMY_TEST_BIT(GetUsage(), CTexture::RENDER_TARGET) )
		return ALCHEMY_NULL;

	UINT uMipLevel = uSurface & CTexture::MIP_LEVEL_MASK;
	if( uMipLevel >= GetMipLevel() )
		return ALCHEMY_NULL;

	m_uLockSurface = uSurface;
	m_uLockFlag = uFlag;

	UINT uWidth, uHeight, uDepth;
	GetMipMapSize(uMipLevel, &uWidth, &uHeight, &uDepth);

	UINT uStride = GetStride();

	if(puRowPitch)
		*puRowPitch = uWidth * uStride;

	if(puSlicePitch)
		*puSlicePitch = uWidth * uHeight * uStride;

	UINT uOffset = 0, uUsage = GetUsage();
	if( !ALCHEMY_TEST_BIT(uUsage, TEXTURE::WRITE_ONLY) || ALCHEMY_TEST_BIT(uUsage, TEXTURE::DYNAMIC) )
	{
		for(UINT i = 0; i < uMipLevel; ++ i)
		{
			GetMipMapSize(i, &uWidth, &uHeight, &uDepth);

			uOffset += uWidth * uHeight * uDepth * uStride;
		}

		if(CTexture::GetTextureType() == CTexture::CUBE)
		{
			CUBEMAPFACE CubeMapFace = static_cast<CUBEMAPFACE>(uSurface >> CTexture::CUBE_TEXTURE_SURFACE_SHIFT);

			uOffset += CubeMapFace * GetByteSize() / 6;
		}

		if(!m_pData)
		{
			if( ALCHEMY_TEST_BIT(uFlag, TEXTURE::READ_ONLY) )
				return ALCHEMY_NULL;

			ALCHEMY_DEBUG_NEW(m_pData, UINT8[GetByteSize()]);
		}
	}
	else
	{
		if( ALCHEMY_TEST_BIT(uFlag, TEXTURE::READ_ONLY) )
			return ALCHEMY_NULL;

		ALCHEMY_DEBUG_DELETE_ARRAY(m_pData);
		ALCHEMY_DEBUG_NEW(m_pData, UINT8[uWidth * uHeight * uDepth * uStride]);
	}

	m_uLockOffset = uOffset;

	return reinterpret_cast<PUINT8>(m_pData) + uOffset;
}

bool  CFlasccTexture::Unlock()
{
	if( ALCHEMY_TEST_BIT(m_uLockFlag, TEXTURE::READ_ONLY) )
		return true;

	if(!m_pData)
		return false;

	UINT uUsage = GetUsage();
#ifdef AS3_H
	inline_as3(
		"import alchemy.core.AlchemyGraphicsManager;\n"
		"import alchemy.core.AlchemyRenderSystem;\n"
		"var renderSystem = AlchemyGraphicsManager.instance.getRenderSystem(%0);\n"
		"if(renderSystem) renderSystem.uploadTexture(%1, %2, ram, %3, %4);\n"
		: : "r"( static_cast<CFlasccContext3D&>( GetGraphicsResourcePool() ).GetRenderSystemIndex() ),
		"r"( static_cast<unsigned int>( GetResourceHandle() ) ), 
		"r"( GetTextureType() ),
		"r"(reinterpret_cast<PUINT8>(m_pData) + m_uLockOffset), 
		"r"(m_uLockSurface)
		);
#endif

	m_uLockSurface = 0;
	m_uLockFlag = 0;
	m_uLockOffset = 0;

	if( ALCHEMY_TEST_BIT(uUsage, TEXTURE::WRITE_ONLY) && !ALCHEMY_TEST_BIT(uUsage, TEXTURE::DYNAMIC) )
		ALCHEMY_DEBUG_DELETE_ARRAY(m_pData);

	return true;
}

void CFlasccTexture::Destroy()
{
	ALCHEMY_DEBUG_DELETE_ARRAY(m_pData);

	CTexture::Destroy();
}

bool CFlasccTexture::_Create()
{
#ifdef AS3_H
	bool bIsOptimizeForRenderToTexture = ALCHEMY_TEST_BIT(GetUsage(), CTexture::RENDER_TARGET);
	UINT uWidth = GetWidth();
	UINT uHeight = GetHeight();

	uWidth = ALCHEMY_FLAG( GetWidthShift() );
	uHeight = ALCHEMY_FLAG( GetHeightShift() );

	inline_as3(
		"import alchemy.core.AlchemyGraphicsManager;\n"
		"import alchemy.core.AlchemyRenderSystem;\n"
		"var renderSystem = AlchemyGraphicsManager.instance.getRenderSystem(%0);\n"
		"if(renderSystem) renderSystem.createTexture(%1, %2, %3, %4, %5, %6);\n"
		: : "r"( static_cast<CFlasccContext3D&>( GetGraphicsResourcePool() ).GetRenderSystemIndex() ), "r"( GetTextureType() ), "r"(uWidth), "r"(uHeight), "r"( GetFormat() ), "r"(bIsOptimizeForRenderToTexture), "r"( static_cast<UINT>( GetResourceHandle() ) )
		);
#endif
	return true;
}
