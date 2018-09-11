#include "AlchemyUtils.h"

using namespace alchemy;

UINT alchemy::Digital(UINT uNumber)
{
	UINT uDigital = 0;

	while(uNumber)
	{
		++ uDigital;

		uNumber /= 10;
	}

	return uDigital;
}

UINT alchemy::Digital(INT nNumber)
{
	UINT uDigital = 0;

	while(nNumber)
	{
		++ uDigital;

		nNumber /= 10;
	}

	return uDigital;
}