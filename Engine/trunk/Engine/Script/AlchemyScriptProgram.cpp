#include "AlchemyScriptProgram.h"
#include "AlchemyScriptDataTypeInterface.h"

using namespace alchemy;

CScriptProgram::CScriptProgram(void) : 
m_bIsDirty(false)
{
}

CScriptProgram::~CScriptProgram(void)
{
}

UINT CScriptProgram::AppendInstruction(UINT uOpcode, UINT uDestination, UINT uDestinationType, UINT uSource1, UINT uSourceType1, UINT uSource2, UINT uSourceType2, void* pUserData)
{
	INSTRUNCTION Instruction;

	Instruction.uOpcode = uOpcode;
	Instruction.uDestination = uDestination;
	Instruction.uDestinationType = uDestinationType;
	Instruction.uSource1 = uSource1;
	Instruction.uSourceType1 = uSourceType1;
	Instruction.uSource2 = uSource2;
	Instruction.uSourceType2 = uSourceType2;
	Instruction.pUserData = pUserData;

	m_bIsDirty = true;

	return m_InstructionSet.Add(Instruction) + 1;
}

UINT CScriptProgram::JumpTo(UINT uJumpToInstruction, UINT uJudgmentSource, UINT uFromInstruction)
{
	UINT uLength = m_InstructionSet.GetLength();
	uFromInstruction = uFromInstruction ? uFromInstruction : uLength;
	if(uLength && uLength >= uJumpToInstruction && uLength >= uFromInstruction)
	{
		INSTRUNCTIONSETINFO InstructionSetInfo;
		LPINSTRUNCTIONSETINFO pInstructionSetInfo;

		const UINT* puInstructionSetInfo = m_InstructionSetInfoMap.Get(uFromInstruction);
		UINT uResult = puInstructionSetInfo ? *puInstructionSetInfo : 0;
		if(uResult)
		{
			pInstructionSetInfo = m_InstructionSetInfos.Get(uResult);
			pInstructionSetInfo->uJudgmentSource = uJudgmentSource;
			pInstructionSetInfo->uJumpToInstruction = uJumpToInstruction ? uJumpToInstruction : ( pInstructionSetInfo->uJumpToInstruction ? pInstructionSetInfo->uJumpToInstruction : (uFromInstruction + 1) );
		}
		else
		{
			InstructionSetInfo.pInstructionSet = ALCHEMY_NULL;
			InstructionSetInfo.uJudgmentSource = uJudgmentSource;
			InstructionSetInfo.uJumpToInstructionSet = 0;
			InstructionSetInfo.uJumpToInstruction = uJumpToInstruction ? uJumpToInstruction : (uFromInstruction + 1);
			InstructionSetInfo.uFromInstruction = uFromInstruction;

			uResult = m_InstructionSetInfos.Add(InstructionSetInfo);

			pInstructionSetInfo = m_InstructionSetInfos.Get(uResult ++);

			m_InstructionSetInfoMap.Insert(uFromInstruction, uResult);
		}

		puInstructionSetInfo = uJumpToInstruction ? m_InstructionSetInfoMap.Get(uJumpToInstruction) : ALCHEMY_NULL;
		if(!puInstructionSetInfo || !(pInstructionSetInfo->uJumpToInstructionSet = *puInstructionSetInfo) && uJumpToInstruction)
		{
			InstructionSetInfo.pInstructionSet = ALCHEMY_NULL;
			InstructionSetInfo.uJudgmentSource = 0;
			InstructionSetInfo.uJumpToInstructionSet = 0;
			InstructionSetInfo.uJumpToInstruction = uJumpToInstruction + 1;
			InstructionSetInfo.uFromInstruction = uJumpToInstruction;

			m_InstructionSetInfoMap.Insert(uJumpToInstruction, pInstructionSetInfo->uJumpToInstructionSet = m_InstructionSetInfos.Add(InstructionSetInfo) + 1);
		}

		m_bIsDirty = true;

		return uResult;
	}

	return 0;
}

void CScriptProgram::Run(CScriptMemery& Memery, IScriptTypeDefinition& TypeDefinition, CScriptInstructionSet::LPERRORINFO pErrorInfos, UINT uErrorInfoCount)
{
	if(m_bIsDirty)
	{
		const UINT* puInstructionSetInfo = m_InstructionSetInfoMap.Get(1);
		UINT uInstructionSetInfo = puInstructionSetInfo ? *puInstructionSetInfo : 0;
		LPINSTRUNCTIONSETINFO pInstructionSetInfo = uInstructionSetInfo ? m_InstructionSetInfos.Get(uInstructionSetInfo - 1) : ALCHEMY_NULL;
		if(!pInstructionSetInfo)
		{
			INSTRUNCTIONSETINFO InstructionSetInfo;

			InstructionSetInfo.pInstructionSet = ALCHEMY_NULL;
			InstructionSetInfo.uJudgmentSource = 0;
			InstructionSetInfo.uJumpToInstructionSet = 0;
			InstructionSetInfo.uJumpToInstruction = 2;
			InstructionSetInfo.uFromInstruction = 1;

			pInstructionSetInfo = m_InstructionSetInfos.Get( uInstructionSetInfo = m_InstructionSetInfos.Add(InstructionSetInfo) );

			m_InstructionSetInfoMap.Insert(1, ++ uInstructionSetInfo);
		}

		ALCHEMY_DEBUG_DELETE_ARRAY(pInstructionSetInfo->pInstructionSet);
		ALCHEMY_DEBUG_NEW( pInstructionSetInfo->pInstructionSet, CScriptInstructionSet(Memery, TypeDefinition) );

		m_InstructionSetInfoIndices.Clear();

		pInstructionSetInfo->uIndex = m_InstructionSetInfoIndices.Add(uInstructionSetInfo - 1);

		LPINSTRUNCTION pInstruction = m_InstructionSet.Get(0);

		if(pInstruction)
			pInstructionSetInfo->pInstructionSet->AppendInstruction(
			pInstruction->uOpcode, 
			pInstruction->uDestination, 
			pInstruction->uDestinationType, 
			pInstruction->uSource1, 
			pInstruction->uSourceType1, 
			pInstruction->uSource2, 
			pInstruction->uSourceType2, 
			pInstruction->pUserData);

		LPINSTRUNCTIONSETINFO pCurrentInstructionSetInfo;
		UINT uLength = m_InstructionSet.GetLength();
		for(UINT i = 1; i < uLength; ++ i)
		{
			puInstructionSetInfo = m_InstructionSetInfoMap.Get(i + 1);
			uInstructionSetInfo = puInstructionSetInfo ? *puInstructionSetInfo : 0;
			pCurrentInstructionSetInfo = uInstructionSetInfo ? m_InstructionSetInfos.Get(uInstructionSetInfo - 1) : ALCHEMY_NULL;
			if(pCurrentInstructionSetInfo)
			{
				pInstructionSetInfo = pCurrentInstructionSetInfo;

				ALCHEMY_DEBUG_DELETE_ARRAY(pInstructionSetInfo->pInstructionSet);
				ALCHEMY_DEBUG_NEW( pInstructionSetInfo->pInstructionSet, CScriptInstructionSet(Memery, TypeDefinition) );
				pInstructionSetInfo->uIndex = m_InstructionSetInfoIndices.Add(uInstructionSetInfo - 1);
			}
			else
			{
				pInstruction = m_InstructionSet.Get(0);

				if(pInstruction)
					pInstructionSetInfo->pInstructionSet->AppendInstruction(
					pInstruction->uOpcode, 
					pInstruction->uDestination, 
					pInstruction->uDestinationType, 
					pInstruction->uSource1, 
					pInstruction->uSourceType1, 
					pInstruction->uSource2, 
					pInstruction->uSourceType2, 
					pInstruction->pUserData);
			}
		}

		m_bIsDirty = false;
	}

	const UINT* puInstructionSetInfoIndex = m_InstructionSetInfoIndices.Get(0);
	LPINSTRUNCTIONSETINFO pInstructionSetInfo = puInstructionSetInfoIndex ? m_InstructionSetInfos.Get(*puInstructionSetInfoIndex) : ALCHEMY_NULL;
	void* pBoolean;
	while(pInstructionSetInfo)
	{
		if(pInstructionSetInfo->pInstructionSet)
			pInstructionSetInfo->pInstructionSet->Run(pErrorInfos, uErrorInfoCount);

		if( pInstructionSetInfo->uJumpToInstructionSet && ( !pInstructionSetInfo->uJudgmentSource || !( pBoolean = Memery.Get(pInstructionSetInfo->uJudgmentSource) ) || *reinterpret_cast<bool*>(pBoolean) ) )
			pInstructionSetInfo = m_InstructionSetInfos.Get(pInstructionSetInfo->uJumpToInstructionSet - 1);
		else
			pInstructionSetInfo = m_InstructionSetInfos.Get(pInstructionSetInfo->uIndex + 1);
	}
}