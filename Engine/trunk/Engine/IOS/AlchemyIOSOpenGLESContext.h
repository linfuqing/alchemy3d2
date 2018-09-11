#pragma once

#import <QuartzCore/QuartzCore.h>

#include "../OpenGLES/AlchemyGLESContext.h"

namespace alchemy 
{
    class CIOSOpenGLESContext :
    public CGLESContext
    {
    public:
        CIOSOpenGLESContext(CAEAGLLayer& Layer);
        ~CIOSOpenGLESContext();
		
		LPTEXTURE CreateTextureFromFile(const CHAR* pcFilePath, UINT uUsage, UINT uMipLevel = 0);
		LPTEXTURE CreateCubeTextureFromFile(const CHAR* pcPositiveXFilePath,
											const CHAR* pcNegativeXFilePath,
											const CHAR* pcPositiveYFilePath,
											const CHAR* pcNegativeYFilePath,
											const CHAR* pcPositiveZFilePath,
											const CHAR* pcNegativeZFilePath, 
											UINT uUsage, UINT uMipLevel = 0);
        
		bool SetViewport(UINT uWidth, UINT uHeight);
		
		bool Present();
	
    private:
		CAEAGLLayer& m_Layer;
		
        EAGLContext* m_pContext;
    };
}