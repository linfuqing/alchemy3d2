// Test.cpp : 定义控制台应用程序的入口点。
//
#include <AS3.H>
//#include "stdafx.h"
//#include "../Core/AlchemyString.h"
//#include <stdio.h>
//#include "contains.h"

//using namespace alchemy;

//#define AS3_H
typedef struct TEST
{
	int nValue;
	float fValue;
}TEST;

TEST g_Test = {1, 0.25};

const TEST& GetTestAddress()
{
	return g_Test;
}

const TEST* GetTestPointer()
{
	return &g_Test;
}

AS3_Val g_Object;
AS3_Val g_Function;
int g_nCount;
int g_nValue;

AS3_Val Init(void* pSelf, AS3_Val Args)
{
	AS3_Val Array;

	AS3_ArrayValue(Args, "AS3ValType", &Array);
	AS3_ArrayValue(Array, "AS3ValType, AS3ValType, IntType", &g_Object, &g_Function, &g_nCount);

	return AS3_Ptr(&g_nValue);
}

AS3_Val TestAS3CallCPP(void* pSelf, AS3_Val Args)
{
	//AS3_Call(g_Function, g_Object, 0);
	++ g_nValue;

	int i, nValue = 0 ;
	float fValue = 0;
	const TEST* pTest;

	if(g_nValue % 2)
	{
		for(i = 0; i < g_nCount; ++ i)
		{
			pTest = GetTestPointer();

			nValue += pTest->nValue;
		}
	}
	else
	{
		for(i = 0; i < g_nCount; ++ i)
		{
			pTest = &GetTestAddress();

			nValue += pTest->nValue;
		}
	}

	AS3_Trace(AS3_Int(nValue));

	return 0;
}

AS3_Val TestCPPCallAS3(void* pSelf, AS3_Val Args)
{
	for(int i = 0; i < g_nCount; ++ i)
		AS3_Call(g_Function, g_Object, 0);

	return 0;
}

int main(/*int argc, _TCHAR* argv[]*/)
{
	AS3_Val InitMethod    = AS3_Function(NULL, Init);
	AS3_Val TestAS3CallCPPMethod = AS3_Function(NULL, TestAS3CallCPP);
	AS3_Val TestCPPCallAS3Method = AS3_Function(NULL, TestCPPCallAS3);

	AS3_Val Result = AS3_Object("Init:AS3ValType, TestAS3CallCPP:AS3ValType, TestCPPCallAS3:AS3ValType", InitMethod, TestAS3CallCPPMethod, TestCPPCallAS3Method);

	AS3_Release(InitMethod);
	AS3_Release(TestAS3CallCPPMethod);
	AS3_Release(TestCPPCallAS3Method);

	AS3_LibInit( Result );

	return 0;
}

