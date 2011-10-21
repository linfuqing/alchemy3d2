#pragma once

#include "../Core/AlchemyDataType.h"

namespace alchemy
{
#ifndef AS3_H
	typedef INT AS3TYPE;
#define ALCHEMY_FLASH_CALL(Method, Object)
#else
	typedef AS3_Val AS3TYPE;
#define ALCHEMY_FLASH_CALL(Method, Object) AS3_Call(Method, Object, ALCHEMY_NULL)
#endif

	class CFlashObjectInterface
	{
	public:
		CFlashObjectInterface(void) {}
		virtual ~CFlashObjectInterface(void) {}

#ifdef AS3_H
		virtual AS3TYPE Init() = 0;
#endif
	};
}
