#pragma once

#include "AlchemyGraphicsResource.h"

#include "../Core/AlchemyBitFlag.h"
#include "../Core/AlchemyConstant.h"

namespace alchemy
{
	class CRenderSystem;
	class CTexture :
		public CGraphicsResource
	{
		friend class CRenderSystem;
	public:
		typedef enum
		{
			MIP_LEVEL_MASK = ALCHEMY_TOTAL_BITS(UINT) - 1,
			CUBE_TEXTURE_SURFACE_SHIFT = ALCHEMY_SHIFT( ALCHEMY_TOTAL_BITS(UINT) )
		}CONTANTS;

		typedef enum
		{
			NORMAL,
			CUBE,
			VOLUME
		}TYPE;

		typedef enum
		{
			UNKNOWN = 0,

			A8R8G8B8
		}FORMAT;

		typedef enum
		{
			RENDER_TARGET = 2
		}USAGE;

		typedef enum
		{
			POSITIVE_X = 0,
			NEGATIVE_X,
			POSITIVE_Y,
			NEGATIVE_Y,
			POSITIVE_Z,
			NEGATIVE_Z,
		}CUBEMAPFACE;

		class CTextureInfoInterface
		{
		public:
			virtual bool GetSize(PUINT puWidth, PUINT puHeight, PUINT puDepth) = 0;
		};

		CTexture(void);
		~CTexture(void);

		static UINT GetFormatSize(FORMAT Format);

		TYPE GetTextureType()const;

		FORMAT GetForamt()const;

		UINT GetMipLevel()const;

		UINT GetWidthShift()const;
		UINT GetHeightShift()const;
		UINT GetDepthShift()const;

		CTextureInfoInterface* GetTextureInfo()const;

		bool GetMipMapSize(UINT uMipLevel, PUINT puWidth, PUINT puHeight, PUINT puDepth = ALCHEMY_NULL);

		virtual PVOID Lock(UINT uSurface, UINT uFlag, PUINT puRowPitch, PUINT puSlicePitch) = 0;
		virtual bool Unlock() = 0;
	private:
		TYPE m_Type;

		FORMAT m_Format;
		UINT m_uMipLevel;

		UINT m_uWidthShift;
		UINT m_uHeightShift;
		UINT m_uDepthShift;

		CTextureInfoInterface* m_pTextureInfo;
	};

	ALCHEMY_INLINE CTexture::TYPE CTexture::GetTextureType()const
	{
		return m_Type;
	}

	ALCHEMY_INLINE CTexture::FORMAT CTexture::GetForamt()const
	{
		return m_Format;
	}

	ALCHEMY_INLINE UINT CTexture::GetMipLevel()const
	{
		return m_uMipLevel;
	}

	ALCHEMY_INLINE UINT CTexture::GetWidthShift()const
	{
		return m_uWidthShift;
	}

	ALCHEMY_INLINE UINT CTexture::GetHeightShift()const
	{
		return m_uHeightShift;
	}

	ALCHEMY_INLINE UINT CTexture::GetDepthShift()const
	{
		return m_uDepthShift;
	}

	ALCHEMY_INLINE CTexture::CTextureInfoInterface* CTexture::GetTextureInfo()const
	{
		return m_pTextureInfo;
	}
}