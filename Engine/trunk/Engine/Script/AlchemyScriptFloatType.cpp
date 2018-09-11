#include "AlchemyScriptFloatType.h"

using namespace alchemy;

CScriptFloatType::CScriptFloatType(void)
{
}

CScriptFloatType::~CScriptFloatType(void)
{
}

void CScriptFloatType::Compute(const CScriptInstructionSet::INSTRUCTION* pInstructionSet, CScriptMemery& Memery, const CScriptInstructionSet::ERRORINFO* pErrorInfos, UINT uErrorInfoCount)
{
	const CScriptInstructionSet::INSTRUCTION* pInstruction = pInstructionSet;
	while(pInstruction)
	{
		switch(pInstruction->uOpcode)
		{
		case MOV:
			break;
		case ADD:
			break;
		case NEG:
			break;
		case MUL:
			break;
		case RCP:
			break;

		case CAST_UINT:
			break;
		case CAST_INT:
			break;

		default:
			break;
		}

		pInstruction = pInstruction->pNext;
	}
}