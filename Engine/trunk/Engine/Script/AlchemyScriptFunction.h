#pragma once

#include "AlchemyScriptDataTypeInterface.h"

namespace alchemy
{
	class CScriptFunction
	{
		typedef struct
		{
			UINT uType;
			UINT uCount;
			bool bIsHeap;
			const void* pData;
		}VARIABLE;

		typedef struct
		{
			UINT uOpcode;
			UINT uDestination;
			UINT uSource1;
			UINT uSource2;
			void* pUserData;
		}INSTRUCTION;
	public:
		CScriptFunction(void);
		~CScriptFunction(void);

		UINT Alloc(UINT uType, UINT uCount, bool bIsHeap, const void* pData = ALCHEMY_NULL);

		UINT AppendInstruction(UINT uOpcode, UINT uDestination, UINT uSource1, UINT uSource2, void* pUserData = ALCHEMY_NULL);
		UINT Alloc(UINT uDestination, UINT uSizeSource);

		UINT InstructionBeginIf(UINT uSource);
		UINT InstructionElseIf(UINT uSource);
		UINT InstructionElse();
		UINT InstructionEndIf();

		UINT InstructionBeginWhile(UINT uSource);
		UINT InstructionEndWhile();

	private:
		CPool<VARIABLE> m_VariablePool;
		CPool<INSTRUCTION> m_InstructionPool;
	};
}