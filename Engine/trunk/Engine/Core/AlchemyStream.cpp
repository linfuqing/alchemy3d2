/*
 *  AlchemyOutputStream.cpp
 *  IOS
 *
 *  Created by lin fuqing on 12-5-16.
 *  Copyright 2012 AstepGame. All rights reserved.
 *
 */

#include "AlchemyStream.h"

using namespace alchemy;

CStream::CStream()
{
}

CStream::~CStream()
{
}

CStream::IOutputStream& alchemy::EndLine(IOutputStream& Stream)
{
	static_cast<CStream&>(Stream).m_String += ALCHEMY_TEXT('\n');
	
	return Stream;
}
