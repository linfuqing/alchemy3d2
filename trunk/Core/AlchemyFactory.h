#pragma once

#include "AlchemyString.h"
namespace alchemy
{
	template<typename TYPE>
	class CFactory
	{
	public:
		bool Create(TYPE** ppInstance) = 0;
	};
}
