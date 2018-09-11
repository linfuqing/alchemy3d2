#include "AlchemyD3D9Texture.h"
#include "AlchemyD3D9Device.h"

#include <d3d9.h>

#pragma comment(lib,"d3d9.lib")

using namespace alchemy;

CD3D9Texture::CD3D9Texture(void) : 
m_pD3D9Texture(ALCHEMY_NULL), 
m_pData(ALCHEMY_NULL), 
m_pLockData(ALCHEMY_NULL), 
m_uLockSurface(0), 
m_uLockFlag(0), 
m_uCubeMapSize(0)
{
}

CD3D9Texture::~CD3D9Texture(void)
{
	if(m_pD3D9Texture)
		m_pD3D9Texture->Release();

	ALCHEMY_DEBUG_DELETE_ARRAY(m_pData);
}

void* CD3D9Texture::Lock(alchemy::UINT uSurface, alchemy::UINT uFlag, alchemy::PUINT puRowPitch, alchemy::PUINT puSlicePitch)
{
	if(!m_pD3D9Texture)
		return ALCHEMY_NULL;

	m_uLockSurface = uSurface;
	m_uLockFlag = uFlag;

	m_pLockData = ALCHEMY_NULL;

	if( ALCHEMY_TEST_BIT(GetUsage(), RENDER_TARGET) && ALCHEMY_TEST_BIT(uFlag, READ_ONLY) )
	{
		if(m_pData)
			return reinterpret_cast<PUINT8>(m_pData) + __ComputeDataOffset(uSurface);
		else
		{
			ALCHEMY_DEBUG_WARNING("CD3D9Texture::Lock: Texture is not be upload.");

			return ALCHEMY_NULL;
		}
	}

	TEXTURETYPE Type = GetTextureType();
	switch(Type)
	{
	case NORMAL:
		D3DLOCKED_RECT Rect;
		if(static_cast<IDirect3DTexture9*>(m_pD3D9Texture)->LockRect( uSurface & MIP_LEVEL_MASK, &Rect, ALCHEMY_NULL, ALCHEMY_TEST_BIT(uFlag, READ_ONLY) ? D3DLOCK_READONLY : 0) == D3D_OK)
		{
			m_pLockData = Rect.pBits;

			if(puRowPitch)
				*puRowPitch = Rect.Pitch;
		}

		if(puSlicePitch)
			*puSlicePitch = 0;

		break;
	case CUBE:
		D3DLOCKED_RECT CubeRect;
		if(static_cast<IDirect3DCubeTexture9*>(m_pD3D9Texture)->LockRect(static_cast<D3DCUBEMAP_FACES>(uSurface >> CUBE_TEXTURE_SURFACE_SHIFT), uSurface & MIP_LEVEL_MASK, &CubeRect, ALCHEMY_NULL, ALCHEMY_TEST_BIT(uFlag, READ_ONLY) ? D3DLOCK_READONLY : 0) == D3D_OK)
		{
			m_pLockData = CubeRect.pBits;

			if(puRowPitch)
				*puRowPitch = CubeRect.Pitch;
		}

		if(puSlicePitch)
			*puSlicePitch = 0;

		break;
	case VOLUME:
		D3DLOCKED_BOX VolumeBox;
		if(static_cast<IDirect3DVolumeTexture9*>(m_pD3D9Texture)->LockBox(uSurface & MIP_LEVEL_MASK, &VolumeBox, ALCHEMY_NULL, ALCHEMY_TEST_BIT(uFlag, READ_ONLY) ? D3DLOCK_READONLY : 0) == D3D_OK)
		{
			m_pLockData = VolumeBox.pBits;

			if(puRowPitch)
				*puRowPitch = VolumeBox.RowPitch;

			if(puSlicePitch)
				*puSlicePitch = VolumeBox.SlicePitch;
		}
		break;
	default:
		break;
	}

	return m_pLockData;
}

bool  CD3D9Texture::Unlock()
{
	UINT uMipLevel = m_uLockSurface & MIP_LEVEL_MASK;
	if( m_pLockData && ALCHEMY_TEST_BIT(GetUsage(), RENDER_TARGET) && !ALCHEMY_TEST_BIT(m_uLockFlag, READ_ONLY) )
	{
		UINT uWidth, uHeight, uDepth;
		if( GetMipMapSize(uMipLevel, &uWidth, &uHeight, &uDepth) )
		{
			if(!m_pData)
				ALCHEMY_DEBUG_NEW(m_pData, UINT8[GetByteSize()]);
		
			ALCHEMY_MEMORY_COPY( m_pData + __ComputeDataOffset(m_uLockSurface), m_pLockData, uWidth * uHeight * uDepth * GetStride() );
		}
	}

	TEXTURETYPE Type = GetTextureType();
	switch(Type)
	{
	case NORMAL:
		return static_cast<IDirect3DTexture9*>(m_pD3D9Texture)->UnlockRect(uMipLevel) == D3D_OK;
		break;
	case CUBE:
		return static_cast<IDirect3DCubeTexture9*>(m_pD3D9Texture)->UnlockRect(static_cast<D3DCUBEMAP_FACES>(m_uLockSurface >> CUBE_TEXTURE_SURFACE_SHIFT), uMipLevel) == D3D_OK;
		break;
	case VOLUME:
		return static_cast<IDirect3DTexture9*>(m_pD3D9Texture)->UnlockRect(uMipLevel) == D3D_OK;
		break;
	default:
		break;
	}

	m_pLockData = ALCHEMY_NULL;

	return false;
}

void CD3D9Texture::Destroy()
{
	if(m_pD3D9Texture)
	{
		m_pD3D9Texture->Release();

		m_pD3D9Texture = ALCHEMY_NULL;
	}

	ALCHEMY_DEBUG_DELETE_ARRAY(m_pData);

	m_pLockData = 0;

	m_uLockSurface = 0;

	m_uLockFlag = 0;

	m_uCubeMapSize = 0;
}

bool CD3D9Texture::_Create()
{
	IDirect3DDevice9* pDevice = static_cast<CD3D9Device&>( GetGraphicsResourcePool() ).GetDevice();
	if(pDevice)
	{
		FORMAT Format = GetFormat();
		if(Format)
		{
			if(m_pD3D9Texture)
				m_pD3D9Texture->Release();

			static D3DFORMAT s_FormatTable[] = 
			{
				D3DFMT_A8R8G8B8, 
				D3DFMT_R32F, 
				D3DFMT_A32B32G32R32F
			};

			TEXTURETYPE Type = GetTextureType();
			bool bIsRenderTarget = ALCHEMY_TEST_BIT(GetUsage(), RENDER_TARGET);
			UINT uUsage = bIsRenderTarget ? D3DUSAGE_RENDERTARGET : 0;
			D3DPOOL Pool = bIsRenderTarget ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED;
			switch(Type)
			{
			case NORMAL:
				IDirect3DTexture9* pTexture;
				if( pDevice->CreateTexture(ALCHEMY_FLAG( GetWidthShift() ), ALCHEMY_FLAG( GetHeightShift() ), GetMipLevel(), uUsage, s_FormatTable[Format - 1], Pool, &pTexture, ALCHEMY_NULL) );
				{
					m_pD3D9Texture = pTexture;

					return true;
				}
				break;

			case CUBE:
				IDirect3DCubeTexture9* pCubeTexture;
				if(pDevice->CreateCubeTexture(ALCHEMY_FLAG( GetWidthShift() ), GetMipLevel(), uUsage, s_FormatTable[Format - 1], Pool, &pCubeTexture, ALCHEMY_NULL) == D3D_OK)
				{
					m_pD3D9Texture = pCubeTexture;

					return true;
				}
				break;

			case VOLUME:
				IDirect3DVolumeTexture9* pVolumeTexture;
				if(pDevice->CreateVolumeTexture(ALCHEMY_FLAG( GetWidthShift() ), ALCHEMY_FLAG( GetHeightShift() ), ALCHEMY_FLAG( GetDepthShift() ), GetMipLevel(), uUsage, s_FormatTable[Format - 1], Pool, &pVolumeTexture, ALCHEMY_NULL) == D3D_OK)
				{
					m_pD3D9Texture = pVolumeTexture;

					return true;
				}
				break;

			default:
				break;
			}
		}
	}

	return false;
}

alchemy::UINT CD3D9Texture::__ComputeDataOffset(alchemy::UINT uSurface)
{
	alchemy::UINT uWidth, uHeight, uDepth, uOffset = 0, uStride = GetStride(), uLevel = uSurface & MIP_LEVEL_MASK;
	for(alchemy::UINT i = 0; i < uLevel; ++ i)
	{
		if( GetMipMapSize(i, &uWidth, &uHeight, &uDepth) )
			uOffset += uWidth * uHeight * uDepth * uStride;
		else
			break;
	}

	CUBEMAPFACE CubeMapFace = static_cast<CUBEMAPFACE>(uSurface >> CUBE_TEXTURE_SURFACE_SHIFT);
	if(CubeMapFace)
	{
		if(!m_uCubeMapSize)
		{
			alchemy::UINT uMipLevel = GetMipLevel();
			for(alchemy::UINT i = 0; i < uMipLevel; ++ i)
			{
				if( GetMipMapSize(i, &uWidth, &uHeight, &uDepth) )
					m_uCubeMapSize += uWidth * uHeight * uDepth * uStride;
				else
					break;
			}
		}

		uOffset += m_uCubeMapSize * CubeMapFace;
	}

	return uOffset;
}