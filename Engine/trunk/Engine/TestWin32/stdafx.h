// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
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

#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ���ų�����ʹ�õ�����
// Windows ͷ�ļ�:
#include <windows.h>

// C ����ʱͷ�ļ�
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>


// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
