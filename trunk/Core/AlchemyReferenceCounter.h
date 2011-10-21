#pragma once

#include "../Core/AlchemyDataType.h"

namespace alchemy
{
	class CReferenceCounter
	{
	public:
		CReferenceCounter(void);
		~CReferenceCounter(void);

		INT GetReferenceCount()const;

		virtual INT AddRef();
		virtual INT Release();

	protected:
		INT m_nReferenceCount;
	};

	ALCHEMY_INLINE INT CReferenceCounter::GetReferenceCount()const
	{
		return m_nReferenceCount;
	}
}
