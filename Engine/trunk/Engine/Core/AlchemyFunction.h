#ifndef ALCHEMY_FUNCTION
#define ALCHEMY_FUNCTION

#include "AlchemyDebug.h"

namespace alchemy
{
	template<typename T>
	struct BASICLESS
	{
		bool operator()(const T& Left, const T& Right)const
		{
			return Left < Right;
		}
	};

	template<typename T>
	struct BASICEQUAL
	{
		bool operator()(const T& Left, const T& Right)const
		{
			return Left == Right;
		}
	};

	template<typename T>
	struct BASICALLOC
	{
		T* operator()(UINT uCount)
		{
			T* pData;
			ALCHEMY_DEBUG_NEW(pData, T[uCount]);

			return pData;
		}
	};
}

#endif