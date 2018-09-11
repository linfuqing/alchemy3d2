#include "AlchemyEffectCode.h"
#include "AlchemyEffectInfoInterface.h"
#include "AlchemyEffectInfoBlendMethodInterface.h"
#include "AlchemyRenderMethod.h"

//#include "AlchemyEffectInfoAddition.h"
//#include "AlchemyEffectInfoModulation.h"

#include "../Core/AlchemyUtils.h"

using namespace alchemy;

const CEffectCode::TEMPORARYDECLARATION CEffectCode::CPassDefiner::sm_COLOR_KEY_TEMPORARYDECLARATION = 
{
	IEffect::FRAGMENT_SHADER,
	CEffectCode::VECTOR4_VARIABLE,
	1
};

CEffectCode::CRegisterApplicant::CRegisterApplicant(CEffectCode& Parent) : 
m_uRegisterCount(0), 
m_Parent(Parent)
{
}

CEffectCode::CRegisterApplicant::~CRegisterApplicant()
{
}
			
UINT CEffectCode::CRegisterApplicant::Malloc(VARIABLETYPE Type, UINT uDeclaration, UINT uCount)
{
	UINT uRegister = m_uRegisterCount, uVariableSize = m_Parent.GetRegisterCount(Type, uDeclaration, false), uByteAlignment = 1 << m_Parent.m_Compiler.GetCompilerInfo().uByteAlignmentShift, uByteAlignmentMask = uByteAlignment - 1;
	if(uCount == 1 && uVariableSize <= uByteAlignment)
	{
		CPool<REGISTERSEGMENT>::CIterator Iterator = m_RegisterSegmentPool.GetIterator(0);
		LPREGISTERSEGMENT pRegisterSegment, pCurrentRegisterSegment = ALCHEMY_NULL;
		UINT uIndex;
		while( CPool<REGISTERSEGMENT>::IsValuable(Iterator) )
		{
			pRegisterSegment = &(*Iterator);
			if( pRegisterSegment->uCount == uVariableSize && pRegisterSegment->uBegin < uRegister && ( Type != VECTOR3_VARIABLE || !(pRegisterSegment->uBegin & uByteAlignmentMask) ) )
			{
				pCurrentRegisterSegment = pRegisterSegment;
				uRegister = pRegisterSegment->uBegin;
				uIndex = Iterator.GetIndex();
			}

			++ Iterator;
		}

		if(pCurrentRegisterSegment)
		{
			pCurrentRegisterSegment->uBegin += uVariableSize;
			pCurrentRegisterSegment->uCount -= uVariableSize;

			if(pCurrentRegisterSegment->uCount == 0)
				m_RegisterSegmentPool.Remove(uIndex, 1);
		}
		else
		{
			Iterator = m_RegisterSegmentPool.GetIterator(0);
			while( CPool<REGISTERSEGMENT>::IsValuable(Iterator) )
			{
				pRegisterSegment = &(*Iterator);
				if(pRegisterSegment->uCount > uVariableSize && pRegisterSegment->uBegin < uRegister)
				{
					pCurrentRegisterSegment = pRegisterSegment;
					uRegister = pRegisterSegment->uBegin;
					uIndex = Iterator.GetIndex();
				}

				++ Iterator;
			}

			if(pCurrentRegisterSegment)
			{
				pCurrentRegisterSegment->uBegin += uVariableSize;
				pCurrentRegisterSegment->uCount -= uVariableSize;

				if(pCurrentRegisterSegment->uCount == 0)
					m_RegisterSegmentPool.Remove(uIndex, 1);
			}
			else
			{
				UINT uRegisterCount = m_Parent.GetRegisterCount(Type, uDeclaration);

				if(uVariableSize < uRegisterCount)
				{
					REGISTERSEGMENT RegisterSegment;
					RegisterSegment.uBegin = m_uRegisterCount + uVariableSize;
					RegisterSegment.uCount = uRegisterCount   - uVariableSize;

					m_RegisterSegmentPool.Add(RegisterSegment);
				}

				m_uRegisterCount += uRegisterCount;
			}
		}
	}
	else
	{
		UINT uRegisterCount = m_Parent.GetRegisterCount(Type, uDeclaration);

		if(uVariableSize < uRegisterCount)
		{
			REGISTERSEGMENT RegisterSegment;
			RegisterSegment.uCount = uRegisterCount - uVariableSize;
			UINT uBegin = m_uRegisterCount + uVariableSize;
			for(UINT i = 0; i < uCount; ++ i)
			{
				RegisterSegment.uBegin = uBegin;
				m_RegisterSegmentPool.Add(RegisterSegment);

				uBegin += uRegisterCount;
			}
		}

		m_uRegisterCount += uRegisterCount * uCount;
	}
	
	return uRegister;
}
			
void CEffectCode::CRegisterApplicant::Free(UINT uRegister, VARIABLETYPE Type, UINT uDeclaration, UINT uCount)
{
	UINT uVariableSize = m_Parent.GetRegisterCount(Type, uDeclaration, false),
		 uRegisterCount = m_Parent.GetRegisterCount(Type, uDeclaration),
		 uRegisterBegin = uRegister, 
		 uByteAlignment = 1 << m_Parent.m_Compiler.GetCompilerInfo().uByteAlignmentShift, 
		 uByteAlignmentMask = uByteAlignment - 1,
		 uRegisterEnd,
		 uCurrentRegisterCount;

	CPool<REGISTERSEGMENT>::CIterator RegisterSegmentIterator = m_RegisterSegmentPool.GetIterator(0);
	LPREGISTERSEGMENT pRegisterSegment;
	REGISTERSEGMENT RegisterSegment;
	for(UINT i = 0; i < uCount; ++ i)
	{
		uRegisterEnd = uRegisterBegin + uVariableSize;

		RegisterSegmentIterator = m_RegisterSegmentPool.GetIterator(0);
		while( CPool<REGISTERSEGMENT>::IsValuable(RegisterSegmentIterator) )
		{
			pRegisterSegment = &(*RegisterSegmentIterator);
			uCurrentRegisterCount = pRegisterSegment->uBegin + pRegisterSegment->uCount;

			if( (pRegisterSegment->uCount + uVariableSize) <= uByteAlignment )
			{
				if(pRegisterSegment->uBegin == uRegisterEnd && (uRegisterEnd & uByteAlignmentMask) != 0)
				{
					pRegisterSegment->uBegin  = uRegisterBegin;
					pRegisterSegment->uCount += uVariableSize;

					break;
				}
				else if(uCurrentRegisterCount == uRegisterBegin && (uCurrentRegisterCount & uByteAlignmentMask) != 0)
				{
					pRegisterSegment->uCount += uVariableSize;

					break;
				}
			}

			++ RegisterSegmentIterator;
		}

		if( !CPool<REGISTERSEGMENT>::IsValuable(RegisterSegmentIterator) )
		{
			RegisterSegment.uBegin = uRegisterBegin;
			RegisterSegment.uCount = uVariableSize;

			m_RegisterSegmentPool.Add(RegisterSegment);
		}

		uRegisterBegin += uRegisterCount;
	}
}

void CEffectCode::CRegisterApplicant::Clear()
{
	m_uRegisterCount = 0;
	m_RegisterSegmentPool.Clear();
}

CEffectCode::CGlobalInputHandler::CGlobalInputHandler() :
m_ppGlobalInput(ALCHEMY_NULL),
m_puGlobalInputCount(ALCHEMY_NULL),
m_ppParameterList(ALCHEMY_NULL),
m_pInstructionSet(ALCHEMY_NULL),
m_ppInputValue(ALCHEMY_NULL),
m_puInputValueCount(ALCHEMY_NULL),
m_uInputCount(0),
m_pParameterBuffer(ALCHEMY_NULL),
m_uParameterBufferSize(0)
{

}

CEffectCode::CGlobalInputHandler::~CGlobalInputHandler()
{
	if(m_ppGlobalInput)
	{
		for(UINT i = 0; i < m_uInputCount; ++ i)
			ALCHEMY_DEBUG_DELETE_ARRAY(m_ppGlobalInput[i]);

		ALCHEMY_DEBUG_DELETE_ARRAY(m_ppGlobalInput);
	}

	if(m_ppParameterList)
	{
		CRenderMethod::LPPARAMETERLIST pParameterList;
		for(UINT i = 0; i < m_uInputCount; ++ i)
		{
			pParameterList = m_ppParameterList[i];
			if(pParameterList)
				CRenderMethod::DestroyParameterList(pParameterList, 1);
		}

		ALCHEMY_DEBUG_DELETE_ARRAY(m_ppParameterList);
	}

	if(m_ppInputValue)
	{
		UINT i;
		for(i = 0; i < m_uInputCount; ++ i)
			ALCHEMY_DEBUG_DELETE_ARRAY(m_ppInputValue[i]);

		ALCHEMY_DEBUG_DELETE_ARRAY(m_ppInputValue);
	}

	ALCHEMY_DEBUG_DELETE_ARRAY(m_puGlobalInputCount);

	ALCHEMY_DEBUG_DELETE_ARRAY(m_pParameterBuffer);

	ALCHEMY_DEBUG_DELETE_ARRAY(m_pInstructionSet);

	ALCHEMY_DEBUG_DELETE_ARRAY(m_puInputValueCount);
}

bool CEffectCode::CGlobalInputHandler::SetParameters(IEffect& Effect, UINT uIndex, bool bIsActivate)
{
	if(uIndex < m_uInputCount)
	{
		if(m_pInstructionSet && m_pParameterBuffer)
		{
			UINT i;
			if(m_ppInputValue && m_puInputValueCount)
			{
				LPINPUTVALUE pInputValue = m_ppInputValue[uIndex];
				UINT uInputValueCount = m_puInputValueCount[uIndex];

				for(i = 0; i < uInputValueCount; ++ i)
				{
					ALCHEMY_MEMORY_COPY(&static_cast<PUINT8>(m_pParameterBuffer)[pInputValue->uIndex], pInputValue->pValue, pInputValue->uSize);

					++ pInputValue;
				}
			}

			const CSmartPointer<CBuffer>* pAddressBuffer;
			const UINT* puDestinationAddress, *puSourceAddress1, *puSourceAddress2;
			PFLOAT pfDestiantion;
			const FLOAT* pfSource1, * pfSource2;
			UINT uDestinationAddressIndex, uSourceAddressIndex1, uSourceAddressIndex2, uDestinationAddressCount, uSourceAddressCount1, uSourceAddressCount2;
			const GLOBALINSTRUCTION* pGlobalInstruction;
			CList<GLOBALINSTRUCTION>::CConstantIterator Iterator = m_pInstructionSet[uIndex].GetBeginIterator();
			while( CList<GLOBALINSTRUCTION>::IsValuable(Iterator) )
			{
				pGlobalInstruction = &(*Iterator);

				if(pGlobalInstruction->uDestinationSize)
				{
					if(pGlobalInstruction->uDestinationIndexAddress < pGlobalInstruction->uDestinationIndexSize)
					{
						uDestinationAddressIndex = static_cast<UINT>( *(reinterpret_cast<const FLOAT*>(m_pParameterBuffer + pGlobalInstruction->uDestinationIndex) + pGlobalInstruction->uDestinationIndexAddress) );
						if(uDestinationAddressIndex < pGlobalInstruction->uDestinationSize)
							uDestinationAddressIndex = pGlobalInstruction->uDestination + uDestinationAddressIndex;
						else
						{
							++ Iterator;

							continue;
						}
					}
					else
						uDestinationAddressIndex = pGlobalInstruction->uDestination;

					pfDestiantion = reinterpret_cast<PFLOAT>(reinterpret_cast<PUINT8>(m_pParameterBuffer) + uDestinationAddressIndex);

					pAddressBuffer = pGlobalInstruction->uDestinationAddress ? m_AddressBuffers.Get(pGlobalInstruction->uDestinationAddress - 1) : ALCHEMY_NULL;
					if(pAddressBuffer)
					{
						puDestinationAddress     = static_cast<const UINT*>( (*pAddressBuffer)->GetBufferPointer() );
						uDestinationAddressCount = (*pAddressBuffer)->GetBufferSize() / sizeof(UINT);
					}
					else
					{
						puDestinationAddress     = ALCHEMY_NULL;
						uDestinationAddressCount = pGlobalInstruction->uDestinationSize / sizeof(FLOAT);
					}
				}
				else
					pfDestiantion = ALCHEMY_NULL;

				if(pGlobalInstruction->uSourceSize1)
				{
					if(pGlobalInstruction->uSourceIndexAddress1 < pGlobalInstruction->uSourceIndexSize1)
					{
						uSourceAddressIndex1 = static_cast<UINT>( *(reinterpret_cast<const FLOAT*>(m_pParameterBuffer + pGlobalInstruction->uSourceIndex1) + pGlobalInstruction->uSourceIndexAddress1) );
						if(uSourceAddressIndex1 < pGlobalInstruction->uSourceSize1)
							uSourceAddressIndex1 = pGlobalInstruction->uSource1 + uSourceAddressIndex1;
						else
						{
							++ Iterator;

							continue;
						}
					}
					else
						uSourceAddressIndex1 = pGlobalInstruction->uSource1;

					pfSource1 = reinterpret_cast<PFLOAT>(reinterpret_cast<PUINT8>(m_pParameterBuffer) + uSourceAddressIndex1);

					pAddressBuffer = pGlobalInstruction->uSourceAddress1     ? m_AddressBuffers.Get(pGlobalInstruction->uSourceAddress1     - 1) : ALCHEMY_NULL;
					if(pAddressBuffer)
					{
						puSourceAddress1     = static_cast<const UINT*>( (*pAddressBuffer)->GetBufferPointer() );
						uSourceAddressCount1 = (*pAddressBuffer)->GetBufferSize() / sizeof(UINT);
					}
					else
					{
						puSourceAddress1     = ALCHEMY_NULL;
						uSourceAddressCount1 = pGlobalInstruction->uSourceSize1 / sizeof(FLOAT);
					}
				}
				else
					pfSource1 = ALCHEMY_NULL;

				if(pGlobalInstruction->uSourceSize2)
				{
					if(pGlobalInstruction->uSourceIndexAddress2 < pGlobalInstruction->uSourceIndexSize2)
					{
						uSourceAddressIndex2 = static_cast<UINT>( *(reinterpret_cast<const FLOAT*>(m_pParameterBuffer + pGlobalInstruction->uSourceIndex2) + pGlobalInstruction->uSourceIndexAddress2) );
						if(uSourceAddressIndex2 < pGlobalInstruction->uSourceSize2)
							uSourceAddressIndex2 = pGlobalInstruction->uSource2 + uSourceAddressIndex1;
						else
						{
							++ Iterator;

							continue;
						}
					}
					else
						uSourceAddressIndex2 = pGlobalInstruction->uSource2;

					pfSource2 = reinterpret_cast<PFLOAT>(reinterpret_cast<PUINT8>(m_pParameterBuffer) + uSourceAddressIndex2);

					pAddressBuffer = pGlobalInstruction->uSourceAddress2     ? m_AddressBuffers.Get(pGlobalInstruction->uSourceAddress2     - 1) : ALCHEMY_NULL;
					if(pAddressBuffer)
					{
						puSourceAddress2    = static_cast<const UINT*>( (*pAddressBuffer)->GetBufferPointer() );
						uSourceAddressCount2 = (*pAddressBuffer)->GetBufferSize() / sizeof(UINT);
					}
					else
					{
						puSourceAddress2     = ALCHEMY_NULL;
						uSourceAddressCount2 = pGlobalInstruction->uSourceSize2 / sizeof(FLOAT);;
					}
				}
				else
					pfSource2 = ALCHEMY_NULL;

				switch(pGlobalInstruction->Opcode)
				{
				case ICompiler::MOV:
					if(pfDestiantion && pfSource1)
					{
						if(puDestinationAddress && puSourceAddress1)
						{
							for(i = 0; i < uDestinationAddressCount; ++ i)
								pfDestiantion[puDestinationAddress[i]] = pfSource1[puSourceAddress1[i < uSourceAddressCount1 ? i : (uSourceAddressCount1 - 1)]];
						}
						else if(puDestinationAddress)
						{
							for(i = 0; i < uDestinationAddressCount; ++ i)
								pfDestiantion[puDestinationAddress[i]] = pfSource1[i < uSourceAddressCount1 ? i : (uSourceAddressCount1 - 1)];
						}
						else if(puSourceAddress1)
						{
							for(i = 0; i < uDestinationAddressCount; ++ i)
								pfDestiantion[i] = pfSource1[puSourceAddress1[i < uSourceAddressCount1 ? i : (uSourceAddressCount1 - 1)]];
						}
						else
						{
							for(i = 0; i < uDestinationAddressCount; ++ i)
								pfDestiantion[i] = pfSource1[i < uSourceAddressCount1 ? i : (uSourceAddressCount1 - 1)];
						}
					}
					break;
				case ICompiler::ADD:
					if(pfDestiantion && pfSource1 && pfSource2)
					{
						if(puDestinationAddress && puSourceAddress1 && puSourceAddress2)
						{
							for(i = 0; i < uDestinationAddressCount; ++ i)
							{
								pfDestiantion[puDestinationAddress[i]] = 
									pfSource1[puSourceAddress1[i < uSourceAddressCount1 ? i : (uSourceAddressCount1 - 1)]] + 
									pfSource2[puSourceAddress2[i < uSourceAddressCount2 ? i : (uSourceAddressCount2 - 1)]];
							}
						}
						else if(puDestinationAddress && puSourceAddress1)
						{
							for(i = 0; i < uDestinationAddressCount; ++ i)
							{
								pfDestiantion[puDestinationAddress[i]] = 
									pfSource1[puSourceAddress1[i < uSourceAddressCount1 ? i : (uSourceAddressCount1 - 1)]] + 
									pfSource2[i < uSourceAddressCount2 ? i : (uSourceAddressCount2 - 1)];
							}
						}
						else if(puDestinationAddress && puSourceAddress2)
						{
							for(i = 0; i < uDestinationAddressCount; ++ i)
							{
								pfDestiantion[puDestinationAddress[i]] = 
									pfSource1[i < uSourceAddressCount1 ? i : (uSourceAddressCount1 - 1)] + 
									pfSource2[puSourceAddress2[i < uSourceAddressCount2 ? i : (uSourceAddressCount2 - 1)]];
							}
						}
						else if(puSourceAddress1 && puSourceAddress2)
						{
							for(i = 0; i < uDestinationAddressCount; ++ i)
							{
								pfDestiantion[i] = 
									pfSource1[puSourceAddress1[i < uSourceAddressCount1 ? i : (uSourceAddressCount1 - 1)]] + 
									pfSource2[puSourceAddress2[i < uSourceAddressCount2 ? i : (uSourceAddressCount2 - 1)]];
							}
						}
						else if(puDestinationAddress)
						{
							for(i = 0; i < uDestinationAddressCount; ++ i)
							{
								pfDestiantion[puDestinationAddress[i]] = 
									pfSource1[i < uSourceAddressCount1 ? i : (uSourceAddressCount1 - 1)] + 
									pfSource2[i < uSourceAddressCount2 ? i : (uSourceAddressCount2 - 1)];
							}
						}
						else if(puSourceAddress1)
						{
							for(i = 0; i < uDestinationAddressCount; ++ i)
							{
								pfDestiantion[i] = 
									pfSource1[puSourceAddress1[i < uSourceAddressCount1 ? i : (uSourceAddressCount1 - 1)]] + 
									pfSource2[i < uSourceAddressCount2 ? i : (uSourceAddressCount2 - 1)];
							}
						}
						else if(puSourceAddress2)
						{
							for(i = 0; i < uDestinationAddressCount; ++ i)
							{
								pfDestiantion[i] = 
									pfSource1[i < uSourceAddressCount1 ? i : (uSourceAddressCount1 - 1)] + 
									pfSource2[puSourceAddress2[i < uSourceAddressCount2 ? i : (uSourceAddressCount2 - 1)]];
							}
						}
						else
						{
							for(i = 0; i < uDestinationAddressCount; ++ i)
							{
								pfDestiantion[i] = 
									pfSource1[i < uSourceAddressCount1 ? i : (uSourceAddressCount1 - 1)] + 
									pfSource2[i < uSourceAddressCount2 ? i : (uSourceAddressCount2 - 1)];
							}
						}
					}
					break;
				case ICompiler::SUB:
					if(pfDestiantion && pfSource1 && pfSource2)
					{
						if(puDestinationAddress && puSourceAddress1 && puSourceAddress2)
						{
							for(i = 0; i < uDestinationAddressCount; ++ i)
							{
								pfDestiantion[puDestinationAddress[i]] = 
									pfSource1[puSourceAddress1[i < uSourceAddressCount1 ? i : (uSourceAddressCount1 - 1)]] - 
									pfSource2[puSourceAddress2[i < uSourceAddressCount2 ? i : (uSourceAddressCount2 - 1)]];
							}
						}
						else if(puDestinationAddress && puSourceAddress1)
						{
							for(i = 0; i < uDestinationAddressCount; ++ i)
							{
								pfDestiantion[puDestinationAddress[i]] = 
									pfSource1[puSourceAddress1[i < uSourceAddressCount1 ? i : (uSourceAddressCount1 - 1)]] - 
									pfSource2[i < uSourceAddressCount2 ? i : (uSourceAddressCount2 - 1)];
							}
						}
						else if(puDestinationAddress && puSourceAddress2)
						{
							for(i = 0; i < uDestinationAddressCount; ++ i)
							{
								pfDestiantion[puDestinationAddress[i]] = 
									pfSource1[i < uSourceAddressCount1 ? i : (uSourceAddressCount1 - 1)] - 
									pfSource2[puSourceAddress2[i < uSourceAddressCount2 ? i : (uSourceAddressCount2 - 1)]];
							}
						}
						else if(puSourceAddress1 && puSourceAddress2)
						{
							for(i = 0; i < uDestinationAddressCount; ++ i)
							{
								pfDestiantion[i] = 
									pfSource1[puSourceAddress1[i < uSourceAddressCount1 ? i : (uSourceAddressCount1 - 1)]] - 
									pfSource2[puSourceAddress2[i < uSourceAddressCount2 ? i : (uSourceAddressCount2 - 1)]];
							}
						}
						else if(puDestinationAddress)
						{
							for(i = 0; i < uDestinationAddressCount; ++ i)
							{
								pfDestiantion[puDestinationAddress[i]] = 
									pfSource1[i < uSourceAddressCount1 ? i : (uSourceAddressCount1 - 1)] - 
									pfSource2[i < uSourceAddressCount2 ? i : (uSourceAddressCount2 - 1)];
							}
						}
						else if(puSourceAddress1)
						{
							for(i = 0; i < uDestinationAddressCount; ++ i)
							{
								pfDestiantion[i] = 
									pfSource1[puSourceAddress1[i < uSourceAddressCount1 ? i : (uSourceAddressCount1 - 1)]] - 
									pfSource2[i < uSourceAddressCount2 ? i : (uSourceAddressCount2 - 1)];
							}
						}
						else if(puSourceAddress2)
						{
							for(i = 0; i < uDestinationAddressCount; ++ i)
							{
								pfDestiantion[i] = 
									pfSource1[i < uSourceAddressCount1 ? i : (uSourceAddressCount1 - 1)] - 
									pfSource2[puSourceAddress2[i < uSourceAddressCount2 ? i : (uSourceAddressCount2 - 1)]];
							}
						}
						else
						{
							for(i = 0; i < uDestinationAddressCount; ++ i)
							{
								pfDestiantion[i] = 
									pfSource1[i < uSourceAddressCount1 ? i : (uSourceAddressCount1 - 1)] - 
									pfSource2[i < uSourceAddressCount2 ? i : (uSourceAddressCount2 - 1)];
							}
						}
					}
					break;
				case ICompiler::MUL:
					break;
				case ICompiler::DIV:
					break;
				case ICompiler::RCP:
					break;
				case ICompiler::MIN:
					break;
				case ICompiler::MAX:
					break;
				case ICompiler::FRC:
					break;
				case ICompiler::SQT:
					break;
				case ICompiler::RSQ:
					break;
				case ICompiler::POW:
					break;
				case ICompiler::LOG:
					break;
				case ICompiler::EXP:
					break;
				case ICompiler::NRM:
					break;
				case ICompiler::SIN:
					break;
				case ICompiler::COS:
					break;
				case ICompiler::CRS:
					break;
				case ICompiler::DP3:
					break;
				case ICompiler::DP4:
					break;
				case ICompiler::ABS:
					break;
				case ICompiler::NEG:
					break;
				case ICompiler::SAT:
					break;
				case ICompiler::M33:
					break;
				case ICompiler::M44:
					break;
				case ICompiler::M34:
					break;
				case ICompiler::KIL:
					break;
				case ICompiler::TEX:
					break;
				case ICompiler::SGE:
					break;
				case ICompiler::SLT:
					break;
				case ICompiler::SGN:
					break;
                default:
                    break;
				}

				++ Iterator;
			}
		}

		LPGLOBALINPUT pGlobalInput = m_ppGlobalInput[uIndex];
		if(pGlobalInput)
		{
			VARIABLETYPE VariableType;
			UINT uRowOffset, uOffset;
			bool bIsPointer;

			UINT uGlobalInpuTCount = m_puGlobalInputCount[uIndex];

			for(UINT i = 0; i < uGlobalInpuTCount; ++ i)
			{
				if(bIsActivate || !pGlobalInput->bIsStatic)
				{
					CEffectCode::GetGlobalInputInfo(pGlobalInput->uCode, &VariableType, &uRowOffset, &uOffset, &bIsPointer);

					switch(VariableType)
					{
					case VECTOR1_VARIABLE:
						Effect.SetShaderParameters(pGlobalInput->ShaderType, pGlobalInput->uIndex, reinterpret_cast<const FLOAT*>(pGlobalInput->pValue), 1 * pGlobalInput->uCount);
						break;
					case VECTOR2_VARIABLE:
						Effect.SetShaderParameters(pGlobalInput->ShaderType, pGlobalInput->uIndex, reinterpret_cast<const FLOAT*>(pGlobalInput->pValue), 2 * pGlobalInput->uCount);
						break;
					case VECTOR3_VARIABLE:
						Effect.SetShaderParameters(pGlobalInput->ShaderType, pGlobalInput->uIndex, reinterpret_cast<const FLOAT*>(pGlobalInput->pValue), 3 * pGlobalInput->uCount);
						break;
					case VECTOR4_VARIABLE:
						Effect.SetShaderParameters(pGlobalInput->ShaderType, pGlobalInput->uIndex, reinterpret_cast<const FLOAT*>(pGlobalInput->pValue), 4 * pGlobalInput->uCount);
						break;
					case MATRIX2X2_VARIABLE:
						if(bIsPointer)
							Effect.SetShaderMatrixPointerArray(pGlobalInput->ShaderType, pGlobalInput->uIndex, reinterpret_cast<const PFLOAT*>(pGlobalInput->pValue), 2, 2, uRowOffset, pGlobalInput->uCount);
						else
							Effect.SetShaderMatrixArray(pGlobalInput->ShaderType, pGlobalInput->uIndex, reinterpret_cast<const FLOAT*>(pGlobalInput->pValue), 2, 2, uRowOffset, uOffset, pGlobalInput->uCount);
						break;
					case MATRIX3X3_VARIABLE:
						if(bIsPointer)
							Effect.SetShaderMatrixPointerArray(pGlobalInput->ShaderType, pGlobalInput->uIndex, reinterpret_cast<const PFLOAT*>(pGlobalInput->pValue), 3, 3, uRowOffset, pGlobalInput->uCount);
						else
							Effect.SetShaderMatrixArray(pGlobalInput->ShaderType, pGlobalInput->uIndex, reinterpret_cast<const FLOAT*>(pGlobalInput->pValue), 3, 3, uRowOffset, uOffset, pGlobalInput->uCount);
						break;
					case MATRIX4X3_VARIABLE:
						if(bIsPointer)
							Effect.SetShaderMatrixPointerArray(pGlobalInput->ShaderType, pGlobalInput->uIndex, reinterpret_cast<const PFLOAT*>(pGlobalInput->pValue), 3, 4, uRowOffset, pGlobalInput->uCount);
						else
							Effect.SetShaderMatrixArray(pGlobalInput->ShaderType, pGlobalInput->uIndex, reinterpret_cast<const FLOAT*>(pGlobalInput->pValue), 3, 4, uRowOffset, uOffset, pGlobalInput->uCount);
						break;
					case MATRIX4X4_VARIABLE:
						if(bIsPointer)
							Effect.SetShaderMatrixPointerArray(pGlobalInput->ShaderType, pGlobalInput->uIndex, reinterpret_cast<const PFLOAT*>(pGlobalInput->pValue), 4, 4, uRowOffset, pGlobalInput->uCount);
						else
						//Effect.SetMatrix(pGlobalInput->ShaderType, pGlobalInput->uIndex, reinterpret_cast<const MATRIX4X4&>(* (MATRIX4X4*)pGlobalInput->pValue) );
							Effect.SetShaderMatrixArray(pGlobalInput->ShaderType, pGlobalInput->uIndex, reinterpret_cast<const FLOAT*>(pGlobalInput->pValue), 4, 4, uRowOffset, uOffset, pGlobalInput->uCount);
						break;
                    default:
                        break;
					}
				}

				++ pGlobalInput;
			}
		}

		return true;
	}

	ALCHEMY_DEBUG_WARNING("CEffectCode::CConstomConstantHandler::SetParameters: Pass out of range.");

	return false;
}

bool CEffectCode::CGlobalInputHandler::SetParameters(const void* pValue, UINT uBufferIndex, UINT uSize)
{
	if(m_pParameterBuffer && (uBufferIndex + uSize) < m_uParameterBufferSize)
	{
		ALCHEMY_MEMORY_COPY(reinterpret_cast<PUINT8>(m_pParameterBuffer) + uBufferIndex, pValue, uSize); 

		return true;
	}

	return false;
}

bool CEffectCode::CGlobalInputHandler::SetParameters(const FLOAT* pfValues, UINT uBufferIndex, UINT uRow, UINT uColumn, UINT uRowOffset, UINT uOffset, UINT uCount)
{
	UINT uSize = uRow * uColumn * uCount * sizeof(FLOAT);
	if(m_pParameterBuffer && (uBufferIndex + uSize) < m_uParameterBufferSize)
	{
		UINT uCurrentOffset = ALCHEMY_MININUM(uRowOffset * uColumn, uOffset);
		uCurrentOffset = uOffset - uCurrentOffset;
		PFLOAT pfParameterBuffer = reinterpret_cast<PFLOAT>(reinterpret_cast<PUINT8>(m_pParameterBuffer) + uBufferIndex); 
		UINT i, j, k;
		for(i = 0; i < uCount; ++ i)
		{
			for(j = 0; j < uRow; ++ j)
			{
				for(k = 0; k < uColumn; ++ k)
					*(pfParameterBuffer ++) = pfValues[k];

				pfValues += uRowOffset;
			}

			pfValues += uCurrentOffset;
		}

		return true;
	}

	return false;
}

bool CEffectCode::CGlobalInputHandler::SetParameters(const PFLOAT* ppfValues, UINT uBufferIndex, UINT uRow, UINT uColumn, UINT uRowOffset, UINT uCount)
{
	UINT uSize = uRow * uColumn * uCount * sizeof(FLOAT);
	if(m_pParameterBuffer && (uBufferIndex + uSize) < m_uParameterBufferSize)
	{
		PFLOAT pfParameterBuffer = reinterpret_cast<PFLOAT>(reinterpret_cast<PUINT8>(m_pParameterBuffer) + uBufferIndex); 
		const FLOAT* pfValues;
		UINT i, j, k;
		for(i = 0; i < uCount; ++ i)
		{
			pfValues = ppfValues[i];
			for(j = 0; j < uRow; ++ j)
			{
				for(k = 0; k < uColumn; ++ k)
					*(pfParameterBuffer ++) = pfValues[k];

				pfValues += uRowOffset;
			}
		}

		return true;
	}

	return false;
}

void CEffectCode::CGlobalInputHandler::Destroy()
{
	if(m_ppGlobalInput)
	{
		for(UINT i = 0; i < m_uInputCount; ++ i)
			ALCHEMY_DEBUG_DELETE_ARRAY(m_ppGlobalInput[i]);

		ALCHEMY_DEBUG_DELETE_ARRAY(m_ppGlobalInput);
	}

	if(m_ppParameterList)
	{
		CRenderMethod::LPPARAMETERLIST pParameterList;
		for(UINT i = 0; i < m_uInputCount; ++ i)
		{
			pParameterList = m_ppParameterList[i];
			if(pParameterList)
				CRenderMethod::DestroyParameterList(pParameterList, 1);
		}

		ALCHEMY_DEBUG_DELETE_ARRAY(m_ppParameterList);
	}

	if(m_ppInputValue)
	{
		UINT i;
		for(i = 0; i < m_uInputCount; ++ i)
			ALCHEMY_DEBUG_DELETE_ARRAY(m_ppInputValue[i]);

		ALCHEMY_DEBUG_DELETE_ARRAY(m_ppInputValue);
	}

	ALCHEMY_DEBUG_DELETE_ARRAY(m_puGlobalInputCount);

	ALCHEMY_DEBUG_DELETE_ARRAY(m_pParameterBuffer);

	ALCHEMY_DEBUG_DELETE_ARRAY(m_pInstructionSet);

	ALCHEMY_DEBUG_DELETE_ARRAY(m_puInputValueCount)

	m_uInputCount = 0;
	m_uParameterBufferSize = 0;

	m_AddressBuffers.Clear();
}

CEffectCode::CDefiner::CDefiner(CEffectCode& Parent) :
m_Parent(Parent),
m_uHandle(0)
{
}

CEffectCode::CDefiner::~CDefiner()
{
}

CEffectCode::CStructDefiner::CStructDefiner(CEffectCode& Parent) :
CDefiner(Parent),
m_pCurrentDeclaration(ALCHEMY_NULL)
{
}

CEffectCode::CStructDefiner::~CStructDefiner()
{
}

void CEffectCode::CStructDefiner::AddType(VARIABLETYPE Type, UINT uStruct)
{
	STRUCTDECLARATION StructDeclaration;

	StructDeclaration.Type    = Type;
	StructDeclaration.uStruct = uStruct;
	StructDeclaration.uNext   = 0;

	UINT uIndex = m_Parent.m_StructDeclarationPool.Add(StructDeclaration);

	if(m_pCurrentDeclaration)
		m_pCurrentDeclaration->uNext = uIndex + 1;
	else
		m_uHandle = m_Parent.m_StructPool.Add(uIndex + 1) + 1;

	m_pCurrentDeclaration = m_Parent.m_StructDeclarationPool.Get(uIndex);
}

UINT CEffectCode::CStructDefiner::EndDefine()
{
	m_pCurrentDeclaration = ALCHEMY_NULL;

	UINT uResult = m_uHandle;
	m_uHandle    = 0;

	return uResult;
}

CEffectCode::CConstantBufferDefiner::CConstantBufferDefiner(CEffectCode& Parent) :
CDefiner(Parent),
m_StructDefiner(Parent), 
m_pConstantBufferDeclaration(ALCHEMY_NULL)
{
}

CEffectCode::CConstantBufferDefiner::~CConstantBufferDefiner()
{
}
			
bool CEffectCode::CConstantBufferDefiner::BeginBuffer(IEffect::EFFECTTYPE Type)
{
	const ICompiler::COMPILERINFO& CompilerInfo = m_Parent.m_Compiler.GetCompilerInfo();
	if( (Type == IEffect::VERTEX_SHADER && !CompilerInfo.uMaxVertexConstantBufferSize) ||
		(Type == IEffect::FRAGMENT_SHADER && !CompilerInfo.uMaxFragmentConstantBufferSize) )
	{
		ALCHEMY_DEBUG_WARNING("CEffectCode::CConstantBufferDefiner::BeginBuffer: shader type has not been supported.");
		return false;
	}
	
	CONSTANTBUFFERDECLARATION ConstantBufferDeclaration;
	ConstantBufferDeclaration.Type           = Type;
	ConstantBufferDeclaration.uDeclaration   = 0;
	ConstantBufferDeclaration.uRegisterCount = 0;
	UINT uIndex = m_Parent.m_ConstantBufferDeclarationPool.Add(ConstantBufferDeclaration);
	
	m_pConstantBufferDeclaration = m_Parent.m_ConstantBufferDeclarationPool.Get(uIndex);
	
	m_uHandle = uIndex + 1;
	
	return true;
}

UINT CEffectCode::CConstantBufferDefiner::AddConstant(GLOBALVARIABLETYPE GlobalType, UINT uDetail, UINT uCount, UINT uIndex)
{
	if(!m_pConstantBufferDeclaration)
	{
		ALCHEMY_DEBUG_WARNING("CEffectCode::CConstantBufferDefiner::AddConstant: Buffer has not been began.");
		
		return 0;
	}
		
	const ICompiler::COMPILERINFO& CompilerInfo = m_Parent.m_Compiler.GetCompilerInfo();
	if( (m_pConstantBufferDeclaration->Type == IEffect::VERTEX_SHADER && m_pConstantBufferDeclaration->uRegisterCount >= CompilerInfo.uMaxVertexConstantBufferSize) ||
		(m_pConstantBufferDeclaration->Type == IEffect::FRAGMENT_SHADER && m_pConstantBufferDeclaration->uRegisterCount >= CompilerInfo.uMaxFragmentConstantBufferSize) )
	{
		ALCHEMY_DEBUG_WARNING("CEffectCode::CConstantBufferDefiner::AddConstant: Buffer out of range.");
		
		return 0;
	}
		
	VARIABLETYPE VariableType = GetGlobalVariableType(GlobalType, uDetail);
	if(VariableType == UNKNOWN_VARIABLE)
	{
		ALCHEMY_DEBUG_WARNING("CEffectCode::CConstantBufferDefiner::AddConstant: Unknown global type.");
		
		return 0;
	}
		
	GLOBALVARIABLE GlobalVariable;
	GlobalVariable.ShaderType = m_pConstantBufferDeclaration->Type;
	GlobalVariable.GlobalType = GlobalType;
	GlobalVariable.uDetail    = uDetail;
	GlobalVariable.uCount     = uCount;
	GlobalVariable.uIndex     = uIndex;
	GlobalVariable.pValue     = ALCHEMY_NULL;

	PUINT puHandle = m_Parent.m_GlobalVariableHandleMap.Get(GlobalVariable);
	UINT uHandle = puHandle ? *puHandle : 0;
	if(uHandle)
	{
		puHandle = m_Parent.m_GlobalConstantBufferMap.Get(uHandle);
		if(puHandle && *puHandle == m_uHandle)
			return uHandle;
			
		return 0;
	}

	VARIABLE Variable;
	Variable.uDeclaration = 0;
	Variable.uFunction    = 0;
	Variable.uGlobal      = m_Parent.m_GlobalVariablePool.Add(GlobalVariable) + 1;
	Variable.uParent      = 0;

	Variable.Type                                 = VariableType;
	Variable.uRegister                            = m_pConstantBufferDeclaration->uRegisterCount;
	m_pConstantBufferDeclaration->uRegisterCount += uCount * m_Parent.GetRegisterCount(VariableType, 0);

	uHandle = m_Parent.m_VariablePool.Add(Variable) + 1;

	m_Parent.m_GlobalVariableHandleMap.Insert(GlobalVariable, uHandle);
	
	if(CompilerInfo.bIsSupportConstantBuffer)
		m_Parent.m_GlobalConstantBufferMap.Insert(uHandle, m_uHandle);
	else
	{
		GlobalVariable.ShaderType = m_pConstantBufferDeclaration->Type;
		GlobalVariable.GlobalType = BUFFER_VARIABLE;
		GlobalVariable.uDetail    = m_uHandle;
		GlobalVariable.uCount     = uCount;
		GlobalVariable.uIndex     = Variable.uRegister;
		GlobalVariable.pValue     = ALCHEMY_NULL;
		
		m_Parent.m_GlobalConstantBufferMap.Insert( uHandle, m_Parent.AddGlobalVariable(GlobalVariable) );
	}
	
	m_StructDefiner.AddType(VariableType, 0);

	return uHandle;
}

UINT CEffectCode::CConstantBufferDefiner::EndDefine()
{
	m_pConstantBufferDeclaration->uDeclaration = m_StructDefiner.EndDefine();
	m_pConstantBufferDeclaration               = ALCHEMY_NULL;
	
	UINT uResult = m_uHandle;
	m_uHandle    = 0;

	return uResult;
}

CEffectCode::CShaderParametersDefiner::CShaderParametersDefiner(CEffectCode& Parent) :
CDefiner(Parent),
m_pShaderParameter(ALCHEMY_NULL)
{

}

CEffectCode::CShaderParametersDefiner::~CShaderParametersDefiner()
{

}

/*const UINT* CEffectCode::CShaderParametersDefiner::GetShaderParameterKeyCount(const SHADERPARAMETERKEY& ShaderParameterKey)
{
	return m_ShaderParameterCountMap.Get(ShaderParameterKey);
}*/

void CEffectCode::CShaderParametersDefiner::AppendParameter(CVertexDeclaration::ELEMENTTYPE Type, CVertexDeclaration::ELEMENTUSAGE Usage, UINT uIndex)
{
	/*SHADERPARAMETERKEY ShaderParameterKey;
	ShaderParameterKey.Type  = Type;
	ShaderParameterKey.Usage = Usage;
	PUINT puCount = m_ShaderParameterCountMap.Get(ShaderParameterKey);
	UINT uIndex;
	if(puCount)
		uIndex = (*puCount) ++;
	else
	{
		m_ShaderParameterCountMap.Insert(ShaderParameterKey, 1);
		uIndex = 0;
	}*/

	SHADERPARAMETER ShaderParameter;

	ShaderParameter.Type   = Type;
	ShaderParameter.Usage  = Usage;
	ShaderParameter.uIndex = uIndex;
	ShaderParameter.uNext  = 0;

	uIndex = m_Parent.m_ShaderParameterPool.Add(ShaderParameter);
	if(m_pShaderParameter)
		m_pShaderParameter->uNext = uIndex + 1;
	else
		m_uHandle = m_Parent.m_ShaderParametersPool.Add(uIndex + 1) + 1;

	m_pShaderParameter = m_Parent.m_ShaderParameterPool.Get(uIndex);
}

UINT CEffectCode::CShaderParametersDefiner::EndDefine()
{
	//m_ShaderParameterCountMap.Clear();

	m_pShaderParameter = ALCHEMY_NULL;

	UINT uResult = m_uHandle;
	m_uHandle    = 0;

	return uResult;
}

CEffectCode::CBasicFunctionDefiner::CBasicFunctionDefiner(CEffectCode& Parent) :
CDefiner(Parent),
m_uUserVariables(0),
m_uParametersVariable(0),
m_uResultVariable(0),
m_Type(IEffect::PROGRAM),
m_pFunction(ALCHEMY_NULL),
m_TemporaryRegisterApplicant(Parent)
//m_uTemporaryRegisterCount(0)
{
}

CEffectCode::CBasicFunctionDefiner::~CBasicFunctionDefiner()
{
}

bool CEffectCode::CBasicFunctionDefiner::AppendInstruction(
					   ICompiler::INSTRUCTION Instruction,
					   UINT uDestination,
					   UINT uSource1,
					   UINT uSource2)
{
	//USERVARIABLEKEY UserVariableKey;
	if(Instruction == ICompiler::MOV && uDestination == uSource1)
		return true;

	LPVARIABLE pDestinationVariable = ALCHEMY_NULL, pVariable1 = ALCHEMY_NULL, pVariable2 = ALCHEMY_NULL;
	if(uDestination && ( pDestinationVariable = m_Parent.m_VariablePool.Get(uDestination - 1) ) != ALCHEMY_NULL)
	{
		if(pDestinationVariable->Type == STRUCT_VARIABLE)
		{
			ALCHEMY_DEBUG_WARNING("CEffectCode::CBasicFunctionDefiner::AppendInstruction: Destination can not be struct variable.");

			return false;
		}
	}

	if(uSource1 && ( pVariable1 = m_Parent.m_VariablePool.Get(uSource1 - 1) ) != ALCHEMY_NULL)
	{
		if(pVariable1->Type == STRUCT_VARIABLE)
		{
			ALCHEMY_DEBUG_WARNING("CEffectCode::CBasicFunctionDefiner::AppendInstruction: Source1 can not be struct variable.");

			return false;
		}
	}

	if(uSource2 && ( pVariable2 = m_Parent.m_VariablePool.Get(uSource2 - 1) ) != ALCHEMY_NULL)
	{
		if(pVariable2->Type == STRUCT_VARIABLE)
		{
			ALCHEMY_DEBUG_WARNING("CEffectCode::CBasicFunctionDefiner::AppendInstruction: Source2 can not be struct variable.");

			return false;
		}
	}

	const UINT uADDRESS[] = {0, 1, 2};

	UINT uDestinationAddressCount = 0, uAddressCount1 = 0, uAddressCount2 = 0;//, uDeclaration;
	//PUINT puShaderParameters;
	//LPSHADERPARAMETER pShaderParameter;
	if(pDestinationVariable)
	{
		switch(pDestinationVariable->Type)
		{
		case VECTOR1_VARIABLE:
			uDestinationAddressCount = 1;
			break;
		case VECTOR2_VARIABLE:
			uDestinationAddressCount = 2;
			break;
		case VECTOR3_VARIABLE:
			uDestinationAddressCount = 3;
			break;
        default:
            break;
		}

		/*UserVariableKey.Type = m_Parent.GetRegisterType(uDestination);
		UserVariableKey.uRegister = pDestinationVariable->uRegister;
		uDeclaration = 0;

		if(UserVariableKey.Type == ICompiler::GLOBAL || UserVariableKey.Type == ICompiler::PARAMETER)
		{
			uDeclaration = 0;
			while(pDestinationVariable && pDestinationVariable->uParent)
			{
				uDeclaration += pDestinationVariable->uGlobal;
				pDestinationVariable = m_Parent.m_VariablePool.Get(pDestinationVariable->uParent - 1);
			}

			if(pDestinationVariable && pDestinationVariable->uGlobal)
			{
				if(UserVariableKey.Type == ICompiler::GLOBAL)
				{
					UserVariableKey.uRegister    = pDestinationVariable->uRegister;
					uDeclaration = pDestinationVariable->uGlobal;
				}
				else if(UserVariableKey.Type == ICompiler::PARAMETER)
				{
					puShaderParameters = m_Parent.m_ShaderParametersPool.Get(pDestinationVariable->uGlobal - 1);
					if( puShaderParameters && (uDeclaration = *puShaderParameters) )
					{
						pShaderParameter = m_Parent.m_ShaderParameterPool.Get(uDeclaration - 1);

						while(pShaderParameter && uDeclaration --)
						{
							if(pShaderParameter->uNext)
								uDeclaration = pShaderParameter->uNext;

							pShaderParameter = pShaderParameter->uNext ? m_Parent.m_ShaderParameterPool.Get(pShaderParameter->uNext - 1) : ALCHEMY_NULL;
						}
					}
				}
			}
		}

		m_UserVariableDeclarationMap.Insert(UserVariableKey, uDeclaration);*/
	}
	else
		uDestinationAddressCount = 0;

	if(pVariable1)
	{
		switch(pVariable1->Type)
		{
		case VECTOR1_VARIABLE:
			uAddressCount1 = 1;
			break;
		case VECTOR2_VARIABLE:
			uAddressCount1 = 2;
			break;
		case VECTOR3_VARIABLE:
			uAddressCount1 = 3;
			break;
        default:
            break;
		}

		/*UserVariableKey.Type = m_Parent.GetRegisterType(uDestination);
		UserVariableKey.uRegister = pVariable1->uRegister;
		uDeclaration = 0;

		if(UserVariableKey.Type == ICompiler::GLOBAL || UserVariableKey.Type == ICompiler::PARAMETER)
		{
			uDeclaration = 0;
			while(pVariable1 && pVariable1->uParent)
			{
				uDeclaration += pVariable1->uGlobal;
				pVariable1 = m_Parent.m_VariablePool.Get(pVariable1->uParent - 1);
			}

			if(pVariable1 && pVariable1->uGlobal)
			{
				if(UserVariableKey.Type == ICompiler::GLOBAL)
				{
					UserVariableKey.uRegister    = pVariable1->uRegister;
					uDeclaration = pVariable1->uGlobal;
				}
				else if(UserVariableKey.Type == ICompiler::PARAMETER)
				{
					puShaderParameters = m_Parent.m_ShaderParametersPool.Get(pVariable1->uGlobal - 1);
					if( puShaderParameters && (uDeclaration = *puShaderParameters) )
					{
						pShaderParameter = m_Parent.m_ShaderParameterPool.Get(uDeclaration - 1);

						while(pShaderParameter && uDeclaration --)
						{
							if(pShaderParameter->uNext)
								uDeclaration = pShaderParameter->uNext;

							pShaderParameter = pShaderParameter->uNext ? m_Parent.m_ShaderParameterPool.Get(pShaderParameter->uNext - 1) : ALCHEMY_NULL;
						}
					}
				}
			}
		}

		m_UserVariableDeclarationMap.Insert(UserVariableKey, uDeclaration);*/
	}
	else
		uAddressCount1 = 0;

	if(pVariable2)
	{
		switch(pVariable2->Type)
		{
		case VECTOR1_VARIABLE:
			uAddressCount2 = 1;
			break;
		case VECTOR2_VARIABLE:
			uAddressCount2 = 2;
			break;
		case VECTOR3_VARIABLE:
			uAddressCount2 = 3;
			break;
        default:
            break;
		}

		/*UserVariableKey.Type = m_Parent.GetRegisterType(uDestination);
		UserVariableKey.uRegister = pVariable2->uRegister;
		uDeclaration = 0;

		if(UserVariableKey.Type == ICompiler::GLOBAL || UserVariableKey.Type == ICompiler::PARAMETER)
		{
			uDeclaration = 0;
			while(pVariable2 && pVariable2->uParent)
			{
				uDeclaration += pVariable2->uGlobal;
				pVariable2 = m_Parent.m_VariablePool.Get(pVariable2->uParent - 1);
			}

			if(pVariable2 && pVariable2->uGlobal)
			{
				if(UserVariableKey.Type == ICompiler::GLOBAL)
				{
					UserVariableKey.uRegister    = pVariable2->uRegister;
					uDeclaration = pVariable2->uGlobal;
				}
				else if(UserVariableKey.Type == ICompiler::PARAMETER)
				{
					puShaderParameters = m_Parent.m_ShaderParametersPool.Get(pVariable2->uGlobal - 1);
					if( puShaderParameters && (uDeclaration = *puShaderParameters) )
					{
						pShaderParameter = m_Parent.m_ShaderParameterPool.Get(uDeclaration - 1);

						while(pShaderParameter && uDeclaration --)
						{
							if(pShaderParameter->uNext)
								uDeclaration = pShaderParameter->uNext;

							pShaderParameter = pShaderParameter->uNext ? m_Parent.m_ShaderParameterPool.Get(pShaderParameter->uNext - 1) : ALCHEMY_NULL;
						}
					}
				}
			}
		}

		m_UserVariableDeclarationMap.Insert(UserVariableKey, uDeclaration);*/
	}
	else
		uAddressCount2 = 0;

	UINT uCodeBegin = m_Code.GetLength();
	INSTRUCTION CurrentInstruction;
	CurrentInstruction.Opcode                   = Instruction;
	CurrentInstruction.uCodeBegin               = uCodeBegin;
	CurrentInstruction.uDestination             = uDestination;
	CurrentInstruction.uDestinationIndex        = 0;
	CurrentInstruction.uDestinationIndexAddress = 0;
	CurrentInstruction.uSource1                 = uSource1;
	CurrentInstruction.uSourceIndex1            = 0;
	CurrentInstruction.uSourceIndexAddress1     = 0;
	CurrentInstruction.uSource2                 = uSource2;
	CurrentInstruction.uSourceIndex2            = 0;
	CurrentInstruction.uSourceIndexAddress2     = 0;
	CurrentInstruction.uPrevious                = 0;

	ICompiler::REGISTERTYPE Type = uDestination ? m_Parent.GetRegisterType(uDestination) : ICompiler::NONE;

	if( Type != ICompiler::RESULT && Type != ICompiler::OUTPUT && (!uSource1 || m_Parent.GetRegisterType(uSource1) ==  ICompiler::GLOBAL) && (!uSource2 || m_Parent.GetRegisterType(uSource2) ==  ICompiler::GLOBAL) )
	{
		CSmartPointer<CBuffer> AddressPointer;

		if(uDestination && uDestinationAddressCount)
		{
			AddressPointer = &CreateBuffer(uDestinationAddressCount * sizeof(UINT), true);
			AddressPointer->CopyFrom(uADDRESS);
			CurrentInstruction.uDestinationAddressInfo = m_Parent.m_BufferPool.Add(AddressPointer) + 1;
		}
		else
			CurrentInstruction.uDestinationAddressInfo = 0;

		if(uSource1 && uAddressCount1)
		{
			AddressPointer = &CreateBuffer(uAddressCount1 * sizeof(UINT), true);
			AddressPointer->CopyFrom(uADDRESS);
			CurrentInstruction.uSourceAddressInfo1 = m_Parent.m_BufferPool.Add(AddressPointer) + 1;
		}
		else
			CurrentInstruction.uSourceAddressInfo1 = 0;


		if(uSource2 && uAddressCount2)
		{
			AddressPointer = &CreateBuffer(uAddressCount2 * sizeof(UINT), true);
			AddressPointer->CopyFrom(uADDRESS);
			CurrentInstruction.uSourceAddressInfo2 = m_Parent.m_BufferPool.Add(AddressPointer) + 1;
		}
		else
			CurrentInstruction.uSourceAddressInfo2 = 0;
	}
	else
	{
		if( !m_Parent.m_Compiler.AppendInstruction(
			m_Code, 
			Instruction, 
			uDestination, 
			0,
			0,
			uADDRESS, 
			uDestinationAddressCount, 
			uSource1, 
			0,
			0,
			uADDRESS, 
			uAddressCount1,
			uSource2, 
			0,
			0,
			uADDRESS, 
			uAddressCount2, 
			m_Parent) )
			return false;

		CurrentInstruction.uDestinationAddressInfo = 0;
		CurrentInstruction.uSourceAddressInfo1     = 0;
		CurrentInstruction.uSourceAddressInfo2     = 0;
	}

	CurrentInstruction.uCodeLength = m_Code.GetLength() - uCodeBegin;

	if(uDestinationAddressCount)
	{
		CurrentInstruction.uDestinationAddress = 0;
		while(uDestinationAddressCount --)
			ALCHEMY_SET_BIT(CurrentInstruction.uDestinationAddress, uADDRESS[uDestinationAddressCount]);

		CurrentInstruction.uDestinationAddress <<= m_Parent.m_Compiler.GetAddressShift(uDestination, m_Parent);
	}
	else
		CurrentInstruction.uDestinationAddress = ~0;

	if(uAddressCount1)
	{
		CurrentInstruction.uSourceAddress1 = 0;
		while(uAddressCount1 --)
			ALCHEMY_SET_BIT(CurrentInstruction.uSourceAddress1, uADDRESS[uAddressCount1]);

		CurrentInstruction.uSourceAddress1 <<= m_Parent.m_Compiler.GetAddressShift(uSource1, m_Parent);
	}
	else
		CurrentInstruction.uSourceAddress1 = ~0;

	if(uAddressCount2)
	{
		CurrentInstruction.uSourceAddress2 = 0;
		while(uAddressCount2 --)
			ALCHEMY_SET_BIT(CurrentInstruction.uSourceAddress2, uADDRESS[uAddressCount2]);

		CurrentInstruction.uSourceAddress2 <<= m_Parent.m_Compiler.GetAddressShift(uSource2, m_Parent);
	}
	else
		CurrentInstruction.uSourceAddress2 = ~0;

	m_InstructionSet.Add(CurrentInstruction);

	return true;
}

bool CEffectCode::CBasicFunctionDefiner::AppendInstruction(
	ICompiler::INSTRUCTION Instruction,
	UINT uDestination, 
	const UINT* puDestinationAddress, 
	UINT uDestinationAddressCount, 
	UINT uSource1, 
	const UINT* puAddress1, 
	UINT uAddressCount1, 
	UINT uSource2, 
	const UINT* puAddress2, 
	UINT uAddressCount2)
{
	//USERVARIABLEKEY UserVariableKey;

	LPVARIABLE pDestinationVariable = ALCHEMY_NULL, pVariable1 = ALCHEMY_NULL, pVariable2 = ALCHEMY_NULL;
	if(uDestination && ( pDestinationVariable = m_Parent.m_VariablePool.Get(uDestination - 1) ) != ALCHEMY_NULL)
	{
		if(pDestinationVariable->Type == STRUCT_VARIABLE)
		{
			ALCHEMY_DEBUG_WARNING("CEffectCode::CBasicFunctionDefiner::AppendInstruction: Destination can not be struct variable.");

			return false;
		}
	}

	if(uSource1 && ( pVariable1 = m_Parent.m_VariablePool.Get(uSource1 - 1) ) != ALCHEMY_NULL)
	{
		if(pVariable1->Type == STRUCT_VARIABLE)
		{
			ALCHEMY_DEBUG_WARNING("CEffectCode::CBasicFunctionDefiner::AppendInstruction: Source1 can not be struct variable.");

			return false;
		}
	}

	if(uSource2 && ( pVariable2 = m_Parent.m_VariablePool.Get(uSource2 - 1) ) != ALCHEMY_NULL)
	{
		if(pVariable2->Type == STRUCT_VARIABLE)
		{
			ALCHEMY_DEBUG_WARNING("CEffectCode::CBasicFunctionDefiner::AppendInstruction: Source2 can not be struct variable.");

			return false;
		}
	}

	if(pDestinationVariable)
	{
		//UserVariableKey.Type = m_Parent.GetRegisterType(uDestination);
		//UserVariableKey.uRegister = pDestinationVariable->uRegister;

		//m_UserVariableDeclarationMap.Insert(UserVariableKey, (UserVariableKey.Type == ICompiler::GLOBAL || UserVariableKey.Type == ICompiler::PARAMETER) ? pDestinationVariable->uGlobal : 0);
	}
	else
		uDestinationAddressCount = 0;

	const UINT uADDRESS[] = {0, 1, 2};

	if(pVariable1)
	{
		//UserVariableKey.Type = m_Parent.GetRegisterType(uSource1);
		//UserVariableKey.uRegister = pVariable1->uRegister;

		//m_UserVariableDeclarationMap.Insert(UserVariableKey, (UserVariableKey.Type == ICompiler::GLOBAL || UserVariableKey.Type == ICompiler::PARAMETER) ? pVariable1->uGlobal : 0);

		if(!uAddressCount1)
		{
			switch(pVariable1->Type)
			{
			case VECTOR1_VARIABLE:
				uAddressCount1 = 1;
				break;
			case VECTOR2_VARIABLE:
				uAddressCount1 = 2;
				break;
			case VECTOR3_VARIABLE:
				uAddressCount1 = 3;
				break;
            default:
                break;
			}

			puAddress1 = uADDRESS;
		}
	}
	else
		uAddressCount1 = 0;

	if(pVariable2)
	{
		//UserVariableKey.Type = m_Parent.GetRegisterType(uSource2);
		//UserVariableKey.uRegister = pVariable2->uRegister;

		//m_UserVariableDeclarationMap.Insert(UserVariableKey, (UserVariableKey.Type == ICompiler::GLOBAL || UserVariableKey.Type == ICompiler::PARAMETER) ? pVariable2->uGlobal : 0);

		if(!uAddressCount2)
		{
			switch(pVariable2->Type)
			{
			case VECTOR1_VARIABLE:
				uAddressCount2 = 1;
				break;
			case VECTOR2_VARIABLE:
				uAddressCount2 = 2;
				break;
			case VECTOR3_VARIABLE:
				uAddressCount2 = 3;
				break;
            default:
                break;
			}

			puAddress2 = uADDRESS;
		}
	}
	else
		uAddressCount2 = 0;

	if(Instruction == ICompiler::MOV && uDestination == uSource1 && uDestinationAddressCount == uAddressCount1)
	{
		if(puDestinationAddress == puAddress1)
			return true;

		UINT i;
		for(i = 0; i < uDestinationAddressCount; ++ i)
		{
			if(puDestinationAddress[i] != puAddress1[i])
				break;
		}

		if(i == uDestinationAddressCount)
			return true;
	}

	UINT uCodeBegin = m_Code.GetLength();

	INSTRUCTION CurrentInstruction;
	CurrentInstruction.Opcode                   = Instruction;
	CurrentInstruction.uCodeBegin               = uCodeBegin;
	CurrentInstruction.uDestination             = uDestination;
	CurrentInstruction.uDestinationIndex        = 0;
	CurrentInstruction.uDestinationIndexAddress = 0;
	CurrentInstruction.uSource1                 = uSource1;
	CurrentInstruction.uSourceIndex1            = 0;
	CurrentInstruction.uSourceIndexAddress1     = 0;
	CurrentInstruction.uSource2                 = uSource2;
	CurrentInstruction.uSourceIndex2            = 0;
	CurrentInstruction.uSourceIndexAddress2     = 0;
	CurrentInstruction.uPrevious                = 0;

	ICompiler::REGISTERTYPE Type = uDestination ? m_Parent.GetRegisterType(uDestination) : ICompiler::NONE;

	if( Type != ICompiler::RESULT && Type != ICompiler::OUTPUT && (!uSource1 || m_Parent.GetRegisterType(uSource1) ==  ICompiler::GLOBAL) && (!uSource2 || m_Parent.GetRegisterType(uSource2) ==  ICompiler::GLOBAL) )
	{
		CSmartPointer<CBuffer> AddressPointer;

		if(uDestination && uDestinationAddressCount)
		{
			AddressPointer = &CreateBuffer(uDestinationAddressCount * sizeof(UINT), true);
			AddressPointer->CopyFrom(puDestinationAddress);
			CurrentInstruction.uDestinationAddressInfo = m_Parent.m_BufferPool.Add(AddressPointer) + 1;
		}
		else
			CurrentInstruction.uDestinationAddressInfo = 0;

		if(uSource1 && uAddressCount1)
		{
			AddressPointer = &CreateBuffer(uAddressCount1 * sizeof(UINT), true);
			AddressPointer->CopyFrom(puAddress1);
			CurrentInstruction.uSourceAddressInfo1 = m_Parent.m_BufferPool.Add(AddressPointer) + 1;
		}
		else
			CurrentInstruction.uSourceAddressInfo1 = 0;

		if(uSource2 && uAddressCount2)
		{
			AddressPointer = &CreateBuffer(uAddressCount2 * sizeof(UINT), true);
			AddressPointer->CopyFrom(puAddress2);
			CurrentInstruction.uSourceAddressInfo2 = m_Parent.m_BufferPool.Add(AddressPointer) + 1;
		}
		else
			CurrentInstruction.uSourceAddressInfo2 = 0;
	}
	else
	{
		if(!m_Parent.m_Compiler.AppendInstruction(
			m_Code, 
			Instruction, 
			uDestination, 
			0,
			0,
			puDestinationAddress, 
			uDestinationAddressCount, 
			uSource1, 
			0,
			0,
			puAddress1, 
			uAddressCount1,
			uSource2, 
			0,
			0,
			puAddress2, 
			uAddressCount2, 
			m_Parent) )
			return false;

		CurrentInstruction.uDestinationAddressInfo = 0;
		CurrentInstruction.uSourceAddressInfo1     = 0;
		CurrentInstruction.uSourceAddressInfo2     = 0;
	}

	CurrentInstruction.uCodeLength = m_Code.GetLength() - uCodeBegin;

	if(uDestinationAddressCount)
	{
		CurrentInstruction.uDestinationAddress = 0;
		while(uDestinationAddressCount --)
			ALCHEMY_SET_BIT(CurrentInstruction.uDestinationAddress, puDestinationAddress[uDestinationAddressCount]);

		CurrentInstruction.uDestinationAddress <<= m_Parent.m_Compiler.GetAddressShift(uDestination, m_Parent);
	}
	else
		CurrentInstruction.uDestinationAddress = ~0;

	if(uAddressCount1)
	{
		CurrentInstruction.uSourceAddress1 = 0;
		while(uAddressCount1 --)
			ALCHEMY_SET_BIT(CurrentInstruction.uSourceAddress1, puAddress1[uAddressCount1]);

		CurrentInstruction.uSourceAddress1 <<= m_Parent.m_Compiler.GetAddressShift(uSource1, m_Parent);
	}
	else
		CurrentInstruction.uSourceAddress1 = ~0;

	if(uAddressCount2)
	{
		CurrentInstruction.uSourceAddress2 = 0;
		while(uAddressCount2 --)
			ALCHEMY_SET_BIT(CurrentInstruction.uSourceAddress2, puAddress2[uAddressCount2]);

		CurrentInstruction.uSourceAddress2 <<= m_Parent.m_Compiler.GetAddressShift(uSource2, m_Parent);
	}
	else
		CurrentInstruction.uSourceAddress2 = ~0;

	m_InstructionSet.Add(CurrentInstruction);

	return true;
}

bool CEffectCode::CBasicFunctionDefiner::AppendInstruction(
					   ICompiler::INSTRUCTION Instruction, 
					   UINT uDestination, 
					   UINT uDestinationIndex,
					   UINT uDestinationIndexAddress, 
					   const UINT* puDestinationAddress, 
					   UINT uDestinationAddressCount, 
					   UINT uSource1, 
					   UINT uSourceIndex1,
					   UINT uSourceIndexAddress1, 
					   const UINT* puAddress1, 
					   UINT uAddressCount1, 
					   UINT uSource2, 
					   UINT uSourceIndex2,
					   UINT uSourceIndexAddress2,
					   const UINT* puAddress2, 
					   UINT uAddressCount2)
{
	//USERVARIABLEKEY UserVariableKey;

	LPVARIABLE pDestinationVariable = ALCHEMY_NULL, pVariable1 = ALCHEMY_NULL, pVariable2 = ALCHEMY_NULL;
	if(uDestination && ( pDestinationVariable = m_Parent.m_VariablePool.Get(uDestination - 1) ) != ALCHEMY_NULL)
	{
		if(pDestinationVariable->Type == STRUCT_VARIABLE)
		{
			ALCHEMY_DEBUG_WARNING("CEffectCode::CBasicFunctionDefiner::AppendInstruction: Destination can not be struct variable.");

			return false;
		}
	}

	if(uSource1 && ( pVariable1 = m_Parent.m_VariablePool.Get(uSource1 - 1) ) != ALCHEMY_NULL)
	{
		if(pVariable1->Type == STRUCT_VARIABLE)
		{
			ALCHEMY_DEBUG_WARNING("CEffectCode::CBasicFunctionDefiner::AppendInstruction: Source1 can not be struct variable.");

			return false;
		}
	}

	if(uSource2 && ( pVariable2 = m_Parent.m_VariablePool.Get(uSource2 - 1) ) != ALCHEMY_NULL)
	{
		if(pVariable2->Type == STRUCT_VARIABLE)
		{
			ALCHEMY_DEBUG_WARNING("CEffectCode::CBasicFunctionDefiner::AppendInstruction: Source2 can not be struct variable.");

			return false;
		}
	}

	/*if(pDestinationVariable)
	{
		UserVariableKey.Type = m_Parent.GetRegisterType(uDestination);
		UserVariableKey.uRegister = pDestinationVariable->uRegister;

		m_UserVariableDeclarationMap.Insert(UserVariableKey, (UserVariableKey.Type == ICompiler::GLOBAL || UserVariableKey.Type == ICompiler::PARAMETER) ? pDestinationVariable->uGlobal : 0);
	}

	if(pVariable1)
	{
		UserVariableKey.Type = m_Parent.GetRegisterType(uSource1);
		UserVariableKey.uRegister = pVariable1->uRegister;

		m_UserVariableDeclarationMap.Insert(UserVariableKey, (UserVariableKey.Type == ICompiler::GLOBAL || UserVariableKey.Type == ICompiler::PARAMETER) ? pVariable1->uGlobal : 0);
	}

	if(pVariable2)
	{
		UserVariableKey.Type = m_Parent.GetRegisterType(uSource2);
		UserVariableKey.uRegister = pVariable2->uRegister;

		m_UserVariableDeclarationMap.Insert(UserVariableKey, (UserVariableKey.Type == ICompiler::GLOBAL || UserVariableKey.Type == ICompiler::PARAMETER) ? pVariable2->uGlobal : 0);
	}*/

	if(Instruction == ICompiler::MOV && uDestination == uSource1 && uDestinationIndex == uSourceIndex1 && uDestinationIndexAddress == uSourceIndexAddress1 && uDestinationAddressCount == uAddressCount1)
	{
		if(puDestinationAddress == puAddress1)
			return true;

		UINT i;
		for(i = 0; i < uDestinationAddressCount; ++ i)
		{
			if(puDestinationAddress[i] != puAddress1[i])
				break;
		}

		if(i == uDestinationAddressCount)
			return true;
	}

	UINT uCodeBegin = m_Code.GetLength();

	INSTRUCTION CurrentInstruction;
	CurrentInstruction.Opcode                   = Instruction;
	CurrentInstruction.uCodeBegin               = uCodeBegin;
	CurrentInstruction.uDestination             = uDestination;
	CurrentInstruction.uDestinationIndex        = uDestinationIndex;
	CurrentInstruction.uSource1                 = uSource1;
	CurrentInstruction.uSourceIndex1            = uSourceIndex1;
	CurrentInstruction.uSource2                 = uSource2;
	CurrentInstruction.uSourceIndex2            = uSourceIndex2;
	CurrentInstruction.uPrevious                = 0;

	ICompiler::REGISTERTYPE Type = uDestination ? m_Parent.GetRegisterType(uDestination) : ICompiler::NONE;

	if( Type != ICompiler::RESULT && Type != ICompiler::OUTPUT && (!uSource1 || m_Parent.GetRegisterType(uSource1) ==  ICompiler::GLOBAL) && (!uSource2 || m_Parent.GetRegisterType(uSource2) ==  ICompiler::GLOBAL) )
	{
		CSmartPointer<CBuffer> AddressPointer;

		if(uDestination && uDestinationAddressCount)
		{
			AddressPointer = &CreateBuffer(uDestinationAddressCount * sizeof(UINT), true);
			AddressPointer->CopyFrom(puDestinationAddress);
			CurrentInstruction.uDestinationAddressInfo = m_Parent.m_BufferPool.Add(AddressPointer) + 1;
		}
		else
			CurrentInstruction.uDestinationAddressInfo = 0;

		if(uSource1 && uAddressCount1)
		{
			AddressPointer = &CreateBuffer(uAddressCount1 * sizeof(UINT), true);
			AddressPointer->CopyFrom(puAddress1);
			CurrentInstruction.uSourceAddressInfo1 = m_Parent.m_BufferPool.Add(AddressPointer) + 1;
		}
		else
			CurrentInstruction.uSourceAddressInfo1 = 0;

		if(uSource2 && uAddressCount2)
		{
			AddressPointer = &CreateBuffer(uAddressCount2 * sizeof(UINT), true);
			AddressPointer->CopyFrom(puAddress2);
			CurrentInstruction.uSourceAddressInfo2 = m_Parent.m_BufferPool.Add(AddressPointer) + 1;
		}
		else
			CurrentInstruction.uSourceAddressInfo2 = 0;
	}
	else
	{
		if( !m_Parent.m_Compiler.AppendInstruction(
			m_Code, 
			Instruction, 
			uDestination, 
			uDestinationIndex,
			uDestinationIndexAddress,
			puDestinationAddress, 
			uDestinationAddressCount, 
			uSource1, 
			uSourceIndex1,
			uSourceIndexAddress1,
			puAddress1, 
			uAddressCount1,
			uSource2, 
			uSourceIndex2,
			uSourceIndexAddress2,
			puAddress2, 
			uAddressCount2, 
			m_Parent) )
			return false;

		CurrentInstruction.uDestinationAddressInfo = 0;
		CurrentInstruction.uSourceAddressInfo1     = 0;
		CurrentInstruction.uSourceAddressInfo2     = 0;
	}

	CurrentInstruction.uCodeLength = m_Code.GetLength() - uCodeBegin;

	if(uDestinationAddressCount)
	{
		CurrentInstruction.uDestinationAddress = 0;
		while(uDestinationAddressCount --)
			ALCHEMY_SET_BIT(CurrentInstruction.uDestinationAddress, puDestinationAddress[uDestinationAddressCount]);

		CurrentInstruction.uDestinationAddress <<= m_Parent.m_Compiler.GetAddressShift(uDestination, m_Parent);
	}
	else
		CurrentInstruction.uDestinationAddress = ~0;


	CurrentInstruction.uDestinationIndexAddress = uDestinationIndexAddress;
	CurrentInstruction.uDestinationAddress <<= m_Parent.m_Compiler.GetAddressShift(uDestinationIndex, m_Parent);

	if(uAddressCount1)
	{
		CurrentInstruction.uSourceAddress1 = 0;
		while(uAddressCount1 --)
			ALCHEMY_SET_BIT(CurrentInstruction.uSourceAddress1, puAddress1[uAddressCount1]);

		CurrentInstruction.uSourceAddress1 <<= m_Parent.m_Compiler.GetAddressShift(uSource1, m_Parent);
	}
	else
		CurrentInstruction.uSourceAddress1 = ~0;

	CurrentInstruction.uSourceIndexAddress1 = uSourceIndexAddress1;
	CurrentInstruction.uSourceIndexAddress1 <<= m_Parent.m_Compiler.GetAddressShift(uSourceIndex1, m_Parent);

	if(uAddressCount2)
	{
		CurrentInstruction.uSourceAddress2 = 0;
		while(uAddressCount2 --)
			ALCHEMY_SET_BIT(CurrentInstruction.uSourceAddress2, puAddress2[uAddressCount2]);

		CurrentInstruction.uSourceAddress2 <<= m_Parent.m_Compiler.GetAddressShift(uSource2, m_Parent);
	}
	else
		CurrentInstruction.uSourceAddress2 = ~0;

	CurrentInstruction.uSourceIndexAddress2 = uSourceIndexAddress2;
	CurrentInstruction.uSourceIndexAddress2 <<= m_Parent.m_Compiler.GetAddressShift(uSourceIndex2, m_Parent);

	m_InstructionSet.Add(CurrentInstruction);

	return true;
}

UINT CEffectCode::CBasicFunctionDefiner::ApplyVariable(VARIABLETYPE Type, UINT uDeclaration, UINT uCount)
{
	return _ApplyVariable(Type, uDeclaration, uCount, true);
}

UINT CEffectCode::CBasicFunctionDefiner::ApplyVariable(VARIABLETYPE Type, UINT uCount)
{
	if(Type == STRUCT_VARIABLE)
	{
		ALCHEMY_DEBUG_WARNING("CEffectCode::CBasicFunctionDefiner::ApplyVariable: Apply varialbe fail.");

		return 0;
	}

	return _ApplyVariable(Type, 0, uCount, true);
}

UINT CEffectCode::CBasicFunctionDefiner::ApplyVariable(UINT uShaderParameters)
{
	return _ApplyVariable(uShaderParameters, true);
}

UINT CEffectCode::CBasicFunctionDefiner::ApplyConstant(UINT uVariable)
{
	const UINT* puConstant = m_VariableConstantMap.Get(uVariable);
	if(puConstant)
		return *puConstant;
	
	if(!m_Parent.m_Compiler.GetCompilerInfo().bIsSupportConstantBuffer)
	{
		const UINT* puHandle = m_Parent.m_GlobalConstantBufferMap.Get(uVariable);
		if(puHandle && *puHandle)
		{
			const VARIABLE* pVariable = uVariable ? m_Parent.m_VariablePool.Get(uVariable - 1) : ALCHEMY_NULL;
			if(pVariable)
			{
				const GLOBALVARIABLE* pGlobalVariable = pVariable->uGlobal ? m_Parent.m_GlobalVariablePool.Get(pVariable->uGlobal - 1) : ALCHEMY_NULL;
				if(pGlobalVariable)
				{
					UINT uTemp = ApplyVariable(pVariable->Type, pGlobalVariable->uCount);
					INSTRUCTION CurrentInstruction;
					CurrentInstruction.Opcode                  = ICompiler::TEX;
					CurrentInstruction.uCodeBegin              = 0;
					CurrentInstruction.uDestination            = uTemp;
					CurrentInstruction.uDestinationIndex       = 0;
					CurrentInstruction.uDestinationAddress     = ~0;
					CurrentInstruction.uDestinationAddressInfo = 0;
					CurrentInstruction.uSource1                = *puHandle;
					CurrentInstruction.uSourceIndex1           = 0;
					CurrentInstruction.uSourceAddress1         = ~0;
					CurrentInstruction.uSourceAddressInfo1     = 0;
					CurrentInstruction.uSource2                = 0;
					CurrentInstruction.uSourceIndex2           = 0;
					CurrentInstruction.uSourceAddress2         = ~0;
					CurrentInstruction.uSourceAddressInfo2     = 0;
					CurrentInstruction.uPrevious               = 0;
					
					m_InstructionSet.Add(CurrentInstruction);
					
					m_VariableConstantMap.Insert(uVariable, uTemp);
					
					return uTemp;
				}
			}
			
			return 0;
		}
	}
	
	return uVariable;
}

//bool CEffectCode::CBasicFunctionDefiner::EndFunction(UINT uResultVariable)
//{
//	//if(!m_pFunction)
//	//	__ApplyFunction();
//
//	if( _IsValuableResultVariable(uResultVariable) )
//	{
//		m_uResultVariable = uResultVariable;
//
//		return true;
//	}
//
//	return false;
//}

bool CEffectCode::CBasicFunctionDefiner::FreeVariable(UINT uHandle)
{
	if(!uHandle)
	{
		ALCHEMY_DEBUG_WARNING("CEffectCode::CBasicFunctionDefiner::FreeVariable: Error handle.");

		return false;
	}

	UINT uIndex = uHandle - 1;
	LPVARIABLE pVariable = m_Parent.m_VariablePool.Get(uIndex);
	if(pVariable)
	{
		const UINT* puCount = m_Parent.m_TemporaryVariableCountMap.Get(uHandle);

		if(puCount)
		{
			/*CPool<VARIABLE>::CIterator VariableIterator = m_Parent.m_VariablePool.GetIterator(0);
			while( CPool<VARIABLE>::IsValuable(VariableIterator) )
			{
				if(VariableIterator->uParent == uHandle)
					m_Parent.m_VariablePool.Remove(VariableIterator.GetIndex(), 1);

				++ VariableIterator;
			}*/

			/*UINT uCount = *puCount, 
				uRegisterCount = m_Parent.GetRegisterCount(pVariable->Type),
				uRegisterBegin = pVariable->uRegister, 
				uByteAlignment = 1 << m_Parent.m_Compiler.GetCompilerInfo().uByteAlignmentShift, 
				uByteAlignmentMask = uByteAlignment - 1,
				uRegisterEnd,
				uCurrentRegisterCount;

			CPool<REGISTERSEGMENT>::CIterator RegisterSegmentIterator = m_RegisterSegmentPool.GetIterator(0);
			LPREGISTERSEGMENT pRegisterSegment;
			REGISTERSEGMENT RegisterSegment;
			for(UINT i = 0; i < uCount; ++ i)
			{
				uRegisterEnd = uRegisterBegin + pVariable->Type;

				RegisterSegmentIterator = m_RegisterSegmentPool.GetIterator(0);
				while( CPool<REGISTERSEGMENT>::IsValuable(RegisterSegmentIterator) )
				{
					pRegisterSegment = &(*RegisterSegmentIterator);
					uCurrentRegisterCount = pRegisterSegment->uBegin + pRegisterSegment->uCount;

					if( (pRegisterSegment->uCount + pVariable->Type) <= uByteAlignment )
					{
						if(pRegisterSegment->uBegin == uRegisterEnd && (uRegisterEnd & uByteAlignmentMask) != 0)
						{
							pRegisterSegment->uBegin  = uRegisterBegin;
							pRegisterSegment->uCount += pVariable->Type;

							break;
						}
						else if(uCurrentRegisterCount == uRegisterBegin && (uCurrentRegisterCount & uByteAlignmentMask) != 0)
						{
							pRegisterSegment->uCount += pVariable->Type;

							break;
						}
					}

					++ RegisterSegmentIterator;
				}

				if( !CPool<REGISTERSEGMENT>::IsValuable(RegisterSegmentIterator) )
				{
					RegisterSegment.uBegin = uRegisterBegin;
					RegisterSegment.uCount = pVariable->Type;

					m_RegisterSegmentPool.Add(RegisterSegment);
				}

				uRegisterBegin += uRegisterCount;
			}*/

			//*puCount = 0;

			//m_Parent.m_VariablePool.Remove(uIndex, 1);
			
			m_TemporaryRegisterApplicant.Free(pVariable->uRegister, pVariable->Type, pVariable->uDeclaration, *puCount);

			return true;
		}
	}

	ALCHEMY_DEBUG_WARNING("CEffectCode::CBasicFunctionDefiner::FreeVariable: Error handle.");

	return false;
}

UINT CEffectCode::CBasicFunctionDefiner::AddSamplerVariable(const IEffect::SAMPLERDECLARATION& Sampler, UINT uIndex)
{
	VARIABLE Variable;
	Variable.Type         = SAMPLER_VARIABLE;
	Variable.uRegister    = uIndex;
	Variable.uDeclaration = m_Parent.m_SamplerDeclarationPool.Add(Sampler) + 1;
	Variable.uFunction    = m_uHandle;
	Variable.uGlobal      = 0;
	Variable.uParent      = 0;
	
	UINT uHandle = m_Parent.m_VariablePool.Add(Variable) + 1;
	
	UINT uLength = m_Parent.m_CommonSamplers.GetLength();
	if(uIndex >= uLength)
		m_Parent.m_CommonSamplers.AddArray( (uIndex - uLength + CVector< CVector<UINT> >::ARRAY_MEMBER_COUNT) >> CVector< CVector<UINT> >::ARRAY_MEMBER_COUNT_SHIFT, true );

	m_Parent.m_CommonSamplers[uIndex].Add(uHandle);

	return uHandle;
}

UINT CEffectCode::CBasicFunctionDefiner::EndDefine()
{
	if(!m_pFunction)
		_ApplyFunction();

	m_pFunction->uCode               = m_Parent.m_CodePool.Add(m_Code) + 1;
	//m_pFunction->uUserVariables      = __ApplyUserVariables();
	m_pFunction->uParametersVariable = m_uParametersVariable;
	m_pFunction->uResultVariable     = m_uResultVariable;
	m_pFunction->uInstructionSet     = __ApplyInstructionSet();
	//m_pFunction->uShader             = 0;

	m_Code = ALCHEMY_NULL;

	//m_UserVariableDeclarationMap.Clear();
	//m_VariableCountMap.Clear();
	//m_RegisterSegmentPool.Clear();
	m_TemporaryRegisterApplicant.Clear();
	m_InstructionSet.Clear();
	m_VariableConstantMap.Clear();

	m_uUserVariables      = 0;
	m_uParametersVariable = 0;
	m_uResultVariable     = 0;
	m_pFunction           = ALCHEMY_NULL;

	//m_uTemporaryRegisterCount = 0;

	UINT uResult = m_uHandle;
	m_uHandle    = 0;

	return uResult;
}

UINT CEffectCode::CBasicFunctionDefiner::_ApplyVariable(VARIABLETYPE Type, UINT uDeclaration, UINT uCount, bool bIsTemporary)
{
	if(Type == SAMPLER_VARIABLE)
		return 0;

	if(!m_pFunction)
		_ApplyFunction();

	UINT uRegister = 0;

	if(bIsTemporary)
	{
		uRegister = m_TemporaryRegisterApplicant.Malloc(Type, uDeclaration, uCount);//m_uTemporaryRegisterCount;

		/*UINT uCurrentCount = Type, uByteAlignment = 1 << m_Parent.m_Compiler.GetCompilerInfo().uByteAlignmentShift, uByteAlignmentMask = uByteAlignment - 1;
		if(uCount == 1 && uCurrentCount <= uByteAlignment)
		{
			CPool<REGISTERSEGMENT>::CIterator Iterator = m_RegisterSegmentPool.GetIterator(0);
			LPREGISTERSEGMENT pRegisterSegment, pCurrentRegisterSegment = ALCHEMY_NULL;
			UINT uIndex;
			while( CPool<REGISTERSEGMENT>::IsValuable(Iterator) )
			{
				pRegisterSegment = &(*Iterator);
				if( pRegisterSegment->uCount == uCurrentCount && pRegisterSegment->uBegin < uRegister && ( Type != VECTOR3_VARIABLE || !(pRegisterSegment->uBegin & uByteAlignmentMask) ) )
				{
					pCurrentRegisterSegment = pRegisterSegment;
					uRegister = pRegisterSegment->uBegin;
					uIndex = Iterator.GetIndex();
				}

				++ Iterator;
			}

			if(pCurrentRegisterSegment)
			{
				pCurrentRegisterSegment->uBegin += uCurrentCount;
				pCurrentRegisterSegment->uCount -= uCurrentCount;

				if(pCurrentRegisterSegment->uCount == 0)
					m_RegisterSegmentPool.Remove(uIndex, 1);
			}
			else
			{
				Iterator = m_RegisterSegmentPool.GetIterator(0);
				while( CPool<REGISTERSEGMENT>::IsValuable(Iterator) )
				{
					pRegisterSegment = &(*Iterator);
					if(pRegisterSegment->uCount > uCurrentCount && pRegisterSegment->uBegin < uRegister)
					{
						pCurrentRegisterSegment = pRegisterSegment;
						uRegister = pRegisterSegment->uBegin;
						uIndex = Iterator.GetIndex();
					}

					++ Iterator;
				}

				if(pCurrentRegisterSegment)
				{
					pCurrentRegisterSegment->uBegin += uCurrentCount;
					pCurrentRegisterSegment->uCount -= uCurrentCount;

					if(pCurrentRegisterSegment->uCount == 0)
						m_RegisterSegmentPool.Remove(uIndex, 1);
				}
				else
				{
					UINT uRegisterCount = m_Parent.GetRegisterCount(Type, uDeclaration);

					if(uCurrentCount < uRegisterCount)
					{
						REGISTERSEGMENT RegisterSegment;
						RegisterSegment.uBegin = m_uTemporaryRegisterCount + uCurrentCount;
						RegisterSegment.uCount = uRegisterCount            - uCurrentCount;

						m_RegisterSegmentPool.Add(RegisterSegment);
					}

					m_uTemporaryRegisterCount += uRegisterCount;
				}
			}
		}
		else
		{
			UINT uRegisterCount = m_Parent.GetRegisterCount(Type, uDeclaration);

			if(uCurrentCount < uRegisterCount)
			{
				REGISTERSEGMENT RegisterSegment;
				RegisterSegment.uCount = uRegisterCount - uCurrentCount;
				UINT uBegin = m_uTemporaryRegisterCount + uCurrentCount;
				for(UINT i = 0; i < uCount; ++ i)
				{
					RegisterSegment.uBegin = uBegin;
					m_RegisterSegmentPool.Add(RegisterSegment);

					uBegin += uRegisterCount;
				}
			}

			m_uTemporaryRegisterCount += uRegisterCount * uCount;
		}*/
	}

	VARIABLE Variable;
	Variable.Type         = Type;
	Variable.uRegister    = uRegister;
	Variable.uDeclaration = uDeclaration;
	Variable.uFunction    = m_uHandle;
	Variable.uGlobal      = 0;
	Variable.uParent      = 0;

	UINT uHandle = m_Parent.m_VariablePool.Add(Variable) + 1;

	m_Parent.m_TemporaryVariableCountMap.Insert(uHandle, uCount);

	return uHandle;
}

UINT CEffectCode::CBasicFunctionDefiner::_ApplyVariable(UINT uShaderParameters, bool bIsTemporary)
{
	PUINT puShaderParameters;
	UINT uFirstShaderParameter;
	if( !uShaderParameters || !(puShaderParameters = m_Parent.m_ShaderParametersPool.Get(uShaderParameters - 1) ) ||  !(uFirstShaderParameter = *puShaderParameters) )
	{
		ALCHEMY_DEBUG_WARNING("CEffectCode::CShaderFunctionDefiner::BeginFunction: Error parameter handle.");

		return 0;
	}

	LPSHADERPARAMETER pShaderParameter = m_Parent.m_ShaderParameterPool.Get(uFirstShaderParameter - 1);
	CStructDefiner StructDefiner(m_Parent);
	while(pShaderParameter)
	{
		StructDefiner.AddType( static_cast<VARIABLETYPE>(pShaderParameter->Type) );
		pShaderParameter = pShaderParameter->uNext ? m_Parent.m_ShaderParameterPool.Get(pShaderParameter->uNext - 1) : ALCHEMY_NULL;
	}

	if(bIsTemporary)
		return _ApplyVariable(STRUCT_VARIABLE, StructDefiner.EndDefine(), 1, true);
	else
	{
		UINT uVariable = _ApplyVariable(STRUCT_VARIABLE, StructDefiner.EndDefine(), 1, false);

		LPVARIABLE pVariable = m_Parent.m_VariablePool.Get(uVariable - 1);

		if(pVariable)
		{
			pVariable->uGlobal = uShaderParameters;

			return uVariable;
		}
	}

	return 0;
}

//bool CEffectCode::CBasicFunctionDefiner::_IsValuableResultVariable(UINT uResultVariable)
//{
//	if(uResultVariable)
//	{
//		LPVARIABLE pVariable;
//		if(!( pVariable = m_Parent.m_VariablePool.Get(uResultVariable) ) || !pVariable->uFunction || pVariable->uFunction != m_uHandle)
//		{
//			ALCHEMY_DEBUG_WARNING("CEffectCode::CFunctionDefiner::EndFunction: Error result variable.");
//
//			return false;
//		}
//	}
//
//	m_uResultVariable = uResultVariable;
//
//	return true;
//}

void CEffectCode::CBasicFunctionDefiner::_ApplyFunction()
{
	FUNCTION Function;

	Function.uCode               = 0;
	Function.uParametersVariable = 0;
	Function.uResultVariable     = 0;
	Function.uShader             = 0;
	//Function.uUserVariables      = 0;

	UINT uIndex      = m_Parent.m_FunctionPool.Add(Function);

	m_pFunction      = m_Parent.m_FunctionPool.Get(uIndex);

	m_uHandle        = uIndex + 1;
}

/*UINT CEffectCode::CBasicFunctionDefiner::__ApplyUserVariables()
{
	UINT uDestination = 0, uIndex;
	const USERVARIABLEKEY* pKey;
	USERVARIABLE Temp;
	USERVARIABLE* pPrevious = ALCHEMY_NULL;
	CMap<USERVARIABLEKEY, UINT, USERVARIABLEKEYLESS, USERVARIABLEKEYEQUAL>::CIterator Iterator = m_UserVariableDeclarationMap.GetBeginIterator();

	while( CMap<USERVARIABLEKEY, UINT, USERVARIABLEKEYLESS, USERVARIABLEKEYEQUAL>::IsValuable(Iterator) )
	{
		pKey              = &Iterator.GetKey();
		Temp.Type         = pKey->Type;
		Temp.uRegister    = pKey->uRegister;
		Temp.uDeclaration = Iterator.GetValue();
		Temp.uNext        = 0;
		uIndex = m_Parent.m_UserVariablePool.Add(Temp);

		if(pPrevious)
			pPrevious->uNext = uIndex + 1;
		else
			uDestination = m_Parent.m_UserVariablesPool.Add(uIndex + 1) + 1;

		pPrevious = m_Parent.m_UserVariablePool.Get(uIndex);

		++ Iterator;
	}

	return uDestination;
}*/

UINT CEffectCode::CBasicFunctionDefiner::__ApplyInstructionSet()
{
	CVector<INSTRUCTION>::CIterator Iterator = m_InstructionSet.GetIterator(0);
	LPINSTRUCTION pInstruction;
	UINT uInstruction = 0;
	while( CVector<INSTRUCTION>::IsValuable(Iterator) )
	{
		pInstruction = &(*Iterator);

		pInstruction->uPrevious = uInstruction;

		uInstruction = m_Parent.m_InstructionPool.Add(*pInstruction) + 1;

		++ Iterator;
	}

	if(uInstruction)
		return m_Parent.m_InstructionSetPool.Add(uInstruction) + 1;

	return 0;
}

CEffectCode::CShaderFunctionDefiner::CShaderFunctionDefiner(CEffectCode& Parent, bool bIsVertexShader) :
CBasicFunctionDefiner(Parent)
//m_pShaderParameter(ALCHEMY_NULL)
{
	m_Type = bIsVertexShader ? IEffect::VERTEX_SHADER : IEffect::FRAGMENT_SHADER;
}

CEffectCode::CShaderFunctionDefiner::~CShaderFunctionDefiner()
{
}

//bool CEffectCode::CShaderFunctionDefiner::AppendParameter(CVertexDeclaration::ELEMENTTYPE Type, CVertexDeclaration::ELEMENTUSAGE Usage)
//{
//	if(m_uParametersVariable)
//	{
//		ALCHEMY_DEBUG_WARNING("CEffectCode::CShaderFunctionDefiner::AppendParameter: Parameter has been defined.");
//
//		return false;
//	}
//
//	SHADERPARAMETER ShaderParameter;
//
//	ShaderParameter.Type  = Type;
//	ShaderParameter.Usage = Usage;
//	ShaderParameter.uNext = 0;
//
//	UINT uIndex = m_ShaderParameterPool.Add(ShaderParameter);
//	if(m_pShaderParameter)
//		m_pShaderParameter->uNext = uIndex + 1;
//
//	m_pShaderParameter = m_ShaderParameterPool.Get(uIndex);
//
//	return true;
//}

//UINT CEffectCode::CShaderFunctionDefiner::EndAppendParameter()
//{
//	if(m_uParametersVariable)
//	{
//		ALCHEMY_DEBUG_WARNING("CEffectCode::CShaderFunctionDefiner::EndAppendParameter: Parameter has been defined.");
//
//		return 0;
//	}
//
//	CStructDefiner StructDefiner(m_Parent);
//	LPSHADERPARAMETER pShaderParameter = m_ShaderParameterPool.Get(0);
//	while(pShaderParameter)
//	{
//		StructDefiner.AddType( static_cast<VARIABLETYPE>(pShaderParameter->Type) );
//		pShaderParameter = pShaderParameter->uNext ? ALCHEMY_NULL : m_ShaderParameterPool.Get(pShaderParameter->uNext - 1);
//	}
//
//	m_uParametersVariable = _ApplyVariable(STRUCT_VARIABLE, StructDefiner.EndDefine(), false);
//
//	LPVARIABLE pVariable = m_Parent.m_VariablePool.Get(m_uParametersVariable - 1);
//
//	UINT uOldParameterIndex = 0;
//	LPSHADERPARAMETER pOldShaderParamter;
//	pShaderParameter = m_ShaderParameterPool.Get(0);
//
//	if(pShaderParameter)
//	{
//		uOldParameterIndex = m_Parent.m_ShaderParameterPool.Add(*pShaderParameter);
//
//		pVariable->uGlobal = m_Parent.m_ShaderParametersPool.Add(uOldParameterIndex + 1) + 1;
//
//		if(pShaderParameter->uNext)
//		{
//			pShaderParameter = m_ShaderParameterPool.Get(pShaderParameter->uNext - 1);
//
//			while(pShaderParameter)
//			{
//				pOldShaderParamter        = m_Parent.m_ShaderParameterPool.Get(uOldParameterIndex);
//
//				uOldParameterIndex        = m_Parent.m_ShaderParameterPool.Add(*pShaderParameter);
//
//				pOldShaderParamter->uNext = uOldParameterIndex + 1;
//
//				pShaderParameter          = pShaderParameter->uNext ? m_ShaderParameterPool.Get(pShaderParameter->uNext - 1) : ALCHEMY_NULL;
//			}
//		}
//
//		pOldShaderParamter        = m_Parent.m_ShaderParameterPool.Get(uOldParameterIndex);
//		pOldShaderParamter->uNext = 0;
//	}
//
//	return m_uParametersVariable;
//}

UINT CEffectCode::CShaderFunctionDefiner::BeginFunction(UINT uParameters)
{
	if(m_uParametersVariable)
	{
		ALCHEMY_DEBUG_WARNING("CEffectCode::CShaderFunctionDefiner::BeginFunction: Parameter has been defined.");

		return 0;
	}

	if(!m_uHandle)
		_ApplyFunction();

	/*PUINT puShaderParameters;
	UINT uFirstShaderParameter;
	if( !uParameters || !(puShaderParameters = m_Parent.m_ShaderParametersPool.Get(uParameters - 1) ) ||  !(uFirstShaderParameter = *puShaderParameters) )
	{
		ALCHEMY_DEBUG_WARNING("CEffectCode::CShaderFunctionDefiner::BeginFunction: Error parameter handle.");

		return 0;
	}

	LPSHADERPARAMETER pShaderParameter = m_Parent.m_ShaderParameterPool.Get(uFirstShaderParameter - 1);
	CStructDefiner StructDefiner(m_Parent);

	while(pShaderParameter)
	{
		StructDefiner.AddType( static_cast<VARIABLETYPE>(pShaderParameter->Type) );
		pShaderParameter = pShaderParameter->uNext ? ALCHEMY_NULL : m_Parent.m_ShaderParameterPool.Get(pShaderParameter->uNext - 1);
	}

	m_uParametersVariable = _ApplyVariable(STRUCT_VARIABLE, StructDefiner.EndDefine(), false);

	LPVARIABLE pVariable = m_Parent.m_VariablePool.Get(m_uParametersVariable - 1);

	if(pVariable)
	{
		pVariable->uGlobal = uParameters;

		return m_uParametersVariable;
	}*/

	LPFUNCTION pFunction;
	if(m_uHandle && ( pFunction = m_Parent.m_FunctionPool.Get(m_uHandle - 1) ) != ALCHEMY_NULL)
	{
		pFunction->uParametersVariable  = _ApplyVariable(uParameters, false);

		return m_uParametersVariable = pFunction->uParametersVariable;
	}

	return 0;
}

UINT CEffectCode::CShaderFunctionDefiner::EndFunction  (UINT uParameters)
{
	if(m_uResultVariable)
	{
		ALCHEMY_DEBUG_WARNING("CEffectCode::CShaderFunctionDefiner::EndFunction: result has been defined.");

		return 0;
	}

	if(!m_uHandle)
		_ApplyFunction();

	LPFUNCTION pFunction;
	if(m_uHandle && ( pFunction = m_Parent.m_FunctionPool.Get(m_uHandle - 1) ) != ALCHEMY_NULL)
	{
		pFunction->uResultVariable  = _ApplyVariable(uParameters, false);

		return m_uResultVariable = pFunction->uResultVariable;
	}

	return 0;
}

UINT CEffectCode::CShaderFunctionDefiner::EndDefine()
{
	if(!m_uResultVariable)
	{
		ALCHEMY_DEBUG_WARNING("CEffectCode::CShaderFunctionDefiner::EndDefine: Shader function can not be created without result variable.");

		return 0;
	}

	UINT uParameters = 0;
	if(m_uParametersVariable)
	{
		LPVARIABLE pVariable = m_Parent.m_VariablePool.Get(m_uParametersVariable - 1);
		if(!pVariable || !pVariable->uGlobal)
		{
			ALCHEMY_DEBUG_WARNING("CEffectCode::CShaderFunctionDefiner::EndDefine: Error parameters variable.");

			return 0;
		}

		uParameters = pVariable->uGlobal;
	}
	else if(m_Type == IEffect::VERTEX_SHADER)
	{
		ALCHEMY_DEBUG_WARNING("CEffectCode::CShaderFunctionDefiner::EndDefine: Vertex shader function can not be created without parameters variable.");

		return 0;
	}

	UINT uFunction = CBasicFunctionDefiner::EndDefine();
	LPFUNCTION pFunction;
	if( !uFunction || !( pFunction = m_Parent.m_FunctionPool.Get(uFunction - 1) ) )
		return 0;

	LPSHADERFUNCTION pShaderFunction;
	if( !pFunction->uShader || !( pShaderFunction = m_Parent.m_ShaderFunctionPool.Get(pFunction->uShader - 1) ) )
		return 0;

	pShaderFunction->uParameters = uParameters;
	//SHADERFUNCTION ShaderFunction;

	//ShaderFunction.Type        = m_Type;
	//ShaderFunction.uFunction   = uFunction;
	//ShaderFunction.uParameters = uParameters;

	//pFunction->uShader = m_Parent.m_ShaderFunctionPool.Add(ShaderFunction) + 1;

	//m_ShaderParameterPool.Clear();

	//m_pShaderParameter = ALCHEMY_NULL;

	//m_Type = CEffectInterface::PROGRAM;

	return uFunction;
}

//bool CEffectCode::CShaderFunctionDefiner::_IsValuableResultVariable(UINT uResultVariable)
//{
//	if(!uResultVariable)
//	{
//		ALCHEMY_DEBUG_WARNING("CEffectCode::CShaderFunctionDefiner::EndFunction: Shader function can not be created without result variable.");
//
//		return false;
//	}
//
//	LPVARIABLE pVariable;
//	if( !( pVariable = m_Parent.m_VariablePool.Get(uResultVariable - 1) ) || (pVariable->uFunction ? pVariable->uFunction != m_uHandle : !pVariable->uGlobal) )
//	{
//		ALCHEMY_DEBUG_WARNING("CEffectCode::CShaderFunctionDefiner::EndFunction: Error result variable.");
//
//		return false;
//	}
//
//	if(m_Type == CEffectInterface::VERTEX_SHADER)
//	{
//		if(pVariable->Type != VECTOR4_VARIABLE)
//		{
//			PUINT puFirstDeclaration;
//			LPSTRUCTDECLARATION pStructDeclaration;
//			if( pVariable->Type != STRUCT_VARIABLE || 
//				!pVariable->uDeclaration || 
//				!(puFirstDeclaration = m_Parent.m_StructPool.Get(pVariable->uDeclaration - 1) ) ||
//				!(*puFirstDeclaration) ||
//				!( pStructDeclaration = m_Parent.m_StructDeclarationPool.Get(*puFirstDeclaration - 1) ) ||
//				pStructDeclaration->Type != VECTOR4_VARIABLE)
//			{
//				ALCHEMY_DEBUG_WARNING("CEffectCode::CShaderFunctionDefiner::EndFunction: Vertex shader must return position(VECTOR4) value.");
//
//				return false;
//			}
//		}
//	}
//	else if(pVariable->Type != VECTOR4_VARIABLE || pVariable->uDeclaration)
//	{
//		ALCHEMY_DEBUG_WARNING("CEffectCode::CShaderFunctionDefiner::EndFunction: The function is not a shader function.");
//
//		return false;
//	}
//
//	return true;
//}

void CEffectCode::CShaderFunctionDefiner::_ApplyFunction()
{
	CBasicFunctionDefiner::_ApplyFunction();

	LPFUNCTION pFunction;
	if(m_uHandle && ( pFunction = m_Parent.m_FunctionPool.Get(m_uHandle - 1) ) != ALCHEMY_NULL)
	{
		SHADERFUNCTION ShaderFunction;

		ShaderFunction.Type        = m_Type;
		ShaderFunction.uParameters = 0;

		pFunction->uShader  = m_Parent.m_ShaderFunctionPool.Add(ShaderFunction) + 1;
	}
}

CEffectCode::CPassDefiner::CEffectCodeUtils::CEffectCodeUtils(CEffectCode& Parent) : 
m_Parent(Parent)
{

}

CEffectCode::CPassDefiner::CEffectCodeUtils::~CEffectCodeUtils()
{

}

UINT CEffectCode::CPassDefiner::CEffectCodeUtils::GetAddressVariable(UINT uSource, UINT uAddress)
{
	return m_Parent.GetAddressVariable(uSource, uAddress);
}

CEffectCode::CPassDefiner::CPassDefiner(CEffectCode& Parent) :
CDefiner(Parent),
m_VertexShaderFunctionDefiner(Parent, true),
m_FragmentShaderFunctionDefiner(Parent, false),
m_AttributesDefiner(Parent),
m_VaryingDefiner(Parent),
m_puBuffer(ALCHEMY_NULL),
m_uBufferLength(0),
m_pVaryingBuffer(ALCHEMY_NULL),
m_uVaryingBufferLength(0),
m_pVariableBuffer(ALCHEMY_NULL),
m_uVariableBufferLength(0),
m_uAttributeAddress(0),
m_uVaryingAddress(0),
m_uVariableInfoCount(0),
m_uFinalOutputEffectInfoIndex(0),
m_uConstantVaryingCount(0),
m_bIsBlend(false),
m_EffectCodeUtils(Parent)
{
}

CEffectCode::CPassDefiner::~CPassDefiner()
{
	ALCHEMY_DEBUG_DELETE_ARRAY(m_puBuffer);
	ALCHEMY_DEBUG_DELETE_ARRAY(m_pVaryingBuffer);
	ALCHEMY_DEBUG_DELETE_ARRAY(m_pVariableBuffer);
}

bool CEffectCode::CPassDefiner::AddInfo(IEffectInfo& Info, IEffectInfoBlendMethod* pBlendMethod, UINT uInputColorKey, UINT uOutputColorKey)
{
	EFFECTINFO EffectInfo;

	EffectInfo.pEffectInfo  = &Info;
	EffectInfo.pBlendMethod = pBlendMethod;
	EffectInfo.uInputColorKey  = __GetColorKey(uInputColorKey);
	EffectInfo.uSourceColorKey = 0;
	EffectInfo.uOutputColorKey = __IsValuableColorKey(uOutputColorKey) ? uOutputColorKey : 0;

	if(EffectInfo.uOutputColorKey)
		m_EffectInfos.Add(EffectInfo);
	else
		m_uFinalOutputEffectInfoIndex = m_EffectInfos.Add(EffectInfo);

	UINT i, /*j, */uAttributeCount = 0, uVaryingCount = 0, uConstantCount = 0, /*uTemporaryCount = 0, */uAttributeVariableCount = 0, uConstantVariableCount = 0/*, uCurrentCount = 0, uDestinationCount = 0*/;

	const UINT* puValue;

	VARIABLETYPE VariableType;

	LPSHADERVARIABLE pVariable;

	//SHADERPARAMETERKEY Parameter;

	ELEMENTDECLARATION ElementTemp;

	//GLOBALVARIABLEKEY GlobalVariableKey;

	GLOBALVARIABLE GlobalVariable;

	SHADERVARIABLE Variable;

	ATTRIBUTEVARIABLEKEY VariableKey;

	const ELEMENTDECLARATION* pElement, * pAttributes = Info.GetAttributes(uAttributeCount), * pVarying = Info.GetVarying(uVaryingCount);

	const GLOBALVARIABLE* pConstant, * pConstants = Info.GetConstants(uConstantCount);

	//const TEMPORARYDECLARATION* pTemporary = Info.GetTemporary(uTemporaryCount);

	const ATTRIBUTEVARIABLEDECLARATION* pAttributeVariableDeclaration = Info.GetAttributeVariables(uAttributeVariableCount);

	const CONSTANTVARIABLEDECLARATION* pConstantVariableDeclaration = Info.GetConstantVariables(uConstantVariableCount);

	for(i = 0; i < uAttributeCount; ++ i)
	{
		pElement = &pAttributes[i];

		/*Parameter.Type    = pElement->Type;
		Parameter.Usage   = pElement->Usage;
		uDestinationCount = pElement->uIndex + 1;

		puValue = m_AttributesDefiner.GetShaderParameterKeyCount(Parameter);
		uCurrentCount = 0;
		if(!puValue || !(uCurrentCount = *puValue) || uCurrentCount < uDestinationCount)
		{
			for(j = uCurrentCount; j < uDestinationCount; ++ j)
			{
				ElementTemp.Type   = Parameter.Type;
				ElementTemp.Usage  = Parameter.Usage;
				ElementTemp.uIndex = j;

				m_AttributeAddressMap.Insert(ElementTemp, m_uAttributeAddress ++);

				m_AttributesDefiner.AppendParameter(Parameter.Type, Parameter.Usage);
			}
		}*/

		puValue = m_AttributeAddressMap.Get(*pElement);
		if(!puValue)
		{
			m_AttributeAddressMap.Insert(*pElement, m_uAttributeAddress ++);

			m_AttributesDefiner.AppendParameter(pElement->Type, pElement->Usage, pElement->uIndex);
		}
	}

	for(i = 0; i < uVaryingCount; ++ i)
	{
		pElement = &pVarying[i];

		/*Parameter.Type    = pElement->Type;
		Parameter.Usage   = pElement->Usage;
		uDestinationCount = pElement->uIndex + 1;

		puValue = m_VaryingDefiner.GetShaderParameterKeyCount(Parameter);
		uCurrentCount = 0;
		if(!puValue || !(uCurrentCount = *puValue) || uCurrentCount < uDestinationCount)
		{
			for(j = uCurrentCount; j < uDestinationCount; ++ j)
			{
				ElementTemp.Type   = Parameter.Type;
				ElementTemp.Usage  = Parameter.Usage;
				ElementTemp.uIndex = j;

				m_VaryingAddressMap.Insert(ElementTemp, m_uVaryingAddress ++);

				m_VaryingDefiner.AppendParameter(Parameter.Type, Parameter.Usage);
			}
		}*/
		puValue = m_VaryingAddressMap.Get(*pElement);
		if(!puValue)
		{
			m_VaryingAddressMap.Insert(*pElement, m_uVaryingAddress ++);

			m_VaryingDefiner.AppendParameter(pElement->Type, pElement->Usage, pElement->uIndex);
		}
	}

	for(i = 0; i < uConstantCount; ++ i)
	{
		pConstant = &pConstants[i];

		//GlobalVariableKey.ShaderType = pConstant->ShaderType;
		//GlobalVariableKey.GlobalType = pConstant->GlobalType;
		//GlobalVariableKey.uDetail    = pConstant->uDetail;
		//GlobalVariableKey.uCount     = pConstant->uCount;
		//GlobalVariableKey.pValue     = pConstant->pValue;
		//uDestinationCount            = pConstant->uIndex + 1;

		//puValue = m_Parent.m_GlobalVariableCountMap.Get(GlobalVariableKey);
		//uCurrentCount = 0;
		//if(!puValue || !(uCurrentCount = *puValue) || uCurrentCount < uDestinationCount)
		//{
		//	for(j = uCurrentCount; j < uDestinationCount; ++ j)
		//	{
		//		//ConstantTemp.Type    = GlobalVariableKey.Type;
		//		//ConstantTemp.uDetail = GlobalVariableKey.uDetail;
		//		//ConstantTemp.uIndex  = j;

		//		//m_ConstantMap.Insert( ConstantTemp, m_Parent.AddGlobalVariable(GlobalVariableKey) );
		//		m_Parent.AddGlobalVariable(GlobalVariableKey);
		//	}
		//}
		m_Parent.AddGlobalVariable(*pConstant);
	}

	/*m_TemporaryCountMap.Clear();
	for(i = 0; i < uTemporaryCount; ++ i)
	{
		puTemporaryCount = m_TemporaryCountMap.Get(*pTemporary);
		if(!puTemporaryCount)
		{
			m_TemporaryCountMap.Insert(*pTemporary, 1);

			uCurrentIndex = 0;
		}
		else
			uCurrentIndex = (*puTemporaryCount) ++;

		//m_TemporaryMaximumCountMap.Insert(*pTemporary, uCurrentIndex + 1);

		Temporary.ShaderType   = pTemporary->ShaderType;
		Temporary.VariableType = pTemporary->VariableType;
		//Temporary.uDeclaration = pTemporary->uDeclaration;
		Temporary.uIndex       = uCurrentIndex;

		puValue = m_TemporaryHandleMap.Get(Temporary);
		if(!puValue)
		{
			if(Temporary.ShaderType == CEffectInterface::VERTEX_SHADER)
				m_TemporaryHandleMap.Insert( Temporary, m_VertexShaderFunctionDefiner.ApplyVariable(Temporary.VariableType, Temporary.uDeclaration, 1) );
			else if(Temporary.ShaderType == CEffectInterface::FRAGMENT_SHADER)
				m_TemporaryHandleMap.Insert( Temporary, m_FragmentShaderFunctionDefiner.ApplyVariable(Temporary.VariableType, Temporary.uDeclaration, 1) );

			puValue = m_TemporaryHandleMap.Get(Temporary);
		}

		__PushFreeTemporary(*pTemporary, *puValue, false);

		++ pTemporary;
	}*/

	for(i = 0; i < uAttributeVariableCount; ++ i)
	{
		VariableKey.ShaderType   = pAttributeVariableDeclaration->ShaderType;
		VariableKey.ElementType  = pAttributeVariableDeclaration->ElementType;
		VariableKey.ElementUsage = pAttributeVariableDeclaration->ElementUsage;
		VariableKey.uIndex       = pAttributeVariableDeclaration->uIndex;

		pVariable = m_AttributeVariableMap.Get(VariableKey);

		if(!pVariable && VariableKey.ShaderType == IEffect::FRAGMENT_SHADER)
		{
			VariableKey.ShaderType = IEffect::VERTEX_SHADER;

			pVariable = m_AttributeVariableMap.Get(VariableKey);

			if(pVariable)
			{
				if(pAttributeVariableDeclaration->bIsReadOnly)
				{
					ElementTemp.Type   = VariableKey.ElementType;
					ElementTemp.Usage  = VariableKey.ElementUsage;
					ElementTemp.uIndex = VariableKey.uIndex;

					/*puValue = m_AttributeVaryingMap.Get(ElementTemp);

					if(!puValue)
					{
						m_AttributeVaryingMap.Insert(ElementTemp, 0);

						Parameter.Type    = VariableKey.ElementType;
						Parameter.Usage   = VariableKey.ElementUsage;
						uDestinationCount = VariableKey.uIndex + 1;
						puValue = m_VaryingDefiner.GetShaderParameterKeyCount(Parameter);
						uCurrentCount = 0;
						if(!puValue || !(uCurrentCount = *puValue) || uCurrentCount < uDestinationCount)
						{
							for(j = uCurrentCount; j < uDestinationCount; ++ j)
							{
								ElementTemp.Type   = Parameter.Type;
								ElementTemp.Usage  = Parameter.Usage;
								ElementTemp.uIndex = j;

								m_VaryingAddressMap.Insert(ElementTemp, m_uVaryingAddress ++);

								m_VaryingDefiner.AppendParameter(Parameter.Type, Parameter.Usage);
							}
						}
					}*/
					puValue = m_AttributeVaryingMap.Get(ElementTemp);
					if(!puValue)
					{
						m_AttributeVaryingMap.Insert(ElementTemp, 0);

						puValue = m_VaryingAddressMap.Get(ElementTemp);
						if(!puValue)
						{
							m_VaryingAddressMap.Insert(ElementTemp, m_uVaryingAddress ++);

							m_VaryingDefiner.AppendParameter(ElementTemp.Type, ElementTemp.Usage, ElementTemp.uIndex);
						}
					}
				}
			}
			else
				VariableKey.ShaderType = IEffect::FRAGMENT_SHADER;
		}

		if(pVariable)
		{
			if(pAttributeVariableDeclaration->bIsReadOnly)
			{
				if(pVariable->bIsUsed && !pVariable->uSource)
				{
					if(VariableKey.ShaderType == IEffect::VERTEX_SHADER)
					{
						/*Parameter.Type    = VariableKey.ElementType;
						Parameter.Usage   = VariableKey.ElementUsage;
						uDestinationCount = VariableKey.uIndex + 1;

						puValue = m_AttributesDefiner.GetShaderParameterKeyCount(Parameter);
						uCurrentCount = 0;
						if(!puValue || !(uCurrentCount = *puValue) || uCurrentCount < uDestinationCount)
						{
							for(j = uCurrentCount; j < uDestinationCount; ++ j)
							{
								ElementTemp.Type   = Parameter.Type;
								ElementTemp.Usage  = Parameter.Usage;
								ElementTemp.uIndex = j;

								m_AttributeAddressMap.Insert(ElementTemp, m_uAttributeAddress ++);

								m_AttributesDefiner.AppendParameter(Parameter.Type, Parameter.Usage);
							}
						}*/
						ElementTemp.Type   = VariableKey.ElementType;
						ElementTemp.Usage  = VariableKey.ElementUsage;
						ElementTemp.uIndex = VariableKey.uIndex;

						puValue = m_AttributeAddressMap.Get(ElementTemp);
						if(!puValue)
						{
							m_AttributeAddressMap.Insert(ElementTemp, m_uAttributeAddress ++);

							m_AttributesDefiner.AppendParameter(ElementTemp.Type, ElementTemp.Usage, ElementTemp.uIndex);
						}
					}
					else if(VariableKey.ShaderType == IEffect::FRAGMENT_SHADER)
					{
						/*Parameter.Type    = VariableKey.ElementType;
						Parameter.Usage   = VariableKey.ElementUsage;
						uDestinationCount = VariableKey.uIndex + 1;

						puValue = m_VaryingDefiner.GetShaderParameterKeyCount(Parameter);
						uCurrentCount = 0;
						if(!puValue || !(uCurrentCount = *puValue) || uCurrentCount < uDestinationCount)
						{
							for(j = uCurrentCount; j < uDestinationCount; ++ j)
							{
								ElementTemp.Type   = Parameter.Type;
								ElementTemp.Usage  = Parameter.Usage;
								ElementTemp.uIndex = j;

								m_VaryingAddressMap.Insert(ElementTemp, m_uVaryingAddress ++);

								m_VaryingDefiner.AppendParameter(Parameter.Type, Parameter.Usage);
							}
						}*/
						ElementTemp.Type   = VariableKey.ElementType;
						ElementTemp.Usage  = VariableKey.ElementUsage;
						ElementTemp.uIndex = VariableKey.uIndex;

						puValue = m_VaryingAddressMap.Get(ElementTemp);
						if(!puValue)
						{
							m_VaryingAddressMap.Insert(ElementTemp, m_uVaryingAddress ++);

							m_VaryingDefiner.AppendParameter(ElementTemp.Type, ElementTemp.Usage, ElementTemp.uIndex);
						}

						/*puValue = m_AttributesDefiner.GetShaderParameterKeyCount(Parameter);
						uCurrentCount = 0;
						if(!puValue || !(uCurrentCount = *puValue) || uCurrentCount < uDestinationCount)
						{
							for(j = uCurrentCount; j < uDestinationCount; ++ j)
							{
								ElementTemp.Type   = Parameter.Type;
								ElementTemp.Usage  = Parameter.Usage;
								ElementTemp.uIndex = j;

								m_AttributeAddressMap.Insert(ElementTemp, m_uAttributeAddress ++);

								m_AttributesDefiner.AppendParameter(Parameter.Type, Parameter.Usage);
							}
						}*/
						puValue = m_AttributeAddressMap.Get(ElementTemp);
						if(!puValue)
						{
							m_AttributeAddressMap.Insert(ElementTemp, m_uAttributeAddress ++);

							m_AttributesDefiner.AppendParameter(ElementTemp.Type, ElementTemp.Usage, ElementTemp.uIndex);
						}
					}

					pVariable->bIsUsed = false;
				}

				if(!pVariable->uHandle)
					pVariable->uHandle = 1;

				m_AttributeVariableMaxInfoMap.Insert(VariableKey, m_uVariableInfoCount);

				if(VariableKey.ShaderType == IEffect::VERTEX_SHADER && VariableKey.ShaderType != pAttributeVariableDeclaration->ShaderType)
				{
					VariableKey.ShaderType = IEffect::FRAGMENT_SHADER;
					m_AttributeVariableMaxInfoMap.Insert(VariableKey, m_uVariableInfoCount);
				}
			}
			else if(pAttributeVariableDeclaration->bIsForce && !pVariable->uHandle && !pVariable->uSource)
				pVariable->bIsUsed = true;
		}
		else if(!pAttributeVariableDeclaration->bIsReadOnly)
		{
			Variable.uHandle = 0;
			Variable.uSource = 0;
			Variable.bIsUsed = pAttributeVariableDeclaration->bIsForce;

			m_AttributeVariableMap.Insert(VariableKey, Variable);
		}
		else if(pAttributeVariableDeclaration->bIsForce)
		{
			if(VariableKey.ShaderType == IEffect::VERTEX_SHADER)
			{
				/*Parameter.Type    = VariableKey.ElementType;
				Parameter.Usage   = VariableKey.ElementUsage;
				uDestinationCount = VariableKey.uIndex + 1;

				puValue = m_AttributesDefiner.GetShaderParameterKeyCount(Parameter);
				uCurrentCount = 0;
				if(!puValue || !(uCurrentCount = *puValue) || uCurrentCount < uDestinationCount)
				{
					for(j = uCurrentCount; j < uDestinationCount; ++ j)
					{
						ElementTemp.Type   = Parameter.Type;
						ElementTemp.Usage  = Parameter.Usage;
						ElementTemp.uIndex = j;

						m_AttributeAddressMap.Insert(ElementTemp, m_uAttributeAddress ++);

						m_AttributesDefiner.AppendParameter(Parameter.Type, Parameter.Usage);
					}
				}*/
				ElementTemp.Type   = VariableKey.ElementType;
				ElementTemp.Usage  = VariableKey.ElementUsage;
				ElementTemp.uIndex = VariableKey.uIndex;

				puValue = m_AttributeAddressMap.Get(ElementTemp);
				if(!puValue)
				{
					m_AttributeAddressMap.Insert(ElementTemp, m_uAttributeAddress ++);

					m_AttributesDefiner.AppendParameter(ElementTemp.Type, ElementTemp.Usage, ElementTemp.uIndex);
				}
			}
			else if(VariableKey.ShaderType == IEffect::FRAGMENT_SHADER)
			{
				/*Parameter.Type    = VariableKey.ElementType;
				Parameter.Usage   = VariableKey.ElementUsage;
				uDestinationCount = VariableKey.uIndex + 1;

				puValue = m_VaryingDefiner.GetShaderParameterKeyCount(Parameter);
				uCurrentCount = 0;
				if(!puValue || !(uCurrentCount = *puValue) || uCurrentCount < uDestinationCount)
				{
					for(j = uCurrentCount; j < uDestinationCount; ++ j)
					{
						ElementTemp.Type   = Parameter.Type;
						ElementTemp.Usage  = Parameter.Usage;
						ElementTemp.uIndex = j;

						m_VaryingAddressMap.Insert(ElementTemp, m_uVaryingAddress ++);

						m_VaryingDefiner.AppendParameter(Parameter.Type, Parameter.Usage);
					}
				}*/
				ElementTemp.Type   = VariableKey.ElementType;
				ElementTemp.Usage  = VariableKey.ElementUsage;
				ElementTemp.uIndex = VariableKey.uIndex;

				puValue = m_VaryingAddressMap.Get(ElementTemp);
				if(!puValue)
				{
					m_VaryingAddressMap.Insert(ElementTemp, m_uVaryingAddress ++);

					m_VaryingDefiner.AppendParameter(ElementTemp.Type, ElementTemp.Usage, ElementTemp.uIndex);
				}

				/*puValue = m_AttributesDefiner.GetShaderParameterKeyCount(Parameter);
				uCurrentCount = 0;
				if(!puValue || !(uCurrentCount = *puValue) || uCurrentCount < uDestinationCount)
				{
					for(j = uCurrentCount; j < uDestinationCount; ++ j)
					{
						ElementTemp.Type   = Parameter.Type;
						ElementTemp.Usage  = Parameter.Usage;
						ElementTemp.uIndex = j;

						m_AttributeAddressMap.Insert(ElementTemp, m_uAttributeAddress ++);

						m_AttributesDefiner.AppendParameter(Parameter.Type, Parameter.Usage);
					}
				}*/
				puValue = m_AttributeAddressMap.Get(ElementTemp);
				if(!puValue)
				{
					m_AttributeAddressMap.Insert(ElementTemp, m_uAttributeAddress ++);

					m_AttributesDefiner.AppendParameter(ElementTemp.Type, ElementTemp.Usage, ElementTemp.uIndex);
				}
			}
		}

		++ pAttributeVariableDeclaration;
	}

	for(i = 0; i < uConstantVariableCount; ++ i)
	{
		GlobalVariable.ShaderType = pConstantVariableDeclaration->ShaderType;
		GlobalVariable.GlobalType = pConstantVariableDeclaration->GlobalType;
		GlobalVariable.uDetail    = pConstantVariableDeclaration->uDetail;
		GlobalVariable.uIndex     = pConstantVariableDeclaration->uIndex;
		GlobalVariable.uCount     = pConstantVariableDeclaration->uCount;
		GlobalVariable.pValue     = pConstantVariableDeclaration->pValue;

		pVariable = m_ConstantVariableMap.Get(GlobalVariable);

		if(!pVariable && GlobalVariable.ShaderType == IEffect::FRAGMENT_SHADER)
		{
			VariableType = CEffectCode::GetGlobalVariableType(GlobalVariable.GlobalType, GlobalVariable.uDetail);
			if( CEffectCode::IsElementVariable(VariableType) )
			{
				GlobalVariable.ShaderType = IEffect::VERTEX_SHADER;

				pVariable = m_ConstantVariableMap.Get(GlobalVariable);

				if(pVariable)
				{
					if(pConstantVariableDeclaration->bIsReadOnly)
					{
						pElement = m_ConstantVaryingMap.Get(GlobalVariable);

						if(!pElement)
						{
							/*Parameter.Type    = static_cast<CVertexDeclaration::ELEMENTTYPE>(VariableType);
							Parameter.Usage   = CVertexDeclaration::UNKNOWN;

							puValue = m_VaryingDefiner.GetShaderParameterKeyCount(Parameter);

							ElementTemp.Type   = Parameter.Type;
							ElementTemp.Usage  = Parameter.Usage;
							ElementTemp.uIndex = puValue ? *puValue : 0;

							m_VaryingAddressMap.Insert(ElementTemp, m_uVaryingAddress ++);

							m_VaryingDefiner.AppendParameter(Parameter.Type, Parameter.Usage);

							m_ConstantVaryingMap.Insert(GlobalVariable, ElementTemp);*/
							ElementTemp.Type   = static_cast<CVertexDeclaration::ELEMENTTYPE>(VariableType);
							ElementTemp.Usage  = CVertexDeclaration::UNKNOWN;
							ElementTemp.uIndex = m_uConstantVaryingCount ++;

							m_VaryingAddressMap.Insert(ElementTemp, m_uVaryingAddress ++);

							m_VaryingDefiner.AppendParameter(ElementTemp.Type, ElementTemp.Usage, ElementTemp.uIndex);

							m_ConstantVaryingMap.Insert(GlobalVariable, ElementTemp);
						}
					}
				}
				else
					GlobalVariable.ShaderType = IEffect::FRAGMENT_SHADER;
			}
		}

		if(pVariable)
			///Read after write
		{
			if(pConstantVariableDeclaration->bIsReadOnly)
			{
				if(pVariable->bIsUsed && !pVariable->uSource)
					///Force to write
				{
					/*GlobalVariableKey.ShaderType = GlobalVariable.ShaderType;
					GlobalVariableKey.GlobalType = GlobalVariable.GlobalType;
					GlobalVariableKey.uDetail    = GlobalVariable.uDetail;
					GlobalVariableKey.uCount     = GlobalVariable.uCount;
					GlobalVariableKey.pValue     = GlobalVariable.pValue;
					uDestinationCount            = GlobalVariable.uIndex + 1;

					puValue = m_Parent.m_GlobalVariableCountMap.Get(GlobalVariableKey);
					uCurrentCount = 0;
					if(!puValue || !(uCurrentCount = *puValue) || uCurrentCount < uDestinationCount)
						for(j = uCurrentCount; j < uDestinationCount; ++ j)
							m_Parent.AddGlobalVariable(GlobalVariableKey);*/

					pVariable->uSource = m_Parent.AddGlobalVariable(GlobalVariable);//m_Parent.m_GlobalVariableHandleMap[GlobalVariable];

					pVariable->bIsUsed = false;
				}

				if(!pVariable->uHandle)
					pVariable->uHandle = 1;

				m_ConstantVariableMaxInfoMap.Insert(GlobalVariable, m_uVariableInfoCount);

				if(GlobalVariable.ShaderType == IEffect::VERTEX_SHADER && GlobalVariable.ShaderType != pConstantVariableDeclaration->ShaderType)
				{
					GlobalVariable.ShaderType = IEffect::FRAGMENT_SHADER;
					m_ConstantVariableMaxInfoMap.Insert(GlobalVariable, m_uVariableInfoCount);
				}
			}
			else if(pConstantVariableDeclaration->bIsForce && !pVariable->uHandle && !pVariable->uSource)
				pVariable->bIsUsed = true;
		}
		else if(!pConstantVariableDeclaration->bIsReadOnly)
		{
			Variable.uHandle = 0;
			Variable.uSource = 0;
			Variable.bIsUsed = pConstantVariableDeclaration->bIsForce;

			m_ConstantVariableMap.Insert(GlobalVariable, Variable);
		}

		++ pConstantVariableDeclaration;
	}

	++ m_uVariableInfoCount;
	if(pBlendMethod)
	{
		/*m_TemporaryCountMap.Clear();

		const VARIABLETYPE* pTemporaryType = pBlendMethod->GetTemporary(uTemporaryCount);

		for(i = 0; i < uTemporaryCount; ++ i)
		{
			TemporaryDeclaration.ShaderType   = CEffectInterface::FRAGMENT_SHADER;
			TemporaryDeclaration.VariableType = *pTemporaryType;

			puTemporaryCount = m_TemporaryCountMap.Get(TemporaryDeclaration);
			if(!puTemporaryCount)
			{
				m_TemporaryCountMap.Insert(TemporaryDeclaration, 1);

				uCurrentIndex = 0;
			}
			else
				uCurrentIndex = (*puTemporaryCount) ++;

			Temporary.ShaderType   = TemporaryDeclaration.ShaderType;
			Temporary.VariableType = TemporaryDeclaration.VariableType;
			Temporary.uIndex       = uCurrentIndex;

			puValue = m_TemporaryHandleMap.Get(Temporary);
			if(!puValue)
			{
				m_TemporaryHandleMap.Insert( Temporary, m_FragmentShaderFunctionDefiner.ApplyVariable(Temporary.VariableType, Temporary.uDeclaration, 1) );
				puValue = m_TemporaryHandleMap.Get(Temporary);
			}

			__PushFreeTemporary(TemporaryDeclaration, *puValue, false);

			++ pTemporaryType;
		}*/

		const ATTRIBUTE* pBlendAttribute = pBlendMethod->GetAttributes(uAttributeCount);

		for(i = 0; i < uAttributeCount; ++ i)
		{
			if(pBlendAttribute->bIsForce)
			{
				/*Parameter.Type    = pBlendAttribute->Type;
				Parameter.Usage   = pBlendAttribute->Usage;
				uDestinationCount = pBlendAttribute->uIndex + 1;

				puValue = m_AttributesDefiner.GetShaderParameterKeyCount(Parameter);
				uCurrentCount = 0;
				if(!puValue || !(uCurrentCount = *puValue) || uCurrentCount < uDestinationCount)
				{
					for(j = uCurrentCount; j < uDestinationCount; ++ j)
					{
						ElementTemp.Type   = Parameter.Type;
						ElementTemp.Usage  = Parameter.Usage;
						ElementTemp.uIndex = j;

						m_AttributeAddressMap.Insert(ElementTemp, m_uAttributeAddress ++);

						m_AttributesDefiner.AppendParameter(Parameter.Type, Parameter.Usage);
					}
				}*/
				ElementTemp.Type   = pBlendAttribute->Type;
				ElementTemp.Usage  = pBlendAttribute->Usage;
				ElementTemp.uIndex = pBlendAttribute->uIndex;

				puValue = m_AttributeAddressMap.Get(ElementTemp);
				if(!puValue)
				{
					m_AttributeAddressMap.Insert(ElementTemp, m_uAttributeAddress ++);

					m_AttributesDefiner.AppendParameter(ElementTemp.Type, ElementTemp.Usage, ElementTemp.uIndex);
				}
			}
			else
			{
				VariableKey.ShaderType   = IEffect::FRAGMENT_SHADER;
				VariableKey.ElementType  = pBlendAttribute->Type;
				VariableKey.ElementUsage = pBlendAttribute->Usage;
				VariableKey.uIndex       = pBlendAttribute->uIndex;

				pVariable = m_AttributeVariableMap.Get(VariableKey);
				if(!pVariable)
				{
					VariableKey.ShaderType = IEffect::VERTEX_SHADER;
					pVariable = m_AttributeVariableMap.Get(VariableKey);
					if(!pVariable)
						VariableKey.ShaderType = IEffect::FRAGMENT_SHADER;
				}

				ElementTemp.Type   = VariableKey.ElementType;
				ElementTemp.Usage  = VariableKey.ElementUsage;
				ElementTemp.uIndex = VariableKey.uIndex;

				if(pVariable)
				{
					/*Parameter.Type    = VariableKey.ElementType;
					Parameter.Usage   = VariableKey.ElementUsage;
					uDestinationCount = VariableKey.uIndex + 1;*/

					if(VariableKey.ShaderType == IEffect::VERTEX_SHADER)
					{
						/*ElementTemp.Type   = VariableKey.ElementType;
						ElementTemp.Usage  = VariableKey.ElementUsage;
						ElementTemp.uIndex = VariableKey.uIndex;*/

						puValue = m_AttributeVaryingMap.Get(ElementTemp);

						if(!puValue)
						{
							m_AttributeVaryingMap.Insert(ElementTemp, 0);

							/*puValue = m_VaryingDefiner.GetShaderParameterKeyCount(Parameter);
							uCurrentCount = 0;
							if(!puValue || !(uCurrentCount = *puValue) || uCurrentCount < uDestinationCount)
							{
								for(j = uCurrentCount; j < uDestinationCount; ++ j)
								{
									ElementTemp.Type   = Parameter.Type;
									ElementTemp.Usage  = Parameter.Usage;
									ElementTemp.uIndex = j;

									m_VaryingAddressMap.Insert(ElementTemp, m_uVaryingAddress ++);

									m_VaryingDefiner.AppendParameter(Parameter.Type, Parameter.Usage);
								}
							}*/
							puValue = m_VaryingAddressMap.Get(ElementTemp);
							if(!puValue)
							{
								m_VaryingAddressMap.Insert(ElementTemp, m_uVaryingAddress ++);

								m_VaryingDefiner.AppendParameter(ElementTemp.Type, ElementTemp.Usage, ElementTemp.uIndex);
							}
						}
					}

					if(pVariable->bIsUsed && !pVariable->uSource)
					{
						/*puValue = m_AttributesDefiner.GetShaderParameterKeyCount(Parameter);
						uCurrentCount = 0;
						if(!puValue || !(uCurrentCount = *puValue) || uCurrentCount < uDestinationCount)
						{
							for(j = uCurrentCount; j < uDestinationCount; ++ j)
							{
								ElementTemp.Type   = Parameter.Type;
								ElementTemp.Usage  = Parameter.Usage;
								ElementTemp.uIndex = j;

								m_AttributeAddressMap.Insert(ElementTemp, m_uAttributeAddress ++);

								m_AttributesDefiner.AppendParameter(Parameter.Type, Parameter.Usage);
							}
						}*/
						puValue = m_AttributeAddressMap.Get(ElementTemp);
						if(!puValue)
						{
							m_AttributeAddressMap.Insert(ElementTemp, m_uAttributeAddress ++);

							m_AttributesDefiner.AppendParameter(ElementTemp.Type, ElementTemp.Usage, ElementTemp.uIndex);
						}

						pVariable->bIsUsed = false;
					}

					if(!pVariable->uHandle)
						pVariable->uHandle = 1;

					m_AttributeVariableMaxInfoMap.Insert(VariableKey, m_uVariableInfoCount);

					if(VariableKey.ShaderType == IEffect::VERTEX_SHADER)
					{
						VariableKey.ShaderType = IEffect::FRAGMENT_SHADER;
						m_AttributeVariableMaxInfoMap.Insert(VariableKey, m_uVariableInfoCount);
					}
				}
				else
				{
					/*Parameter.Type    = VariableKey.ElementType;
					Parameter.Usage   = VariableKey.ElementUsage;
					uDestinationCount = VariableKey.uIndex + 1;

					puValue = m_VaryingDefiner.GetShaderParameterKeyCount(Parameter);
					uCurrentCount = 0;
					if(!puValue || !(uCurrentCount = *puValue) || uCurrentCount < uDestinationCount)
					{
						for(j = uCurrentCount; j < uDestinationCount; ++ j)
						{
							ElementTemp.Type   = Parameter.Type;
							ElementTemp.Usage  = Parameter.Usage;
							ElementTemp.uIndex = j;

							m_VaryingAddressMap.Insert(ElementTemp, m_uVaryingAddress ++);

							m_VaryingDefiner.AppendParameter(Parameter.Type, Parameter.Usage);
						}
					}*/
					puValue = m_VaryingAddressMap.Get(ElementTemp);
					if(!puValue)
					{
						m_VaryingAddressMap.Insert(ElementTemp, m_uVaryingAddress ++);

						m_VaryingDefiner.AppendParameter(ElementTemp.Type, ElementTemp.Usage, ElementTemp.uIndex);
					}

					/*puValue = m_AttributesDefiner.GetShaderParameterKeyCount(Parameter);
					uCurrentCount = 0;
					if(!puValue || !(uCurrentCount = *puValue) || uCurrentCount < uDestinationCount)
					{
						for(j = uCurrentCount; j < uDestinationCount; ++ j)
						{
							ElementTemp.Type   = Parameter.Type;
							ElementTemp.Usage  = Parameter.Usage;
							ElementTemp.uIndex = j;

							m_AttributeAddressMap.Insert(ElementTemp, m_uAttributeAddress ++);

							m_AttributesDefiner.AppendParameter(Parameter.Type, Parameter.Usage);
						}
					}*/
					puValue = m_AttributeAddressMap.Get(ElementTemp);
					if(!puValue)
					{
						m_AttributeAddressMap.Insert(ElementTemp, m_uAttributeAddress ++);

						m_AttributesDefiner.AppendParameter(ElementTemp.Type, ElementTemp.Usage, ElementTemp.uIndex);
					}
				}
			}

			++ pBlendAttribute;
		}

		const CONSTANT* pBlendConstant = pBlendMethod->GetConstants(uConstantCount);
		for(i = 0; i < uConstantCount; ++ i)
		{
			GlobalVariable.ShaderType = IEffect::FRAGMENT_SHADER;
			GlobalVariable.GlobalType = pBlendConstant->Type;
			GlobalVariable.uDetail    = pBlendConstant->uDetail;
			GlobalVariable.uIndex     = pBlendConstant->uIndex;
			GlobalVariable.uCount     = pBlendConstant->uCount;
			GlobalVariable.pValue     = pBlendConstant->pValue;

			if(pBlendConstant->bIsForce)
			{
				/*GlobalVariableKey.ShaderType = IEffect::FRAGMENT_SHADER;
				GlobalVariableKey.GlobalType = pBlendConstant->Type;
				GlobalVariableKey.uDetail    = pBlendConstant->uDetail;
				GlobalVariableKey.uCount     = pBlendConstant->uCount;
				GlobalVariableKey.pValue     = pBlendConstant->pValue;
				uDestinationCount            = pBlendConstant->uIndex + 1;

				puValue = m_Parent.m_GlobalVariableCountMap.Get(GlobalVariableKey);
				uCurrentCount = 0;
				if(!puValue || !(uCurrentCount = *puValue) || uCurrentCount < uDestinationCount)
				{
					for(j = uCurrentCount; j < uDestinationCount; ++ j)
						m_Parent.AddGlobalVariable(GlobalVariableKey);
				}*/
				m_Parent.AddGlobalVariable(GlobalVariable);
			}
			else
			{
				/*GlobalVariable.ShaderType = IEffect::FRAGMENT_SHADER;
				GlobalVariable.GlobalType = pBlendConstant->Type;
				GlobalVariable.uDetail    = pBlendConstant->uDetail;
				GlobalVariable.uIndex     = pBlendConstant->uIndex;
				GlobalVariable.uCount     = pBlendConstant->uCount;
				GlobalVariable.pValue     = pBlendConstant->pValue;*/

				pVariable = m_ConstantVariableMap.Get(GlobalVariable);

				if(!pVariable)
				{
					VariableType = CEffectCode::GetGlobalVariableType(GlobalVariable.GlobalType, GlobalVariable.uDetail);
					if( CEffectCode::IsElementVariable(VariableType) )
					{
						GlobalVariable.ShaderType = IEffect::VERTEX_SHADER;

						pVariable = m_ConstantVariableMap.Get(GlobalVariable);

						if(pVariable)
						{
							pElement = m_ConstantVaryingMap.Get(GlobalVariable);

							if(!pElement)
							{
								/*Parameter.Type    = static_cast<CVertexDeclaration::ELEMENTTYPE>(VariableType);
								Parameter.Usage   = CVertexDeclaration::UNKNOWN;

								puValue = m_VaryingDefiner.GetShaderParameterKeyCount(Parameter);

								ElementTemp.Type   = Parameter.Type;
								ElementTemp.Usage  = Parameter.Usage;
								ElementTemp.uIndex = puValue ? *puValue : 0;

								m_VaryingAddressMap.Insert(ElementTemp, m_uVaryingAddress ++);

								m_VaryingDefiner.AppendParameter(Parameter.Type, Parameter.Usage);

								m_ConstantVaryingMap.Insert(GlobalVariable, ElementTemp);*/
								ElementTemp.Type   = static_cast<CVertexDeclaration::ELEMENTTYPE>(VariableType);
								ElementTemp.Usage  = CVertexDeclaration::UNKNOWN;
								ElementTemp.uIndex = m_uConstantVaryingCount ++;

								m_VaryingAddressMap.Insert(ElementTemp, m_uVaryingAddress ++);

								m_VaryingDefiner.AppendParameter(ElementTemp.Type, ElementTemp.Usage, ElementTemp.uIndex);

								m_ConstantVaryingMap.Insert(GlobalVariable, ElementTemp);
							}
						}
						else
							GlobalVariable.ShaderType = IEffect::FRAGMENT_SHADER;
					}
				}

				if(pVariable)
				{
					if(pVariable->bIsUsed && !pVariable->uSource)
					{
						/*GlobalVariableKey.ShaderType = GlobalVariable.ShaderType;
						GlobalVariableKey.GlobalType = GlobalVariable.GlobalType;
						GlobalVariableKey.uDetail    = GlobalVariable.uDetail;
						GlobalVariableKey.uCount     = GlobalVariable.uCount;
						GlobalVariableKey.pValue     = GlobalVariable.pValue;
						uDestinationCount            = GlobalVariable.uIndex + 1;

						puValue = m_Parent.m_GlobalVariableCountMap.Get(GlobalVariableKey);
						uCurrentCount = 0;
						if(!puValue || !(uCurrentCount = *puValue) || uCurrentCount < uDestinationCount)
							for(j = uCurrentCount; j < uDestinationCount; ++ j)
								m_Parent.AddGlobalVariable(GlobalVariableKey);*/

						pVariable->uSource = m_Parent.AddGlobalVariable(GlobalVariable);/*m_Parent.m_GlobalVariableHandleMap[GlobalVariable];*/

						pVariable->bIsUsed = false;
					}

					if(!pVariable->uHandle)
						pVariable->uHandle = 1;

					m_ConstantVariableMaxInfoMap.Insert(GlobalVariable, m_uVariableInfoCount);

					if(GlobalVariable.ShaderType == IEffect::VERTEX_SHADER)
					{
						GlobalVariable.ShaderType = IEffect::FRAGMENT_SHADER;
						m_ConstantVariableMaxInfoMap.Insert(GlobalVariable, m_uVariableInfoCount);
					}
				}
			}

			++ pBlendConstant;
		}

		++ m_uVariableInfoCount;
	}

	m_bIsBlend = m_bIsBlend || (m_EffectInfos.GetLength() > 1 && (pBlendMethod != ALCHEMY_NULL) && !EffectInfo.uOutputColorKey);

	return true;
}

bool CEffectCode::CPassDefiner::AddInfo(UINT uInputColorKey, IEffectInfoBlendMethod* pBlendMethod, UINT uSourceColorKey, UINT uOutputColorKey)
{
	uInputColorKey = __GetColorKey(uInputColorKey);
	if(!uInputColorKey)
	{
		ALCHEMY_DEBUG_WARNING("CEffectCode::CPassDefiner::AddInfo: Error InputColor key.");

		return false;
	}

	EFFECTINFO EffectInfo;

	EffectInfo.pEffectInfo  = ALCHEMY_NULL;
	EffectInfo.pBlendMethod = pBlendMethod;
	EffectInfo.uInputColorKey  = uInputColorKey;
	EffectInfo.uSourceColorKey = __GetColorKey(uSourceColorKey);
	EffectInfo.uOutputColorKey = __IsValuableColorKey(uOutputColorKey) ? uOutputColorKey : 0;

	if(EffectInfo.uOutputColorKey)
		m_EffectInfos.Add(EffectInfo);
	else
		m_uFinalOutputEffectInfoIndex = m_EffectInfos.Add(EffectInfo);

	if(pBlendMethod)
	{
		UINT i, /*j, */uAttributeCount = 0, /*uVaryingCount = 0, */uConstantCount = 0/*, uCurrentCount = 0, uDestinationCount = 0*/;

		const ATTRIBUTE* pBlendAttribute = pBlendMethod->GetAttributes(uAttributeCount);

		const ELEMENTDECLARATION* pElement;

		const UINT* puValue;

		LPSHADERVARIABLE pVariable;

		//SHADERPARAMETERKEY Parameter;

		ELEMENTDECLARATION ElementTemp;

		//GLOBALVARIABLEKEY GlobalVariableKey;

		GLOBALVARIABLE GlobalVariable;

		ATTRIBUTEVARIABLEKEY VariableKey;

		VARIABLETYPE VariableType;

		for(i = 0; i < uAttributeCount; ++ i)
		{
			ElementTemp.Type   = pBlendAttribute->Type;
			ElementTemp.Usage  = pBlendAttribute->Usage;
			ElementTemp.uIndex = pBlendAttribute->uIndex;

			if(pBlendAttribute->bIsForce)
			{
				/*Parameter.Type    = pBlendAttribute->Type;
				Parameter.Usage   = pBlendAttribute->Usage;
				uDestinationCount = pBlendAttribute->uIndex + 1;

				puValue = m_AttributesDefiner.GetShaderParameterKeyCount(Parameter);
				uCurrentCount = 0;
				if(!puValue || !(uCurrentCount = *puValue) || uCurrentCount < uDestinationCount)
				{
					for(j = uCurrentCount; j < uDestinationCount; ++ j)
					{
						ElementTemp.Type   = Parameter.Type;
						ElementTemp.Usage  = Parameter.Usage;
						ElementTemp.uIndex = j;

						m_AttributeAddressMap.Insert(ElementTemp, m_uAttributeAddress ++);

						m_AttributesDefiner.AppendParameter(Parameter.Type, Parameter.Usage);
					}
				}*/

				puValue = m_AttributeAddressMap.Get(ElementTemp);
				if(!puValue)
				{
					m_AttributeAddressMap.Insert(ElementTemp, m_uAttributeAddress ++);

					m_AttributesDefiner.AppendParameter(ElementTemp.Type, ElementTemp.Usage, ElementTemp.uIndex);
				}
			}
			else
			{
				VariableKey.ShaderType   = IEffect::FRAGMENT_SHADER;
				VariableKey.ElementType  = ElementTemp.Type;
				VariableKey.ElementUsage = ElementTemp.Usage;
				VariableKey.uIndex       = ElementTemp.uIndex;

				pVariable = m_AttributeVariableMap.Get(VariableKey);
				if(!pVariable)
				{
					VariableKey.ShaderType = IEffect::VERTEX_SHADER;
					pVariable = m_AttributeVariableMap.Get(VariableKey);
					if(!pVariable)
						VariableKey.ShaderType = IEffect::FRAGMENT_SHADER;
				}

				if(pVariable)
				{
					/*Parameter.Type    = VariableKey.ElementType;
					Parameter.Usage   = VariableKey.ElementUsage;
					uDestinationCount = VariableKey.uIndex + 1;*/

					if(VariableKey.ShaderType == IEffect::VERTEX_SHADER)
					{
						/*ElementTemp.Type   = VariableKey.ElementType;
						ElementTemp.Usage  = VariableKey.ElementUsage;
						ElementTemp.uIndex = VariableKey.uIndex;*/

						puValue = m_AttributeVaryingMap.Get(ElementTemp);

						if(!puValue)
						{
							m_AttributeVaryingMap.Insert(ElementTemp, 0);

							/*puValue = m_VaryingDefiner.GetShaderParameterKeyCount(Parameter);
							uCurrentCount = 0;
							if(!puValue || !(uCurrentCount = *puValue) || uCurrentCount < uDestinationCount)
							{
								for(j = uCurrentCount; j < uDestinationCount; ++ j)
								{
									ElementTemp.Type   = Parameter.Type;
									ElementTemp.Usage  = Parameter.Usage;
									ElementTemp.uIndex = j;

									m_VaryingAddressMap.Insert(ElementTemp, m_uVaryingAddress ++);

									m_VaryingDefiner.AppendParameter(Parameter.Type, Parameter.Usage);
								}
							}*/

							puValue = m_VaryingAddressMap.Get(ElementTemp);
							if(!puValue)
							{
								m_VaryingAddressMap.Insert(ElementTemp, m_uVaryingAddress ++);

								m_VaryingDefiner.AppendParameter(ElementTemp.Type, ElementTemp.Usage, ElementTemp.uIndex);
							}
						}
					}

					if(pVariable->bIsUsed && !pVariable->uSource)
					{
						/*puValue = m_AttributesDefiner.GetShaderParameterKeyCount(Parameter);
						uCurrentCount = 0;
						if(!puValue || !(uCurrentCount = *puValue) || uCurrentCount < uDestinationCount)
						{
							for(j = uCurrentCount; j < uDestinationCount; ++ j)
							{
								ElementTemp.Type   = Parameter.Type;
								ElementTemp.Usage  = Parameter.Usage;
								ElementTemp.uIndex = j;

								m_AttributeAddressMap.Insert(ElementTemp, m_uAttributeAddress ++);

								m_AttributesDefiner.AppendParameter(Parameter.Type, Parameter.Usage);
							}
						}*/
						puValue = m_AttributeAddressMap.Get(ElementTemp);
						if(!puValue)
						{
							m_AttributeAddressMap.Insert(ElementTemp, m_uAttributeAddress ++);

							m_AttributesDefiner.AppendParameter(ElementTemp.Type, ElementTemp.Usage, ElementTemp.uIndex);
						}

						pVariable->bIsUsed = false;
					}

					if(!pVariable->uHandle)
						pVariable->uHandle = 1;

					m_AttributeVariableMaxInfoMap.Insert(VariableKey, m_uVariableInfoCount);

					if(VariableKey.ShaderType == IEffect::VERTEX_SHADER)
					{
						VariableKey.ShaderType = IEffect::FRAGMENT_SHADER;
						m_AttributeVariableMaxInfoMap.Insert(VariableKey, m_uVariableInfoCount);
					}
				}
				else
				{
					/*Parameter.Type    = VariableKey.ElementType;
					Parameter.Usage   = VariableKey.ElementUsage;
					uDestinationCount = VariableKey.uIndex + 1;

					puValue = m_VaryingDefiner.GetShaderParameterKeyCount(Parameter);
					uCurrentCount = 0;
					if(!puValue || !(uCurrentCount = *puValue) || uCurrentCount < uDestinationCount)
					{
						for(j = uCurrentCount; j < uDestinationCount; ++ j)
						{
							ElementTemp.Type   = Parameter.Type;
							ElementTemp.Usage  = Parameter.Usage;
							ElementTemp.uIndex = j;

							m_VaryingAddressMap.Insert(ElementTemp, m_uVaryingAddress ++);

							m_VaryingDefiner.AppendParameter(Parameter.Type, Parameter.Usage);
						}
					}*/
					puValue = m_VaryingAddressMap.Get(ElementTemp);
					if(!puValue)
					{
						m_VaryingAddressMap.Insert(ElementTemp, m_uVaryingAddress ++);

						m_VaryingDefiner.AppendParameter(ElementTemp.Type, ElementTemp.Usage, ElementTemp.uIndex);
					}

					/*puValue = m_AttributesDefiner.GetShaderParameterKeyCount(Parameter);
					uCurrentCount = 0;
					if(!puValue || !(uCurrentCount = *puValue) || uCurrentCount < uDestinationCount)
					{
						for(j = uCurrentCount; j < uDestinationCount; ++ j)
						{
							ElementTemp.Type   = Parameter.Type;
							ElementTemp.Usage  = Parameter.Usage;
							ElementTemp.uIndex = j;

							m_AttributeAddressMap.Insert(ElementTemp, m_uAttributeAddress ++);

							m_AttributesDefiner.AppendParameter(Parameter.Type, Parameter.Usage);
						}
					}*/
					puValue = m_AttributeAddressMap.Get(ElementTemp);
					if(!puValue)
					{
						m_AttributeAddressMap.Insert(ElementTemp, m_uAttributeAddress ++);

						m_AttributesDefiner.AppendParameter(ElementTemp.Type, ElementTemp.Usage, ElementTemp.uIndex);
					}
				}
			}

			++ pBlendAttribute;
		}

		const CONSTANT* pBlendConstant = pBlendMethod->GetConstants(uConstantCount);
		for(i = 0; i < uConstantCount; ++ i)
		{
			GlobalVariable.ShaderType = IEffect::FRAGMENT_SHADER;
			GlobalVariable.GlobalType = pBlendConstant->Type;
			GlobalVariable.uDetail    = pBlendConstant->uDetail;
			GlobalVariable.uIndex     = pBlendConstant->uIndex;
			GlobalVariable.uCount     = pBlendConstant->uCount;
			GlobalVariable.pValue     = pBlendConstant->pValue;

			if(pBlendConstant->bIsForce)
			{
				/*GlobalVariableKey.ShaderType = IEffect::FRAGMENT_SHADER;
				GlobalVariableKey.GlobalType = pBlendConstant->Type;
				GlobalVariableKey.uDetail    = pBlendConstant->uDetail;
				GlobalVariableKey.uCount     = pBlendConstant->uCount;
				GlobalVariableKey.pValue     = pBlendConstant->pValue;
				uDestinationCount            = pBlendConstant->uIndex + 1;

				puValue = m_Parent.m_GlobalVariableCountMap.Get(GlobalVariableKey);
				uCurrentCount = 0;
				if(!puValue || !(uCurrentCount = *puValue) || uCurrentCount < uDestinationCount)
				{
					for(j = uCurrentCount; j < uDestinationCount; ++ j)
						m_Parent.AddGlobalVariable(GlobalVariableKey);
				}*/
				m_Parent.AddGlobalVariable(GlobalVariable);
			}
			else
			{
				/*GlobalVariable.ShaderType = IEffect::FRAGMENT_SHADER;
				GlobalVariable.GlobalType = pBlendConstant->Type;
				GlobalVariable.uDetail    = pBlendConstant->uDetail;
				GlobalVariable.uIndex     = pBlendConstant->uIndex;
				GlobalVariable.uCount     = pBlendConstant->uCount;
				GlobalVariable.pValue     = pBlendConstant->pValue;*/

				pVariable = m_ConstantVariableMap.Get(GlobalVariable);

				if(!pVariable)
				{
					VariableType = CEffectCode::GetGlobalVariableType(GlobalVariable.GlobalType, GlobalVariable.uDetail);
					if( CEffectCode::IsElementVariable(VariableType) )
					{
						GlobalVariable.ShaderType = IEffect::VERTEX_SHADER;

						pVariable = m_ConstantVariableMap.Get(GlobalVariable);

						if(pVariable)
						{
							pElement = m_ConstantVaryingMap.Get(GlobalVariable);

							if(!pElement)
							{
								/*Parameter.Type    = static_cast<CVertexDeclaration::ELEMENTTYPE>(VariableType);
								Parameter.Usage   = CVertexDeclaration::UNKNOWN;

								puValue = m_VaryingDefiner.GetShaderParameterKeyCount(Parameter);

								ElementTemp.Type   = Parameter.Type;
								ElementTemp.Usage  = Parameter.Usage;
								ElementTemp.uIndex = puValue ? *puValue : 0;

								m_VaryingAddressMap.Insert(ElementTemp, m_uVaryingAddress ++);

								m_VaryingDefiner.AppendParameter(Parameter.Type, Parameter.Usage);

								m_ConstantVaryingMap.Insert(GlobalVariable, ElementTemp);*/
								ElementTemp.Type   = static_cast<CVertexDeclaration::ELEMENTTYPE>(VariableType);
								ElementTemp.Usage  = CVertexDeclaration::UNKNOWN;
								ElementTemp.uIndex = m_uConstantVaryingCount ++;

								m_VaryingAddressMap.Insert(ElementTemp, m_uVaryingAddress ++);

								m_VaryingDefiner.AppendParameter(ElementTemp.Type, ElementTemp.Usage, ElementTemp.uIndex);

								m_ConstantVaryingMap.Insert(GlobalVariable, ElementTemp);
							}
						}
						else
							GlobalVariable.ShaderType = IEffect::FRAGMENT_SHADER;
					}
				}

				if(pVariable)
				{
					if(pVariable->bIsUsed && !pVariable->uSource)
					{
						/*GlobalVariableKey.ShaderType = GlobalVariable.ShaderType;
						GlobalVariableKey.GlobalType = GlobalVariable.GlobalType;
						GlobalVariableKey.uDetail    = GlobalVariable.uDetail;
						GlobalVariableKey.uCount     = GlobalVariable.uCount;
						GlobalVariableKey.pValue     = GlobalVariable.pValue;
						uDestinationCount            = GlobalVariable.uIndex + 1;

						puValue = m_Parent.m_GlobalVariableCountMap.Get(GlobalVariableKey);
						uCurrentCount = 0;
						if(!puValue || !(uCurrentCount = *puValue) || uCurrentCount < uDestinationCount)
							for(j = uCurrentCount; j < uDestinationCount; ++ j)
								m_Parent.AddGlobalVariable(GlobalVariableKey);*/

						pVariable->uSource = m_Parent.AddGlobalVariable(GlobalVariable);/*m_Parent.m_GlobalVariableHandleMap[GlobalVariable];*/

						pVariable->bIsUsed = false;
					}

					if(!pVariable->uHandle)
						pVariable->uHandle = 1;

					m_ConstantVariableMaxInfoMap.Insert(GlobalVariable, m_uVariableInfoCount);

					if(GlobalVariable.ShaderType == IEffect::VERTEX_SHADER)
					{
						GlobalVariable.ShaderType = IEffect::FRAGMENT_SHADER;
						m_ConstantVariableMaxInfoMap.Insert(GlobalVariable, m_uVariableInfoCount);
					}
				}
			}

			++ pBlendConstant;
		}

		++ m_uVariableInfoCount;
	}

	m_bIsBlend = m_bIsBlend || (m_EffectInfos.GetLength() > 1 && (pBlendMethod != ALCHEMY_NULL) && !EffectInfo.uOutputColorKey);

	return true;
}

/*bool CEffectCode::CPassDefiner::AddInfo(IEffectInfo& Info, BLENDMETHOD        BlendMethod)
{
	static CEffectInfoModulation s_Modulation;
	static CEffectInfoAddition s_Addition;

	static IEffectInfoBlendMethod* s_pBlendMethod[TOTAL_BLEND_METHOD] = {
		ALCHEMY_NULL, 
		ALCHEMY_NULL,
		&s_Modulation,
		ALCHEMY_NULL,
		ALCHEMY_NULL,
		&s_Addition,
		ALCHEMY_NULL,
		ALCHEMY_NULL,
		ALCHEMY_NULL,
		ALCHEMY_NULL,
		ALCHEMY_NULL,
		ALCHEMY_NULL,
		ALCHEMY_NULL,
		ALCHEMY_NULL,
		ALCHEMY_NULL,
		ALCHEMY_NULL,
		ALCHEMY_NULL,
		ALCHEMY_NULL,
		ALCHEMY_NULL,
		ALCHEMY_NULL,
		ALCHEMY_NULL,
		ALCHEMY_NULL,
		ALCHEMY_NULL,
		ALCHEMY_NULL,
		ALCHEMY_NULL
	};

	if(BlendMethod >= 0 && BlendMethod < TOTAL_BLEND_METHOD)
		return AddInfo(Info, s_pBlendMethod[BlendMethod]);

	return false;
}*/

//bool CEffectCode::CPassDefiner::FreeTemporary(UINT uHandle)
//{
//	LPVARIABLE pVarialbe;
//	if(uHandle && ( pVarialbe = m_Parent.m_VariablePool.Get(uHandle - 1) ) != ALCHEMY_NULL)
//	{
//		CEffectInterface::EFFECTTYPE ShaderType = CEffectInterface::PROGRAM;
//		if( pVarialbe->uFunction == _GetHandle(m_VertexShaderFunctionDefiner) )
//			ShaderType = CEffectInterface::VERTEX_SHADER;
//		else if( pVarialbe->uFunction == _GetHandle(m_FragmentShaderFunctionDefiner) )
//			ShaderType = CEffectInterface::FRAGMENT_SHADER;
//		else
//		{
//			ALCHEMY_DEBUG_WARNING("CEffectCode::CPassDefiner::FreeTemporary: Error varialbe.");
//
//			return false;
//		}
//
//		TEMPORARYDECLARATION TemporayKey;
//		TEMPORARY Temporay;
//		Temporay.ShaderType   = TemporayKey.ShaderType   = ShaderType;
//		Temporay.VariableType = TemporayKey.VariableType = pVarialbe->Type;
//
//		PUINT puValue = m_TemporaryMaximumCountMap.Get(TemporayKey);
//		if(puValue)
//			Temporay.uIndex = (*puValue) ++;
//		else
//		{
//			m_TemporaryMaximumCountMap.Insert(TemporayKey, 1);
//
//			Temporay.uIndex = 0;
//		}
//
//		m_TemporaryHandleMap.Insert(Temporay, uHandle);
//
//		return true;
//	}
//
//	ALCHEMY_DEBUG_WARNING("CEffectCode::CPassDefiner::FreeTemporary: Error handle.");
//
//	return false;
//}

UINT CEffectCode::CPassDefiner::AddColorKey(CVertexDeclaration::ELEMENTTYPE Type, bool bIsResultColor)
{
	COLORKEY ColorKey;
	ColorKey.Type                   = Type;
	ColorKey.uVariable              = 0;
	ColorKey.uRef                   = 0;
	ColorKey.uResultVariableAddress = bIsResultColor ? 1 : 0;

	return m_ColorKeyPool.Add(ColorKey) + 1;
}

UINT CEffectCode::CPassDefiner::EndDefine()
{
	CShaderParametersDefiner OutputColorDefiner(m_Parent);

	OutputColorDefiner.AppendParameter(CVertexDeclaration::FLOAT4, CVertexDeclaration::COLOR, 0);

	UINT uResultVaraibleAddress = 1;
	CPool<COLORKEY>::CIterator Iterator = m_ColorKeyPool.GetIterator(0);
	while( CPool<COLORKEY>::IsValuable(Iterator) )
	{
		if(Iterator->uResultVariableAddress)
		{
			Iterator->uResultVariableAddress = uResultVaraibleAddress/* ++*/;

			OutputColorDefiner.AppendParameter(Iterator->Type, CVertexDeclaration::COLOR, uResultVaraibleAddress);

			++ uResultVaraibleAddress;
		}

		++ Iterator;
	}

	bool bIsBlend = false, bIsInfo;

	UINT uVarying = m_VaryingDefiner.EndDefine(),
		uAttributes = m_VertexShaderFunctionDefiner.BeginFunction( m_AttributesDefiner.EndDefine() ), 
		uVertexShaderVarying = m_VertexShaderFunctionDefiner.EndFunction(uVarying), 
		uFragmentShaderVarying = m_FragmentShaderFunctionDefiner.BeginFunction(uVarying),
		uResultVariable = m_FragmentShaderFunctionDefiner.EndFunction( OutputColorDefiner.EndDefine() ),
		uResultColor = m_Parent.GetAddressVariable(uResultVariable, 0),
		uDestinationColor = uResultColor,
		uSourceColor1 = 0, 
		uSourceColor2 = 0,
		uSamplerCount = 0, uAttributeCount = 0, uVaryingCount = 0, uConstantCount = 0, uTemporaryCount = 0, uAttributeVariableCount = 0, uConstantVariableCount  = 0, uVariableCount = 0, uSourceColor, uSourceDestinationColor, uOutputColor, uTotal, uHandle, uAddress, /*uDestinationCount, uCurrentCount, */i/*, j*/;

	PUINT puValue, puSamplers, puAttributes, puConstants, puTemporary;

	LPVARYING pVaryingTemp;

	LPEFFECTINFO pInfo;

	LPSHADERVARIABLE pVariable;

	LPSHADERVARIABLE pAttributeVariables, pConstantVariables;

	//GLOBALVARIABLEKEY GlobalVariableKey;

	GLOBALVARIABLE GlobalVariable;

	VARYING Varying;

	//TEMPORARYDECLARATION TemporaryKey;

	TEMPORARYDECLARATION TemporaryDecalaration;

	ATTRIBUTEVARIABLEKEY VariableKey;

	SHADERVARIABLE Variable;

	ELEMENTDECLARATION ElementTemp;

	const VARIABLETYPE* pTemporaryType;

	const ATTRIBUTE* pBlendAttribute;

	const CONSTANT* pBlendConstant;

	const IEffectInfo::SAMPLER* pSamplers, * pSampler;

	const ELEMENTDECLARATION* pAttributes, * pVarying, * pElement;

	const GLOBALVARIABLE* pConstants;

	const TEMPORARYDECLARATION* pTemporary;

	const ATTRIBUTEVARIABLEDECLARATION* pAttributeVariableDeclaration;

	const CONSTANTVARIABLEDECLARATION* pConstantVariableDeclaration;
	
	CVector<EFFECTINFO>::CIterator EffectInfoIterator = m_EffectInfos.GetIterator(0);

	m_uVariableInfoCount = 0;
	while( CVector<EFFECTINFO>::IsValuable(EffectInfoIterator) )
	{
		pInfo = &(*EffectInfoIterator);

		if(pInfo)
		{ 
			if(pInfo->pEffectInfo)
			{
				pSamplers                     = pInfo->pEffectInfo->GetSamplers          (uSamplerCount          );

				pAttributes                   = pInfo->pEffectInfo->GetAttributes        (uAttributeCount         );
				pVarying                      = pInfo->pEffectInfo->GetVarying           (uVaryingCount           );

				pConstants                    = pInfo->pEffectInfo->GetConstants         (uConstantCount          );

				pTemporary                    = pInfo->pEffectInfo->GetTemporary         (uTemporaryCount         );

				pAttributeVariableDeclaration = pInfo->pEffectInfo->GetAttributeVariables(uAttributeVariableCount );

				pConstantVariableDeclaration  = pInfo->pEffectInfo->GetConstantVariables (uConstantVariableCount  );

				uVariableCount = uAttributeVariableCount + uConstantVariableCount;

				uTotal = uSamplerCount + uAttributeCount + uConstantCount + uTemporaryCount;

				if(m_uBufferLength < uTotal)
				{
					ALCHEMY_DEBUG_DELETE_ARRAY(m_puBuffer);

					ALCHEMY_DEBUG_NEW(m_puBuffer, UINT[uTotal]);

					m_uBufferLength = uTotal;
				}

				if(m_uVaryingBufferLength < uVaryingCount)
				{
					ALCHEMY_DEBUG_DELETE_ARRAY(m_pVaryingBuffer);

					ALCHEMY_DEBUG_NEW(m_pVaryingBuffer, VARYING[uVaryingCount]);

					m_uVaryingBufferLength = uVaryingCount;
				}

				if(m_uVariableBufferLength < uVariableCount)
				{
					ALCHEMY_DEBUG_DELETE_ARRAY(m_pVariableBuffer);

					ALCHEMY_DEBUG_NEW(m_pVariableBuffer, SHADERVARIABLE[uVariableCount]);

					m_uVariableBufferLength = uVariableCount;
				}

				puSamplers = m_puBuffer;
				for(i = 0; i < uSamplerCount; ++ i)
				{
					pSampler = &pSamplers[i];

					if(pSampler->Type == IEffect::VERTEX_SHADER)
						puSamplers[i] = m_VertexShaderFunctionDefiner.AddSamplerVariable(pSampler->Declaration, pSampler->uIndex);
					else if(pSampler->Type == IEffect::FRAGMENT_SHADER)
						puSamplers[i] = m_FragmentShaderFunctionDefiner.AddSamplerVariable(pSampler->Declaration, pSampler->uIndex);
					else
						puSamplers[i] = 0;
				}

				puAttributes = puSamplers + uSamplerCount;
				for(i = 0; i < uAttributeCount; ++ i)
				{
					pElement = &pAttributes[i];

					puValue = m_AttributeMap.Get(*pElement);

					if( !puValue || !(uHandle = *puValue) )
					{
						uHandle = m_Parent.GetAddressVariable(uAttributes, m_AttributeAddressMap[*pElement]);
						m_AttributeMap.Insert(*pElement, uHandle);
					}

					puAttributes[i] = uHandle;
				}

				for(i = 0; i < uVaryingCount; ++ i)
				{
					pElement = &pVarying[i];

					pVaryingTemp = m_VaryingMap.Get(*pElement);

					if(!pVaryingTemp)
					{
						uAddress = m_VaryingAddressMap[*pElement];

						Varying.uVertexShaderHandle   = m_Parent.GetAddressVariable(uVertexShaderVarying, uAddress);
						Varying.uFragmentShaderHandle = m_Parent.GetAddressVariable(uFragmentShaderVarying, uAddress);
						Varying.IsUsed                = false;

						m_VaryingMap.Insert(*pElement, Varying);

						pVaryingTemp = m_VaryingMap.Get(*pElement);
					}
					
					m_pVaryingBuffer[i] = *pVaryingTemp;

					pVaryingTemp->IsUsed = true;
				}

				puConstants = puAttributes + uAttributeCount;
				for(i = 0; i < uConstantCount; ++ i)
					puConstants[i] = m_Parent.m_GlobalVariableHandleMap[pConstants[i]];//m_ConstantMap[pConstants[i]];

				__UpdateTemporary();

				__FreeTemporary();

				puTemporary = puConstants + uConstantCount;
				for(i = 0; i < uTemporaryCount; ++ i)
				{
					uHandle = __PopFreeTemporary(*pTemporary);
					puTemporary[i] = uHandle;
					__PushFreeTemporary(*pTemporary, uHandle, true);

					++ pTemporary;
				}

				pAttributeVariables = m_pVariableBuffer;
				for(i = 0; i < uAttributeVariableCount; ++ i)
				{
					VariableKey.ShaderType   = pAttributeVariableDeclaration->ShaderType;
					VariableKey.ElementType  = pAttributeVariableDeclaration->ElementType;
					VariableKey.ElementUsage = pAttributeVariableDeclaration->ElementUsage;
					VariableKey.uIndex       = pAttributeVariableDeclaration->uIndex;

					pVariable = m_AttributeVariableMap.Get(VariableKey);

					if(!pVariable && VariableKey.ShaderType == IEffect::FRAGMENT_SHADER)
					{
						VariableKey.ShaderType = IEffect::VERTEX_SHADER;

						pVariable = m_AttributeVariableMap.Get(VariableKey);

						if(!pVariable)
							VariableKey.ShaderType = IEffect::FRAGMENT_SHADER;
					}

					if(pVariable)
					{
						if(pAttributeVariableDeclaration->bIsReadOnly)
						{
							if(!pVariable->uHandle)
							{
								if(pAttributeVariableDeclaration->bIsForce)
								{
									if(!pVariable->uSource)
									{
										ElementTemp.Type   = pAttributeVariableDeclaration->ElementType;
										ElementTemp.Usage  = pAttributeVariableDeclaration->ElementUsage;
										ElementTemp.uIndex = pAttributeVariableDeclaration->uIndex;

										if(pAttributeVariableDeclaration->ShaderType == IEffect::VERTEX_SHADER)
										{
											puValue = m_AttributeMap.Get(ElementTemp);

											if( !puValue || !(uHandle = *puValue) )
											{
												uHandle = m_Parent.GetAddressVariable(uAttributes, m_AttributeAddressMap[ElementTemp]);
												m_AttributeMap.Insert(ElementTemp, uHandle);
											}

											pVariable->uSource = uHandle;
										}
										else if(pAttributeVariableDeclaration->ShaderType == IEffect::FRAGMENT_SHADER)
										{
											pVaryingTemp = m_VaryingMap.Get(ElementTemp);

											if(!pVaryingTemp)
											{
												uAddress = m_VaryingAddressMap[ElementTemp];

												Varying.uVertexShaderHandle   = m_Parent.GetAddressVariable(uVertexShaderVarying, uAddress);
												Varying.uFragmentShaderHandle = m_Parent.GetAddressVariable(uFragmentShaderVarying, uAddress);
												Varying.IsUsed                = true;

												m_VaryingMap.Insert(ElementTemp, Varying);

												puValue = m_AttributeMap.Get(ElementTemp);

												if( !puValue || !(uHandle = *puValue) )
												{
													uHandle = m_Parent.GetAddressVariable(uAttributes, m_AttributeAddressMap[ElementTemp]);
													m_AttributeMap.Insert(ElementTemp, uHandle);
												}

												m_VertexShaderFunctionDefiner.AppendInstruction(ICompiler::MOV, Varying.uVertexShaderHandle, ALCHEMY_NULL, 0, uHandle, ALCHEMY_NULL, 0, 0, ALCHEMY_NULL, 0);

												pVaryingTemp = &Varying;
											}

											pVariable->uSource = pVaryingTemp->uFragmentShaderHandle;
										}
									}

									pVariable->uHandle = pVariable->uSource;
								}
							}
							else
							{
								uHandle = pVariable->uHandle;
								if(VariableKey.ShaderType == IEffect::VERTEX_SHADER && pAttributeVariableDeclaration->ShaderType != VariableKey.ShaderType)
								{
									ElementTemp.Type   = pAttributeVariableDeclaration->ElementType;
									ElementTemp.Usage  = pAttributeVariableDeclaration->ElementUsage;
									ElementTemp.uIndex = pAttributeVariableDeclaration->uIndex;

									pVaryingTemp = m_VaryingMap.Get(ElementTemp);

									if(pVaryingTemp)
									{
										pVariable = m_AttributeShaderVariableMap.Get(ElementTemp);

										if(pVariable)
										{
											if(!pVaryingTemp->IsUsed)
											{
												m_VertexShaderFunctionDefiner.AppendInstruction(ICompiler::MOV, pVaryingTemp->uVertexShaderHandle, ALCHEMY_NULL, 0, uHandle, ALCHEMY_NULL, 0, 0, ALCHEMY_NULL, 0);

												pVaryingTemp->IsUsed = true;
											}
										}
										else
										{
											Variable.bIsUsed = false;
											Variable.uSource = pVaryingTemp->uFragmentShaderHandle;
											Variable.uHandle = pVaryingTemp->uFragmentShaderHandle;

											pVariable = &Variable;
										}
									}
									else
									{
										uAddress = m_VaryingAddressMap[ElementTemp];

										Varying.uVertexShaderHandle   = m_Parent.GetAddressVariable(uVertexShaderVarying, uAddress);
										Varying.uFragmentShaderHandle = m_Parent.GetAddressVariable(uFragmentShaderVarying, uAddress);
										Varying.IsUsed                = true;

										m_VaryingMap.Insert(ElementTemp, Varying);

										m_VertexShaderFunctionDefiner.AppendInstruction(ICompiler::MOV, Varying.uVertexShaderHandle, ALCHEMY_NULL, 0, uHandle, ALCHEMY_NULL, 0, 0, ALCHEMY_NULL, 0);

										pVaryingTemp = &Varying;

										Variable.bIsUsed = false;
										Variable.uSource = pVaryingTemp->uFragmentShaderHandle;
										Variable.uHandle = pVaryingTemp->uFragmentShaderHandle;

										pVariable = &Variable;
									}

									puValue = m_AttributeVariableMaxInfoMap.Get(VariableKey);
									if(puValue && m_uVariableInfoCount == *puValue)
									{
										TemporaryDecalaration.ShaderType   = VariableKey.ShaderType;
										TemporaryDecalaration.VariableType = static_cast<VARIABLETYPE>(VariableKey.ElementType);
										TemporaryDecalaration.uCount       = 1;

										__PushFreeTemporary(TemporaryDecalaration, uHandle, true);
									}

									VariableKey.ShaderType = IEffect::FRAGMENT_SHADER;

									m_AttributeVariableMap.Insert(VariableKey, *pVariable);

									/*puValue = m_AttributeVariableMaxInfoMap.Get(VariableKey);
									if(!puValue)
									{
										TemporaryDecalaration.ShaderType   = VariableKey.ShaderType;
										TemporaryDecalaration.VariableType = static_cast<VARIABLETYPE>(VariableKey.ElementType);
										TemporaryDecalaration.uCount       = 1;

										__PushFreeTemporary(TemporaryDecalaration, pVariable->uHandle, false);
									}*/
								}
								else
								{
									puValue = m_AttributeVariableMaxInfoMap.Get(VariableKey);
									if(puValue && m_uVariableInfoCount == *puValue)
									{
										TemporaryDecalaration.ShaderType   = VariableKey.ShaderType;
										TemporaryDecalaration.VariableType = static_cast<VARIABLETYPE>(VariableKey.ElementType);
										TemporaryDecalaration.uCount       = 1;

										__PushFreeTemporary(TemporaryDecalaration, uHandle, true);
									}
								}
							}
						}
						else if(pVariable->uHandle)
						{
							puValue = m_AttributeVariableMaxInfoMap.Get(VariableKey);
							if(!puValue || m_uVariableInfoCount > *puValue)
								pVariable->uHandle = 0;
							else
							{
								if(pAttributeVariableDeclaration->bIsForce && !pVariable->uSource)
								{
									ElementTemp.Type   = pAttributeVariableDeclaration->ElementType;
									ElementTemp.Usage  = pAttributeVariableDeclaration->ElementUsage;
									ElementTemp.uIndex = pAttributeVariableDeclaration->uIndex;

									if(pAttributeVariableDeclaration->ShaderType == IEffect::VERTEX_SHADER)
									{
										puValue = m_AttributeMap.Get(ElementTemp);

										if( !puValue || !(uHandle = *puValue) )
										{
											uHandle = m_Parent.GetAddressVariable(uAttributes, m_AttributeAddressMap[ElementTemp]);
											m_AttributeMap.Insert(ElementTemp, uHandle);
										}

										pVariable->uSource = uHandle;
									}
									else if(pAttributeVariableDeclaration->ShaderType == IEffect::FRAGMENT_SHADER)
									{
										pVaryingTemp = m_VaryingMap.Get(ElementTemp);

										if(!pVaryingTemp)
										{
											uAddress = m_VaryingAddressMap[ElementTemp];

											Varying.uVertexShaderHandle   = m_Parent.GetAddressVariable(uVertexShaderVarying, uAddress);
											Varying.uFragmentShaderHandle = m_Parent.GetAddressVariable(uFragmentShaderVarying, uAddress);
											Varying.IsUsed                = true;

											m_VaryingMap.Insert(ElementTemp, Varying);

											puValue = m_AttributeMap.Get(ElementTemp);

											if( !puValue || !(uHandle = *puValue) )
											{
												uHandle = m_Parent.GetAddressVariable(uAttributes, m_AttributeAddressMap[ElementTemp]);
												m_AttributeMap.Insert(ElementTemp, uHandle);
											}

											m_VertexShaderFunctionDefiner.AppendInstruction(ICompiler::MOV, Varying.uVertexShaderHandle, ALCHEMY_NULL, 0, uHandle, ALCHEMY_NULL, 0, 0, ALCHEMY_NULL, 0);

											pVaryingTemp = &Varying;
										}

										pVariable->uSource = pVaryingTemp->uFragmentShaderHandle;
									}
								}

								if(!pVariable->bIsUsed)
								{
									TemporaryDecalaration.ShaderType   = VariableKey.ShaderType;
									TemporaryDecalaration.VariableType = static_cast<VARIABLETYPE>(VariableKey.ElementType);
									TemporaryDecalaration.uCount = 1;

									pVariable->uHandle = __PopFreeTemporary(TemporaryDecalaration);
								}
								else if(VariableKey.ShaderType == IEffect::VERTEX_SHADER && VariableKey.ShaderType != pAttributeVariableDeclaration->ShaderType)
								{
									ElementTemp.Type   = pAttributeVariableDeclaration->ElementType;
									ElementTemp.Usage  = pAttributeVariableDeclaration->ElementUsage;
									ElementTemp.uIndex = pAttributeVariableDeclaration->uIndex;

									puValue = m_AttributeVaryingMap.Get(ElementTemp);
									if(puValue)
									{
										pVaryingTemp = m_VaryingMap.Get(ElementTemp);

										if(!pVaryingTemp)
										{
											uAddress = m_VaryingAddressMap[ElementTemp];

											Varying.uVertexShaderHandle   = m_Parent.GetAddressVariable(uVertexShaderVarying, uAddress);
											Varying.uFragmentShaderHandle = m_Parent.GetAddressVariable(uFragmentShaderVarying, uAddress);
											Varying.IsUsed                = false;

											m_VaryingMap.Insert(ElementTemp, Varying);

											pVaryingTemp = &Varying;
										}

										pVariable = m_AttributeShaderVariableMap.Get(ElementTemp);
										if(!pVariable)
										{
											TemporaryDecalaration.ShaderType   = IEffect::FRAGMENT_SHADER;
											TemporaryDecalaration.VariableType = static_cast<VARIABLETYPE>(ElementTemp.Type);
											TemporaryDecalaration.uCount       = 1;

											Variable.bIsUsed = false;
											Variable.uSource = pVaryingTemp->uFragmentShaderHandle;
											Variable.uHandle = __PopFreeTemporary(TemporaryDecalaration);

											m_AttributeShaderVariableMap.Insert(ElementTemp, Variable);

											pVariable = m_AttributeShaderVariableMap.Get(ElementTemp);
										}

										VariableKey.ShaderType = IEffect::FRAGMENT_SHADER;

										puValue = m_AttributeVariableMaxInfoMap.Get(VariableKey);
										if(!puValue || m_uVariableInfoCount > *puValue)
											pVariable->uHandle = 0;
									}
									else
									{
										Variable.uHandle = 0;
										Variable.uSource = 0;
										Variable.bIsUsed = false;
										pVariable = &Variable;
									}
								}
							}
						}

						pAttributeVariables[i] = *pVariable;

						if(!pAttributeVariableDeclaration->bIsReadOnly)
							pVariable->bIsUsed = true;
					}
					else
					{
						pVariable = &pAttributeVariables[i];
						pVariable->uHandle = 0;
						pVariable->uSource = 0;
						pVariable->bIsUsed = false;

						if(pAttributeVariableDeclaration->bIsReadOnly && pAttributeVariableDeclaration->bIsForce)
						{
							ElementTemp.Type   = pAttributeVariableDeclaration->ElementType;
							ElementTemp.Usage  = pAttributeVariableDeclaration->ElementUsage;
							ElementTemp.uIndex = pAttributeVariableDeclaration->uIndex;

							if(pAttributeVariableDeclaration->ShaderType == IEffect::VERTEX_SHADER)
							{
								puValue = m_AttributeMap.Get(ElementTemp);

								if( !puValue || !(uHandle = *puValue) )
								{
									uHandle = m_Parent.GetAddressVariable(uAttributes, m_AttributeAddressMap[ElementTemp]);
									m_AttributeMap.Insert(ElementTemp, uHandle);
								}

								pVariable->uSource = uHandle;
							}
							else if(pAttributeVariableDeclaration->ShaderType == IEffect::FRAGMENT_SHADER)
							{
								pVaryingTemp = m_VaryingMap.Get(ElementTemp);

								if(!pVaryingTemp)
								{
									uAddress = m_VaryingAddressMap[ElementTemp];

									Varying.uVertexShaderHandle   = m_Parent.GetAddressVariable(uVertexShaderVarying, uAddress);
									Varying.uFragmentShaderHandle = m_Parent.GetAddressVariable(uFragmentShaderVarying, uAddress);
									Varying.IsUsed                = true;

									m_VaryingMap.Insert(ElementTemp, Varying);

									puValue = m_AttributeMap.Get(ElementTemp);

									if( !puValue || !(uHandle = *puValue) )
									{
										uHandle = m_Parent.GetAddressVariable(uAttributes, m_AttributeAddressMap[ElementTemp]);
										m_AttributeMap.Insert(ElementTemp, uHandle);
									}

									m_VertexShaderFunctionDefiner.AppendInstruction(ICompiler::MOV, Varying.uVertexShaderHandle, ALCHEMY_NULL, 0, uHandle, ALCHEMY_NULL, 0, 0, ALCHEMY_NULL, 0);

									pVaryingTemp = &Varying;
								}

								pVariable->uSource = pVaryingTemp->uFragmentShaderHandle;
							}

							pVariable->uHandle = pVariable->uSource;

							m_AttributeVariableMap.Insert(VariableKey, *pVariable);
						}
					}

					++ pAttributeVariableDeclaration;
				}

				pConstantVariables = m_pVariableBuffer + uAttributeVariableCount;
				for(i = 0; i < uConstantVariableCount; ++ i)
				{
					GlobalVariable.ShaderType = pConstantVariableDeclaration->ShaderType;
					GlobalVariable.GlobalType = pConstantVariableDeclaration->GlobalType;
					GlobalVariable.uDetail    = pConstantVariableDeclaration->uDetail;
					GlobalVariable.uIndex     = pConstantVariableDeclaration->uIndex;
					GlobalVariable.uCount     = pConstantVariableDeclaration->uCount;
					GlobalVariable.pValue     = pConstantVariableDeclaration->pValue;

					pVariable = m_ConstantVariableMap.Get(GlobalVariable);

					if(!pVariable && GlobalVariable.ShaderType == IEffect::FRAGMENT_SHADER)
					{
						GlobalVariable.ShaderType = IEffect::VERTEX_SHADER;

						pVariable = m_ConstantVariableMap.Get(GlobalVariable);

						if(pVariable)
							pElement = m_ConstantVaryingMap.Get(GlobalVariable);
						else
							pElement = ALCHEMY_NULL;

						if(!pElement)
						{
							GlobalVariable.ShaderType = IEffect::FRAGMENT_SHADER;

							pVariable = ALCHEMY_NULL;
						}
					}

					if(pVariable)
					{
						if(pConstantVariableDeclaration->bIsReadOnly)
						{
							if(!pVariable->uHandle)
							{
								if(pConstantVariableDeclaration->bIsForce)
								{
									if(!pVariable->uSource)
									{
										/*GlobalVariableKey.ShaderType = GlobalVariable.ShaderType;
										GlobalVariableKey.GlobalType = GlobalVariable.GlobalType;
										GlobalVariableKey.uDetail    = GlobalVariable.uDetail;
										GlobalVariableKey.uCount     = GlobalVariable.uCount;
										GlobalVariableKey.pValue     = GlobalVariable.pValue;
										uDestinationCount            = GlobalVariable.uIndex + 1;

										puValue = m_Parent.m_GlobalVariableCountMap.Get(GlobalVariableKey);
										uCurrentCount = 0;
										if(!puValue || !(uCurrentCount = *puValue) || uCurrentCount < uDestinationCount)
											for(j = uCurrentCount; j < uDestinationCount; ++ j)
												m_Parent.AddGlobalVariable(GlobalVariableKey);*/

										pVariable->uSource = m_Parent.AddGlobalVariable(GlobalVariable);/*m_Parent.m_GlobalVariableHandleMap[GlobalVariable];*/
									}

									pVariable->uHandle = pVariable->uSource;
								}
							}
							else
							{
								uHandle = pVariable->uHandle;

								if(GlobalVariable.ShaderType == IEffect::VERTEX_SHADER && GlobalVariable.ShaderType != pConstantVariableDeclaration->ShaderType)
								{
									pVaryingTemp = m_VaryingMap.Get(*pElement);

									if(pVaryingTemp)
									{
										pVariable = m_AttributeShaderVariableMap.Get(*pElement);

										if(pVariable)
										{
											if(!pVaryingTemp->IsUsed)
											{
												m_VertexShaderFunctionDefiner.AppendInstruction(ICompiler::MOV, pVaryingTemp->uVertexShaderHandle, ALCHEMY_NULL, 0, uHandle, ALCHEMY_NULL, 0, 0, ALCHEMY_NULL, 0);

												pVaryingTemp->IsUsed = true;
											}
										}
										else
										{
											Variable.bIsUsed = false;
											Variable.uSource = pVaryingTemp->uFragmentShaderHandle;
											Variable.uHandle = pVaryingTemp->uFragmentShaderHandle;

											pVariable = &Variable;
										}
									}
									else
									{
										uAddress = m_VaryingAddressMap[*pElement];

										Varying.uVertexShaderHandle   = m_Parent.GetAddressVariable(uVertexShaderVarying, uAddress);
										Varying.uFragmentShaderHandle = m_Parent.GetAddressVariable(uFragmentShaderVarying, uAddress);
										Varying.IsUsed                = true;

										m_VaryingMap.Insert(*pElement, Varying);

										m_VertexShaderFunctionDefiner.AppendInstruction(ICompiler::MOV, Varying.uVertexShaderHandle, ALCHEMY_NULL, 0, uHandle, ALCHEMY_NULL, 0, 0, ALCHEMY_NULL, 0);

										pVaryingTemp = &Varying;

										Variable.bIsUsed = false;
										Variable.uSource = pVaryingTemp->uFragmentShaderHandle;
										Variable.uHandle = pVaryingTemp->uFragmentShaderHandle;

										pVariable = &Variable;
									}

									puValue = m_ConstantVariableMaxInfoMap.Get(GlobalVariable);
									if(puValue && m_uVariableInfoCount == *puValue)
									{
										TemporaryDecalaration.ShaderType   = GlobalVariable.ShaderType;
										TemporaryDecalaration.VariableType = CEffectCode::GetGlobalVariableType(GlobalVariable.GlobalType, GlobalVariable.uDetail);
										TemporaryDecalaration.uCount       = GlobalVariable.uCount;

										__PushFreeTemporary(TemporaryDecalaration, uHandle, true);
									}

									GlobalVariable.ShaderType = IEffect::FRAGMENT_SHADER;

									m_ConstantVariableMap.Insert(GlobalVariable, *pVariable);
								}
								else
								{
									puValue = m_ConstantVariableMaxInfoMap.Get(GlobalVariable);
									if(puValue && m_uVariableInfoCount == *puValue)
									{
										TemporaryDecalaration.ShaderType   = GlobalVariable.ShaderType;
										TemporaryDecalaration.VariableType = CEffectCode::GetGlobalVariableType(GlobalVariable.GlobalType, GlobalVariable.uDetail);
										TemporaryDecalaration.uCount       = GlobalVariable.uCount;

										__PushFreeTemporary(TemporaryDecalaration, uHandle, true);
									}
								}
							}
						}
						else if(pVariable->uHandle)
						{
							puValue = m_ConstantVariableMaxInfoMap.Get(GlobalVariable);
							if(!puValue || m_uVariableInfoCount > *puValue)
								pVariable->uHandle = 0;
							else 
							{
								if(!pVariable->bIsUsed)
								{
									TemporaryDecalaration.ShaderType   = GlobalVariable.ShaderType;
									TemporaryDecalaration.VariableType = CEffectCode::GetGlobalVariableType(GlobalVariable.GlobalType, GlobalVariable.uDetail);
									TemporaryDecalaration.uCount = GlobalVariable.uCount;

									pVariable->uHandle = __PopFreeTemporary(TemporaryDecalaration);
								}
								else if(GlobalVariable.ShaderType == IEffect::VERTEX_SHADER && GlobalVariable.ShaderType != pConstantVariableDeclaration->ShaderType && pElement)
								{
									pVaryingTemp = m_VaryingMap.Get(*pElement);

									if(!pVaryingTemp)
									{
										uAddress = m_VaryingAddressMap[*pElement];

										Varying.uVertexShaderHandle   = m_Parent.GetAddressVariable(uVertexShaderVarying, uAddress);
										Varying.uFragmentShaderHandle = m_Parent.GetAddressVariable(uFragmentShaderVarying, uAddress);
										Varying.IsUsed                = false;

										m_VaryingMap.Insert(*pElement, Varying);

										pVaryingTemp = &Varying;
									}

									pVariable = m_AttributeShaderVariableMap.Get(*pElement);

									if(!pVariable)
									{
										TemporaryDecalaration.ShaderType   = IEffect::FRAGMENT_SHADER;
										TemporaryDecalaration.VariableType = static_cast<VARIABLETYPE>(pElement->Type);
										TemporaryDecalaration.uCount       = GlobalVariable.uCount;

										Variable.bIsUsed = false;
										Variable.uSource = pVaryingTemp->uFragmentShaderHandle;
										Variable.uHandle = __PopFreeTemporary(TemporaryDecalaration);

										m_AttributeShaderVariableMap.Insert(*pElement, Variable);

										pVariable = m_AttributeShaderVariableMap.Get(*pElement);
									}

									GlobalVariable.ShaderType = IEffect::FRAGMENT_SHADER;

									puValue = m_ConstantVariableMaxInfoMap.Get(GlobalVariable);
									if(!puValue || m_uVariableInfoCount > *puValue)
										pVariable->uHandle = 0;
								}
							}
						}

						pConstantVariables[i] = *pVariable;

						if(!pConstantVariableDeclaration->bIsReadOnly)
							pVariable->bIsUsed = true;
					}
					else
					{
						pVariable = &pConstantVariables[i];
						pVariable->uHandle = 0;
						pVariable->uSource = 0;
						pVariable->bIsUsed = false;

						if(pConstantVariableDeclaration->bIsReadOnly && pConstantVariableDeclaration->bIsForce)
						{
							/*GlobalVariableKey.ShaderType = GlobalVariable.ShaderType;
							GlobalVariableKey.GlobalType = GlobalVariable.GlobalType;
							GlobalVariableKey.uDetail    = GlobalVariable.uDetail;
							GlobalVariableKey.uCount     = GlobalVariable.uCount;
							GlobalVariableKey.pValue     = GlobalVariable.pValue;
							uDestinationCount            = GlobalVariable.uIndex + 1;

							puValue = m_Parent.m_GlobalVariableCountMap.Get(GlobalVariableKey);
							uCurrentCount = 0;
							if(!puValue || !(uCurrentCount = *puValue) || uCurrentCount < uDestinationCount)
								for(j = uCurrentCount; j < uDestinationCount; ++ j)
									m_Parent.AddGlobalVariable(GlobalVariableKey);*/

							pVariable->uSource = m_Parent.AddGlobalVariable(GlobalVariable);/*m_Parent.m_GlobalVariableHandleMap[GlobalVariable];*/

							pVariable->uHandle = pVariable->uSource;

							m_ConstantVariableMap.Insert(GlobalVariable, *pVariable);
						}
					}

					++ pConstantVariableDeclaration;
				}

				++ m_uVariableInfoCount;
			}

			bIsInfo = pInfo->pEffectInfo && ALCHEMY_TEST_BIT(pInfo->pEffectInfo->GetEffectInfoFlag(), IEffectInfo::FRAGMENT_SHADER_OUTPUT);
			if( bIsBlend && pInfo->pBlendMethod && (bIsInfo || pInfo->uInputColorKey) )
			{
				uSourceColor            = __GetColorValue(pInfo->uInputColorKey, uResultVariable);
				uSourceDestinationColor = __GetColorValue(pInfo->uSourceColorKey, uResultVariable);
				uOutputColor            = __GetColorValue(pInfo->uOutputColorKey, uResultVariable);

				if(bIsInfo)
				{
					TemporaryDecalaration.ShaderType   = IEffect::FRAGMENT_SHADER;
					TemporaryDecalaration.VariableType = VECTOR4_VARIABLE;
					TemporaryDecalaration.uCount       = 1;
					uSourceColor1 = __PopFreeTemporary(TemporaryDecalaration);

					pInfo->pEffectInfo->ShaderDefineHandler(uSourceColor1, puSamplers, puAttributes, puConstants, m_pVaryingBuffer, puTemporary, pAttributeVariables, pConstantVariables, m_VertexShaderFunctionDefiner, m_FragmentShaderFunctionDefiner, m_EffectCodeUtils);
				}
				else
					uSourceColor1 = 0;

				pTemporaryType = pInfo->pBlendMethod->GetTemporary(uTemporaryCount);

				pBlendAttribute = pInfo->pBlendMethod->GetAttributes(uAttributeCount);

				pBlendConstant = pInfo->pBlendMethod->GetConstants(uConstantCount);

				uTotal = uTemporaryCount + uAttributeCount + uConstantCount;

				if(m_uBufferLength < uTotal)
				{
					ALCHEMY_DEBUG_DELETE_ARRAY(m_puBuffer);

					ALCHEMY_DEBUG_NEW(m_puBuffer, UINT[uTotal]);

					m_uBufferLength = uTotal;
				}

				__UpdateTemporary();

				__FreeTemporary();

				puTemporary = m_puBuffer;
				for(i = 0; i < uTemporaryCount; ++ i)
				{
					TemporaryDecalaration.ShaderType   = IEffect::FRAGMENT_SHADER;
					TemporaryDecalaration.VariableType = pTemporaryType[i];
					TemporaryDecalaration.uCount       = 1;

					uHandle = __PopFreeTemporary(TemporaryDecalaration);
					puTemporary[i] = uHandle;
					__PushFreeTemporary(TemporaryDecalaration, uHandle, true);
				}

				puAttributes = m_puBuffer + uTemporaryCount;
				for(i = 0; i < uAttributeCount; ++ i)
				{
					if(pBlendAttribute->bIsForce)
					{
						ElementTemp.Type   = pBlendAttribute->Type;
						ElementTemp.Usage  = pBlendAttribute->Usage;
						ElementTemp.uIndex = pBlendAttribute->uIndex;

						puValue = m_AttributeMap.Get(ElementTemp);
						if( !puValue || !(uHandle = *puValue) )
						{
							uHandle = m_Parent.GetAddressVariable(uAttributes, m_AttributeAddressMap[ElementTemp]);
							m_AttributeMap.Insert(ElementTemp, uHandle);
						}

						puAttributes[i] = uHandle;
					}
					else
					{
						VariableKey.ShaderType   = IEffect::FRAGMENT_SHADER;
						VariableKey.ElementType  = pBlendAttribute->Type;
						VariableKey.ElementUsage = pBlendAttribute->Usage;
						VariableKey.uIndex       = pBlendAttribute->uIndex;

						pVariable = m_AttributeVariableMap.Get(VariableKey);

						if(!pVariable)
						{
							VariableKey.ShaderType = IEffect::VERTEX_SHADER;

							pVariable = m_AttributeVariableMap.Get(VariableKey);

							if(!pVariable)
								VariableKey.ShaderType = IEffect::FRAGMENT_SHADER;
						}

						if(!pVariable)
						{
							m_AttributeVariableMap.Insert(VariableKey, Variable);

							pVariable = m_AttributeVariableMap.Get(VariableKey);

							pVariable->uHandle = 0;
							pVariable->uSource = 0;
							pVariable->bIsUsed = false;
						}

						if(!pVariable->uHandle)
						{
							if(!pVariable->uSource)
							{
								ElementTemp.Type   = VariableKey.ElementType;
								ElementTemp.Usage  = VariableKey.ElementUsage;
								ElementTemp.uIndex = VariableKey.uIndex;

								pVaryingTemp = m_VaryingMap.Get(ElementTemp);

								if(!pVaryingTemp)
								{
									uAddress = m_VaryingAddressMap[ElementTemp];

									Varying.uVertexShaderHandle   = m_Parent.GetAddressVariable(uVertexShaderVarying, uAddress);
									Varying.uFragmentShaderHandle = m_Parent.GetAddressVariable(uFragmentShaderVarying, uAddress);
									Varying.IsUsed                = true;

									m_VaryingMap.Insert(ElementTemp, Varying);

									puValue = m_AttributeMap.Get(ElementTemp);

									if( !puValue || !(uHandle = *puValue) )
									{
										uHandle = m_Parent.GetAddressVariable(uAttributes, m_AttributeAddressMap[ElementTemp]);
										m_AttributeMap.Insert(ElementTemp, uHandle);
									}

									m_VertexShaderFunctionDefiner.AppendInstruction(ICompiler::MOV, Varying.uVertexShaderHandle, ALCHEMY_NULL, 0, uHandle, ALCHEMY_NULL, 0, 0, ALCHEMY_NULL, 0);

									pVaryingTemp = &Varying;
								}

								pVariable->uSource = pVaryingTemp->uFragmentShaderHandle;
							}

							pVariable->uHandle = pVariable->uSource;
						}
						else
						{
							
							if(VariableKey.ShaderType == IEffect::VERTEX_SHADER)
							{
								ElementTemp.Type   = VariableKey.ElementType;
								ElementTemp.Usage  = VariableKey.ElementUsage;
								ElementTemp.uIndex = VariableKey.uIndex;

								pVaryingTemp = m_VaryingMap.Get(ElementTemp);

								if(!pVaryingTemp)
								{
									uAddress = m_VaryingAddressMap[ElementTemp];

									Varying.uVertexShaderHandle   = m_Parent.GetAddressVariable(uVertexShaderVarying, uAddress);
									Varying.uFragmentShaderHandle = m_Parent.GetAddressVariable(uFragmentShaderVarying, uAddress);
									Varying.IsUsed                = true;

									m_VaryingMap.Insert(ElementTemp, Varying);

									m_VertexShaderFunctionDefiner.AppendInstruction(ICompiler::MOV, Varying.uVertexShaderHandle, ALCHEMY_NULL, 0, pVariable->uHandle, ALCHEMY_NULL, 0, 0, ALCHEMY_NULL, 0);

									pVaryingTemp = &Varying;
								}

								puValue = m_AttributeVariableMaxInfoMap.Get(VariableKey);
								if(puValue && m_uVariableInfoCount == *puValue)
								{
									TemporaryDecalaration.ShaderType   = VariableKey.ShaderType;
									TemporaryDecalaration.VariableType = static_cast<VARIABLETYPE>(VariableKey.ElementType);
									TemporaryDecalaration.uCount       = 1;

									__PushFreeTemporary(TemporaryDecalaration, pVariable->uHandle, true);
								}

								Variable.bIsUsed = false;
								Variable.uSource = pVaryingTemp->uFragmentShaderHandle;
								Variable.uHandle = pVaryingTemp->uFragmentShaderHandle;

								VariableKey.ShaderType = IEffect::FRAGMENT_SHADER;

								m_AttributeVariableMap.Insert(VariableKey, Variable);

								pVariable = &Variable;
							}
							else
							{
								puValue = m_AttributeVariableMaxInfoMap.Get(VariableKey);
								if(puValue && m_uVariableInfoCount == *puValue)
								{
									TemporaryDecalaration.ShaderType   = VariableKey.ShaderType;
									TemporaryDecalaration.VariableType = static_cast<VARIABLETYPE>(VariableKey.ElementType);
									TemporaryDecalaration.uCount       = 1;

									__PushFreeTemporary(TemporaryDecalaration, pVariable->uHandle, true);
								}
							}
						}

						puAttributes[i] = pVariable->uHandle;
					}

					++ pBlendAttribute;
				}

				puConstants = puAttributes + uAttributeCount;
				for(i = 0; i < uConstantCount; ++ i)
				{
					if(pBlendConstant->bIsForce)
					{
						GlobalVariable.ShaderType = IEffect::FRAGMENT_SHADER;
						GlobalVariable.GlobalType = pBlendConstant->Type;
						GlobalVariable.uDetail    = pBlendConstant->uDetail;
						GlobalVariable.uCount     = pBlendConstant->uCount;
						GlobalVariable.uIndex     = pBlendConstant->uIndex;
						GlobalVariable.pValue     = pBlendConstant->pValue;

						puConstants[i] = m_Parent.m_GlobalVariableHandleMap[GlobalVariable];
					}
					else
					{
						GlobalVariable.ShaderType = IEffect::FRAGMENT_SHADER;
						GlobalVariable.GlobalType = pBlendConstant->Type;
						GlobalVariable.uDetail    = pBlendConstant->uDetail;
						GlobalVariable.uIndex     = pBlendConstant->uIndex;
						GlobalVariable.uCount     = pBlendConstant->uCount;
						GlobalVariable.pValue     = pBlendConstant->pValue;

						pVariable = m_ConstantVariableMap.Get(GlobalVariable);

						if(!pVariable)
						{
							GlobalVariable.ShaderType = IEffect::VERTEX_SHADER;

							pVariable = m_ConstantVariableMap.Get(GlobalVariable);

							if(pVariable)
								pElement = m_ConstantVaryingMap.Get(GlobalVariable);
							else
								pElement = ALCHEMY_NULL;

							if(!pElement)
							{
								GlobalVariable.ShaderType = IEffect::FRAGMENT_SHADER;
								pVariable = ALCHEMY_NULL;
							}
						}

						if(!pVariable)
						{
							m_ConstantVariableMap.Insert(GlobalVariable, Variable);
							pVariable = m_ConstantVariableMap.Get(GlobalVariable);

							pVariable->uHandle = 0;
							pVariable->uSource = 0;
							pVariable->bIsUsed = false;
						}

						if(!pVariable->uHandle)
						{
							if(!pVariable->uSource)
							{
								/*GlobalVariableKey.ShaderType = GlobalVariable.ShaderType;
								GlobalVariableKey.GlobalType = GlobalVariable.GlobalType;
								GlobalVariableKey.uDetail    = GlobalVariable.uDetail;
								GlobalVariableKey.uCount     = GlobalVariable.uCount;
								GlobalVariableKey.pValue     = GlobalVariable.pValue;
								uDestinationCount            = GlobalVariable.uIndex + 1;

								puValue = m_Parent.m_GlobalVariableCountMap.Get(GlobalVariableKey);
								uCurrentCount = 0;
								if(!puValue || !(uCurrentCount = *puValue) || uCurrentCount < uDestinationCount)
									for(j = uCurrentCount; j < uDestinationCount; ++ j)
										m_Parent.AddGlobalVariable(GlobalVariableKey);*/

								pVariable->uSource = m_Parent.AddGlobalVariable(GlobalVariable);/*m_Parent.m_GlobalVariableHandleMap[GlobalVariable];*/
							}

							pVariable->uHandle = pVariable->uSource;
						}
						else
						{
							uHandle = pVariable->uHandle;

							if(GlobalVariable.ShaderType == IEffect::VERTEX_SHADER)
							{
								pVaryingTemp = m_VaryingMap.Get(*pElement);

								if(pVaryingTemp)
								{
									pVariable = m_AttributeShaderVariableMap.Get(*pElement);

									if(pVariable)
									{
										if(!pVaryingTemp->IsUsed)
										{
											m_VertexShaderFunctionDefiner.AppendInstruction(ICompiler::MOV, pVaryingTemp->uVertexShaderHandle, ALCHEMY_NULL, 0, uHandle, ALCHEMY_NULL, 0, 0, ALCHEMY_NULL, 0);

											pVaryingTemp->IsUsed = true;
										}
									}
									else
									{
										Variable.bIsUsed = false;
										Variable.uSource = pVaryingTemp->uFragmentShaderHandle;
										Variable.uHandle = pVaryingTemp->uFragmentShaderHandle;

										pVariable = &Variable;
									}
								}
								else
								{
									uAddress = m_VaryingAddressMap[*pElement];

									Varying.uVertexShaderHandle   = m_Parent.GetAddressVariable(uVertexShaderVarying, uAddress);
									Varying.uFragmentShaderHandle = m_Parent.GetAddressVariable(uFragmentShaderVarying, uAddress);
									Varying.IsUsed                = true;

									m_VaryingMap.Insert(*pElement, Varying);

									m_VertexShaderFunctionDefiner.AppendInstruction(ICompiler::MOV, Varying.uVertexShaderHandle, ALCHEMY_NULL, 0, uHandle, ALCHEMY_NULL, 0, 0, ALCHEMY_NULL, 0);

									pVaryingTemp = &Varying;

									Variable.bIsUsed = false;
									Variable.uSource = pVaryingTemp->uFragmentShaderHandle;
									Variable.uHandle = pVaryingTemp->uFragmentShaderHandle;

									pVariable = &Variable;
								}

								puValue = m_ConstantVariableMaxInfoMap.Get(GlobalVariable);
								if(puValue && m_uVariableInfoCount == *puValue)
								{
									TemporaryDecalaration.ShaderType   = GlobalVariable.ShaderType;
									TemporaryDecalaration.VariableType = CEffectCode::GetGlobalVariableType(GlobalVariable.GlobalType, GlobalVariable.uDetail);
									TemporaryDecalaration.uCount       = 1;

									__PushFreeTemporary(TemporaryDecalaration, uHandle, true);
								}

								GlobalVariable.ShaderType = IEffect::FRAGMENT_SHADER;

								m_ConstantVariableMap.Insert(GlobalVariable, *pVariable);
							}
							else
							{
								puValue = m_ConstantVariableMaxInfoMap.Get(GlobalVariable);
								if(puValue && m_uVariableInfoCount == *puValue)
								{
									TemporaryDecalaration.ShaderType   = GlobalVariable.ShaderType;
									TemporaryDecalaration.VariableType = CEffectCode::GetGlobalVariableType(GlobalVariable.GlobalType, GlobalVariable.uDetail);
									TemporaryDecalaration.uCount       = 1;

									__PushFreeTemporary(TemporaryDecalaration, uHandle, true);
								}
							}
						}

						puConstants[i] = pVariable->uHandle;
					}

					++ pBlendConstant;
				}

				if(bIsInfo)
				{
					uSourceColor = uSourceColor ? uSourceColor : uDestinationColor;
					if( m_uFinalOutputEffectInfoIndex == EffectInfoIterator.GetIndex() )
						pInfo->pBlendMethod->OutputColorBlendHandler(uResultColor, uSourceColor, uSourceColor1, puAttributes, puConstants, puTemporary, m_FragmentShaderFunctionDefiner);
					else if(uOutputColor)
						pInfo->pBlendMethod->OutputColorBlendHandler(uOutputColor, uSourceColor, uSourceColor1, puAttributes, puConstants, puTemporary, m_FragmentShaderFunctionDefiner);
					else
					{
						TemporaryDecalaration.ShaderType = IEffect::FRAGMENT_SHADER;
						TemporaryDecalaration.VariableType = VECTOR4_VARIABLE;
						TemporaryDecalaration.uCount       = 1;
						uSourceColor2 = __PopFreeTemporary(TemporaryDecalaration);

						pInfo->pBlendMethod->OutputColorBlendHandler(uSourceColor2, uSourceColor, uSourceColor1, puAttributes, puConstants, puTemporary, m_FragmentShaderFunctionDefiner);

						if(uDestinationColor != uResultColor)
							__PushFreeTemporary(TemporaryDecalaration, uDestinationColor, false);

						uDestinationColor = uSourceColor2;
					}
					__PushColorKey(pInfo->uInputColorKey);
					__PushFreeTemporary(TemporaryDecalaration, uSourceColor1, false);
				}
				else if(uSourceColor)
				{
					uSourceDestinationColor = uSourceDestinationColor ? uSourceDestinationColor : uDestinationColor;
					if( m_uFinalOutputEffectInfoIndex == EffectInfoIterator.GetIndex() )
						pInfo->pBlendMethod->OutputColorBlendHandler(uResultColor, uSourceDestinationColor, uSourceColor, puAttributes, puConstants, puTemporary, m_FragmentShaderFunctionDefiner);
					else if(uOutputColor)
						pInfo->pBlendMethod->OutputColorBlendHandler(uOutputColor, uSourceDestinationColor, uSourceColor, puAttributes, puConstants, puTemporary, m_FragmentShaderFunctionDefiner);
					else
					{
						TemporaryDecalaration.ShaderType = IEffect::FRAGMENT_SHADER;
						TemporaryDecalaration.VariableType = VECTOR4_VARIABLE;
						TemporaryDecalaration.uCount       = 1;
						uSourceColor2 = __PopFreeTemporary(TemporaryDecalaration);

						pInfo->pBlendMethod->OutputColorBlendHandler(uSourceColor2, uSourceDestinationColor, uSourceColor, puAttributes, puConstants, puTemporary, m_FragmentShaderFunctionDefiner);

						if(uDestinationColor != uResultColor)
							__PushFreeTemporary(TemporaryDecalaration, uDestinationColor, false);

						uDestinationColor = uSourceColor2;
					}
					__PushColorKey(pInfo->uInputColorKey);
				}
			}
			else if(pInfo->pEffectInfo)
			{
				if(!pInfo->uOutputColorKey && bIsInfo && m_bIsBlend)
				{
					TemporaryDecalaration.ShaderType = IEffect::FRAGMENT_SHADER;
					TemporaryDecalaration.VariableType = VECTOR4_VARIABLE;
					TemporaryDecalaration.uCount = 1;
					uDestinationColor = __PopFreeTemporary(TemporaryDecalaration);
				}

				pInfo->pEffectInfo->ShaderDefineHandler( 
					pInfo->uOutputColorKey ? __GetColorValue(pInfo->uOutputColorKey, uResultVariable) : uDestinationColor, 
					puSamplers, 
					puAttributes, 
					puConstants, 
					m_pVaryingBuffer, 
					puTemporary, 
					pAttributeVariables, 
					pConstantVariables, 
					m_VertexShaderFunctionDefiner, 
					m_FragmentShaderFunctionDefiner, 
					m_EffectCodeUtils );

				bIsBlend = true;
			}

			if(pInfo->pBlendMethod)
				++ m_uVariableInfoCount;
		}

		++ EffectInfoIterator;
	}

	//m_VertexShaderFunctionDefiner.EndFunction(uVertexShaderVarying);
	//m_FragmentShaderFunctionDefiner.EndFunction(uDestinationColor);

	m_AttributeAddressMap.Clear();
	m_VaryingAddressMap.Clear();

	m_AttributeMap.Clear();
	m_VaryingMap.Clear();

	//m_TemporaryCountMap.Clear();
	//m_TemporaryHandleMap.Clear();

	m_AttributeVariableMap.Clear();
	m_ConstantVariableMap.Clear();

	m_AttributeVariableMaxInfoMap.Clear();
	m_ConstantVariableMaxInfoMap.Clear();

	m_AttributeShaderVariableMap.Clear();

	m_AttributeVaryingMap.Clear();
	m_ConstantVaryingMap.Clear();

	m_FreeTemporaryMap.Clear();

	m_FreeTemporaryCache.Clear();

	m_EffectInfos.Clear();

	m_ColorKeyPool.Clear();

	m_uAttributeAddress = 0;
	m_uVaryingAddress   = 0;

	m_uVariableInfoCount = 0;

	m_uFinalOutputEffectInfoIndex = 0;
	m_uConstantVaryingCount = 0;

	m_bIsBlend = false;

	//PASS Pass;

	//Pass.uVertexShader   = m_VertexShaderFunctionDefiner.EndDefine();
	//Pass.uFragmentShader = m_FragmentShaderFunctionDefiner.EndDefine();

	return m_Parent.AddPass( m_VertexShaderFunctionDefiner.EndDefine(), m_FragmentShaderFunctionDefiner.EndDefine() );//m_Parent.m_Passes.Add(Pass) + 1;
}

UINT CEffectCode::CPassDefiner::__PopFreeTemporary(const CEffectCode::TEMPORARYDECLARATION& TemporaryDeclaration)
{
	CList<FREETEMPORARY>* pFreeTemporary = m_FreeTemporaryMap.Get(TemporaryDeclaration);
	if(pFreeTemporary)
	{
		pFreeTemporary->Sort<FREETEMPORARYLESS>();

		FREETEMPORARY FreeTemporary;
		if( pFreeTemporary->Shift(&FreeTemporary) )
			return FreeTemporary.uHandle;
	}

	if(TemporaryDeclaration.ShaderType == IEffect::VERTEX_SHADER)
		return m_VertexShaderFunctionDefiner.ApplyVariable(TemporaryDeclaration.VariableType, TemporaryDeclaration.uCount);
	else if(TemporaryDeclaration.ShaderType == IEffect::FRAGMENT_SHADER)
		return m_FragmentShaderFunctionDefiner.ApplyVariable(TemporaryDeclaration.VariableType, TemporaryDeclaration.uCount);

	/*CEffectCode::TEMPORARYDECLARATION Temp = TemporaryDeclaration;
	if(Temp.VariableType == VECTOR1_VARIABLE)
	{
		Temp.VariableType = VECTOR2_VARIABLE;

		pHandles = m_FreeTemporaryHandleMap.Get(Temp);
		if(pHandles)
		{
			pHandles->Sort();

			UINT uHandle = 0;
			if( pHandles->Shift(&uHandle) )
				return uHandle;
		}
	}

	if(Temp.VariableType == VECTOR2_VARIABLE)
	{
		Temp.VariableType = VECTOR3_VARIABLE;

		pHandles = m_FreeTemporaryHandleMap.Get(Temp);
		if(pHandles)
		{
			pHandles->Sort();

			UINT uHandle = 0;
			if( pHandles->Shift(&uHandle) )
				return uHandle;
		}
	}

	if(Temp.VariableType == VECTOR3_VARIABLE)
	{
		Temp.VariableType = VECTOR4_VARIABLE;

		pHandles = m_FreeTemporaryHandleMap.Get(Temp);
		if(pHandles)
		{
			pHandles->Sort();

			UINT uHandle = 0;
			if( pHandles->Shift(&uHandle) )
				return uHandle;
		}
	}*/

	return 0;
}

void CEffectCode::CPassDefiner::__PushFreeTemporary(const CEffectCode::TEMPORARYDECLARATION& TemporaryDeclaration, UINT uHandle, bool bIsCached)
{
	if(bIsCached)
	{
		TEMPORARY Temporary;
		Temporary.ShaderType   = TemporaryDeclaration.ShaderType;
		Temporary.VariableType = TemporaryDeclaration.VariableType;
		Temporary.uCount       = TemporaryDeclaration.uCount;
		Temporary.uHandle      = uHandle;

		m_FreeTemporaryCache.Add(Temporary);
	}
	else
	{
		CList<FREETEMPORARY>* pFreeTemporary = m_FreeTemporaryMap.Get(TemporaryDeclaration);
		if(!pFreeTemporary)
		{
			CList<FREETEMPORARY> List;

			m_FreeTemporaryMap.Insert(TemporaryDeclaration, List);

			pFreeTemporary = m_FreeTemporaryMap.Get(TemporaryDeclaration);
		}

		if(uHandle)
		{
			LPVARIABLE pVariable = m_Parent.m_VariablePool.Get(uHandle - 1);

			if(pVariable)
			{
				FREETEMPORARY FreeTemporary;
				FreeTemporary.uSortValue = pVariable->uRegister;
				FreeTemporary.uHandle    = uHandle;

				pFreeTemporary->Unshift(FreeTemporary);
			}
		}
	}
}

void CEffectCode::CPassDefiner::__UpdateTemporary()
{
	CVector<TEMPORARY>::CIterator Iteractor = m_FreeTemporaryCache.GetIterator(0);
	TEMPORARYDECLARATION TemporaryDeclaration;
	LPVARIABLE pVariable;
	while( CVector<TEMPORARY>::IsValuable(Iteractor) )
	{
		TemporaryDeclaration.ShaderType   = Iteractor->ShaderType;
		TemporaryDeclaration.VariableType = Iteractor->VariableType;
		TemporaryDeclaration.uCount       = Iteractor->uCount;

		CList<FREETEMPORARY>* pFreeTemporary = m_FreeTemporaryMap.Get(TemporaryDeclaration);
		if(!pFreeTemporary)
		{
			CList<FREETEMPORARY> List;

			m_FreeTemporaryMap.Insert(TemporaryDeclaration, List);

			pFreeTemporary = m_FreeTemporaryMap.Get(TemporaryDeclaration);
		}

		if(Iteractor->uHandle)
		{
			pVariable = m_Parent.m_VariablePool.Get(Iteractor->uHandle - 1);

			if(pVariable)
			{
				FREETEMPORARY FreeTemporary;
				FreeTemporary.uSortValue = pVariable->uRegister;
				FreeTemporary.uHandle    = Iteractor->uHandle;

				pFreeTemporary->Unshift(FreeTemporary);
			}
		}

		++ Iteractor;
	}

	m_FreeTemporaryCache.Clear();
}

void CEffectCode::CPassDefiner::__FreeTemporary()
{
	CMap<TEMPORARYDECLARATION, CList<FREETEMPORARY>, TEMPORARYDECLARATIONLESS, TEMPORARYDECLARATIONEQUAL>::CIterator MapIterator = m_FreeTemporaryMap.GetBeginIterator();
	const TEMPORARYDECLARATION* pTemporaryDeclaration;
	CList<FREETEMPORARY>* pFreeTemporary;
	CList<FREETEMPORARY>::CIterator FreeTemporaryIterator;
	while( CMap<TEMPORARYDECLARATION, CList<FREETEMPORARY>, TEMPORARYDECLARATIONLESS, TEMPORARYDECLARATIONEQUAL>::IsValuable(MapIterator) )
	{
		pTemporaryDeclaration = &MapIterator.GetKey();
		pFreeTemporary = &MapIterator.GetValue();
		FreeTemporaryIterator = pFreeTemporary->GetBeginIterator();

		while( CList<FREETEMPORARY>::IsValuable(FreeTemporaryIterator) )
		{
			if(pTemporaryDeclaration->ShaderType == IEffect::VERTEX_SHADER)
				m_VertexShaderFunctionDefiner.FreeVariable(FreeTemporaryIterator->uHandle);
			else if(pTemporaryDeclaration->ShaderType == IEffect::FRAGMENT_SHADER)
				m_FragmentShaderFunctionDefiner.FreeVariable(FreeTemporaryIterator->uHandle);

			++ FreeTemporaryIterator;
		}

		++ MapIterator;
	}

	m_FreeTemporaryMap.Clear();
}

bool CEffectCode::CPassDefiner::__IsValuableColorKey(UINT uKey)
{
	return uKey && m_ColorKeyPool.GetLength() >= uKey;
}

UINT CEffectCode::CPassDefiner::__GetColorKey(UINT uKey)
{
	if(!uKey)
		return 0;

	LPCOLORKEY pColorKey = m_ColorKeyPool.Get(uKey - 1);
	if(pColorKey && !pColorKey->uResultVariableAddress)
	{
		++ pColorKey->uRef;

		//if(!pColorKey->uVariable)
		//	pColorKey->uVariable  = __PopFreeTemporary(sm_COLOR_KEY_TEMPORARYDECLARATION);

		return uKey;
	}

	return 0;
}

UINT CEffectCode::CPassDefiner::__GetColorValue(UINT uKey, UINT uResultVariable)
{
	if(!uKey)
		return 0;

	LPCOLORKEY pColorKey = m_ColorKeyPool.Get(uKey - 1);
	if(pColorKey)
	{
		if(!pColorKey->uVariable)
		{
			if(pColorKey->uResultVariableAddress)
				pColorKey->uVariable = m_Parent.GetAddressVariable(uResultVariable, pColorKey->uResultVariableAddress);
			else
				pColorKey->uVariable  = __PopFreeTemporary(sm_COLOR_KEY_TEMPORARYDECLARATION);
		}

		return pColorKey->uVariable;
	}

	return 0;
}

bool CEffectCode::CPassDefiner::__PushColorKey(UINT uKey)
{
	if(!uKey)
		return false;

	-- uKey;

	LPCOLORKEY pColorKey = m_ColorKeyPool.Get(uKey);
	if(pColorKey)
	{
		if(!pColorKey->uResultVariableAddress)
		{
			if(!pColorKey->uRef)
				m_ColorKeyPool.Remove(uKey, 1);
			else if( !(-- pColorKey->uRef) )
			{
				if(pColorKey->uVariable)
					__PushFreeTemporary(sm_COLOR_KEY_TEMPORARYDECLARATION, pColorKey->uVariable, false);

				m_ColorKeyPool.Remove(uKey, 1);
			}
		}

		return true;
	}

	return false;
}

CEffectCode::CEffectCode(ICompiler& Compiler) :
m_Compiler(Compiler),
//m_uVertexShaderGlobalRegisterCount(0),
//m_uFragmentShaderGlobalRegisterCount(0),
m_VertexGlobalReigsterApplicant(*this), 
m_FragmentGlobalReigsterApplicant(*this), 
m_uBufferSamplerDeclaration(0),
m_pCurrentGlobalInputHandler(ALCHEMY_NULL)
{
}

CEffectCode::~CEffectCode(void)
{
}

bool CEffectCode::IsElementVariable(VARIABLETYPE VariableType)
{
	switch(VariableType)
	{
	case VECTOR1_VARIABLE:
		return true;
		break;
	case VECTOR2_VARIABLE:
		return true;
		break;
	case VECTOR3_VARIABLE:
		return true;
		break;
	case VECTOR4_VARIABLE:
		return true;
		break;
    default:
        break;
	}

	return false;
}

UINT CEffectCode::BuildGlobalInputCode(VARIABLETYPE VariableType, UINT uRowOffset, UINT uOffset, bool bIsPointer)
{
	UINT uCode = 0;
	//5 Bit
	switch(VariableType)
	{
	case VECTOR1_VARIABLE:
		uCode = 1 ;//(0 << 3) | 0 << 1 | 1

		if(uRowOffset < 1)
			uRowOffset = 1;

		if(uOffset < 1)
			uOffset = 1;
		break;
	case VECTOR2_VARIABLE:
		uCode = 3 ;//(0 << 3) | 1 << 1 | 1

		if(uRowOffset < 1)
			uRowOffset = 1;

		if(uOffset < 2)
			uOffset = 2;
		break;
	case VECTOR3_VARIABLE:
		uCode = 5 ;//(0 << 3) | 1 << 2 | 1

		if(uRowOffset < 1)
			uRowOffset = 1;

		if(uOffset < 3)
			uOffset = 3;
		break;
	case VECTOR4_VARIABLE:
		uCode = 7 ;//(0 << 3) | 1 << 3 | 1

		if(uRowOffset < 1)
			uRowOffset = 1;

		if(uOffset < 4)
			uOffset = 4;
		break;
	case MATRIX2X2_VARIABLE:
		uCode = 11;//(1 << 3) | 1 << 1 | 1

		if(uRowOffset < 2)
			uRowOffset = 2;

		if(uOffset < 4)
			uOffset = 4;
		break;
	case MATRIX3X3_VARIABLE:
		uCode = 21;//(2 << 3) | 1 << 2 | 1

		if(uRowOffset < 3)
			uRowOffset = 3;

		if(uOffset < 9)
			uOffset = 9;
		break;
	case MATRIX4X3_VARIABLE:
		uCode = 29;//(3 << 3) | 1 << 2 | 1

		if(uRowOffset < 3)
			uRowOffset = 3;

		if(uOffset < 16)
			uOffset = 16;
		break;
	case MATRIX4X4_VARIABLE:
		uCode = 31;//(3 << 3) | 1 << 3 | 1

		if(uRowOffset < 4)
			uRowOffset = 4;

		if(uOffset < 16)
			uOffset = 16;
		break;
            
    default:
        break;
	}

	//2 Bit
	if(uRowOffset && uRowOffset < 5)
		uCode |= (uRowOffset - 1) << 5;

	//4 Bit
	if(uOffset && uOffset < 17)
		uCode |= (uOffset - 1) << 7;

	//1 Bit
	if(bIsPointer)
		uCode |= 1 << 11;

	//End: Total 12 Bit
	return uCode;
}

void CEffectCode::GetGlobalInputInfo(UINT uCode, VARIABLETYPE* pVariableType, PUINT puRowOffset, PUINT puOffset, bool* pbIsPointer)
{
	if(pVariableType)
	{
		*pVariableType = UNKNOWN_VARIABLE;

		UINT uVariableTypeCode = uCode & 31;

		if(uVariableTypeCode & 1)
		{
			switch(uCode & 31)
			{
			case 1:
				*pVariableType = VECTOR1_VARIABLE;
				break;
			case 3:
				*pVariableType = VECTOR2_VARIABLE;
				break;
			case 5:
				*pVariableType = VECTOR3_VARIABLE;
				break;
			case 7:
				*pVariableType = VECTOR4_VARIABLE;
				break;
			case 11:
				*pVariableType = MATRIX2X2_VARIABLE;
				break;
			case 21:
				*pVariableType = MATRIX3X3_VARIABLE;
				break;
			case 29:
				*pVariableType = MATRIX4X3_VARIABLE;
				break;
			case 31:
				*pVariableType = MATRIX4X4_VARIABLE;
				break;
			}
		}
	}

	if(puRowOffset)
		*puRowOffset = ( (uCode >> 5) & 3 ) + 1;

	if(puOffset)
		*puOffset = ( (uCode >> 7) & 15 ) + 1;

	if(pbIsPointer)
		*pbIsPointer = ( (uCode >> 11) & 1 ) ? true : false;
}

CEffectCode::VARIABLETYPE CEffectCode::GetGlobalVariableType(GLOBALVARIABLETYPE GlobalVariableType, UINT uDetail)
{
	switch(GlobalVariableType)
	{
	case NORMAL_VARIABLE:
		switch(uDetail)
		{
		case IRenderMethod::POSITION_OFFSET:
			return VECTOR3_VARIABLE;
			break;

		case IRenderMethod::UV_OFFSET:
			return VECTOR2_VARIABLE;
			break;

		case IRenderMethod::ELAPSED_TIME:
			return VECTOR1_VARIABLE;
			break;
		case IRenderMethod::TOTAL_TIME:
			return VECTOR1_VARIABLE;
			break;

		case IRenderMethod::AMBIENT_LIGHT:
			return VECTOR4_VARIABLE;
			break;

		case CRenderMethod::WORLD_MATRIX:
			return MATRIX4X4_VARIABLE;
			break;

		case CRenderMethod::INVERSE_WORLD_MATRIX:
			return MATRIX4X4_VARIABLE;
			break;
		}

		break;

	case ARRAY_VARIABLE:
		switch(uDetail)
		{
		case IRenderMethod::BONE_MATRIX_ARRAY:
			return MATRIX4X3_VARIABLE;
			break;
		case IRenderMethod::INSTANCE_TRANSFORM_ARRAY:
			return VECTOR4_VARIABLE;
			break;
		}
		break;

	/*case MATRIX_VARIABLE:

		switch(uDetail)
		{
		case CRenderMethod::WORLD_MATRIX:
			return MATRIX4X4_VARIABLE;
			break;

		case CRenderMethod::VIEW_MATRIX:
			return MATRIX4X4_VARIABLE;
			break;

		case CRenderMethod::PROJECTION_MATRIX:
			return MATRIX4X4_VARIABLE;
			break;

		case CRenderMethod::WORLD_VIEW_MATRIX:
			return MATRIX4X4_VARIABLE;
			break;

		case CRenderMethod::VIEW_PROJECTION_MATRIX:
			return MATRIX4X4_VARIABLE;
			break;

		case CRenderMethod::WORLD_VIEW_PROJECTION_MATRIX:
			return MATRIX4X4_VARIABLE;
			break;

		case CRenderMethod::INVERSE_WORLD_MATRIX:
			return MATRIX4X4_VARIABLE;
			break;

		case CRenderMethod::INVERSE_VIEW_MATRIX:
			return MATRIX4X4_VARIABLE;
			break;

		case CRenderMethod::INVERSE_PROJECTION_MATRIX:
			return MATRIX4X4_VARIABLE;
			break;
		}

		break;*/
	case CAMERA_VARIABLE:

		switch(uDetail)
		{
		case CRenderMethod::CAMERA_POSITION:
			return VECTOR3_VARIABLE;
			break;

		case CRenderMethod::CAMERA_DIRECTION:
			return VECTOR3_VARIABLE;
			break;

		case CRenderMethod::CAMERA_NEGATE_DIRECTION:
			return VECTOR3_VARIABLE;
			break;

		case CRenderMethod::CAMERA_NEAR:
			return VECTOR1_VARIABLE;
			break;

		case CRenderMethod::CAMERA_FAR:
			return VECTOR1_VARIABLE;
			break;

		case CRenderMethod::CAMERA_DISTANCE:
			return VECTOR1_VARIABLE;
			break;

		case CRenderMethod::CAMERA_LOCAL_POSITION:
			return VECTOR3_VARIABLE;
			break;

		case CRenderMethod::CAMERA_LOCAL_DIRECTION:
			return VECTOR3_VARIABLE;
			break;

		case CRenderMethod::CAMERA_LOCAL_DIRECTION_NEGATE:
			return VECTOR3_VARIABLE;
			break;

		case CRenderMethod::CAMERA_VIEW_MATRIX:
			return MATRIX4X4_VARIABLE;
			break;

		case CRenderMethod::CAMERA_PROJECTION_MATRIX:
			return MATRIX4X4_VARIABLE;
			break;

		case CRenderMethod::CAMERA_WORLD_VIEW_MATRIX:
			return MATRIX4X4_VARIABLE;
			break;

		case CRenderMethod::CAMERA_VIEW_PROJECTION_MATRIX:
			return MATRIX4X4_VARIABLE;
			break;

		case CRenderMethod::CAMERA_WORLD_VIEW_PROJECTION_MATRIX:
			return MATRIX4X4_VARIABLE;
			break;

		case CRenderMethod::CAMERA_INVERSE_VIEW_MATRIX:
			return MATRIX4X4_VARIABLE;
			break;

		case CRenderMethod::CAMERA_INVERSE_PROJECTION_MATRIX:
			return MATRIX4X4_VARIABLE;
			break;
		}

		break;

	case MATERIAL_VARIABLE:
		switch(uDetail)
		{
		case CRenderMethod::MATERIAL_AMBIENT:
			return VECTOR4_VARIABLE;
			break;

		case CRenderMethod::MATERIAL_DIFFUSE:
			return VECTOR4_VARIABLE;
			break;

		case CRenderMethod::MATERIAL_SPECULAR:
			return VECTOR4_VARIABLE;
			break;

		case CRenderMethod::MATERIAL_EMISSIVE:
			return VECTOR4_VARIABLE;
			break;

		case CRenderMethod::MATERIAL_POWER:
			return VECTOR1_VARIABLE;
			break;
		case CRenderMethod::MATERIAL_INVERSE_255_POWER:
			return VECTOR1_VARIABLE;
			break;
		}

	case LIGHT_VARIABLE:
		switch(uDetail)
		{
		case CRenderMethod::LIGHT_AMBIENT:
			return VECTOR4_VARIABLE;
			break;
		case CRenderMethod::LIGHT_DIFFUSE:
			return VECTOR4_VARIABLE;
			break;
		case CRenderMethod::LIGHT_SPECULAR:
			return VECTOR4_VARIABLE;
			break;
		case CRenderMethod::LIGHT_POSITION:
			return VECTOR4_VARIABLE;
			break;
		case CRenderMethod::LIGHT_DIRECTION:
			return VECTOR4_VARIABLE;
			break;
		case CRenderMethod::LIGHT_RANGE:
			return VECTOR1_VARIABLE;
			break;
		case CRenderMethod::LIGHT_FALLOFF:
			return VECTOR1_VARIABLE;
			break;
		case CRenderMethod::LIGHT_ATTENUATION0:
			return VECTOR1_VARIABLE;
			break;
		case CRenderMethod::LIGHT_ATTENUATION1:
			return VECTOR1_VARIABLE;
			break;
		case CRenderMethod::LIGHT_ATTENUATION2:
			return VECTOR1_VARIABLE;
			break;
		case CRenderMethod::LIGHT_THETA:
			return VECTOR1_VARIABLE;
			break;
		case CRenderMethod::LIGHT_PHI:
			return VECTOR1_VARIABLE;
			break;
		case CRenderMethod::LIGHT_COS_THETA:
			return VECTOR1_VARIABLE;
			break;
		case CRenderMethod::LIGHT_INVERSE_COS_PHI_SUBSTRUCT_COS_THETA:
			return VECTOR1_VARIABLE;
			break;
		case CRenderMethod::LIGHT_LOCAL_POSITION:
			return VECTOR3_VARIABLE;
			break;
		case CRenderMethod::LIGHT_LOCAL_DIRECTION:
			return VECTOR3_VARIABLE;
			break;
		case CRenderMethod::LIGHT_VIEW_POSITION:
			return VECTOR3_VARIABLE;
			break;
		case CRenderMethod::LIGHT_VIEW_DIRECTION:
			return VECTOR3_VARIABLE;
			break;
		case CRenderMethod::LIGHT_LOCAL_DIRECTION_NEGATE:
			return VECTOR3_VARIABLE;
			break;
		case CRenderMethod::LIGHT_VIEW_DIRECTION_NEGATE:
			return VECTOR3_VARIABLE;
			break;
		/*case CRenderMethod::LIGHT_WORLD_VIEW_PROJECT_MATRIX:
			return MATRIX4X4_VARIABLE;
			break;*/
		}

		break;

	case CONSTANT_VARIABLE:
		switch(uDetail)
		{
		case CRenderMethod::CONSTANT_ZERO:
			return VECTOR1_VARIABLE;
			break;
		case CRenderMethod::CONSTANT_ONE:
			return VECTOR1_VARIABLE;
		case CRenderMethod::CONSTANT_HALF_ONE:
			return VECTOR1_VARIABLE;
			break;
		case CRenderMethod::CONSTANT_TWO:
			return VECTOR1_VARIABLE;
			break;
		case CRenderMethod::CONSTANT_FOUR:
			return VECTOR1_VARIABLE;
			break;
		case CRenderMethod::CONSTANT_255:
			return VECTOR1_VARIABLE;
			break;
		case CRenderMethod::CONSTANT_INVERSE_255:
			return VECTOR1_VARIABLE;
			break;
		case CRenderMethod::CONSTANT_FLOAT_TO_RGBA_CODE:
			return VECTOR4_VARIABLE;
			break;
		case CRenderMethod::CONSTANT_RGBA_TO_FLOAT_CODE:
			return VECTOR4_VARIABLE;
			break;
		}

	case TEXTURE_VARIABLE:
		switch(uDetail)
		{
		case CRenderMethod::TEXTURE_WIDTH:
			return VECTOR1_VARIABLE;
			break;
		case CRenderMethod::TEXTURE_HEIGHT:
			return VECTOR1_VARIABLE;
			break;
		}
		break;

	case INPUT_VARIABLE:
		VARIABLETYPE VariableType;
		GetGlobalInputInfo(uDetail, &VariableType);
		return VariableType;
		break;
	
	case BUFFER_VARIABLE:
		return VECTOR2_VARIABLE;
		break;
	}

	return UNKNOWN_VARIABLE;
}

const CEffectCode::VARIABLE          * CEffectCode::GetVariable          (UINT uHandle)const
{
	if(uHandle)
		return m_VariablePool.Get(uHandle - 1);

	return ALCHEMY_NULL;
}

const CEffectCode::GLOBALVARIABLE    * CEffectCode::GetGlobalVariable    (UINT uHandle)const
{
	if(uHandle)
		return m_GlobalVariablePool.Get(uHandle - 1);

	return ALCHEMY_NULL;
}

const CEffectCode::USERVARIABLE * CEffectCode::GetUserVariable           (UINT uHandle)const
{
	if(uHandle)
		return m_UserVariablePool.Get(uHandle - 1);

	return ALCHEMY_NULL;
}

const UINT                           * CEffectCode::GetUserVariables     (UINT uHandle)const
{
	if(uHandle)
		return m_UserVariablesPool.Get(uHandle - 1);

	return ALCHEMY_NULL;
}

const IEffect::SAMPLERDECLARATION    * CEffectCode::GetSamplerDeclaration(UINT uHandle)const
{
	if(uHandle)
		return m_SamplerDeclarationPool.Get(uHandle - 1);

	return ALCHEMY_NULL;
}

const CEffectCode::STRUCTDECLARATION * CEffectCode::GetStructDeclaration (UINT uHandle)const
{
	if(uHandle)
		return m_StructDeclarationPool.Get(uHandle - 1);

	return ALCHEMY_NULL;
}

const UINT                           * CEffectCode::GetStruct            (UINT uHandle)const
{
	if(uHandle)
		return m_StructPool.Get(uHandle - 1);

	return ALCHEMY_NULL;
}

const STRING                         * CEffectCode::GetCode              (UINT uHandle)const
{
	if(uHandle)
		return m_CodePool.Get(uHandle - 1);

	return ALCHEMY_NULL;
}

const CEffectCode::RENDERTARGETCODE  * CEffectCode::GetRenderTargetCode  (UINT uHandle)const
{
	if(uHandle)
		return m_RenderTargetCodePool.Get(uHandle - 1);

	return ALCHEMY_NULL;
}

const UINT                           * CEffectCode::GetRenderTargetCodes (UINT uHandle)const
{
	if(uHandle)
		return m_RenderTargetCodesPool.Get(uHandle - 1);

	return ALCHEMY_NULL;
}

const CEffectCode::FUNCTION          * CEffectCode::GetFunction          (UINT uHandle)const
{
	if(uHandle)
		return m_FunctionPool.Get(uHandle - 1);

	return ALCHEMY_NULL;
}

const CEffectCode::SHADERPARAMETER   * CEffectCode::GetShaderParameter   (UINT uHandle)const
{
	if(uHandle)
		return m_ShaderParameterPool.Get(uHandle - 1);

	return ALCHEMY_NULL;
}

const UINT                           * CEffectCode::GetShaderParameters  (UINT uHandle)const
{
	if(uHandle)
		return m_ShaderParametersPool.Get(uHandle - 1);

	return ALCHEMY_NULL;
}

const CEffectCode::SHADERFUNCTION    * CEffectCode::GetShaderFunction    (UINT uHandle)const
{
	if(uHandle)
		return m_ShaderFunctionPool.Get(uHandle - 1);

	return ALCHEMY_NULL;
}

const CEffectCode::PASS              * CEffectCode::GetPass              (UINT uHandle)const
{
	if(uHandle)
		return m_Passes.Get(uHandle - 1);

	return ALCHEMY_NULL;
}

UINT CEffectCode::GetTemporaryVariableCount(UINT uTemporary)const
{
	const UINT* puCount = m_TemporaryVariableCountMap.Get(uTemporary);
	if(puCount)
		return *puCount;
		
	return 0;
}

ICompiler::REGISTERTYPE CEffectCode::GetRegisterType(UINT uVariable)const
{
	const VARIABLE* pVariable;
	if(!uVariable || ( pVariable = m_VariablePool.Get(uVariable - 1) ) == ALCHEMY_NULL)
		return ICompiler::NONE;

	if(pVariable->Type == SAMPLER_VARIABLE)
		return ICompiler::SAMPLER;

	const FUNCTION* pFunction;
	if(pVariable->uFunction && ( pFunction = m_FunctionPool.Get(pVariable->uFunction - 1) ) != ALCHEMY_NULL)
	{
		if(pFunction->uParametersVariable == uVariable)
			return ICompiler::PARAMETER;
		else if(pFunction->uResultVariable == uVariable)
			return pVariable->uGlobal ? ICompiler::OUTPUT : ICompiler::RESULT;
		else if(pVariable->uParent)
		{
			const VARIABLE* pTemp = pVariable;
			UINT uParent, uGlobal = 0;

			 while(pTemp && pTemp->uParent)
			 {
				 uParent = pTemp->uParent;
				 uGlobal += pTemp->uGlobal;

				 pTemp = m_VariablePool.Get(pTemp->uParent - 1);
			 }

			if(uParent == pFunction->uParametersVariable)
				return ICompiler::PARAMETER;
			if(uParent == pFunction->uResultVariable)
			{
				const SHADERFUNCTION* pShaderFunction;
				if(pFunction->uShader && ( pShaderFunction = m_ShaderFunctionPool.Get(pFunction->uShader - 1) ) != ALCHEMY_NULL)
				{
					if(pShaderFunction->Type == IEffect::VERTEX_SHADER)
					{
						const UINT* puShaderParameter = pTemp->uGlobal ? m_ShaderParametersPool.Get(pTemp->uGlobal - 1) : ALCHEMY_NULL;
						if(puShaderParameter && *puShaderParameter)
						{
							const SHADERPARAMETER* pShaderParameter = m_ShaderParameterPool.Get(*puShaderParameter - 1);
							while(pShaderParameter && uGlobal --)
								pShaderParameter = pShaderParameter->uNext ? m_ShaderParameterPool.Get(pShaderParameter->uNext - 1) : ALCHEMY_NULL;

							if(pShaderParameter && pShaderParameter->Usage == CVertexDeclaration::POSITIONT )
								return ICompiler::OUTPUT;
						}
					}
					else if(pShaderFunction->Type == IEffect::FRAGMENT_SHADER)
						return ICompiler::OUTPUT;
				}

				return ICompiler::RESULT;
			}
		}

		return ICompiler::TEMPORARY;
	}

	return ICompiler::GLOBAL;
}

UINT CEffectCode::GetRegisterCount(VARIABLETYPE Type, UINT uDeclaration, bool bIsByteAlignment)const
{
	switch(Type)
	{
	case VECTOR1_VARIABLE:
	case VECTOR2_VARIABLE:
	case VECTOR3_VARIABLE:
	case VECTOR4_VARIABLE:

		if(bIsByteAlignment)
		{
			UINT uByteAlignmentShift, uByteAlignment, uCount;

			uByteAlignmentShift = m_Compiler.GetCompilerInfo().uByteAlignmentShift;
			uByteAlignment = 1 << uByteAlignmentShift;
			uCount = Type;

			return uCount > uByteAlignment ?  ( ( (uCount >> uByteAlignmentShift) + 1 ) << uByteAlignmentShift ) : uByteAlignment;
		}
		else
			return Type;

		break;

	case MATRIX2X2_VARIABLE:

		return GetRegisterCount(VECTOR2_VARIABLE) * 2;

		break;

	case MATRIX3X3_VARIABLE:

		return GetRegisterCount(VECTOR3_VARIABLE) * 3;

		break;

	case MATRIX4X3_VARIABLE:

		return GetRegisterCount(VECTOR4_VARIABLE) * 3;

		break;

	case MATRIX4X4_VARIABLE:

		return GetRegisterCount(VECTOR4_VARIABLE) * 4;

		break;

	case SAMPLER_VARIABLE:

		return 0;

		break;

	case STRUCT_VARIABLE:

		const UINT* puStruct;
		UINT uStructDeclaration;
		if( !uDeclaration || !( puStruct = m_StructPool.Get(uDeclaration - 1) ) || !(uStructDeclaration = *puStruct) )
		{
			ALCHEMY_DEBUG_WARNING("CEffectCode::AddAddressVariable: Error struct variable.");

			return 0;
		}

		const STRUCTDECLARATION* pStructDeclaration = m_StructDeclarationPool.Get(uStructDeclaration - 1);

		UINT uRegisterCount = 0;

		while(pStructDeclaration)
		{
			uRegisterCount += GetRegisterCount(pStructDeclaration->Type, pStructDeclaration->uStruct);

			pStructDeclaration = pStructDeclaration->uNext ? m_StructDeclarationPool.Get(pStructDeclaration->uNext - 1) : 0;
		}

		return uRegisterCount;

		break;
	}

	return 0;
}

void CEffectCode::Clear()
{
	m_VariablePool.Clear();
	m_GlobalVariablePool.Clear();

	m_UserVariablePool.Clear();
	m_UserVariablesPool.Clear();

	m_SamplerDeclarationPool.Clear();

	m_StructDeclarationPool.Clear();
	m_StructPool.Clear();

	m_CodePool.Clear();

	m_InstructionPool.Clear();
	m_InstructionSetPool.Clear();

	m_RenderTargetCodePool.Clear();
	m_RenderTargetCodesPool.Clear();

	m_FunctionPool.Clear();

	m_ShaderParameterPool.Clear();
	m_ShaderParametersPool.Clear();

	m_ShaderFunctionPool.Clear();

	m_BufferPool.Clear();

	m_Passes.Clear();
	
	m_CommonSamplers.Clear();

	//m_GlobalVariableCountMap.Clear();
	
	m_BufferSamplerMap.Clear();

	m_GlobalVariableHandleMap.Clear();

	m_AddressVariableHandleMap.Clear();
	
	m_TemporaryVariableCountMap.Clear();
	
	m_VertexGlobalReigsterApplicant.Clear();
	m_FragmentGlobalReigsterApplicant.Clear();

	//m_uVertexShaderGlobalRegisterCount = 0;
	//m_uFragmentShaderGlobalRegisterCount = 0;
	m_uBufferSamplerDeclaration = 0;
}

UINT CEffectCode::GetAddressVariable(UINT uSource, UINT uAddress, PUINT puRegisterCount)
{
	ADDRESSVARIABLE AddressVariable;
	AddressVariable.uParent  = uSource;
	AddressVariable.uAddress = uAddress;

	PUINT puHandle = m_AddressVariableHandleMap.Get(AddressVariable);
	if(puHandle)
		return *puHandle;

	LPVARIABLE pVariable;
	if( !uSource || !( pVariable = m_VariablePool.Get(uSource - 1) ) )
	{
		ALCHEMY_DEBUG_WARNING("CEffectCode::AddAddressVariable: Error source.");

		return 0;
	}

	VARIABLE Variable;
	Variable.uFunction = pVariable->uFunction;
	Variable.uParent   = uSource;

	switch(pVariable->Type)
	{
	case VECTOR1_VARIABLE:
		ALCHEMY_DEBUG_WARNING("CEffectCode::AddAddressVariable: Vector1 can not be address.");

		return 0;

		break;

	case VECTOR2_VARIABLE:
		if(uAddress > 1)
		{
			ALCHEMY_DEBUG_WARNING("CEffectCode::AddAddressVariable: Address out of range.");

			return 0;
		}

		Variable.Type         = VECTOR1_VARIABLE;
		Variable.uRegister    = uAddress << ALCHEMY_CONSTANT_SHIFT(VECTOR1_VARIABLE);
		Variable.uDeclaration = 0;
		Variable.uGlobal      = uAddress;

		break;

	case VECTOR3_VARIABLE:
		if(uAddress > 2)
		{
			ALCHEMY_DEBUG_WARNING("CEffectCode::AddAddressVariable: Address out of range.");

			return 0;
		}

		Variable.Type         = VECTOR1_VARIABLE;
		Variable.uRegister    = uAddress << ALCHEMY_CONSTANT_SHIFT(VECTOR1_VARIABLE);
		Variable.uDeclaration = 0;
		Variable.uGlobal      = uAddress;

		break;

	case VECTOR4_VARIABLE:
		if(uAddress > 3)
		{
			ALCHEMY_DEBUG_WARNING("CEffectCode::AddAddressVariable: Address out of range.");

			return 0;
		}

		Variable.Type         = VECTOR1_VARIABLE;
		Variable.uRegister    = uAddress << ALCHEMY_CONSTANT_SHIFT(VECTOR1_VARIABLE);
		Variable.uDeclaration = 0;
		Variable.uGlobal      = uAddress;

		break;

	case MATRIX2X2_VARIABLE:
		if(uAddress > 1)
		{
			ALCHEMY_DEBUG_WARNING("CEffectCode::AddAddressVariable: Address out of range.");

			return 0;
		}

		Variable.Type         = VECTOR2_VARIABLE;
		Variable.uRegister    = uAddress * GetRegisterCount(VECTOR2_VARIABLE, 0);
		Variable.uDeclaration = 0;
		Variable.uGlobal      = uAddress;

		break;

	case MATRIX3X3_VARIABLE:
		if(uAddress > 2)
		{
			ALCHEMY_DEBUG_WARNING("CEffectCode::AddAddressVariable: Address out of range.");

			return 0;
		}

		Variable.Type         = VECTOR3_VARIABLE;
		Variable.uRegister    = uAddress * GetRegisterCount(VECTOR3_VARIABLE, 0);
		Variable.uDeclaration = 0;
		Variable.uGlobal      = uAddress;

		break;

	case MATRIX4X3_VARIABLE:
		if(uAddress > 2)
		{
			ALCHEMY_DEBUG_WARNING("CEffectCode::AddAddressVariable: Address out of range.");

			return 0;
		}

		Variable.Type         = VECTOR4_VARIABLE;
		Variable.uRegister    = uAddress * GetRegisterCount(VECTOR4_VARIABLE, 0);
		Variable.uDeclaration = 0;
		Variable.uGlobal      = uAddress;

		break;

	case MATRIX4X4_VARIABLE:
		if(uAddress > 3)
		{
			ALCHEMY_DEBUG_WARNING("CEffectCode::AddAddressVariable: Address out of range.");

			return 0;
		}

		Variable.Type         = VECTOR4_VARIABLE;
		Variable.uRegister    = uAddress * GetRegisterCount(VECTOR4_VARIABLE, 0);
		Variable.uDeclaration = 0;
		Variable.uGlobal      = uAddress;

		break;

	case SAMPLER_VARIABLE:
		ALCHEMY_DEBUG_WARNING("CEffectCode::AddAddressVariable: Sampler can not be address.");

		return 0;

		break;

	case STRUCT_VARIABLE:
		PUINT puStruct;
		UINT uStructDeclaration;
		if( !pVariable->uDeclaration || !( puStruct = m_StructPool.Get(pVariable->uDeclaration - 1) ) || !(uStructDeclaration = *puStruct) )
		{
			ALCHEMY_DEBUG_WARNING("CEffectCode::AddAddressVariable: Error struct variable.");

			return 0;
		}

		//LPSTRUCTDECLARATION pStructDeclaration = m_StructDeclarationPool.Get(uStructDeclaration - 1);

		bool bIsVertexShaderOutput = false;
		UINT uShaderParameter = 0, uShaderParameterCount;

		LPFUNCTION pFunction;
		LPSHADERFUNCTION pShaderFunction;
		LPSHADERPARAMETER pShaderParameter;

		if( pVariable->uFunction && ( pFunction = m_FunctionPool.Get(pVariable->uFunction - 1) )!= ALCHEMY_NULL)
		{
			if(uSource == pFunction->uResultVariable)
				bIsVertexShaderOutput = true;
			else
			{
				UINT uParent = pVariable->uParent;
				LPVARIABLE pPrevious = pVariable, pTemp = pPrevious->uParent ? m_VariablePool.Get(pPrevious->uParent - 1) : ALCHEMY_NULL;;

				while(pTemp)
				{
					pPrevious = pTemp;

					pTemp = pPrevious->uParent ? m_VariablePool.Get(pTemp->uParent - 1) : ALCHEMY_NULL;

					if(pTemp)
						uParent = pPrevious->uParent;
				}

				if(uParent == pFunction->uResultVariable)
					bIsVertexShaderOutput = true;
			}

			PUINT puShaderParameters;
			if(pFunction->uShader && ( pShaderFunction = m_ShaderFunctionPool.Get(pFunction->uShader - 1) )!= ALCHEMY_NULL)
			{
				if(pVariable->uParent)
				{
					LPVARIABLE pTopVariable = m_VariablePool.Get(pVariable->uParent - 1);
					uShaderParameterCount = pVariable->uGlobal;
					while(pTopVariable && pTopVariable->uParent)
					{
						uShaderParameterCount += pTopVariable->uGlobal;
						pTopVariable           = m_VariablePool.Get(pVariable->uParent - 1);
					}

					if(pTopVariable->uGlobal && 
						( puShaderParameters = m_ShaderParametersPool.Get(pTopVariable->uGlobal - 1) )!= ALCHEMY_NULL)
					{
						uShaderParameter = *puShaderParameters;
						pShaderParameter = uShaderParameter ? m_ShaderParameterPool.Get(uShaderParameter - 1) : ALCHEMY_NULL;
						while(uShaderParameterCount -- && pShaderParameter)
						{
							uShaderParameter = pShaderParameter->uNext;
							pShaderParameter = uShaderParameter ? m_ShaderParameterPool.Get(uShaderParameter - 1) : ALCHEMY_NULL;
						}

						if(!pShaderParameter)
							uShaderParameter = 0;
					}
				}
				else if(pVariable->uGlobal && 
					( puShaderParameters = m_ShaderParametersPool.Get(pVariable->uGlobal - 1) )!= ALCHEMY_NULL)
					uShaderParameter = *puShaderParameters;
			}
		}

		LPSTRUCTDECLARATION pStructDeclaration = ALCHEMY_NULL;

		UINT uAddressCount = uAddress + 1, uCurrentShaderParamer = 0, uRegister = 0, uPreviousRegister = 0, uCurrentRegister, uIndex, uHandle, i;

		uShaderParameterCount = 0;
		for(i = 0; i < uAddressCount; ++ i)
		{
			uRegister += uPreviousRegister;

			if(!uStructDeclaration)
			{
				//ALCHEMY_DEBUG_WARNING("CEffectCode::AddAddressVariable: Address out of range.");
				if(puRegisterCount)
					*puRegisterCount = uRegister;

				return 0;
			}

			pStructDeclaration = m_StructDeclarationPool.Get(uStructDeclaration - 1);

			if(pStructDeclaration)
			{
				if(pStructDeclaration->Type != STRUCT_VARIABLE)
				{
					uPreviousRegister = GetRegisterCount(pStructDeclaration->Type);

					if(uShaderParameter)
					{
						pShaderParameter = m_ShaderParameterPool.Get(uShaderParameter - 1);

						if( (pShaderParameter->Usage == CVertexDeclaration::POSITIONT) && (pShaderFunction->Type == IEffect::FRAGMENT_SHADER || bIsVertexShaderOutput) )
							uPreviousRegister = 0;

						uCurrentShaderParamer = uShaderParameter;
						uShaderParameter      = pShaderParameter ? pShaderParameter->uNext : 0;
					}

					++ uShaderParameterCount;
				}
				else if(pStructDeclaration->uStruct)
				{
					Variable.Type         = STRUCT_VARIABLE;
					Variable.uRegister    = uRegister;
					Variable.uDeclaration = pStructDeclaration->uStruct;
					Variable.uGlobal      = uShaderParameterCount ? (uShaderParameterCount - 1) : 0;

					uIndex = m_VariablePool.Add(Variable);

					uHandle = GetAddressVariable(uIndex + 1, uAddressCount - i, &uCurrentRegister);

					m_VariablePool.Remove(uIndex, 1);

					if(uHandle)
					{
						LPVARIABLE pCurrentVariable = m_VariablePool.Get(uHandle - 1);

						if(pCurrentVariable)
						{
							pCurrentVariable->uParent = uSource;

							m_AddressVariableHandleMap.Insert(AddressVariable, uHandle);

							if(puRegisterCount)
								*puRegisterCount = uCurrentRegister + uRegister;

							return uHandle;
						}
					}
					else
					{
						uPreviousRegister = 0;
						while(uPreviousRegister < uCurrentRegister)
						{
							uCurrentShaderParamer = uShaderParameter;
							uPreviousRegister    += GetRegisterCount(static_cast<VARIABLETYPE>(pShaderParameter->Type), 0);
							uShaderParameter      = pShaderParameter ? pShaderParameter->uNext : 0;

							++ uShaderParameterCount;
						}
					}
				}

				uStructDeclaration = pStructDeclaration->uNext;
			}
			else
				uStructDeclaration = 0;
		}

		if(pStructDeclaration)
		{
			Variable.Type         = pStructDeclaration->Type;
			Variable.uRegister    = uRegister;
			Variable.uDeclaration = pStructDeclaration->uStruct;
			Variable.uGlobal      = uShaderParameterCount ? (uShaderParameterCount - 1) : 0;
		}

		break;
	}

	if(puRegisterCount)
		*puRegisterCount = Variable.uRegister;

	UINT uHandle = m_VariablePool.Add(Variable) + 1;
	m_AddressVariableHandleMap.Insert(AddressVariable, uHandle);

	return uHandle;
}

UINT CEffectCode::AddGlobalVariable(const GLOBALVARIABLE& GlobalVariable)
{
	VARIABLETYPE VariableType = GetGlobalVariableType(GlobalVariable.GlobalType, GlobalVariable.uDetail);
	//VARIABLETYPE VariableType = GetGlobalVariableType(GlobalVariableKey.GlobalType, GlobalVariableKey.uDetail);
	if(VariableType == UNKNOWN_VARIABLE)
		return 0;

	/*PUINT puCount = m_GlobalVariableCountMap.Get(GlobalVariableKey);
	UINT uIndex;
	if(puCount)
		uIndex = (*puCount) ++;
	else
	{
		m_GlobalVariableCountMap.Insert(GlobalVariableKey, 1);
		uIndex = 0;
	}

	GLOBALVARIABLE GlobalVariable;
	GlobalVariable.ShaderType = GlobalVariableKey.ShaderType;
	GlobalVariable.GlobalType = GlobalVariableKey.GlobalType;
	GlobalVariable.uDetail    = GlobalVariableKey.uDetail;
	GlobalVariable.uCount     = GlobalVariableKey.uCount;
	GlobalVariable.uIndex     = uIndex;
	GlobalVariable.pValue     = GlobalVariableKey.pValue;*/
	PUINT puHandle = m_GlobalVariableHandleMap.Get(GlobalVariable);
	if(puHandle && *puHandle)
		return *puHandle;

	VARIABLE Variable;
	Variable.Type         = VariableType;
	Variable.uDeclaration = 0;
	Variable.uFunction    = 0;
	Variable.uGlobal      = m_GlobalVariablePool.Add(GlobalVariable) + 1;
	Variable.uParent      = 0;

	if(GlobalVariable.ShaderType == IEffect::VERTEX_SHADER)
	{
		Variable.uRegister                  = m_VertexGlobalReigsterApplicant.Malloc(VariableType, 0, GlobalVariable.uCount);//m_uVertexShaderGlobalRegisterCount;
		//m_uVertexShaderGlobalRegisterCount += GlobalVariable/*GlobalVariableKey*/.uCount * GetRegisterCount(VariableType, 0);
	}
	else if(GlobalVariable.ShaderType == IEffect::FRAGMENT_SHADER)
	{
		Variable.uRegister                    = m_FragmentGlobalReigsterApplicant.Malloc(VariableType, 0, GlobalVariable.uCount);//m_uFragmentShaderGlobalRegisterCount;
		//m_uFragmentShaderGlobalRegisterCount += GlobalVariable/*GlobalVariableKey*/.uCount * GetRegisterCount(VariableType, 0);
	}

	UINT uHandle = m_VariablePool.Add(Variable) + 1;

	m_GlobalVariableHandleMap.Insert(GlobalVariable, uHandle);

	return uHandle;
}

//UINT CEffectCode::AddSamplerVariable(const SAMPLERDECLARATION& Sampler, UINT uIndex)
//{
//	VARIABLE Variable;
//	Variable.Type         = SAMPLER_VARIABLE;
//	Variable.uRegister    = uIndex;
//	Variable.uDeclaration = m_SamplerDeclarationPool.Add(Sampler) + 1;
//	Variable.uFunction    = 0;
//	Variable.uGlobal      = 0;
//	Variable.uParent      = 0;
//
//	return m_VariablePool.Add(Variable) + 1;
//}
//#include <stdio.h>
UINT CEffectCode::AddPass(UINT uVertexShaderFunction, UINT uFragmentShaderFunction)
{
	if(!uVertexShaderFunction || !uFragmentShaderFunction)
	{
		ALCHEMY_DEBUG_WARNING("CEffectCode::AddPass: Function handle must not be 0.");

		return 0;
	}

	LPFUNCTION pVertexFunction = m_FunctionPool.Get(uVertexShaderFunction - 1), pFragmentFunction = m_FunctionPool.Get(uFragmentShaderFunction - 1);
	if(!pVertexFunction && pFragmentFunction)
	{
		ALCHEMY_DEBUG_WARNING("CEffectCode::AddPass: Error Function handle.");

		return 0;
	}

	if(!pVertexFunction->uInstructionSet || !pFragmentFunction->uInstructionSet)
	{
		ALCHEMY_DEBUG_WARNING("CEffectCode::AddPass: Invalid function instruction set.");

		return 0;
	}

	PUINT puVertexInstructionSet = m_InstructionSetPool.Get(pVertexFunction->uInstructionSet - 1), puFragmentInstructionSet = m_InstructionSetPool.Get(pFragmentFunction->uInstructionSet - 1);
	if(!*puVertexInstructionSet || !*puFragmentInstructionSet)
	{
		ALCHEMY_DEBUG_WARNING("CEffectCode::AddPass: Invalid function instruction set.");

		return 0;
	}

	if(!pVertexFunction->uCode || !pFragmentFunction->uCode)
	{
		ALCHEMY_DEBUG_WARNING("CEffectCode::AddPass: Invalid function code.");

		return 0;
	}

	STRING* pVertexCode = m_CodePool.Get(pVertexFunction->uCode - 1), * pFragmentCode = m_CodePool.Get(pFragmentFunction->uCode - 1);
	if(!pVertexCode || !pFragmentCode)
	{
		return 0;
	}

	/*wprintf(L"-------------------------Source---------------------------\n");
	wprintf(L"---Vertex Shader---\n");
	wprintf( static_cast<const CHAR*>(*pVertexCode) );
	wprintf(L"---Fragment Shader---\n");
	wprintf( static_cast<const CHAR*>(*pFragmentCode) );*/

	if(!pFragmentFunction->uResultVariable)
	{
		ALCHEMY_DEBUG_WARNING("CEffectCode::AddPass: Invalid fragment function result variable.");

		return 0;
	}

	LPVARIABLE pResultVariable = m_VariablePool.Get(pFragmentFunction->uResultVariable - 1);
	if(!pResultVariable)
	{
		ALCHEMY_DEBUG_WARNING("CEffectCode::AddPass: Invalid fragment function result variable.");

		return 0;
	}

	UINT uResultVariable = pFragmentFunction->uResultVariable;
	if(pResultVariable->Type == STRUCT_VARIABLE)
	{
		uResultVariable = GetAddressVariable(pFragmentFunction->uResultVariable, 0);
		if(!uResultVariable)
		{
			ALCHEMY_DEBUG_WARNING("CEffectCode::AddPass: Invalid fragment function result variable.");

			return 0;
		}
	}

	PASS Pass;
	if(pVertexFunction->uResultVariable && pFragmentFunction->uParametersVariable)
	{
		LPVARIABLE pVertexVariable = m_VariablePool.Get(pVertexFunction->uResultVariable - 1), pFragmentVariable = m_VariablePool.Get(pFragmentFunction->uParametersVariable - 1);
		if(pVertexVariable && pFragmentVariable)
		{
			if(pVertexVariable->Type == STRUCT_VARIABLE && pVertexVariable->uDeclaration && pFragmentVariable->Type == STRUCT_VARIABLE && pFragmentVariable->uDeclaration)
			{
				UINT uVertexVariable = GetAddressVariable(pVertexFunction->uResultVariable, 0), uFragmentVariable = GetAddressVariable(pFragmentFunction->uParametersVariable, 0), uVertexAddress = 1, uFragmentAddress = 1;

				if(uVertexVariable && uFragmentVariable)
				{
					STRING Code;
					CVector<UINT> Registers;
					UINT uUserVariable = 0, uConstantInstruction = 0, uRenderTargetCodeHandle = 0, uRegisterIndex, uRegister, uRegisterCount;
					PUINT puVertexParameters = pVertexVariable->uGlobal ? m_ShaderParametersPool.Get(pVertexVariable->uGlobal - 1) : ALCHEMY_NULL,
						puFragmentParameters = pFragmentVariable->uGlobal ? m_ShaderParametersPool.Get(pFragmentVariable->uGlobal - 1) : ALCHEMY_NULL;
					LPSHADERPARAMETER pVertexParameters = (puVertexParameters && *puVertexParameters) ? m_ShaderParameterPool.Get(*puVertexParameters - 1) : ALCHEMY_NULL, 
						pFragmentParameters = (puFragmentParameters && *puFragmentParameters) ? m_ShaderParameterPool.Get(*puFragmentParameters - 1) : ALCHEMY_NULL, 
						pVertexParameter, 
						pFragmentParameter;

					LPRENDERTARGETCODE pRenderTargetCode = ALCHEMY_NULL;

					RENDERTARGETCODE RenderTargetCode;
					LPUSERVARIABLE pUserVariable;
					bool bIsUpdateMap;

					AppendCodeInstruction(Code, *puFragmentInstructionSet, *pFragmentCode, &uUserVariable, uResultVariable, true, &uConstantInstruction);
					if(Code.GetLength() && uUserVariable)
					{
						/*wprintf(L"-------------------------Result---------------------------\n");
						wprintf(L"---Fragment Shader---\n");
						wprintf( static_cast<const CHAR*>(Code) );*/
						RenderTargetCode.uFragmentCode                   = m_CodePool.Add(Code) + 1;
						RenderTargetCode.uFragmentUserVariables          = uUserVariable ? m_UserVariablesPool.Add(uUserVariable) + 1 : 0;
						RenderTargetCode.uVertexCode                     = 0;
						RenderTargetCode.uVertexUserVariables            = 0;
						RenderTargetCode.uVertexConstantInstructionSet   = 0;
						RenderTargetCode.uFragmentConstantInstructionSet = uConstantInstruction ? (m_InstructionSetPool.Add(uConstantInstruction) + 1) : 0;
						RenderTargetCode.uIndex                          = 0;
						RenderTargetCode.uNext                           = 0;

						bIsUpdateMap = true;
						if(uUserVariable)
						{
							pUserVariable = m_UserVariablePool.Get(uUserVariable - 1);
							while(pUserVariable)
							{
								if(pUserVariable->RegisterType == ICompiler::PARAMETER)
									Registers.Add(pUserVariable->uRegister);

								pUserVariable = pUserVariable->uNext ? m_UserVariablePool.Get(pUserVariable->uNext - 1) : ALCHEMY_NULL;
							}

							uRegisterIndex = 0;
							uRegisterCount = Registers.GetLength();
							if(uRegisterIndex < uRegisterCount)
							{
								Registers.Sort< BASICLESS<UINT> >();
								pFragmentVariable = m_VariablePool.Get(uFragmentVariable - 1);
								uRegister = Registers[0];

								if(pFragmentParameters)
									pFragmentParameter = pFragmentParameters;
								else
									pFragmentParameter = ALCHEMY_NULL;

								while(pFragmentVariable)
								{
									if(pFragmentParameter)
									{
										if(uRegister == pFragmentVariable->uRegister && pFragmentParameter->Usage != CVertexDeclaration::POSITIONT)
											break;

										pFragmentParameter = pFragmentParameter->uNext ? m_ShaderParameterPool.Get(pFragmentParameter->uNext - 1) : ALCHEMY_NULL;
									}
									else if(uRegister == pFragmentVariable->uRegister)
										break;

									uFragmentVariable = GetAddressVariable(pFragmentFunction->uParametersVariable, uFragmentAddress);
									pFragmentVariable = uFragmentVariable ? m_VariablePool.Get(uFragmentVariable - 1) : ALCHEMY_NULL;

									++ uFragmentAddress;
								}

								if(pFragmentVariable)
								{
									Code = ALCHEMY_NULL;
									uUserVariable = 0;
									uConstantInstruction = 0;

									pVertexVariable = m_VariablePool.Get(uVertexVariable - 1);

									if(pVertexParameters)
										pVertexParameter = pVertexParameters;
									else
										pVertexParameter = ALCHEMY_NULL;

									while(pVertexVariable)
									{
										if(pVertexParameter)
										{
											if(pVertexParameter->Usage == CVertexDeclaration::POSITIONT)
											{
												AppendCodeInstruction(Code, *puVertexInstructionSet, *pVertexCode, &uUserVariable, uVertexVariable, bIsUpdateMap, &uConstantInstruction);

												bIsUpdateMap = false;
											}
											else if(pFragmentParameter)
											{
												if(pFragmentParameter->Type == pVertexParameter->Type && pFragmentParameter->Usage == pVertexParameter->Usage && pFragmentParameter->uIndex == pVertexParameter->uIndex)
													break;
											}
											else if(pVertexVariable->Type == pFragmentVariable->Type)
												break;

											pVertexParameter = pVertexParameter->uNext ? m_ShaderParameterPool.Get(pVertexParameter->uNext - 1) : ALCHEMY_NULL;
										}
										else if(pVertexVariable->Type == pFragmentVariable->Type)
											break;

										uVertexVariable = GetAddressVariable(pVertexFunction->uResultVariable, uVertexAddress);
										pVertexVariable = uVertexVariable ? m_VariablePool.Get(uVertexVariable - 1) : ALCHEMY_NULL;

										++ uVertexAddress;
									}

									if(pVertexVariable)
									{
										AppendCodeInstruction(Code, *puVertexInstructionSet, *pVertexCode, &uUserVariable, uVertexVariable, bIsUpdateMap, &uConstantInstruction);

										while(pVertexVariable && pFragmentVariable)
										{
											if(pVertexParameter)
												pVertexParameter = pVertexParameter->uNext ? m_ShaderParameterPool.Get(pVertexParameter->uNext - 1) : ALCHEMY_NULL;

											if(pFragmentParameter)
												pFragmentParameter = pFragmentParameter->uNext ? m_ShaderParameterPool.Get(pFragmentParameter->uNext - 1) : ALCHEMY_NULL;

											uVertexVariable = GetAddressVariable(pVertexFunction->uResultVariable, uVertexAddress);
											uFragmentVariable = GetAddressVariable(pFragmentFunction->uParametersVariable, uFragmentAddress);
											pVertexVariable = uVertexVariable ? m_VariablePool.Get(uVertexVariable - 1) : ALCHEMY_NULL;
											pFragmentVariable = uFragmentVariable ? m_VariablePool.Get(uFragmentVariable - 1) : ALCHEMY_NULL;

											++ uVertexAddress;
											++ uFragmentAddress;

											++ uRegisterIndex;
											if(uRegisterIndex < uRegisterCount)
											{
												uRegister = Registers[uRegisterIndex];

												while(pFragmentVariable)
												{
													if(pFragmentParameter)
													{
														if(uRegister == pFragmentVariable->uRegister && pFragmentParameter->Usage != CVertexDeclaration::POSITIONT)
															break;

														pFragmentParameter = pFragmentParameter->uNext ? m_ShaderParameterPool.Get(pFragmentParameter->uNext - 1) : ALCHEMY_NULL;
													}
													else if(uRegister == pFragmentVariable->uRegister)
														break;

													uFragmentVariable = GetAddressVariable(pFragmentFunction->uParametersVariable, uFragmentAddress);
													pFragmentVariable = uFragmentVariable ? m_VariablePool.Get(uFragmentVariable - 1) : ALCHEMY_NULL;

													++ uFragmentAddress;
												}

												if(!pFragmentVariable)
													break;
											}
											else
												break;

											while(pVertexVariable)
											{
												if(pVertexParameter)
												{
													if(pVertexParameter->Usage == CVertexDeclaration::POSITIONT)
													{
														AppendCodeInstruction(Code, *puVertexInstructionSet, *pVertexCode, &uUserVariable, uVertexVariable, bIsUpdateMap, &uConstantInstruction);

														bIsUpdateMap = false;
													}
													else if(pFragmentParameter)
													{
														if(pFragmentParameter->Type == pVertexParameter->Type && pFragmentParameter->Usage == pVertexParameter->Usage && pFragmentParameter->uIndex == pVertexParameter->uIndex)
															break;
													}
													else if(pVertexVariable->Type == pFragmentVariable->Type)
														break;

													pVertexParameter = pVertexParameter->uNext ? m_ShaderParameterPool.Get(pVertexParameter->uNext - 1) : ALCHEMY_NULL;
												}
												else if(pVertexVariable->Type == pFragmentVariable->Type)
													break;

												uVertexVariable = GetAddressVariable(pVertexFunction->uResultVariable, uVertexAddress);
												pVertexVariable = uVertexVariable ? m_VariablePool.Get(uVertexVariable - 1) : ALCHEMY_NULL;

												++ uVertexAddress;
											}

											if(!pVertexVariable)
												break;

											AppendCodeInstruction(Code, *puVertexInstructionSet, *pVertexCode, &uUserVariable, uVertexVariable, false, &uConstantInstruction);
										}
									}
								}
							}
						}

						if(bIsUpdateMap)
						{
							Code = ALCHEMY_NULL;
							uUserVariable = 0;
							uConstantInstruction = 0;

							pVertexVariable = m_VariablePool.Get(uVertexVariable - 1);

							if(pVertexParameters)
								pVertexParameter = pVertexParameters;
							else
								pVertexParameter = ALCHEMY_NULL;

							while(pVertexVariable && pVertexParameter)
							{
								if(pVertexParameter->Usage == CVertexDeclaration::POSITIONT)
								{
									AppendCodeInstruction(Code, *puVertexInstructionSet, *pVertexCode, &uUserVariable, uVertexVariable, bIsUpdateMap, &uConstantInstruction);

									bIsUpdateMap = false;
								}

								pVertexParameter = pVertexParameter->uNext ? m_ShaderParameterPool.Get(pVertexParameter->uNext - 1) : ALCHEMY_NULL;

								uVertexVariable = GetAddressVariable(pVertexFunction->uResultVariable, uVertexAddress);
								pVertexVariable = uVertexVariable ? m_VariablePool.Get(uVertexVariable - 1) : ALCHEMY_NULL;

								++ uVertexAddress;
							}
						}

						/*wprintf(L"---Vertex Shader---\n");
						wprintf( static_cast<const CHAR*>(Code) );
						wprintf(L"---End Output---\n");*/

						RenderTargetCode.uVertexCode                   = m_CodePool.Add(Code) + 1;
						RenderTargetCode.uVertexUserVariables          = uUserVariable ? (m_UserVariablesPool.Add(uUserVariable) + 1) : 0;

						RenderTargetCode.uVertexConstantInstructionSet = uConstantInstruction ? (m_InstructionSetPool.Add(uConstantInstruction) + 1) : 0;

						uRenderTargetCodeHandle                        = m_RenderTargetCodePool.Add(RenderTargetCode);
						pRenderTargetCode                              = &m_RenderTargetCodePool[uRenderTargetCodeHandle ++];
					}

					UINT uAddress = 1;

					if(uResultVariable != pFragmentFunction->uResultVariable)
					{
						uResultVariable = GetAddressVariable(pFragmentFunction->uResultVariable, uAddress ++);
						while(uResultVariable)
						{
							Code = ALCHEMY_NULL;
							uUserVariable = 0;
							uConstantInstruction = 0;
							AppendCodeInstruction(Code, *puFragmentInstructionSet, *pFragmentCode, &uUserVariable, uResultVariable, true, &uConstantInstruction);
							if(Code.GetLength() && uUserVariable)
							{
								/*wprintf(L"---Fragment Shader---\n");
								wprintf( static_cast<const CHAR*>(Code) );*/
								RenderTargetCode.uFragmentCode                   = m_CodePool.Add(Code) + 1;
								RenderTargetCode.uFragmentUserVariables          = uUserVariable ? m_UserVariablesPool.Add(uUserVariable) + 1 : 0;
								RenderTargetCode.uVertexCode                     = 0;
								RenderTargetCode.uVertexUserVariables            = 0;
								RenderTargetCode.uVertexConstantInstructionSet   = 0;
								RenderTargetCode.uFragmentConstantInstructionSet = uConstantInstruction ? (m_InstructionSetPool.Add(uConstantInstruction) + 1) : 0;
								RenderTargetCode.uIndex                          = uAddress - 1;
								RenderTargetCode.uNext                           = 0;

								bIsUpdateMap = true;
								if(uUserVariable)
								{
									Registers.Reset();

									pUserVariable = m_UserVariablePool.Get(uUserVariable - 1);
									while(pUserVariable)
									{
										if(pUserVariable->RegisterType == ICompiler::PARAMETER)
											Registers.Add(pUserVariable->uRegister);

										pUserVariable = pUserVariable->uNext ? m_UserVariablePool.Get(pUserVariable->uNext - 1) : ALCHEMY_NULL;
									}

									uRegisterIndex = 0;
									uRegisterCount = Registers.GetLength();
									if(uRegisterIndex < uRegisterCount)
									{
										uRegister = Registers[0];
										Registers.Sort< BASICLESS<UINT> >();

										uFragmentAddress = 1;
										uFragmentVariable = GetAddressVariable(pFragmentFunction->uParametersVariable, 0);
										pFragmentVariable = m_VariablePool.Get(uFragmentVariable - 1);

										if(pFragmentParameters)
											pFragmentParameter = pFragmentParameters;
										else
											pFragmentParameter = ALCHEMY_NULL;

										while(pFragmentVariable)
										{
											if(pFragmentParameter)
											{
												if(uRegister == pFragmentVariable->uRegister && pFragmentParameter->Usage != CVertexDeclaration::POSITIONT)
													break;

												pFragmentParameter = pFragmentParameter->uNext ? m_ShaderParameterPool.Get(pFragmentParameter->uNext - 1) : ALCHEMY_NULL;
											}
											else if(uRegister == pFragmentVariable->uRegister)
												break;

											uFragmentVariable = GetAddressVariable(pFragmentFunction->uParametersVariable, uFragmentAddress);
											pFragmentVariable = uFragmentVariable ? m_VariablePool.Get(uFragmentVariable - 1) : ALCHEMY_NULL;

											++ uFragmentAddress;
										}

										if(pFragmentVariable)
										{
											Code = ALCHEMY_NULL;
											uUserVariable = 0;
											uConstantInstruction = 0;

											uVertexAddress = 1;
											uVertexVariable = GetAddressVariable(pVertexFunction->uResultVariable, 0);
											pVertexVariable = m_VariablePool.Get(uVertexVariable - 1);

											if(pVertexParameters)
												pVertexParameter = pVertexParameters;
											else
												pVertexParameter = ALCHEMY_NULL;

											while(pVertexVariable)
											{
												if(pVertexParameter)
												{
													if(pVertexParameter->Usage == CVertexDeclaration::POSITIONT)
													{
														AppendCodeInstruction(Code, *puVertexInstructionSet, *pVertexCode, &uUserVariable, uVertexVariable, bIsUpdateMap, &uConstantInstruction);

														bIsUpdateMap = false;
													}
													else if(pFragmentParameter)
													{
														if(pFragmentParameter->Type == pVertexParameter->Type && pFragmentParameter->Usage == pVertexParameter->Usage && pFragmentParameter->uIndex == pVertexParameter->uIndex)
															break;
													}
													else if(pVertexVariable->Type == pFragmentVariable->Type)
														break;

													pVertexParameter = pVertexParameter->uNext ? m_ShaderParameterPool.Get(pVertexParameter->uNext - 1) : ALCHEMY_NULL;
												}
												else if(pVertexVariable->Type == pFragmentVariable->Type)
													break;

												uVertexVariable = GetAddressVariable(pVertexFunction->uResultVariable, uVertexAddress);
												pVertexVariable = uVertexVariable ? m_VariablePool.Get(uVertexVariable - 1) : ALCHEMY_NULL;

												++ uVertexAddress;
											}

											if(pVertexVariable)
											{
												AppendCodeInstruction(Code, *puVertexInstructionSet, *pVertexCode, &uUserVariable, uVertexVariable, bIsUpdateMap, &uConstantInstruction);

												while(pVertexVariable && pFragmentVariable)
												{
													if(pVertexParameter)
														pVertexParameter = pVertexParameter->uNext ? m_ShaderParameterPool.Get(pVertexParameter->uNext - 1) : ALCHEMY_NULL;

													if(pFragmentParameter)
														pFragmentParameter = pFragmentParameter->uNext ? m_ShaderParameterPool.Get(pFragmentParameter->uNext - 1) : ALCHEMY_NULL;

													uVertexVariable = GetAddressVariable(pVertexFunction->uResultVariable, uVertexAddress);
													uFragmentVariable = GetAddressVariable(pFragmentFunction->uParametersVariable, uFragmentAddress);
													pVertexVariable = uVertexVariable ? m_VariablePool.Get(uVertexVariable - 1) : ALCHEMY_NULL;
													pFragmentVariable = uFragmentVariable ? m_VariablePool.Get(uFragmentVariable - 1) : ALCHEMY_NULL;

													++ uVertexAddress;
													++ uFragmentAddress;

													++ uRegisterIndex;
													if(uRegisterIndex < uRegisterCount)
													{
														uRegister = Registers[uRegisterIndex];
														while(pFragmentVariable)
														{
															if(pFragmentParameter)
															{
																if(uRegister == pFragmentVariable->uRegister && pFragmentParameter->Usage != CVertexDeclaration::POSITIONT)
																	break;

																pFragmentParameter = pFragmentParameter->uNext ? m_ShaderParameterPool.Get(pFragmentParameter->uNext - 1) : ALCHEMY_NULL;
															}
															else if(uRegister == pFragmentVariable->uRegister)
																break;

															uFragmentVariable = GetAddressVariable(pFragmentFunction->uParametersVariable, uFragmentAddress);
															pFragmentVariable = uFragmentVariable ? m_VariablePool.Get(uFragmentVariable - 1) : ALCHEMY_NULL;

															++ uFragmentAddress;
														}

														if(!pFragmentVariable)
															break;
													}
													else
														break;

													while(pVertexVariable)
													{
														if(pVertexParameter)
														{
															if(pVertexParameter->Usage == CVertexDeclaration::POSITIONT)
															{
																AppendCodeInstruction(Code, *puVertexInstructionSet, *pVertexCode, &uUserVariable, uVertexVariable, bIsUpdateMap, &uConstantInstruction);

																bIsUpdateMap = false;
															}
															else if(pFragmentParameter)
															{
																if(pFragmentParameter->Type == pVertexParameter->Type && pFragmentParameter->Usage == pVertexParameter->Usage && pFragmentParameter->uIndex == pVertexParameter->uIndex)
																	break;
															}
															else if(pVertexVariable->Type == pFragmentVariable->Type)
																break;

															pVertexParameter = pVertexParameter->uNext ? m_ShaderParameterPool.Get(pVertexParameter->uNext - 1) : ALCHEMY_NULL;
														}
														else if(pVertexVariable->Type == pFragmentVariable->Type)
															break;

														uVertexVariable = GetAddressVariable(pVertexFunction->uResultVariable, uVertexAddress);
														pVertexVariable = uVertexVariable ? m_VariablePool.Get(uVertexVariable - 1) : ALCHEMY_NULL;

														++ uVertexAddress;
													}

													if(!pVertexVariable)
														break;

													AppendCodeInstruction(Code, *puVertexInstructionSet, *pVertexCode, &uUserVariable, uVertexVariable, false, &uConstantInstruction);
												}
											}

											/*wprintf(L"---Vertex Shader---\n");
											wprintf( static_cast<const CHAR*>(Code) );
											wprintf(L"---End Output---\n");*/
										}
									}
								}

								if(bIsUpdateMap)
								{
									Code = ALCHEMY_NULL;
									uUserVariable = 0;
									uConstantInstruction = 0;

									pVertexVariable = m_VariablePool.Get(uVertexVariable - 1);

									if(pVertexParameters)
										pVertexParameter = pVertexParameters;
									else
										pVertexParameter = ALCHEMY_NULL;

									while(pVertexVariable && pVertexParameter)
									{
										if(pVertexParameter->Usage == CVertexDeclaration::POSITIONT)
										{
											AppendCodeInstruction(Code, *puVertexInstructionSet, *pVertexCode, &uUserVariable, uVertexVariable, bIsUpdateMap, &uConstantInstruction);

											bIsUpdateMap = false;
										}

										pVertexParameter = pVertexParameter->uNext ? m_ShaderParameterPool.Get(pVertexParameter->uNext - 1) : ALCHEMY_NULL;

										uVertexVariable = GetAddressVariable(pVertexFunction->uResultVariable, uVertexAddress);
										pVertexVariable = uVertexVariable ? m_VariablePool.Get(uVertexVariable - 1) : ALCHEMY_NULL;

										++ uVertexAddress;
									}
								}

								RenderTargetCode.uVertexCode                   = m_CodePool.Add(Code) + 1;
								RenderTargetCode.uVertexUserVariables          = uUserVariable ? (m_UserVariablesPool.Add(uUserVariable) + 1) : 0;

								RenderTargetCode.uVertexConstantInstructionSet = uConstantInstruction ? (m_InstructionSetPool.Add(uConstantInstruction) + 1) : 0;

								if(pRenderTargetCode)
									pRenderTargetCode->uNext                   = m_RenderTargetCodePool.Add(RenderTargetCode);

								pRenderTargetCode                              = &m_RenderTargetCodePool[pRenderTargetCode->uNext ++];

								if(!uRenderTargetCodeHandle)
									uRenderTargetCodeHandle = pRenderTargetCode->uNext;
							}

							uResultVariable = GetAddressVariable(pFragmentFunction->uResultVariable, uAddress ++);
						}
					}

					Pass.uRenderTargetCodes = uRenderTargetCodeHandle ? m_RenderTargetCodesPool.Add(uRenderTargetCodeHandle) + 1 : 0;
				}
				else
				{
					ALCHEMY_DEBUG_WARNING("CEffectCode::AddPass: Invalid function result/parameter varialbes.");

					return 0;
				}
			}
			else if(pVertexVariable->Type == pFragmentVariable->Type && pVertexVariable->Type != STRUCT_VARIABLE && !pVertexVariable->uDeclaration && pFragmentVariable->Type != STRUCT_VARIABLE && !pFragmentVariable->uDeclaration)
			{
				STRING Code;
				UINT uUserVariable = 0, uConstantInstruction = 0, uVertexCode, uVertexUserVariables, uVertexConstantInstructionSet;
				RENDERTARGETCODE RenderTargetCode;
				AppendCodeInstruction(Code, *puVertexInstructionSet, *pVertexCode, &uUserVariable, pVertexFunction->uResultVariable, true, &uConstantInstruction);
				uVertexCode                   = m_CodePool.Add(Code) + 1;
				uVertexUserVariables          = uUserVariable ? (m_UserVariablesPool.Add(uUserVariable) + 1) : 0;
				uVertexConstantInstructionSet = uConstantInstruction ? (m_InstructionSetPool.Add(uConstantInstruction) + 1) : 0;

				Code = ALCHEMY_NULL;
				uUserVariable = 0;
				uConstantInstruction = 0;

				LPRENDERTARGETCODE pRenderTargetCode = ALCHEMY_NULL;
				UINT uRenderTargetCodeHandle = 0;
				AppendCodeInstruction(Code, *puFragmentInstructionSet, *pFragmentCode, &uUserVariable, uResultVariable, true, &uConstantInstruction);
				if(Code.GetLength() && uUserVariable)
				{
					RenderTargetCode.uVertexCode                     = uVertexCode;
					RenderTargetCode.uVertexUserVariables            = uVertexUserVariables;
					RenderTargetCode.uFragmentCode                   = m_CodePool.Add(Code) + 1;
					RenderTargetCode.uFragmentUserVariables          = uUserVariable ? (m_UserVariablesPool.Add(uUserVariable) + 1) : 0;
					RenderTargetCode.uVertexConstantInstructionSet   = uVertexConstantInstructionSet;
					RenderTargetCode.uFragmentConstantInstructionSet = uConstantInstruction ? (m_InstructionSetPool.Add(uConstantInstruction) + 1) : 0;
					RenderTargetCode.uIndex                          = 0;
					RenderTargetCode.uNext                           = 0;

					uRenderTargetCodeHandle = m_RenderTargetCodePool.Add(RenderTargetCode);

					pRenderTargetCode = &m_RenderTargetCodePool[uRenderTargetCodeHandle ++];
				}

				if(uResultVariable != pFragmentFunction->uResultVariable)
				{
					UINT uAddress = 1;
					uResultVariable = GetAddressVariable(pFragmentFunction->uResultVariable, uAddress ++);
					while(uResultVariable)
					{
						Code = ALCHEMY_NULL;
						uUserVariable = 0;
						uConstantInstruction = 0;
						AppendCodeInstruction(Code, *puFragmentInstructionSet, *pFragmentCode, &uUserVariable, uResultVariable, true, &uConstantInstruction);
						if(Code.GetLength() && uUserVariable)
						{
							RenderTargetCode.uVertexCode                     = uVertexCode;
							RenderTargetCode.uVertexUserVariables            = uVertexUserVariables;
							RenderTargetCode.uFragmentCode                   = m_CodePool.Add(Code) + 1;
							RenderTargetCode.uFragmentUserVariables          = uUserVariable ? (m_UserVariablesPool.Add(uUserVariable) + 1) : 0;
							RenderTargetCode.uVertexConstantInstructionSet   = uVertexConstantInstructionSet;
							RenderTargetCode.uFragmentConstantInstructionSet = uConstantInstruction ? (m_InstructionSetPool.Add(uConstantInstruction) + 1) : 0;
							RenderTargetCode.uIndex                          = uAddress - 1;
							RenderTargetCode.uNext                           = 0;

							if(pRenderTargetCode)
								pRenderTargetCode->uNext                     = m_RenderTargetCodePool.Add(RenderTargetCode);

							pRenderTargetCode                                = &m_RenderTargetCodePool[pRenderTargetCode->uNext ++];

							if(!uRenderTargetCodeHandle)
								uRenderTargetCodeHandle                      = pRenderTargetCode->uNext;
						}

						uResultVariable = GetAddressVariable(pFragmentFunction->uResultVariable, uAddress ++);
					}
				}

				Pass.uRenderTargetCodes = uRenderTargetCodeHandle ? m_RenderTargetCodesPool.Add(uRenderTargetCodeHandle) + 1 : 0;
			}
			else
			{
				ALCHEMY_DEBUG_WARNING("CEffectCode::AddPass: Invalid function result/parameter varialbes.");

				return 0;
			}
		}
		else
		{
			ALCHEMY_DEBUG_WARNING("CEffectCode::AddPass: Invalid function result/parameter varialbes.");

			return 0;
		}
	}
	else
	{
		ALCHEMY_DEBUG_WARNING("CEffectCode::AddPass: Invalid function result/parameter varialbes.");

		return 0;
	}

	Pass.uVertexShader   = uVertexShaderFunction;
	Pass.uFragmentShader = uFragmentShaderFunction;

	return m_Passes.Add(Pass) + 1;
}

typedef struct
{
	bool bIsInit;
	bool bIsConstantInstruction;
	UINT uConstantInstructionHandle;
	//UINT uConstantInstructionSetHandle;
}INSTRUCTIONSTATE, * LPINSTRUCTIONSTATE;

void CEffectCode::AppendCodeInstruction(STRING& Code, UINT uInstruction, const STRING& Source, PUINT puUserVaraible, UINT uDestination, bool bIsUpdateMap, PUINT puConstantInstructionSet)
{
	if(!uInstruction)
		return;

	LPINSTRUCTION pInstruction = m_InstructionPool.Get(uInstruction - 1);
	if(!pInstruction)
		return;

	UINT uPreviousInstruction = uInstruction;
	if(uDestination)
	{
		while(pInstruction)
		{
			if(uDestination == pInstruction->uDestination)
				break;

			uPreviousInstruction = pInstruction->uPrevious;
			pInstruction = pInstruction->uPrevious ? m_InstructionPool.Get(pInstruction->uPrevious - 1) : ALCHEMY_NULL;
		}

		if(!pInstruction)
			return;

		uInstruction = uPreviousInstruction;
	}

	static UINT s_uByteAlignmentShift = m_Compiler.GetCompilerInfo().uByteAlignmentShift;

	//static CVector<UINT> s_ConstantInstructionSet;
	static CMap<UINT, INSTRUCTIONSTATE> s_InstructionStateMap;
	static CMap<UINT, UINT> s_UserVariableMap;
	static CMap<UINT, UINT> s_ConstantAddressMap;

	static GLOBALVARIABLE s_GlobalVariable;

	static bool s_bIsUpdateInstructionMap = false;
	static UINT s_uConstantInstruction = 0;

	if(bIsUpdateMap)
	{
		s_uByteAlignmentShift = m_Compiler.GetCompilerInfo().uByteAlignmentShift;

		//if(puConstantInstructionSet)
		//	s_ConstantInstructionSet.Clear();

		s_InstructionStateMap.Clear();
		s_UserVariableMap.Clear();
		s_ConstantAddressMap.Clear();

		s_GlobalVariable.GlobalType = INPUT_VARIABLE;
		s_GlobalVariable.uCount     = 1;
		s_GlobalVariable.uIndex     = 0;
		s_GlobalVariable.pValue     = ALCHEMY_NULL;
	}

	if(uDestination || bIsUpdateMap)
	{
		if(uDestination)
			AppendCodeInstruction(Code, pInstruction->uPrevious, Source, puUserVaraible, uDestination, false, puConstantInstructionSet);

		s_bIsUpdateInstructionMap = true;
		AppendCodeInstruction(Code, uInstruction, Source, ALCHEMY_NULL, 0, false, puConstantInstructionSet);
		s_bIsUpdateInstructionMap = false;
	}

	LPINSTRUCTIONSTATE pInstructionState = ALCHEMY_NULL;
	if(!bIsUpdateMap)
	{
		pInstructionState = s_InstructionStateMap.Get(uInstruction);
		if(pInstructionState/* && *pbIsValuableInstruction*/)
		{
			if(pInstructionState->bIsInit || s_bIsUpdateInstructionMap)
			{
				s_uConstantInstruction = pInstructionState->bIsInit ? pInstructionState->uConstantInstructionHandle : (pInstructionState->bIsConstantInstruction ? 1 : 0);

				return;
			}
		}
		else if(!s_bIsUpdateInstructionMap)
		{
			s_uConstantInstruction = 0;

			return;
		}
	}

	static CSmartPointer<CBuffer> s_BufferPointer = ALCHEMY_NULL;

	ICompiler::REGISTERTYPE RegisterType;
	LPINSTRUCTION pCurrentInstruction = pInstruction, pConstantInstruction;
	bool bIsConstantInstruction = pInstructionState ? pInstructionState->bIsConstantInstruction : !bIsUpdateMap;
	UINT uConstantInstructionHandle, uConstant, uAddressShift, uRegister, uAddress, uAddressCount, uCurrentAddress, uDestinationIndexConstant = 0, uSourceConstant1 = 0, uSourceAddress1 = 0, uSourceAddressInfo1 = 0, uSourceIndexConstant1 = 0, uSourceConstant2 = 0, uSourceAddress2 = 0, uSourceAddressInfo2 = 0, uSourceIndexConstant2 = 0, x = 0;
	PUINT puDestinationAddress, puSourceAddress, puAddress;
	CSmartPointer<CBuffer>* pBufferPointer;
	LPVARIABLE pVariable;
	VARIABLETYPE ConstantVariableType;
	USERVARIABLE UserVariable;
	IEffect::EFFECTTYPE ShaderType;
	LPFUNCTION pFunction;
	LPSHADERFUNCTION pShaderFunction;
	const UINT* puCount;

	pVariable = pCurrentInstruction->uDestinationIndex ? m_VariablePool.Get(pCurrentInstruction->uDestinationIndex - 1) : ALCHEMY_NULL;
	if(pVariable)
	{
		uConstantInstructionHandle = 0;

		uRegister = pVariable->uRegister >> s_uByteAlignmentShift;
		RegisterType = GetRegisterType(pCurrentInstruction->uDestinationIndex);
		uPreviousInstruction = pCurrentInstruction->uPrevious;
		pInstruction = pCurrentInstruction->uPrevious ? m_InstructionPool.Get(pCurrentInstruction->uPrevious - 1) : ALCHEMY_NULL;
		while(pInstruction)
		{
			if(pInstruction->uDestination)
			{
				pVariable = m_VariablePool.Get(pInstruction->uDestination - 1);
				if(pVariable)
				{
					if( ( (pVariable->uRegister >> s_uByteAlignmentShift) == uRegister && GetRegisterType(pInstruction->uDestination) == RegisterType && ALCHEMY_TEST_BIT(pInstruction->uDestinationAddress, pCurrentInstruction->uDestinationIndexAddress) ) )
					{
						AppendCodeInstruction(Code, uPreviousInstruction, Source, puUserVaraible, 0, false, puConstantInstructionSet);

						if(puConstantInstructionSet)
						{
							if(s_bIsUpdateInstructionMap)
							{
								uConstantInstructionHandle = s_uConstantInstruction;
								if(!uConstantInstructionHandle)
									bIsConstantInstruction = false;
							}
							else
							{
								uConstantInstructionHandle = s_uConstantInstruction;
								if(uConstantInstructionHandle && RegisterType != ICompiler::GLOBAL)
								{
									pConstantInstruction = m_InstructionPool.Get(uConstantInstructionHandle - 1);
									if(pConstantInstruction)
									{
										if(!bIsConstantInstruction)
										{
											if(pConstantInstruction->uDestination == pInstruction->uDestination)
											{
												ShaderType = IEffect::PROGRAM;
												if(pVariable->uFunction)
												{
													pFunction = m_FunctionPool.Get(pVariable->uFunction - 1);
													if(pFunction && pFunction->uShader)
													{
														pShaderFunction = m_ShaderFunctionPool.Get(pFunction->uShader - 1);
														if(pShaderFunction)
															ShaderType = pShaderFunction->Type;
													}
												}

												uCurrentAddress = ALCHEMY_MININUM_BIT(pInstruction->uDestinationAddress);
												if(pConstantInstruction->uDestinationAddressInfo && ( pBufferPointer = m_BufferPool.Get(pConstantInstruction->uDestinationAddressInfo - 1) ) != ALCHEMY_NULL)
													uAddressCount = (*pBufferPointer)->GetBufferSize() / sizeof(UINT);
												else
													uAddressCount = ALCHEMY_MAXINUM_BIT(pInstruction->uDestinationAddress) - uCurrentAddress + 1;

												ConstantVariableType = (!uAddressCount || uAddressCount > 4) ? pVariable->Type : static_cast<VARIABLETYPE>(sizeof(FLOAT) * uAddressCount);

												if(RegisterType == ICompiler::TEMPORARY)
												{
													puCount = m_TemporaryVariableCountMap.Get(pInstruction->uDestination);
													s_GlobalVariable.uCount = puCount ? *puCount : 1;
												}
												else
													s_GlobalVariable.uCount = 1;

												s_GlobalVariable.ShaderType = ShaderType;
												s_GlobalVariable.uDetail   = BuildGlobalInputCode(ConstantVariableType);
												uConstant = AddGlobalVariable(s_GlobalVariable);
												++ s_GlobalVariable.uIndex;

												uCurrentAddress = pInstruction->uDestinationAddress >> (uCurrentAddress - 1);

												pConstantInstruction->uDestination             = uConstant;
												pConstantInstruction->uDestinationAddress      = uCurrentAddress << m_Compiler.GetAddressShift(uConstant, *this);
												pConstantInstruction->uDestinationAddressInfo  = 0;
												pConstantInstruction->uDestinationIndex        = 0;
												pConstantInstruction->uDestinationIndexAddress = 0;

												s_ConstantAddressMap.Insert(uConstant, uCurrentAddress);

												uDestinationIndexConstant = uConstant;

												if(puUserVaraible)
												{
													pVariable = m_VariablePool.Get(uConstant - 1);

													UserVariable.VariableType = ConstantVariableType;
													UserVariable.RegisterType = ICompiler::GLOBAL;
													UserVariable.uRegister    = pVariable->uRegister;
													UserVariable.uDeclaration = pVariable->uGlobal;
													UserVariable.uIndex       = 0;
													UserVariable.uCount       = s_GlobalVariable.uCount;
													UserVariable.uNext        = *puUserVaraible;

													s_UserVariableMap.Insert(pInstruction->uDestination, *puUserVaraible = m_UserVariablePool.Add(UserVariable) + 1);
												}
											}
											else
												uDestinationIndexConstant = pConstantInstruction->uDestination;
										}
										else
											uDestinationIndexConstant = pConstantInstruction->uDestination;
									}

									//s_uConstantInstruction = 0;
									//*puConstantInstructionSet = 0;
								}
							}
						}

						break;
					}
				}
			}
			else
				AppendCodeInstruction(Code, uPreviousInstruction, Source, puUserVaraible, 0, false, puConstantInstructionSet);

			uPreviousInstruction = pInstruction->uPrevious;
			pInstruction = pInstruction->uPrevious ? m_InstructionPool.Get(pInstruction->uPrevious - 1) : ALCHEMY_NULL;
		}

		if(bIsConstantInstruction && !uConstantInstructionHandle && RegisterType != ICompiler::GLOBAL)
			bIsConstantInstruction = false;
	}

	pVariable = pCurrentInstruction->uSourceIndex1 ? m_VariablePool.Get(pCurrentInstruction->uSourceIndex1 - 1) : ALCHEMY_NULL;
	if(pVariable)
	{
		uConstantInstructionHandle = 0;

		uRegister = pVariable->uRegister >> s_uByteAlignmentShift;
		RegisterType = GetRegisterType(pCurrentInstruction->uSourceIndex1);
		uPreviousInstruction = pCurrentInstruction->uPrevious;
		pInstruction = pCurrentInstruction->uPrevious ? m_InstructionPool.Get(pCurrentInstruction->uPrevious - 1) : ALCHEMY_NULL;
		while(pInstruction)
		{
			if(pInstruction->uDestination)
			{
				pVariable = m_VariablePool.Get(pInstruction->uDestination - 1);
				if(pVariable)
				{
					if( ( (pVariable->uRegister >> s_uByteAlignmentShift) == uRegister && GetRegisterType(pInstruction->uDestination) == RegisterType && ALCHEMY_TEST_BIT(pInstruction->uDestinationAddress, pCurrentInstruction->uSourceIndexAddress1) ) )
					{
						AppendCodeInstruction(Code, uPreviousInstruction, Source, puUserVaraible, 0, false, puConstantInstructionSet);

						if(puConstantInstructionSet)
						{
							if(s_bIsUpdateInstructionMap)
							{
								uConstantInstructionHandle = s_uConstantInstruction;//*puConstantInstructionSet;
								if(!uConstantInstructionHandle)
									bIsConstantInstruction = false;
							}
							else
							{
								uConstantInstructionHandle = s_uConstantInstruction;//*puConstantInstructionSet;
								if(uConstantInstructionHandle && RegisterType != ICompiler::GLOBAL)
								{
									pConstantInstruction = m_InstructionPool.Get(uConstantInstructionHandle - 1);
									if(pConstantInstruction)
									{
										if(!bIsConstantInstruction)
										{
											if(pConstantInstruction->uDestination == pInstruction->uDestination)
											{
												ShaderType = IEffect::PROGRAM;
												if(pVariable->uFunction)
												{
													pFunction = m_FunctionPool.Get(pVariable->uFunction - 1);
													if(pFunction && pFunction->uShader)
													{
														pShaderFunction = m_ShaderFunctionPool.Get(pFunction->uShader - 1);
														if(pShaderFunction)
															ShaderType = pShaderFunction->Type;
													}
												}

												uCurrentAddress = ALCHEMY_MININUM_BIT(pInstruction->uDestinationAddress);
												if(pConstantInstruction->uDestinationAddressInfo && ( pBufferPointer = m_BufferPool.Get(pConstantInstruction->uDestinationAddressInfo - 1) ) != ALCHEMY_NULL)
													uAddressCount = (*pBufferPointer)->GetBufferSize() / sizeof(UINT);
												else
													uAddressCount = ALCHEMY_MAXINUM_BIT(pInstruction->uDestinationAddress) - uCurrentAddress + 1;

												ConstantVariableType = (!uAddressCount || uAddressCount > 4) ? pVariable->Type : static_cast<VARIABLETYPE>(sizeof(FLOAT) * uAddressCount);

												if(RegisterType == ICompiler::TEMPORARY)
												{
													puCount = m_TemporaryVariableCountMap.Get(pInstruction->uDestination);
													s_GlobalVariable.uCount = puCount ? *puCount : 1;
												}
												else
													s_GlobalVariable.uCount = 1;

												s_GlobalVariable.ShaderType = ShaderType;
												s_GlobalVariable.uDetail   = BuildGlobalInputCode(ConstantVariableType);
												uConstant = AddGlobalVariable(s_GlobalVariable);
												++ s_GlobalVariable.uIndex;

												uCurrentAddress = pInstruction->uDestinationAddress >> (uCurrentAddress - 1);

												pConstantInstruction->uDestination             = uConstant;
												pConstantInstruction->uDestinationAddress      = uCurrentAddress << m_Compiler.GetAddressShift(uConstant, *this);
												pConstantInstruction->uDestinationAddressInfo  = 0;
												pConstantInstruction->uDestinationIndex        = 0;
												pConstantInstruction->uDestinationIndexAddress = 0;

												s_ConstantAddressMap.Insert(uConstant, uCurrentAddress);

												uSourceIndexConstant1 = uConstant;

												if(puUserVaraible)
												{
													pVariable = m_VariablePool.Get(uConstant - 1);

													UserVariable.VariableType = ConstantVariableType;
													UserVariable.RegisterType = ICompiler::GLOBAL;
													UserVariable.uRegister    = pVariable->uRegister;
													UserVariable.uDeclaration = pVariable->uGlobal;
													UserVariable.uIndex       = 0;
													UserVariable.uCount       = s_GlobalVariable.uCount;
													UserVariable.uNext        = *puUserVaraible;

													s_UserVariableMap.Insert(pInstruction->uDestination, *puUserVaraible = m_UserVariablePool.Add(UserVariable) + 1);
												}
											}
											else
												uSourceIndexConstant1 = pConstantInstruction->uDestination;
										}
										else
											uSourceIndexConstant1 = pConstantInstruction->uDestination;
									}

									//*puConstantInstructionSet = 0;
								}
							}
						}

						break;
					}
				}
			}
			else
				AppendCodeInstruction(Code, uPreviousInstruction, Source, puUserVaraible, 0, false, puConstantInstructionSet);

			uPreviousInstruction = pInstruction->uPrevious;
			pInstruction = pInstruction->uPrevious ? m_InstructionPool.Get(pInstruction->uPrevious - 1) : ALCHEMY_NULL;
		}

		if(bIsConstantInstruction && !uConstantInstructionHandle && RegisterType != ICompiler::GLOBAL)
			bIsConstantInstruction = false;
	}

	pVariable = pCurrentInstruction->uSource1 ? m_VariablePool.Get(pCurrentInstruction->uSource1 - 1) : ALCHEMY_NULL;
	if(pVariable)
	{
		uConstantInstructionHandle = 0;

		uRegister = pVariable->uRegister >> s_uByteAlignmentShift;
		uAddress  = pCurrentInstruction->uSourceAddress1;
		RegisterType = GetRegisterType(pCurrentInstruction->uSource1);
		uPreviousInstruction = pCurrentInstruction->uPrevious;
		pInstruction = pCurrentInstruction->uPrevious ? m_InstructionPool.Get(pCurrentInstruction->uPrevious - 1) : ALCHEMY_NULL;
		while(pInstruction)
		{
			if(pInstruction->uDestination)
			{
				pVariable = m_VariablePool.Get(pInstruction->uDestination - 1);
				if(pVariable)
				{
					if( ( (pVariable->uRegister >> s_uByteAlignmentShift) == uRegister && GetRegisterType(pInstruction->uDestination) == RegisterType && ALCHEMY_TEST_ANY(pInstruction->uDestinationAddress, uAddress) ) )
					{
						AppendCodeInstruction(Code, uPreviousInstruction, Source, puUserVaraible, 0, false, puConstantInstructionSet);

						if(puConstantInstructionSet)
						{
							if(s_bIsUpdateInstructionMap)
							{
								uConstantInstructionHandle = s_uConstantInstruction;
								if(!uConstantInstructionHandle)
									bIsConstantInstruction = false;
							}
							else
							{
								uConstantInstructionHandle = s_uConstantInstruction;
								if(uConstantInstructionHandle && RegisterType != ICompiler::GLOBAL)
								{
									pConstantInstruction = m_InstructionPool.Get(uConstantInstructionHandle - 1);
									if(pConstantInstruction)
									{
										if(!bIsConstantInstruction)
										{
											if(pConstantInstruction->uDestination == pInstruction->uDestination)
											{
												ShaderType = IEffect::PROGRAM;
												if(pVariable->uFunction)
												{
													pFunction = m_FunctionPool.Get(pVariable->uFunction - 1);
													if(pFunction && pFunction->uShader)
													{
														pShaderFunction = m_ShaderFunctionPool.Get(pFunction->uShader - 1);
														if(pShaderFunction)
															ShaderType = pShaderFunction->Type;
													}
												}

												uCurrentAddress = ALCHEMY_MININUM_BIT(pInstruction->uDestinationAddress);
												if(pConstantInstruction->uDestinationAddressInfo && ( pBufferPointer = m_BufferPool.Get(pConstantInstruction->uDestinationAddressInfo - 1) ) != ALCHEMY_NULL)
													uAddressCount = (*pBufferPointer)->GetBufferSize() / sizeof(UINT);
												else
												{
													uAddressCount = ALCHEMY_MAXINUM_BIT(pInstruction->uDestinationAddress) - uCurrentAddress + 1;

													pBufferPointer = ALCHEMY_NULL;
												}

												ConstantVariableType = (!uAddressCount || uAddressCount > 4) ? pVariable->Type : static_cast<VARIABLETYPE>(sizeof(FLOAT) * uAddressCount);

												if(RegisterType == ICompiler::TEMPORARY)
												{
													puCount = m_TemporaryVariableCountMap.Get(pInstruction->uDestination);
													s_GlobalVariable.uCount = puCount ? *puCount : 1;
												}
												else
													s_GlobalVariable.uCount = 1;

												s_GlobalVariable.ShaderType = ShaderType;
												s_GlobalVariable.uDetail   = BuildGlobalInputCode(ConstantVariableType);
												uConstant = AddGlobalVariable(s_GlobalVariable);
												++ s_GlobalVariable.uIndex;

												uCurrentAddress = pInstruction->uDestinationAddress >> (uCurrentAddress - 1);

												pConstantInstruction->uDestination             = uConstant;
												pConstantInstruction->uDestinationAddress      = uCurrentAddress << ( uAddressShift = m_Compiler.GetAddressShift(uConstant, *this) );
												pConstantInstruction->uDestinationAddressInfo  = 0;
												pConstantInstruction->uDestinationIndex        = 0;
												pConstantInstruction->uDestinationIndexAddress = 0;

												if( uAddress == pCurrentInstruction->uSourceAddress1 && ALCHEMY_TEST_FLAG(pInstruction->uDestinationAddress, uAddress) )
												{
													uSourceConstant1    = uConstant;
													uSourceAddress1     = pConstantInstruction->uDestinationAddress;
													uSourceAddressInfo1 = 0;

													s_ConstantAddressMap.Insert(uConstant, uCurrentAddress);
												}
												else
												{
													if(pInstruction->uDestinationAddress == ~0)
													{
														puDestinationAddress = ALCHEMY_NULL;
														puSourceAddress      = ALCHEMY_NULL;
														uAddressCount        = 0;
													}
													else
													{
														if(!pBufferPointer)
														{
															uAddressCount *= sizeof(UINT) * 2;
															if(s_BufferPointer == ALCHEMY_NULL || s_BufferPointer->GetBufferSize() < uAddressCount)
																s_BufferPointer = &CreateBuffer(uAddressCount, true);

															UnserializeAddress( pInstruction->uDestinationAddress >> m_Compiler.GetAddressShift(pInstruction->uDestination, *this), puDestinationAddress = reinterpret_cast<PUINT>( s_BufferPointer->GetBufferPointer() ), &uAddressCount);

															UnserializeAddress( pConstantInstruction->uDestinationAddress >> uAddressShift, puSourceAddress = puDestinationAddress + uAddressCount);
														}
														else
														{
															puDestinationAddress = reinterpret_cast<PUINT>( (*pBufferPointer)->GetBufferPointer() );

															uAddressCount *= sizeof(UINT);
															if(s_BufferPointer == ALCHEMY_NULL || s_BufferPointer->GetBufferSize() < uAddressCount)
																s_BufferPointer = &CreateBuffer(uAddressCount, true);

															UnserializeAddress( pConstantInstruction->uDestinationAddress >> uAddressShift, puSourceAddress = reinterpret_cast<PUINT>( s_BufferPointer->GetBufferPointer() ), &uAddressCount);
														}
													}

													m_Compiler.AppendInstruction(
														Code, 
														ICompiler::MOV, 
														pInstruction->uDestination, 
														pInstruction->uDestinationIndex, 
														pInstruction->uDestinationIndex ? ( pInstruction->uDestinationIndexAddress >> m_Compiler.GetAddressShift(pInstruction->uDestinationIndex, *this) ) : 0,
														puDestinationAddress, 
														uAddressCount, 
														uConstant, 
														0,
														0, 
														puSourceAddress, 
														uAddressCount, 
														0, 
														0, 
														0, 
														ALCHEMY_NULL, 
														0, 
														*this);
												}

												if(puUserVaraible)
												{
													pVariable = m_VariablePool.Get(uConstant - 1);

													UserVariable.VariableType = ConstantVariableType;
													UserVariable.RegisterType = ICompiler::GLOBAL;
													UserVariable.uRegister    = pVariable->uRegister;
													UserVariable.uDeclaration = pVariable->uGlobal;
													UserVariable.uIndex       = 0;
													UserVariable.uCount       = s_GlobalVariable.uCount;
													UserVariable.uNext        = *puUserVaraible;

													s_UserVariableMap.Insert(pInstruction->uDestination, *puUserVaraible = m_UserVariablePool.Add(UserVariable) + 1);
												}
											}
											else
											{
												if( uAddress == pCurrentInstruction->uSourceAddress1 && ALCHEMY_TEST_FLAG(pInstruction->uDestinationAddress, uAddress) )
												{
													uSourceConstant1    = pConstantInstruction->uDestination;
													uSourceAddress1     = pConstantInstruction->uDestinationAddress;
													uSourceAddressInfo1 = pConstantInstruction->uDestinationAddressInfo;
												}
												else
												{
													uCurrentAddress = uAddress & pInstruction->uDestinationAddress;
													UnserializeAddress(uCurrentAddress, ALCHEMY_NULL, &uAddressCount);
													uAddressCount *= 2 * sizeof(UINT);

													if(s_BufferPointer == ALCHEMY_NULL || s_BufferPointer->GetBufferSize() < uAddressCount)
														s_BufferPointer = &CreateBuffer(uAddressCount, true);

													UnserializeAddress(uCurrentAddress >> m_Compiler.GetAddressShift(pCurrentInstruction->uSource1, *this), puDestinationAddress = reinterpret_cast<PUINT>( s_BufferPointer->GetBufferPointer() ), &uAddressCount);

													//UnserializeAddress( (uCurrentAddress & pConstantInstruction->uDestinationAddress) >> m_Compiler.GetAddressShift(pConstantInstruction->uDestination, *this), puSourceAddress = puDestinationAddress + uAddressCount );

													puSourceAddress = puDestinationAddress + uAddressCount;
													pBufferPointer = pConstantInstruction->uDestinationAddressInfo ? m_BufferPool.Get(pConstantInstruction->uDestinationAddressInfo - 1) : ALCHEMY_NULL;
													if(pBufferPointer)
														UnserializeInstructionAddress(pInstruction->uDestinationAddress, uCurrentAddress, reinterpret_cast<PUINT>( (*pBufferPointer)->GetBufferPointer() ), (*pBufferPointer)->GetBufferSize() / sizeof(UINT), puSourceAddress, &uCurrentAddress);
													/*else if(pConstantInstruction->uDestinationAddress)
													{
														UnserializeAddress(pConstantInstruction->uDestinationAddress >> m_Compiler.GetAddressShift(pConstantInstruction->uDestination, *this), puSourceAddress);
														UnserializeInstructionAddress(pInstruction->uDestinationAddress, uCurrentAddress, puSourceAddress, ALCHEMY_MAXINUM_BIT(pConstantInstruction->uDestinationAddress) - ALCHEMY_MININUM_BIT(pConstantInstruction->uDestinationAddress) + 1, puSourceAddress, &uCurrentAddress);
													}
													else*/
														UnserializeInstructionAddress(pInstruction->uDestinationAddress, uCurrentAddress, ALCHEMY_NULL, 0, puSourceAddress, &uCurrentAddress);

													puAddress = s_ConstantAddressMap.Get(pConstantInstruction->uDestination);
													if( puAddress && ALCHEMY_TEST_ANY(*puAddress, uCurrentAddress) )
													{
														if( ALCHEMY_TEST_FLAG(*puAddress, uCurrentAddress) )
															ALCHEMY_CLEAR_FLAG(*puAddress, uCurrentAddress);
														else
														{
															ALCHEMY_CLEAR_FLAG(*puAddress, uCurrentAddress);
															UnserializeInstructionAddress(uCurrentAddress, *puAddress, puDestinationAddress, uAddressCount, puDestinationAddress);
															UnserializeAddress(*puAddress, puSourceAddress, &uAddressCount);
														}

														if(uSourceIndexConstant1)
															m_Compiler.AppendInstruction(
																Code, 
																ICompiler::MOV, 
																pCurrentInstruction->uSource1, 
																uSourceIndexConstant1, 
																x,
																puDestinationAddress, 
																uAddressCount, 
																pConstantInstruction->uDestination, 
																pConstantInstruction->uDestinationIndex, 
																pConstantInstruction->uDestinationIndex ? ( pConstantInstruction->uDestinationIndexAddress >> m_Compiler.GetAddressShift(pConstantInstruction->uDestinationIndex, *this) ) : 0,
																puSourceAddress, 
																uAddressCount, 
																0, 
																0, 
																0, 
																ALCHEMY_NULL, 
																0, 
																*this);
														else
															m_Compiler.AppendInstruction(
																Code, 
																ICompiler::MOV, 
																pCurrentInstruction->uSource1, 
																pCurrentInstruction->uSourceIndex1, 
																pCurrentInstruction->uSourceIndex1 ? ( pCurrentInstruction->uSourceIndexAddress1 >> m_Compiler.GetAddressShift(pCurrentInstruction->uSourceIndex1, *this) ) : 0,
																puDestinationAddress, 
																uAddressCount, 
																pConstantInstruction->uDestination, 
																pConstantInstruction->uDestinationIndex, 
																pConstantInstruction->uDestinationIndex ? ( pConstantInstruction->uDestinationIndexAddress >> m_Compiler.GetAddressShift(pConstantInstruction->uDestinationIndex, *this) ) : 0,
																puSourceAddress, 
																uAddressCount, 
																0, 
																0, 
																0, 
																ALCHEMY_NULL, 
																0, 
																*this);
													}
												}
											}
										}
										else if( uAddress == pCurrentInstruction->uSourceAddress1 && ALCHEMY_TEST_FLAG(pInstruction->uDestinationAddress, uAddress) )
											uSourceConstant1 = pConstantInstruction->uDestination;
									}

									//*puConstantInstructionSet = 0;
								}
							}
						}

						ALCHEMY_CLEAR_FLAG(uAddress, pInstruction->uDestinationAddress);

						if(!uAddress)
							break;
					}
				}
			}
			else
				AppendCodeInstruction(Code, uPreviousInstruction, Source, puUserVaraible, 0, false, puConstantInstructionSet);

			uPreviousInstruction = pInstruction->uPrevious;
			pInstruction = pInstruction->uPrevious ? m_InstructionPool.Get(pInstruction->uPrevious - 1) : ALCHEMY_NULL;
		}

		if(bIsConstantInstruction && !uConstantInstructionHandle && RegisterType != ICompiler::GLOBAL)
			bIsConstantInstruction = false;
	}

	pVariable = pCurrentInstruction->uSourceIndex2 ? m_VariablePool.Get(pCurrentInstruction->uSourceIndex2 - 1) : ALCHEMY_NULL;
	if(pVariable)
	{
		uConstantInstructionHandle = 0;

		uRegister = pVariable->uRegister >> s_uByteAlignmentShift;
		RegisterType = GetRegisterType(pCurrentInstruction->uSourceIndex2);
		uPreviousInstruction = pCurrentInstruction->uPrevious;
		pInstruction = pCurrentInstruction->uPrevious ? m_InstructionPool.Get(pCurrentInstruction->uPrevious - 1) : ALCHEMY_NULL;
		while(pInstruction)
		{
			if(pInstruction->uDestination)
			{
				pVariable = m_VariablePool.Get(pInstruction->uDestination - 1);
				if(pVariable)
				{
					if( ( (pVariable->uRegister >> s_uByteAlignmentShift) == uRegister && GetRegisterType(pInstruction->uDestination) == RegisterType && ALCHEMY_TEST_BIT(pInstruction->uDestinationAddress, pCurrentInstruction->uSourceIndexAddress2) ) )
					{
						AppendCodeInstruction(Code, uPreviousInstruction, Source, puUserVaraible, 0, false, puConstantInstructionSet);

						if(puConstantInstructionSet)
						{
							if(s_bIsUpdateInstructionMap)
							{
								uConstantInstructionHandle = s_uConstantInstruction;//*puConstantInstructionSet;
								if(!uConstantInstructionHandle)
									bIsConstantInstruction = false;
							}
							else
							{
								uConstantInstructionHandle = s_uConstantInstruction;//*puConstantInstructionSet;
								if(uConstantInstructionHandle && RegisterType != ICompiler::GLOBAL)
								{
									pConstantInstruction = m_InstructionPool.Get(uConstantInstructionHandle - 1);
									if(pConstantInstruction)
									{
										if(!bIsConstantInstruction)
										{
											if(pConstantInstruction->uDestination == pInstruction->uDestination)
											{
												ShaderType = IEffect::PROGRAM;
												if(pVariable->uFunction)
												{
													pFunction = m_FunctionPool.Get(pVariable->uFunction - 1);
													if(pFunction && pFunction->uShader)
													{
														pShaderFunction = m_ShaderFunctionPool.Get(pFunction->uShader - 1);
														if(pShaderFunction)
															ShaderType = pShaderFunction->Type;
													}
												}

												uCurrentAddress = ALCHEMY_MININUM_BIT(pInstruction->uDestinationAddress);
												if(pConstantInstruction->uDestinationAddressInfo && ( pBufferPointer = m_BufferPool.Get(pConstantInstruction->uDestinationAddressInfo - 1) ) != ALCHEMY_NULL)
													uAddressCount = (*pBufferPointer)->GetBufferSize() / sizeof(UINT);
												else
													uAddressCount = ALCHEMY_MAXINUM_BIT(pInstruction->uDestinationAddress) - uCurrentAddress + 1;

												ConstantVariableType = (!uAddressCount || uAddressCount > 4) ? pVariable->Type : static_cast<VARIABLETYPE>(sizeof(FLOAT) * uAddressCount);

												if(RegisterType == ICompiler::TEMPORARY)
												{
													puCount = m_TemporaryVariableCountMap.Get(pInstruction->uDestination);
													s_GlobalVariable.uCount = puCount ? *puCount : 1;
												}
												else
													s_GlobalVariable.uCount = 1;

												s_GlobalVariable.ShaderType = ShaderType;
												s_GlobalVariable.uDetail   = BuildGlobalInputCode(ConstantVariableType);
												uConstant = AddGlobalVariable(s_GlobalVariable);
												++ s_GlobalVariable.uIndex;

												uCurrentAddress = pInstruction->uDestinationAddress >> (uCurrentAddress - 1);

												pConstantInstruction->uDestination             = uConstant;
												pConstantInstruction->uDestinationAddress      = uCurrentAddress << m_Compiler.GetAddressShift(uConstant, *this);
												pConstantInstruction->uDestinationAddressInfo  = 0;
												pConstantInstruction->uDestinationIndex        = 0;
												pConstantInstruction->uDestinationIndexAddress = 0;

												s_ConstantAddressMap.Insert(uConstant, uCurrentAddress);

												uSourceIndexConstant2 = uConstant;

												if(puUserVaraible)
												{
													pVariable = m_VariablePool.Get(uConstant - 1);

													UserVariable.VariableType = ConstantVariableType;
													UserVariable.RegisterType = ICompiler::GLOBAL;
													UserVariable.uRegister    = pVariable->uRegister;
													UserVariable.uDeclaration = pVariable->uGlobal;
													UserVariable.uIndex       = 0;
													UserVariable.uCount       = s_GlobalVariable.uCount;
													UserVariable.uNext        = *puUserVaraible;

													s_UserVariableMap.Insert(pInstruction->uDestination, *puUserVaraible = m_UserVariablePool.Add(UserVariable) + 1);
												}
											}
											else
												uSourceIndexConstant2 = pConstantInstruction->uDestination;
										}
										else
											uSourceIndexConstant2 = pConstantInstruction->uDestination;
									}

									//*puConstantInstructionSet = 0;
								}
							}
						}

						break;
					}
				}
			}
			else
				AppendCodeInstruction(Code, uPreviousInstruction, Source, puUserVaraible, 0, false, puConstantInstructionSet);

			uPreviousInstruction = pInstruction->uPrevious;
			pInstruction = pInstruction->uPrevious ? m_InstructionPool.Get(pInstruction->uPrevious - 1) : ALCHEMY_NULL;
		}

		if(bIsConstantInstruction && !uConstantInstructionHandle && RegisterType != ICompiler::GLOBAL)
			bIsConstantInstruction = false;
	}

	pVariable = pCurrentInstruction->uSource2 ? m_VariablePool.Get(pCurrentInstruction->uSource2 - 1) : ALCHEMY_NULL;
	if(pVariable)
	{
		uConstantInstructionHandle = 0;

		uRegister = pVariable->uRegister >> s_uByteAlignmentShift;
		uAddress  = pCurrentInstruction->uSourceAddress2;
		RegisterType = GetRegisterType(pCurrentInstruction->uSource2);
		uPreviousInstruction = pCurrentInstruction->uPrevious;
		pInstruction = pCurrentInstruction->uPrevious ? m_InstructionPool.Get(pCurrentInstruction->uPrevious - 1) : ALCHEMY_NULL;
		while(pInstruction)
		{
			if(pInstruction->uDestination)
			{
				pVariable = m_VariablePool.Get(pInstruction->uDestination - 1);
				if(pVariable)
				{
					if( ( (pVariable->uRegister >> s_uByteAlignmentShift) == uRegister && GetRegisterType(pInstruction->uDestination) == RegisterType && ALCHEMY_TEST_ANY(pInstruction->uDestinationAddress, uAddress) ) )
					{
						AppendCodeInstruction(Code, uPreviousInstruction, Source, puUserVaraible, 0, false, puConstantInstructionSet);

						if(puConstantInstructionSet)
						{
							if(s_bIsUpdateInstructionMap)
							{
								uConstantInstructionHandle = s_uConstantInstruction;//*puConstantInstructionSet;
								if(!uConstantInstructionHandle)
									bIsConstantInstruction = false;
							}
							else
							{
								uConstantInstructionHandle = s_uConstantInstruction;//*puConstantInstructionSet;
								if(uConstantInstructionHandle && RegisterType != ICompiler::GLOBAL)
								{
									pConstantInstruction = m_InstructionPool.Get(uConstantInstructionHandle - 1);
									if(pConstantInstruction)
									{
										if(!bIsConstantInstruction)
										{
											if(pConstantInstruction->uDestination == pInstruction->uDestination)
											{
												ShaderType = IEffect::PROGRAM;
												if(pVariable->uFunction)
												{
													pFunction = m_FunctionPool.Get(pVariable->uFunction - 1);
													if(pFunction && pFunction->uShader)
													{
														pShaderFunction = m_ShaderFunctionPool.Get(pFunction->uShader - 1);
														if(pShaderFunction)
															ShaderType = pShaderFunction->Type;
													}
												}

												uCurrentAddress = ALCHEMY_MININUM_BIT(pInstruction->uDestinationAddress);
												if(pConstantInstruction->uDestinationAddressInfo && ( pBufferPointer = m_BufferPool.Get(pConstantInstruction->uDestinationAddressInfo - 1) ) != ALCHEMY_NULL)
													uAddressCount = (*pBufferPointer)->GetBufferSize() / sizeof(UINT);
												else
												{
													uAddressCount = ALCHEMY_MAXINUM_BIT(pInstruction->uDestinationAddress) - uCurrentAddress + 1;
													
													pBufferPointer = ALCHEMY_NULL;
												}

												ConstantVariableType = (!uAddressCount || uAddressCount > 4) ? pVariable->Type : static_cast<VARIABLETYPE>(sizeof(FLOAT) * uAddressCount);

												if(RegisterType == ICompiler::TEMPORARY)
												{
													puCount = m_TemporaryVariableCountMap.Get(pInstruction->uDestination);
													s_GlobalVariable.uCount = puCount ? *puCount : 1;
												}
												else
													s_GlobalVariable.uCount = 1;

												s_GlobalVariable.ShaderType = ShaderType;
												s_GlobalVariable.uDetail   = BuildGlobalInputCode(ConstantVariableType);
												uConstant = AddGlobalVariable(s_GlobalVariable);
												++ s_GlobalVariable.uIndex;

												uCurrentAddress = pInstruction->uDestinationAddress >> (uCurrentAddress - 1);

												pConstantInstruction->uDestination             = uConstant;
												pConstantInstruction->uDestinationAddress      = uCurrentAddress << ( uAddressShift = m_Compiler.GetAddressShift(uConstant, *this) );
												pConstantInstruction->uDestinationAddressInfo  = 0;
												pConstantInstruction->uDestinationIndex        = 0;
												pConstantInstruction->uDestinationIndexAddress = 0;

												if( uAddress == pCurrentInstruction->uSourceAddress2 && ALCHEMY_TEST_FLAG(pInstruction->uDestinationAddress, uAddress) )
												{
													uSourceConstant2    = uConstant;
													uSourceAddress2     = pConstantInstruction->uDestinationAddress;
													uSourceAddressInfo2 = 0;

													s_ConstantAddressMap.Insert(uConstant, uCurrentAddress);
												}
												else
												{
													if(pInstruction->uDestinationAddress == ~0)
													{
														puSourceAddress      = ALCHEMY_NULL;
														puDestinationAddress = ALCHEMY_NULL;
														uAddressCount        = 0;
													}
													else
													{
														if(!pBufferPointer)
														{
															uAddressCount *= sizeof(UINT) * 2;
															if(s_BufferPointer == ALCHEMY_NULL || s_BufferPointer->GetBufferSize() < uAddressCount)
																s_BufferPointer = &CreateBuffer(uAddressCount, true);

															UnserializeAddress( pInstruction->uDestinationAddress >> m_Compiler.GetAddressShift(pInstruction->uDestination, *this), puDestinationAddress = reinterpret_cast<PUINT>( s_BufferPointer->GetBufferPointer() ), &uAddressCount);

															UnserializeAddress( pConstantInstruction->uDestinationAddress >> uAddressShift, puSourceAddress = puDestinationAddress + uAddressCount);
														}
														else
														{
															puDestinationAddress = reinterpret_cast<PUINT>( (*pBufferPointer)->GetBufferPointer() );

															uAddressCount *= sizeof(UINT);
															if(s_BufferPointer == ALCHEMY_NULL || s_BufferPointer->GetBufferSize() < uAddressCount)
																s_BufferPointer = &CreateBuffer(uAddressCount, true);

															UnserializeAddress( pConstantInstruction->uDestinationAddress >> uAddressShift, puSourceAddress = reinterpret_cast<PUINT>( s_BufferPointer->GetBufferPointer() ), &uAddressCount);
														}
													}

													m_Compiler.AppendInstruction(
														Code, 
														ICompiler::MOV, 
														pInstruction->uDestination, 
														pInstruction->uDestinationIndex, 
														pInstruction->uDestinationIndex ? ( pInstruction->uDestinationIndexAddress >> m_Compiler.GetAddressShift(pInstruction->uDestinationIndex, *this) ) : 0,
														puDestinationAddress, 
														uAddressCount, 
														uConstant, 
														0,
														0, 
														puSourceAddress, 
														uAddressCount, 
														0, 
														0, 
														0, 
														ALCHEMY_NULL, 
														0, 
														*this);
												}

												if(puUserVaraible)
												{
													pVariable = m_VariablePool.Get(uConstant - 1);

													UserVariable.VariableType = ConstantVariableType;
													UserVariable.RegisterType = ICompiler::GLOBAL;
													UserVariable.uRegister    = pVariable->uRegister;
													UserVariable.uDeclaration = pVariable->uGlobal;
													UserVariable.uIndex       = 0;
													UserVariable.uCount       = s_GlobalVariable.uCount;
													UserVariable.uNext        = *puUserVaraible;

													s_UserVariableMap.Insert(pInstruction->uDestination, *puUserVaraible = m_UserVariablePool.Add(UserVariable) + 1);
												}
											}
											else
											{
												if( uAddress == pCurrentInstruction->uSourceAddress2 && ALCHEMY_TEST_FLAG(pInstruction->uDestinationAddress, uAddress) )
												{
													uSourceConstant2    = pConstantInstruction->uDestination;
													uSourceAddress2     = pConstantInstruction->uDestinationAddress;
													uSourceAddressInfo2 = pConstantInstruction->uDestinationAddressInfo;
												}
												else
												{
													uCurrentAddress = uAddress & pInstruction->uDestinationAddress;
													UnserializeAddress(uCurrentAddress, ALCHEMY_NULL, &uAddressCount);
													uAddressCount *= 2 * sizeof(UINT);

													if(s_BufferPointer == ALCHEMY_NULL || s_BufferPointer->GetBufferSize() < uAddressCount)
														s_BufferPointer = &CreateBuffer(uAddressCount, true);

													UnserializeAddress(uCurrentAddress >> m_Compiler.GetAddressShift(pCurrentInstruction->uSource2, *this), puDestinationAddress = reinterpret_cast<PUINT>( s_BufferPointer->GetBufferPointer() ), &uAddressCount);

													//UnserializeAddress( (uCurrentAddress & pConstantInstruction->uDestinationAddress) >> m_Compiler.GetAddressShift(pConstantInstruction->uDestination, *this), puSourceAddress = puDestinationAddress + uAddressCount );
													puSourceAddress = puDestinationAddress + uAddressCount;
													pBufferPointer = pConstantInstruction->uDestinationAddressInfo ? m_BufferPool.Get(pConstantInstruction->uDestinationAddressInfo - 1) : ALCHEMY_NULL;
													if(pBufferPointer)
														UnserializeInstructionAddress(pInstruction->uDestinationAddress, uCurrentAddress, reinterpret_cast<PUINT>( (*pBufferPointer)->GetBufferPointer() ), (*pBufferPointer)->GetBufferSize() / sizeof(UINT), puSourceAddress, &uCurrentAddress);
													/*else if(pConstantInstruction->uDestinationAddress)
													{
														UnserializeAddress(pConstantInstruction->uDestinationAddress >> m_Compiler.GetAddressShift(pConstantInstruction->uDestination, *this), puSourceAddress);
														UnserializeInstructionAddress(pInstruction->uDestinationAddress, uCurrentAddress, puSourceAddress, ALCHEMY_MAXINUM_BIT(pConstantInstruction->uDestinationAddress) - ALCHEMY_MININUM_BIT(pConstantInstruction->uDestinationAddress) + 1, puSourceAddress, &uCurrentAddress);
													}*/
													else
														UnserializeInstructionAddress(pInstruction->uDestinationAddress, uCurrentAddress, puSourceAddress, 0, puSourceAddress, &uCurrentAddress);

													puAddress = s_ConstantAddressMap.Get(pConstantInstruction->uDestination);
													if( puAddress && ALCHEMY_TEST_ANY(*puAddress, uCurrentAddress) )
													{
														if( ALCHEMY_TEST_FLAG(*puAddress, uCurrentAddress) )
															ALCHEMY_CLEAR_FLAG(*puAddress, uCurrentAddress);
														else
														{
															ALCHEMY_CLEAR_FLAG(*puAddress, uCurrentAddress);
															UnserializeInstructionAddress(uCurrentAddress, *puAddress, puDestinationAddress, uAddressCount, puDestinationAddress);
															UnserializeAddress(*puAddress, puSourceAddress, &uAddressCount);
														}

														if(uSourceIndexConstant2)
															m_Compiler.AppendInstruction(
																Code, 
																ICompiler::MOV, 
																pCurrentInstruction->uSource2, 
																uSourceIndexConstant2, 
																x,
																puDestinationAddress, 
																uAddressCount, 
																pConstantInstruction->uDestination, 
																pConstantInstruction->uDestinationIndex,
																pConstantInstruction->uDestinationIndex ? ( pConstantInstruction->uDestinationIndexAddress >> m_Compiler.GetAddressShift(pConstantInstruction->uDestinationIndex, *this) ) : 0,
																puSourceAddress, 
																uAddressCount, 
																0, 
																0, 
																0, 
																ALCHEMY_NULL, 
																0, 
																*this);
														else
															m_Compiler.AppendInstruction(
																Code, 
																ICompiler::MOV, 
																pCurrentInstruction->uSource2, 
																pCurrentInstruction->uSourceIndex2, 
																pCurrentInstruction->uSourceIndex2 ? ( pCurrentInstruction->uSourceIndexAddress2 >> m_Compiler.GetAddressShift(pCurrentInstruction->uSourceIndex2, *this) ) : 0,
																puDestinationAddress, 
																uAddressCount, 
																pConstantInstruction->uDestination, 
																pConstantInstruction->uDestinationIndex,
																pConstantInstruction->uDestinationIndex ? ( pConstantInstruction->uDestinationIndexAddress >> m_Compiler.GetAddressShift(pConstantInstruction->uDestinationIndex, *this) ) : 0,
																puSourceAddress, 
																uAddressCount, 
																0, 
																0, 
																0, 
																ALCHEMY_NULL, 
																0, 
																*this);
													}
												}
											}
										}
										else if( uAddress == pCurrentInstruction->uSourceAddress2 && ALCHEMY_TEST_FLAG(pInstruction->uDestinationAddress, uAddress) )
											uSourceConstant2 = pConstantInstruction->uDestination;
									}

									//*puConstantInstructionSet = 0;
								}
							}
						}

						ALCHEMY_CLEAR_FLAG(uAddress, pInstruction->uDestinationAddress);

						if(!uAddress)
							break;
					}
				}
			}
			else
				AppendCodeInstruction(Code, uPreviousInstruction, Source, puUserVaraible, 0, false, puConstantInstructionSet);

			uPreviousInstruction = pInstruction->uPrevious;
			pInstruction = pInstruction->uPrevious ? m_InstructionPool.Get(pInstruction->uPrevious - 1) : ALCHEMY_NULL;
		}

		if(bIsConstantInstruction && !uConstantInstructionHandle && RegisterType != ICompiler::GLOBAL)
			bIsConstantInstruction = false;
	}

	if(s_bIsUpdateInstructionMap)
	{
		INSTRUCTIONSTATE InstructionState;
		InstructionState.bIsInit                       = false;
		InstructionState.bIsConstantInstruction        = bIsConstantInstruction;
		InstructionState.uConstantInstructionHandle    = 0;
		//InstructionState.uConstantInstructionSetHandle = 0;
		s_InstructionStateMap.Insert(uInstruction, InstructionState);

		s_uConstantInstruction/**puConstantInstructionSet*/ = bIsConstantInstruction ? 1 : 0;

		return;
	}

	pVariable = pCurrentInstruction->uDestination ? m_VariablePool.Get(pCurrentInstruction->uDestination - 1) : ALCHEMY_NULL;
	if(pVariable)
	{
		LPINSTRUCTIONSTATE pCurrentInstructionState;

		uRegister = pVariable->uRegister >> s_uByteAlignmentShift;
		uAddress  = pCurrentInstruction->uDestinationAddress;
		RegisterType = GetRegisterType(pCurrentInstruction->uDestination);
		uPreviousInstruction = pCurrentInstruction->uPrevious;
		pInstruction = pCurrentInstruction->uPrevious ? m_InstructionPool.Get(pCurrentInstruction->uPrevious - 1) : ALCHEMY_NULL;
		while(pInstruction)
		{
			pCurrentInstructionState = s_InstructionStateMap.Get(uPreviousInstruction);

			if( pCurrentInstructionState && !(pCurrentInstructionState->bIsInit) )
			{
				if(pInstruction->uDestination)
				{
					pVariable = m_VariablePool.Get(pInstruction->uDestination - 1);
					if(pVariable)
					{
						if( ( (pVariable->uRegister >> s_uByteAlignmentShift) == uRegister && GetRegisterType(pInstruction->uDestination) == RegisterType && ALCHEMY_TEST_ANY(pCurrentInstruction->uDestinationAddress, pInstruction->uDestinationAddress) ) )
						{
							AppendCodeInstruction(Code, uPreviousInstruction, Source, puUserVaraible, 0, false, puConstantInstructionSet);
							
							ALCHEMY_CLEAR_FLAG(uAddress, pInstruction->uDestinationAddress);
							
							if(!uAddress)
								break;
						}
					}
				}

				if(pInstruction->uDestinationIndex)
				{
					pVariable = m_VariablePool.Get(pInstruction->uDestinationIndex - 1);
					if(pVariable)
					{
						if( ( (pVariable->uRegister >> s_uByteAlignmentShift) == uRegister && GetRegisterType(pInstruction->uDestinationIndex) == RegisterType && ALCHEMY_TEST_BIT(uAddress, pInstruction->uDestinationIndexAddress) ) )
						{
							AppendCodeInstruction(Code, uPreviousInstruction, Source, puUserVaraible, 0, false, puConstantInstructionSet);

							//ALCHEMY_CLEAR_BIT(uAddress, pInstruction->uDestinationIndexAddress);
						}
					}
				}

				if(pInstruction->uSource1)
				{
					pVariable = m_VariablePool.Get(pInstruction->uSource1 - 1);
					if(pVariable)
					{
						if( ( (pVariable->uRegister >> s_uByteAlignmentShift) == uRegister && GetRegisterType(pInstruction->uSource1) == RegisterType && ALCHEMY_TEST_ANY(pInstruction->uSourceAddress1, uAddress) ) )
						{
							AppendCodeInstruction(Code, uPreviousInstruction, Source, puUserVaraible, 0, false, puConstantInstructionSet);

							//ALCHEMY_CLEAR_FLAG(uAddress, pInstruction->uSourceAddress1);
						}
					}
				}

				if(pInstruction->uSourceIndex1)
				{
					pVariable = m_VariablePool.Get(pInstruction->uSourceIndex1 - 1);
					if(pVariable)
					{
						if( ( (pVariable->uRegister >> s_uByteAlignmentShift) == uRegister && GetRegisterType(pInstruction->uSourceIndex1) == RegisterType && ALCHEMY_TEST_BIT(uAddress, pInstruction->uSourceIndexAddress1) ) )
						{
							AppendCodeInstruction(Code, uPreviousInstruction, Source, puUserVaraible, 0, false, puConstantInstructionSet);

							//ALCHEMY_CLEAR_BIT(uAddress, pInstruction->uSourceIndexAddress1);
						}
					}
				}

				if(pInstruction->uSource2)
				{
					pVariable = m_VariablePool.Get(pInstruction->uSource2 - 1);
					if(pVariable)
					{
						if( ( (pVariable->uRegister >> s_uByteAlignmentShift) == uRegister && GetRegisterType(pInstruction->uSource2) == RegisterType && ALCHEMY_TEST_ANY(pInstruction->uSourceAddress2, uAddress) ) )
						{
							AppendCodeInstruction(Code, uPreviousInstruction, Source, puUserVaraible, 0, false, puConstantInstructionSet);

							//ALCHEMY_CLEAR_FLAG(uAddress, pInstruction->uSourceAddress2);
						}
					}
				}

				if(pInstruction->uSourceIndex2)
				{
					pVariable = m_VariablePool.Get(pInstruction->uSourceIndex2 - 1);
					if(pVariable)
					{
						if( ( (pVariable->uRegister >> s_uByteAlignmentShift) == uRegister && GetRegisterType(pInstruction->uSourceIndex2) == RegisterType && ALCHEMY_TEST_BIT(uAddress, pInstruction->uSourceIndexAddress2) ) )
						{
							AppendCodeInstruction(Code, uPreviousInstruction, Source, puUserVaraible, 0, false, puConstantInstructionSet);

							//ALCHEMY_CLEAR_BIT(uAddress, pInstruction->uSourceIndexAddress2);
						}
					}
				}

				/*if(!uAddress)
					break;*/
			}

			uPreviousInstruction = pInstruction->uPrevious;
			pInstruction = pInstruction->uPrevious ? m_InstructionPool.Get(pInstruction->uPrevious - 1) : ALCHEMY_NULL;
		}
	}

	//if(uDestination)
	//	AppendCodeInstruction(Code, pCurrentInstruction->uPrevious, Source, puUserVaraible, uDestination, false);

	bIsConstantInstruction = puConstantInstructionSet && bIsConstantInstruction;
	if(bIsConstantInstruction)
	{
		RegisterType = GetRegisterType(pCurrentInstruction->uDestination);
		if(RegisterType == ICompiler::RESULT || RegisterType == ICompiler::OUTPUT)
		{
			pVariable = pCurrentInstruction->uDestination ? m_VariablePool.Get(pCurrentInstruction->uDestination - 1) : ALCHEMY_NULL;
			if(!pVariable || pCurrentInstruction->Opcode == ICompiler::MOV)
				bIsConstantInstruction = false;
			else
			{
				ShaderType = IEffect::PROGRAM;
				if(pVariable->uFunction)
				{
					pFunction = m_FunctionPool.Get(pVariable->uFunction - 1);
					if(pFunction && pFunction->uShader)
					{
						pShaderFunction = m_ShaderFunctionPool.Get(pFunction->uShader - 1);
						if(pShaderFunction)
							ShaderType = pShaderFunction->Type;
					}
				}

				uCurrentAddress = ALCHEMY_MININUM_BIT(pCurrentInstruction->uDestinationAddress);
				uAddressCount = ALCHEMY_MAXINUM_BIT(pCurrentInstruction->uDestinationAddress) - uCurrentAddress + 1;

				ConstantVariableType = (!uAddressCount || uAddressCount > 4) ? pVariable->Type : static_cast<VARIABLETYPE>(sizeof(FLOAT) * uAddressCount);

				if(RegisterType == ICompiler::TEMPORARY)
				{
					puCount = m_TemporaryVariableCountMap.Get(pCurrentInstruction->uDestination);
					s_GlobalVariable.uCount = puCount ? *puCount : 1;
				}
				else
					s_GlobalVariable.uCount = 1;

				s_GlobalVariable.ShaderType = ShaderType;
				s_GlobalVariable.uDetail   = BuildGlobalInputCode(ConstantVariableType);
				uConstant = AddGlobalVariable(s_GlobalVariable);
				++ s_GlobalVariable.uIndex;

				if(pCurrentInstruction->uDestinationAddress == ~0)
				{
					puSourceAddress      = ALCHEMY_NULL;
					puDestinationAddress = ALCHEMY_NULL;
					uAddressCount        = 0;
				}
				else
				{
					uAddressCount *= sizeof(UINT) * 2;
					if(s_BufferPointer == ALCHEMY_NULL || s_BufferPointer->GetBufferSize() < uAddressCount)
						s_BufferPointer = &CreateBuffer(uAddressCount, true);

					UnserializeAddress( pCurrentInstruction->uDestinationAddress >> m_Compiler.GetAddressShift(pCurrentInstruction->uDestination, *this), puDestinationAddress = reinterpret_cast<PUINT>( s_BufferPointer->GetBufferPointer() ), &uAddressCount);

					UnserializeAddress(uCurrentAddress = pCurrentInstruction->uDestinationAddress >> (uCurrentAddress - 1), puSourceAddress = puDestinationAddress + uAddressCount);
				}

				m_Compiler.AppendInstruction(
					Code, 
					ICompiler::MOV, 
					pCurrentInstruction->uDestination, 
					pCurrentInstruction->uDestinationIndex, 
					pCurrentInstruction->uDestinationIndexAddress, 
					puDestinationAddress, 
					uAddressCount, 
					uConstant, 
					0,
					0, 
					puSourceAddress, 
					uAddressCount, 
					0, 
					0, 
					0, 
					ALCHEMY_NULL, 
					0, 
					*this);

				if(puUserVaraible)
				{
					pVariable = m_VariablePool.Get(uConstant - 1);

					UserVariable.VariableType = ConstantVariableType;
					UserVariable.RegisterType = ICompiler::GLOBAL;
					UserVariable.uRegister    = pVariable->uRegister;
					UserVariable.uDeclaration = pVariable->uGlobal;
					UserVariable.uIndex       = 0;
					UserVariable.uCount       = s_GlobalVariable.uCount;
					UserVariable.uNext        = *puUserVaraible;

					s_UserVariableMap.Insert(pCurrentInstruction->uDestination, *puUserVaraible = m_UserVariablePool.Add(UserVariable) + 1);
				}
			}
		}
		else
			uConstant = 0;
	}

	if(bIsConstantInstruction)
	{
		INSTRUCTION ConstantInstruction;
		ConstantInstruction           = *pCurrentInstruction;
		ConstantInstruction.uPrevious = *puConstantInstructionSet;

		if(uConstant)
		{
			ConstantInstruction.uDestination            = uConstant;
			ConstantInstruction.uDestinationAddress     = uCurrentAddress << m_Compiler.GetAddressShift(uConstant, *this);
			ConstantInstruction.uDestinationAddressInfo = 0;

			ConstantInstruction.uDestinationIndex        = 0;
			ConstantInstruction.uDestinationIndexAddress = 0;

			s_ConstantAddressMap.Insert(uConstant, uCurrentAddress);
		}
		else if(pCurrentInstruction->Opcode == ICompiler::MOV)
		{
			if(uSourceConstant1)
			{
				ConstantInstruction.uDestination = uSourceConstant1;

				if(uSourceConstant1 == pCurrentInstruction->uSource1)
				{
					ConstantInstruction.uDestinationAddress     = pCurrentInstruction->uSourceAddress1;
					ConstantInstruction.uDestinationAddressInfo = pCurrentInstruction->uSourceAddressInfo1;
				}
				else
				{
					ConstantInstruction.uDestinationAddress     = pCurrentInstruction->uSourceAddress1 >> m_Compiler.GetAddressShift(pCurrentInstruction->uSource1, *this);
					ConstantInstruction.uDestinationAddressInfo = 0;
				}
			}
			else
			{
				ConstantInstruction.uDestination            = pCurrentInstruction->uSource1;
				ConstantInstruction.uDestinationAddress     = pCurrentInstruction->uSourceAddress1;
				ConstantInstruction.uDestinationAddressInfo = pCurrentInstruction->uSourceAddressInfo1;
			}

			if(uSourceIndexConstant1)
			{
				ConstantInstruction.uDestinationIndex        = uSourceIndexConstant1;
				ConstantInstruction.uDestinationIndexAddress = uSourceIndexConstant1 == ConstantInstruction.uSourceIndex1 ? pCurrentInstruction->uSourceIndexAddress1 : x;
			}
			else
			{
				ConstantInstruction.uDestinationIndex        = pCurrentInstruction->uSourceIndex1;
				ConstantInstruction.uDestinationIndexAddress = pCurrentInstruction->uSourceIndexAddress1;
			}

			s_ConstantAddressMap.Insert( ConstantInstruction.uDestination, ConstantInstruction.uDestinationAddress >> m_Compiler.GetAddressShift(ConstantInstruction.uDestination, *this) );

			if(puUserVaraible && !uSourceConstant1 && GetRegisterType(pCurrentInstruction->uSource1) == ICompiler::GLOBAL)
			{
				UINT uIndex = 0;
				pVariable = m_VariablePool.Get(pCurrentInstruction->uSource1 - 1);
				while(pVariable->uParent)
				{
					uIndex += pVariable->uGlobal;
					pVariable = pVariable->uParent ? m_VariablePool.Get(pVariable->uParent - 1) : ALCHEMY_NULL;
				}

				const GLOBALVARIABLE* pGlobalVariable = pVariable->uGlobal ? m_GlobalVariablePool.Get(pVariable->uGlobal - 1) : ALCHEMY_NULL;
				if(pGlobalVariable)
				{
					UserVariable.VariableType = pVariable->Type;
					UserVariable.RegisterType = ICompiler::GLOBAL;
					UserVariable.uRegister    = pVariable->uRegister;
					UserVariable.uDeclaration = pVariable->uGlobal;
					UserVariable.uIndex       = uIndex;
					UserVariable.uCount       = pGlobalVariable->uCount;
					UserVariable.uNext        = *puUserVaraible;

					s_UserVariableMap.Insert(pCurrentInstruction->uSource1, *puUserVaraible = m_UserVariablePool.Add(UserVariable) + 1);

					if(!uSourceIndexConstant1 && GetRegisterType(pCurrentInstruction->uSourceIndex1) == ICompiler::GLOBAL)
					{
						UINT uIndex = 0;
						pVariable = m_VariablePool.Get(pCurrentInstruction->uSourceIndex1 - 1);
						while(pVariable->uParent)
						{
							uIndex   += pVariable->uGlobal;
							pVariable = pVariable->uParent ? m_VariablePool.Get(pVariable->uParent - 1) : ALCHEMY_NULL;
						}

						pGlobalVariable = pVariable->uGlobal ? m_GlobalVariablePool.Get(pVariable->uGlobal - 1) : ALCHEMY_NULL;

						if(pGlobalVariable)
						{
							UserVariable.VariableType = pVariable->Type;
							UserVariable.RegisterType = ICompiler::GLOBAL;
							UserVariable.uRegister    = pVariable->uRegister;
							UserVariable.uDeclaration = pVariable->uGlobal;
							UserVariable.uIndex       = uIndex;
							UserVariable.uCount       = pGlobalVariable->uCount;
							UserVariable.uNext        = *puUserVaraible;

							s_UserVariableMap.Insert(pCurrentInstruction->uSourceIndex1, *puUserVaraible = m_UserVariablePool.Add(UserVariable) + 1);
						}
					}
				}
			}
		}

		s_uConstantInstruction = pInstructionState->uConstantInstructionHandle = *puConstantInstructionSet = m_InstructionPool.Add(ConstantInstruction) + 1;
	}
	else
	{
		UINT uFrom = Code.GetLength();
		
		if(pCurrentInstruction->Opcode == ICompiler::TEX && !pCurrentInstruction->uSource2)
		{
			const UINT* puSampler = m_BufferSamplerMap.Get(pCurrentInstruction->uSource1);
			UINT uSampler = puSampler ? *puSampler : 0;
			if(!uSampler)
			{
				pVariable = pCurrentInstruction->uSource1 ? m_VariablePool.Get(pCurrentInstruction->uSource1 - 1) : ALCHEMY_NULL;
				if(pVariable)
				{
					UINT uFreeSampler = m_CommonSamplers.GetLength();
					CVector< CVector<UINT> >::CIterator Iterator = m_CommonSamplers.GetIterator(0);
					while ( CVector< CVector<UINT> >::IsValuable(Iterator) ) 
					{
						if( !Iterator->GetLength() )
						{
							uFreeSampler = Iterator.GetIndex();
							break;
						}
						
						++ Iterator;
					}
					
					static const IEffect::SAMPLERDECLARATION s_BUFFER_SAMPLER_DECLARATION = 
					{
						IEffect::TEXTURE2D, 
						IEffect::POINT, 
						IEffect::POINT, 
						IEffect::NONE,
						IEffect::CLAMP, 
						IEffect::CLAMP, 
						IEffect::CLAMP
					};
					
					if(!m_uBufferSamplerDeclaration)
						m_uBufferSamplerDeclaration = m_SamplerDeclarationPool.Add(s_BUFFER_SAMPLER_DECLARATION) + 1;
					
					VARIABLE Variable;
					Variable.Type         = SAMPLER_VARIABLE;
					Variable.uRegister    = uFreeSampler;
					Variable.uDeclaration = m_uBufferSamplerDeclaration;
					Variable.uFunction    = pVariable->uFunction;
					Variable.uGlobal      = 0;
					Variable.uParent      = 0;

					uSampler = m_VariablePool.Add(Variable) + 1;
					
					m_BufferSamplerMap.Insert(pCurrentInstruction->uSource1, uSampler);
				}
			}
			
			pBufferPointer = pCurrentInstruction->uDestinationAddressInfo ? m_BufferPool.Get(pCurrentInstruction->uDestinationAddressInfo - 1) : ALCHEMY_NULL;

			if(pBufferPointer)
			{
				puDestinationAddress = reinterpret_cast<PUINT>( (*pBufferPointer)->GetBufferPointer() );

				uAddressCount = (*pBufferPointer)->GetBufferSize() / sizeof(UINT);

				pBufferPointer = pCurrentInstruction->uSourceAddressInfo1 ? m_BufferPool.Get(pCurrentInstruction->uSourceAddressInfo1 - 1) : ALCHEMY_NULL;
				if(pBufferPointer)
				{
					puSourceAddress = reinterpret_cast<PUINT>( (*pBufferPointer)->GetBufferPointer() );
					uCurrentAddress   = (*pBufferPointer)->GetBufferSize() / sizeof(UINT);
				}
				else
				{
					uCurrentAddress  = ALCHEMY_MAXINUM_BIT(pCurrentInstruction->uSourceAddress1) - ALCHEMY_MININUM_BIT(pCurrentInstruction->uSourceAddress1) + 1;
					uCurrentAddress *= sizeof(UINT);
					if(s_BufferPointer == ALCHEMY_NULL || s_BufferPointer->GetBufferSize() < uCurrentAddress)
						s_BufferPointer = &CreateBuffer(uCurrentAddress, true);

					UnserializeAddress(pCurrentInstruction->uSourceAddress1 >> m_Compiler.GetAddressShift(pCurrentInstruction->uSource1, *this), puSourceAddress = reinterpret_cast<PUINT>( s_BufferPointer->GetBufferPointer() ), &uCurrentAddress);
				}
			}
			else
			{
				pBufferPointer = pCurrentInstruction->uSourceAddressInfo1 ? m_BufferPool.Get(pCurrentInstruction->uSourceAddressInfo1 - 1) : ALCHEMY_NULL;
				if(pBufferPointer)
					uCurrentAddress = (*pBufferPointer)->GetBufferSize() / sizeof(UINT);
				else
				{
					uCurrentAddress = ALCHEMY_MAXINUM_BIT(pCurrentInstruction->uSourceAddress1) - ALCHEMY_MININUM_BIT(pCurrentInstruction->uSourceAddress1) + 1;
					uAddressCount   = ALCHEMY_MAXINUM_BIT(pCurrentInstruction->uDestinationAddress) - ALCHEMY_MININUM_BIT(pCurrentInstruction->uDestinationAddress) + 1;
					uAddressCount  += uCurrentAddress;
					uAddressCount  *= sizeof(UINT);
				}

				if(s_BufferPointer == ALCHEMY_NULL || s_BufferPointer->GetBufferSize() < uAddressCount)
					s_BufferPointer = &CreateBuffer(uAddressCount, true);

				UnserializeAddress(pCurrentInstruction->uDestinationAddress >> m_Compiler.GetAddressShift(pCurrentInstruction->uDestination, *this), puDestinationAddress = reinterpret_cast<PUINT>( s_BufferPointer->GetBufferPointer() ), &uAddressCount);

				if(pBufferPointer)
					puSourceAddress = reinterpret_cast<PUINT>( (*pBufferPointer)->GetBufferPointer() );
				else
					UnserializeAddress(pCurrentInstruction->uSourceAddress1 >> m_Compiler.GetAddressShift(pCurrentInstruction->uSource1, *this), puSourceAddress = puDestinationAddress + uAddressCount);
			}
			
			m_Compiler.AppendInstruction(
					Code, 
					ICompiler::TEX, 
					pCurrentInstruction->uDestination, 
					pCurrentInstruction->uDestinationIndex, 
					pCurrentInstruction->uDestinationIndexAddress, 
					puDestinationAddress, 
					uAddressCount, 
					pCurrentInstruction->uSource1, 
					pCurrentInstruction->uSourceIndex1,
					pCurrentInstruction->uSourceIndexAddress1, 
					puSourceAddress, 
					uCurrentAddress, 
					uSampler, 
					0, 
					0, 
					ALCHEMY_NULL, 
					0, 
					*this);
					
			static INSTRUCTION s_Instruction;
			s_Instruction = *pCurrentInstruction;
			s_Instruction.uSource2 = uSampler;
			
			pCurrentInstruction = &s_Instruction;
		}
		else
			Code.Append(Source, pCurrentInstruction->uCodeBegin, pCurrentInstruction->uCodeLength);
			
		UINT uLength = Code.GetLength(), uMinDestinationAddress = (pCurrentInstruction->uDestination && pCurrentInstruction->uDestinationAddress) ? (ALCHEMY_MININUM_BIT(pCurrentInstruction->uDestinationAddress) - 1) : 0;
		if( uMinDestinationAddress && pCurrentInstruction->uDestinationAddress == ALCHEMY_FLAG(uMinDestinationAddress) )
			uMinDestinationAddress = 0;

		if(uDestinationIndexConstant)
			m_Compiler.ReplaceVariable(Code, pCurrentInstruction->uDestinationIndex, &pCurrentInstruction->uDestinationIndexAddress, uDestinationIndexConstant, &x, 1, uMinDestinationAddress, *this, uFrom, uLength);

		if(uSourceConstant1)
		{
			if(pCurrentInstruction->uSourceAddress1 == ~0)
			{
				puDestinationAddress = ALCHEMY_NULL;
				puSourceAddress      = ALCHEMY_NULL;
				uAddressCount        = 0;
			}
			else
			{
				pBufferPointer = pCurrentInstruction->uSourceAddressInfo1 ? m_BufferPool.Get(pCurrentInstruction->uSourceAddressInfo1 - 1) : ALCHEMY_NULL;

				if(pBufferPointer)
				{
					puDestinationAddress = reinterpret_cast<PUINT>( (*pBufferPointer)->GetBufferPointer() );

					uAddressCount = (*pBufferPointer)->GetBufferSize();//(*pBufferPointer)->GetBufferSize() / sizeof(UINT);

					pBufferPointer = uSourceAddressInfo1 ? m_BufferPool.Get(uSourceAddressInfo1 - 1) : ALCHEMY_NULL;
					if(pBufferPointer)
					{
						puSourceAddress = reinterpret_cast<PUINT>( (*pBufferPointer)->GetBufferPointer() );
						uAddressCount  /= sizeof(UINT);
					}
					else
					{
						if(s_BufferPointer == ALCHEMY_NULL || s_BufferPointer->GetBufferSize() < uAddressCount)
							s_BufferPointer = &CreateBuffer(uAddressCount, true);

						uAddressCount /= sizeof(UINT);

						if(uSourceAddress1)
							UnserializeAddress( ( uSourceAddress1 >> m_Compiler.GetAddressShift(uSourceConstant1, *this) ) & ( (1 << uAddressCount) - 1 ), puSourceAddress = reinterpret_cast<PUINT>( s_BufferPointer->GetBufferPointer() ), &uAddressCount);
						else
							UnserializeAddress(pCurrentInstruction->uSourceAddress1 >> (ALCHEMY_MININUM_BIT(pCurrentInstruction->uSourceAddress1) - 1), puSourceAddress = reinterpret_cast<PUINT>( s_BufferPointer->GetBufferPointer() ), &uAddressCount);
					}
				}
				else
				{
					pBufferPointer = uSourceAddressInfo1 ? m_BufferPool.Get(uSourceAddressInfo1 - 1) : ALCHEMY_NULL;
					if(pBufferPointer)
						uAddressCount   = (*pBufferPointer)->GetBufferSize();
					else
					{
						uCurrentAddress = ALCHEMY_MININUM_BIT(pCurrentInstruction->uSourceAddress1);

						uAddressCount  = ALCHEMY_MAXINUM_BIT(pCurrentInstruction->uSourceAddress1) - uCurrentAddress + 1;
						uAddressCount *= sizeof(UINT) * 2;
					}

					if(s_BufferPointer == ALCHEMY_NULL || s_BufferPointer->GetBufferSize() < uAddressCount)
						s_BufferPointer = &CreateBuffer(uAddressCount, true);

					UnserializeAddress(pCurrentInstruction->uSourceAddress1 >> m_Compiler.GetAddressShift(pCurrentInstruction->uSource1, *this), puDestinationAddress = reinterpret_cast<PUINT>( s_BufferPointer->GetBufferPointer() ), &uAddressCount);

					if(pBufferPointer)
						puSourceAddress = reinterpret_cast<PUINT>( (*pBufferPointer)->GetBufferPointer() );
					else if(uSourceAddress1)
						UnserializeAddress( ( uSourceAddress1 >> m_Compiler.GetAddressShift(uSourceConstant1, *this) ) & ( (1 << uAddressCount) - 1 ), puSourceAddress = puDestinationAddress + uAddressCount);
					else
						UnserializeAddress(pCurrentInstruction->uSourceAddress1 >> (uCurrentAddress - 1), puSourceAddress = puDestinationAddress + uAddressCount);
				}
			}

			m_Compiler.ReplaceVariable(Code, pCurrentInstruction->uSource1, puDestinationAddress, uSourceConstant1, puSourceAddress, uAddressCount, uMinDestinationAddress, *this, uFrom, uLength);
		}

		if(uSourceIndexConstant1)
			m_Compiler.ReplaceVariable(Code, pCurrentInstruction->uSourceIndex1, &pCurrentInstruction->uSourceIndexAddress1, uSourceIndexConstant1, &x, 1, uMinDestinationAddress, *this, uFrom, uLength);

		if(uSourceConstant2)
		{
			if(pCurrentInstruction->uSourceAddress2 == ~0)
			{
				puDestinationAddress = ALCHEMY_NULL;
				puSourceAddress      = ALCHEMY_NULL;
				uAddressCount        = 0;
			}
			else
			{
				pBufferPointer = pCurrentInstruction->uSourceAddressInfo2 ? m_BufferPool.Get(pCurrentInstruction->uSourceAddressInfo2 - 1) : ALCHEMY_NULL;

				if(pBufferPointer)
				{
					puDestinationAddress = reinterpret_cast<PUINT>( (*pBufferPointer)->GetBufferPointer() );

					uAddressCount = (*pBufferPointer)->GetBufferSize();//(*pBufferPointer)->GetBufferSize() / sizeof(UINT);

					pBufferPointer = uSourceAddressInfo2 ? m_BufferPool.Get(uSourceAddressInfo2 - 1) : ALCHEMY_NULL;
					if(pBufferPointer)
					{
						puSourceAddress = reinterpret_cast<PUINT>( (*pBufferPointer)->GetBufferPointer() );
						uAddressCount  /= sizeof(UINT);
					}
					else
					{
						if(s_BufferPointer == ALCHEMY_NULL || s_BufferPointer->GetBufferSize() < uAddressCount)
							s_BufferPointer = &CreateBuffer(uAddressCount, true);

						uAddressCount /= sizeof(UINT);
						if(uSourceAddress2)
							UnserializeAddress( ( uSourceAddress2 >> m_Compiler.GetAddressShift(uSourceConstant2, *this) ) & ( (1 << uAddressCount) - 1 ), puSourceAddress = reinterpret_cast<PUINT>( s_BufferPointer->GetBufferPointer() ) );
						else
							UnserializeAddress( pCurrentInstruction->uSourceAddress2 >> (ALCHEMY_MININUM_BIT(pCurrentInstruction->uSourceAddress2) - 1), puSourceAddress = reinterpret_cast<PUINT>( s_BufferPointer->GetBufferPointer() ) );
					}
				}
				else
				{
					pBufferPointer = uSourceAddressInfo2 ? m_BufferPool.Get(uSourceAddressInfo2 - 1) : ALCHEMY_NULL;
					if(pBufferPointer)
						uAddressCount   = (*pBufferPointer)->GetBufferSize();
					else
					{
						uCurrentAddress = ALCHEMY_MININUM_BIT(pCurrentInstruction->uSourceAddress2);

						uAddressCount  = ALCHEMY_MAXINUM_BIT(pCurrentInstruction->uSourceAddress2) - uCurrentAddress + 1;
						uAddressCount *= sizeof(UINT) * 2;
					}

					if(s_BufferPointer == ALCHEMY_NULL || s_BufferPointer->GetBufferSize() < uAddressCount)
						s_BufferPointer = &CreateBuffer(uAddressCount, true);

					UnserializeAddress(pCurrentInstruction->uSourceAddress2 >> m_Compiler.GetAddressShift(pCurrentInstruction->uSource2, *this), puDestinationAddress = reinterpret_cast<PUINT>( s_BufferPointer->GetBufferPointer() ), &uAddressCount);

					if(pBufferPointer)
						puSourceAddress = reinterpret_cast<PUINT>( (*pBufferPointer)->GetBufferPointer() );
					else if(uSourceAddress2)
						UnserializeAddress( ( uSourceAddress2 >> m_Compiler.GetAddressShift(uSourceConstant2, *this) ) & ( (1 << uAddressCount) - 1 ), puSourceAddress = puDestinationAddress + uAddressCount);
					else
						UnserializeAddress(pCurrentInstruction->uSourceAddress2 >> (uCurrentAddress - 1), puSourceAddress = puDestinationAddress + uAddressCount);
				}
			}

			m_Compiler.ReplaceVariable(Code, pCurrentInstruction->uSource2, puDestinationAddress, uSourceConstant2, puSourceAddress, uAddressCount, uMinDestinationAddress, *this, uFrom, uLength);
		}

		if(uSourceIndexConstant2)
			m_Compiler.ReplaceVariable(Code, pCurrentInstruction->uSourceIndex2, &pCurrentInstruction->uSourceIndexAddress2, uSourceIndexConstant2, &x, 1, uMinDestinationAddress, *this, uFrom, uLength);

		if(puUserVaraible)
		{
			UINT uDeclaration;
			PUINT puCurrentUserVaraible, puShaderParameters;
			//LPVARIABLE pVariable;
			//USERVARIABLE UserVariable;
			LPSHADERPARAMETER pShaderParameter;
			const GLOBALVARIABLE* pGlobalVariable;

			if(pCurrentInstruction->uDestination)
			{
				puCurrentUserVaraible = s_UserVariableMap.Get(pCurrentInstruction->uDestination);
				if(!puCurrentUserVaraible)
				{
					pVariable = m_VariablePool.Get(pCurrentInstruction->uDestination - 1);
					if(pVariable)
					{
						UserVariable.VariableType = pVariable->Type;
						UserVariable.uRegister    = pVariable->uRegister;
						UserVariable.uDeclaration = 0;
						UserVariable.uIndex       = 0;
						UserVariable.uCount       = 1;
						UserVariable.RegisterType = GetRegisterType(pCurrentInstruction->uDestination);
						UserVariable.uNext        = *puUserVaraible;

						if (UserVariable.RegisterType == ICompiler::SAMPLER)
						{
							UserVariable.uDeclaration = pVariable->uDeclaration;
						}
						else if(UserVariable.RegisterType == ICompiler::GLOBAL || UserVariable.RegisterType == ICompiler::PARAMETER || UserVariable.RegisterType == ICompiler::TEMPORARY)
						{
							uDeclaration = 0;
							while(pVariable && pVariable->uParent)
							{
								uDeclaration += pVariable->uGlobal;
								pVariable = m_VariablePool.Get(pVariable->uParent - 1);
							}

							UserVariable.uIndex = uDeclaration;

							if(pVariable && pVariable->uGlobal)
							{
								if(UserVariable.RegisterType == ICompiler::GLOBAL)
								{
									UserVariable.uRegister    = pVariable->uRegister;
									UserVariable.uDeclaration = pVariable->uGlobal;

									pGlobalVariable = pVariable->uGlobal ? m_GlobalVariablePool.Get(pVariable->uGlobal - 1) : ALCHEMY_NULL;
									if(pGlobalVariable)
										UserVariable.uCount = pGlobalVariable->uCount;
								}
								else if(UserVariable.RegisterType == ICompiler::PARAMETER)
								{
									puShaderParameters = m_ShaderParametersPool.Get(pVariable->uGlobal - 1);
									if( puShaderParameters && (UserVariable.uDeclaration = *puShaderParameters) )
									{
										pShaderParameter = m_ShaderParameterPool.Get(UserVariable.uDeclaration - 1);

										while(pShaderParameter && uDeclaration --)
										{
											if(pShaderParameter->uNext)
												UserVariable.uDeclaration = pShaderParameter->uNext;

											pShaderParameter = pShaderParameter->uNext ? m_ShaderParameterPool.Get(pShaderParameter->uNext - 1) : ALCHEMY_NULL;
										}
									}
								}
								else if(UserVariable.RegisterType == ICompiler::TEMPORARY)
								{
									puCount = m_TemporaryVariableCountMap.Get(pCurrentInstruction->uDestination);
									if(puCount)
										UserVariable.uCount = *puCount;
								}
							}
						}

						s_UserVariableMap.Insert(pCurrentInstruction->uDestination, *puUserVaraible = m_UserVariablePool.Add(UserVariable) + 1);
					}
				}
			}

			if(pCurrentInstruction->uDestinationIndex && !uDestinationIndexConstant)
			{
				puCurrentUserVaraible = s_UserVariableMap.Get(pCurrentInstruction->uDestinationIndex);
				if(!puCurrentUserVaraible)
				{
					pVariable = m_VariablePool.Get(pCurrentInstruction->uDestinationIndex - 1);
					if(pVariable)
					{
						UserVariable.VariableType = pVariable->Type;
						UserVariable.uRegister    = pVariable->uRegister;
						UserVariable.uDeclaration = 0;
						UserVariable.uIndex       = 0;
						UserVariable.RegisterType = GetRegisterType(pCurrentInstruction->uDestinationIndex);
						UserVariable.uNext        = *puUserVaraible;

						if(UserVariable.RegisterType == ICompiler::GLOBAL || UserVariable.RegisterType == ICompiler::PARAMETER || UserVariable.RegisterType == ICompiler::TEMPORARY)
						{
							uDeclaration = 0;
							while(pVariable && pVariable->uParent)
							{
								uDeclaration += pVariable->uGlobal;
								pVariable = m_VariablePool.Get(pVariable->uParent - 1);
							}

							UserVariable.uIndex = uDeclaration;

							if(pVariable && pVariable->uGlobal)
							{
								if(UserVariable.RegisterType == ICompiler::GLOBAL)
								{
									UserVariable.uRegister    = pVariable->uRegister;
									UserVariable.uDeclaration = pVariable->uGlobal;

									pGlobalVariable = pVariable->uGlobal ? m_GlobalVariablePool.Get(pVariable->uGlobal - 1) : ALCHEMY_NULL;
									if(pGlobalVariable)
										UserVariable.uCount = pGlobalVariable->uCount;
								}
								else if(UserVariable.RegisterType == ICompiler::PARAMETER)
								{
									puShaderParameters = m_ShaderParametersPool.Get(pVariable->uGlobal - 1);
									if( puShaderParameters && (UserVariable.uDeclaration = *puShaderParameters) )
									{
										pShaderParameter = m_ShaderParameterPool.Get(UserVariable.uDeclaration - 1);

										while(pShaderParameter && uDeclaration --)
										{
											if(pShaderParameter->uNext)
												UserVariable.uDeclaration = pShaderParameter->uNext;

											pShaderParameter = pShaderParameter->uNext ? m_ShaderParameterPool.Get(pShaderParameter->uNext - 1) : ALCHEMY_NULL;
										}
									}
								}
								else if(UserVariable.RegisterType == ICompiler::TEMPORARY)
								{
									puCount = m_TemporaryVariableCountMap.Get(pCurrentInstruction->uDestination);
									if(puCount)
										UserVariable.uCount = *puCount;
								}
							}
						}

						s_UserVariableMap.Insert(pCurrentInstruction->uDestinationIndex, *puUserVaraible = m_UserVariablePool.Add(UserVariable) + 1);
					}
				}
			}

			UINT uSource1 = uSourceConstant1 ? 0 : pCurrentInstruction->uSource1, 
			     uSource2 = uSourceConstant2 ? 0 : pCurrentInstruction->uSource2, 
				 uSourceIndex1 = uSourceIndexConstant1 ? 0 : pCurrentInstruction->uSourceIndex1, 
				 uSourceIndex2 = uSourceIndexConstant2 ? 0 : pCurrentInstruction->uSourceIndex2;
			if( m_Compiler.IsInverceInstruction(pCurrentInstruction->Opcode) )
			{
				ALCHEMY_SWAP(uSource1, uSource2);
				ALCHEMY_SWAP(uSourceIndex1, uSourceIndex2);
			}
			
			if(uSource1)
			{
				puCurrentUserVaraible = s_UserVariableMap.Get(uSource1);
				if(!puCurrentUserVaraible)
				{
					pVariable = m_VariablePool.Get(uSource1 - 1);
					if(pVariable)
					{
						UserVariable.VariableType = pVariable->Type;
						UserVariable.uRegister    = pVariable->uRegister;
						UserVariable.uDeclaration = 0;
						UserVariable.uIndex       = 0;
						UserVariable.RegisterType = GetRegisterType(uSource1);
						UserVariable.uNext        = *puUserVaraible;

						if (UserVariable.RegisterType == ICompiler::SAMPLER)
						{
							UserVariable.uDeclaration = pVariable->uDeclaration;
						}
						else if(UserVariable.RegisterType == ICompiler::GLOBAL || UserVariable.RegisterType == ICompiler::PARAMETER || UserVariable.RegisterType == ICompiler::TEMPORARY)
						{
							uDeclaration = 0;
							while(pVariable && pVariable->uParent)
							{
								uDeclaration += pVariable->uGlobal;
								pVariable = m_VariablePool.Get(pVariable->uParent - 1);
							}

							UserVariable.uIndex = uDeclaration;

							if(pVariable && pVariable->uGlobal)
							{
								if(UserVariable.RegisterType == ICompiler::GLOBAL)
								{
									UserVariable.uRegister    = pVariable->uRegister;
									UserVariable.uDeclaration = pVariable->uGlobal;

									pGlobalVariable = pVariable->uGlobal ? m_GlobalVariablePool.Get(pVariable->uGlobal - 1) : ALCHEMY_NULL;
									if(pGlobalVariable)
										UserVariable.uCount = pGlobalVariable->uCount;
								}
								else if(UserVariable.RegisterType == ICompiler::PARAMETER)
								{
									puShaderParameters = m_ShaderParametersPool.Get(pVariable->uGlobal - 1);
									if( puShaderParameters && (UserVariable.uDeclaration = *puShaderParameters) )
									{
										pShaderParameter = m_ShaderParameterPool.Get(UserVariable.uDeclaration - 1);

										while(pShaderParameter && uDeclaration --)
										{
											if(pShaderParameter->uNext)
												UserVariable.uDeclaration = pShaderParameter->uNext;

											pShaderParameter = pShaderParameter->uNext ? m_ShaderParameterPool.Get(pShaderParameter->uNext - 1) : ALCHEMY_NULL;
										}
									}
								}
								else if(UserVariable.RegisterType == ICompiler::TEMPORARY)
								{
									puCount = m_TemporaryVariableCountMap.Get(uSource1);
									if(puCount)
										UserVariable.uCount = *puCount;
								}
							}
						}

						s_UserVariableMap.Insert(uSource1, *puUserVaraible = m_UserVariablePool.Add(UserVariable) + 1);
					}
				}
			}

			if(uSourceIndex1)
			{
				puCurrentUserVaraible = s_UserVariableMap.Get(uSourceIndex1);
				if(!puCurrentUserVaraible)
				{
					pVariable = m_VariablePool.Get(uSourceIndex1 - 1);
					if(pVariable)
					{
						UserVariable.VariableType = pVariable->Type;
						UserVariable.uRegister    = pVariable->uRegister;
						UserVariable.uDeclaration = 0;
						UserVariable.uIndex       = 0;
						UserVariable.RegisterType = GetRegisterType(uSourceIndex1);
						UserVariable.uNext        = *puUserVaraible;

						if(UserVariable.RegisterType == ICompiler::GLOBAL || UserVariable.RegisterType == ICompiler::PARAMETER || UserVariable.RegisterType == ICompiler::TEMPORARY)
						{
							uDeclaration = 0;
							while(pVariable && pVariable->uParent)
							{
								uDeclaration += pVariable->uGlobal;
								pVariable = m_VariablePool.Get(pVariable->uParent - 1);
							}

							UserVariable.uIndex = uDeclaration;

							if(pVariable && pVariable->uGlobal)
							{
								if(UserVariable.RegisterType == ICompiler::GLOBAL)
								{
									UserVariable.uRegister    = pVariable->uRegister;
									UserVariable.uDeclaration = pVariable->uGlobal;

									pGlobalVariable = pVariable->uGlobal ? m_GlobalVariablePool.Get(pVariable->uGlobal - 1) : ALCHEMY_NULL;
									if(pGlobalVariable)
										UserVariable.uCount = pGlobalVariable->uCount;
								}
								else if(UserVariable.RegisterType == ICompiler::PARAMETER)
								{
									puShaderParameters = m_ShaderParametersPool.Get(pVariable->uGlobal - 1);
									if( puShaderParameters && (UserVariable.uDeclaration = *puShaderParameters) )
									{
										pShaderParameter = m_ShaderParameterPool.Get(UserVariable.uDeclaration - 1);

										while(pShaderParameter && uDeclaration --)
										{
											if(pShaderParameter->uNext)
												UserVariable.uDeclaration = pShaderParameter->uNext;

											pShaderParameter = pShaderParameter->uNext ? m_ShaderParameterPool.Get(pShaderParameter->uNext - 1) : ALCHEMY_NULL;
										}
									}
								}
								else if(UserVariable.RegisterType == ICompiler::TEMPORARY)
								{
									puCount = m_TemporaryVariableCountMap.Get(uSourceIndex1);
									if(puCount)
										UserVariable.uCount = *puCount;
								}
							}
						}

						s_UserVariableMap.Insert(uSourceIndex1, *puUserVaraible = m_UserVariablePool.Add(UserVariable) + 1);
					}
				}
			}

			if(uSource2)
			{
				puCurrentUserVaraible = s_UserVariableMap.Get(uSource2);
				if(!puCurrentUserVaraible)
				{
					pVariable = m_VariablePool.Get(uSource2 - 1);
					if(pVariable)
					{
						UserVariable.VariableType = pVariable->Type;
						UserVariable.uRegister    = pVariable->uRegister;
						UserVariable.uDeclaration = 0;
						UserVariable.uIndex       = 0;
						UserVariable.RegisterType = GetRegisterType(uSource2);
						UserVariable.uNext        = *puUserVaraible;

						if (UserVariable.RegisterType == ICompiler::SAMPLER)
						{
							UserVariable.uDeclaration = pVariable->uDeclaration;
						}
						else if(UserVariable.RegisterType == ICompiler::GLOBAL || UserVariable.RegisterType == ICompiler::PARAMETER || UserVariable.RegisterType == ICompiler::TEMPORARY)
						{
							uDeclaration = 0;
							while(pVariable && pVariable->uParent)
							{
								uDeclaration += pVariable->uGlobal;
								pVariable = m_VariablePool.Get(pVariable->uParent - 1);
							}

							UserVariable.uIndex = uDeclaration;

							if(pVariable && pVariable->uGlobal)
							{
								if(UserVariable.RegisterType == ICompiler::GLOBAL)
								{
									UserVariable.uRegister    = pVariable->uRegister;
									UserVariable.uDeclaration = pVariable->uGlobal;

									pGlobalVariable = pVariable->uGlobal ? m_GlobalVariablePool.Get(pVariable->uGlobal - 1) : ALCHEMY_NULL;
									if(pGlobalVariable)
										UserVariable.uCount = pGlobalVariable->uCount;
								}
								else if(UserVariable.RegisterType == ICompiler::PARAMETER)
								{
									puShaderParameters = m_ShaderParametersPool.Get(pVariable->uGlobal - 1);
									if( puShaderParameters && (UserVariable.uDeclaration = *puShaderParameters) )
									{
										pShaderParameter = m_ShaderParameterPool.Get(UserVariable.uDeclaration - 1);

										while(pShaderParameter && uDeclaration --)
										{
											if(pShaderParameter->uNext)
												UserVariable.uDeclaration = pShaderParameter->uNext;

											pShaderParameter = pShaderParameter->uNext ? m_ShaderParameterPool.Get(pShaderParameter->uNext - 1) : ALCHEMY_NULL;
										}
									}
								}
								else if(UserVariable.RegisterType == ICompiler::TEMPORARY)
								{
									puCount = m_TemporaryVariableCountMap.Get(uSource2);
									if(puCount)
										UserVariable.uCount = *puCount;
								}
							}
						}

						s_UserVariableMap.Insert(uSource2, *puUserVaraible = m_UserVariablePool.Add(UserVariable) + 1);
					}
				}
			}

			if(uSourceIndex2)
			{
				puCurrentUserVaraible = s_UserVariableMap.Get(uSourceIndex2);
				if(!puCurrentUserVaraible)
				{
					pVariable = m_VariablePool.Get(uSourceIndex2 - 1);
					if(pVariable)
					{
						UserVariable.VariableType = pVariable->Type;
						UserVariable.uRegister    = pVariable->uRegister;
						UserVariable.uDeclaration = 0;
						UserVariable.uIndex       = 0;
						UserVariable.RegisterType = GetRegisterType(uSourceIndex2);
						UserVariable.uNext        = *puUserVaraible;

						if(UserVariable.RegisterType == ICompiler::GLOBAL || UserVariable.RegisterType == ICompiler::PARAMETER || UserVariable.RegisterType == ICompiler::TEMPORARY)
						{
							uDeclaration = 0;
							while(pVariable && pVariable->uParent)
							{
								uDeclaration += pVariable->uGlobal;
								pVariable = m_VariablePool.Get(pVariable->uParent - 1);
							}

							UserVariable.uIndex = uDeclaration;

							if(pVariable && pVariable->uGlobal)
							{
								if(UserVariable.RegisterType == ICompiler::GLOBAL)
								{
									UserVariable.uRegister    = pVariable->uRegister;
									UserVariable.uDeclaration = pVariable->uGlobal;

									pGlobalVariable = pVariable->uGlobal ? m_GlobalVariablePool.Get(pVariable->uGlobal - 1) : ALCHEMY_NULL;
									if(pGlobalVariable)
										UserVariable.uCount = pGlobalVariable->uCount;
								}
								else if(UserVariable.RegisterType == ICompiler::PARAMETER)
								{
									puShaderParameters = m_ShaderParametersPool.Get(pVariable->uGlobal - 1);
									if( puShaderParameters && (UserVariable.uDeclaration = *puShaderParameters) )
									{
										pShaderParameter = m_ShaderParameterPool.Get(UserVariable.uDeclaration - 1);

										while(pShaderParameter && uDeclaration --)
										{
											if(pShaderParameter->uNext)
												UserVariable.uDeclaration = pShaderParameter->uNext;

											pShaderParameter = pShaderParameter->uNext ? m_ShaderParameterPool.Get(pShaderParameter->uNext - 1) : ALCHEMY_NULL;
										}
									}
								}
								else if(UserVariable.RegisterType == ICompiler::TEMPORARY)
								{
									puCount = m_TemporaryVariableCountMap.Get(uSourceIndex2);
									if(puCount)
										UserVariable.uCount = *puCount;
								}
							}
						}

						s_UserVariableMap.Insert(uSourceIndex2, *puUserVaraible = m_UserVariablePool.Add(UserVariable) + 1);
					}
				}
			}
		}

		s_uConstantInstruction = 0;
	}

	//if(pInstructionState)
	if(!bIsUpdateMap)
		pInstructionState->bIsInit = true;

	//s_InstructionMap.Insert(uInstruction, true);
}

bool CEffectCode::BuildShaderFunctionCode(STRING& Desination, IEffect::EFFECTTYPE Type, UINT uUserVariables, UINT uCode)
{
	return m_Compiler.BuildShaderFunctionCode(Desination, Type, uUserVariables, uCode, *this);
}

UINT CEffectCode::SerializeAddress(const UINT* puAddress, UINT uAddressCount)
{
	UINT uAddress = 0;
	for(UINT i = 0; i < uAddressCount; ++ i)
		ALCHEMY_SET_BIT(uAddress, puAddress[i]);

	return uAddress;
}

void CEffectCode::UnserializeAddress(UINT uAddress, PUINT puOutputAddress, PUINT puOutputAddressCount)
{
	if(uAddress)
	{
		UINT uOutputAddressCount = 0, uMaxAddress = ALCHEMY_MAXINUM_BIT(uAddress), uMinAddress = ALCHEMY_MININUM_BIT(uAddress) - 1;

		if(puOutputAddress)
		{
			for(UINT i = uMinAddress; i < uMaxAddress; ++ i)
			{
				if( ALCHEMY_TEST_BIT(uAddress, i) )
					puOutputAddress[uOutputAddressCount ++] = i;
			}

			if(puOutputAddressCount)
				*puOutputAddressCount = uOutputAddressCount;
		}
		else if(puOutputAddressCount)
		{
			//*puOutputAddressCount = uMaxAddress - uMinAddress + 1;
			for(UINT i = uMinAddress; i < uMaxAddress; ++ i)
			{
				if( ALCHEMY_TEST_BIT(uAddress, i) )
					++ uOutputAddressCount;
			}

			*puOutputAddressCount = uOutputAddressCount;
		}
	}
	else if(puOutputAddressCount)
		*puOutputAddressCount = 0;
}

void CEffectCode::UnserializeInstructionAddress(UINT uSourceAddress, 
												UINT uDestinationAddress, 
												const UINT* puAddress, 
												UINT uAddressCount, 
												PUINT puOutputAddress, 
												PUINT puOutputAddressInfo,
												PUINT puOutputAddressCount)
{
	if(puOutputAddress || puOutputAddressCount)
	{
		UINT uOutputAddressCount = 0, uEnd = ALCHEMY_MAXINUM_BIT(uSourceAddress), uBegin = uSourceAddress ? (ALCHEMY_MININUM_BIT(uSourceAddress) - 1) : 0;

		if(puOutputAddress)
		{
			UINT uAddress = 0;
			if(puAddress && uAddressCount)
			{
				for(UINT i = uBegin; i < uEnd; ++ i)
				{
					if( ALCHEMY_TEST_BIT(uDestinationAddress, i) )
						ALCHEMY_SET_BIT(uAddress, puOutputAddress[uOutputAddressCount ++] = puAddress[i - uBegin]);
				}
			}
			else
			{
				for(UINT i = uBegin; i < uEnd; ++ i)
				{
					if( ALCHEMY_TEST_BIT(uDestinationAddress, i) )
						ALCHEMY_SET_BIT(uAddress, puOutputAddress[uOutputAddressCount ++] = i - uBegin);
				}
			}

			if(puOutputAddressInfo)
				*puOutputAddressInfo = uAddress;
		}
		else
		{
			for(UINT i = uBegin; i < uEnd; ++ i)
			{
				if( ALCHEMY_TEST_BIT(uDestinationAddress, i) )
					++ uOutputAddressCount;
			}
		}
		
		if(puOutputAddressCount)
			*puOutputAddressCount = uOutputAddressCount;
	}
}

CBuffer& CEffectCode::CreatePassInfos(PUINT puRenderTargetIndexCount)
{
	UINT uPassCount = m_Passes.GetLength(), uPassInfoCount = 0, uRenderTargetIndexCount, i;
	const UINT* puRenderTargetCodes;
	const PASS* pPass;
	LPRENDERTARGETCODE pRenderTargetCode;
	for(i = 0; i < uPassCount; ++ i)
	{
		pPass = m_Passes.Get(i);
		uRenderTargetIndexCount = 0;
		if(pPass && pPass->uRenderTargetCodes && ( puRenderTargetCodes = m_RenderTargetCodesPool.Get(pPass->uRenderTargetCodes - 1) ) && *puRenderTargetCodes)
		{
			pRenderTargetCode = m_RenderTargetCodePool.Get(*puRenderTargetCodes - 1);
			while(pRenderTargetCode)
			{
				++ uRenderTargetIndexCount;

				pRenderTargetCode = pRenderTargetCode->uNext ? m_RenderTargetCodePool.Get(pRenderTargetCode->uNext - 1) : ALCHEMY_NULL;
			}
		}
		uPassInfoCount += uRenderTargetIndexCount; 

		if(puRenderTargetIndexCount)
			* (puRenderTargetIndexCount ++) = uRenderTargetIndexCount;
	}

	//CGlobalInputHandler* pGlobalInputHandler = ALCHEMY_NULL;
	//UINT uByteAlignmentShift = m_Compiler.GetCompilerInfo().uByteAlignmentShift;
	const UINT* puUserVariable/*, * puShaderParameter = ALCHEMY_NULL*/;

	CEffect::LPPASSINFO pPassInfos;

	ALCHEMY_DEBUG_NEW(pPassInfos, CEffect::PASSINFO[uPassInfoCount]);

	CEffect::LPPASSINFO pCurrentPassInfo = pPassInfos;
	CEffect::LPPARAMETERINFO pParameterInfo;

	const USERVARIABLE* pUserVariable;

	const SHADERPARAMETER* pShaderParameter;
	CVertexDeclaration::LPELEMENT pVertexElement;

	IEffect::EFFECTTYPE Type;
	
	STRING Name;
	PCHAR pcName;
	
	UINT uRegisterOffset, uMinRegister, uMaxRegister, uSamplerCount, uVertexSamplerCount, uFragmentSamplerCount, uConstantSamplerCount, uConstantVertexSamplerCount, uConstantFragmentSamplerCount;
	CMap<UINT, UINT> m_SamplerCountMap;
	PUINT puCount;

	for(i = 0; i < uPassCount; ++ i)
	{
		pPass = m_Passes.Get(i);
		if(pPass && pPass->uRenderTargetCodes && ( puRenderTargetCodes = m_RenderTargetCodesPool.Get(pPass->uRenderTargetCodes - 1) ) && *puRenderTargetCodes)
		{
			pRenderTargetCode = m_RenderTargetCodePool.Get(*puRenderTargetCodes - 1);
			while(pRenderTargetCode)
			{
				pCurrentPassInfo->pParameterInfos        = ALCHEMY_NULL;
				pCurrentPassInfo->pTextureParameterInfos = ALCHEMY_NULL;
				pCurrentPassInfo->pVertexElements        = ALCHEMY_NULL;
				pCurrentPassInfo->ppcSamplerNames        = ALCHEMY_NULL;
				pCurrentPassInfo->ppcAttributeNames      = ALCHEMY_NULL;
				pCurrentPassInfo->uParameterInfoCount    = 0;
				pCurrentPassInfo->uVertexElementCount    = 0;
				pCurrentPassInfo->uTextureCount          = 0;

				puUserVariable = pRenderTargetCode->uVertexUserVariables ? m_UserVariablesPool.Get(pRenderTargetCode->uVertexUserVariables - 1) : ALCHEMY_NULL;
				pUserVariable  = (puUserVariable && *puUserVariable) ? m_UserVariablePool.Get(*puUserVariable - 1) : ALCHEMY_NULL;
				Type = IEffect::VERTEX_SHADER;
				uMinRegister                  = ~0;
				uVertexSamplerCount           = 0;
				uFragmentSamplerCount         = 0;
				uConstantVertexSamplerCount   = 0;
				uConstantFragmentSamplerCount = 0;
				while(pUserVariable)
				{
					switch(pUserVariable->RegisterType)
					{
					case ICompiler::PARAMETER:
						if(Type == IEffect::VERTEX_SHADER)
						{
							pShaderParameter = pUserVariable->uDeclaration ? m_ShaderParameterPool.Get(pUserVariable->uDeclaration - 1) : ALCHEMY_NULL;
							if(pShaderParameter)
								++ pCurrentPassInfo->uVertexElementCount;
						}
						break;
					case ICompiler::GLOBAL:
						++ pCurrentPassInfo->uParameterInfoCount;
						
						uConstantFragmentSamplerCount = uConstantVertexSamplerCount;
						if(Type == IEffect::VERTEX_SHADER)
							uMinRegister = ALCHEMY_MININUM(uMinRegister, pUserVariable->uRegister);
						break;
					case ICompiler::SAMPLER:
						puCount = m_SamplerCountMap.Get(pUserVariable->uRegister);
						if(!puCount)
							m_SamplerCountMap.Insert(pUserVariable->uRegister, uFragmentSamplerCount ++);
							
						++ pCurrentPassInfo->uTextureCount;
						
						break;
                            
                    default:
                        break;
					}

					pUserVariable = pUserVariable->uNext ? m_UserVariablePool.Get(pUserVariable->uNext - 1) : ALCHEMY_NULL;

					if(!pUserVariable && Type == IEffect::VERTEX_SHADER)
					{
						Type = IEffect::FRAGMENT_SHADER;
						uVertexSamplerCount           = uFragmentSamplerCount;
						uConstantVertexSamplerCount   = uConstantFragmentSamplerCount;
						uFragmentSamplerCount         = 0;
						uConstantFragmentSamplerCount = 0;

						puUserVariable = pRenderTargetCode->uFragmentUserVariables/*pFragmentShaderFunction->uUserVariables*/ ? m_UserVariablesPool.Get(pRenderTargetCode->uFragmentUserVariables/*pFragmentShaderFunction->uUserVariables*/ - 1) : ALCHEMY_NULL;
						pUserVariable  = (puUserVariable && *puUserVariable) ? m_UserVariablePool.Get(*puUserVariable - 1) : ALCHEMY_NULL;
					}
				}
				
				uConstantVertexSamplerCount   = uVertexSamplerCount   - uConstantVertexSamplerCount;
				uConstantFragmentSamplerCount = uFragmentSamplerCount - uConstantFragmentSamplerCount;

				ALCHEMY_DEBUG_NEW(pCurrentPassInfo->pParameterInfos       , CEffect::PARAMETERINFO     [pCurrentPassInfo->uParameterInfoCount]);
				ALCHEMY_DEBUG_NEW(pCurrentPassInfo->pTextureParameterInfos, CEffect::PARAMETERINFO     [pCurrentPassInfo->uTextureCount      ]);
				ALCHEMY_DEBUG_NEW(pCurrentPassInfo->pVertexElements       , CVertexDeclaration::ELEMENT[pCurrentPassInfo->uVertexElementCount]);
				ALCHEMY_DEBUG_NEW(pCurrentPassInfo->ppcSamplerNames       , const CHAR*                [pCurrentPassInfo->uTextureCount      ]);
				ALCHEMY_DEBUG_NEW(pCurrentPassInfo->ppcAttributeNames     , const CHAR*                [pCurrentPassInfo->uVertexElementCount]);

				pCurrentPassInfo->uParameterInfoCount = 0;
				pCurrentPassInfo->uTextureCount       = 0;
				pCurrentPassInfo->uVertexElementCount = 0;

				puUserVariable = pRenderTargetCode->uVertexUserVariables ? m_UserVariablesPool.Get(pRenderTargetCode->uVertexUserVariables - 1) : ALCHEMY_NULL;
				pUserVariable  = (puUserVariable && *puUserVariable) ? m_UserVariablePool.Get(*puUserVariable - 1) : ALCHEMY_NULL;
				Type = IEffect::VERTEX_SHADER;
				uRegisterOffset       = 0;
				uMaxRegister          = 0;
				uSamplerCount         = uVertexSamplerCount;
				uConstantSamplerCount = uConstantVertexSamplerCount;
				while(pUserVariable)
				{
					switch(pUserVariable->RegisterType)
					{
					case ICompiler::PARAMETER:
						if(Type == IEffect::VERTEX_SHADER)
						{
							pShaderParameter = pUserVariable->uDeclaration ? m_ShaderParameterPool.Get(pUserVariable->uDeclaration - 1) : ALCHEMY_NULL;
							if(pShaderParameter)
							{ 
								pVertexElement              = &pCurrentPassInfo->pVertexElements[pCurrentPassInfo->uVertexElementCount];
								pVertexElement->Type        = pShaderParameter->Type;
								pVertexElement->Usage       = pShaderParameter->Usage;
								pVertexElement->uUsageIndex = pShaderParameter->uIndex;
								pVertexElement->uIndex      = pUserVariable->uIndex;
								
								Name                        = ALCHEMY_NULL;
								
								m_Compiler.AppendRegisterName(Name, pUserVariable->uRegister, pUserVariable->RegisterType, Type);
								ALCHEMY_DEBUG_NEW(pcName, CHAR[Name.GetLength() + 1]);
								Name.CopyTo(pcName);
								pCurrentPassInfo->ppcAttributeNames[pCurrentPassInfo->uVertexElementCount] = pcName;
								
								++ pCurrentPassInfo->uVertexElementCount;
							}
						}
						break;
					case ICompiler::GLOBAL:
						pParameterInfo         = &pCurrentPassInfo->pParameterInfos[pCurrentPassInfo->uParameterInfoCount ++];
						pParameterInfo->Type   = Type;
						pParameterInfo->uIndex = m_Compiler.ComputeRegisterLocation(pUserVariable->uRegister, uRegisterOffset, uMinRegister, uSamplerCount, uConstantSamplerCount);
							
						uMaxRegister = ALCHEMY_MAXINUM(uMaxRegister, 
													   pUserVariable->uRegister + GetRegisterCount(pUserVariable->VariableType) * pUserVariable->uCount);
						break;
					case ICompiler::SAMPLER:
						pParameterInfo         = &pCurrentPassInfo->pTextureParameterInfos[pCurrentPassInfo->uTextureCount];
						pParameterInfo->Type   = Type;
						pParameterInfo->uIndex = pUserVariable->uRegister;
						
						Name                   = ALCHEMY_NULL;
						
						m_Compiler.AppendRegisterName(Name, pUserVariable->uRegister, pUserVariable->RegisterType, Type);
						ALCHEMY_DEBUG_NEW(pcName, CHAR[Name.GetLength() + 1]);
						Name.CopyTo(pcName);
						pCurrentPassInfo->ppcSamplerNames[pCurrentPassInfo->uTextureCount] = pcName;
						
						++ pCurrentPassInfo->uTextureCount;
						break;
                    default:
                        break;
					}

					pUserVariable = pUserVariable->uNext ? m_UserVariablePool.Get(pUserVariable->uNext - 1) : ALCHEMY_NULL;

					if(!pUserVariable && Type == IEffect::VERTEX_SHADER)
					{
						Type = IEffect::FRAGMENT_SHADER;
						uRegisterOffset       = uMaxRegister;
						uSamplerCount         = uFragmentSamplerCount;
						uConstantSamplerCount = uConstantFragmentSamplerCount;

						puUserVariable = pRenderTargetCode->uFragmentUserVariables ? m_UserVariablesPool.Get(pRenderTargetCode->uFragmentUserVariables - 1) : ALCHEMY_NULL;
						pUserVariable  = (puUserVariable && *puUserVariable) ? m_UserVariablePool.Get(*puUserVariable - 1) : ALCHEMY_NULL;
					}
				}

				pRenderTargetCode = pRenderTargetCode->uNext ? m_RenderTargetCodePool.Get(pRenderTargetCode->uNext - 1) : ALCHEMY_NULL;

				++ pCurrentPassInfo;
			}
		}
	}
	
	CBuffer& Buffer = CEffect::CreatePassInfoBuffer(pPassInfos, uPassInfoCount);

	CEffect::DestroyPassInfos(pPassInfos, uPassInfoCount);

	return Buffer;
}

typedef struct  
{
	const void** ppValue;
	UINT uValueFrom;
	UINT uValueCount;
	UINT uOffset;
}CONSTANTINPUTINFO, * LPCONSTANTINPUTINFO;

CBuffer& CEffectCode::CreateParameterList(CGlobalInputHandler** ppGlobalInputHandler, PUINT puRenderTargetIndexCount)
{
	UINT uPassCount = m_Passes.GetLength(), uParameterListCount = 0, uRenderTargetIndexCount, i;
	const UINT* puRenderTargetCodes;
	const PASS* pPass;
	LPRENDERTARGETCODE pRenderTargetCode;
	for(i = 0; i < uPassCount; ++ i)
	{
		pPass = m_Passes.Get(i);
		uRenderTargetIndexCount = 0;
		if(pPass && pPass->uRenderTargetCodes && ( puRenderTargetCodes = m_RenderTargetCodesPool.Get(pPass->uRenderTargetCodes - 1) ) && *puRenderTargetCodes)
		{
			pRenderTargetCode = m_RenderTargetCodePool.Get(*puRenderTargetCodes - 1);
			while(pRenderTargetCode)
			{
				++ uRenderTargetIndexCount;

				pRenderTargetCode = pRenderTargetCode->uNext ? m_RenderTargetCodePool.Get(pRenderTargetCode->uNext - 1) : ALCHEMY_NULL;
			}
		}
		uParameterListCount += uRenderTargetIndexCount; 

		if(puRenderTargetIndexCount)
			* (puRenderTargetIndexCount ++) = uRenderTargetIndexCount;
	}

	CGlobalInputHandler* pGlobalInputHandler = ALCHEMY_NULL;
	UINT uByteAlignmentShift = m_Compiler.GetCompilerInfo().uByteAlignmentShift, uCurrentMaxOffset, uOffset, uRegister, uCurrentCount, j;
	const UINT* puUserVariable, * puConstantInstruction, * puOffset;//, * puShaderParameter = ALCHEMY_NULL;

	CRenderMethod::LPPARAMETERLIST pParameterList;

	ALCHEMY_DEBUG_NEW(pParameterList, CRenderMethod::PARAMETERLIST[uParameterListCount]);

	CRenderMethod::LPPARAMETERLIST pCurrentParameterList = pParameterList, pCurrentConstantParameterList;
	CRenderMethod::LPPARAMETERINFO pParameterInfo;

	//const FUNCTION* pVertexShaderFunction, * pFragmentShaderFunction;
	const USERVARIABLE* pUserVariable;
	const GLOBALVARIABLE* pGlobalVariable;
	const VARIABLE* pVariable, * pCurrentVariable;

	//const SHADERFUNCTION* pShaderFunction;
	//const SHADERPARAMETER* pShaderParameter;
	//LPELEMENTDECLARATION pVertexElement;

	IEffect::EFFECTTYPE Type;
	CGlobalInputHandler::LPGLOBALINPUT pGlobalInput;
	CGlobalInputHandler::GLOBALINSTRUCTION GlobalInstruction;
	LPINSTRUCTION pConstantInstruction;
	CMap<UINT, UINT> ConstantOffsetMap;
	CMap<UINT, UINT> VariableOffsetMap;
	CSmartPointer<CBuffer>* pAddressBufferPointer;

	CONSTANTINPUTINFO ConstantInputInfo;
	CVector<CONSTANTINPUTINFO> ConstantInputInfos;
	CVector<FLOAT> ConstantInputInfoValue;
	CMap<GLOBALCODECONSTANTS, UINT> ConstantInputValueMap;

	CGlobalInputHandler::LPINPUTVALUE pInputValue;
	
	UINT uRegisterOffset, uMaxRegister, uMinRegister, uSamplerCount, uVertexSamplerCount, uFragmentSamplerCount, uConstantSamplerCount, uConstantVertexSamplerCount, uConstantFragmentSamplerCount, uRegisterIndex, uValue, uTemp;
	CMap<UINT, UINT> m_SamplerCountMap;
	PUINT puCount;
	
	const IEffect::SAMPLERDECLARATION* pSamplerDeclaration;
	
	const CONSTANTBUFFERDECLARATION* pConstantBufferDeclaration;

	uRenderTargetIndexCount = 0;
	for(i = 0; i < uPassCount; ++ i)
	{
		//pCurrentParameterList->pVertexElements = ALCHEMY_NULL;
		//pCurrentParameterList->uVertexElementCount = 0;

		pPass = m_Passes.Get(i);
		/*if( pPass && 
			(pVertexShaderFunction   = pPass->uVertexShader   ? m_FunctionPool.Get(pPass->uVertexShader   - 1) : ALCHEMY_NULL) != ALCHEMY_NULL &&
			(pFragmentShaderFunction = pPass->uFragmentShader ? m_FunctionPool.Get(pPass->uFragmentShader - 1) : ALCHEMY_NULL) != ALCHEMY_NULL )*/
		if(pPass && pPass->uRenderTargetCodes && ( puRenderTargetCodes = m_RenderTargetCodesPool.Get(pPass->uRenderTargetCodes - 1) ) && *puRenderTargetCodes)
		{
			/*if( ( (pVertexShaderFunction   = pPass->uVertexShader   ? m_FunctionPool.Get(pPass->uVertexShader   - 1) : ALCHEMY_NULL) != ALCHEMY_NULL) &&
				( (pFragmentShaderFunction = pPass->uFragmentShader ? m_FunctionPool.Get(pPass->uFragmentShader - 1) : ALCHEMY_NULL) != ALCHEMY_NULL ) &&
				pVertexShaderFunction->uShader && ( pShaderFunction = m_ShaderFunctionPool.Get(pVertexShaderFunction->uShader - 1) ) != ALCHEMY_NULL)
					puShaderParameter = pShaderFunction->uParameters ? m_ShaderParametersPool.Get(pShaderFunction->uParameters - 1) : ALCHEMY_NULL;*/

			/*if(pVertexShaderFunction->uShader && ( pShaderFunction = m_ShaderFunctionPool.Get(pVertexShaderFunction->uShader - 1) ) != ALCHEMY_NULL)
			{
				if( pShaderFunction->uParameters && ( puShaderParameter = m_ShaderParametersPool.Get(pShaderFunction->uParameters - 1) ) != ALCHEMY_NULL)
				{
					pShaderParameter = m_ShaderParameterPool.Get(*puShaderParameter - 1);

					while(pShaderParameter)
					{
						++ pCurrentParameterList->uVertexElementCount;
						pShaderParameter = pShaderParameter->uNext ? m_ShaderParameterPool.Get(pShaderParameter->uNext - 1) : ALCHEMY_NULL;
					}

					ALCHEMY_DEBUG_NEW(pCurrentParameterList->pVertexElements, ELEMENTDECLARATION[pCurrentParameterList->uVertexElementCount]);

					pShaderParameter = m_ShaderParameterPool.Get(*puShaderParameter - 1);

					uCurrentCount = 0;
					while(pShaderParameter)
					{
						pVertexElement         = &pCurrentParameterList->pVertexElements[uCurrentCount ++];
						pVertexElement->Type   = pShaderParameter->Type;
						pVertexElement->Usage  = pShaderParameter->Usage;
						pVertexElement->uIndex = pShaderParameter->uIndex;

						pShaderParameter                = pShaderParameter->uNext ? m_ShaderParameterPool.Get(pShaderParameter->uNext - 1) : ALCHEMY_NULL;
					}
				}
			}*/

			uOffset = 0;
			uCurrentMaxOffset = 0;
			ConstantOffsetMap.Clear();
			VariableOffsetMap.Clear();

			pRenderTargetCode = m_RenderTargetCodePool.Get(*puRenderTargetCodes - 1);
			while(pRenderTargetCode)
			{
				if( ppGlobalInputHandler && (pRenderTargetCode->uVertexConstantInstructionSet || pRenderTargetCode->uFragmentConstantInstructionSet) )
				{
					puConstantInstruction = m_InstructionSetPool.Get(pRenderTargetCode->uVertexConstantInstructionSet - 1);
					if(puConstantInstruction)
						Type = IEffect::VERTEX_SHADER;
					else
					{
						puConstantInstruction = m_InstructionSetPool.Get(pRenderTargetCode->uFragmentConstantInstructionSet - 1);

						Type = IEffect::FRAGMENT_SHADER;
					}

					pConstantInstruction = (puConstantInstruction && *puConstantInstruction) ? m_InstructionPool.Get(*puConstantInstruction - 1) : ALCHEMY_NULL;
					if(pConstantInstruction)
					{
						if(!pGlobalInputHandler)
						{
							pGlobalInputHandler = *ppGlobalInputHandler;
							if(pGlobalInputHandler)
								pGlobalInputHandler->Destroy();
							else
								ALCHEMY_DEBUG_NEW(pGlobalInputHandler, CGlobalInputHandler);

							pGlobalInputHandler->m_uInputCount = uParameterListCount;

							ALCHEMY_DEBUG_NEW(pGlobalInputHandler->m_ppGlobalInput     , CGlobalInputHandler::LPGLOBALINPUT[uParameterListCount]);
							ALCHEMY_DEBUG_NEW(pGlobalInputHandler->m_puGlobalInputCount, UINT                              [uParameterListCount]);

							for(j = 0; j < uParameterListCount; ++ j)
							{
								pGlobalInputHandler->m_ppGlobalInput     [j] = ALCHEMY_NULL;
								pGlobalInputHandler->m_puGlobalInputCount[j] = 0;
							}

							*ppGlobalInputHandler = pGlobalInputHandler;
						}

						pCurrentConstantParameterList = ALCHEMY_NULL;

						while(pConstantInstruction)
						{
							GlobalInstruction.Opcode = pConstantInstruction->Opcode;

							puOffset = VariableOffsetMap.Get(pConstantInstruction->uDestination);
							if(puOffset)
							{
								GlobalInstruction.uDestination = *puOffset;

								pVariable = pConstantInstruction->uDestination ? m_VariablePool.Get(pConstantInstruction->uDestination - 1) : ALCHEMY_NULL;
								if(pVariable)
								{
									uCurrentCount = 1;
									if(pVariable->uGlobal)
									{
										pGlobalVariable = m_GlobalVariablePool.Get(pVariable->uGlobal - 1);
										if(pGlobalVariable)
											uCurrentCount = pGlobalVariable->uCount;
									}

									GlobalInstruction.uDestinationSize = GetRegisterCount(pVariable->Type) * uCurrentCount;

									pAddressBufferPointer = pConstantInstruction->uDestinationAddress ? m_BufferPool.Get(pConstantInstruction->uDestinationAddress - 1) : ALCHEMY_NULL;
									GlobalInstruction.uDestinationAddress = pAddressBufferPointer ? (pGlobalInputHandler->m_AddressBuffers.Add(*pAddressBufferPointer) + 1) : 0;
								}
								else
								{
									GlobalInstruction.uDestinationSize = 0;

									GlobalInstruction.uDestinationAddress = 0;
								}
							}
							else
							{
								pVariable = pConstantInstruction->uDestination ? m_VariablePool.Get(pConstantInstruction->uDestination - 1) : ALCHEMY_NULL;
								if(pVariable)
								{
									if(pVariable->uGlobal)
									{
										pGlobalVariable = m_GlobalVariablePool.Get(pVariable->uGlobal - 1);
										if(pGlobalVariable)
										{
											if(pGlobalVariable->GlobalType == INPUT_VARIABLE)
											{
												uRegister = 0;

												pCurrentVariable = pVariable;
												while(pCurrentVariable)
												{
													pVariable = pCurrentVariable;

													uRegister += pCurrentVariable->uRegister;
													pCurrentVariable = pCurrentVariable->uParent ? m_VariablePool.Get(pCurrentVariable->uParent - 1) : ALCHEMY_NULL;
												}

												if(pGlobalVariable->pValue)
												{
													if(!pGlobalInputHandler->m_puInputValueCount)
													{
														ALCHEMY_DEBUG_NEW(pGlobalInputHandler->m_puInputValueCount, UINT[uParameterListCount]);

														for(j = 0; j < uParameterListCount; ++ j)
															pGlobalInputHandler->m_puInputValueCount[j] = 0;
													}

													ConstantOffsetMap.Insert(uRegister, pGlobalInputHandler->m_puInputValueCount[uRenderTargetIndexCount] ++);
												}
												else
													ConstantOffsetMap.Insert(uRegister, uOffset);
											}
											else
											{
												if(!pGlobalInputHandler->m_ppParameterList)
												{
													ALCHEMY_DEBUG_NEW(pGlobalInputHandler->m_ppParameterList, CRenderMethod::LPPARAMETERLIST[uParameterListCount]);
													for(j = 0; j < uParameterListCount; ++ j)
														pGlobalInputHandler->m_ppParameterList[j] = ALCHEMY_NULL;
												}

												pCurrentConstantParameterList = pGlobalInputHandler->m_ppParameterList[uRenderTargetIndexCount];
												if(!pCurrentConstantParameterList)
												{
													ALCHEMY_DEBUG_NEW(pCurrentConstantParameterList, CRenderMethod::PARAMETERLIST);

													pCurrentConstantParameterList->pTextures = ALCHEMY_NULL;
													pCurrentConstantParameterList->pSamplerDeclaration = ALCHEMY_NULL;
													pCurrentConstantParameterList->pCameraParameters = ALCHEMY_NULL;
													pCurrentConstantParameterList->pLightParameters = ALCHEMY_NULL;
													pCurrentConstantParameterList->pTextureParameters = ALCHEMY_NULL;

													pCurrentConstantParameterList->uParameterFlag = 0;
													pCurrentConstantParameterList->uArrayParameterFlag = 0;
													//pCurrentConstantParameterList->uMatrixParameterFlag = 0;
													//pCurrentConstantParameterList->uCameraParameterFlag = 0;
													pCurrentConstantParameterList->uMaterialParameterFlag = 0;
													pCurrentConstantParameterList->uConstantParameterFlag = 0;

													pCurrentConstantParameterList->puCameraParameterFlag = ALCHEMY_NULL;
													pCurrentConstantParameterList->puLightParameterFlag = ALCHEMY_NULL;
													pCurrentConstantParameterList->puTextureParameterFlag = ALCHEMY_NULL;

													pCurrentConstantParameterList->uCameraCount = 0;
													pCurrentConstantParameterList->uLightCount = 0;
													pCurrentConstantParameterList->uTextureCount = 0;

													pGlobalInputHandler->m_ppParameterList[uRenderTargetIndexCount] = pCurrentConstantParameterList;
												}

												switch(pGlobalVariable->GlobalType)
												{
												case NORMAL_VARIABLE:
													if(!pGlobalVariable->uIndex && pGlobalVariable->uDetail < IRenderMethod::TOTAL_PARAMETERS)
													{
														pParameterInfo = &pCurrentConstantParameterList->Parameters[pGlobalVariable->uDetail];

														if(ALCHEMY_TEST_BIT(pCurrentConstantParameterList->uParameterFlag, pGlobalVariable->uDetail) && pGlobalVariable->ShaderType != pParameterInfo->Type)
														{
															if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
																pParameterInfo->uBetaIndex = uOffset;
															else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
																pParameterInfo->uAlphaIndex = uOffset;

															pParameterInfo->Type = IEffect::PROGRAM;
														}
														else
														{
															pParameterInfo->Type        = pGlobalVariable->ShaderType;
															pParameterInfo->uAlphaIndex = uOffset;
														}

														ALCHEMY_SET_BIT(pCurrentConstantParameterList->uParameterFlag, pGlobalVariable->uDetail);
													}
													break;

												case ARRAY_VARIABLE:
													if(!pGlobalVariable->uIndex && pGlobalVariable->uDetail < IRenderMethod::TOTAL_ARRAY_PARAMETERS)
													{
														pParameterInfo = &pCurrentConstantParameterList->ArrayParameters[pGlobalVariable->uDetail];

														if(ALCHEMY_TEST_BIT(pCurrentConstantParameterList->uArrayParameterFlag, pGlobalVariable->uDetail) && pGlobalVariable->ShaderType != pParameterInfo->Type)
														{
															if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
																pParameterInfo->uBetaIndex = uOffset;
															else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
																pParameterInfo->uAlphaIndex = uOffset;

															pParameterInfo->Type = IEffect::PROGRAM;
														}
														else
														{
															pParameterInfo->Type        = pGlobalVariable->ShaderType;
															pParameterInfo->uAlphaIndex = uOffset;
														}

														ALCHEMY_SET_BIT(pCurrentConstantParameterList->uArrayParameterFlag, pGlobalVariable->uDetail);
													}
													break;

												/*case MATRIX_VARIABLE:
													if(!pGlobalVariable->uIndex && pGlobalVariable->uDetail < CRenderMethod::TOTAL_MATRIX_PARAMETERS)
													{
														pParameterInfo = &pCurrentConstantParameterList->MatrixParameters[pGlobalVariable->uDetail];

														if(ALCHEMY_TEST_BIT(pCurrentConstantParameterList->uMatrixParameterFlag, pGlobalVariable->uDetail) && pGlobalVariable->ShaderType != pParameterInfo->Type)
														{
															if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
																pParameterInfo->uBetaIndex = uOffset;
															else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
																pParameterInfo->uAlphaIndex = uOffset;

															pParameterInfo->Type = IEffect::PROGRAM;
														}
														else
														{
															pParameterInfo->Type        = pGlobalVariable->ShaderType;
															pParameterInfo->uAlphaIndex = uOffset;
														}

														ALCHEMY_SET_BIT(pCurrentConstantParameterList->uMatrixParameterFlag, pGlobalVariable->uDetail);
													}
													break;*
												case CAMERA_VARIABLE:
													if(!pGlobalVariable->uIndex && pGlobalVariable->uDetail < CRenderMethod::TOTAL_CAMERA_PARAMETERS)
													{
														pParameterInfo = &pCurrentConstantParameterList->CameraParameters[pGlobalVariable->uDetail];

														if(ALCHEMY_TEST_BIT(pCurrentConstantParameterList->uCameraParameterFlag, pGlobalVariable->uDetail) && pGlobalVariable->ShaderType != pParameterInfo->Type)
														{
															if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
																pParameterInfo->uBetaIndex = uOffset;
															else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
																pParameterInfo->uAlphaIndex = uOffset;

															pParameterInfo->Type = IEffect::PROGRAM;
														}
														else
														{
															pParameterInfo->Type        = pGlobalVariable->ShaderType;
															pParameterInfo->uAlphaIndex = uOffset;
														}

														ALCHEMY_SET_BIT(pCurrentConstantParameterList->uCameraParameterFlag, pGlobalVariable->uDetail);
													}
													break;*/
												case MATERIAL_VARIABLE:
													if(!pGlobalVariable->uIndex && pGlobalVariable->uDetail < CRenderMethod::TOTAL_MATERIAL_PARAMETERS)
													{
														pParameterInfo = &pCurrentConstantParameterList->MaterialParameters[pGlobalVariable->uDetail];

														if(ALCHEMY_TEST_BIT(pCurrentConstantParameterList->uMaterialParameterFlag, pGlobalVariable->uDetail) && pGlobalVariable->ShaderType != pParameterInfo->Type)
														{
															if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
																pParameterInfo->uBetaIndex = uOffset;
															else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
																pParameterInfo->uAlphaIndex = uOffset;

															pParameterInfo->Type = IEffect::PROGRAM;
														}
														else
														{
															pParameterInfo->Type        = pGlobalVariable->ShaderType;
															pParameterInfo->uAlphaIndex = uOffset;
														}

														ALCHEMY_SET_BIT(pCurrentConstantParameterList->uMaterialParameterFlag, pGlobalVariable->uDetail);
													}
													break;

												case CAMERA_VARIABLE:
													if(pGlobalVariable->uDetail < CRenderMethod::TOTAL_CAMERA_PARAMETERS)
													{
														uCurrentCount = pGlobalVariable->uIndex + 1;

														pCurrentConstantParameterList->uCameraCount = ALCHEMY_MAXINUM(pCurrentConstantParameterList->uCameraCount, uCurrentCount);
													}
													break;

												case LIGHT_VARIABLE:
													if(pGlobalVariable->uDetail < CRenderMethod::TOTAL_LIGHT_PARAMETERS)
													{
														uCurrentCount = pGlobalVariable->uIndex + 1;

														pCurrentConstantParameterList->uLightCount = ALCHEMY_MAXINUM(pCurrentConstantParameterList->uLightCount, uCurrentCount);
													}
													break;

												case CONSTANT_VARIABLE:
													if(!pGlobalVariable->uIndex && pGlobalVariable->uDetail < CRenderMethod::TOTAL_CONSTANT_PARAMETERS)
													{
														pParameterInfo = &pCurrentConstantParameterList->ConstantParameters[pGlobalVariable->uDetail];

														if(ALCHEMY_TEST_BIT(pCurrentConstantParameterList->uConstantParameterFlag, pGlobalVariable->uDetail) && pGlobalVariable->ShaderType != pParameterInfo->Type)
														{
															if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
																pParameterInfo->uBetaIndex = uOffset;
															else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
																pParameterInfo->uAlphaIndex = uOffset;

															pParameterInfo->Type = IEffect::PROGRAM;
														}
														else
														{
															pParameterInfo->Type        = pGlobalVariable->ShaderType;
															pParameterInfo->uAlphaIndex = uOffset;
														}

														ALCHEMY_SET_BIT(pCurrentConstantParameterList->uConstantParameterFlag, pGlobalVariable->uDetail);
													}
													break;
                                                        
                                                default:
                                                    break;
												}
											}

											uCurrentCount = pGlobalVariable->uCount;
										}
										else
											uCurrentCount = 1;
									}
									else
										uCurrentCount = 1;

									VariableOffsetMap.Insert(pConstantInstruction->uDestination, uOffset);

									GlobalInstruction.uDestination = uOffset;

									uOffset += GlobalInstruction.uDestinationSize = GetRegisterCount(pVariable->Type) * uCurrentCount;

									pAddressBufferPointer = pConstantInstruction->uDestinationAddress ? m_BufferPool.Get(pConstantInstruction->uDestinationAddress - 1) : ALCHEMY_NULL;
									GlobalInstruction.uDestinationAddress = pAddressBufferPointer ? (pGlobalInputHandler->m_AddressBuffers.Add(*pAddressBufferPointer) + 1) : 0;
								}
								else
								{
									GlobalInstruction.uDestination        = 0;
									GlobalInstruction.uDestinationSize    = 0;
									GlobalInstruction.uDestinationAddress = 0;
								}
							}

							puOffset = VariableOffsetMap.Get(pConstantInstruction->uDestinationIndex);
							if(puOffset)
							{
								GlobalInstruction.uDestinationIndex = *puOffset;

								pVariable = pConstantInstruction->uDestinationIndex ? m_VariablePool.Get(pConstantInstruction->uDestinationIndex - 1) : ALCHEMY_NULL;
								if(pVariable)
								{
									uCurrentCount = 1;
									if(pVariable->uGlobal)
									{
										pGlobalVariable = m_GlobalVariablePool.Get(pVariable->uGlobal - 1);
										if(pGlobalVariable)
											uCurrentCount = pGlobalVariable->uCount;
									}

									GlobalInstruction.uDestinationIndexSize = GetRegisterCount(pVariable->Type) * uCurrentCount;

									GlobalInstruction.uDestinationIndexAddress = pConstantInstruction->uDestinationIndexAddress;
								}
								else
								{
									GlobalInstruction.uDestinationIndexSize = 0;

									GlobalInstruction.uDestinationIndexAddress = 0;
								}
							}
							else
							{
								pVariable = pConstantInstruction->uDestinationIndex ? m_VariablePool.Get(pConstantInstruction->uDestinationIndex - 1) : ALCHEMY_NULL;
								if(pVariable)
								{
									if(pVariable->uGlobal)
									{
										pGlobalVariable = m_GlobalVariablePool.Get(pVariable->uGlobal - 1);
										if(pGlobalVariable)
										{
											if(pGlobalVariable->GlobalType == INPUT_VARIABLE)
											{
												uRegister = 0;

												pCurrentVariable = pVariable;
												while(pCurrentVariable)
												{
													pVariable = pCurrentVariable;

													uRegister += pCurrentVariable->uRegister;
													pCurrentVariable = pCurrentVariable->uParent ? m_VariablePool.Get(pCurrentVariable->uParent - 1) : ALCHEMY_NULL;
												}

												if(pGlobalVariable->pValue)
												{
													if(!pGlobalInputHandler->m_puInputValueCount)
													{
														ALCHEMY_DEBUG_NEW(pGlobalInputHandler->m_puInputValueCount, UINT[uParameterListCount]);

														for(j = 0; j < uParameterListCount; ++ j)
															pGlobalInputHandler->m_puInputValueCount[j] = 0;
													}

													ConstantOffsetMap.Insert(uRegister, pGlobalInputHandler->m_puInputValueCount[uRenderTargetIndexCount] ++);
												}
												else
													ConstantOffsetMap.Insert(uRegister, uOffset);
											}
											else
											{
												if(!pGlobalInputHandler->m_ppParameterList)
												{
													ALCHEMY_DEBUG_NEW(pGlobalInputHandler->m_ppParameterList, CRenderMethod::LPPARAMETERLIST[uParameterListCount]);
													for(j = 0; j < uParameterListCount; ++ j)
														pGlobalInputHandler->m_ppParameterList[j] = ALCHEMY_NULL;
												}

												pCurrentConstantParameterList = pGlobalInputHandler->m_ppParameterList[uRenderTargetIndexCount];
												if(!pCurrentConstantParameterList)
												{
													ALCHEMY_DEBUG_NEW(pCurrentConstantParameterList, CRenderMethod::PARAMETERLIST);

													pCurrentConstantParameterList->pTextures = ALCHEMY_NULL;
													pCurrentConstantParameterList->pSamplerDeclaration = ALCHEMY_NULL;
													pCurrentConstantParameterList->pCameraParameters = ALCHEMY_NULL;
													pCurrentConstantParameterList->pLightParameters = ALCHEMY_NULL;
													pCurrentConstantParameterList->pTextureParameters = ALCHEMY_NULL;

													pCurrentConstantParameterList->uParameterFlag = 0;
													pCurrentConstantParameterList->uArrayParameterFlag = 0;
													//pCurrentConstantParameterList->uMatrixParameterFlag = 0;
													//pCurrentConstantParameterList->uCameraParameterFlag = 0;
													pCurrentConstantParameterList->uMaterialParameterFlag = 0;
													pCurrentConstantParameterList->uConstantParameterFlag = 0;

													pCurrentConstantParameterList->puCameraParameterFlag = ALCHEMY_NULL;
													pCurrentConstantParameterList->puLightParameterFlag = ALCHEMY_NULL;
													pCurrentConstantParameterList->puTextureParameterFlag = ALCHEMY_NULL;

													pCurrentConstantParameterList->uCameraCount = 0;
													pCurrentConstantParameterList->uLightCount = 0;
													pCurrentConstantParameterList->uTextureCount = 0;

													pGlobalInputHandler->m_ppParameterList[uRenderTargetIndexCount] = pCurrentConstantParameterList;
												}

												switch(pGlobalVariable->GlobalType)
												{
												case NORMAL_VARIABLE:
													if(!pGlobalVariable->uIndex && pGlobalVariable->uDetail < IRenderMethod::TOTAL_PARAMETERS)
													{
														pParameterInfo = &pCurrentConstantParameterList->Parameters[pGlobalVariable->uDetail];

														if(ALCHEMY_TEST_BIT(pCurrentConstantParameterList->uParameterFlag, pGlobalVariable->uDetail) && pGlobalVariable->ShaderType != pParameterInfo->Type)
														{
															if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
																pParameterInfo->uBetaIndex = uOffset;
															else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
																pParameterInfo->uAlphaIndex = uOffset;

															pParameterInfo->Type = IEffect::PROGRAM;
														}
														else
														{
															pParameterInfo->Type        = pGlobalVariable->ShaderType;
															pParameterInfo->uAlphaIndex = uOffset;
														}

														ALCHEMY_SET_BIT(pCurrentConstantParameterList->uParameterFlag, pGlobalVariable->uDetail);
													}
													break;

												case ARRAY_VARIABLE:
													if(!pGlobalVariable->uIndex && pGlobalVariable->uDetail < IRenderMethod::TOTAL_ARRAY_PARAMETERS)
													{
														pParameterInfo = &pCurrentConstantParameterList->ArrayParameters[pGlobalVariable->uDetail];

														if(ALCHEMY_TEST_BIT(pCurrentConstantParameterList->uArrayParameterFlag, pGlobalVariable->uDetail) && pGlobalVariable->ShaderType != pParameterInfo->Type)
														{
															if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
																pParameterInfo->uBetaIndex = uOffset;
															else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
																pParameterInfo->uAlphaIndex = uOffset;

															pParameterInfo->Type = IEffect::PROGRAM;
														}
														else
														{
															pParameterInfo->Type        = pGlobalVariable->ShaderType;
															pParameterInfo->uAlphaIndex = uOffset;
														}

														ALCHEMY_SET_BIT(pCurrentConstantParameterList->uArrayParameterFlag, pGlobalVariable->uDetail);
													}
													break;

												/*case MATRIX_VARIABLE:
													if(!pGlobalVariable->uIndex && pGlobalVariable->uDetail < CRenderMethod::TOTAL_MATRIX_PARAMETERS)
													{
														pParameterInfo = &pCurrentConstantParameterList->MatrixParameters[pGlobalVariable->uDetail];

														if(ALCHEMY_TEST_BIT(pCurrentConstantParameterList->uMatrixParameterFlag, pGlobalVariable->uDetail) && pGlobalVariable->ShaderType != pParameterInfo->Type)
														{
															if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
																pParameterInfo->uBetaIndex = uOffset;
															else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
																pParameterInfo->uAlphaIndex = uOffset;

															pParameterInfo->Type = IEffect::PROGRAM;
														}
														else
														{
															pParameterInfo->Type        = pGlobalVariable->ShaderType;
															pParameterInfo->uAlphaIndex = uOffset;
														}

														ALCHEMY_SET_BIT(pCurrentConstantParameterList->uMatrixParameterFlag, pGlobalVariable->uDetail);
													}
													break;
												case CAMERA_VARIABLE:
													if(!pGlobalVariable->uIndex && pGlobalVariable->uDetail < CRenderMethod::TOTAL_CAMERA_PARAMETERS)
													{
														pParameterInfo = &pCurrentConstantParameterList->CameraParameters[pGlobalVariable->uDetail];

														if(ALCHEMY_TEST_BIT(pCurrentConstantParameterList->uCameraParameterFlag, pGlobalVariable->uDetail) && pGlobalVariable->ShaderType != pParameterInfo->Type)
														{
															if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
																pParameterInfo->uBetaIndex = uOffset;
															else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
																pParameterInfo->uAlphaIndex = uOffset;

															pParameterInfo->Type = IEffect::PROGRAM;
														}
														else
														{
															pParameterInfo->Type        = pGlobalVariable->ShaderType;
															pParameterInfo->uAlphaIndex = uOffset;
														}

														ALCHEMY_SET_BIT(pCurrentConstantParameterList->uCameraParameterFlag, pGlobalVariable->uDetail);
													}
													break;*/

												case MATERIAL_VARIABLE:
													if(!pGlobalVariable->uIndex && pGlobalVariable->uDetail < CRenderMethod::TOTAL_MATERIAL_PARAMETERS)
													{
														pParameterInfo = &pCurrentConstantParameterList->MaterialParameters[pGlobalVariable->uDetail];

														if(ALCHEMY_TEST_BIT(pCurrentConstantParameterList->uMaterialParameterFlag, pGlobalVariable->uDetail) && pGlobalVariable->ShaderType != pParameterInfo->Type)
														{
															if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
																pParameterInfo->uBetaIndex = uOffset;
															else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
																pParameterInfo->uAlphaIndex = uOffset;

															pParameterInfo->Type = IEffect::PROGRAM;
														}
														else
														{
															pParameterInfo->Type        = pGlobalVariable->ShaderType;
															pParameterInfo->uAlphaIndex = uOffset;
														}

														ALCHEMY_SET_BIT(pCurrentConstantParameterList->uMaterialParameterFlag, pGlobalVariable->uDetail);
													}
													break;

												case CAMERA_VARIABLE:
													if(pGlobalVariable->uDetail < CRenderMethod::TOTAL_CAMERA_PARAMETERS)
													{
														uCurrentCount = pGlobalVariable->uIndex + 1;

														pCurrentConstantParameterList->uCameraCount = ALCHEMY_MAXINUM(pCurrentConstantParameterList->uCameraCount, uCurrentCount);
													}
													break;

												case LIGHT_VARIABLE:
													if(pGlobalVariable->uDetail < CRenderMethod::TOTAL_LIGHT_PARAMETERS)
													{
														uCurrentCount = pGlobalVariable->uIndex + 1;

														pCurrentConstantParameterList->uLightCount = ALCHEMY_MAXINUM(pCurrentConstantParameterList->uLightCount, uCurrentCount);
													}
													break;

												case CONSTANT_VARIABLE:
													if(!pGlobalVariable->uIndex && pGlobalVariable->uDetail < CRenderMethod::TOTAL_CONSTANT_PARAMETERS)
													{
														pParameterInfo = &pCurrentConstantParameterList->ConstantParameters[pGlobalVariable->uDetail];

														if(ALCHEMY_TEST_BIT(pCurrentConstantParameterList->uConstantParameterFlag, pGlobalVariable->uDetail) && pGlobalVariable->ShaderType != pParameterInfo->Type)
														{
															if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
																pParameterInfo->uBetaIndex = uOffset;
															else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
																pParameterInfo->uAlphaIndex = uOffset;

															pParameterInfo->Type = IEffect::PROGRAM;
														}
														else
														{
															pParameterInfo->Type        = pGlobalVariable->ShaderType;
															pParameterInfo->uAlphaIndex = uOffset;
														}

														ALCHEMY_SET_BIT(pCurrentConstantParameterList->uConstantParameterFlag, pGlobalVariable->uDetail);
													}
													break;
                                                        
                                                default:
                                                    break;
												}
											}

											uCurrentCount = pGlobalVariable->uCount;
										}
										else
											uCurrentCount = 1;
									}
									else
										uCurrentCount = 1;

									VariableOffsetMap.Insert(pConstantInstruction->uDestinationIndex, uOffset);

									GlobalInstruction.uDestinationIndex = uOffset;

									uOffset += GlobalInstruction.uDestinationIndexSize = GetRegisterCount(pVariable->Type) * uCurrentCount;

									GlobalInstruction.uDestinationIndexAddress = pConstantInstruction->uDestinationIndexAddress;
								}
								else
								{
									GlobalInstruction.uDestinationIndex        = 0;
									GlobalInstruction.uDestinationIndexSize    = 0;
									GlobalInstruction.uDestinationIndexAddress = 0;
								}
							}

							puOffset = VariableOffsetMap.Get(pConstantInstruction->uSource1);
							if(puOffset)
							{
								GlobalInstruction.uSource1 = *puOffset;

								pVariable = pConstantInstruction->uSource1 ? m_VariablePool.Get(pConstantInstruction->uSource1 - 1) : ALCHEMY_NULL;
								if(pVariable)
								{
									uCurrentCount = 1;
									if(pVariable->uGlobal)
									{
										pGlobalVariable = m_GlobalVariablePool.Get(pVariable->uGlobal - 1);
										if(pGlobalVariable)
											uCurrentCount = pGlobalVariable->uCount;
									}

									GlobalInstruction.uSourceSize1 = GetRegisterCount(pVariable->Type) * uCurrentCount;

									pAddressBufferPointer = pConstantInstruction->uSourceAddress1 ? m_BufferPool.Get(pConstantInstruction->uSourceAddress1 - 1) : ALCHEMY_NULL;
									GlobalInstruction.uSourceAddress1 = pAddressBufferPointer ? (pGlobalInputHandler->m_AddressBuffers.Add(*pAddressBufferPointer) + 1) : 0;
								}
								else
								{
									GlobalInstruction.uSourceSize1 = 0;

									GlobalInstruction.uSourceAddress1 = 0;
								}
							}
							else
							{
								pVariable = pConstantInstruction->uSource1 ? m_VariablePool.Get(pConstantInstruction->uSource1 - 1) : ALCHEMY_NULL;
								if(pVariable)
								{
									if(pVariable->uGlobal)
									{
										pGlobalVariable = m_GlobalVariablePool.Get(pVariable->uGlobal - 1);
										if(pGlobalVariable)
										{
											if(pGlobalVariable->GlobalType == INPUT_VARIABLE)
											{
												uRegister = 0;

												pCurrentVariable = pVariable;
												while(pCurrentVariable)
												{
													pVariable = pCurrentVariable;

													uRegister += pCurrentVariable->uRegister;
													pCurrentVariable = pCurrentVariable->uParent ? m_VariablePool.Get(pCurrentVariable->uParent - 1) : ALCHEMY_NULL;
												}

												if(pGlobalVariable->pValue)
												{
													if(!pGlobalInputHandler->m_puInputValueCount)
													{
														ALCHEMY_DEBUG_NEW(pGlobalInputHandler->m_puInputValueCount, UINT[uParameterListCount]);

														for(j = 0; j < uParameterListCount; ++ j)
															pGlobalInputHandler->m_puInputValueCount[j] = 0;
													}

													ConstantOffsetMap.Insert(uRegister, pGlobalInputHandler->m_puInputValueCount[uRenderTargetIndexCount] ++);
												}
												else
													ConstantOffsetMap.Insert(uRegister, uOffset);
											}
											else
											{
												if(!pGlobalInputHandler->m_ppParameterList)
												{
													ALCHEMY_DEBUG_NEW(pGlobalInputHandler->m_ppParameterList, CRenderMethod::LPPARAMETERLIST[uParameterListCount]);
													for(j = 0; j < uParameterListCount; ++ j)
														pGlobalInputHandler->m_ppParameterList[j] = ALCHEMY_NULL;
												}

												pCurrentConstantParameterList = pGlobalInputHandler->m_ppParameterList[uRenderTargetIndexCount];
												if(!pCurrentConstantParameterList)
												{
													ALCHEMY_DEBUG_NEW(pCurrentConstantParameterList, CRenderMethod::PARAMETERLIST);

													pCurrentConstantParameterList->pTextures = ALCHEMY_NULL;
													pCurrentConstantParameterList->pSamplerDeclaration = ALCHEMY_NULL;
													pCurrentConstantParameterList->pCameraParameters = ALCHEMY_NULL;
													pCurrentConstantParameterList->pLightParameters = ALCHEMY_NULL;
													pCurrentConstantParameterList->pTextureParameters = ALCHEMY_NULL;

													pCurrentConstantParameterList->uParameterFlag = 0;
													pCurrentConstantParameterList->uArrayParameterFlag = 0;
													//pCurrentConstantParameterList->uMatrixParameterFlag = 0;
													//pCurrentConstantParameterList->uCameraParameterFlag = 0;
													pCurrentConstantParameterList->uMaterialParameterFlag = 0;
													pCurrentConstantParameterList->uConstantParameterFlag = 0;

													pCurrentConstantParameterList->puCameraParameterFlag = ALCHEMY_NULL;
													pCurrentConstantParameterList->puLightParameterFlag = ALCHEMY_NULL;
													pCurrentConstantParameterList->puTextureParameterFlag = ALCHEMY_NULL;

													pCurrentConstantParameterList->uCameraCount = 0;
													pCurrentConstantParameterList->uLightCount = 0;
													pCurrentConstantParameterList->uTextureCount = 0;

													pGlobalInputHandler->m_ppParameterList[uRenderTargetIndexCount] = pCurrentConstantParameterList;
												}

												switch(pGlobalVariable->GlobalType)
												{
												case NORMAL_VARIABLE:
													if(!pGlobalVariable->uIndex && pGlobalVariable->uDetail < IRenderMethod::TOTAL_PARAMETERS)
													{
														pParameterInfo = &pCurrentConstantParameterList->Parameters[pGlobalVariable->uDetail];

														if(ALCHEMY_TEST_BIT(pCurrentConstantParameterList->uParameterFlag, pGlobalVariable->uDetail) && pGlobalVariable->ShaderType != pParameterInfo->Type)
														{
															if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
																pParameterInfo->uBetaIndex = uOffset;
															else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
																pParameterInfo->uAlphaIndex = uOffset;

															pParameterInfo->Type = IEffect::PROGRAM;
														}
														else
														{
															pParameterInfo->Type        = pGlobalVariable->ShaderType;
															pParameterInfo->uAlphaIndex = uOffset;
														}

														ALCHEMY_SET_BIT(pCurrentConstantParameterList->uParameterFlag, pGlobalVariable->uDetail);
													}
													break;

												case ARRAY_VARIABLE:
													if(!pGlobalVariable->uIndex && pGlobalVariable->uDetail < IRenderMethod::TOTAL_ARRAY_PARAMETERS)
													{
														pParameterInfo = &pCurrentConstantParameterList->ArrayParameters[pGlobalVariable->uDetail];

														if(ALCHEMY_TEST_BIT(pCurrentConstantParameterList->uArrayParameterFlag, pGlobalVariable->uDetail) && pGlobalVariable->ShaderType != pParameterInfo->Type)
														{
															if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
																pParameterInfo->uBetaIndex = uOffset;
															else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
																pParameterInfo->uAlphaIndex = uOffset;

															pParameterInfo->Type = IEffect::PROGRAM;
														}
														else
														{
															pParameterInfo->Type        = pGlobalVariable->ShaderType;
															pParameterInfo->uAlphaIndex = uOffset;
														}

														ALCHEMY_SET_BIT(pCurrentConstantParameterList->uArrayParameterFlag, pGlobalVariable->uDetail);
													}
													break;

												/*case MATRIX_VARIABLE:
													if(!pGlobalVariable->uIndex && pGlobalVariable->uDetail < CRenderMethod::TOTAL_MATRIX_PARAMETERS)
													{
														pParameterInfo = &pCurrentConstantParameterList->MatrixParameters[pGlobalVariable->uDetail];

														if(ALCHEMY_TEST_BIT(pCurrentConstantParameterList->uMatrixParameterFlag, pGlobalVariable->uDetail) && pGlobalVariable->ShaderType != pParameterInfo->Type)
														{
															if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
																pParameterInfo->uBetaIndex = uOffset;
															else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
																pParameterInfo->uAlphaIndex = uOffset;

															pParameterInfo->Type = IEffect::PROGRAM;
														}
														else
														{
															pParameterInfo->Type        = pGlobalVariable->ShaderType;
															pParameterInfo->uAlphaIndex = uOffset;
														}

														ALCHEMY_SET_BIT(pCurrentConstantParameterList->uMatrixParameterFlag, pGlobalVariable->uDetail);
													}
													break;
												case CAMERA_VARIABLE:
													if(!pGlobalVariable->uIndex && pGlobalVariable->uDetail < CRenderMethod::TOTAL_CAMERA_PARAMETERS)
													{
														pParameterInfo = &pCurrentConstantParameterList->CameraParameters[pGlobalVariable->uDetail];

														if(ALCHEMY_TEST_BIT(pCurrentConstantParameterList->uCameraParameterFlag, pGlobalVariable->uDetail) && pGlobalVariable->ShaderType != pParameterInfo->Type)
														{
															if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
																pParameterInfo->uBetaIndex = uOffset;
															else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
																pParameterInfo->uAlphaIndex = uOffset;

															pParameterInfo->Type = IEffect::PROGRAM;
														}
														else
														{
															pParameterInfo->Type        = pGlobalVariable->ShaderType;
															pParameterInfo->uAlphaIndex = uOffset;
														}

														ALCHEMY_SET_BIT(pCurrentConstantParameterList->uCameraParameterFlag, pGlobalVariable->uDetail);
													}
													break;*/

												case MATERIAL_VARIABLE:
													if(!pGlobalVariable->uIndex && pGlobalVariable->uDetail < CRenderMethod::TOTAL_MATERIAL_PARAMETERS)
													{
														pParameterInfo = &pCurrentConstantParameterList->MaterialParameters[pGlobalVariable->uDetail];

														if(ALCHEMY_TEST_BIT(pCurrentConstantParameterList->uMaterialParameterFlag, pGlobalVariable->uDetail) && pGlobalVariable->ShaderType != pParameterInfo->Type)
														{
															if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
																pParameterInfo->uBetaIndex = uOffset;
															else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
																pParameterInfo->uAlphaIndex = uOffset;

															pParameterInfo->Type = IEffect::PROGRAM;
														}
														else
														{
															pParameterInfo->Type        = pGlobalVariable->ShaderType;
															pParameterInfo->uAlphaIndex = uOffset;
														}

														ALCHEMY_SET_BIT(pCurrentConstantParameterList->uMaterialParameterFlag, pGlobalVariable->uDetail);
													}
													break;

												case CAMERA_VARIABLE:
													if(pGlobalVariable->uDetail < CRenderMethod::TOTAL_CAMERA_PARAMETERS)
													{
														uCurrentCount = pGlobalVariable->uIndex + 1;

														pCurrentConstantParameterList->uCameraCount = ALCHEMY_MAXINUM(pCurrentConstantParameterList->uCameraCount, uCurrentCount);
													}
													break;

												case LIGHT_VARIABLE:
													if(pGlobalVariable->uDetail < CRenderMethod::TOTAL_LIGHT_PARAMETERS)
													{
														uCurrentCount = pGlobalVariable->uIndex + 1;

														pCurrentConstantParameterList->uLightCount = ALCHEMY_MAXINUM(pCurrentConstantParameterList->uLightCount, uCurrentCount);
													}
													break;

												case CONSTANT_VARIABLE:
													if(!pGlobalVariable->uIndex && pGlobalVariable->uDetail < CRenderMethod::TOTAL_CONSTANT_PARAMETERS)
													{
														pParameterInfo = &pCurrentConstantParameterList->ConstantParameters[pGlobalVariable->uDetail];

														if(ALCHEMY_TEST_BIT(pCurrentConstantParameterList->uConstantParameterFlag, pGlobalVariable->uDetail) && pGlobalVariable->ShaderType != pParameterInfo->Type)
														{
															if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
																pParameterInfo->uBetaIndex = uOffset;
															else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
																pParameterInfo->uAlphaIndex = uOffset;

															pParameterInfo->Type = IEffect::PROGRAM;
														}
														else
														{
															pParameterInfo->Type        = pGlobalVariable->ShaderType;
															pParameterInfo->uAlphaIndex = uOffset;
														}

														ALCHEMY_SET_BIT(pCurrentConstantParameterList->uConstantParameterFlag, pGlobalVariable->uDetail);
													}
													break;
                                                        
                                                default:
                                                    break;
												}
											}

											uCurrentCount = pGlobalVariable->uCount;
										}
										else
											uCurrentCount = 1;
									}
									else
										uCurrentCount = 1;

									VariableOffsetMap.Insert(pConstantInstruction->uSource1, uOffset);

									GlobalInstruction.uSource1 = uOffset;

									uOffset += GlobalInstruction.uSourceSize1 = GetRegisterCount(pVariable->Type) * uCurrentCount;

									pAddressBufferPointer = pConstantInstruction->uSourceAddress1 ? m_BufferPool.Get(pConstantInstruction->uSourceAddress1 - 1) : ALCHEMY_NULL;
									GlobalInstruction.uSourceAddress1 = pAddressBufferPointer ? (pGlobalInputHandler->m_AddressBuffers.Add(*pAddressBufferPointer) + 1) : 0;
								}
								else
								{
									GlobalInstruction.uSource1        = 0;
									GlobalInstruction.uSourceSize1    = 0;
									GlobalInstruction.uSourceAddress1 = 0;
								}
							}

							puOffset = VariableOffsetMap.Get(pConstantInstruction->uSourceIndex1);
							if(puOffset)
							{
								GlobalInstruction.uSourceIndex1 = *puOffset;

								pVariable = pConstantInstruction->uSourceIndex1 ? m_VariablePool.Get(pConstantInstruction->uSourceIndex1 - 1) : ALCHEMY_NULL;
								if(pVariable)
								{
									uCurrentCount = 1;
									if(pVariable->uGlobal)
									{
										pGlobalVariable = m_GlobalVariablePool.Get(pVariable->uGlobal - 1);
										if(pGlobalVariable)
											uCurrentCount = pGlobalVariable->uCount;
									}

									GlobalInstruction.uSourceIndexSize1 = GetRegisterCount(pVariable->Type) * uCurrentCount;

									GlobalInstruction.uSourceIndexAddress1 = pConstantInstruction->uSourceIndexAddress1;
								}
								else
								{
									GlobalInstruction.uSourceIndexSize1 = 0;

									GlobalInstruction.uSourceIndexAddress1 = 0;
								}
							}
							else
							{
								pVariable = pConstantInstruction->uSourceIndex1 ? m_VariablePool.Get(pConstantInstruction->uSourceIndex1 - 1) : ALCHEMY_NULL;
								if(pVariable)
								{
									if(pVariable->uGlobal)
									{
										pGlobalVariable = m_GlobalVariablePool.Get(pVariable->uGlobal - 1);
										if(pGlobalVariable)
										{
											if(pGlobalVariable->GlobalType == INPUT_VARIABLE)
											{
												uRegister = 0;

												pCurrentVariable = pVariable;
												while(pCurrentVariable)
												{
													pVariable = pCurrentVariable;

													uRegister += pCurrentVariable->uRegister;
													pCurrentVariable = pCurrentVariable->uParent ? m_VariablePool.Get(pCurrentVariable->uParent - 1) : ALCHEMY_NULL;
												}

												if(pGlobalVariable->pValue)
												{
													if(!pGlobalInputHandler->m_puInputValueCount)
													{
														ALCHEMY_DEBUG_NEW(pGlobalInputHandler->m_puInputValueCount, UINT[uParameterListCount]);

														for(j = 0; j < uParameterListCount; ++ j)
															pGlobalInputHandler->m_puInputValueCount[j] = 0;
													}

													ConstantOffsetMap.Insert(uRegister, pGlobalInputHandler->m_puInputValueCount[uRenderTargetIndexCount] ++);
												}
												else
													ConstantOffsetMap.Insert(uRegister, uOffset);
											}
											else
											{
												if(!pGlobalInputHandler->m_ppParameterList)
												{
													ALCHEMY_DEBUG_NEW(pGlobalInputHandler->m_ppParameterList, CRenderMethod::LPPARAMETERLIST[uParameterListCount]);
													for(j = 0; j < uParameterListCount; ++ j)
														pGlobalInputHandler->m_ppParameterList[j] = ALCHEMY_NULL;
												}

												pCurrentConstantParameterList = pGlobalInputHandler->m_ppParameterList[uRenderTargetIndexCount];
												if(!pCurrentConstantParameterList)
												{
													ALCHEMY_DEBUG_NEW(pCurrentConstantParameterList, CRenderMethod::PARAMETERLIST);

													pCurrentConstantParameterList->pTextures = ALCHEMY_NULL;
													pCurrentConstantParameterList->pSamplerDeclaration = ALCHEMY_NULL;
													pCurrentConstantParameterList->pCameraParameters = ALCHEMY_NULL;
													pCurrentConstantParameterList->pLightParameters = ALCHEMY_NULL;
													pCurrentConstantParameterList->pTextureParameters = ALCHEMY_NULL;

													pCurrentConstantParameterList->uParameterFlag = 0;
													pCurrentConstantParameterList->uArrayParameterFlag = 0;
													//pCurrentConstantParameterList->uMatrixParameterFlag = 0;
													//pCurrentConstantParameterList->uCameraParameterFlag = 0;
													pCurrentConstantParameterList->uMaterialParameterFlag = 0;
													pCurrentConstantParameterList->uConstantParameterFlag = 0;

													pCurrentConstantParameterList->puCameraParameterFlag = ALCHEMY_NULL;
													pCurrentConstantParameterList->puLightParameterFlag = ALCHEMY_NULL;
													pCurrentConstantParameterList->puTextureParameterFlag = ALCHEMY_NULL;

													pCurrentConstantParameterList->uCameraCount = 0;
													pCurrentConstantParameterList->uLightCount = 0;
													pCurrentConstantParameterList->uTextureCount = 0;

													pGlobalInputHandler->m_ppParameterList[uRenderTargetIndexCount] = pCurrentConstantParameterList;
												}

												switch(pGlobalVariable->GlobalType)
												{
												case NORMAL_VARIABLE:
													if(!pGlobalVariable->uIndex && pGlobalVariable->uDetail < IRenderMethod::TOTAL_PARAMETERS)
													{
														pParameterInfo = &pCurrentConstantParameterList->Parameters[pGlobalVariable->uDetail];

														if(ALCHEMY_TEST_BIT(pCurrentConstantParameterList->uParameterFlag, pGlobalVariable->uDetail) && pGlobalVariable->ShaderType != pParameterInfo->Type)
														{
															if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
																pParameterInfo->uBetaIndex = uOffset;
															else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
																pParameterInfo->uAlphaIndex = uOffset;

															pParameterInfo->Type = IEffect::PROGRAM;
														}
														else
														{
															pParameterInfo->Type        = pGlobalVariable->ShaderType;
															pParameterInfo->uAlphaIndex = uOffset;
														}

														ALCHEMY_SET_BIT(pCurrentConstantParameterList->uParameterFlag, pGlobalVariable->uDetail);
													}
													break;

												case ARRAY_VARIABLE:
													if(!pGlobalVariable->uIndex && pGlobalVariable->uDetail < IRenderMethod::TOTAL_ARRAY_PARAMETERS)
													{
														pParameterInfo = &pCurrentConstantParameterList->ArrayParameters[pGlobalVariable->uDetail];

														if(ALCHEMY_TEST_BIT(pCurrentConstantParameterList->uArrayParameterFlag, pGlobalVariable->uDetail) && pGlobalVariable->ShaderType != pParameterInfo->Type)
														{
															if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
																pParameterInfo->uBetaIndex = uOffset;
															else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
																pParameterInfo->uAlphaIndex = uOffset;

															pParameterInfo->Type = IEffect::PROGRAM;
														}
														else
														{
															pParameterInfo->Type        = pGlobalVariable->ShaderType;
															pParameterInfo->uAlphaIndex = uOffset;
														}

														ALCHEMY_SET_BIT(pCurrentConstantParameterList->uArrayParameterFlag, pGlobalVariable->uDetail);
													}
													break;

												/*case MATRIX_VARIABLE:
													if(!pGlobalVariable->uIndex && pGlobalVariable->uDetail < CRenderMethod::TOTAL_MATRIX_PARAMETERS)
													{
														pParameterInfo = &pCurrentConstantParameterList->MatrixParameters[pGlobalVariable->uDetail];

														if(ALCHEMY_TEST_BIT(pCurrentConstantParameterList->uMatrixParameterFlag, pGlobalVariable->uDetail) && pGlobalVariable->ShaderType != pParameterInfo->Type)
														{
															if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
																pParameterInfo->uBetaIndex = uOffset;
															else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
																pParameterInfo->uAlphaIndex = uOffset;

															pParameterInfo->Type = IEffect::PROGRAM;
														}
														else
														{
															pParameterInfo->Type        = pGlobalVariable->ShaderType;
															pParameterInfo->uAlphaIndex = uOffset;
														}

														ALCHEMY_SET_BIT(pCurrentConstantParameterList->uMatrixParameterFlag, pGlobalVariable->uDetail);
													}
													break;
												case CAMERA_VARIABLE:
													if(!pGlobalVariable->uIndex && pGlobalVariable->uDetail < CRenderMethod::TOTAL_CAMERA_PARAMETERS)
													{
														pParameterInfo = &pCurrentConstantParameterList->CameraParameters[pGlobalVariable->uDetail];

														if(ALCHEMY_TEST_BIT(pCurrentConstantParameterList->uCameraParameterFlag, pGlobalVariable->uDetail) && pGlobalVariable->ShaderType != pParameterInfo->Type)
														{
															if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
																pParameterInfo->uBetaIndex = uOffset;
															else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
																pParameterInfo->uAlphaIndex = uOffset;

															pParameterInfo->Type = IEffect::PROGRAM;
														}
														else
														{
															pParameterInfo->Type        = pGlobalVariable->ShaderType;
															pParameterInfo->uAlphaIndex = uOffset;
														}

														ALCHEMY_SET_BIT(pCurrentConstantParameterList->uCameraParameterFlag, pGlobalVariable->uDetail);
													}
													break;*/

												case MATERIAL_VARIABLE:
													if(!pGlobalVariable->uIndex && pGlobalVariable->uDetail < CRenderMethod::TOTAL_MATERIAL_PARAMETERS)
													{
														pParameterInfo = &pCurrentConstantParameterList->MaterialParameters[pGlobalVariable->uDetail];

														if(ALCHEMY_TEST_BIT(pCurrentConstantParameterList->uMaterialParameterFlag, pGlobalVariable->uDetail) && pGlobalVariable->ShaderType != pParameterInfo->Type)
														{
															if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
																pParameterInfo->uBetaIndex = uOffset;
															else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
																pParameterInfo->uAlphaIndex = uOffset;

															pParameterInfo->Type = IEffect::PROGRAM;
														}
														else
														{
															pParameterInfo->Type        = pGlobalVariable->ShaderType;
															pParameterInfo->uAlphaIndex = uOffset;
														}

														ALCHEMY_SET_BIT(pCurrentConstantParameterList->uMaterialParameterFlag, pGlobalVariable->uDetail);
													}
													break;

												case CAMERA_VARIABLE:
													if(pGlobalVariable->uDetail < CRenderMethod::TOTAL_CAMERA_PARAMETERS)
													{
														uCurrentCount = pGlobalVariable->uIndex + 1;

														pCurrentConstantParameterList->uCameraCount = ALCHEMY_MAXINUM(pCurrentConstantParameterList->uCameraCount, uCurrentCount);
													}
													break;

												case LIGHT_VARIABLE:
													if(pGlobalVariable->uDetail < CRenderMethod::TOTAL_LIGHT_PARAMETERS)
													{
														uCurrentCount = pGlobalVariable->uIndex + 1;

														pCurrentConstantParameterList->uLightCount = ALCHEMY_MAXINUM(pCurrentConstantParameterList->uLightCount, uCurrentCount);
													}
													break;

												case CONSTANT_VARIABLE:
													if(!pGlobalVariable->uIndex && pGlobalVariable->uDetail < CRenderMethod::TOTAL_CONSTANT_PARAMETERS)
													{
														pParameterInfo = &pCurrentConstantParameterList->ConstantParameters[pGlobalVariable->uDetail];

														if(ALCHEMY_TEST_BIT(pCurrentConstantParameterList->uConstantParameterFlag, pGlobalVariable->uDetail) && pGlobalVariable->ShaderType != pParameterInfo->Type)
														{
															if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
																pParameterInfo->uBetaIndex = uOffset;
															else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
																pParameterInfo->uAlphaIndex = uOffset;

															pParameterInfo->Type = IEffect::PROGRAM;
														}
														else
														{
															pParameterInfo->Type        = pGlobalVariable->ShaderType;
															pParameterInfo->uAlphaIndex = uOffset;
														}

														ALCHEMY_SET_BIT(pCurrentConstantParameterList->uConstantParameterFlag, pGlobalVariable->uDetail);
													}
													break;
                                                default:
                                                    break;
												}
											}

											uCurrentCount = pGlobalVariable->uCount;
										}
										else
											uCurrentCount = 1;
									}
									else
										uCurrentCount = 1;

									VariableOffsetMap.Insert(pConstantInstruction->uSourceIndex1, uOffset);

									GlobalInstruction.uSourceIndex1 = uOffset;

									uOffset += GlobalInstruction.uSourceIndexSize1 = GetRegisterCount(pVariable->Type) * uCurrentCount;

									GlobalInstruction.uSourceIndexAddress1 = pConstantInstruction->uSourceIndexAddress1;
								}
								else
								{
									GlobalInstruction.uSourceIndex1        = 0;
									GlobalInstruction.uSourceIndexSize1    = 0;
									GlobalInstruction.uSourceIndexAddress1 = 0;
								}
							}

							puOffset = VariableOffsetMap.Get(pConstantInstruction->uSource2);
							if(puOffset)
							{
								GlobalInstruction.uSource2 = *puOffset;

								pVariable = pConstantInstruction->uSource2 ? m_VariablePool.Get(pConstantInstruction->uSource2 - 1) : ALCHEMY_NULL;
								if(pVariable)
								{
									uCurrentCount = 1;
									if(pVariable->uGlobal)
									{
										pGlobalVariable = m_GlobalVariablePool.Get(pVariable->uGlobal - 1);
										if(pGlobalVariable)
											uCurrentCount = pGlobalVariable->uCount;
									}

									GlobalInstruction.uSourceSize2 = GetRegisterCount(pVariable->Type) * uCurrentCount;

									pAddressBufferPointer = pConstantInstruction->uSourceAddress2 ? m_BufferPool.Get(pConstantInstruction->uSourceAddress2 - 1) : ALCHEMY_NULL;
									GlobalInstruction.uSourceAddress2 = pAddressBufferPointer ? (pGlobalInputHandler->m_AddressBuffers.Add(*pAddressBufferPointer) + 1) : 0;
								}
								else
								{
									GlobalInstruction.uSourceSize2 = 0;

									GlobalInstruction.uSourceAddress2 = 0;
								}
							}
							else
							{
								pVariable = pConstantInstruction->uSource2 ? m_VariablePool.Get(pConstantInstruction->uSource2 - 1) : ALCHEMY_NULL;
								if(pVariable)
								{
									if(pVariable->uGlobal)
									{
										pGlobalVariable = m_GlobalVariablePool.Get(pVariable->uGlobal - 1);
										if(pGlobalVariable)
										{
											if(pGlobalVariable->GlobalType == INPUT_VARIABLE)
											{
												uRegister = 0;

												pCurrentVariable = pVariable;
												while(pCurrentVariable)
												{
													pVariable = pCurrentVariable;

													uRegister += pCurrentVariable->uRegister;
													pCurrentVariable = pCurrentVariable->uParent ? m_VariablePool.Get(pCurrentVariable->uParent - 1) : ALCHEMY_NULL;
												}

												if(pGlobalVariable->pValue)
												{
													if(!pGlobalInputHandler->m_puInputValueCount)
													{
														ALCHEMY_DEBUG_NEW(pGlobalInputHandler->m_puInputValueCount, UINT[uParameterListCount]);

														for(j = 0; j < uParameterListCount; ++ j)
															pGlobalInputHandler->m_puInputValueCount[j] = 0;
													}

													ConstantOffsetMap.Insert(uRegister, pGlobalInputHandler->m_puInputValueCount[uRenderTargetIndexCount] ++);
												}
												else
													ConstantOffsetMap.Insert(uRegister, uOffset);
											}
											else
											{
												if(!pGlobalInputHandler->m_ppParameterList)
												{
													ALCHEMY_DEBUG_NEW(pGlobalInputHandler->m_ppParameterList, CRenderMethod::LPPARAMETERLIST[uParameterListCount]);
													for(j = 0; j < uParameterListCount; ++ j)
														pGlobalInputHandler->m_ppParameterList[j] = ALCHEMY_NULL;
												}

												pCurrentConstantParameterList = pGlobalInputHandler->m_ppParameterList[uRenderTargetIndexCount];
												if(!pCurrentConstantParameterList)
												{
													ALCHEMY_DEBUG_NEW(pCurrentConstantParameterList, CRenderMethod::PARAMETERLIST);

													pCurrentConstantParameterList->pTextures = ALCHEMY_NULL;
													pCurrentConstantParameterList->pSamplerDeclaration = ALCHEMY_NULL;
													
													pCurrentConstantParameterList->pCameraParameters = ALCHEMY_NULL;
													pCurrentConstantParameterList->pLightParameters = ALCHEMY_NULL;
													pCurrentConstantParameterList->pTextureParameters = ALCHEMY_NULL;

													pCurrentConstantParameterList->uParameterFlag = 0;
													pCurrentConstantParameterList->uArrayParameterFlag = 0;
													//pCurrentConstantParameterList->uMatrixParameterFlag = 0;
													//pCurrentConstantParameterList->uCameraParameterFlag = 0;
													pCurrentConstantParameterList->uMaterialParameterFlag = 0;
													pCurrentConstantParameterList->uConstantParameterFlag = 0;

													pCurrentConstantParameterList->puCameraParameterFlag = ALCHEMY_NULL;
													pCurrentConstantParameterList->puLightParameterFlag = ALCHEMY_NULL;
													pCurrentConstantParameterList->puTextureParameterFlag = ALCHEMY_NULL;

													pCurrentConstantParameterList->uCameraCount = 0;
													pCurrentConstantParameterList->uLightCount = 0;
													pCurrentConstantParameterList->uTextureCount = 0;

													pGlobalInputHandler->m_ppParameterList[uRenderTargetIndexCount] = pCurrentConstantParameterList;
												}

												switch(pGlobalVariable->GlobalType)
												{
												case NORMAL_VARIABLE:
													if(!pGlobalVariable->uIndex && pGlobalVariable->uDetail < IRenderMethod::TOTAL_PARAMETERS)
													{
														pParameterInfo = &pCurrentConstantParameterList->Parameters[pGlobalVariable->uDetail];

														if(ALCHEMY_TEST_BIT(pCurrentConstantParameterList->uParameterFlag, pGlobalVariable->uDetail) && pGlobalVariable->ShaderType != pParameterInfo->Type)
														{
															if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
																pParameterInfo->uBetaIndex = uOffset;
															else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
																pParameterInfo->uAlphaIndex = uOffset;

															pParameterInfo->Type = IEffect::PROGRAM;
														}
														else
														{
															pParameterInfo->Type        = pGlobalVariable->ShaderType;
															pParameterInfo->uAlphaIndex = uOffset;
														}

														ALCHEMY_SET_BIT(pCurrentConstantParameterList->uParameterFlag, pGlobalVariable->uDetail);
													}
													break;

												case ARRAY_VARIABLE:
													if(!pGlobalVariable->uIndex && pGlobalVariable->uDetail < IRenderMethod::TOTAL_ARRAY_PARAMETERS)
													{
														pParameterInfo = &pCurrentConstantParameterList->ArrayParameters[pGlobalVariable->uDetail];

														if(ALCHEMY_TEST_BIT(pCurrentConstantParameterList->uArrayParameterFlag, pGlobalVariable->uDetail) && pGlobalVariable->ShaderType != pParameterInfo->Type)
														{
															if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
																pParameterInfo->uBetaIndex = uOffset;
															else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
																pParameterInfo->uAlphaIndex = uOffset;

															pParameterInfo->Type = IEffect::PROGRAM;
														}
														else
														{
															pParameterInfo->Type        = pGlobalVariable->ShaderType;
															pParameterInfo->uAlphaIndex = uOffset;
														}

														ALCHEMY_SET_BIT(pCurrentConstantParameterList->uArrayParameterFlag, pGlobalVariable->uDetail);
													}
													break;

												/*case MATRIX_VARIABLE:
													if(!pGlobalVariable->uIndex && pGlobalVariable->uDetail < CRenderMethod::TOTAL_MATRIX_PARAMETERS)
													{
														pParameterInfo = &pCurrentConstantParameterList->MatrixParameters[pGlobalVariable->uDetail];

														if(ALCHEMY_TEST_BIT(pCurrentConstantParameterList->uMatrixParameterFlag, pGlobalVariable->uDetail) && pGlobalVariable->ShaderType != pParameterInfo->Type)
														{
															if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
																pParameterInfo->uBetaIndex = uOffset;
															else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
																pParameterInfo->uAlphaIndex = uOffset;

															pParameterInfo->Type = IEffect::PROGRAM;
														}
														else
														{
															pParameterInfo->Type        = pGlobalVariable->ShaderType;
															pParameterInfo->uAlphaIndex = uOffset;
														}

														ALCHEMY_SET_BIT(pCurrentConstantParameterList->uMatrixParameterFlag, pGlobalVariable->uDetail);
													}
													break;
												case CAMERA_VARIABLE:
													if(!pGlobalVariable->uIndex && pGlobalVariable->uDetail < CRenderMethod::TOTAL_CAMERA_PARAMETERS)
													{
														pParameterInfo = &pCurrentConstantParameterList->CameraParameters[pGlobalVariable->uDetail];

														if(ALCHEMY_TEST_BIT(pCurrentConstantParameterList->uCameraParameterFlag, pGlobalVariable->uDetail) && pGlobalVariable->ShaderType != pParameterInfo->Type)
														{
															if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
																pParameterInfo->uBetaIndex = uOffset;
															else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
																pParameterInfo->uAlphaIndex = uOffset;

															pParameterInfo->Type = IEffect::PROGRAM;
														}
														else
														{
															pParameterInfo->Type        = pGlobalVariable->ShaderType;
															pParameterInfo->uAlphaIndex = uOffset;
														}

														ALCHEMY_SET_BIT(pCurrentConstantParameterList->uCameraParameterFlag, pGlobalVariable->uDetail);
													}
													break;*/

												case MATERIAL_VARIABLE:
													if(!pGlobalVariable->uIndex && pGlobalVariable->uDetail < CRenderMethod::TOTAL_MATERIAL_PARAMETERS)
													{
														pParameterInfo = &pCurrentConstantParameterList->MaterialParameters[pGlobalVariable->uDetail];

														if(ALCHEMY_TEST_BIT(pCurrentConstantParameterList->uMaterialParameterFlag, pGlobalVariable->uDetail) && pGlobalVariable->ShaderType != pParameterInfo->Type)
														{
															if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
																pParameterInfo->uBetaIndex = uOffset;
															else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
																pParameterInfo->uAlphaIndex = uOffset;

															pParameterInfo->Type = IEffect::PROGRAM;
														}
														else
														{
															pParameterInfo->Type        = pGlobalVariable->ShaderType;
															pParameterInfo->uAlphaIndex = uOffset;
														}

														ALCHEMY_SET_BIT(pCurrentConstantParameterList->uMaterialParameterFlag, pGlobalVariable->uDetail);
													}
													break;

												case CAMERA_VARIABLE:
													if(pGlobalVariable->uDetail < CRenderMethod::TOTAL_CAMERA_PARAMETERS)
													{
														uCurrentCount = pGlobalVariable->uIndex + 1;

														pCurrentConstantParameterList->uCameraCount = ALCHEMY_MAXINUM(pCurrentConstantParameterList->uCameraCount, uCurrentCount);
													}
													break;

												case LIGHT_VARIABLE:
													if(pGlobalVariable->uDetail < CRenderMethod::TOTAL_LIGHT_PARAMETERS)
													{
														uCurrentCount = pGlobalVariable->uIndex + 1;

														pCurrentConstantParameterList->uLightCount = ALCHEMY_MAXINUM(pCurrentConstantParameterList->uLightCount, uCurrentCount);
													}
													break;

												case CONSTANT_VARIABLE:
													if(!pGlobalVariable->uIndex && pGlobalVariable->uDetail < CRenderMethod::TOTAL_CONSTANT_PARAMETERS)
													{
														pParameterInfo = &pCurrentConstantParameterList->ConstantParameters[pGlobalVariable->uDetail];

														if(ALCHEMY_TEST_BIT(pCurrentConstantParameterList->uConstantParameterFlag, pGlobalVariable->uDetail) && pGlobalVariable->ShaderType != pParameterInfo->Type)
														{
															if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
																pParameterInfo->uBetaIndex = uOffset;
															else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
																pParameterInfo->uAlphaIndex = uOffset;

															pParameterInfo->Type = IEffect::PROGRAM;
														}
														else
														{
															pParameterInfo->Type        = pGlobalVariable->ShaderType;
															pParameterInfo->uAlphaIndex = uOffset;
														}

														ALCHEMY_SET_BIT(pCurrentConstantParameterList->uConstantParameterFlag, pGlobalVariable->uDetail);
													}
													break;
                                                default:
                                                    break;
												}
											}

											uCurrentCount = pGlobalVariable->uCount;
										}
										else
											uCurrentCount = 1;
									}
									else
										uCurrentCount = 1;

									VariableOffsetMap.Insert(pConstantInstruction->uSource2, uOffset);

									GlobalInstruction.uSource2 = uOffset;

									uOffset += GlobalInstruction.uSourceSize2 = GetRegisterCount(pVariable->Type) * uCurrentCount;

									pAddressBufferPointer = pConstantInstruction->uSourceAddress2 ? m_BufferPool.Get(pConstantInstruction->uSourceAddress2 - 1) : ALCHEMY_NULL;
									GlobalInstruction.uSourceAddress2 = pAddressBufferPointer ? (pGlobalInputHandler->m_AddressBuffers.Add(*pAddressBufferPointer) + 1) : 0;
								}
								else
								{
									GlobalInstruction.uSource2        = 0;
									GlobalInstruction.uSourceSize2    = 0;
									GlobalInstruction.uSourceAddress2 = 0;
								}
							}

							puOffset = VariableOffsetMap.Get(pConstantInstruction->uSourceIndex2);
							if(puOffset)
							{
								GlobalInstruction.uSourceIndex2 = *puOffset;

								pVariable = pConstantInstruction->uSourceIndex2 ? m_VariablePool.Get(pConstantInstruction->uSourceIndex2 - 1) : ALCHEMY_NULL;
								if(pVariable)
								{
									uCurrentCount = 1;
									if(pVariable->uGlobal)
									{
										pGlobalVariable = m_GlobalVariablePool.Get(pVariable->uGlobal - 1);
										if(pGlobalVariable)
											uCurrentCount = pGlobalVariable->uCount;
									}

									GlobalInstruction.uSourceIndexSize2 = GetRegisterCount(pVariable->Type) * uCurrentCount;

									pAddressBufferPointer = pConstantInstruction->uSourceIndexAddress2 ? m_BufferPool.Get(pConstantInstruction->uSourceIndexAddress2 - 1) : ALCHEMY_NULL;
									GlobalInstruction.uSourceIndexAddress2 = pAddressBufferPointer ? (pGlobalInputHandler->m_AddressBuffers.Add(*pAddressBufferPointer) + 1) : 0;
								}
								else
								{
									GlobalInstruction.uSourceIndexSize2 = 0;

									GlobalInstruction.uSourceIndexAddress2 = 0;
								}
							}
							else
							{
								pVariable = pConstantInstruction->uSourceIndex2 ? m_VariablePool.Get(pConstantInstruction->uSourceIndex2 - 1) : ALCHEMY_NULL;
								if(pVariable)
								{
									if(pVariable->uGlobal)
									{
										pGlobalVariable = m_GlobalVariablePool.Get(pVariable->uGlobal - 1);
										if(pGlobalVariable)
										{
											if(pGlobalVariable->GlobalType == INPUT_VARIABLE)
											{
												uRegister = 0;

												pCurrentVariable = pVariable;
												while(pCurrentVariable)
												{
													pVariable = pCurrentVariable;

													uRegister += pCurrentVariable->uRegister;
													pCurrentVariable = pCurrentVariable->uParent ? m_VariablePool.Get(pCurrentVariable->uParent - 1) : ALCHEMY_NULL;
												}

												if(pGlobalVariable->pValue)
												{
													if(!pGlobalInputHandler->m_puInputValueCount)
													{
														ALCHEMY_DEBUG_NEW(pGlobalInputHandler->m_puInputValueCount, UINT[uParameterListCount]);

														for(j = 0; j < uParameterListCount; ++ j)
															pGlobalInputHandler->m_puInputValueCount[j] = 0;
													}

													ConstantOffsetMap.Insert(uRegister, pGlobalInputHandler->m_puInputValueCount[uRenderTargetIndexCount] ++);
												}
												else
													ConstantOffsetMap.Insert(uRegister, uOffset);
											}
											else
											{
												if(!pGlobalInputHandler->m_ppParameterList)
												{
													ALCHEMY_DEBUG_NEW(pGlobalInputHandler->m_ppParameterList, CRenderMethod::LPPARAMETERLIST[uParameterListCount]);
													for(j = 0; j < uParameterListCount; ++ j)
														pGlobalInputHandler->m_ppParameterList[j] = ALCHEMY_NULL;
												}

												pCurrentConstantParameterList = pGlobalInputHandler->m_ppParameterList[uRenderTargetIndexCount];
												if(!pCurrentConstantParameterList)
												{
													ALCHEMY_DEBUG_NEW(pCurrentConstantParameterList, CRenderMethod::PARAMETERLIST);

													pCurrentConstantParameterList->pTextures = ALCHEMY_NULL;
													pCurrentConstantParameterList->pSamplerDeclaration = ALCHEMY_NULL;
													
													pCurrentConstantParameterList->pCameraParameters = ALCHEMY_NULL;
													pCurrentConstantParameterList->pLightParameters = ALCHEMY_NULL;
													pCurrentConstantParameterList->pTextureParameters = ALCHEMY_NULL;

													pCurrentConstantParameterList->uParameterFlag = 0;
													pCurrentConstantParameterList->uArrayParameterFlag = 0;
													//pCurrentConstantParameterList->uMatrixParameterFlag = 0;
													//pCurrentConstantParameterList->uCameraParameterFlag = 0;
													pCurrentConstantParameterList->uMaterialParameterFlag = 0;
													pCurrentConstantParameterList->uConstantParameterFlag = 0;

													pCurrentConstantParameterList->puCameraParameterFlag = ALCHEMY_NULL;
													pCurrentConstantParameterList->puLightParameterFlag = ALCHEMY_NULL;
													pCurrentConstantParameterList->puTextureParameterFlag = ALCHEMY_NULL;

													pCurrentConstantParameterList->uCameraCount = 0;
													pCurrentConstantParameterList->uLightCount = 0;
													pCurrentConstantParameterList->uTextureCount = 0;

													pGlobalInputHandler->m_ppParameterList[uRenderTargetIndexCount] = pCurrentConstantParameterList;
												}

												switch(pGlobalVariable->GlobalType)
												{
												case NORMAL_VARIABLE:
													if(!pGlobalVariable->uIndex && pGlobalVariable->uDetail < IRenderMethod::TOTAL_PARAMETERS)
													{
														pParameterInfo = &pCurrentConstantParameterList->Parameters[pGlobalVariable->uDetail];

														if(ALCHEMY_TEST_BIT(pCurrentConstantParameterList->uParameterFlag, pGlobalVariable->uDetail) && pGlobalVariable->ShaderType != pParameterInfo->Type)
														{
															if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
																pParameterInfo->uBetaIndex = uOffset;
															else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
																pParameterInfo->uAlphaIndex = uOffset;

															pParameterInfo->Type = IEffect::PROGRAM;
														}
														else
														{
															pParameterInfo->Type        = pGlobalVariable->ShaderType;
															pParameterInfo->uAlphaIndex = uOffset;
														}

														ALCHEMY_SET_BIT(pCurrentConstantParameterList->uParameterFlag, pGlobalVariable->uDetail);
													}
													break;

												case ARRAY_VARIABLE:
													if(!pGlobalVariable->uIndex && pGlobalVariable->uDetail < IRenderMethod::TOTAL_ARRAY_PARAMETERS)
													{
														pParameterInfo = &pCurrentConstantParameterList->ArrayParameters[pGlobalVariable->uDetail];

														if(ALCHEMY_TEST_BIT(pCurrentConstantParameterList->uArrayParameterFlag, pGlobalVariable->uDetail) && pGlobalVariable->ShaderType != pParameterInfo->Type)
														{
															if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
																pParameterInfo->uBetaIndex = uOffset;
															else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
																pParameterInfo->uAlphaIndex = uOffset;

															pParameterInfo->Type = IEffect::PROGRAM;
														}
														else
														{
															pParameterInfo->Type        = pGlobalVariable->ShaderType;
															pParameterInfo->uAlphaIndex = uOffset;
														}

														ALCHEMY_SET_BIT(pCurrentConstantParameterList->uArrayParameterFlag, pGlobalVariable->uDetail);
													}
													break;

												/*case MATRIX_VARIABLE:
													if(!pGlobalVariable->uIndex && pGlobalVariable->uDetail < CRenderMethod::TOTAL_MATRIX_PARAMETERS)
													{
														pParameterInfo = &pCurrentConstantParameterList->MatrixParameters[pGlobalVariable->uDetail];

														if(ALCHEMY_TEST_BIT(pCurrentConstantParameterList->uMatrixParameterFlag, pGlobalVariable->uDetail) && pGlobalVariable->ShaderType != pParameterInfo->Type)
														{
															if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
																pParameterInfo->uBetaIndex = uOffset;
															else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
																pParameterInfo->uAlphaIndex = uOffset;

															pParameterInfo->Type = IEffect::PROGRAM;
														}
														else
														{
															pParameterInfo->Type        = pGlobalVariable->ShaderType;
															pParameterInfo->uAlphaIndex = uOffset;
														}

														ALCHEMY_SET_BIT(pCurrentConstantParameterList->uMatrixParameterFlag, pGlobalVariable->uDetail);
													}
													break;
												case CAMERA_VARIABLE:
													if(!pGlobalVariable->uIndex && pGlobalVariable->uDetail < CRenderMethod::TOTAL_CAMERA_PARAMETERS)
													{
														pParameterInfo = &pCurrentConstantParameterList->CameraParameters[pGlobalVariable->uDetail];

														if(ALCHEMY_TEST_BIT(pCurrentConstantParameterList->uCameraParameterFlag, pGlobalVariable->uDetail) && pGlobalVariable->ShaderType != pParameterInfo->Type)
														{
															if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
																pParameterInfo->uBetaIndex = uOffset;
															else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
																pParameterInfo->uAlphaIndex = uOffset;

															pParameterInfo->Type = IEffect::PROGRAM;
														}
														else
														{
															pParameterInfo->Type        = pGlobalVariable->ShaderType;
															pParameterInfo->uAlphaIndex = uOffset;
														}

														ALCHEMY_SET_BIT(pCurrentConstantParameterList->uCameraParameterFlag, pGlobalVariable->uDetail);
													}
													break;*/

												case MATERIAL_VARIABLE:
													if(!pGlobalVariable->uIndex && pGlobalVariable->uDetail < CRenderMethod::TOTAL_MATERIAL_PARAMETERS)
													{
														pParameterInfo = &pCurrentConstantParameterList->MaterialParameters[pGlobalVariable->uDetail];

														if(ALCHEMY_TEST_BIT(pCurrentConstantParameterList->uMaterialParameterFlag, pGlobalVariable->uDetail) && pGlobalVariable->ShaderType != pParameterInfo->Type)
														{
															if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
																pParameterInfo->uBetaIndex = uOffset;
															else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
																pParameterInfo->uAlphaIndex = uOffset;

															pParameterInfo->Type = IEffect::PROGRAM;
														}
														else
														{
															pParameterInfo->Type        = pGlobalVariable->ShaderType;
															pParameterInfo->uAlphaIndex = uOffset;
														}

														ALCHEMY_SET_BIT(pCurrentConstantParameterList->uMaterialParameterFlag, pGlobalVariable->uDetail);
													}
													break;

												case CAMERA_VARIABLE:
													if(pGlobalVariable->uDetail < CRenderMethod::TOTAL_CAMERA_PARAMETERS)
													{
														uCurrentCount = pGlobalVariable->uIndex + 1;

														pCurrentConstantParameterList->uCameraCount = ALCHEMY_MAXINUM(pCurrentConstantParameterList->uCameraCount, uCurrentCount);
													}
													break;

												case LIGHT_VARIABLE:
													if(pGlobalVariable->uDetail < CRenderMethod::TOTAL_LIGHT_PARAMETERS)
													{
														uCurrentCount = pGlobalVariable->uIndex + 1;

														pCurrentConstantParameterList->uLightCount = ALCHEMY_MAXINUM(pCurrentConstantParameterList->uLightCount, uCurrentCount);
													}
													break;

												case CONSTANT_VARIABLE:
													if(!pGlobalVariable->uIndex && pGlobalVariable->uDetail < CRenderMethod::TOTAL_CONSTANT_PARAMETERS)
													{
														pParameterInfo = &pCurrentConstantParameterList->ConstantParameters[pGlobalVariable->uDetail];

														if(ALCHEMY_TEST_BIT(pCurrentConstantParameterList->uConstantParameterFlag, pGlobalVariable->uDetail) && pGlobalVariable->ShaderType != pParameterInfo->Type)
														{
															if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
																pParameterInfo->uBetaIndex = uOffset;
															else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
																pParameterInfo->uAlphaIndex = uOffset;

															pParameterInfo->Type = IEffect::PROGRAM;
														}
														else
														{
															pParameterInfo->Type        = pGlobalVariable->ShaderType;
															pParameterInfo->uAlphaIndex = uOffset;
														}

														ALCHEMY_SET_BIT(pCurrentConstantParameterList->uConstantParameterFlag, pGlobalVariable->uDetail);
													}
													break;
                                                        
                                                default:
                                                    break;
												}
											}

											uCurrentCount = pGlobalVariable->uCount;
										}
										else
											uCurrentCount = 1;
									}
									else
										uCurrentCount = 1;

									VariableOffsetMap.Insert(pConstantInstruction->uSourceIndex2, uOffset);

									GlobalInstruction.uSourceIndex2 = uOffset;

									uOffset += GlobalInstruction.uSourceIndexSize2 = GetRegisterCount(pVariable->Type) * uCurrentCount;

									GlobalInstruction.uSourceIndexAddress2 = pConstantInstruction->uSourceIndexAddress2;
								}
								else
								{
									GlobalInstruction.uSourceIndex2        = 0;
									GlobalInstruction.uSourceIndexSize2    = 0;
									GlobalInstruction.uSourceIndexAddress2 = 0;
								}
							}

							if(!pGlobalInputHandler->m_pInstructionSet)
								ALCHEMY_DEBUG_NEW(pGlobalInputHandler->m_pInstructionSet, CList<CGlobalInputHandler::GLOBALINSTRUCTION>[uParameterListCount]);

							pGlobalInputHandler->m_pInstructionSet[uRenderTargetIndexCount].Unshift(GlobalInstruction);

							pConstantInstruction = pConstantInstruction->uPrevious ? m_InstructionPool.Get(pConstantInstruction->uPrevious - 1) : ALCHEMY_NULL;

							if(pConstantInstruction == ALCHEMY_NULL && Type == IEffect::VERTEX_SHADER)
							{
								puConstantInstruction = m_InstructionSetPool.Get(pRenderTargetCode->uFragmentConstantInstructionSet - 1);

								pConstantInstruction  = (puConstantInstruction && *puConstantInstruction) ? m_InstructionPool.Get(*puConstantInstruction - 1) : ALCHEMY_NULL;

								Type = IEffect::FRAGMENT_SHADER;
							}
						}

						if(pCurrentConstantParameterList)
						{
							uCurrentMaxOffset = uOffset;
							pGlobalInputHandler->m_uParameterBufferSize = ALCHEMY_MAXINUM(uOffset, pGlobalInputHandler->m_uParameterBufferSize);

							ALCHEMY_DEBUG_NEW(pCurrentConstantParameterList->pCameraParameters     , CRenderMethod::PARAMETERINFO       [pCurrentConstantParameterList->uCameraCount ][CRenderMethod::TOTAL_CAMERA_PARAMETERS ]);
							ALCHEMY_DEBUG_NEW(pCurrentConstantParameterList->pLightParameters      , CRenderMethod::PARAMETERINFO       [pCurrentConstantParameterList->uLightCount  ][CRenderMethod::TOTAL_LIGHT_PARAMETERS  ]);
							ALCHEMY_DEBUG_NEW(pCurrentConstantParameterList->pTextureParameters    , CRenderMethod::PARAMETERINFO       [pCurrentConstantParameterList->uTextureCount][CRenderMethod::TOTAL_TEXTURE_PARAMETERS]);

							//ALCHEMY_DEBUG_NEW(pCurrentConstantParameterList->pTextures             , CRenderMethod::PARAMETERINFO       [pCurrentConstantParameterList->uTextureCount                                         ]);

							ALCHEMY_DEBUG_NEW(pCurrentConstantParameterList->puCameraParameterFlag , UINT                               [pCurrentConstantParameterList->uCameraCount                                          ]);
							ALCHEMY_DEBUG_NEW(pCurrentConstantParameterList->puLightParameterFlag  , UINT                               [pCurrentConstantParameterList->uLightCount                                           ]);
							ALCHEMY_DEBUG_NEW(pCurrentConstantParameterList->puTextureParameterFlag, UINT                               [pCurrentConstantParameterList->uTextureCount                                         ]);

							for(j = 0; j < pCurrentConstantParameterList->uCameraCount; ++ j)
								pCurrentConstantParameterList->puCameraParameterFlag[j] = 0;

							for(j = 0; j < pCurrentConstantParameterList->uLightCount; ++ j)
								pCurrentConstantParameterList->puLightParameterFlag[j] = 0;

							for(j = 0; j < pCurrentConstantParameterList->uTextureCount; ++ j)
								pCurrentConstantParameterList->puTextureParameterFlag[j] = 0;

							puConstantInstruction = m_InstructionSetPool.Get(pRenderTargetCode->uVertexConstantInstructionSet - 1);
							if(puConstantInstruction)
								Type = IEffect::VERTEX_SHADER;
							else
							{
								puConstantInstruction = m_InstructionSetPool.Get(pRenderTargetCode->uFragmentConstantInstructionSet - 1);

								Type = IEffect::FRAGMENT_SHADER;
							}

							pConstantInstruction = (puConstantInstruction && *puConstantInstruction) ? m_InstructionPool.Get(*puConstantInstruction - 1) : ALCHEMY_NULL;
							while(pConstantInstruction)
							{
								pVariable = pConstantInstruction->uDestination ? m_VariablePool.Get(pConstantInstruction->uDestination - 1) : ALCHEMY_NULL;
								if(pVariable)
								{
									if(pVariable->uGlobal)
									{
										pGlobalVariable = m_GlobalVariablePool.Get(pVariable->uGlobal - 1);
										if(pGlobalVariable)
										{
											switch(pGlobalVariable->GlobalType)
											{
											case INPUT_VARIABLE:
												if(pGlobalVariable->pValue)
												{
													puOffset = VariableOffsetMap.Get(pConstantInstruction->uDestination);
													if(puOffset)
													{
														uOffset = *puOffset;

														uRegister = 0;

														while(pVariable)
														{
															pCurrentVariable = pVariable;

															uRegister += pVariable->uRegister;
															pVariable = pVariable->uParent ? m_VariablePool.Get(pVariable->uParent - 1) : ALCHEMY_NULL;
														}

														pVariable = pCurrentVariable;

														puOffset = ConstantOffsetMap.Get(uRegister);
														if(puOffset)
														{
															if(!pGlobalInputHandler->m_ppInputValue)
															{
																ALCHEMY_DEBUG_NEW(pGlobalInputHandler->m_ppInputValue, CGlobalInputHandler::LPINPUTVALUE[uParameterListCount]);

																for(j = 0; j < uParameterListCount; ++ j)
																	pGlobalInputHandler->m_ppInputValue[j] = ALCHEMY_NULL;
															}

															pInputValue = pGlobalInputHandler->m_ppInputValue[uRenderTargetIndexCount];
															if(!pInputValue)
															{
																uCurrentCount = pGlobalInputHandler->m_puInputValueCount[uRenderTargetIndexCount];
																ALCHEMY_DEBUG_NEW(pInputValue, CGlobalInputHandler::INPUTVALUE[uCurrentCount]);

																for(j = 0; j < uCurrentCount; ++ j)
																{
																	pInputValue->pValue = ALCHEMY_NULL;
																	pInputValue->uIndex = 0;
																	pInputValue->uSize  = 0;
																}

																pGlobalInputHandler->m_ppInputValue[uRenderTargetIndexCount] = pInputValue;
															}

															pInputValue += *puOffset;
															pInputValue->pValue = pGlobalVariable->pValue;
															pInputValue->uIndex = uOffset;
															pInputValue->uSize  = GetRegisterCount(pVariable->Type) * pGlobalVariable->uCount;
														}
													}
												}
												break;
											case CAMERA_VARIABLE:
												if(pGlobalVariable->uDetail < CRenderMethod::TOTAL_CAMERA_PARAMETERS && pGlobalVariable->uIndex < pCurrentConstantParameterList->uCameraCount)
												{
													puOffset = VariableOffsetMap.Get(pConstantInstruction->uDestination);
													if(puOffset)
													{
														uOffset = *puOffset;

														pParameterInfo = &pCurrentConstantParameterList->pCameraParameters[pGlobalVariable->uIndex][pGlobalVariable->uDetail];

														if(ALCHEMY_TEST_BIT(pCurrentConstantParameterList->puCameraParameterFlag[pGlobalVariable->uIndex], pGlobalVariable->uDetail) && pGlobalVariable->ShaderType != pParameterInfo->Type)
														{
															if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
																pParameterInfo->uBetaIndex = uOffset;
															else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
																pParameterInfo->uAlphaIndex = uOffset;

															pParameterInfo->Type = IEffect::PROGRAM;
														}
														else
														{
															pParameterInfo->Type   = pGlobalVariable->ShaderType;
															pParameterInfo->uAlphaIndex = uOffset;
														}

														ALCHEMY_SET_BIT(pCurrentConstantParameterList->puCameraParameterFlag[pGlobalVariable->uIndex], pGlobalVariable->uDetail);
													}
												}
												break;
											case LIGHT_VARIABLE:
												if(pGlobalVariable->uDetail < CRenderMethod::TOTAL_LIGHT_PARAMETERS && pGlobalVariable->uIndex < pCurrentConstantParameterList->uLightCount)
												{
													puOffset = VariableOffsetMap.Get(pConstantInstruction->uDestination);
													if(puOffset)
													{
														uOffset = *puOffset;

														pParameterInfo = &pCurrentConstantParameterList->pLightParameters[pGlobalVariable->uIndex][pGlobalVariable->uDetail];

														if(ALCHEMY_TEST_BIT(pCurrentConstantParameterList->puLightParameterFlag[pGlobalVariable->uIndex], pGlobalVariable->uDetail) && pGlobalVariable->ShaderType != pParameterInfo->Type)
														{
															if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
																pParameterInfo->uBetaIndex = uOffset;
															else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
																pParameterInfo->uAlphaIndex = uOffset;

															pParameterInfo->Type = IEffect::PROGRAM;
														}
														else
														{
															pParameterInfo->Type   = pGlobalVariable->ShaderType;
															pParameterInfo->uAlphaIndex = uOffset;
														}

														ALCHEMY_SET_BIT(pCurrentConstantParameterList->puLightParameterFlag[pGlobalVariable->uIndex], pGlobalVariable->uDetail);
													}
												}
												break;
											case TEXTURE_VARIABLE:
												if(pGlobalVariable->uDetail < CRenderMethod::TOTAL_TEXTURE_PARAMETERS && pGlobalVariable->uIndex < pCurrentConstantParameterList->uTextureCount)
												{
													puOffset = VariableOffsetMap.Get(pConstantInstruction->uDestination);
													if(puOffset)
													{
														uOffset = *puOffset;

														pParameterInfo = &pCurrentConstantParameterList->pTextureParameters[pGlobalVariable->uIndex][pGlobalVariable->uDetail];

														if(ALCHEMY_TEST_BIT(pCurrentConstantParameterList->puTextureParameterFlag[pGlobalVariable->uIndex], pGlobalVariable->uDetail) && pGlobalVariable->ShaderType != pParameterInfo->Type)
														{
															if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
																pParameterInfo->uBetaIndex = uOffset;
															else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
																pParameterInfo->uAlphaIndex = uOffset;

															pParameterInfo->Type = IEffect::PROGRAM;
														}
														else
														{
															pParameterInfo->Type = pGlobalVariable->ShaderType;
															pParameterInfo->uAlphaIndex = uOffset;
														}

														ALCHEMY_SET_BIT(pCurrentConstantParameterList->puTextureParameterFlag[pGlobalVariable->uIndex], pGlobalVariable->uDetail);
													}
												}
												break;
                                            default:
                                                break;
											}
										}
									}
								}

								pVariable = pConstantInstruction->uDestinationIndex ? m_VariablePool.Get(pConstantInstruction->uDestinationIndex - 1) : ALCHEMY_NULL;
								if(pVariable)
								{
									if(pVariable->uGlobal)
									{
										pGlobalVariable = m_GlobalVariablePool.Get(pVariable->uGlobal - 1);
										if(pGlobalVariable)
										{
											switch(pGlobalVariable->GlobalType)
											{
											case INPUT_VARIABLE:
												if(pGlobalVariable->pValue)
												{
													puOffset = VariableOffsetMap.Get(pConstantInstruction->uDestinationIndex);
													if(puOffset)
													{
														uOffset = *puOffset;

														uRegister = 0;

														while(pVariable)
														{
															pCurrentVariable = pVariable;

															uRegister += pVariable->uRegister;
															pVariable = pVariable->uParent ? m_VariablePool.Get(pVariable->uParent - 1) : ALCHEMY_NULL;
														}

														pVariable = pCurrentVariable;

														puOffset = ConstantOffsetMap.Get(uRegister);
														if(puOffset)
														{
															if(!pGlobalInputHandler->m_ppInputValue)
															{
																ALCHEMY_DEBUG_NEW(pGlobalInputHandler->m_ppInputValue, CGlobalInputHandler::LPINPUTVALUE[uParameterListCount]);

																for(j = 0; j < uParameterListCount; ++ j)
																	pGlobalInputHandler->m_ppInputValue[j] = ALCHEMY_NULL;
															}

															pInputValue = pGlobalInputHandler->m_ppInputValue[uRenderTargetIndexCount];
															if(!pInputValue)
															{
																uCurrentCount = pGlobalInputHandler->m_puInputValueCount[uRenderTargetIndexCount];
																ALCHEMY_DEBUG_NEW(pInputValue, CGlobalInputHandler::INPUTVALUE[uCurrentCount]);

																for(j = 0; j < uCurrentCount; ++ j)
																{
																	pInputValue->pValue = ALCHEMY_NULL;
																	pInputValue->uIndex = 0;
																	pInputValue->uSize  = 0;
																}

																pGlobalInputHandler->m_ppInputValue[uRenderTargetIndexCount] = pInputValue;
															}

															pInputValue += *puOffset;
															pInputValue->pValue = pGlobalVariable->pValue;
															pInputValue->uIndex = uOffset;
															pInputValue->uSize  = GetRegisterCount(pVariable->Type) * pGlobalVariable->uCount;
														}
													}
												}
												break;
											case CAMERA_VARIABLE:
												if(pGlobalVariable->uDetail < CRenderMethod::TOTAL_CAMERA_PARAMETERS && pGlobalVariable->uIndex < pCurrentConstantParameterList->uCameraCount)
												{
													puOffset = VariableOffsetMap.Get(pConstantInstruction->uDestinationIndex);
													if(puOffset)
													{
														uOffset = *puOffset;

														pParameterInfo = &pCurrentConstantParameterList->pCameraParameters[pGlobalVariable->uIndex][pGlobalVariable->uDetail];

														if(ALCHEMY_TEST_BIT(pCurrentConstantParameterList->puCameraParameterFlag[pGlobalVariable->uIndex], pGlobalVariable->uDetail) && pGlobalVariable->ShaderType != pParameterInfo->Type)
														{
															if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
																pParameterInfo->uBetaIndex = uOffset;
															else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
																pParameterInfo->uAlphaIndex = uOffset;

															pParameterInfo->Type = IEffect::PROGRAM;
														}
														else
														{
															pParameterInfo->Type   = pGlobalVariable->ShaderType;
															pParameterInfo->uAlphaIndex = uOffset;
														}

														ALCHEMY_SET_BIT(pCurrentConstantParameterList->puCameraParameterFlag[pGlobalVariable->uIndex], pGlobalVariable->uDetail);
													}
												}
												break;
											case LIGHT_VARIABLE:
												if(pGlobalVariable->uDetail < CRenderMethod::TOTAL_LIGHT_PARAMETERS && pGlobalVariable->uIndex < pCurrentConstantParameterList->uLightCount)
												{
													puOffset = VariableOffsetMap.Get(pConstantInstruction->uDestinationIndex);
													if(puOffset)
													{
														uOffset = *puOffset;

														pParameterInfo = &pCurrentConstantParameterList->pLightParameters[pGlobalVariable->uIndex][pGlobalVariable->uDetail];

														if(ALCHEMY_TEST_BIT(pCurrentConstantParameterList->puLightParameterFlag[pGlobalVariable->uIndex], pGlobalVariable->uDetail) && pGlobalVariable->ShaderType != pParameterInfo->Type)
														{
															if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
																pParameterInfo->uBetaIndex = uOffset;
															else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
																pParameterInfo->uAlphaIndex = uOffset;

															pParameterInfo->Type = IEffect::PROGRAM;
														}
														else
														{
															pParameterInfo->Type   = pGlobalVariable->ShaderType;
															pParameterInfo->uAlphaIndex = uOffset;
														}

														ALCHEMY_SET_BIT(pCurrentConstantParameterList->puLightParameterFlag[pGlobalVariable->uIndex], pGlobalVariable->uDetail);
													}
												}
												break;
											case TEXTURE_VARIABLE:
												if(pGlobalVariable->uDetail < CRenderMethod::TOTAL_TEXTURE_PARAMETERS && pGlobalVariable->uIndex < pCurrentConstantParameterList->uTextureCount)
												{
													puOffset = VariableOffsetMap.Get(pConstantInstruction->uDestinationIndex);
													if(puOffset)
													{
														uOffset = *puOffset;

														pParameterInfo = &pCurrentConstantParameterList->pTextureParameters[pGlobalVariable->uIndex][pGlobalVariable->uDetail];

														if(ALCHEMY_TEST_BIT(pCurrentConstantParameterList->puTextureParameterFlag[pGlobalVariable->uIndex], pGlobalVariable->uDetail) && pGlobalVariable->ShaderType != pParameterInfo->Type)
														{
															if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
																pParameterInfo->uBetaIndex = uOffset;
															else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
																pParameterInfo->uAlphaIndex = uOffset;

															pParameterInfo->Type = IEffect::PROGRAM;
														}
														else
														{
															pParameterInfo->Type = pGlobalVariable->ShaderType;
															pParameterInfo->uAlphaIndex = uOffset;
														}

														ALCHEMY_SET_BIT(pCurrentConstantParameterList->puTextureParameterFlag[pGlobalVariable->uIndex], pGlobalVariable->uDetail);
													}
												}
												break;
                                            default:
                                                break;
											}
										}
									}
								}

								pVariable = pConstantInstruction->uSource1 ? m_VariablePool.Get(pConstantInstruction->uSource1 - 1) : ALCHEMY_NULL;
								if(pVariable)
								{
									if(pVariable->uGlobal)
									{
										pGlobalVariable = m_GlobalVariablePool.Get(pVariable->uGlobal - 1);
										if(pGlobalVariable)
										{
											switch(pGlobalVariable->GlobalType)
											{
											case INPUT_VARIABLE:
												if(pGlobalVariable->pValue)
												{
													puOffset = VariableOffsetMap.Get(pConstantInstruction->uSource1);
													if(puOffset)
													{
														uOffset = *puOffset;

														uRegister = 0;

														while(pVariable)
														{
															pCurrentVariable = pVariable;

															uRegister += pVariable->uRegister;
															pVariable = pVariable->uParent ? m_VariablePool.Get(pVariable->uParent - 1) : ALCHEMY_NULL;
														}

														pVariable = pCurrentVariable;

														puOffset = ConstantOffsetMap.Get(uRegister);
														if(puOffset)
														{
															if(!pGlobalInputHandler->m_ppInputValue)
															{
																ALCHEMY_DEBUG_NEW(pGlobalInputHandler->m_ppInputValue, CGlobalInputHandler::LPINPUTVALUE[uParameterListCount]);

																for(j = 0; j < uParameterListCount; ++ j)
																	pGlobalInputHandler->m_ppInputValue[j] = ALCHEMY_NULL;
															}

															pInputValue = pGlobalInputHandler->m_ppInputValue[uRenderTargetIndexCount];
															if(!pInputValue)
															{
																uCurrentCount = pGlobalInputHandler->m_puInputValueCount[uRenderTargetIndexCount];
																ALCHEMY_DEBUG_NEW(pInputValue, CGlobalInputHandler::INPUTVALUE[uParameterListCount]);

																for(j = 0; j < uCurrentCount; ++ j)
																	pInputValue = ALCHEMY_NULL;

																pGlobalInputHandler->m_ppInputValue[uRenderTargetIndexCount] = pInputValue;
															}

															pInputValue += *puOffset;
															pInputValue->pValue = pGlobalVariable->pValue;
															pInputValue->uIndex = uOffset;
															pInputValue->uSize  = GetRegisterCount(pVariable->Type) * pGlobalVariable->uCount;
														}
													}
												}
												break;
											case CAMERA_VARIABLE:
												if(pGlobalVariable->uDetail < CRenderMethod::TOTAL_CAMERA_PARAMETERS && pGlobalVariable->uIndex < pCurrentConstantParameterList->uCameraCount)
												{
													puOffset = VariableOffsetMap.Get(pConstantInstruction->uSource1);
													if(puOffset)
													{
														uOffset = *puOffset;

														pParameterInfo = &pCurrentConstantParameterList->pCameraParameters[pGlobalVariable->uIndex][pGlobalVariable->uDetail];

														if(ALCHEMY_TEST_BIT(pCurrentConstantParameterList->puCameraParameterFlag[pGlobalVariable->uIndex], pGlobalVariable->uDetail) && pGlobalVariable->ShaderType != pParameterInfo->Type)
														{
															if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
																pParameterInfo->uBetaIndex = uOffset;
															else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
																pParameterInfo->uAlphaIndex = uOffset;

															pParameterInfo->Type = IEffect::PROGRAM;
														}
														else
														{
															pParameterInfo->Type   = pGlobalVariable->ShaderType;
															pParameterInfo->uAlphaIndex = uOffset;
														}

														ALCHEMY_SET_BIT(pCurrentConstantParameterList->puCameraParameterFlag[pGlobalVariable->uIndex], pGlobalVariable->uDetail);
													}
												}
												break;
											case LIGHT_VARIABLE:
												if(pGlobalVariable->uDetail < CRenderMethod::TOTAL_LIGHT_PARAMETERS && pGlobalVariable->uIndex < pCurrentConstantParameterList->uLightCount)
												{
													puOffset = VariableOffsetMap.Get(pConstantInstruction->uSource1);
													if(puOffset)
													{
														uOffset = *puOffset;

														pParameterInfo = &pCurrentConstantParameterList->pLightParameters[pGlobalVariable->uIndex][pGlobalVariable->uDetail];

														if(ALCHEMY_TEST_BIT(pCurrentConstantParameterList->puLightParameterFlag[pGlobalVariable->uIndex], pGlobalVariable->uDetail) && pGlobalVariable->ShaderType != pParameterInfo->Type)
														{
															if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
																pParameterInfo->uBetaIndex = uOffset;
															else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
																pParameterInfo->uAlphaIndex = uOffset;

															pParameterInfo->Type = IEffect::PROGRAM;
														}
														else
														{
															pParameterInfo->Type   = pGlobalVariable->ShaderType;
															pParameterInfo->uAlphaIndex = uOffset;
														}

														ALCHEMY_SET_BIT(pCurrentConstantParameterList->puLightParameterFlag[pGlobalVariable->uIndex], pGlobalVariable->uDetail);
													}
												}
												break;
											case TEXTURE_VARIABLE:
												if(pGlobalVariable->uDetail < CRenderMethod::TOTAL_TEXTURE_PARAMETERS && pGlobalVariable->uIndex < pCurrentConstantParameterList->uTextureCount)
												{
													puOffset = VariableOffsetMap.Get(pConstantInstruction->uSource1);
													if(puOffset)
													{
														uOffset = *puOffset;

														pParameterInfo = &pCurrentConstantParameterList->pTextureParameters[pGlobalVariable->uIndex][pGlobalVariable->uDetail];

														if(ALCHEMY_TEST_BIT(pCurrentConstantParameterList->puTextureParameterFlag[pGlobalVariable->uIndex], pGlobalVariable->uDetail) && pGlobalVariable->ShaderType != pParameterInfo->Type)
														{
															if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
																pParameterInfo->uBetaIndex = uOffset;
															else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
																pParameterInfo->uAlphaIndex = uOffset;

															pParameterInfo->Type = IEffect::PROGRAM;
														}
														else
														{
															pParameterInfo->Type = pGlobalVariable->ShaderType;
															pParameterInfo->uAlphaIndex = uOffset;
														}

														ALCHEMY_SET_BIT(pCurrentConstantParameterList->puTextureParameterFlag[pGlobalVariable->uIndex], pGlobalVariable->uDetail);
													}
												}
												break;
                                            default:
                                                break;
											}
										}
									}
								}


								pVariable = pConstantInstruction->uSourceIndex1 ? m_VariablePool.Get(pConstantInstruction->uSourceIndex1 - 1) : ALCHEMY_NULL;
								if(pVariable)
								{
									if(pVariable->uGlobal)
									{
										pGlobalVariable = m_GlobalVariablePool.Get(pVariable->uGlobal - 1);
										if(pGlobalVariable)
										{
											switch(pGlobalVariable->GlobalType)
											{
											case INPUT_VARIABLE:
												if(pGlobalVariable->pValue)
												{
													puOffset = VariableOffsetMap.Get(pConstantInstruction->uSourceIndex1);
													if(puOffset)
													{
														uOffset = *puOffset;

														uRegister = 0;

														while(pVariable)
														{
															pCurrentVariable = pVariable;

															uRegister += pVariable->uRegister;
															pVariable = pVariable->uParent ? m_VariablePool.Get(pVariable->uParent - 1) : ALCHEMY_NULL;
														}

														pVariable = pCurrentVariable;

														puOffset = ConstantOffsetMap.Get(uRegister);
														if(puOffset)
														{
															if(!pGlobalInputHandler->m_ppInputValue)
															{
																ALCHEMY_DEBUG_NEW(pGlobalInputHandler->m_ppInputValue, CGlobalInputHandler::LPINPUTVALUE[uParameterListCount]);

																for(j = 0; j < uParameterListCount; ++ j)
																	pGlobalInputHandler->m_ppInputValue[j] = ALCHEMY_NULL;
															}

															pInputValue = pGlobalInputHandler->m_ppInputValue[uRenderTargetIndexCount];
															if(!pInputValue)
															{
																uCurrentCount = pGlobalInputHandler->m_puInputValueCount[uRenderTargetIndexCount];
																ALCHEMY_DEBUG_NEW(pInputValue, CGlobalInputHandler::INPUTVALUE[uCurrentCount]);

																for(j = 0; j < uCurrentCount; ++ j)
																{
																	pInputValue->pValue = ALCHEMY_NULL;
																	pInputValue->uIndex = 0;
																	pInputValue->uSize  = 0;
																}

																pGlobalInputHandler->m_ppInputValue[uRenderTargetIndexCount] = pInputValue;
															}

															pInputValue += *puOffset;
															pInputValue->pValue = pGlobalVariable->pValue;
															pInputValue->uIndex = uOffset;
															pInputValue->uSize  = GetRegisterCount(pVariable->Type) * pGlobalVariable->uCount;
														}
													}
												}
												break;
											case CAMERA_VARIABLE:
												if(pGlobalVariable->uDetail < CRenderMethod::TOTAL_CAMERA_PARAMETERS && pGlobalVariable->uIndex < pCurrentConstantParameterList->uCameraCount)
												{
													puOffset = VariableOffsetMap.Get(pConstantInstruction->uSourceIndex1);
													if(puOffset)
													{
														uOffset = *puOffset;

														pParameterInfo = &pCurrentConstantParameterList->pCameraParameters[pGlobalVariable->uIndex][pGlobalVariable->uDetail];

														if(ALCHEMY_TEST_BIT(pCurrentConstantParameterList->puCameraParameterFlag[pGlobalVariable->uIndex], pGlobalVariable->uDetail) && pGlobalVariable->ShaderType != pParameterInfo->Type)
														{
															if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
																pParameterInfo->uBetaIndex = uOffset;
															else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
																pParameterInfo->uAlphaIndex = uOffset;

															pParameterInfo->Type = IEffect::PROGRAM;
														}
														else
														{
															pParameterInfo->Type   = pGlobalVariable->ShaderType;
															pParameterInfo->uAlphaIndex = uOffset;
														}

														ALCHEMY_SET_BIT(pCurrentConstantParameterList->puCameraParameterFlag[pGlobalVariable->uIndex], pGlobalVariable->uDetail);
													}
												}
												break;
											case LIGHT_VARIABLE:
												if(pGlobalVariable->uDetail < CRenderMethod::TOTAL_LIGHT_PARAMETERS && pGlobalVariable->uIndex < pCurrentConstantParameterList->uLightCount)
												{
													puOffset = VariableOffsetMap.Get(pConstantInstruction->uSourceIndex1);
													if(puOffset)
													{
														uOffset = *puOffset;

														pParameterInfo = &pCurrentConstantParameterList->pLightParameters[pGlobalVariable->uIndex][pGlobalVariable->uDetail];

														if(ALCHEMY_TEST_BIT(pCurrentConstantParameterList->puLightParameterFlag[pGlobalVariable->uIndex], pGlobalVariable->uDetail) && pGlobalVariable->ShaderType != pParameterInfo->Type)
														{
															if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
																pParameterInfo->uBetaIndex = uOffset;
															else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
																pParameterInfo->uAlphaIndex = uOffset;

															pParameterInfo->Type = IEffect::PROGRAM;
														}
														else
														{
															pParameterInfo->Type   = pGlobalVariable->ShaderType;
															pParameterInfo->uAlphaIndex = uOffset;
														}

														ALCHEMY_SET_BIT(pCurrentConstantParameterList->puLightParameterFlag[pGlobalVariable->uIndex], pGlobalVariable->uDetail);
													}
												}
												break;
											case TEXTURE_VARIABLE:
												if(pGlobalVariable->uDetail < CRenderMethod::TOTAL_TEXTURE_PARAMETERS && pGlobalVariable->uIndex < pCurrentConstantParameterList->uTextureCount)
												{
													puOffset = VariableOffsetMap.Get(pConstantInstruction->uSourceIndex1);
													if(puOffset)
													{
														uOffset = *puOffset;

														pParameterInfo = &pCurrentConstantParameterList->pTextureParameters[pGlobalVariable->uIndex][pGlobalVariable->uDetail];

														if(ALCHEMY_TEST_BIT(pCurrentConstantParameterList->puTextureParameterFlag[pGlobalVariable->uIndex], pGlobalVariable->uDetail) && pGlobalVariable->ShaderType != pParameterInfo->Type)
														{
															if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
																pParameterInfo->uBetaIndex = uOffset;
															else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
																pParameterInfo->uAlphaIndex = uOffset;

															pParameterInfo->Type = IEffect::PROGRAM;
														}
														else
														{
															pParameterInfo->Type = pGlobalVariable->ShaderType;
															pParameterInfo->uAlphaIndex = uOffset;
														}

														ALCHEMY_SET_BIT(pCurrentConstantParameterList->puTextureParameterFlag[pGlobalVariable->uIndex], pGlobalVariable->uDetail);
													}
												}
												break;
                                            default:
                                                break;
											}
										}
									}
								}


								pVariable = pConstantInstruction->uSource2 ? m_VariablePool.Get(pConstantInstruction->uSource2 - 1) : ALCHEMY_NULL;
								if(pVariable)
								{
									if(pVariable->uGlobal)
									{
										pGlobalVariable = m_GlobalVariablePool.Get(pVariable->uGlobal - 1);
										if(pGlobalVariable)
										{
											switch(pGlobalVariable->GlobalType)
											{
											case INPUT_VARIABLE:
												if(pGlobalVariable->pValue)
												{
													puOffset = VariableOffsetMap.Get(pConstantInstruction->uSource2);
													if(puOffset)
													{
														uOffset = *puOffset;

														uRegister = 0;

														while(pVariable)
														{
															pCurrentVariable = pVariable;

															uRegister += pVariable->uRegister;
															pVariable = pVariable->uParent ? m_VariablePool.Get(pVariable->uParent - 1) : ALCHEMY_NULL;
														}

														pVariable = pCurrentVariable;

														puOffset = ConstantOffsetMap.Get(uRegister);
														if(puOffset)
														{
															if(!pGlobalInputHandler->m_ppInputValue)
															{
																ALCHEMY_DEBUG_NEW(pGlobalInputHandler->m_ppInputValue, CGlobalInputHandler::LPINPUTVALUE[uParameterListCount]);

																for(j = 0; j < uParameterListCount; ++ j)
																	pGlobalInputHandler->m_ppInputValue[j] = ALCHEMY_NULL;
															}

															pInputValue = pGlobalInputHandler->m_ppInputValue[uRenderTargetIndexCount];
															if(!pInputValue)
															{
																uCurrentCount = pGlobalInputHandler->m_puInputValueCount[uRenderTargetIndexCount];
																ALCHEMY_DEBUG_NEW(pInputValue, CGlobalInputHandler::INPUTVALUE[uCurrentCount]);

																for(j = 0; j < uCurrentCount; ++ j)
																{
																	pInputValue->pValue = ALCHEMY_NULL;
																	pInputValue->uIndex = 0;
																	pInputValue->uSize  = 0;
																}

																pGlobalInputHandler->m_ppInputValue[uRenderTargetIndexCount] = pInputValue;
															}

															pInputValue += *puOffset;
															pInputValue->pValue = pGlobalVariable->pValue;
															pInputValue->uIndex = uOffset;
															pInputValue->uSize  = GetRegisterCount(pVariable->Type) * pGlobalVariable->uCount;
														}
													}
												}
												break;
											case CAMERA_VARIABLE:
												if(pGlobalVariable->uDetail < CRenderMethod::TOTAL_CAMERA_PARAMETERS && pGlobalVariable->uIndex < pCurrentConstantParameterList->uCameraCount)
												{
													puOffset = VariableOffsetMap.Get(pConstantInstruction->uSource2);
													if(puOffset)
													{
														uOffset = *puOffset;

														pParameterInfo = &pCurrentConstantParameterList->pCameraParameters[pGlobalVariable->uIndex][pGlobalVariable->uDetail];

														if(ALCHEMY_TEST_BIT(pCurrentConstantParameterList->puCameraParameterFlag[pGlobalVariable->uIndex], pGlobalVariable->uDetail) && pGlobalVariable->ShaderType != pParameterInfo->Type)
														{
															if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
																pParameterInfo->uBetaIndex = uOffset;
															else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
																pParameterInfo->uAlphaIndex = uOffset;

															pParameterInfo->Type = IEffect::PROGRAM;
														}
														else
														{
															pParameterInfo->Type   = pGlobalVariable->ShaderType;
															pParameterInfo->uAlphaIndex = uOffset;
														}

														ALCHEMY_SET_BIT(pCurrentConstantParameterList->puCameraParameterFlag[pGlobalVariable->uIndex], pGlobalVariable->uDetail);
													}
												}
												break;
											case LIGHT_VARIABLE:
												if(pGlobalVariable->uDetail < CRenderMethod::TOTAL_LIGHT_PARAMETERS && pGlobalVariable->uIndex < pCurrentConstantParameterList->uLightCount)
												{
													puOffset = VariableOffsetMap.Get(pConstantInstruction->uSource2);
													if(puOffset)
													{
														uOffset = *puOffset;

														pParameterInfo = &pCurrentConstantParameterList->pLightParameters[pGlobalVariable->uIndex][pGlobalVariable->uDetail];

														if(ALCHEMY_TEST_BIT(pCurrentConstantParameterList->puLightParameterFlag[pGlobalVariable->uIndex], pGlobalVariable->uDetail) && pGlobalVariable->ShaderType != pParameterInfo->Type)
														{
															if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
																pParameterInfo->uBetaIndex = uOffset;
															else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
																pParameterInfo->uAlphaIndex = uOffset;

															pParameterInfo->Type = IEffect::PROGRAM;
														}
														else
														{
															pParameterInfo->Type   = pGlobalVariable->ShaderType;
															pParameterInfo->uAlphaIndex = uOffset;
														}

														ALCHEMY_SET_BIT(pCurrentConstantParameterList->puLightParameterFlag[pGlobalVariable->uIndex], pGlobalVariable->uDetail);
													}
												}
												break;
											case TEXTURE_VARIABLE:
												if(pGlobalVariable->uDetail < CRenderMethod::TOTAL_TEXTURE_PARAMETERS && pGlobalVariable->uIndex < pCurrentConstantParameterList->uTextureCount)
												{
													puOffset = VariableOffsetMap.Get(pConstantInstruction->uSource2);
													if(puOffset)
													{
														uOffset = *puOffset;

														pParameterInfo = &pCurrentConstantParameterList->pTextureParameters[pGlobalVariable->uIndex][pGlobalVariable->uDetail];

														if(ALCHEMY_TEST_BIT(pCurrentConstantParameterList->puTextureParameterFlag[pGlobalVariable->uIndex], pGlobalVariable->uDetail) && pGlobalVariable->ShaderType != pParameterInfo->Type)
														{
															if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
																pParameterInfo->uBetaIndex = uOffset;
															else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
																pParameterInfo->uAlphaIndex = uOffset;

															pParameterInfo->Type = IEffect::PROGRAM;
														}
														else
														{
															pParameterInfo->Type = pGlobalVariable->ShaderType;
															pParameterInfo->uAlphaIndex = uOffset;
														}

														ALCHEMY_SET_BIT(pCurrentConstantParameterList->puTextureParameterFlag[pGlobalVariable->uIndex], pGlobalVariable->uDetail);
													}
												}
												break;
                                            default:
                                                break;
											}
										}
									}
								}


								pVariable = pConstantInstruction->uSourceIndex2 ? m_VariablePool.Get(pConstantInstruction->uSourceIndex2 - 1) : ALCHEMY_NULL;
								if(pVariable)
								{
									if(pVariable->uGlobal)
									{
										pGlobalVariable = m_GlobalVariablePool.Get(pVariable->uGlobal - 1);
										if(pGlobalVariable)
										{
											switch(pGlobalVariable->GlobalType)
											{
											case INPUT_VARIABLE:
												if(pGlobalVariable->pValue)
												{
													puOffset = VariableOffsetMap.Get(pConstantInstruction->uSourceIndex2);
													if(puOffset)
													{
														uOffset = *puOffset;

														uRegister = 0;

														while(pVariable)
														{
															pCurrentVariable = pVariable;

															uRegister += pVariable->uRegister;
															pVariable = pVariable->uParent ? m_VariablePool.Get(pVariable->uParent - 1) : ALCHEMY_NULL;
														}

														pVariable = pCurrentVariable;

														puOffset = ConstantOffsetMap.Get(uRegister);
														if(puOffset)
														{
															if(!pGlobalInputHandler->m_ppInputValue)
															{
																ALCHEMY_DEBUG_NEW(pGlobalInputHandler->m_ppInputValue, CGlobalInputHandler::LPINPUTVALUE[uParameterListCount]);

																for(j = 0; j < uParameterListCount; ++ j)
																	pGlobalInputHandler->m_ppInputValue[j] = ALCHEMY_NULL;
															}

															pInputValue = pGlobalInputHandler->m_ppInputValue[uRenderTargetIndexCount];
															if(!pInputValue)
															{
																uCurrentCount = pGlobalInputHandler->m_puInputValueCount[uRenderTargetIndexCount];
																ALCHEMY_DEBUG_NEW(pInputValue, CGlobalInputHandler::INPUTVALUE[uCurrentCount]);

																for(j = 0; j < uCurrentCount; ++ j)
																{
																	pInputValue->pValue = ALCHEMY_NULL;
																	pInputValue->uIndex = 0;
																	pInputValue->uSize  = 0;
																}

																pGlobalInputHandler->m_ppInputValue[uRenderTargetIndexCount] = pInputValue;
															}

															pInputValue += *puOffset;
															pInputValue->pValue = pGlobalVariable->pValue;
															pInputValue->uIndex = uOffset;
															pInputValue->uSize  = GetRegisterCount(pVariable->Type) * pGlobalVariable->uCount;
														}
													}
												}
												break;
											case CAMERA_VARIABLE:
												if(pGlobalVariable->uDetail < CRenderMethod::TOTAL_CAMERA_PARAMETERS && pGlobalVariable->uIndex < pCurrentConstantParameterList->uCameraCount)
												{
													puOffset = VariableOffsetMap.Get(pConstantInstruction->uSourceIndex2);
													if(puOffset)
													{
														uOffset = *puOffset;

														pParameterInfo = &pCurrentConstantParameterList->pCameraParameters[pGlobalVariable->uIndex][pGlobalVariable->uDetail];

														if(ALCHEMY_TEST_BIT(pCurrentConstantParameterList->puCameraParameterFlag[pGlobalVariable->uIndex], pGlobalVariable->uDetail) && pGlobalVariable->ShaderType != pParameterInfo->Type)
														{
															if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
																pParameterInfo->uBetaIndex = uOffset;
															else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
																pParameterInfo->uAlphaIndex = uOffset;

															pParameterInfo->Type = IEffect::PROGRAM;
														}
														else
														{
															pParameterInfo->Type   = pGlobalVariable->ShaderType;
															pParameterInfo->uAlphaIndex = uOffset;
														}

														ALCHEMY_SET_BIT(pCurrentConstantParameterList->puCameraParameterFlag[pGlobalVariable->uIndex], pGlobalVariable->uDetail);
													}
												}
												break;
											case LIGHT_VARIABLE:
												if(pGlobalVariable->uDetail < CRenderMethod::TOTAL_LIGHT_PARAMETERS && pGlobalVariable->uIndex < pCurrentConstantParameterList->uLightCount)
												{
													puOffset = VariableOffsetMap.Get(pConstantInstruction->uSourceIndex2);
													if(puOffset)
													{
														uOffset = *puOffset;

														pParameterInfo = &pCurrentConstantParameterList->pLightParameters[pGlobalVariable->uIndex][pGlobalVariable->uDetail];

														if(ALCHEMY_TEST_BIT(pCurrentConstantParameterList->puLightParameterFlag[pGlobalVariable->uIndex], pGlobalVariable->uDetail) && pGlobalVariable->ShaderType != pParameterInfo->Type)
														{
															if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
																pParameterInfo->uBetaIndex = uOffset;
															else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
																pParameterInfo->uAlphaIndex = uOffset;

															pParameterInfo->Type = IEffect::PROGRAM;
														}
														else
														{
															pParameterInfo->Type   = pGlobalVariable->ShaderType;
															pParameterInfo->uAlphaIndex = uOffset;
														}

														ALCHEMY_SET_BIT(pCurrentConstantParameterList->puLightParameterFlag[pGlobalVariable->uIndex], pGlobalVariable->uDetail);
													}
												}
												break;
											case TEXTURE_VARIABLE:
												if(pGlobalVariable->uDetail < CRenderMethod::TOTAL_TEXTURE_PARAMETERS && pGlobalVariable->uIndex < pCurrentConstantParameterList->uTextureCount)
												{
													puOffset = VariableOffsetMap.Get(pConstantInstruction->uSourceIndex2);
													if(puOffset)
													{
														uOffset = *puOffset;

														pParameterInfo = &pCurrentConstantParameterList->pTextureParameters[pGlobalVariable->uIndex][pGlobalVariable->uDetail];

														if(ALCHEMY_TEST_BIT(pCurrentConstantParameterList->puTextureParameterFlag[pGlobalVariable->uIndex], pGlobalVariable->uDetail) && pGlobalVariable->ShaderType != pParameterInfo->Type)
														{
															if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
																pParameterInfo->uBetaIndex = uOffset;
															else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
																pParameterInfo->uAlphaIndex = uOffset;

															pParameterInfo->Type = IEffect::PROGRAM;
														}
														else
														{
															pParameterInfo->Type = pGlobalVariable->ShaderType;
															pParameterInfo->uAlphaIndex = uOffset;
														}

														ALCHEMY_SET_BIT(pCurrentConstantParameterList->puTextureParameterFlag[pGlobalVariable->uIndex], pGlobalVariable->uDetail);
													}
												}
												break;
                                            default:
                                                break;
											}
										}
									}
								}

								pConstantInstruction = pConstantInstruction->uPrevious ? m_InstructionPool.Get(pConstantInstruction->uPrevious - 1) : ALCHEMY_NULL;

								if(pConstantInstruction == ALCHEMY_NULL && Type == IEffect::VERTEX_SHADER)
								{
									puConstantInstruction = m_InstructionSetPool.Get(pRenderTargetCode->uFragmentConstantInstructionSet - 1);

									pConstantInstruction  = (puConstantInstruction && *puConstantInstruction) ? m_InstructionPool.Get(*puConstantInstruction - 1) : ALCHEMY_NULL;

									Type = IEffect::FRAGMENT_SHADER;
								}
							}
						}
					}
				}

				pCurrentParameterList->pTextures = ALCHEMY_NULL;
				pCurrentParameterList->pSamplerDeclaration = ALCHEMY_NULL;
				
				pCurrentParameterList->pCameraParameters = ALCHEMY_NULL;
				pCurrentParameterList->pLightParameters = ALCHEMY_NULL;
				pCurrentParameterList->pTextureParameters = ALCHEMY_NULL;

				pCurrentParameterList->uParameterFlag = 0;
				pCurrentParameterList->uArrayParameterFlag = 0;
				//pCurrentParameterList->uMatrixParameterFlag = 0;
				//pCurrentParameterList->uCameraParameterFlag = 0;
				pCurrentParameterList->uMaterialParameterFlag = 0;
				pCurrentParameterList->uConstantParameterFlag = 0;

				pCurrentParameterList->puCameraParameterFlag = ALCHEMY_NULL;
				pCurrentParameterList->puLightParameterFlag = ALCHEMY_NULL;
				pCurrentParameterList->puTextureParameterFlag = ALCHEMY_NULL;

				pCurrentParameterList->uCameraCount = 0;
				pCurrentParameterList->uLightCount = 0;
				pCurrentParameterList->uTextureCount = 0;

				puUserVariable = pRenderTargetCode->uVertexUserVariables/*pVertexShaderFunction->uUserVariables*/ ? m_UserVariablesPool.Get(pRenderTargetCode->uVertexUserVariables/*pVertexShaderFunction->uUserVariables*/ - 1) : ALCHEMY_NULL;
				pUserVariable  = (puUserVariable && *puUserVariable) ? m_UserVariablePool.Get(*puUserVariable - 1) : ALCHEMY_NULL;
				Type = IEffect::VERTEX_SHADER;
				uMinRegister                  = ~0;
				uVertexSamplerCount           = 0;
				uFragmentSamplerCount         = 0;
				uConstantVertexSamplerCount   = 0;
				uConstantFragmentSamplerCount = 0;
				while(pUserVariable)
				{
					switch(pUserVariable->RegisterType)
					{
					//case ICompiler::PARAMETER:
						//pShaderParameter = pUserVariable->uDeclaration ? m_ShaderParameterPool.Get(pUserVariable->uDeclaration - 1) : ALCHEMY_NULL;
						//if(pShaderParameter)
						//	++ pCurrentParameterList->uVertexElementCount;
						//break;
					case ICompiler::GLOBAL:
						pGlobalVariable = pUserVariable->uDeclaration ? m_GlobalVariablePool.Get(pUserVariable->uDeclaration - 1) : ALCHEMY_NULL;
						if(pGlobalVariable)
						{
							uMinRegister = ALCHEMY_MININUM(uMinRegister, pUserVariable->uRegister);
							
							uConstantFragmentSamplerCount = uFragmentSamplerCount;
							
							if(pGlobalVariable->pValue || pGlobalVariable->GlobalType == INPUT_VARIABLE || pGlobalVariable->GlobalType == BUFFER_VARIABLE)
							{
								if(ppGlobalInputHandler)
								{
									if(!pGlobalInputHandler)
									{
										pGlobalInputHandler = *ppGlobalInputHandler;
										if(pGlobalInputHandler)
											pGlobalInputHandler->Destroy();
										else
											ALCHEMY_DEBUG_NEW(pGlobalInputHandler, CGlobalInputHandler);

										pGlobalInputHandler->m_uInputCount = uParameterListCount;

										ALCHEMY_DEBUG_NEW(pGlobalInputHandler->m_ppGlobalInput     , CGlobalInputHandler::LPGLOBALINPUT[uParameterListCount]);
										ALCHEMY_DEBUG_NEW(pGlobalInputHandler->m_puGlobalInputCount, UINT                              [uParameterListCount]);

										for(j = 0; j < uParameterListCount; ++ j)
										{
											pGlobalInputHandler->m_ppGlobalInput     [j] = ALCHEMY_NULL;
											pGlobalInputHandler->m_puGlobalInputCount[j] = 0;
										}

										*ppGlobalInputHandler = pGlobalInputHandler;
									}

									++ pGlobalInputHandler->m_puGlobalInputCount[uRenderTargetIndexCount];
								}
							}
							else
							{
								switch(pGlobalVariable->GlobalType)
								{
								
								case CAMERA_VARIABLE:
									if(pGlobalVariable->uDetail < CRenderMethod::TOTAL_CAMERA_PARAMETERS)
									{
										uCurrentCount = pGlobalVariable->uIndex + 1;

										pCurrentParameterList->uCameraCount = ALCHEMY_MAXINUM(pCurrentParameterList->uCameraCount, uCurrentCount);
									}
									break;
								case LIGHT_VARIABLE:
									if(pGlobalVariable->uDetail < CRenderMethod::TOTAL_LIGHT_PARAMETERS)
									{
										uCurrentCount = pGlobalVariable->uIndex + 1;

										pCurrentParameterList->uLightCount = ALCHEMY_MAXINUM(pCurrentParameterList->uLightCount, uCurrentCount);
									}
									break;
                                default:
                                    break;
								}
							}
						}
						break;

					case ICompiler::SAMPLER:
						puCount = m_SamplerCountMap.Get(pUserVariable->uRegister);
						if(!puCount)
							m_SamplerCountMap.Insert(pUserVariable->uRegister, uFragmentSamplerCount ++);
						
						++ pCurrentParameterList->uTextureCount;
						
						//uMaxFragmentSamplerIndex = ALCHEMY_MAXINUM(uMaxFragmentSamplerIndex, pUserVariable->uRegister + 1);
						break;
                    default:
                        break;
					}

					pUserVariable = pUserVariable->uNext ? m_UserVariablePool.Get(pUserVariable->uNext - 1) : ALCHEMY_NULL;

					if(!pUserVariable && Type == IEffect::VERTEX_SHADER)
					{
						Type = IEffect::FRAGMENT_SHADER;
						uVertexSamplerCount           = uFragmentSamplerCount;
						uConstantVertexSamplerCount   = uConstantFragmentSamplerCount;
						uFragmentSamplerCount         = 0;
						uConstantFragmentSamplerCount = 0;

						puUserVariable = pRenderTargetCode->uFragmentUserVariables/*pFragmentShaderFunction->uUserVariables*/ ? m_UserVariablesPool.Get(pRenderTargetCode->uFragmentUserVariables/*pFragmentShaderFunction->uUserVariables*/ - 1) : ALCHEMY_NULL;
						pUserVariable  = (puUserVariable && *puUserVariable) ? m_UserVariablePool.Get(*puUserVariable - 1) : ALCHEMY_NULL;
					}
				}
				
				uConstantVertexSamplerCount   = uVertexSamplerCount   - uConstantVertexSamplerCount;
				uConstantFragmentSamplerCount = uFragmentSamplerCount - uConstantFragmentSamplerCount;

				ALCHEMY_DEBUG_NEW(pCurrentParameterList->pCameraParameters     , CRenderMethod::PARAMETERINFO       [pCurrentParameterList->uCameraCount ][CRenderMethod::TOTAL_CAMERA_PARAMETERS ]);
				ALCHEMY_DEBUG_NEW(pCurrentParameterList->pLightParameters      , CRenderMethod::PARAMETERINFO       [pCurrentParameterList->uLightCount  ][CRenderMethod::TOTAL_LIGHT_PARAMETERS  ]);
				ALCHEMY_DEBUG_NEW(pCurrentParameterList->pTextureParameters    , CRenderMethod::PARAMETERINFO       [pCurrentParameterList->uTextureCount][CRenderMethod::TOTAL_TEXTURE_PARAMETERS]);

				ALCHEMY_DEBUG_NEW(pCurrentParameterList->pTextures             , CRenderMethod::PARAMETERINFO       [pCurrentParameterList->uTextureCount                                         ]);
				ALCHEMY_DEBUG_NEW(pCurrentParameterList->pSamplerDeclaration   , IEffect::SAMPLERDECLARATION        [pCurrentParameterList->uTextureCount                                         ]);

				ALCHEMY_DEBUG_NEW(pCurrentParameterList->puCameraParameterFlag , UINT                               [pCurrentParameterList->uCameraCount                                          ]);
				ALCHEMY_DEBUG_NEW(pCurrentParameterList->puLightParameterFlag  , UINT                               [pCurrentParameterList->uLightCount                                           ]);
				ALCHEMY_DEBUG_NEW(pCurrentParameterList->puTextureParameterFlag, UINT                               [pCurrentParameterList->uTextureCount                                         ]);

				for(j = 0; j < pCurrentParameterList->uCameraCount; ++ j)
					pCurrentParameterList->puCameraParameterFlag[j] = 0;

				for(j = 0; j < pCurrentParameterList->uLightCount; ++ j)
					pCurrentParameterList->puLightParameterFlag[j] = 0;

				for(j = 0; j < pCurrentParameterList->uTextureCount; ++ j)
					pCurrentParameterList->puTextureParameterFlag[j] = 0;

				if(pGlobalInputHandler)
				{
					ALCHEMY_DEBUG_NEW(pGlobalInputHandler->m_ppGlobalInput[uRenderTargetIndexCount], CGlobalInputHandler::GLOBALINPUT[pGlobalInputHandler->m_puGlobalInputCount[uRenderTargetIndexCount]]);

					pGlobalInputHandler->m_puGlobalInputCount[uRenderTargetIndexCount] = 0;
				}

				//ALCHEMY_DEBUG_NEW(pCurrentParameterList->pVertexElements, ELEMENTDECLARATION[pCurrentParameterList->uVertexElementCount]);

				//pCurrentParameterList->uVertexElementCount = 0;

				uCurrentCount = 0;

				puUserVariable = pRenderTargetCode->uVertexUserVariables/*pVertexShaderFunction->uUserVariables*/ ? m_UserVariablesPool.Get(pRenderTargetCode->uVertexUserVariables/*pVertexShaderFunction->uUserVariables*/ - 1) : ALCHEMY_NULL;
				pUserVariable  = (puUserVariable && *puUserVariable) ? m_UserVariablePool.Get(*puUserVariable - 1) : ALCHEMY_NULL;
				Type = IEffect::VERTEX_SHADER;
				uRegisterOffset       = 0;
				uMaxRegister          = 0;
				uSamplerCount         = uVertexSamplerCount;
				uConstantSamplerCount = uConstantVertexSamplerCount;
				while(pUserVariable)
				{
					switch(pUserVariable->RegisterType)
					{
					/*case ICompiler::PARAMETER:
						pShaderParameter = pUserVariable->uDeclaration ? m_ShaderParameterPool.Get(pUserVariable->uDeclaration - 1) : ALCHEMY_NULL;
						if(pShaderParameter)
						{
							pVertexElement = &pCurrentParameterList->pVertexElements[pCurrentParameterList->uVertexElementCount ++];
							pVertexElement->Type   = pShaderParameter->Type;
							pVertexElement->Usage  = pShaderParameter->Usage;
							pVertexElement->uIndex = pShaderParameter->uIndex;
						}
						break;*/
					case ICompiler::GLOBAL:
						pGlobalVariable = pUserVariable->uDeclaration ? m_GlobalVariablePool.Get(pUserVariable->uDeclaration - 1) : ALCHEMY_NULL;
						if(pGlobalVariable)
						{
							uRegisterIndex = m_Compiler.ComputeRegisterLocation(pUserVariable->uRegister, uRegisterOffset, uMinRegister, uSamplerCount, uConstantSamplerCount);
							uMaxRegister = ALCHEMY_MAXINUM(uMaxRegister, 
														   pUserVariable->uRegister + GetRegisterCount(pUserVariable->VariableType) * pUserVariable->uCount);
							
							if(pGlobalVariable->pValue || pGlobalVariable->GlobalType == INPUT_VARIABLE || pGlobalVariable->GlobalType == BUFFER_VARIABLE)
							{
								if(pGlobalInputHandler)
								{
									pGlobalInput = &pGlobalInputHandler->m_ppGlobalInput[uRenderTargetIndexCount][pGlobalInputHandler->m_puGlobalInputCount[uRenderTargetIndexCount] ++];
									pGlobalInput->bIsStatic = true;
									if(pGlobalVariable->GlobalType == INPUT_VARIABLE)
									{
										pGlobalInput->uCode = pGlobalVariable->uDetail;

										if(!pGlobalVariable->pValue)
										{
											puOffset = ConstantOffsetMap.Get(pUserVariable->uRegister);
											if(!puOffset)
											{
												uOffset  = uCurrentMaxOffset;
												puOffset = &uOffset;
												uCurrentMaxOffset += GetRegisterCount(pUserVariable->VariableType, 0, false) * pUserVariable->uCount;
												pGlobalInputHandler->m_uParameterBufferSize = ALCHEMY_MAXINUM(pGlobalInputHandler->m_uParameterBufferSize, uCurrentMaxOffset);
											}
											
											ConstantInputInfo.uOffset = *puOffset;
											ConstantInputInfo.ppValue = &pGlobalInput->pValue;
											
											if(pGlobalVariable->uDetail >> GLOBAL_CODE_SHIFT)
											{
												switch (pGlobalVariable->uDetail) 
												{
													case VECTOR1_SIZE:
														puOffset = ConstantInputValueMap.Get(VECTOR1_SIZE);
														if(puOffset)
															uOffset = *puOffset;
														else
														{
															uOffset = ConstantInputInfoValue.Add( static_cast<FLOAT>(GetRegisterCount(VECTOR1_VARIABLE) >> uByteAlignmentShift) );
															ConstantInputValueMap.Insert(VECTOR1_SIZE, uOffset);
														}
														
														ConstantInputInfo.uValueFrom  = uOffset;
														ConstantInputInfo.uValueCount = 1;
														break;
													case VECTOR2_SIZE:
														puOffset = ConstantInputValueMap.Get(VECTOR2_SIZE);
														if(puOffset)
															uOffset = *puOffset;
														else
														{
															uOffset = ConstantInputInfoValue.Add( static_cast<FLOAT>(GetRegisterCount(VECTOR2_VARIABLE) >> uByteAlignmentShift) );
															ConstantInputValueMap.Insert(VECTOR2_SIZE, uOffset);
														}
														
														ConstantInputInfo.uValueFrom  = uOffset;
														ConstantInputInfo.uValueCount = 1;
														break;
													case VECTOR3_SIZE:
														puOffset = ConstantInputValueMap.Get(VECTOR3_SIZE);
														if(puOffset)
															uOffset = *puOffset;
														else
														{
															uOffset = ConstantInputInfoValue.Add( static_cast<FLOAT>(GetRegisterCount(VECTOR3_VARIABLE) >> uByteAlignmentShift) );
															ConstantInputValueMap.Insert(VECTOR3_SIZE, uOffset);
														}
														
														ConstantInputInfo.uValueFrom  = uOffset;
														ConstantInputInfo.uValueCount = 1;
														break;
													case VECTOR4_SIZE:
														puOffset = ConstantInputValueMap.Get(VECTOR4_SIZE);
														if(puOffset)
															uOffset = *puOffset;
														else
														{
															uOffset = ConstantInputInfoValue.Add( static_cast<FLOAT>(GetRegisterCount(VECTOR4_VARIABLE) >> uByteAlignmentShift) );
															ConstantInputValueMap.Insert(VECTOR4_SIZE, uOffset);
														}
														
														ConstantInputInfo.uValueFrom  = uOffset;
														ConstantInputInfo.uValueCount = 1;
														break;
													case MATRIX2X2_SIZE:
														puOffset = ConstantInputValueMap.Get(MATRIX2X2_SIZE);
														if(puOffset)
															uOffset = *puOffset;
														else
														{
															uOffset = ConstantInputInfoValue.Add( static_cast<FLOAT>(GetRegisterCount(MATRIX2X2_VARIABLE) >> uByteAlignmentShift) );
															ConstantInputValueMap.Insert(MATRIX2X2_SIZE, uOffset);
														}
														
														ConstantInputInfo.uValueFrom  = uOffset;
														ConstantInputInfo.uValueCount = 1;
														break;
													case MATRIX3X3_SIZE:
														puOffset = ConstantInputValueMap.Get(MATRIX3X3_SIZE);
														if(puOffset)
															uOffset = *puOffset;
														else
														{
															uOffset = ConstantInputInfoValue.Add( static_cast<FLOAT>(GetRegisterCount(MATRIX3X3_VARIABLE) >> uByteAlignmentShift) );
															ConstantInputValueMap.Insert(MATRIX3X3_SIZE, uOffset);
														}
														
														ConstantInputInfo.uValueFrom  = uOffset;
														ConstantInputInfo.uValueCount = 1;
														break;
													case MATRIX4X3_SIZE:
														puOffset = ConstantInputValueMap.Get(MATRIX4X3_SIZE);
														if(puOffset)
															uOffset = *puOffset;
														else
														{
															uOffset = ConstantInputInfoValue.Add( static_cast<FLOAT>(GetRegisterCount(MATRIX4X3_VARIABLE) >> uByteAlignmentShift) );
															ConstantInputValueMap.Insert(MATRIX4X3_SIZE, uOffset);
														}
														
														ConstantInputInfo.uValueFrom  = uOffset;
														ConstantInputInfo.uValueCount = 1;
														break;
													case MATRIX4X4_SIZE:
														puOffset = ConstantInputValueMap.Get(MATRIX4X4_SIZE);
														if(puOffset)
															uOffset = *puOffset;
														else
														{
															uOffset = ConstantInputInfoValue.Add( static_cast<FLOAT>(GetRegisterCount(MATRIX4X4_VARIABLE) >> uByteAlignmentShift) );
															ConstantInputValueMap.Insert(MATRIX4X4_SIZE, uOffset);
														}
														
														ConstantInputInfo.uValueFrom  = uOffset;
														ConstantInputInfo.uValueCount = 1;
														break;
													default:
														ConstantInputInfo.uValueFrom  = 0;
														ConstantInputInfo.uValueCount = 0;
														break;
												}
											}
											else 
											{
												ConstantInputInfo.uValueFrom  = 0;
												ConstantInputInfo.uValueCount = 0;
											}


											ConstantInputInfos.Add(ConstantInputInfo);

											pGlobalInput->bIsStatic = false;
										}
									}
									else if(pGlobalVariable->GlobalType == BUFFER_VARIABLE)
									{
										pGlobalInput->uCode = BUFFER_STRIDE_SIZE;
										
										puOffset = ConstantOffsetMap.Get(pUserVariable->uRegister);
										if(!puOffset)
										{
											uOffset  = uCurrentMaxOffset;
											puOffset = &uOffset;
											uCurrentMaxOffset += GetRegisterCount(pUserVariable->VariableType, 0, false) * pUserVariable->uCount;
											pGlobalInputHandler->m_uParameterBufferSize = ALCHEMY_MAXINUM(pGlobalInputHandler->m_uParameterBufferSize, uCurrentMaxOffset);
										}
										
										ConstantInputInfo.uOffset = *puOffset;
										ConstantInputInfo.ppValue = &pGlobalInput->pValue;
										
										puOffset = ConstantInputValueMap.Get(BUFFER_STRIDE_SIZE);
										if(puOffset)
											uOffset = *puOffset;
										else
										{
											pConstantBufferDeclaration = pGlobalVariable->uDetail ? m_ConstantBufferDeclarationPool.Get(pGlobalVariable->uDetail - 1) : ALCHEMY_NULL;
											uOffset = ConstantInputInfoValue.GetLength();
											uValue = pConstantBufferDeclaration ? ALCHEMY_MAXINUM_BIT(pConstantBufferDeclaration->uRegisterCount) : 0;
											uTemp  = ALCHEMY_FLAG(uValue >> 1);
											ConstantInputInfoValue.Add(static_cast<FLOAT>( pGlobalVariable->uIndex & (uTemp - 1) ) / uTemp);
											uTemp  = ALCHEMY_FLAG( ( (uValue & 1) ? (uValue + 1) : uValue ) >> 1);
											ConstantInputInfoValue.Add(static_cast<FLOAT>( pGlobalVariable->uIndex >> (uValue >> 1) ) / uTemp);
											ConstantInputValueMap.Insert(BUFFER_STRIDE_SIZE, uOffset);
										}
										
										ConstantInputInfo.uValueFrom  = uOffset;
										ConstantInputInfo.uValueCount = 2;
										
										ConstantInputInfos.Add(ConstantInputInfo);
									}
									else
										pGlobalInput->uCode = BuildGlobalInputCode( GetGlobalVariableType(pGlobalVariable->GlobalType, pGlobalVariable->uDetail) );

									pGlobalInput->ShaderType = pGlobalVariable->ShaderType;
									pGlobalInput->uIndex     = uRegisterIndex;
									pGlobalInput->uCount     = pGlobalVariable->uCount;
									pGlobalInput->pValue     = pGlobalVariable->pValue;
								}
							}
							else
							{
								switch(pGlobalVariable->GlobalType)
								{
								case NORMAL_VARIABLE:
									if(!pGlobalVariable->uIndex && pGlobalVariable->uDetail < IRenderMethod::TOTAL_PARAMETERS)
									{
										pParameterInfo = &pCurrentParameterList->Parameters[pGlobalVariable->uDetail];

										if(ALCHEMY_TEST_BIT(pCurrentParameterList->uParameterFlag, pGlobalVariable->uDetail) && Type != pParameterInfo->Type)
										{
											if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
												pParameterInfo->uBetaIndex = uRegisterIndex;
											else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
												pParameterInfo->uAlphaIndex = uRegisterIndex;

											pParameterInfo->Type = IEffect::PROGRAM;
										}
										else
										{
											pParameterInfo->Type        = Type;
											pParameterInfo->uAlphaIndex = uRegisterIndex;
										}

										ALCHEMY_SET_BIT(pCurrentParameterList->uParameterFlag, pGlobalVariable->uDetail);
									}
									break;

								case ARRAY_VARIABLE:
									if(!pGlobalVariable->uIndex && pGlobalVariable->uDetail < IRenderMethod::TOTAL_ARRAY_PARAMETERS)
									{
										pParameterInfo = &pCurrentParameterList->ArrayParameters[pGlobalVariable->uDetail];

										if(ALCHEMY_TEST_BIT(pCurrentParameterList->uArrayParameterFlag, pGlobalVariable->uDetail) && Type != pParameterInfo->Type)
										{
											if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
												pParameterInfo->uBetaIndex = uRegisterIndex;
											else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
												pParameterInfo->uAlphaIndex = uRegisterIndex;

											pParameterInfo->Type = IEffect::PROGRAM;
										}
										else
										{
											pParameterInfo->Type        = Type;
											pParameterInfo->uAlphaIndex = uRegisterIndex;
										}

										ALCHEMY_SET_BIT(pCurrentParameterList->uArrayParameterFlag, pGlobalVariable->uDetail);
									}
									break;

								/*case MATRIX_VARIABLE:
									if(!pGlobalVariable->uIndex && pGlobalVariable->uDetail < CRenderMethod::TOTAL_MATRIX_PARAMETERS)
									{
										pParameterInfo = &pCurrentParameterList->MatrixParameters[pGlobalVariable->uDetail];

										if(ALCHEMY_TEST_BIT(pCurrentParameterList->uMatrixParameterFlag, pGlobalVariable->uDetail) && Type != pParameterInfo->Type)
										{
											if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
												pParameterInfo->uBetaIndex = pUserVariable->uRegister >> uByteAlignmentShift;
											else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
												pParameterInfo->uAlphaIndex = pUserVariable->uRegister >> uByteAlignmentShift;

											pParameterInfo->Type = IEffect::PROGRAM;
										}
										else
										{
											pParameterInfo->Type   = Type;
											pParameterInfo->uAlphaIndex = pUserVariable->uRegister >> uByteAlignmentShift;
										}

										ALCHEMY_SET_BIT(pCurrentParameterList->uMatrixParameterFlag, pGlobalVariable->uDetail);
									}
									break;
								case CAMERA_VARIABLE:
									if(!pGlobalVariable->uIndex && pGlobalVariable->uDetail < CRenderMethod::TOTAL_CAMERA_PARAMETERS)
									{
										pParameterInfo = &pCurrentParameterList->CameraParameters[pGlobalVariable->uDetail];

										if(ALCHEMY_TEST_BIT(pCurrentParameterList->uCameraParameterFlag, pGlobalVariable->uDetail) && Type != pParameterInfo->Type)
										{
											if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
												pParameterInfo->uBetaIndex = pUserVariable->uRegister >> uByteAlignmentShift;
											else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
												pParameterInfo->uAlphaIndex = pUserVariable->uRegister >> uByteAlignmentShift;

											pParameterInfo->Type = IEffect::PROGRAM;
										}
										else
										{
											pParameterInfo->Type        = Type;
											pParameterInfo->uAlphaIndex = pUserVariable->uRegister >> uByteAlignmentShift;
										}

										ALCHEMY_SET_BIT(pCurrentParameterList->uCameraParameterFlag, pGlobalVariable->uDetail);
									}
									break;*/
								case MATERIAL_VARIABLE:
									if(!pGlobalVariable->uIndex && pGlobalVariable->uDetail < CRenderMethod::TOTAL_MATERIAL_PARAMETERS)
									{
										pParameterInfo = &pCurrentParameterList->MaterialParameters[pGlobalVariable->uDetail];

										if(ALCHEMY_TEST_BIT(pCurrentParameterList->uMaterialParameterFlag, pGlobalVariable->uDetail) && Type != pParameterInfo->Type)
										{
											if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
												pParameterInfo->uBetaIndex = uRegisterIndex;
											else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
												pParameterInfo->uAlphaIndex = uRegisterIndex;

											pParameterInfo->Type = IEffect::PROGRAM;
										}
										else
										{
											pParameterInfo->Type        = Type;
											pParameterInfo->uAlphaIndex = uRegisterIndex;
										}

										ALCHEMY_SET_BIT(pCurrentParameterList->uMaterialParameterFlag, pGlobalVariable->uDetail);
									}
									break;
								case CAMERA_VARIABLE:
									if(pGlobalVariable->uDetail < CRenderMethod::TOTAL_CAMERA_PARAMETERS)
									{
										pParameterInfo = &pCurrentParameterList->pCameraParameters[pGlobalVariable->uIndex][pGlobalVariable->uDetail];

										if(ALCHEMY_TEST_BIT(pCurrentParameterList->puCameraParameterFlag[pGlobalVariable->uIndex], pGlobalVariable->uDetail) && Type != pParameterInfo->Type)
										{
											if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
												pParameterInfo->uBetaIndex = uRegisterIndex;
											else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
												pParameterInfo->uAlphaIndex = uRegisterIndex;

											pParameterInfo->Type = IEffect::PROGRAM;
										}
										else
										{
											pParameterInfo->Type   = Type;
											pParameterInfo->uAlphaIndex = uRegisterIndex;
										}

										ALCHEMY_SET_BIT(pCurrentParameterList->puCameraParameterFlag[pGlobalVariable->uIndex], pGlobalVariable->uDetail);
									}
									break;
								case LIGHT_VARIABLE:
									if(pGlobalVariable->uDetail < CRenderMethod::TOTAL_LIGHT_PARAMETERS)
									{
										pParameterInfo = &pCurrentParameterList->pLightParameters[pGlobalVariable->uIndex][pGlobalVariable->uDetail];

										if(ALCHEMY_TEST_BIT(pCurrentParameterList->puLightParameterFlag[pGlobalVariable->uIndex], pGlobalVariable->uDetail) && Type != pParameterInfo->Type)
										{
											if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
												pParameterInfo->uBetaIndex = uRegisterIndex;
											else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
												pParameterInfo->uAlphaIndex = uRegisterIndex;

											pParameterInfo->Type = IEffect::PROGRAM;
										}
										else
										{
											pParameterInfo->Type   = Type;
											pParameterInfo->uAlphaIndex = uRegisterIndex;
										}

										ALCHEMY_SET_BIT(pCurrentParameterList->puLightParameterFlag[pGlobalVariable->uIndex], pGlobalVariable->uDetail);
									}
									break;
								case TEXTURE_VARIABLE:
									if(pGlobalVariable->uDetail < CRenderMethod::TOTAL_TEXTURE_PARAMETERS && pGlobalVariable->uIndex < pCurrentParameterList->uTextureCount)
									{
										pParameterInfo = &pCurrentParameterList->pTextureParameters[pGlobalVariable->uIndex][pGlobalVariable->uDetail];

										if(ALCHEMY_TEST_BIT(pCurrentParameterList->puTextureParameterFlag[pGlobalVariable->uIndex], pGlobalVariable->uDetail) && Type != pParameterInfo->Type)
										{
											if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
												pParameterInfo->uBetaIndex = uRegisterIndex;
											else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
												pParameterInfo->uAlphaIndex = uRegisterIndex;

											pParameterInfo->Type = IEffect::PROGRAM;
										}
										else
										{
											pParameterInfo->Type = Type;
											pParameterInfo->uAlphaIndex = uRegisterIndex;
										}

										ALCHEMY_SET_BIT(pCurrentParameterList->puTextureParameterFlag[pGlobalVariable->uIndex], pGlobalVariable->uDetail);
									}
									break;
								case CONSTANT_VARIABLE:
									if(!pGlobalVariable->uIndex && pGlobalVariable->uDetail < CRenderMethod::TOTAL_CONSTANT_PARAMETERS)
									{
										pParameterInfo = &pCurrentParameterList->ConstantParameters[pGlobalVariable->uDetail];

										if(ALCHEMY_TEST_BIT(pCurrentParameterList->uConstantParameterFlag, pGlobalVariable->uDetail) && Type != pParameterInfo->Type)
										{
											if(pParameterInfo->Type == IEffect::VERTEX_SHADER)
												pParameterInfo->uBetaIndex = uRegisterIndex;
											else if(pParameterInfo->Type == IEffect::FRAGMENT_SHADER)
												pParameterInfo->uAlphaIndex = uRegisterIndex;

											pParameterInfo->Type = IEffect::PROGRAM;
										}
										else
										{
											pParameterInfo->Type        = Type;
											pParameterInfo->uAlphaIndex = uRegisterIndex;
										}

										ALCHEMY_SET_BIT(pCurrentParameterList->uConstantParameterFlag, pGlobalVariable->uDetail);
									}
									break;
								default:
									break;
								}
							}
						}
						break;

					case ICompiler::SAMPLER:
						pParameterInfo = &pCurrentParameterList->pTextures[uCurrentCount];
						pParameterInfo->Type        = Type;
						pParameterInfo->uAlphaIndex = pUserVariable->uRegister;
						
						pSamplerDeclaration = pUserVariable->uDeclaration ? m_SamplerDeclarationPool.Get(pUserVariable->uDeclaration - 1) : ALCHEMY_NULL;
						if(pSamplerDeclaration)
							pCurrentParameterList->pSamplerDeclaration[uCurrentCount] = *pSamplerDeclaration;
							
						++ uCurrentCount;
						break;
                            
                    default:
                        break;
					}

					pUserVariable = pUserVariable->uNext ? m_UserVariablePool.Get(pUserVariable->uNext - 1) : ALCHEMY_NULL;

					if(!pUserVariable && Type == IEffect::VERTEX_SHADER)
					{
						Type = IEffect::FRAGMENT_SHADER;
						uRegisterOffset       = uMaxRegister;
						uSamplerCount         = uFragmentSamplerCount;
						uConstantSamplerCount = uConstantFragmentSamplerCount;

						puUserVariable = pRenderTargetCode->uFragmentUserVariables/*pFragmentShaderFunction->uUserVariables*/ ? m_UserVariablesPool.Get(pRenderTargetCode->uFragmentUserVariables/*pFragmentShaderFunction->uUserVariables*/ - 1) : ALCHEMY_NULL;
						pUserVariable  = (puUserVariable && *puUserVariable) ? m_UserVariablePool.Get(*puUserVariable - 1) : ALCHEMY_NULL;
					}
				}

				pRenderTargetCode = pRenderTargetCode->uNext ? m_RenderTargetCodePool.Get(pRenderTargetCode->uNext - 1) : ALCHEMY_NULL;

				++ pCurrentParameterList;

				++ uRenderTargetIndexCount;
			}
		}
	}

	if(pGlobalInputHandler)
	{
		ALCHEMY_DEBUG_NEW(pGlobalInputHandler->m_pParameterBuffer, UINT8[pGlobalInputHandler->m_uParameterBufferSize]);
		LPCONSTANTINPUTINFO pConstantInputInfo;
		CVector<CONSTANTINPUTINFO>::CIterator Iterator = ConstantInputInfos.GetIterator(0);
		while( CVector<CONSTANTINPUTINFO>::IsValuable(Iterator) )
		{
			pConstantInputInfo = &*(Iterator);
			
			*pConstantInputInfo->ppValue = &pGlobalInputHandler->m_pParameterBuffer[pConstantInputInfo->uOffset];
			if(pConstantInputInfo->uValueCount)
			{
				uOffset = pConstantInputInfo->uOffset;
				for(i = 0; i < pConstantInputInfo->uValueCount; ++ i)
				{
					reinterpret_cast<FLOAT&>(pGlobalInputHandler->m_pParameterBuffer[uOffset]) = ConstantInputInfoValue[pConstantInputInfo->uValueFrom + i];
					
					uOffset += sizeof(FLOAT);
				}
			}

			++ Iterator;
		}
	}
	
	CBuffer& Buffer = CRenderMethod::CreateParameterListBuffer(pParameterList, uParameterListCount);

	CRenderMethod::DestroyParameterList(pParameterList, uParameterListCount);

	return Buffer;
}