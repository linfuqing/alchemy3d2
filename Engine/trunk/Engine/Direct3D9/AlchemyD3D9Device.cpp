#include "AlchemyD3D9Device.h"

#include "AlchemyD3D9VertexDeclaration.h"

#include "AlchemyD3D9VertexBuffer.h"
#include "AlchemyD3D9IndexBuffer.h"

#include "AlchemyD3D9Texture.h"

#include "AlchemyD3D9Shader.h"
#include "AlchemyD3D9Effect.h"

#include <d3d9.h>
#include <d3dx9.h>

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")

using namespace alchemy;

CD3D9Device::CD3D9Device(HWND hWindow, bool bIsWindowed, alchemy::UINT uBackBufferWidth, alchemy::UINT uBackBufferHeight, alchemy::INT nAntiAliasing) :
m_ViewportWidth(0), 
m_ViewportHeight(0), 
m_pDirect( Direct3DCreate9(D3D_SDK_VERSION) ), 
m_pBackgroundSurface(ALCHEMY_NULL), 
m_pCompiler(ALCHEMY_NULL), 
m_uNumVertex(0), 
m_bIsBeginScene(false)
{
	ALCHEMY_DEBUG_ASSERT(m_pDirect, "Direct3D 9 has been created fail.");

	D3DMULTISAMPLE_TYPE MultiSampleType = D3DMULTISAMPLE_NONMASKABLE;
	DWORD dwQuality = 0;
	if(m_pDirect->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_A8R8G8B8, bIsWindowed, D3DMULTISAMPLE_NONMASKABLE, &dwQuality) != D3D_OK || dwQuality < nAntiAliasing)
		MultiSampleType = D3DMULTISAMPLE_NONE;

	RECT Rect;
	GetWindowRect(hWindow, &Rect);

	m_ViewportWidth = Rect.right - Rect.left;
	m_ViewportHeight = Rect.bottom - Rect.top;

	D3DPRESENT_PARAMETERS PresentParameters;
	PresentParameters.BackBufferWidth = uBackBufferWidth ? uBackBufferWidth : m_ViewportWidth;
	PresentParameters.BackBufferHeight = uBackBufferHeight ? uBackBufferHeight : m_ViewportHeight;
	PresentParameters.BackBufferFormat = D3DFMT_A8R8G8B8;
	PresentParameters.BackBufferCount = 1;
	PresentParameters.MultiSampleType = MultiSampleType;
	PresentParameters.MultiSampleQuality = nAntiAliasing;
	PresentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	PresentParameters.hDeviceWindow = hWindow;
	PresentParameters.Windowed = bIsWindowed ? TRUE :FALSE;
	PresentParameters.EnableAutoDepthStencil = TRUE;
	PresentParameters.AutoDepthStencilFormat = D3DFMT_D24S8;
	PresentParameters.Flags = 0;
	PresentParameters.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	PresentParameters.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;//D3DPRESENT_INTERVAL_IMMEDIATE;

	ALCHEMY_DEBUG_ASSERT(m_pDirect->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWindow, D3DCREATE_HARDWARE_VERTEXPROCESSING, &PresentParameters, &m_pDevice) == D3D_OK, "Direct3D 9 device has been created fail.");

	D3DCAPS9 Caps;
	ALCHEMY_DEBUG_ASSERT(m_pDirect->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &Caps) == S_OK, "Caps has been get fail.");

	m_RenderSystemInfo.uMaxVertexCount      = m_pDirect->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, 0, D3DRTYPE_INDEXBUFFER, D3DFMT_INDEX32) ? 0xffffffff : 0xffff;
	m_RenderSystemInfo.uMaxVertexIndexCount = Caps.MaxVertexIndex;
	m_RenderSystemInfo.uRenderTargetCount   = Caps.NumSimultaneousRTs;

	alchemy::UINT uConstantBufferSize = Caps.MaxTextureWidth * Caps.MaxTextureHeight;

	ALCHEMY_DEBUG_NEW( m_pCompiler, CHLSLCompiler(Caps.NumSimultaneousRTs, uConstantBufferSize) );
}

CD3D9Device::~CD3D9Device(void)
{
	if(m_pDirect)
		m_pDirect->Release();

	if(m_pDevice)
		m_pDevice->Release();

	if(m_pBackgroundSurface)
		m_pBackgroundSurface->Release();

	ALCHEMY_DEBUG_DELETE(m_pCompiler);
}

bool CD3D9Device::CheckFormat(FORMATTYPE Type, alchemy::UINT uFormat)
{
	if(!m_pDirect)
		return false;

	switch(Type)
	{
	case CRenderSystem::TEXTURE:
		switch(uFormat)
		{
		case CTexture::DEFAULT:
			return m_pDirect->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, 0, D3DRTYPE_TEXTURE, D3DFMT_A8R8G8B8) == D3D_OK;
			break;
		case CTexture::FLOAT1:
			return m_pDirect->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, 0, D3DRTYPE_TEXTURE, D3DFMT_R32F) == D3D_OK;
			break;
		case CTexture::FLOAT4:
			return m_pDirect->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, 0, D3DRTYPE_TEXTURE, D3DFMT_A32B32G32R32F) == D3D_OK;
			break;
		}
		break;

	case CRenderSystem::DEPTH_STENCIL:
		return false;
		break;

	case CRenderSystem::INDEX_BUFFER:
		switch(uFormat)
		{
		case CIndexBuffer::BIT16:
			return m_pDirect->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, 0, D3DRTYPE_INDEXBUFFER, D3DFMT_INDEX16) == D3D_OK;
			break;
		case CIndexBuffer::BIT32:
			return m_pDirect->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, 0, D3DRTYPE_INDEXBUFFER, D3DFMT_INDEX32) == D3D_OK;
			break;
		}
		break;
	}
	return false;
}

ICompiler& CD3D9Device::GetCompiler()
{
	ALCHEMY_DEBUG_ASSERT(m_pCompiler, "HLSL Compiler is NULL.");

	return *m_pCompiler;
}

bool CD3D9Device::GetViewport(alchemy::PUINT puWidth, alchemy::PUINT puHeight)
{
	if(puWidth)
		*puWidth = m_ViewportWidth;

	if(puHeight)
		*puHeight = m_ViewportHeight;

	return true;
}

LPTEXTURE CD3D9Device::CreateTextureFromFile(const alchemy::CHAR* pcPath)
{
	D3DXIMAGE_INFO ImageInfo;
	if(D3DXGetImageInfoFromFile(pcPath, &ImageInfo) == D3D_OK)
	{
		IDirect3DTexture9* pTexture = ALCHEMY_NULL;
		if(D3DXCreateTextureFromFileEx(m_pDevice, pcPath, ImageInfo.Width, ImageInfo.Height, 1, 0, ImageInfo.Format, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_FILTER_NONE, 0, ALCHEMY_NULL, ALCHEMY_NULL, &pTexture) == D3D_OK && pTexture)
		{
			D3DLOCKED_RECT LockedRect;
			if(pTexture->LockRect(0, &LockedRect, ALCHEMY_NULL, D3DLOCK_READONLY) == D3D_OK)
			{
				CTexture::FORMAT Format = CTexture::UNKNOWN;
				if(ImageInfo.Format == D3DFMT_A8B8G8R8)
					Format = CTexture::DEFAULT;
				else if(ImageInfo.Format == D3DFMT_R32F)
					Format = CTexture::FLOAT1;
				else if(ImageInfo.Format == D3DFMT_A32B32G32R32F)
					Format = CTexture::FLOAT4;

				alchemy::UINT uStride = CTexture::GetFormatSize(Format);
				if(!uStride)
				{
					pTexture->Release();

					return ALCHEMY_NULL;
				}

				alchemy::UINT uPitch = uStride * ImageInfo.Width;
				void* pData = LockedRect.pBits;
				if(LockedRect.Pitch != uPitch)
				{
					alchemy::UINT uSize = uPitch * ImageInfo.Height;
					alchemy::PUINT8 puSource = reinterpret_cast<PUINT8>(pData), puDestination;
					ALCHEMY_DEBUG_NEW(puDestination, UINT8[uSize]);

					pData = puDestination;

					for(UINT i = 0; i < ImageInfo.Height; ++ i)
					{
						ALCHEMY_MEMORY_COPY(puDestination, puSource, uPitch);

						puDestination += uPitch;
						puSource      += LockedRect.Pitch;
					}
				}

				LPTEXTURE pOutput = CreateTexture(ImageInfo.Width, ImageInfo.Height, 0, Format, ImageInfo.MipLevels, pData);

				if(LockedRect.Pitch != uPitch)
					ALCHEMY_DEBUG_DELETE_ARRAY(pData);

				pTexture->Release();

				return pOutput;
			}
			else
				ALCHEMY_DEBUG_WARNING("CD3D9Device::CreateTextureFromFile: D3D9 Texture lock rect has been fail.");

			pTexture->Release();
		}
		else
			ALCHEMY_DEBUG_WARNING("CD3D9Device::CreateTextureFromFile: D3D9 texture has been created fail.");
	}
	else
		ALCHEMY_DEBUG_WARNING("CD3D9Device::CreateTextureFromFile: D3D9 image info has been get fail.");

	return ALCHEMY_NULL;
}

bool CD3D9Device::Clear(ARGBCOLOR Color, alchemy::UINT uFlag)
{
	if(m_pDevice)
	{
		UINT uClearFlag = 0;
		if( ALCHEMY_TEST_BIT(uFlag, CRenderSystem::TARGET) )
			uClearFlag |= D3DCLEAR_TARGET;

		if( ALCHEMY_TEST_BIT(uFlag, CRenderSystem::ZBUFFER) )
			uClearFlag |= D3DCLEAR_ZBUFFER;

		if( ALCHEMY_TEST_BIT(uFlag, CRenderSystem::STENCIL) )
			uClearFlag |= D3DCLEAR_STENCIL;

		return m_pDevice->Clear(0, ALCHEMY_NULL, uClearFlag, Color, 1.0f, 0) == S_OK;
	}

	return false;
}

bool CD3D9Device::ResetTexture()
{
	return true;
}

bool CD3D9Device::RegisterRenderState()
{
	m_uRenderStateFlag = 0;

	m_uRenderStateStackCount = 0;

	return true;
}

bool CD3D9Device::RestoreRenderState()
{
	IRenderState::RENDERSTATE State;
	UINT uValue;
	for(UINT i = 0; i < m_uRenderStateStackCount; ++ i)
	{
		State  = m_RenderStateStack[i];
		uValue = m_RenderStateValueStack[i];

		m_uRenderState[State] = uValue;

		if(! __SetRenderState(State, uValue) )
			return false;
	}

	m_uRenderStateFlag = 0;

	m_uRenderStateStackCount = 0;

	return true;
}

bool CD3D9Device::SetRenderState(const RENDERSTATEINFO* pRenderStateInfos, alchemy::UINT uRenderStateInfoCount, bool bIsRegister)
{
	if(bIsRegister)
	{
		m_uRenderStateFlag = 0;

		m_uRenderStateStackCount = 0;
	}

	const RENDERSTATEINFO* pRenderStateInfo;
	for(UINT i = 0; i < uRenderStateInfoCount; ++ i)
	{
		pRenderStateInfo = &pRenderStateInfos[i];
		if(pRenderStateInfo->State < TOTAL_RENDER_STATE)
		{
			if( !ALCHEMY_TEST_BIT(m_uRenderStateFlag, pRenderStateInfo->State) )
			{
				m_RenderStateStack[m_uRenderStateStackCount] = pRenderStateInfo->State;

				m_RenderStateValueStack[m_uRenderStateStackCount] = m_uRenderState[pRenderStateInfo->State];

				ALCHEMY_SET_BIT(m_uRenderStateFlag, pRenderStateInfo->State);

				++ m_uRenderStateStackCount;
			}

			m_uRenderState[pRenderStateInfo->State] = pRenderStateInfo->uValue;

			if( !__SetRenderState(pRenderStateInfo->State, pRenderStateInfo->uValue) )
				return false;
		}
		else
		{
			ALCHEMY_DEBUG_WARNING("CD3D9Device::SetRenderState: Error state.");

			return false;
		}
	}

	return true;
}

bool CD3D9Device::SetRenderTarget(CGraphicsResource::HANDLE Texture, alchemy::UINT uSurface, alchemy::UINT uIndex)
{
	if(!m_pDevice)
		return false;

	CGraphicsResource* pResource = GetResource(Texture, CGraphicsResource::TEXTURE);
	if(pResource)
	{
		CD3D9Texture* pTexture = static_cast<CD3D9Texture*>(pResource);
		IDirect3DBaseTexture9* pInstance = pTexture->GetInstance();
		if(pInstance)
		{
			IDirect3DSurface9* pSurface = ALCHEMY_NULL;

			HRESULT hr = S_FALSE;

			CTexture::TEXTURETYPE Type = pTexture->GetTextureType();
			switch(Type)
			{
			case CTexture::NORMAL:
				static_cast<IDirect3DTexture9*>(pInstance)->GetSurfaceLevel(uSurface & CTexture::MIP_LEVEL_MASK, &pSurface);
				break;
			case CTexture::CUBE:
				static_cast<IDirect3DCubeTexture9*>(pInstance)->GetCubeMapSurface(static_cast<D3DCUBEMAP_FACES>(uSurface >> CTexture::CUBE_TEXTURE_SURFACE_SHIFT), uSurface & CTexture::MIP_LEVEL_MASK, &pSurface);
				break;
			case CTexture::VOLUME:
				static_cast<IDirect3DTexture9*>(pInstance)->GetSurfaceLevel(uSurface & CTexture::MIP_LEVEL_MASK, &pSurface);
				break;
			default:
				break;
			}

			if(hr == D3D_OK && pSurface)
			{
				if(!m_pBackgroundSurface)
					m_pDevice->GetRenderTarget(0, &m_pBackgroundSurface);

				if(m_pDevice->SetRenderTarget(uIndex, pSurface) == D3D_OK)
				{
					pSurface->Release();

					return true;
				}

				pSurface->Release();
			}
		}
	}
	else
	{
		if(m_pBackgroundSurface)
		{
			if(m_pDevice->SetRenderTarget(uIndex, m_pBackgroundSurface) == S_OK)
				return true;

			return false;
		}

		return true;
	}

	return false;
}

bool CD3D9Device::SetVertexDeclaration(CGraphicsResource::HANDLE VertexDeclaration)
{
	if(!m_pDevice)
		return false;

	CGraphicsResource* pResource = GetResource(VertexDeclaration, CGraphicsResource::VERTEX_DECLARATION);
	if(pResource)
	{
		IDirect3DVertexDeclaration9* pInstance = static_cast<CD3D9VertexDeclaration*>(pResource)->GetInstance();
		if(pInstance)
			return m_pDevice->SetVertexDeclaration(pInstance) == D3D_OK;
	}

	return false;
}

bool CD3D9Device::SetStreamSource(CGraphicsResource::HANDLE VertexBuffer, alchemy::UINT uStream)
{
	if(!m_pDevice)
		return false;

	CGraphicsResource* pResource = GetResource(VertexBuffer, CGraphicsResource::VERTEX_BUFFER);
	if(pResource)
	{
		CD3D9VertexBuffer* pVertexBuffer = static_cast<CD3D9VertexBuffer*>(pResource);
		IDirect3DVertexBuffer9* pInstance = pVertexBuffer->GetInstance();
		if(pInstance && m_pDevice->SetStreamSource( uStream, pInstance, 0, pVertexBuffer->GetStride() ) == D3D_OK)
		{
			m_uNumVertex = pVertexBuffer->GetMemberCount();

			return true;
		}
	}

	return false;
}

bool CD3D9Device::SetIndices(CGraphicsResource::HANDLE IndexBuffer)
{
	if(!m_pDevice)
		return false;

	CGraphicsResource* pResource = GetResource(IndexBuffer, CGraphicsResource::INDEX_BUFFER);
	if(pResource)
	{
		IDirect3DIndexBuffer9* pInstance = static_cast<CD3D9IndexBuffer*>(pResource)->GetInstance();
		if(pInstance)
			return m_pDevice->SetIndices(pInstance) == D3D_OK;
	}

	return false;
}

bool CD3D9Device::SetShader(CGraphicsResource::HANDLE Shader)
{
	if(!m_pDevice)
		return false;

	CGraphicsResource* pResource = GetResource(Shader, CGraphicsResource::SHADER);
	if(pResource)
	{
		CD3D9Shader* pShader = static_cast<CD3D9Shader*>(pResource);
		const CD3D9Shader::SHADER& Instance = pShader->GetInstance();

		IEffect::EFFECTTYPE EffectType = pShader->GetShaderType();
		if(ALCHEMY_TEST_FLAG(EffectType, IEffect::VERTEX_SHADER) && m_pDevice->SetVertexShader(Instance.pVertexShader) != S_OK)
			return false;

		if(ALCHEMY_TEST_FLAG(EffectType, IEffect::FRAGMENT_SHADER) && m_pDevice->SetPixelShader(Instance.pPixelShader) != S_OK)
			return false;

		return true;
	}

	return false;
}

bool CD3D9Device::SetShaderParameters(EFFECTTYPE Type, alchemy::UINT uIndex, const alchemy::FLOAT* pfValues, alchemy::UINT uCount)
{
	if(!m_pDevice)
		return false;

	uCount = (uCount + 3) >> 2;

	if(ALCHEMY_TEST_FLAG(Type, IEffect::VERTEX_SHADER) && m_pDevice->SetVertexShaderConstantF(uIndex, pfValues, uCount) != D3D_OK)
		return false;

	if(ALCHEMY_TEST_FLAG(Type, IEffect::FRAGMENT_SHADER) && m_pDevice->SetPixelShaderConstantF(uIndex, pfValues, uCount) != D3D_OK)
		return false;

	return true;
}

bool CD3D9Device::SetShaderMatrixArray(EFFECTTYPE Type, alchemy::UINT uIndex, const alchemy::FLOAT* pfValues, alchemy::UINT uRow, alchemy::UINT uColumn, alchemy::UINT uRowOffset, alchemy::UINT uOffset, alchemy::UINT uCount)
{
	if(!m_pDevice)
		return false;

	UINT uMinOffset = ALCHEMY_MININUM(uRow * uColumn, uOffset);

	if( ALCHEMY_TEST_FLAG(Type, IEffect::VERTEX_SHADER) )
	{
		if(uRowOffset == 4)
		{
			if(uMinOffset == uOffset)
			{
				if(m_pDevice->SetVertexShaderConstantF(uIndex, pfValues, uColumn * uCount) != D3D_OK)
					return false;
			}
			else
			{
				UINT uCurrentIndex = uIndex, uCurrentOffset = 0;
				for(UINT i = 0; i < uCount; ++ i)
				{
					if(m_pDevice->SetVertexShaderConstantF(uCurrentIndex, pfValues + uCurrentOffset, uColumn) != D3D_OK)
						return false;

					uCurrentIndex += uColumn;
					uCurrentOffset += uOffset;
				}
			}
		}
		else if(uRowOffset > 4)
		{
			UINT i, j, uCurrentIndex = uIndex, uCurrentOffset = 0;
			for(i = 0; i < uCount; ++ i)
			{
				for(j = 0; j < uColumn; ++ j)
				{
					if(m_pDevice->SetVertexShaderConstantF(uCurrentIndex ++, pfValues + uCurrentOffset, 1) != D3D_OK)
						return false;

					uCurrentOffset += uRowOffset;
				}
			}
		}
		else
		{
			-- uColumn;

			UINT i, j, uCurrentIndex = uIndex, uCurrentOffset = 0;
			FLOAT fVector4[4];
			for(i = 0; i < uCount; ++ i)
			{
				for(j = 0; j < uColumn; ++ j)
				{
					if(m_pDevice->SetVertexShaderConstantF(uCurrentIndex ++, pfValues + uCurrentOffset, 1) != D3D_OK)
						return false;

					uCurrentOffset += uRowOffset;
				}

				for(j = 0; j < uRow; ++ j)
					fVector4[j] = pfValues[j];

				if(m_pDevice->SetVertexShaderConstantF(uCurrentIndex ++, fVector4, 1) != D3D_OK)
					return false;

				uCurrentOffset += uRowOffset;
			}
		}
	}

	if( ALCHEMY_TEST_FLAG(Type, IEffect::FRAGMENT_SHADER) )
	{
		if(uRowOffset == 4)
		{
			if(uMinOffset == uOffset)
			{
				if(m_pDevice->SetPixelShaderConstantF(uIndex, pfValues, uColumn * uCount) != D3D_OK)
					return false;
			}
			else
			{
				UINT uCurrentIndex = uIndex, uCurrentOffset = 0;
				for(UINT i = 0; i < uCount; ++ i)
				{
					if(m_pDevice->SetPixelShaderConstantF(uCurrentIndex, pfValues + uCurrentOffset, uColumn) != D3D_OK)
						return false;

					uCurrentIndex += uColumn;
					uCurrentOffset += uOffset;
				}
			}
				
		}
		else if(uRowOffset > 4)
		{
			UINT i, j, uCurrentIndex = uIndex, uCurrentOffset = 0;
			for(i = 0; i < uCount; ++ i)
			{
				for(j = 0; j < uColumn; ++ j)
				{
					if(m_pDevice->SetPixelShaderConstantF(uCurrentIndex ++, pfValues + uCurrentOffset, 1) != D3D_OK)
						return false;

					uCurrentOffset += uRowOffset;
				}
			}
		}
		else
		{
			-- uColumn;

			UINT i, j, uCurrentIndex = uIndex, uCurrentOffset = 0;
			FLOAT fVector4[4];
			for(i = 0; i < uCount; ++ i)
			{
				for(j = 0; j < uColumn; ++ j)
				{
					if(m_pDevice->SetPixelShaderConstantF(uCurrentIndex ++, pfValues + uCurrentOffset, 1) != D3D_OK)
						return false;

					uCurrentOffset += uRowOffset;
				}

				for(j = 0; j < uRow; ++ j)
					fVector4[j] = pfValues[j];

				if(m_pDevice->SetPixelShaderConstantF(uCurrentIndex ++, fVector4, 1) != D3D_OK)
					return false;

				uCurrentOffset += uRowOffset;
			}
		}
	}

	return true;
}

bool CD3D9Device::SetShaderMatrixPointerArray(EFFECTTYPE Type, alchemy::UINT uIndex, const alchemy::PFLOAT* ppfValues, alchemy::UINT uRow, alchemy::UINT uColumn, alchemy::UINT uRowOffset, alchemy::UINT uCount)
{
	if(!m_pDevice)
		return false;

	uRow = ALCHEMY_MININUM(uRow, uRowOffset);

	UINT i, uCurrentIndex;

	PFLOAT pfValues;

	if( ALCHEMY_TEST_FLAG(Type, IEffect::VERTEX_SHADER) )
	{
		uCurrentIndex = uIndex;

		if(uRowOffset == 4)
		{
			UINT uVector4Count = (4 * uColumn + 3) >> 2;
			for(i = 0; i < uCount; ++ i)
			{
				pfValues = ppfValues[i];
				if(pfValues)
				{
					if(m_pDevice->SetVertexShaderConstantF(uCurrentIndex, pfValues, uVector4Count) != D3D_OK)
						return false;
					
					uCurrentIndex += uVector4Count;
				}
			}
		}
		else if(uRowOffset > 4)
		{
			UINT j;
			for(i = 0; i < uCount; ++ i)
			{
				pfValues = ppfValues[i];
				if(pfValues)
				{
					for(j = 0; j < uColumn; ++ j)
					{
						if(m_pDevice->SetVertexShaderConstantF(uCurrentIndex ++, pfValues, 1) != D3D_OK)
							return false;

						pfValues += uRowOffset;
					}
				}
			}
		}
		else
		{
			-- uColumn;

			UINT j;
			FLOAT fVector4[4];
			for(i = 0; i < uCount; ++ i)
			{
				pfValues = ppfValues[i];
				if(pfValues)
				{
					for(j = 0; j < uColumn; ++ j)
					{
						if(m_pDevice->SetVertexShaderConstantF(uCurrentIndex ++, pfValues, 1) != D3D_OK)
							return false;

						pfValues += uRowOffset;
					}

					for(j = 0; j < uRow; ++ j)
						fVector4[j] = pfValues[j];

					if(m_pDevice->SetVertexShaderConstantF(uCurrentIndex ++, fVector4, 1) != D3D_OK)
						return false;
				}
			}
		}
	}

	if( ALCHEMY_TEST_FLAG(Type, IEffect::FRAGMENT_SHADER) )
	{
		uCurrentIndex = uIndex;

		if(uRowOffset == 4)
		{
			UINT uVector4Count = (4 * uColumn + 3) >> 2;
			for(i = 0; i < uCount; ++ i)
			{
				pfValues = ppfValues[i];
				if(pfValues)
				{
					if(m_pDevice->SetPixelShaderConstantF(uCurrentIndex, pfValues, uVector4Count) != D3D_OK)
						return false;

					uCurrentIndex += uVector4Count;
				}
			}
		}
		else if(uRowOffset > 4)
		{
			UINT j;
			for(i = 0; i < uCount; ++ i)
			{
				pfValues = ppfValues[i];
				if(pfValues)
				{
					for(j = 0; j < uColumn; ++ j)
					{
						if(m_pDevice->SetPixelShaderConstantF(uCurrentIndex ++, pfValues, 1) != D3D_OK)
							return false;

						pfValues += uRowOffset;
					}
				}
			}
		}
		else
		{
			-- uColumn;

			UINT j;
			FLOAT fVector4[4];
			for(i = 0; i < uCount; ++ i)
			{
				pfValues = ppfValues[i];
				if(pfValues)
				{
					for(j = 0; j < uColumn; ++ j)
					{
						if(m_pDevice->SetPixelShaderConstantF(uCurrentIndex ++, pfValues, 1) != D3D_OK)
							return false;

						pfValues += uRowOffset;
					}

					for(j = 0; j < uRow; ++ j)
						fVector4[j] = pfValues[j];

					if(m_pDevice->SetPixelShaderConstantF(uCurrentIndex ++, fVector4, 1) != D3D_OK)
						return false;
				}
			}
		}
	}

	return true;
}

bool CD3D9Device::SetShaderConstants(IEffect::EFFECTTYPE Type, CGraphicsResource::HANDLE ConstantBuffer, alchemy::UINT uIndex)
{
	return false;
}

bool CD3D9Device::SetTexture(EFFECTTYPE Type, CGraphicsResource::HANDLE Texture, alchemy::UINT uIndex, const SAMPLERDECLARATION* pSamplerDeclaration)
{
	if(!m_pDevice)
		return false;

	CGraphicsResource* pResource = GetResource(Texture, CGraphicsResource::TEXTURE);
	if(pResource)
	{
		IDirect3DBaseTexture9* pInstance = static_cast<CD3D9Texture*>(pResource)->GetInstance();
		if(pInstance && m_pDevice->SetTexture(uIndex, pInstance) == D3D_OK)
		{
			if(pSamplerDeclaration)
			{
				static const D3DTEXTUREADDRESS s_TEXTURE_ADDRESS_TABLE[] = 
				{
					D3DTADDRESS_WRAP,
					D3DTADDRESS_MIRROR,
					D3DTADDRESS_CLAMP
				};

				m_pDevice->SetSamplerState(uIndex, D3DSAMP_ADDRESSU, s_TEXTURE_ADDRESS_TABLE[pSamplerDeclaration->AddressU]);
				m_pDevice->SetSamplerState(uIndex, D3DSAMP_ADDRESSV, s_TEXTURE_ADDRESS_TABLE[pSamplerDeclaration->AddressV]);
				m_pDevice->SetSamplerState(uIndex, D3DSAMP_ADDRESSW, s_TEXTURE_ADDRESS_TABLE[pSamplerDeclaration->AddressW]);

				static const D3DTEXTUREFILTERTYPE s_TEXTURE_FILTER_TYPE_TABLE[] = 
				{
					D3DTEXF_NONE,
					D3DTEXF_POINT,
					D3DTEXF_LINEAR
				};

				m_pDevice->SetSamplerState(uIndex, D3DSAMP_MAGFILTER, s_TEXTURE_FILTER_TYPE_TABLE[pSamplerDeclaration->MagFilter]);
				m_pDevice->SetSamplerState(uIndex, D3DSAMP_MINFILTER, s_TEXTURE_FILTER_TYPE_TABLE[pSamplerDeclaration->MinFilter]);
				m_pDevice->SetSamplerState(uIndex, D3DSAMP_MIPFILTER, s_TEXTURE_FILTER_TYPE_TABLE[pSamplerDeclaration->MipFilter]);
			}

			return true;
		}
	}

	return false;
}

bool CD3D9Device::DrawIndexedTriangles(TRIANGLETYPE Type, alchemy::UINT uStartIndex, alchemy::UINT uTriangleCount)
{
	if(!m_pDevice)
		return false;

	if(!m_bIsBeginScene)
	{
		if(m_pDevice->BeginScene() != D3D_OK)
			return false;

		m_bIsBeginScene = true;
	}

	static D3DPRIMITIVETYPE s_D3DPRIMITIVE_TYPE_TABLE[] = 
	{
		D3DPT_TRIANGLELIST,
		D3DPT_TRIANGLESTRIP,
		D3DPT_TRIANGLEFAN
	};

	return m_pDevice->DrawIndexedPrimitive(s_D3DPRIMITIVE_TYPE_TABLE[Type], 0, 0, m_uNumVertex, uStartIndex, uTriangleCount) == D3D_OK;
}

bool CD3D9Device::Present()
{
	if(m_pDevice)
	{
		if(m_bIsBeginScene)
		{
			if(m_pDevice->EndScene() != D3D_OK)
				return false;

			m_bIsBeginScene = false;
		}

		return m_pDevice->Present(ALCHEMY_NULL, ALCHEMY_NULL, ALCHEMY_NULL, ALCHEMY_NULL) == S_OK;
	}

	return false;
}

void CD3D9Device::Destroy()
{
	if(m_pDirect)
	{
		m_pDirect->Release();

		m_pDirect = ALCHEMY_NULL;
	}

	if(m_pDevice)
	{
		m_pDevice->Release();

		m_pDevice = ALCHEMY_NULL;
	}

	if(m_pBackgroundSurface)
		m_pBackgroundSurface->Release();

	ALCHEMY_DEBUG_DELETE(m_pCompiler);

	CRenderSystem::Destroy();
}

CGraphicsResource* CD3D9Device::_Create(CGraphicsResource::TYPE Type)
{
	CGraphicsResource* pResource = ALCHEMY_NULL;

	switch(Type)
	{
	case CGraphicsResource::VERTEX_DECLARATION:
		ALCHEMY_DEBUG_NEW(pResource, CD3D9VertexDeclaration);
		break;
	case CGraphicsResource::VERTEX_BUFFER:
		ALCHEMY_DEBUG_NEW(pResource, CD3D9VertexBuffer);
		break;
	case CGraphicsResource::INDEX_BUFFER:
		ALCHEMY_DEBUG_NEW(pResource, CD3D9IndexBuffer);
		break;
	case CGraphicsResource::SHADER:
		ALCHEMY_DEBUG_NEW(pResource, CD3D9Shader);
		break;
	case CGraphicsResource::EFFECT:
		ALCHEMY_DEBUG_NEW(pResource, CD3D9Effect);
		break;
	case CGraphicsResource::TEXTURE:
		ALCHEMY_DEBUG_NEW(pResource, CD3D9Texture);
		break;
	default:
		break;
	}

	return pResource;
}

bool CD3D9Device::__SetRenderState(RENDERSTATE State, alchemy::UINT uValue)
{
	static const D3DRENDERSTATETYPE s_RENDER_STATE_TYPE[] = 
	{
		D3DRS_FILLMODE,

		D3DRS_SHADEMODE,

		D3DRS_ZWRITEENABLE,
		D3DRS_ZFUNC,

		D3DRS_SRCBLEND,
		D3DRS_DESTBLEND,
		D3DRS_ALPHAREF,
		D3DRS_ALPHAFUNC,

		D3DRS_CULLMODE,

		D3DRS_STENCILFAIL,
		D3DRS_STENCILZFAIL,
		D3DRS_STENCILPASS,
		D3DRS_STENCILFUNC,
		D3DRS_STENCILREF,
		D3DRS_STENCILMASK,
		D3DRS_STENCILWRITEMASK,
		D3DRS_CCW_STENCILFAIL,
		D3DRS_CCW_STENCILZFAIL,
		D3DRS_CCW_STENCILPASS,
		D3DRS_CCW_STENCILFUNC,

		D3DRS_COLORWRITEENABLE
	};

	return m_pDevice && m_pDevice->SetRenderState(s_RENDER_STATE_TYPE[State], uValue) == S_OK;
}