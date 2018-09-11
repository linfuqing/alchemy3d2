/*
 *  AlchemyOutputStream.h
 *  IOS
 *
 *  Created by lin fuqing on 12-5-16.
 *  Copyright 2012 AstepGame. All rights reserved.
 *
 */

#include "AlchemyOutputStreamInterface.h"
#include "AlchemyString.h"
#include "AlchemySmartClass.h"

namespace alchemy
{
	IOutputStream& EndLine(IOutputStream&);
	class CStream 
	: public CSmartClass<IOutputStream> 
	{
	public:
		friend IOutputStream& EndLine(IOutputStream&);
		
		CStream();
		~CStream();
		
		IOutputStream& operator<<(const CHAR* pcString);
		IOutputStream& operator<<(CHAR cChar);
		IOutputStream& operator<<(UINT uNumber);
		IOutputStream& operator<<(INT nNumber);
		IOutputStream& operator<<(bool bBoolean);
		IOutputStream& operator<<(fnFunction& EndLine);
		
	private:
		STRING m_String;
	};
	
	ALCHEMY_INLINE IOutputStream& CStream::operator<<(const CHAR* pcString)
	{
		m_String += pcString;
		
		return *this;
	}
	
	ALCHEMY_INLINE IOutputStream& CStream::operator<<(CHAR cChar)
	{
		m_String += cChar;
		
		return *this;
	}
	
	ALCHEMY_INLINE IOutputStream& CStream::operator<<(UINT uNumber)
	{
		m_String += uNumber;
		
		return *this;
	}
	
	ALCHEMY_INLINE IOutputStream& CStream::operator<<(INT nNumber)
	{
		m_String += nNumber;
		
		return *this;
	}
	
	ALCHEMY_INLINE IOutputStream& CStream::operator<<(bool bBoolean)
	{
		m_String += bBoolean ? ALCHEMY_TEXT("true") : ALCHEMY_TEXT("false");
		
		return *this;
	}
	
	ALCHEMY_INLINE IOutputStream& CStream::operator<<(fnFunction& EndLine)
	{
		return EndLine(*this);
	}
}