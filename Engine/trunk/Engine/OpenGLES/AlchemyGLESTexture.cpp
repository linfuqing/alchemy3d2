#include "AlchemyGLESTexture.h"

using namespace alchemy;

CGLESTexture::CGLESTexture() :
m_uTextureHandle(0),
//m_uRenderTargetFrame(0), 

m_uLockSurface(0),
m_uLockFlag(0),

m_puData(ALCHEMY_NULL), 
m_uDataSize(0),

m_puLockData(ALCHEMY_NULL), 
m_uLockDataSize(0), 
m_uLockDataOffset(0)
{
}

CGLESTexture::~CGLESTexture()
{
}

GLenum CGLESTexture::GetGLESTextureType(CTexture::TYPE Type)
{
	const GLenum s_TEXTURE_TYPE[2] = 
	{
		GL_TEXTURE_2D, 
		GL_TEXTURE_CUBE_MAP
	};
	
	return (Type < 2 && Type >= 0) ? s_TEXTURE_TYPE[Type] : 0;
}

bool CGLESTexture::GetGLESTextureFormatType(CTexture::FORMAT Format, GLenum* pOutputFormat, GLenum* pOuputType, bool bIsRenderTarget)
{
	switch (Format) 
	{
		default:
			if(pOutputFormat)
				*pOutputFormat = GL_RGBA;
			
			if(pOuputType)
				*pOuputType   = bIsRenderTarget ? GL_UNSIGNED_SHORT_4_4_4_4 : GL_UNSIGNED_BYTE;
			
			break;
	}
	
	return true;
}

void CGLESTexture::Activate()
{	
	glBindTexture(GetGLESTextureType( GetTextureType() ), m_uTextureHandle);
						
	UINT uUsage = TEXTURE::GetUsage();
	
	if( !ALCHEMY_TEST_BIT(uUsage, TEXTURE::DYNAMIC) && ALCHEMY_TEST_BIT(uUsage, TEXTURE::WRITE_ONLY) )
		ALCHEMY_DEBUG_DELETE_ARRAY(m_puData);
}

void* CGLESTexture::Lock(UINT uSurface, UINT uFlag, PUINT puRowPitch, PUINT puSlicePitch)
{
	UINT uWidth, uHeight, uDepth, uMipLevel = uSurface & CTexture::MIP_LEVEL_MASK;
	if( GetMipMapSize(uMipLevel, &uWidth, &uHeight, &uDepth) )
	{
		UINT uStride = TEXTURE::GetStride();
		if(puRowPitch)
			*puRowPitch = uWidth * uStride;
		
		if(puSlicePitch)
			*puSlicePitch = uDepth * uStride;
		
		if( ALCHEMY_TEST_BIT(TEXTURE::GetUsage(), TEXTURE::WRITE_ONLY) ) 
		{
			if( ALCHEMY_TEST_BIT(uFlag, TEXTURE::READ_ONLY) ) 
				return ALCHEMY_NULL;
			
			m_uLockDataSize   = uWidth * uHeight * uDepth * uStride;
			m_uLockDataOffset = 0;
		}
		else
		{
			m_uLockDataSize = GetByteSize();
			
			m_uLockDataOffset = 0;
			for(UINT i = 0; i < uMipLevel; ++ i)
			{
				GetMipMapSize(i, &uWidth, &uHeight, &uDepth);
				
				m_uLockDataOffset += uWidth * uHeight * uDepth * uStride;
			}
		}
		
		if( ALCHEMY_TEST_BIT(uFlag, TEXTURE::DISCARD) || m_uDataSize < m_uLockDataSize)
		{
			ALCHEMY_DEBUG_DELETE_ARRAY(m_puData);
			ALCHEMY_DEBUG_NEW(m_puData, UINT8[m_uLockDataSize]);
			
			m_uDataSize = m_uLockDataSize;
		}
		
		m_uLockSurface = uSurface;
		m_uLockFlag    = uFlag;
		
		return m_puData + m_uLockDataOffset;
	}
	
	return ALCHEMY_NULL;
}

bool CGLESTexture::Unlock()
{
	UINT uTotalByteSize = TEXTURE::GetByteSize();
	if(m_uLockDataOffset < uTotalByteSize)
	{
		UINT uWidth, uHeight, uDepth, uMipLevel = m_uLockSurface & CTexture::MIP_LEVEL_MASK;
		if( GetMipMapSize(uMipLevel, &uWidth, &uHeight, &uDepth) )
		{
			static const GLenum s_CUBE_TEXTURE_FACE[6] = 
			{
				GL_TEXTURE_CUBE_MAP_POSITIVE_X, 
				GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 
				GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 
				GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 
				GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 
				GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
			};
			
			UINT uCubeTextureFace = m_uLockSurface >> CTexture::CUBE_TEXTURE_SURFACE_SHIFT;
			GLenum Format, Type;
			GetGLESTextureFormatType(GetFormat(), &Format, &Type, false);
			
			switch ( TEXTURE::GetTextureType() ) 
			{
				case CTexture::CUBE:
					glBindTexture(GL_TEXTURE_CUBE_MAP, m_uTextureHandle);
					
					glTexImage2D(s_CUBE_TEXTURE_FACE[uCubeTextureFace], 
								 uMipLevel, 
								 Format, 
								 uWidth, 
								 uHeight, 
								 0, 
								 Format, 
								 Type, 
								 m_puData + m_uLockDataOffset);
					break;
				default:
					UINT uUsage = GetUsage(), uRowByteSize = uWidth * GetStride(), uSliceByteSize = uHeight * uRowByteSize;
					if(!m_puLockData)
					{
						if( ALCHEMY_TEST_BIT(uUsage, TEXTURE::DYNAMIC) )
						{
							ALCHEMY_DEBUG_NEW(m_puLockData, UINT8[ALCHEMY_FLAG( GetWidthShift() ) * ALCHEMY_FLAG( GetHeightShift() ) * ALCHEMY_FLAG( GetDepthShift() ) * GetStride()]);
						}
						else
						{
							ALCHEMY_DEBUG_NEW(m_puLockData, UINT8[uSliceByteSize * uDepth]);
						}
					}
					
					UINT i, j, uDestinationOffset = 0, uSourceOffset = m_uLockDataOffset + uSliceByteSize - uRowByteSize;
					uSliceByteSize <<= 1;
					for(i = 0; i < uDepth; ++ i)
					{
						for(j = 0; j < uHeight; ++ j)
						{
							ALCHEMY_MEMORY_COPY(m_puLockData + uDestinationOffset, m_puData + uSourceOffset, uRowByteSize);
							
							uDestinationOffset += uRowByteSize;
							uSourceOffset      -= uRowByteSize;
						}
						
						uSourceOffset += uSliceByteSize;
					}
					
					glBindTexture(GL_TEXTURE_2D, m_uTextureHandle);
					
					glTexImage2D(GL_TEXTURE_2D, 
								 uMipLevel, 
								 Format, 
								 uWidth, 
								 uHeight, 
								 0, 
								 Format, 
								 Type, 
								 m_puLockData);//m_puData + m_uLockDataOffset);
					
					if( !ALCHEMY_TEST_BIT(uUsage, TEXTURE::DYNAMIC) )
						ALCHEMY_DEBUG_DELETE_ARRAY(m_puLockData);
						
					break;
			}
		}
		
		m_uLockDataOffset = uTotalByteSize;
		
		return true;
	}
	
	return false;
}

//bool CGLESTexture::BindToRenderTarget(GLuint uDepthRenderBuffer)
//{
//	if(!m_uRenderTargetFrame)
//		glGenFramebuffers(1, &m_uRenderTargetFrame);
//	
//	glBindFramebuffer(GL_FRAMEBUFFER, m_uRenderTargetFrame);
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GetGLESTextureType( TEXTURE::GetTextureType() ), m_uTextureHandle, 0);
//	
//	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, uDepthRenderBuffer);
//	
//	return true;
//}

void CGLESTexture::Destroy()
{
	if(m_uTextureHandle)
		glDeleteTextures(1, &m_uTextureHandle);
	
	ALCHEMY_DEBUG_DELETE_ARRAY(m_puLockData);
	//if(m_uRenderTargetFrame)
//		glDeleteFramebuffers(1, &m_uRenderTargetFrame);
}

bool CGLESTexture::_Create()
{
	glGenTextures(1, &m_uTextureHandle);
	
	GLenum Format, Type;
	if( ALCHEMY_TEST_BIT(TEXTURE::GetUsage(), CTexture::RENDER_TARGET) )
	{
		if( GetGLESTextureFormatType(TEXTURE::GetFormat(), &Format, &Type, true) )
		{
			UINT uWidth = TEXTURE::GetWidth(), uHeight = TEXTURE::GetHeight();
			switch ( TEXTURE::GetTextureType() ) 
			{
				case CTexture::CUBE:
					glBindTexture(GL_TEXTURE_CUBE_MAP, m_uTextureHandle);
					
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 
								 0, 
								 Format, 
								 uWidth, 
								 uHeight, 
								 0, 
								 Format, 
								 Type, 
								 ALCHEMY_NULL);
					
					glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 
								 0, 
								 Format, 
								 uWidth, 
								 uHeight, 
								 0, 
								 Format, 
								 Type, 
								 ALCHEMY_NULL);
					
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 
								 0, 
								 Format, 
								 uWidth, 
								 uHeight, 
								 0, 
								 Format, 
								 Type, 
								 ALCHEMY_NULL);
					
					glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 
								 0, 
								 Format, 
								 uWidth, 
								 uHeight, 
								 0, 
								 Format, 
								 Type, 
								 ALCHEMY_NULL);
					
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 
								 0, 
								 Format, 
								 uWidth, 
								 uHeight, 
								 0, 
								 Format, 
								 Type, 
								 ALCHEMY_NULL);
					
					glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 
								 0, 
								 Format, 
								 uWidth, 
								 uHeight, 
								 0, 
								 Format, 
								 Type, 
								 ALCHEMY_NULL);
					break;
				default:
					glBindTexture(GL_TEXTURE_2D, m_uTextureHandle);
					
					glTexImage2D(GL_TEXTURE_2D, 
								 0, 
								 Format, 
								 uWidth, 
								 uHeight, 
								 0, 
								 Format, 
								 Type, 
								 ALCHEMY_NULL);
					
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); 
					
					break;
			}
		}
	}
	
	return true;
}