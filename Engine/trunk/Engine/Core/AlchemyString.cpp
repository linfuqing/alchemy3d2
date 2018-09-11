#include "AlchemyString.h"
#include "AlchemyUtils.h"

using namespace alchemy;

/*UINT alchemy::StringLength(const CHAR* pcString)
{	
	UINT uLength = 0;
	while( *(pcString ++) )
		++ uLength;
		
	return uLength;
}

bool alchemy::StringFind(UINT& uOutputIndex, const CHAR* pcString, CHAR cCharacter)
{
	UINT uIndex = 0;
	while(*pcString)
	{
		if(*pcString == cCharacter)
		{
			uOutputIndex = uIndex;
			
			return true;
		}
		
		++ uIndex;
		++ pcString;
	}
	
	uOutputIndex = uIndex;
	
	return false;
}

FLOAT alchemy::StringFloat(const CHAR* pcString)
{
	FLOAT fResult = 0.0f, fDigital = 1.0f, fIsPositive = 1;
	CHAR cTemp;
	UINT uIndex;
	
	if( StringFind( uIndex, pcString, ALCHEMY_TEXT('-') ) )
	{
		pcString += uIndex;
		
		fIsPositive = - 1;
	}
	
	if( StringFind( uIndex, pcString, ALCHEMY_TEXT('.') ) )
	{
		for(UINT i = uIndex - 1; i > 0; -- i)
		{
			cTemp = pcString[i];
			if( cTemp >= ALCHEMY_TEXT('0') && cTemp <= ALCHEMY_TEXT('9') )
			{
				fResult += fDigital * (cTemp - ALCHEMY_TEXT('0') );
				
				fDigital *= 10.0f;
			}
		}
		
		cTemp = *pcString;
		if( cTemp >= ALCHEMY_TEXT('0') && cTemp <= ALCHEMY_TEXT('9') )
			fResult += fDigital * ( cTemp - ALCHEMY_TEXT('0') );
		
		fDigital = 0.1f;
		
		pcString += uIndex + 1;
		while(cTemp = *pcString)
		{
			if( cTemp >= ALCHEMY_TEXT('0') && cTemp <= ALCHEMY_TEXT('9') )
			{
				fResult += fDigital * ( cTemp - ALCHEMY_TEXT('0') );
				
				fDigital *= 0.1f;
			}
				
			++ pcString;
		}
	}
	else
	{
		for(UINT i = uIndex - 1; i > 0; -- i)
		{
			cTemp = pcString[i];
			if( cTemp >= ALCHEMY_TEXT('0') && cTemp <= ALCHEMY_TEXT('9') )
			{
				fResult += fDigital * ( cTemp - ALCHEMY_TEXT('0') );
				
				fDigital *= 10.0f;
			}
		}
		
		cTemp = *pcString;
		if( cTemp >= ALCHEMY_TEXT('0') && cTemp <= ALCHEMY_TEXT('9') )
			fResult += fDigital * ( cTemp - ALCHEMY_TEXT('0') );
	}
	
	return fResult * fIsPositive;
}

INT alchemy::StringInteger(const CHAR* pcString)
{
	INT nResult = 0, nDigital = 1, nIsPositive = 1;
	UINT uIndex;
	CHAR cTemp;
	if( StringFind( uIndex, pcString, ALCHEMY_TEXT('-') ) )
	{
		pcString += uIndex;
		
		nIsPositive = - 1;
	}
	
	for(UINT i = StringLength(pcString) - 1; i > 0; -- i)
	{
		cTemp = pcString[i];
		if( cTemp >= ALCHEMY_TEXT('0') && cTemp <= ALCHEMY_TEXT('9') )
		{
			nResult += nDigital * ( cTemp - ALCHEMY_TEXT('0') );
			
			nDigital *= 10;
		}
	}
	
	cTemp = *pcString;
	if( cTemp >= ALCHEMY_TEXT('0') && cTemp <= ALCHEMY_TEXT('9') )
		nResult += nDigital * ( cTemp - ALCHEMY_TEXT('0') );
		
	return nResult * nIsPositive;
}

UINT alchemy::StringUnsignInteger(const CHAR* pcString)
{
	UINT uResult = 0, uDigital = 1;
	CHAR cTemp;
	
	for(UINT i = StringLength(pcString) - 1; i > 0; -- i)
	{
		cTemp = pcString[i];
		if( cTemp >= ALCHEMY_TEXT('0') && cTemp <= ALCHEMY_TEXT('9') )
		{
			uResult += uDigital * ( cTemp - ALCHEMY_TEXT('0') );
			
			uDigital *= 10;
		}
	}
	
	cTemp = *pcString;
	if( cTemp >= ALCHEMY_TEXT('0') && cTemp <= ALCHEMY_TEXT('9') )
		uResult += uDigital * ( cTemp - ALCHEMY_TEXT('0') );
		
	return uResult;
}

STRING& alchemy::operator+=(STRING& String, UINT uNumber)
{
	if(uNumber)
	{
		UINT uDigital = Digital(uNumber), uTemp = 0, uCurrent, uMedian, i, j;

		for(i = uDigital; i > 0; -- i)
		{
			uMedian = 1;
			for(j = 1; j < i; ++ j)
				uMedian *= 10;

			uCurrent = (uNumber - uTemp) / uMedian;
			String += static_cast<const CHAR>( uCurrent + ALCHEMY_TEXT('0') );

			uTemp += uCurrent * uMedian;
		}
	}
	else
		String += ALCHEMY_TEXT('0');

	return String;
}

STRING& alchemy::operator+=(STRING& String, INT nNumber)
{
	if(nNumber)
	{
		if(nNumber < 0)
		{
			String += ALCHEMY_TEXT('-');

			nNumber = - nNumber;
		}

		UINT uDigital = Digital(nNumber), uTemp = 0, uCurrent, uMedian, i, j;

		for(i = uDigital; i > 0; -- i)
		{
			uMedian = 1;
			for(j = 1; j < i; ++ j)
				uMedian *= 10;

			uCurrent = (nNumber - uTemp) / uMedian;
			String += static_cast<const CHAR>( uCurrent + ALCHEMY_TEXT('0') );

			uTemp += uCurrent * uMedian;
		}
	}
	else
		String += ALCHEMY_TEXT('0');

	return String;
}

STRING& alchemy::StringAppend(STRING& String, const char* pcString, UINT uFrom, UINT uLength)
{
	while(*pcString)
	{
		String += static_cast<CHAR>(*pcString);
		
		++ pcString;
	}
	
	return String;
}

STRING& alchemy::StringAssign(STRING& String, const char* pcString, UINT uFrom, UINT uLength)
{
	String = ALCHEMY_NULL;
	
	while(*pcString)
	{
		String += static_cast<CHAR>(*pcString);
		
		++ pcString;
	}
	
	return String;
}*/

/*FLOAT alchemy::StringFloat(const STRING& String)
{
	FLOAT fResult = 0.0f, fDigital = 1.0f, fIsPositive = 1.0f;
	CHAR cTemp;
	UINT uIndex = String.GetIndexOf( ALCHEMY_TEXT('-') ), uLength = String.GetLength(), i;
	
	if(uIndex < uLength)
		fIsPositive = - 1.0f;
	else 
		uIndex = 0;
	
	uIndex = uIndex + String.GetIndexOf( ALCHEMY_TEXT('.') );
	if(uIndex < uLength)
	{
		for(i = uIndex - 1; i > 0; -- i)
		{
			cTemp = String[i];
			if( cTemp >= ALCHEMY_TEXT('0') && cTemp <= ALCHEMY_TEXT('9') )
			{
				fResult += fDigital * (cTemp - ALCHEMY_TEXT('0') );
				
				fDigital *= 10.0f;
			}
		}
		
		cTemp = String[0];
		if( cTemp >= ALCHEMY_TEXT('0') && cTemp <= ALCHEMY_TEXT('9') )
			fResult += fDigital * ( cTemp - ALCHEMY_TEXT('0') );
		
		fDigital = 0.1f;
		
		for(i = uIndex + 1; i < uLength; ++ i)
		{
			cTemp = String[i];
			if( cTemp >= ALCHEMY_TEXT('0') && cTemp <= ALCHEMY_TEXT('9') )
			{
				fResult += fDigital * ( cTemp - ALCHEMY_TEXT('0') );
				
				fDigital *= 0.1f;
			}
		}
	}
	else
	{
		for(i = uIndex - 1; i > 0; -- i)
		{
			cTemp = String[i];
			if( cTemp >= ALCHEMY_TEXT('0') && cTemp <= ALCHEMY_TEXT('9') )
			{
				fResult += fDigital * ( cTemp - ALCHEMY_TEXT('0') );
				
				fDigital *= 10.0f;
			}
		}
		
		cTemp = String[0];
		if( cTemp >= ALCHEMY_TEXT('0') && cTemp <= ALCHEMY_TEXT('9') )
			fResult += fDigital * ( cTemp - ALCHEMY_TEXT('0') );
	}
	
	return fResult * fIsPositive;

}

INT alchemy::StringInteger(const STRING& String)
{
	INT nResult = 0, nDigital = 1, nIsPositive = 1;
	UINT uIndex = String.GetIndexOf( ALCHEMY_TEXT('-') ), uLength = String.GetLength();
	CHAR cTemp;
	if(uIndex < uLength)
		nIsPositive = - 1;
	else
		uIndex = 0;
	
	for(UINT i = uLength - uIndex - 1; i > 0; -- i)
	{
		cTemp = String[i];
		if( cTemp >= ALCHEMY_TEXT('0') && cTemp <= ALCHEMY_TEXT('9') )
		{
			nResult += nDigital * ( cTemp - ALCHEMY_TEXT('0') );
			
			nDigital *= 10;
		}
	}
	
	cTemp = String[0];
	if( cTemp >= ALCHEMY_TEXT('0') && cTemp <= ALCHEMY_TEXT('9') )
		nResult += nDigital * ( cTemp - ALCHEMY_TEXT('0') );
		
	return nResult * nIsPositive;
}

UINT alchemy::StringUnsignInteger(const STRING& String)
{
	UINT uResult = 0, uDigital = 1;
	CHAR cTemp;
	
	for(UINT i = String.GetLength() - 1; i > 0; -- i)
	{
		cTemp = String[i];
		if( cTemp >= ALCHEMY_TEXT('0') && cTemp <= ALCHEMY_TEXT('9') )
		{
			uResult += uDigital * ( cTemp - ALCHEMY_TEXT('0') );
			
			uDigital *= 10;
		}
	}
	
	cTemp = String[0];
	if( cTemp >= ALCHEMY_TEXT('0') && cTemp <= ALCHEMY_TEXT('9') )
		uResult += uDigital * ( cTemp - ALCHEMY_TEXT('0') );
		
	return uResult;
}*/