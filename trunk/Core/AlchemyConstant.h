#ifndef ALCHEMY_CONSTANT_H
#define ALCHEMY_CONSTANT_H

#include "AlchemyDataType.h"
namespace alchemy
{
#define ALCHEMY_SHIFT(NUMBER) alchemy::SHIFTFUNCTION<NUMBER>::VALUE

	template<UINT NUMBER> 
	struct SHIFTFUNCTION
	{
		enum
		{
			VALUE = SHIFTFUNCTION<NUMBER / 2>::VALUE + 1
		};
	};

	template<> 
	struct SHIFTFUNCTION<1>
	{
		enum
		{
			VALUE = 0
		};
	};
}

#endif