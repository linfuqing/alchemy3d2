#pragma once

#include "../Core/AlchemyInterface.h"

namespace alchemy
{
	ALCHEMY_INTERFACE(IFileWriter)
	{
		typedef enum
		{
			NORMAL, 
			BINARY
		}FORMAT;

		virtual UINT Open(const CHAR* pcPath, FORMAT Format, bool bIsForce) = 0;
		virtual bool Close(UINT uFile);
		virtual bool Set(UINT uFile, UINT uPosition) = 0;
		virtual UINT Write(UINT uFile, const void* pData, UINT uSize) = 0;
	};
}