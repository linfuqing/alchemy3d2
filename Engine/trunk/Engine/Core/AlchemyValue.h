#pragma once

#include "AlchemyDataType.h"

namespace alchemy
{
	template<typename Type, Type DefaultValue = 0>
	class CValue
	{
	public:
		CValue() : m_Value(DefaultValue) {}
		~CValue() {}

		ALCHEMY_INLINE operator const Type&()const
		{
			return m_Value;
		}

		ALCHEMY_INLINE operator Type&()
		{
			return m_Value;
		}
	private:
		Type m_Value;
	};
}