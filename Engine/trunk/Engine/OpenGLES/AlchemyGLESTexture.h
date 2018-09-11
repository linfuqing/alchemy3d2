#pragma once

#include <OpenGLES/ES2/gl.h>

#include "../Graphics/AlchemyGraphicsBuffer.h"

namespace alchemy 
{
	class CGLESContext;
	class CGLESTexture :
	public TEXTURE
	{
		friend class CGLESContext;
	public:
		CGLESTexture();
		~CGLESTexture();
		
		static GLenum GetGLESTextureType(CTexture::TYPE Type);
		static bool GetGLESTextureFormatType(CTexture::FORMAT Format, GLenum* pOutputFormat, GLenum* pOuputType, bool bIsRenderTarget);
		
		GLuint GetTextureHandle()const;
		
		void Activate();
		
		void* Lock(UINT uSurface, UINT uFlag, PUINT puRowPitch, PUINT puSlicePitch);
		bool Unlock();
		
		//bool BindToRenderTarget(GLuint uDepthRenderBuffer);
		
		void Destroy();
		
	protected:
		bool _Create();
		
	private:
		GLuint m_uTextureHandle;
		//GLuint m_uRenderTargetFrame;
		
		UINT m_uLockSurface;
		UINT m_uLockFlag;
		
		PUINT8 m_puData;
		UINT m_uDataSize;
		
		PUINT8 m_puLockData;
		UINT m_uLockDataSize;
		UINT m_uLockDataOffset;
	};
	
	ALCHEMY_INLINE GLuint CGLESTexture::GetTextureHandle()const
	{
		return m_uTextureHandle;
	}
}