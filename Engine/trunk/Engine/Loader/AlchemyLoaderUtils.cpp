#include "AlchemyLoaderUtils.h"

using namespace alchemy;

#ifdef ALCHEMY_UNICODE
BASICSTRING& alchemy::StringToBasicString(BASICSTRING& Output, UINT uCodePage, STRING& String)
{
	INT nLength = WideCharToMultiByte(uCodePage, 0, static_cast<const CHAR*>(String), String.GetLength() + 1, ALCHEMY_NULL, 0, ALCHEMY_NULL, ALCHEMY_NULL);
	char* pcBuffer;
	ALCHEMY_DEBUG_NEW(pcBuffer, char[nLength]);
	WideCharToMultiByte(uCodePage, 0, static_cast<const CHAR*>(String), String.GetLength() + 1, pcBuffer, nLength, ALCHEMY_NULL, ALCHEMY_NULL);

	Output = pcBuffer;

	ALCHEMY_DEBUG_DELETE_ARRAY(pcBuffer);

	return Output;
}

STRING& alchemy::BasicStringToString(STRING& Output, UINT uCodePage, BASICSTRING& String)
{
	INT nLength = MultiByteToWideChar(uCodePage, 0, static_cast<const char*>(String), String.GetLength() + 1, ALCHEMY_NULL, 0);
	CHAR* pcBuffer;
	ALCHEMY_DEBUG_NEW(pcBuffer, CHAR[nLength]);

	MultiByteToWideChar(uCodePage, 0, static_cast<const char*>(String), String.GetLength() + 1, pcBuffer, nLength);

	Output = pcBuffer;

	ALCHEMY_DEBUG_DELETE_ARRAY(pcBuffer);
	return Output;
}
#endif