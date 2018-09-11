#ifndef ALCHEMY_UTILS_H
#define ALCHEMY_UTILS_H

#include "AlchemyMemory.h"

namespace alchemy
{
#define ALCHEMY_ABSOLUTE Absolute
#define ALCHEMY_BOUND    Bound
#define ALCHEMY_MAXINUM  Maxinum
#define ALCHEMY_MININUM  Mininum
#define ALCHEMY_SWAP     Swap
#define ALCHEMY_CLAMP    Clamp
#define ALCHEMY_CEIL     Ceil
#define ALCHEMY_FLOOR    Floor
#define ALCHEMY_FRACT    Fract
#define ALCHEMY_SIGN     Sign

	template<typename T>
	ALCHEMY_INLINE T Absolute(const T& x) 
	{
		return x < 0 ? - x : x;
	}

	template<typename T>
	ALCHEMY_INLINE T Absolute(T& x, T& y) 
	{
		return x > y ? (x - y) : (y - x);
	}

	template<typename T>
	ALCHEMY_INLINE const T& Bound(const T& x, const T& a, const T& b) 
	{
		return x < a ? a : (x > b ? b : x);
	}

	template<typename T>
	ALCHEMY_INLINE const T& Maxinum(const T& a, const T& b) 
	{
		return a > b ? a : b;
	}

	template<typename T>
	ALCHEMY_INLINE const T& Mininum(const T& a, const T& b) 
	{
		return a < b ? a : b;
	}

	template<typename T>
	ALCHEMY_INLINE void Swap(T& a, T& b) 
	{
		T t;

		t=a; 
		a=b; 
		b=t;
	}

	template<typename T>
	ALCHEMY_INLINE const T& Clamp(const T& Value, const T& Min, const T& Max)
	{
		return Value > Max ? Max : (Value < Min ? Min : Value);
	}

	ALCHEMY_INLINE INT Floor(FLOAT x)
	{
		return static_cast<INT>( x > 0 ? x : (x < 0 ? (x < static_cast<INT>(x) ? (x - 1.0f) : x) : 0) );
	}
	
	ALCHEMY_INLINE FLOAT Fract(FLOAT x)
	{
		return x - Floor(x);
	}
	
	ALCHEMY_INLINE INT Ceil(FLOAT x)
	{
		return static_cast<INT>( x < 0 ? x : (x > 0 ? (x > static_cast<INT>(x) ? (x + 1.0f) : x) : 0) );
	}
	
	ALCHEMY_INLINE INT Sign(FLOAT x)
	{
		return x > 0 ? 1 : (x < 0 ? - 1 : 0);
	}

	template<typename T, typename LESSFUNCTION>
	void Sort(T Output[], const T Input[], UINT uLength)
	{
		LESSFUNCTION LessFunction;
		UINT i, j, uStage = 1;
		T Temp;

		if(Output != Input)
			ALCHEMY_MEMORY_COPY(Output, Input, sizeof(T) * uLength);

		while ((uStage * 3 + 1) < uLength) 
			uStage = 3 * uStage + 1;

		while(uStage > 0) 
		{
			for (i = uStage - 1; i < uLength; i ++) 
			{
				Temp = Output[i];

				for(j = i; (j >= uStage) && LessFunction(Temp, Output[j - uStage]); j -= uStage) 
					Output[j] = Output[j - uStage];
	            
				Output[j] = Temp;
			}

			uStage = uStage / 3;
		}
	} 

	UINT Digital(UINT uNumber);
	UINT Digital(INT nNumber);
}

#endif