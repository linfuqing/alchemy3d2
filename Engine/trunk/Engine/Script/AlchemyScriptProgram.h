#pragma once

#include "AlchemyScriptTypeDefinitionInterface.h"

#include "../Core/AlchemyMap.h"

namespace alchemy
{
	class CScriptProgram
	{
		typedef struct  
		{
			UINT uOpcode;
			UINT uDestination;
			UINT uDestinationType;
			UINT uSource1;
			UINT uSourceType1;
			UINT uSource2;
			UINT uSourceType2;
			void* pUserData;
		}INSTRUNCTION, * LPINSTRUNCTION;

		typedef struct
		{
			CScriptInstructionSet* pInstructionSet;
			UINT uJudgmentSource;
			UINT uJumpToInstructionSet;

			UINT uJumpToInstruction;
			UINT uFromInstruction;

			UINT uIndex;
		}INSTRUNCTIONSETINFO, * LPINSTRUNCTIONSETINFO;
	public:
		CScriptProgram(void);
		~CScriptProgram(void);

		UINT AppendInstruction(UINT uOpcode, UINT uDestination, UINT uDestinationType, UINT uSource1, UINT uSourceType1, UINT uSource2, UINT uSourceType2, void* pUserData = ALCHEMY_NULL);

		UINT JumpTo(UINT uJumpToInstruction, UINT uJudgmentSource = 0, UINT uFromInstruction = 0);

		void Run(CScriptMemery& Memery, IScriptTypeDefinition& TypeDefinition, CScriptInstructionSet::LPERRORINFO pErrorInfos = ALCHEMY_NULL, UINT uErrorInfoCount = 0);

	private:
		CVector<INSTRUNCTION> m_InstructionSet;
		CVector<INSTRUNCTIONSETINFO> m_InstructionSetInfos;
		CVector<UINT> m_InstructionSetInfoIndices;
		CMap<UINT, UINT> m_InstructionSetInfoMap;
		bool m_bIsDirty;
	};
}