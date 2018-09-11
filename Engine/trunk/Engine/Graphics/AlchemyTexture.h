#pragma once

#include "AlchemyGraphicsResource.h"

#include "../Core/AlchemyNull.h"
#include "../Core/AlchemyBitFlag.h"
#include "../Core/AlchemyConstant.h"
#include "../Core/AlchemyEventDispatcherInterface.h"
#include "../Core/AlchemySmartClass.h"

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
			CUBE_TEXTURE_SURFACE_SHIFT = ALCHEMY_CONSTANT_SHIFT( ALCHEMY_TOTAL_BITS(UINT) )
		}CONTANTS;

		typedef enum
		{
			NORMAL,
			CUBE,
			VOLUME,
			
			TOTAL_TEXTURE_TYPE
		}TEXTURETYPE;

		typedef enum
		{
			UNKNOWN = 0,

			DEFAULT,
			FLOAT1,
			FLOAT4
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

		/*struct ITextureInfo
		{
			virtual bool GetSize(PUINT puWidth, PUINT puHeight, PUINT puDepth) = 0;
		};*/

		CTexture(void);
		~CTexture(void);

		static UINT GetFormatSize(FORMAT Format);

		static UINT GetFormatChannelCount(FORMAT Format);

		static void BilerpBlends(
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
			FORMAT Format);

		TEXTURETYPE GetTextureType()const;

		FORMAT GetFormat()const;

		UINT GetMipLevel()const;

		UINT GetWidth()const;
		UINT GetHeight()const;
		UINT GetDepth()const;

		UINT GetWidthShift()const;
		UINT GetHeightShift()const;
		UINT GetDepthShift()const;

		//ITextureInfo* GetTextureInfo()const;

		bool GetMipMapSize(UINT uMipLevel, PUINT puWidth, PUINT puHeight, PUINT puDepth = ALCHEMY_NULL);

		CTexture* Clone();

		void Destroy();

		virtual void* Lock(UINT uSurface, UINT uFlag, PUINT puRowPitch, PUINT puSlicePitch) = 0;
		virtual bool Unlock() = 0;
	private:
		TEXTURETYPE m_TextureType;

		FORMAT m_Format;
		UINT m_uMipLevel;

		UINT m_uWidth;
		UINT m_uHeight;
		UINT m_uDepth;

		UINT m_uWidthShift;
		UINT m_uHeightShift;
		UINT m_uDepthShift;

		IEventDispatcher::IListener* m_pRenderTargetViewListener;

		//ITextureInfo* m_pTextureInfo;
	};

	ALCHEMY_INLINE CTexture::TEXTURETYPE CTexture::GetTextureType()const
	{
		return m_TextureType;
	}

	ALCHEMY_INLINE CTexture::FORMAT CTexture::GetFormat()const
	{
		return m_Format;
	}

	ALCHEMY_INLINE UINT CTexture::GetMipLevel()const
	{
		return m_uMipLevel;
	}

	ALCHEMY_INLINE UINT CTexture::GetWidth()const
	{
		return m_uWidth;
	}

	ALCHEMY_INLINE UINT CTexture::GetHeight()const
	{
		return m_uHeight;
	}

	ALCHEMY_INLINE UINT CTexture::GetDepth()const
	{
		return m_uDepth;
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

	/*ALCHEMY_INLINE CTexture::ITextureInfo* CTexture::GetTextureInfo()const
	{
		return m_pTextureInfo;
	}*/
}