#ifndef ALCHEMY_CONSTANT_H
#define ALCHEMY_CONSTANT_H

#include "AlchemyDataType.h"
namespace alchemy
{
#define ALCHEMY_CONSTANT_SHIFT(NUMBER) alchemy::SHIFTFUNCTION<NUMBER>::VALUE
#define ALCHEMY_CONSTANT_IS_SAME_TYPE(SOURCE, DESTINATION) alchemy::ISSAMETYPE<SOURCE, DESTINATION>::VALUE

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

	template<typename SOURCE, typename DESTINATION> // »ù±¾Ä£°å  
	struct ISSAMETYPE
	{
		enum 
		{
			VALUE = ALCHEMY_FALSE
		};
	};

	template<typename T>
	struct ISSAMETYPE<T, T>
	{  
		enum 
		{
			VALUE = ALCHEMY_TRUE
		};
	};  
}

#endif