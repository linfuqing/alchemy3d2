#pragma once

#include "AlchemyFlashObjectInterface.h"

namespace alchemy
{
	class CFlashObject :
		public CFlashObjectInterface
	{
	public:
		CFlashObject(void);
		~CFlashObject(void);

		operator const AS3TYPE()const;

#ifdef AS3_H
		virtual void Fill(AS3TYPE Object, AS3TYPE MethodArray) = 0;
#endif

	protected:
		AS3TYPE m_Object;
	};

	ALCHEMY_INLINE CFlashObject::operator const AS3TYPE()const
	{
#ifdef AS3_H
		AS3_Acquire(m_Object);
#endif

		return m_Object;
	}
}