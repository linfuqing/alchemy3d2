#pragma once

#include "AlchemyScriptDataTypeInterface.h"

#include "../Core/AlchemySmartClass.h"

namespace alchemy
{
	class CScriptFloatType : 
		public CSmartClass<IScriptDataType>
	{
	public:
		typedef enum
		{
			INVALID = 0, 

			MOV, 
			ADD,
			NEG,
			MUL, 
			RCP, 

			CAST_UINT, 
			CAST_INT
		}OPCODE;

		CScriptFloatType(void);
		~CScriptFloatType(void);

		UINT GetSize()const;

		void Compute(const CScriptInstructionSet::INSTRUCTION* pInstructionSet, CScriptMemery& Memery, const CScriptInstructionSet::ERRORINFO* pErrorInfos, UINT uErrorInfoCount);
	};

	ALCHEMY_INLINE UINT CScriptFloatType::GetSize()const
	{
		return sizeof(FLOAT);
	}
}
