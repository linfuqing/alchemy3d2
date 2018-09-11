#pragma once

#include "AlchemyDebug.h"
#include "AlchemyNull.h"

namespace alchemy
{
#ifdef _MSC_VER
#pragma warning(disable : 4355)
#endif

	template<typename TYPE>
	class CSingleton
	{
	public:
		static TYPE& GetInstance()
		{
			ALCHEMY_DEBUG_ASSERT(sm_pInstance, "The singleton has not yet been created.");

			return *sm_pInstance;
		}

		static bool IsValuable()
		{
			return sm_pInstance != ALCHEMY_NULL;
		}

		CSingleton(TYPE& Object)
		{
			ALCHEMY_DEBUG_ASSERT(!sm_pInstance, "Only one instance of this class is permitted.");

			sm_pInstance = &Object;
		}

		~CSingleton(void)
		{
			ALCHEMY_DEBUG_ASSERT(sm_pInstance, "The singleton instance is invalid.");

			sm_pInstance = ALCHEMY_NULL;
		}

	private:
		static TYPE* sm_pInstance;
	};

	template<typename TYPE>
	TYPE* CSingleton<TYPE>::sm_pInstance = ALCHEMY_NULL;
}
