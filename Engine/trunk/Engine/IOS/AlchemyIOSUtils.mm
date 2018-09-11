//
//  AlchemyIOSUtils.m
//  IOS
//
//  Created by lin fuqing on 12-5-23.
//  Copyright 2012 AstepGame. All rights reserved.
//

#import "AlchemyIOSUtils.h"

#include "../Graphics/AlchemyMeshManager.h"

using namespace alchemy;

CBuffer& alchemy::GetRGBAsFromImage(UIImage& Image)
{
    CGImageRef ImageRef = [&Image CGImage];
    UINT uWidth = CGImageGetWidth(ImageRef);
    UINT uHeight = CGImageGetHeight(ImageRef);
    CGColorSpaceRef ColorSpace = CGColorSpaceCreateDeviceRGB();
    CBuffer& Result = CreateBuffer(uWidth * uHeight * 4);
	Result.Clear();
    UINT uBytesPerPixel = 4;
    UINT uBytesPerRow = uBytesPerPixel * uWidth;
    UINT uBitsPerComponent = 8;
    CGContextRef Context = CGBitmapContextCreate(Result.GetBufferPointer(), uWidth, uHeight,
												 uBitsPerComponent, uBytesPerRow, ColorSpace,
												 kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big);
    CGColorSpaceRelease(ColorSpace);
	
    CGContextDrawImage(Context, CGRectMake(0, 0, uWidth, uHeight), ImageRef);
    CGContextRelease(Context);
	
	return Result;
}

UINT alchemy::CreateTextureDataFromFile(const CHAR* pcFilePath, UINT uUsage, UINT uMipLevel)
{
	NSString* pFilePath = [NSString stringWithUTF8String: pcFilePath];
	if(pFilePath)
	{
		UIImage* pImage = [UIImage imageWithContentsOfFile: pFilePath];
		//[pFilePath release];
		if(pImage)
		{
			CBuffer& ImageData = GetRGBAsFromImage(*pImage);
			CGImageRef ImageRef = [pImage CGImage];
			CMeshManager::TEXTUREDATA TextureData;
			TextureData.pData = reinterpret_cast<PVOID>( ImageData.GetBufferPointer() );
			TextureData.uWidth = CGImageGetWidth(ImageRef);
			TextureData.uHeight = CGImageGetHeight(ImageRef);

			TextureData.Format = CTexture::R8G8B8A8;
			TextureData.uUsage = uUsage;

			TextureData.uMipLevel = uMipLevel;
			
			UINT uTextureData = ALCHEMY_MESH_MANAGER.AddTextureData(TextureData);
			
			//[pImage release];
			
			ImageData.Release();
			
			return uTextureData;
		}
		else
			ALCHEMY_DEBUG_WARNING("CreateTextureDataFromFile: Error file path");
	}
	
	return 0;
}
