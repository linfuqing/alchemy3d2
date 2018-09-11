// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "../Graphics/AlchemySceneManager.h"
#include "../Graphics/AlchemyHardwareSkinEffect.h"
#include "../Graphics/AlchemyPositionEffect.h"
#include "../Graphics/AlchemyTextureEffect.h"
#include "../Graphics/AlchemyPixelColorEffect.h"
#include "../Graphics/AlchemyEmptyEffect.h"
#include "../Graphics/AlchemyAlphaTestBlendMethod.h"
#include "../Graphics/AlchemySceneObject.h"
#include "../Graphics/AlchemyBone.h"
#include "../Graphics/AlchemyRectangleInstance.h"
#include "../Graphics/AlchemyInstancePosition2Scale2Effect.h"

#include "../Loader/AlchemyDAEParser.h"

#include "../Platform/AlchemyKeyListener.h"

#include "../Win32/AlchemyWin32Manager.h"
//#include "../Win32/AlchemyWin32Loader.h"

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件:
#include <windows.h>

// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>


// TODO: 在此处引用程序需要的其他头文件
