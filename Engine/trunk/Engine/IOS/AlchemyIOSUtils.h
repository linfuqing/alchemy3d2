#pragma once
//
//  AlchemyIOSUtils.h
//  IOS
//
//  Created by lin fuqing on 12-5-23.
//  Copyright 2012 AstepGame. All rights reserved.
//

#import <UIKit/UIKit.h>

#include "../Core/AlchemyBuffer.h"

namespace alchemy
{
	CBuffer& GetRGBAsFromImage(UIImage& Image);
	UINT CreateTextureDataFromFile(const CHAR* pcPath, UINT uUsage, UINT uMipLevel);
}