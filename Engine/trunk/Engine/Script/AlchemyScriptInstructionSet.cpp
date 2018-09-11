#include "AlchemyScriptInstructionSet.h"
#include "AlchemyScriptTypeDefinitionInterface.h"

using namespace alchemy;

CScriptInstructionSet::CScriptInstructionSet(CScriptMemery& Memery, const IScriptTypeDefinition& TypeDefinition) :
m_Memery(Memery), 
m_TypeDefinition(TypeDefinition)
{
}

CScriptInstructionSet::~CScriptInstructionSet(void)
{
	LPINSTRUCTION pPreviousInstruction, pInstruction;
	CVector<INSTRUCTIONINFO>::CIterator Iterator = m_InstructionSet.GetIterator(0);
	while( CVector<INSTRUCTIONINFO>::IsValuable(Iterator) )
	{
		pInstruction = Iterator->pHeadInstruction;
		while(pInstruction)
		{
			pPreviousInstruction = pInstruction;
			pInstruction = pInstruction->pNext;

			ALCHEMY_DEBUG_DELETE_ARRAY(pPreviousInstruction);
		}

		++ Iterator;
	}
}

bool CScriptInstructionSet::AppendInstruction(UINT uOpcode, UINT uDestination, UINT uDestinationType, UINT uSource1, UINT uSourceType1, UINT uSource2, UINT uSourceType2, void* pUserData)
{
	IScriptDataType* pDestinationDataType = m_TypeDefinition.GetDataType(uDestinationType);
	if(pDestinationDataType)
	{
		UINT uDataTypeCount = m_TypeDefinition.GetDataTypeCount();
		if(uSourceType1 < uDataTypeCount && uSourceType2 < uDataTypeCount)
		{
			UINT uSourceTemp1 = uSource1, uSourceTemp2 = uSource2;

			const UINT* pCastOpcode = m_TypeDefinition.GetCastOpcode(uDestination);
			if(pCastOpcode)
			{
				if(uSource1 && uDestinationType != uSourceType1)
					__InsertInstruction(uDestinationType, pCastOpcode[uSourceType1], uSourceTemp1 = m_Memery.Alloc(pDestinationDataType->GetSize(), false), uSource1, 0, pUserData);

				if(uSource2 && uDestinationType != uSourceType2)
					__InsertInstruction(uDestinationType, pCastOpcode[uSourceType2], uSourceTemp2 = m_Memery.Alloc(pDestinationDataType->GetSize(), false), uSource2, 0, pUserData);
			}

			__InsertInstruction(uDestinationType, uOpcode, uDestination, uSourceTemp1, uSourceTemp2, pUserData);

			return true;
		}
	}

	return false;
}

bool CScriptInstructionSet::Run(LPERRORINFO pErrorInfos, UINT uErrorInfoCount)
{
	UINT uLength = m_InstructionSet.GetLength();
	if(!uLength)
		return false;

	UINT uStackIndexCount = m_Memery.GetStackIndexCount();

	CVector<INSTRUCTIONINFO>::CIterator Iterator = m_InstructionSet.GetIterator(0);
	LPINSTRUCTIONINFO pInstructionInfo;
	const IScriptDataType* pDataType;
	while( CVector<INSTRUCTIONINFO>::IsValuable(Iterator) )
	{
		pInstructionInfo = &(*Iterator);

		pDataType = m_TypeDefinition.GetDataType(pInstructionInfo->uType);
		if(pDataType)
			pDataType->Compute(pInstructionInfo->pHeadInstruction, m_Memery, pErrorInfos, uErrorInfoCount);

		++ Iterator;
	}

	m_Memery.FreeStack(uStackIndexCount);

	return true;
}

void CScriptInstructionSet::Destroy()
{
	LPINSTRUCTION pPreviousInstruction, pInstruction;
	CVector<INSTRUCTIONINFO>::CIterator Iterator = m_InstructionSet.GetIterator(0);
	while( CVector<INSTRUCTIONINFO>::IsValuable(Iterator) )
	{
		pInstruction = Iterator->pHeadInstruction;
		while(pInstruction)
		{
			pPreviousInstruction = pInstruction;
			pInstruction = pInstruction->pNext;

			ALCHEMY_DEBUG_DELETE_ARRAY(pPreviousInstruction);
		}

		++ Iterator;
	}

	m_InstructionSet.Clear();
}

void CScriptInstructionSet::__InsertInstruction(UINT uType, UINT uOpcode, UINT uDestination, UINT uSource1, UINT uSource2, void* pUserData)
{
	LPINSTRUCTION pInstruction;
	ALCHEMY_DEBUG_NEW(pInstruction, INSTRUCTION);
	pInstruction->uOpcode = uOpcode;
	pInstruction->uDestination = uDestination;
	pInstruction->uSource1 = uSource1;
	pInstruction->uSource2 = uSource2;
	pInstruction->pUserData = pUserData;
	pInstruction->pNext    = ALCHEMY_NULL;

	UINT uLength = m_InstructionSet.GetLength();
	LPINSTRUCTIONINFO pInstructionInfo = uLength ? m_InstructionSet.Get(uLength - 1) : ALCHEMY_NULL;
	if(pInstructionInfo && uType == pInstructionInfo->uType && pInstructionInfo->pTailInstruction)
	{
		pInstructionInfo->pTailInstruction->pNext = pInstruction;
		pInstructionInfo->pTailInstruction        = pInstruction;
	}
	else
	{
		INSTRUCTIONINFO InstructionInfo;

		InstructionInfo.uType = uType;
		InstructionInfo.pHeadInstruction = pInstruction;
		InstructionInfo.pTailInstruction = pInstruction;

		m_InstructionSet.Add(InstructionInfo);
	}
}