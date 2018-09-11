/* This is a C++-ified version of the Mersenne Twister pseudo-random number
generator based on a recode by Shawn Cokus. I didn't touch the original 
code, the only changes I made were related to the task of wrapping it into
a C++ class (renaming of functions and variables, changing them to class
members, etc.).
Martin Hinsch (mhinsch@usf.uni-osnabrueck.de), April 4, 1999.

5.6.99 - Changed macro names to begin with a '__MTRAND_'.

This library is free software; you can redistribute it and/or modify it
under the terms of the GNU Library General Public License as published by
the Free Software Foundation (either version 2 of the License or, at your
option, any later version).  This library is distributed in the hope that
it will be useful, but WITHOUT ANY WARRANTY, without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
the GNU Library General Public License for more details.  You should have
received a copy of the GNU Library General Public License along with this
library; if not, write to the Free Software Foundation, Inc., 59 Temple

Place, Suite 330, Boston, MA 02111-1307, USA.
*/


#include "AlchemyMTRandom.h"

using namespace alchemy;

CMTRand CMTRand::sm_Rand;

UINT32 CMTRand::sm_uLastSeed = 1;

CMTRand::CMTRand(UINT32 uSeed)
{
	//
	// We initialize state[0..(N-1)] via the generator
	//
	//   x_new = (69069 * x_old) mod 2^32
	//
	// from Line 15 of Table 1, p. 106, Sec. 3.3.4 of Knuth's
	// _The Art of Computer Programming_, Volume 2, 3rd ed.
	//
	// Notes (SJC): I do not know what the initial state requirements
	// of the Mersenne Twister are, but it seems this seeding generator
	// could be better.  It achieves the maximum period for its modulus
	// (2^30) iff x_initial is odd (p. 20-21, Sec. 3.2.1.2, Knuth); if
	// x_initial can be even, you have sequences like 0, 0, 0, ...;
	// 2^31, 2^31, 2^31, ...; 2^30, 2^30, 2^30, ...; 2^29, 2^29 + 2^31,
	// 2^29, 2^29 + 2^31, ..., etc. so I force seed to be odd below.
	//
	// Even if x_initial is odd, if x_initial is 1 mod 4 then
	//
	//   the          lowest bit of x is always 1,
	//   the  next-to-lowest bit of x is always 0,
	//   the 2nd-from-lowest bit of x alternates      ... 0 1 0 1 0 1 0 1 ... ,
	//   the 3rd-from-lowest bit of x 4-cycles        ... 0 1 1 0 0 1 1 0 ... ,
	//   the 4th-from-lowest bit of x has the 8-cycle ... 0 0 0 1 1 1 1 0 ... ,
	//    ...
	//
	// and if x_initial is 3 mod 4 then
	//
	//   the          lowest bit of x is always 1,
	//   the  next-to-lowest bit of x is always 1,
	//   the 2nd-from-lowest bit of x alternates      ... 0 1 0 1 0 1 0 1 ... ,
	//   the 3rd-from-lowest bit of x 4-cycles        ... 0 0 1 1 0 0 1 1 ... ,
	//   the 4th-from-lowest bit of x has the 8-cycle ... 0 0 1 1 1 1 0 0 ... ,
	//    ...
	//
	// The generator's potency (min. s>=0 with (69069-1)^s = 0 mod 2^32) is
	// 16, which seems to be alright by p. 25, Sec. 3.2.1.3 of Knuth.  It
	// also does well in the dimension 2..5 spectral tests, but it could be
	// better in dimension 6 (Line 15, Table 1, p. 106, Sec. 3.3.4, Knuth).
	//
	// Note that the random number user does not see the values generated
	// here directly since reloadMT() will always munge them first, so maybe
	// none of all of this matters.  In fact, the seed values made here could
	// even be extra-special desirable if the Mersenne Twister theory says
	// so-- that's why the only change I made is to restrict to odd seeds.
	//

	m_nLeft = - 1;
	Seed(sm_uLastSeed = uSeed);
}

CMTRand::CMTRand()
{
	m_nLeft = - 1;
	Seed(sm_uLastSeed += 2);
}

void CMTRand::Seed(UINT32 uSeed)
{
	register UINT32 x = (uSeed | 1) & 0xFFFFFFFF, * puState = m_uState;
	register INT32 j;

	for(m_nLeft = 0, *puState ++ = x, j = N; -- j; *puState ++ = (x *= 69069) & 0xFFFFFFFF);
}

UINT32 CMTRand::Rand32()
{
	UINT32 y;

	if(-- m_nLeft < 0)
		return( __Reload() );

	y  = *m_puNext ++;
	y ^= (y >> 11);
	y ^= (y <<  7) & 0x9D2C5680;
	y ^= (y << 15) & 0xEFC60000;

	return( y ^ (y >> 18) );
}

FLOAT CMTRand::Rand()
{
	return static_cast<FLOAT>( Rand32() ) / 0xffffffff;
}

FLOAT CMTRand::Rand(FLOAT fLow, FLOAT fHeight)
{
	return (Rand() * (fHeight - fLow) + fLow);
}

UINT32 CMTRand::__Reload()
{
#define ALCHEMY_MIX_BITS(u, v) ( ( HIGH_BIT & (u) ) | ( LOW_BITS & (v) ) )

	register UINT32 *pState0 = m_uState, *pState2 = m_uState + 2, *pM = m_uState + M, uState0, uState1;
	register INT32 i;

	if(m_nLeft < - 1)
		Seed(4357);

	m_nLeft = N - 1, m_puNext = m_uState + 1;

	for(uState0 = m_uState[0], uState1 = m_uState[1], i =N - M + 1; -- i; uState0 = uState1, uState1 = *pState2 ++)
		*pState0 ++ = *pM ++ ^ (ALCHEMY_MIX_BITS(uState0, uState1) >> 1) ^ (LOW_BIT & uState1 ? K : 0);

	for(pM = m_uState, i = M; -- i; uState0 = uState1, uState1 = *pState2 ++)
		*pState0 ++ = *pM ++ ^ (ALCHEMY_MIX_BITS(uState0, uState1) >> 1) ^ (LOW_BIT & uState1 ? K : 0);

	uState1 = m_uState[0];
	*pState0 = *pM ^ (ALCHEMY_MIX_BITS(uState0, uState1) >> 1) ^ (LOW_BIT & uState1 ? K : 0);

	uState1 ^= (uState1 >> 11);
	uState1 ^= (uState1 <<  7) & 0x9D2C5680;
	uState1 ^= (uState1 << 15) & 0xEFC60000;

	return( uState1 ^ (uState1 >> 18) );

#undef ALCHEMY_MIX_BITS
}