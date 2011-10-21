#pragma once

#include "AlchemyDataType.h"

namespace alchemy
{
	class COutputStreamInterface
	{
	public:
		typedef COutputStreamInterface& fnFunction(COutputStreamInterface&);

		COutputStreamInterface(void);
		virtual ~COutputStreamInterface(void);

		virtual COutputStreamInterface& operator<<(const CHAR* pcString) = 0;
		virtual COutputStreamInterface& operator<<(CHAR cChar) = 0;
		virtual COutputStreamInterface& operator<<(UINT uNumber) = 0;
		virtual COutputStreamInterface& operator<<(INT nNumber) = 0;
		virtual COutputStreamInterface& operator<<(bool bBoolean) = 0;
		virtual COutputStreamInterface& operator<<(fnFunction& EndLine) = 0;
	};
}
