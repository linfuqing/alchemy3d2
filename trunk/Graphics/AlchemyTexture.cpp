#include "AlchemyTexture.h"

#include "../Core/AlchemyDebug.h"

using namespace alchemy;

CTexture::CTexture(void) :
m_Type(NORMAL),
m_Format(UNKNOWN),
m_uMipLevel(0),
m_uWidthShift(0),
m_uHeightShift(0),
m_uDepthShift(0),
m_pTextureInfo(ALCHEMY_NULL)
{
}

CTexture::~CTexture(void)
{
}

UINT CTexture::GetFormatSize(CTexture::FORMAT Format)
{
	switch(Format)
	{
	case A8R8G8B8:
		return 4;
		break;
	}

	return 0;
}

bool CTexture::GetMipMapSize(UINT uMipLevel, PUINT puWidth, PUINT puHeight, PUINT puDepth)
{
	if(uMipLevel < m_uMipLevel)
	{
		if(puWidth)
			*puWidth = 1 << (m_uWidthShift - uMipLevel);

		if(puHeight)
			*puHeight = 1 << (m_uHeightShift - uMipLevel);

		if(puDepth)
			*puDepth = 1 << (m_uDepthShift - uMipLevel);

		return true;
	}

	return false;
}