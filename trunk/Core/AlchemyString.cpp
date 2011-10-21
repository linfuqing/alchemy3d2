#include "AlchemyString.h"
#include "AlchemyUtils.h"

using namespace alchemy;

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