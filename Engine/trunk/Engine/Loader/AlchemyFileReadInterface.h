#pragma once

#include "../Core/AlchemyInterface.h"

namespace alchemy
{
	ALCHEMY_INTERFACE(IFileReader)
	{
		virtual bool Open(const void* pData);
		virtual bool Close();
		virtual bool Set(UINT uPostion);

		virtual UINT Read(void* pOutput, UINT uSize) = 0;
	};
}