// Test.cpp : 定义控制台应用程序的入口点。
//
#include <AS3.H>
//#include "stdafx.h"
//#include "../Core/AlchemyString.h"
//#include <stdio.h>
//#include "contains.h"

//using namespace alchemy;

//#define AS3_H
AS3_Val g_Object;
AS3_Val g_Function;

AS3_Val Init(void* pSelf, AS3_Val Args)
{
	AS3_Val Array;

	AS3_ArrayValue(Args, "AS3ValType", &Array);
	AS3_ArrayValue(Array, "AS3ValType, AS3ValType", &g_Object, &g_Function);

	return 0;
}

AS3_Val TestAS3(void* pSelf, AS3_Val Args)
{
	return AS3_CallT(g_Function, g_Object, "AS3ValType", g_Object);
}

int main(/*int argc, _TCHAR* argv[]*/)
{
	AS3_Val InitMethod    = AS3_Function(NULL, Init);
	AS3_Val TestAS3Method = AS3_Function(NULL, TestAS3);

	AS3_Val Result = AS3_Object("Init:AS3ValType, TestAS3:AS3ValType", InitMethod, TestAS3Method);

	AS3_Release(InitMethod);
	AS3_Release(TestAS3Method);

	AS3_LibInit( Result );

	return 0;
}

