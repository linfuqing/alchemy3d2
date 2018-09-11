#include "AlchemyIOSOpenGLESContext.h"
#include "AlchemyIOSUtils.h"

using namespace alchemy;

CIOSOpenGLESContext::CIOSOpenGLESContext(CAEAGLLayer& Layer) :
m_Layer(Layer)
{
	ALCHEMY_DEBUG_ASSERT(m_pContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2], 
						 "CIOSOpenGLESContext::CIOSOpenGLESContext::Failed to initialize OpenGLES 2.0 context");
	
    ALCHEMY_DEBUG_ASSERT([EAGLContext setCurrentContext:m_pContext], 
						 "CIOSOpenGLESContext::CIOSOpenGLESContext::Failed to set current OpenGL context");
	
	CGRect Rect = [&Layer frame];
	SetViewport(Rect.size.width, Rect.size.height);
}

CIOSOpenGLESContext::~CIOSOpenGLESContext()
{
    
}

LPTEXTURE CIOSOpenGLESContext::CreateTextureFromFile(const CHAR* pcFilePath, UINT uUsage, UINT uMipLevel)
{
	LPTEXTURE pOutput = ALCHEMY_NULL;
	NSString* pFilePath = [NSString stringWithUTF8String: pcFilePath];
	if(pFilePath)
	{
		UIImage* pImage = [UIImage imageWithContentsOfFile: pFilePath];
		//[pFilePath release];
		if(pImage)
		{
			CBuffer& ImageData = GetRGBAsFromImage(*pImage);
			CGImageRef ImageRef = [pImage CGImage];
			pOutput = CreateTexture(
			CGImageGetWidth(ImageRef), 
			CGImageGetHeight(ImageRef), 
			uUsage, 
			CTexture::R8G8B8A8, 
			uMipLevel, 
			ImageData.GetBufferPointer() );
			
			/*UIImageView* pView = [[UIImageView alloc] initWithImage:pImage];
			UIWindow* pWindow = [[UIApplication sharedApplication] keyWindow];
			[pWindow addSubview:pView];
			[pWindow bringSubviewToFront:pView];*/
						
			//[pImage release];
			
			ImageData.Release();
		}
		else
		{
			ALCHEMY_DEBUG_WARNING("CIOSOpenGLESContext::CreateTextureFromFile: Error file path");
		}
	}
	
	return pOutput;
}

LPTEXTURE CIOSOpenGLESContext::CreateCubeTextureFromFile(const CHAR* pcPositiveXFilePath,
														 const CHAR* pcNegativeXFilePath,
														 const CHAR* pcPositiveYFilePath,
														 const CHAR* pcNegativeYFilePath,
														 const CHAR* pcPositiveZFilePath,
														 const CHAR* pcNegativeZFilePath, 
														 UINT uUsage, 
														 UINT uMipLevel)
{
	LPTEXTURE pOutput = ALCHEMY_NULL;
	NSString* pPositiveXFilePath = [NSString stringWithUTF8String: pcPositiveXFilePath], 
			* pNegativeXFilePath = [NSString stringWithUTF8String: pcNegativeXFilePath], 
			* pPositiveYFilePath = [NSString stringWithUTF8String: pcPositiveYFilePath], 
			* pNegativeYFilePath = [NSString stringWithUTF8String: pcNegativeYFilePath], 
			* pPositiveZFilePath = [NSString stringWithUTF8String: pcPositiveZFilePath], 
			* pNegativeZFilePath = [NSString stringWithUTF8String: pcNegativeZFilePath];
	if(pPositiveXFilePath && 
	   pNegativeXFilePath &&
	   pPositiveYFilePath &&
	   pNegativeYFilePath &&
	   pPositiveZFilePath &&
	   pNegativeZFilePath)
	{
		UIImage* pPositiveXImage = [UIImage imageWithContentsOfFile: pPositiveXFilePath], 
			   * pNegativeXImage = [UIImage imageWithContentsOfFile: pNegativeXFilePath], 
			   * pPositiveYImage = [UIImage imageWithContentsOfFile: pPositiveYFilePath], 
			   * pNegativeYImage = [UIImage imageWithContentsOfFile: pNegativeYFilePath], 
			   * pPositiveZImage = [UIImage imageWithContentsOfFile: pPositiveZFilePath], 
			   * pNegativeZImage = [UIImage imageWithContentsOfFile: pNegativeZFilePath];
		if(pPositiveXImage &&
		   pNegativeXImage &&
		   pPositiveYImage &&
		   pNegativeYImage &&
		   pPositiveZImage &&
		   pNegativeZImage)
		{
			CBuffer& PositiveXImageData = GetRGBAsFromImage(*pPositiveXImage), 
				   & NegativeXImageData = GetRGBAsFromImage(*pNegativeXImage), 
				   & PositiveYImageData = GetRGBAsFromImage(*pPositiveYImage), 
				   & NegativeYImageData = GetRGBAsFromImage(*pNegativeYImage), 
				   & PositiveZImageData = GetRGBAsFromImage(*pPositiveZImage), 
				   & NegativeZImageData = GetRGBAsFromImage(*pNegativeZImage);
			
			UINT uSize = 0;
			CGImageRef ImageRef;
			
			ImageRef = [pPositiveXImage CGImage];
			uSize = ALCHEMY_MAXINUM(static_cast<UINT>( CGImageGetWidth(ImageRef)  ), uSize);
			uSize = ALCHEMY_MAXINUM(static_cast<UINT>( CGImageGetHeight(ImageRef) ), uSize);
			
			ImageRef = [pNegativeXImage CGImage];
			uSize = ALCHEMY_MAXINUM(static_cast<UINT>( CGImageGetWidth(ImageRef)  ), uSize);
			uSize = ALCHEMY_MAXINUM(static_cast<UINT>( CGImageGetHeight(ImageRef) ), uSize);
			
			ImageRef = [pPositiveYImage CGImage];
			uSize = ALCHEMY_MAXINUM(static_cast<UINT>( CGImageGetWidth(ImageRef)  ), uSize);
			uSize = ALCHEMY_MAXINUM(static_cast<UINT>( CGImageGetHeight(ImageRef) ), uSize);
			
			ImageRef = [pNegativeYImage CGImage];
			uSize = ALCHEMY_MAXINUM(static_cast<UINT>( CGImageGetWidth(ImageRef)  ), uSize);
			uSize = ALCHEMY_MAXINUM(static_cast<UINT>( CGImageGetHeight(ImageRef) ), uSize);
			
			ImageRef = [pPositiveZImage CGImage];
			uSize = ALCHEMY_MAXINUM(static_cast<UINT>( CGImageGetWidth(ImageRef)  ), uSize);
			uSize = ALCHEMY_MAXINUM(static_cast<UINT>( CGImageGetHeight(ImageRef) ), uSize);
			
			ImageRef = [pNegativeZImage CGImage];
			uSize = ALCHEMY_MAXINUM(static_cast<UINT>( CGImageGetWidth(ImageRef)  ), uSize);
			uSize = ALCHEMY_MAXINUM(static_cast<UINT>( CGImageGetHeight(ImageRef) ), uSize);
			
			pOutput = CreateCubeTexture(
			uSize, 
			uUsage, 
			CTexture::R8G8B8A8, 
			uMipLevel, 
			PositiveXImageData.GetBufferPointer(), 
			NegativeXImageData.GetBufferPointer(), 
			PositiveYImageData.GetBufferPointer(), 
			NegativeYImageData.GetBufferPointer(), 
			PositiveZImageData.GetBufferPointer(), 
			NegativeZImageData.GetBufferPointer() );
			
			PositiveXImageData.Release();
			NegativeXImageData.Release();
			PositiveYImageData.Release();
			NegativeYImageData.Release();
			PositiveZImageData.Release();
			NegativeZImageData.Release();
		}
		else
		{
			ALCHEMY_DEBUG_WARNING("CIOSOpenGLESContext::CreateTextureFromFile: Error file path");
		}
	}
	
	return pOutput;
}

bool CIOSOpenGLESContext::SetViewport(UINT uWidth, UINT uHeight)
{
	if(m_pContext)
	{
		CGLESContext::SetViewport(uWidth, uHeight);
		
		[m_pContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:&m_Layer];
		
		return true;
	}
	
	return false;
}

bool CIOSOpenGLESContext::Present()
{
	if(m_pContext)
	{
		[m_pContext presentRenderbuffer:GL_RENDERBUFFER];
		
		return true;
	}
	
	return false;
}