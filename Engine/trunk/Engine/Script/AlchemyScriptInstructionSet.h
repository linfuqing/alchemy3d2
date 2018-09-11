#pragma once

#include "AlchemyScriptMemery.h"

namespace alchemy
{
	struct IScriptDataType;
	struct IScriptTypeDefinition;
	class CScriptInstructionSet
	{
	public:
		typedef enum
		{
			UNKNOW = 0, 
			NO_CAST
		}ERRORCODE;

		typedef struct
		{
			ERRORCODE ErrorCode;

			void* pUsrData;
		}ERRORINFO, * LPERRORINFO;

		typedef struct INSTRUCTION
		{
			UINT uOpcode;
			UINT uDestination;
			UINT uSource1;
			UINT uSource2;

			void* pUserData;

			struct INSTRUCTION* pNext;
		}INSTRUCTION, * LPINSTRUCTION;

		CScriptInstructionSet(CScriptMemery& Memery, const IScriptTypeDefinition& TypeDefinition);
		~CScriptInstructionSet(void);

		bool AppendInstruction(UINT uOpcode, UINT uDestination, UINT uDestinationType, UINT uSource1, UINT uSourceType1, UINT uSource2, UINT uSourceType2, void* pUserData = ALCHEMY_NULL);

		bool Run(LPERRORINFO pErrorInfos = ALCHEMY_NULL, UINT uErrorInfoCount = 0);

		void Destroy();

	private:
		typedef struct
		{
			UINT uType;
			LPINSTRUCTION pHeadInstruction;
			LPINSTRUCTION pTailInstruction;
		}INSTRUCTIONINFO, * LPINSTRUCTIONINFO;

		void __InsertInstruction(UINT uType, UINT uOpcode, UINT uDestination, UINT uSource1, UINT uSource2, void* pUserData);

		CScriptMemery& m_Memery;
		const IScriptTypeDefinition& m_TypeDefinition;

		CVector<INSTRUCTIONINFO> m_InstructionSet;
	};
}