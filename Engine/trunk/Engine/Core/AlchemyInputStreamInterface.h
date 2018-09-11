/*
 *  AlchemyInputStreamInterface.h
 *  IOS
 *
 *  Created by lin fuqing on 12-5-16.
 *  Copyright 2012 AstepGame. All rights reserved.
 *
 */


#include "AlchemyInterface.h"

namespace alchemy
{
	ALCHEMY_INTERFACE(IInputStream)
	{
		virtual IInputStream& operator>>(CHAR*& pcString) = 0;
		virtual IInputStream& operator>>(CHAR& cChar) = 0;
		virtual IInputStream& operator>>(UINT& uNumber) = 0;
		virtual IInputStream& operator>>(INT& nNumber) = 0;
		virtual IInputStream& operator>>(bool& bBoolean) = 0;
	};
}
