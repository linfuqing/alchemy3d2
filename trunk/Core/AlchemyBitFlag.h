#ifndef ALCHEMY_BIT_FLAG_H
#define ALCHEMY_BIT_FLAG_H

#include "AlchemyUtils.h"

///
//比特工具
///
namespace alchemy
{
	///
	//比特工具宏
	///
#define ALCHEMY_TEST_FLAG(Value, Flag)             TestFlag(Value, Flag)

#define ALCHEMY_FLAG(x)                            ( 1 << (x) )
#define ALCHEMY_FLAG_VALUE(Bit, IsBit)             ( (IsBit) << (Bit) )
#define ALCHEMY_SET_FLAG(Value, Flag)              ( (Value) |= (Flag) )
#define ALCHEMY_CLEAR_FLAG(Value, Flag)            ( (Value) &= ~(Flag) )

#define ALCHEMY_MASK(x)                            ( ( 1 << ( (x) + 1 ) ) - 1 )
#define ALCHEMY_TEST_MASK(Value, MaxBit)           ( ALCHEMY_TEST_FLAG( Value, ALCHEMY_MASK(MaxBit) ) )
#define ALCHEMY_SET_MASK(Value, MaxBit)            ( (Value) | ALCHEMY_MASK(MaxBit) )  
#define ALCHEMY_CLEAR_MASK(Value, MaxBit)          ( ALCHEMY_CLEAR_FLAG( Value, ALCHEMY_MASK(MaxBit) ) )

#define ALCHEMY_MASK2(MaxBit, MinBit)              ( ALCHEMY_MASK(MaxBit) & ~ALCHEMY_MASK(MinBit) ) 
#define ALCHEMY_TEST_MASK2(Value, MaxBit, MinBit)  ( ALCHEMY_TEST_FLAG( Value, ALCHEMY_MASK2(MaxBit, MinBit) ) )
#define ALCHEMY_SET_MASK2(Value, MaxBit, MinBit)   ( (Value) | ALCHEMY_MASK2(MaxBit, MinBit) )
#define ALCHEMY_CLEAR_MASK2(Value, MaxBit, MinBit) ( ALCHEMY_CLEAR_FLAG( Value, ALCHEMY_MASK2(MaxBit, MinBit) ) )

#define ALCHEMY_TEST_BIT(Value, Bit)               ( ( (Value) & ALCHEMY_FLAG(Bit) ) !=0 )
#define ALCHEMY_SET_BIT(Value, Bit)                ( (Value) |= ALCHEMY_FLAG( (Bit) ) )
#define ALCHEMY_SET_BIT_VALUE(Value, Bit, IsSet)   ( (Value) |= ALCHEMY_FLAG_VALUE(Bit, IsSet) )
#define ALCHEMY_CLEAR_BIT(Value, Bit)              ( (Value) &= ~ALCHEMY_FLAG((Bit) ) )

#define ALCHEMY_TEST_ANY(Value, Set)               ( ( (Value) & (Set) ) != 0 )

#define ALCHEMY_TOTAL_BITS(Type)                   (sizeof(Type) << 3)

#define ALCHEMY_MAXINUM_BIT(Value)                 MaxinumBit(Value)

	template<typename T>
	ALCHEMY_INLINE bool TestFlag(T Value, T Flag)
	{
		return (Value & Flag) == Flag;
	}

	template<typename T>
	ALCHEMY_INLINE UINT MaxinumBit(T Number)
	{
		return MaxinumBit( static_cast<MAXUINT>(Number) );
	}

	template<>
	ALCHEMY_INLINE UINT MaxinumBit(UINT8 uNumber)
	{
		static const UINT8 s_uMAXINUM_BIT_TABLE[256] =
		{
			0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
			6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
			7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
			7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
			8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
			8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
			8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
			8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8
		};

		return s_uMAXINUM_BIT_TABLE[uNumber];
	}

	template<>
	ALCHEMY_INLINE UINT MaxinumBit(INT8 nNumber)
	{
		return MaxinumBit( static_cast<UINT8>( ALCHEMY_ABSOLUTE(nNumber) ) );
	}

	template<>
	ALCHEMY_INLINE UINT MaxinumBit(UINT16 uNumber)
	{
		UINT uResult = 0;

		if(uNumber > 0xFF)
		{
			uNumber >>= 8;
			uResult  += 8;
		}

		return uResult + MaxinumBit( static_cast<UINT8>(uNumber) ) ;
	}

	template<>
	ALCHEMY_INLINE UINT MaxinumBit(INT16 nNumber)
	{
		return MaxinumBit( static_cast<UINT16>( ALCHEMY_ABSOLUTE(nNumber) ) );
	}

	template<>
	ALCHEMY_INLINE UINT MaxinumBit(UINT32 uNumber)
	{
		UINT uResult = 0;

		if(uNumber > 0xFFFF)
		{
			uNumber >>= 16;
			uResult  += 16;
		}

		return uResult + MaxinumBit( static_cast<UINT16>(uNumber) ) ;
	}

	template<>
	ALCHEMY_INLINE UINT MaxinumBit(INT32 nNumber)
	{
		return MaxinumBit( static_cast<UINT32>( ALCHEMY_ABSOLUTE(nNumber) ) );
	}

	template<>
	ALCHEMY_INLINE UINT MaxinumBit(UINT64 uNumber)
	{
		UINT uResult = 0;

		if(uNumber > 0xFFFFFFFF)
		{
			uNumber >>= 32;
			uResult  += 32;
		}

		return uResult + MaxinumBit( static_cast<UINT32>(uNumber) );
	}

	template<>
	ALCHEMY_INLINE UINT MaxinumBit(INT64 nNumber)
	{
		return MaxinumBit( static_cast<UINT64>( ALCHEMY_ABSOLUTE(nNumber) ) );
	}

	template<typename T>
	ALCHEMY_INLINE UINT MininumBit(T uNumber)
	{
		return MininumBit( static_cast<MAXUINT>(uNumber) );
	}

	template<>
	ALCHEMY_INLINE UINT MininumBit(UINT8 uNumber)
	{
		return uNumber ? (MaxinumBit( static_cast<UINT8>( ( ( 1 << MaxinumBit(uNumber) ) - 1 ) & ~uNumber ) ) + 1) : 0;
	}

	template<>
	ALCHEMY_INLINE UINT MininumBit(INT8 nNumber)
	{
		return MininumBit( static_cast<UINT8>( ALCHEMY_ABSOLUTE(nNumber) ) );
	}

	template<>
	ALCHEMY_INLINE UINT MininumBit(UINT16 uNumber)
	{
		UINT uResult = 0;

		if( !(uNumber & 0xFF) )
		{
			uNumber >>= 8;
			uResult  += 8;
		}

		return uResult + MininumBit( static_cast<UINT8>(uNumber) ) ;
	}

	template<>
	ALCHEMY_INLINE UINT MininumBit(INT16 nNumber)
	{
		return MininumBit( static_cast<UINT16>( ALCHEMY_ABSOLUTE(nNumber) ) );
	}

	template<>
	ALCHEMY_INLINE UINT MininumBit(UINT32 uNumber)
	{
		UINT uResult = 0;

		if( !(uNumber & 0xFFFF) )
		{
			uNumber >>= 16;
			uResult  += 16;
		}

		return uResult + MininumBit( static_cast<UINT16>(uNumber) ) ;
	}

	template<>
	ALCHEMY_INLINE UINT MininumBit(INT32 nNumber)
	{
		return MininumBit( static_cast<UINT32>( ALCHEMY_ABSOLUTE(nNumber) ) );
	}

	template<>
	ALCHEMY_INLINE UINT MininumBit(UINT64 uNumber)
	{
		UINT uResult = 0;

		if( !(uNumber & 0xFFFFFFFF) )
		{
			uNumber >>= 32;
			uResult  += 32;
		}

		return uResult + MininumBit( static_cast<UINT32>(uNumber) ) ;
	}

	template<>
	ALCHEMY_INLINE UINT MininumBit(INT64 nNumber)
	{
		return MininumBit( static_cast<UINT64>( ALCHEMY_ABSOLUTE(nNumber) ) );
	}

	template<typename T>
	ALCHEMY_INLINE UINT Shift(T Number)
	{
		if(!Number)
			return 0;

		UINT uMaxinumBit = MaxinumBit(Number), uShift = uMaxinumBit - 1;

		if( (1 << uShift) == Number )
			return uShift;

		return uMaxinumBit;
	}
}

#endif