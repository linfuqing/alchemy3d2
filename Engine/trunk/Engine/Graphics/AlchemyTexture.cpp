#include "AlchemyTexture.h"
#include "AlchemyRenderSystem.h"
//#include "AlchemyRGBAColor.h"

#include "../Core/AlchemyDebug.h"

using namespace alchemy;

CTexture::CTexture(void) :
m_TextureType(NORMAL),
m_Format(UNKNOWN),
m_uMipLevel(0),
m_uWidth(0),
m_uHeight(0),
m_uDepth(0),
m_uWidthShift(0),
m_uHeightShift(0),
m_uDepthShift(0), 
m_pRenderTargetViewListener(ALCHEMY_NULL)//,
//m_pTextureInfo(ALCHEMY_NULL)
{
}

CTexture::~CTexture(void)
{
}

UINT CTexture::GetFormatSize(CTexture::FORMAT Format)
{
	switch(Format)
	{
	case DEFAULT:
		return 4;
		break;
    default:
        break;
	}

	return 0;
}

void CTexture::BilerpBlends(
				  void* pOutput, 
				  UINT uOutputPitch,
				  UINT uOutputWidth, 
				  UINT uOutputHeight, 
				  const void* pSource, 
				  UINT uSourcePitch, 
				  UINT uSourceWidth, 
				  UINT uSourceHeight,
				  UINT uWidth, 
				  UINT uHeight, 
				  FLOAT fGamma, 
				  FORMAT Format)
{
	UINT16 uAvgA, uAvgR, uAvgG, uAvgB;

	UINT i, j, uBasicIndex, uIndex;

	INT k, l, u, v, nCurrentU, nCurrentV, nHalfWidth = uWidth >> 1, nHalfHeight = uHeight >> 1, nWidthEnd = uWidth - nHalfWidth + 1, nHeightEnd = uHeight - nHalfHeight + 1;

	FLOAT fWidthRatio = static_cast<FLOAT>(uSourceWidth) / uOutputWidth, fHeightRatio = static_cast<FLOAT>(uSourceHeight) / uOutputHeight;

	ARGBCOLOR Color;
	uBasicIndex = 0;
	for (i = 0; i < uOutputWidth; ++ i)
	{
		uIndex = uBasicIndex;
		for (j = 0; j < uOutputHeight; ++ j)
		{
			u = static_cast<INT>(i * fWidthRatio);
			v = static_cast<INT>(j * fHeightRatio);

			uAvgA = 0;
			uAvgR = 0;
			uAvgG = 0;
			uAvgB = 0;
			for(k = 1 - nHalfWidth; k < nWidthEnd; ++ k)
			{
				nCurrentU = ALCHEMY_CLAMP(u + k, 0, static_cast<INT>(uSourceWidth) - 1);
				for(l = 1 - nHalfHeight; l < nHeightEnd; ++ l)
				{
					nCurrentV = ALCHEMY_CLAMP(v + l, 0, static_cast<INT>(uSourceHeight) - 1);
					Color = *reinterpret_cast<const UINT32*>(reinterpret_cast<const UINT8*>(pSource) + (nCurrentU << 2) + nCurrentV * uSourcePitch);

					uAvgA += ALCHEMY_ARGBCOLOR_ALPHA_255(Color);
					uAvgR += ALCHEMY_ARGBCOLOR_RED_255(Color);
					uAvgG += ALCHEMY_ARGBCOLOR_GREEN_255(Color);
					uAvgB += ALCHEMY_ARGBCOLOR_BLUE_255(Color);
				}
			}

			uAvgA = static_cast<UINT16>(0.5f + fGamma * uAvgA * 0.25f);
			uAvgR = static_cast<UINT16>(0.5f + fGamma * uAvgR * 0.25f);
			uAvgG = static_cast<UINT16>(0.5f + fGamma * uAvgG * 0.25f);
			uAvgB = static_cast<UINT16>(0.5f + fGamma * uAvgB * 0.25f);

			*reinterpret_cast<PUINT32>( (reinterpret_cast<PUINT8>(pOutput) + uIndex) ) = ALCHEMY_ARGBCOLOR_255(uAvgA, uAvgR, uAvgG, uAvgB);

			uIndex += uOutputPitch;
		}
		uBasicIndex += 4;
	}
}

bool CTexture::GetMipMapSize(UINT uMipLevel, PUINT puWidth, PUINT puHeight, PUINT puDepth)
{
	if(uMipLevel < m_uMipLevel)
	{
		if(puWidth)
			*puWidth = 1 << (m_uWidthShift > uMipLevel ? (m_uWidthShift - uMipLevel) : 0);

		if(puHeight)
			*puHeight = 1 << (m_uHeightShift > uMipLevel ? (m_uHeightShift - uMipLevel) : 0);

		if(puDepth)
			*puDepth = 1 << (m_uDepthShift > uMipLevel ? (m_uDepthShift - uMipLevel) : 0);

		return true;
	}

	return false;
}

CTexture* CTexture::Clone()
{
	LPTEXTURE pTexture = static_cast<LPTEXTURE>(this);
	CRenderSystem& RenderSystem = static_cast<CRenderSystem&>( GetGraphicsResourcePool() );
	if(m_pRenderTargetViewListener)
		return RenderSystem.CreateRenderTargetView( m_Format, static_cast<CRenderSystem::CRenderTargetViewListener*>(m_pRenderTargetViewListener)->GetShift() );
	else
	{
		void* pData;
		switch(m_TextureType)
		{
		case NORMAL:
			pData = Lock(0, ALCHEMY_FLAG(CGraphicsBuffer<CTexture>::READ_ONLY), ALCHEMY_NULL, ALCHEMY_NULL);
			pTexture = RenderSystem.CreateTexture(1 << pTexture->m_uWidthShift, 1 << pTexture->m_uHeightShift, pTexture->GetUsage(), pTexture->m_Format, pTexture->m_uMipLevel, pData);
			Unlock();
			break;
		case CUBE:
			pTexture = ALCHEMY_NULL;
			break;
		case VOLUME:
			pTexture = ALCHEMY_NULL;
			break;
		default:
			pTexture = ALCHEMY_NULL;
		}
	}

	return pTexture;
}

void CTexture::Destroy()
{
	if(m_pRenderTargetViewListener)
	{
		m_pRenderTargetViewListener->Release();
		m_pRenderTargetViewListener = ALCHEMY_NULL;
	}
}