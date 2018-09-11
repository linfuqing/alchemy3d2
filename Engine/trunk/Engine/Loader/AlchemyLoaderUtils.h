#ifndef ALCHEMY_LOADER_UTILS_H
#define ALCHEMY_LOADER_UTILS_H

#include "../Core/AlchemyString.h"

#ifdef ALCHEMY_WINDOW_OS
#include <windows.h>
#endif

namespace alchemy
{
#ifdef ALCHEMY_UNICODE

	static const UINT g_uACP        =         CP_ACP;
	static const UINT g_uOEMCP      =         CP_OEMCP;
	static const UINT g_uMACCP      =         CP_MACCP;
	static const UINT g_uTHREAD_ACP =         CP_THREAD_ACP;
	static const UINT g_uSYMBOL     =         CP_SYMBOL;

	static const UINT g_uUTF7       =         CP_UTF7;
	static const UINT g_uUTF8       =         CP_UTF8;

#define ALCHEMY_STRING_TO_BASICSTRING StringToBasicString
	BASICSTRING& StringToBasicString(BASICSTRING& Output, UINT uCodePage, STRING& String);

#define ALCHEMY_BASICSTRING_TO_STRING BasicStringToString
	STRING& BasicStringToString(STRING& Output, UINT uCodePage, BASICSTRING& String);

#else
#define ALCHEMY_ACP
	static const UINT g_uACP        = 0;
	static const UINT g_uOEMCP      = 0;
	static const UINT g_uMACCP      = 0;
	static const UINT g_uTHREAD_ACP = 0;
	static const UINT g_uSYMBOL     = 0;

	static const UINT g_uUTF7       = 0;
	static const UINT g_uUTF8       = 0;

#define ALCHEMY_STRING_TO_BASICSTRING StringToBasicString
	ALCHEMY_INLINE BASICSTRING& StringToBasicString(BASICSTRING& Output, UINT uCodePage, STRING& String)
	{
		return Output = String;
	}

#define ALCHEMY_BASICSTRING_TO_STRING BasicStringToString
	ALCHEMY_INLINE STRING& BasicStringToString(STRING& Output, UINT uCodePage, BASICSTRING& String)
	{
		return Output = String;
	}
#endif

	ALCHEMY_INLINE void StringCopy( CHAR** ppDest, const CHAR* pSource )
	{
		if(!ppDest || !pSource)
			return;

		UINT i, uLen = ALCHEMY_STRING_LENGTH(pSource);

		ALCHEMY_DEBUG_NEW(*ppDest, CHAR[uLen+1] );
		CHAR* pTemp = *ppDest;

		for(i=0; i<uLen; ++i)
			pTemp[i] = pSource[i];

		pTemp[i] = 0;
	}
}

#endif