#pragma once

#include "AlchemyInterface.h"

namespace alchemy
{
	ALCHEMY_INTERFACE(IOutputStream)
	{
		typedef IOutputStream& fnFunction(IOutputStream&);

		virtual IOutputStream& operator<<(const CHAR* pcString) = 0;
		virtual IOutputStream& operator<<(CHAR cChar) = 0;
		virtual IOutputStream& operator<<(UINT uNumber) = 0;
		virtual IOutputStream& operator<<(INT nNumber) = 0;
		virtual IOutputStream& operator<<(bool bBoolean) = 0;
		virtual IOutputStream& operator<<(fnFunction& EndLine) = 0;
	};
}
