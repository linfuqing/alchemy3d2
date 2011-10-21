#include "AlchemyEffectCode.h"
#include "AlchemyEffectInfoInterface.h"
#include "AlchemyEffectInfoBlendMethodInterface.h"
#include "AlchemyRenderMethod.h"

#include "AlchemyEffectInfoAddition.h"
#include "AlchemyEffectInfoModulation.h"

#include "../Core/AlchemyUtils.h"

using namespace alchemy;

CEffectCode::CGlobalInputHandler::CGlobalInputHandler() :
m_ppGlobalInput(ALCHEMY_NULL),
m_puGlobalInputCount(ALCHEMY_NULL),
m_uPassCount(0)
{

}

CEffectCode::CGlobalInputHandler::~CGlobalInputHandler()
{
	if(m_ppGlobalInput)
	{
		for(UINT i = 0; i < m_uPassCount; ++ i)
			ALCHEMY_DEBUG_DELETE_ARRAY(m_ppGlobalInput[i]);

		ALCHEMY_DEBUG_DELETE_ARRAY(m_ppGlobalInput);
	}

	ALCHEMY_DEBUG_DELETE_ARRAY(m_puGlobalInputCount);
}

bool CEffectCode::CGlobalInputHandler::SetParameters(CEffectInterface& Effect, UINT uPass)
{
	if(uPass < m_uPassCount)
	{
		LPGLOBALINPUT pGlobalInput = m_ppGlobalInput[uPass];
		if(pGlobalInput)
		{
			VARIABLETYPE VariableType;
			UINT uRowOffset, uOffset;
			bool bIsPointer;

			UINT uGlobalInpuTCount = m_puGlobalInputCount[uPass];

			for(UINT i = 0; i < uGlobalInpuTCount; ++ i)
			{
				CEffectCode::GetGlobalInputInfo(pGlobalInput->uCode, &VariableType, &uRowOffset, &uOffset, &bIsPointer);

				switch(VariableType)
				{
				case VECTOR1_VARIABLE:
					Effect.SetParameter(pGlobalInput->ShaderType, pGlobalInput->uIndex, reinterpret_cast<const FLOAT*>(pGlobalInput->pValue), 1 * pGlobalInput->uCount);
					break;
				case VECTOR2_VARIABLE:
					Effect.SetParameter(pGlobalInput->ShaderType, pGlobalInput->uIndex, reinterpret_cast<const FLOAT*>(pGlobalInput->pValue), 2 * pGlobalInput->uCount);
					break;
				case VECTOR3_VARIABLE:
					Effect.SetParameter(pGlobalInput->ShaderType, pGlobalInput->uIndex, reinterpret_cast<const FLOAT*>(pGlobalInput->pValue), 3 * pGlobalInput->uCount);
					break;
				case VECTOR4_VARIABLE:
					Effect.SetParameter(pGlobalInput->ShaderType, pGlobalInput->uIndex, reinterpret_cast<const FLOAT*>(pGlobalInput->pValue), 4 * pGlobalInput->uCount);
					break;
				case MATRIX2X2_VARIABLE:
					Effect.SetMatrixArray(pGlobalInput->ShaderType, pGlobalInput->uIndex, reinterpret_cast<const FLOAT*>(pGlobalInput->pValue), 2, 2, uRowOffset, uOffset, pGlobalInput->uCount);
					break;
				case MATRIX3X3_VARIABLE:
					Effect.SetMatrixArray(pGlobalInput->ShaderType, pGlobalInput->uIndex, reinterpret_cast<const FLOAT*>(pGlobalInput->pValue), 3, 3, uRowOffset, uOffset, pGlobalInput->uCount);
					break;
				case MATRIX4X3_VARIABLE:
					Effect.SetMatrixArray(pGlobalInput->ShaderType, pGlobalInput->uIndex, reinterpret_cast<const FLOAT*>(pGlobalInput->pValue), 3, 4, uRowOffset, uOffset, pGlobalInput->uCount);
					break;
				case MATRIX4X4_VARIABLE:
					//Effect.SetMatrix(pGlobalInput->ShaderType, pGlobalInput->uIndex, reinterpret_cast<const MATRIX4X4&>(* (MATRIX4X4*)pGlobalInput->pValue) );
					Effect.SetMatrixArray(pGlobalInput->ShaderType, pGlobalInput->uIndex, reinterpret_cast<const FLOAT*>(pGlobalInput->pValue), 4, 4, uRowOffset, uOffset, pGlobalInput->uCount);
					break;
				}

				++ pGlobalInput;
			}
		}

		return true;
	}

	ALCHEMY_DEBUG_WARNING("CEffectCode::CConstomConstantHandler::SetParameters: Pass out of range.");

	return false;
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

CEffectCode::CShaderParametersDefiner::CShaderParametersDefiner(CEffectCode& Parent) :
CDefiner(Parent),
m_pShaderParameter(ALCHEMY_NULL)
{

}

CEffectCode::CShaderParametersDefiner::~CShaderParametersDefiner()
{

}

const UINT* CEffectCode::CShaderParametersDefiner::GetShaderParameterKeyCount(const SHADERPARAMETERKEY& ShaderParameterKey)
{
	return m_ShaderParameterCountMap.Get(ShaderParameterKey);
}

void CEffectCode::CShaderParametersDefiner::AppendParameter(CVertexDeclaration::ELEMENTTYPE Type, CVertexDeclaration::ELEMENTUSAGE Usage)
{
	SHADERPARAMETERKEY ShaderParameterKey;
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
	}

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
	m_ShaderParameterCountMap.Clear();

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
m_Type(CEffectInterface::PROGRAM),
m_pFunction(ALCHEMY_NULL),
m_uTemporaryRegisterCount(0)
{
}

CEffectCode::CBasicFunctionDefiner::~CBasicFunctionDefiner()
{
}

bool CEffectCode::CBasicFunctionDefiner::AppendInstruction(
					   CCompilerInterface::INSTRUCTION Instruction,
					   UINT uDestination,
					   UINT uSource1,
					   UINT uSource2)
{
	USERVARIABLEKEY UserVariableKey;

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

	UINT uDestinationAddressCount = 0, uAddressCount1 = 0, uAddressCount2 = 0;
	if(pDestinationVariable)
	{
		UserVariableKey.Type = m_Parent.GetRegisterType(uDestination);
		UserVariableKey.uRegister = pDestinationVariable->uRegister;

		m_UserVariableDeclarationMap.Insert(UserVariableKey, (UserVariableKey.Type == CCompilerInterface::GLOBAL || UserVariableKey.Type == CCompilerInterface::PARAMETER) ? pDestinationVariable->uGlobal : 0);

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
		}
	}

	if(pVariable1)
	{
		UserVariableKey.Type = m_Parent.GetRegisterType(uSource1);
		UserVariableKey.uRegister = pVariable1->uRegister;

		m_UserVariableDeclarationMap.Insert(UserVariableKey, (UserVariableKey.Type == CCompilerInterface::GLOBAL || UserVariableKey.Type == CCompilerInterface::PARAMETER) ? pVariable1->uGlobal : 0);

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
		}
	}

	if(pVariable2)
	{
		UserVariableKey.Type = m_Parent.GetRegisterType(uSource2);
		UserVariableKey.uRegister = pVariable2->uRegister;

		m_UserVariableDeclarationMap.Insert(UserVariableKey, (UserVariableKey.Type == CCompilerInterface::GLOBAL || UserVariableKey.Type == CCompilerInterface::PARAMETER) ? pVariable2->uGlobal : 0);

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
		}
	}

	return m_Parent.m_Compiler.AppendInstruction(
		m_Code, 
		m_Type, 
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
		m_Parent);
}

bool CEffectCode::CBasicFunctionDefiner::AppendInstruction(
	CCompilerInterface::INSTRUCTION Instruction,
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
	USERVARIABLEKEY UserVariableKey;

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
		UserVariableKey.Type = m_Parent.GetRegisterType(uDestination);
		UserVariableKey.uRegister = pDestinationVariable->uRegister;

		m_UserVariableDeclarationMap.Insert(UserVariableKey, (UserVariableKey.Type == CCompilerInterface::GLOBAL || UserVariableKey.Type == CCompilerInterface::PARAMETER) ? pDestinationVariable->uGlobal : 0);
	}

	const UINT uADDRESS[] = {0, 1, 2};

	if(pVariable1)
	{
		UserVariableKey.Type = m_Parent.GetRegisterType(uSource1);
		UserVariableKey.uRegister = pVariable1->uRegister;

		m_UserVariableDeclarationMap.Insert(UserVariableKey, (UserVariableKey.Type == CCompilerInterface::GLOBAL || UserVariableKey.Type == CCompilerInterface::PARAMETER) ? pVariable1->uGlobal : 0);

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
			}

			puAddress1 = uADDRESS;
		}
	}

	if(pVariable2)
	{
		UserVariableKey.Type = m_Parent.GetRegisterType(uSource2);
		UserVariableKey.uRegister = pVariable2->uRegister;

		m_UserVariableDeclarationMap.Insert(UserVariableKey, (UserVariableKey.Type == CCompilerInterface::GLOBAL || UserVariableKey.Type == CCompilerInterface::PARAMETER) ? pVariable2->uGlobal : 0);

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
			}

			puAddress2 = uADDRESS;
		}
	}

	return m_Parent.m_Compiler.AppendInstruction(
		m_Code, 
		m_Type, 
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
		m_Parent);
}

bool CEffectCode::CBasicFunctionDefiner::AppendInstruction(
					   CCompilerInterface::INSTRUCTION Instruction, 
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
	USERVARIABLEKEY UserVariableKey;

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
		UserVariableKey.Type = m_Parent.GetRegisterType(uDestination);
		UserVariableKey.uRegister = pDestinationVariable->uRegister;

		m_UserVariableDeclarationMap.Insert(UserVariableKey, (UserVariableKey.Type == CCompilerInterface::GLOBAL || UserVariableKey.Type == CCompilerInterface::PARAMETER) ? pDestinationVariable->uGlobal : 0);
	}

	if(pVariable1)
	{
		UserVariableKey.Type = m_Parent.GetRegisterType(uSource1);
		UserVariableKey.uRegister = pVariable1->uRegister;

		m_UserVariableDeclarationMap.Insert(UserVariableKey, (UserVariableKey.Type == CCompilerInterface::GLOBAL || UserVariableKey.Type == CCompilerInterface::PARAMETER) ? pVariable1->uGlobal : 0);
	}

	if(pVariable2)
	{
		UserVariableKey.Type = m_Parent.GetRegisterType(uSource2);
		UserVariableKey.uRegister = pVariable2->uRegister;

		m_UserVariableDeclarationMap.Insert(UserVariableKey, (UserVariableKey.Type == CCompilerInterface::GLOBAL || UserVariableKey.Type == CCompilerInterface::PARAMETER) ? pVariable2->uGlobal : 0);
	}

	return m_Parent.m_Compiler.AppendInstruction(
		m_Code, 
		m_Type, 
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
		m_Parent);
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
		PUINT puCount = m_VariableCountMap.Get(uHandle);

		if(puCount)
		{
			CPool<VARIABLE>::CIterator VariableIterator = m_Parent.m_VariablePool.GetBeginIterator();
			while( CPool<VARIABLE>::IsValuable(VariableIterator) )
			{
				if(VariableIterator->uParent == uHandle)
					m_Parent.m_VariablePool.Remove(VariableIterator.GetIndex(), 1);

				++ VariableIterator;
			}

			UINT uCount = *puCount, 
				uRegisterCount = m_Parent.GetRegisterCount(pVariable->Type),
				uRegisterBegin = pVariable->uRegister, 
				uByteAlignment = 1 << m_Parent.m_Compiler.GetByteAlignmentShift(), 
				uByteAlignmentMask = uByteAlignment - 1,
				uRegisterEnd,
				uCurrentRegisterCount;

			CPool<REGISTERSEGMENT>::CIterator RegisterSegmentIterator = m_RegisterSegmentPool.GetBeginIterator();
			LPREGISTERSEGMENT pRegisterSegment;
			REGISTERSEGMENT RegisterSegment;
			for(UINT i = 0; i < uCount; ++ i)
			{
				uRegisterEnd = uRegisterBegin + pVariable->Type;

				RegisterSegmentIterator = m_RegisterSegmentPool.GetBeginIterator();
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
			}

			*puCount = 0;

			m_Parent.m_VariablePool.Remove(uIndex, 1);

			return true;
		}
	}

	ALCHEMY_DEBUG_WARNING("CEffectCode::CBasicFunctionDefiner::FreeVariable: Error handle.");

	return false;
}

UINT CEffectCode::CBasicFunctionDefiner::EndDefine()
{
	if(!m_pFunction)
		_ApplyFunction();

	m_pFunction->uCode               = m_Parent.m_CodePool.Add(m_Code) + 1;
	m_pFunction->uUserVariables      = __ApplyUserVariables();
	m_pFunction->uParametersVariable = m_uParametersVariable;
	m_pFunction->uResultVariable     = m_uResultVariable;
	//m_pFunction->uShader             = 0;

	m_Code = ALCHEMY_NULL;

	m_UserVariableDeclarationMap.Clear();

	m_uUserVariables      = 0;
	m_uParametersVariable = 0;
	m_uResultVariable     = 0;
	m_pFunction           = ALCHEMY_NULL;

	m_uTemporaryRegisterCount = 0;

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
		uRegister = m_uTemporaryRegisterCount;

		UINT uCurrentCount = Type, uByteAlignment = 1 << m_Parent.m_Compiler.GetByteAlignmentShift(), uByteAlignmentMask = uByteAlignment - 1;
		if(uCount == 1 && uCurrentCount <= uByteAlignment)
		{
			CPool<REGISTERSEGMENT>::CIterator Iterator = m_RegisterSegmentPool.GetBeginIterator();
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
				Iterator = m_RegisterSegmentPool.GetBeginIterator();
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
		}
	}

	VARIABLE Variable;
	Variable.Type         = Type;
	Variable.uRegister    = uRegister;
	Variable.uDeclaration = uDeclaration;
	Variable.uFunction    = m_uHandle;
	Variable.uGlobal      = 0;
	Variable.uParent      = 0;

	UINT uHandle = m_Parent.m_VariablePool.Add(Variable) + 1;

	m_VariableCountMap.Insert(uHandle, uCount);

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
	Function.uUserVariables      = 0;

	UINT uIndex      = m_Parent.m_FunctionPool.Add(Function);

	m_pFunction      = m_Parent.m_FunctionPool.Get(uIndex);

	m_uHandle        = uIndex + 1;
}

UINT CEffectCode::CBasicFunctionDefiner::__ApplyUserVariables()
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
}

CEffectCode::CShaderFunctionDefiner::CShaderFunctionDefiner(CEffectCode& Parent, bool bIsVertexShader) :
CBasicFunctionDefiner(Parent)
//m_pShaderParameter(ALCHEMY_NULL)
{
	m_Type = bIsVertexShader ? CEffectInterface::VERTEX_SHADER : CEffectInterface::FRAGMENT_SHADER;
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
		SHADERFUNCTION ShaderFunction;

		ShaderFunction.Type = m_Type;

		pFunction->uParametersVariable  = _ApplyVariable(uParameters, false);
	}

	return m_uParametersVariable = pFunction->uParametersVariable;
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
		SHADERFUNCTION ShaderFunction;

		ShaderFunction.Type = m_Type;

		pFunction->uResultVariable  = _ApplyVariable(uParameters, false);
	}

	return m_uResultVariable = pFunction->uResultVariable;
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
	else if(m_Type == CEffectInterface::VERTEX_SHADER)
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

bool CEffectCode::CPassDefiner::AddInfo(CEffectInfoInterface& Info, CEffectInfoBlendMethodInterface* pBlendMethod)
{
	EFFECTINFO EffectInfo;

	EffectInfo.pEffectInfo  = &Info;
	EffectInfo.pBlendMethod = pBlendMethod;

	UINT i, j, uAttributeCount = 0, uVaryingCount = 0, uConstantCount = 0, uTemporaryCount = 0, uAttributeVariableCount = 0, uConstantVariableCount = 0, uCurrentCount = 0, uDestinationCount = 0, uEffectInfoIndex = m_EffectInfos.Add(EffectInfo);

	const UINT* puValue;

	VARIABLETYPE VariableType;

	LPSHADERVARIABLE pVariable;

	SHADERPARAMETERKEY Parameter;

	ELEMENTPARAMETER ParameterTemp;

	GLOBALVARIABLEKEY GlobalVariableKey;

	GLOBALVARIABLE GlobalVariable;

	SHADERVARIABLE Variable;

	ATTRIBUTEVARIABLEKEY VariableKey;

	const ELEMENTPARAMETER* pElementParameter, * pAttributes = Info.GetAttributes(uAttributeCount), * pVarying = Info.GetVarying(uVaryingCount);

	const GLOBALVARIABLE* pConstant, * pConstants = Info.GetConstants(uConstantCount);

	const TEMPORARYDECLARATION* pTemporary = Info.GetTemporary(uTemporaryCount);

	const ATTRIBUTEVARIABLEDECLARATION* pAttributeVariableDeclaration = Info.GetAttributeVariables(uAttributeVariableCount);

	const CONSTANTVARIABLEDECLARATION* pConstantVariableDeclaration = Info.GetConstantVariables(uConstantVariableCount);

	for(i = 0; i < uAttributeCount; ++ i)
	{
		pElementParameter = &pAttributes[i];

		Parameter.Type    = pElementParameter->Type;
		Parameter.Usage   = pElementParameter->Usage;
		uDestinationCount = pElementParameter->uIndex + 1;

		puValue = m_AttributesDefiner.GetShaderParameterKeyCount(Parameter);
		uCurrentCount = 0;
		if(!puValue || !(uCurrentCount = *puValue) || uCurrentCount < uDestinationCount)
		{
			for(j = uCurrentCount; j < uDestinationCount; ++ j)
			{
				ParameterTemp.Type   = Parameter.Type;
				ParameterTemp.Usage  = Parameter.Usage;
				ParameterTemp.uIndex = j;

				m_AttributeAddressMap.Insert(ParameterTemp, m_uAttributeAddress ++);

				m_AttributesDefiner.AppendParameter(Parameter.Type, Parameter.Usage);
			}
		}
	}

	for(i = 0; i < uVaryingCount; ++ i)
	{
		pElementParameter = &pVarying[i];

		Parameter.Type    = pElementParameter->Type;
		Parameter.Usage   = pElementParameter->Usage;
		uDestinationCount = pElementParameter->uIndex + 1;

		puValue = m_VaryingDefiner.GetShaderParameterKeyCount(Parameter);
		uCurrentCount = 0;
		if(!puValue || !(uCurrentCount = *puValue) || uCurrentCount < uDestinationCount)
		{
			for(j = uCurrentCount; j < uDestinationCount; ++ j)
			{
				ParameterTemp.Type   = Parameter.Type;
				ParameterTemp.Usage  = Parameter.Usage;
				ParameterTemp.uIndex = j;

				m_VaryingAddressMap.Insert(ParameterTemp, m_uVaryingAddress ++);

				m_VaryingDefiner.AppendParameter(Parameter.Type, Parameter.Usage);
			}
		}
	}

	for(i = 0; i < uConstantCount; ++ i)
	{
		pConstant = &pConstants[i];

		GlobalVariableKey.ShaderType = pConstant->ShaderType;
		GlobalVariableKey.GlobalType = pConstant->GlobalType;
		GlobalVariableKey.uDetail    = pConstant->uDetail;
		GlobalVariableKey.uCount     = pConstant->uCount;
		GlobalVariableKey.pValue     = pConstant->pValue;
		uDestinationCount            = pConstant->uIndex + 1;

		puValue = m_Parent.m_GlobalVariableCountMap.Get(GlobalVariableKey);
		uCurrentCount = 0;
		if(!puValue || !(uCurrentCount = *puValue) || uCurrentCount < uDestinationCount)
		{
			for(j = uCurrentCount; j < uDestinationCount; ++ j)
			{
				//ConstantTemp.Type    = GlobalVariableKey.Type;
				//ConstantTemp.uDetail = GlobalVariableKey.uDetail;
				//ConstantTemp.uIndex  = j;

				//m_ConstantMap.Insert( ConstantTemp, m_Parent.AddGlobalVariable(GlobalVariableKey) );
				m_Parent.AddGlobalVariable(GlobalVariableKey);
			}
		}
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
				m_TemporaryHandleMap.Insert( Temporary, m_VertexShaderFunctionDefiner.ApplyVariable(Temporary.VariableType/*, Temporary.uDeclaration, 1) );
			else if(Temporary.ShaderType == CEffectInterface::FRAGMENT_SHADER)
				m_TemporaryHandleMap.Insert( Temporary, m_FragmentShaderFunctionDefiner.ApplyVariable(Temporary.VariableType/*, Temporary.uDeclaration, 1) );

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

		if(!pVariable && VariableKey.ShaderType == CEffectInterface::FRAGMENT_SHADER)
		{
			VariableKey.ShaderType = CEffectInterface::VERTEX_SHADER;

			pVariable = m_AttributeVariableMap.Get(VariableKey);

			if(pVariable)
			{
				if(pAttributeVariableDeclaration->bIsReadOnly)
				{
					ParameterTemp.Type   = VariableKey.ElementType;
					ParameterTemp.Usage  = VariableKey.ElementUsage;
					ParameterTemp.uIndex = VariableKey.uIndex;

					puValue = m_AttributeVaryingMap.Get(ParameterTemp);

					if(!puValue)
					{
						m_AttributeVaryingMap.Insert(ParameterTemp, 0);

						Parameter.Type    = VariableKey.ElementType;
						Parameter.Usage   = VariableKey.ElementUsage;
						uDestinationCount = VariableKey.uIndex + 1;
						puValue = m_VaryingDefiner.GetShaderParameterKeyCount(Parameter);
						uCurrentCount = 0;
						if(!puValue || !(uCurrentCount = *puValue) || uCurrentCount < uDestinationCount)
						{
							for(j = uCurrentCount; j < uDestinationCount; ++ j)
							{
								ParameterTemp.Type   = Parameter.Type;
								ParameterTemp.Usage  = Parameter.Usage;
								ParameterTemp.uIndex = j;

								m_VaryingAddressMap.Insert(ParameterTemp, m_uVaryingAddress ++);

								m_VaryingDefiner.AppendParameter(Parameter.Type, Parameter.Usage);
							}
						}
					}
				}
			}
			else
				VariableKey.ShaderType = CEffectInterface::FRAGMENT_SHADER;
		}

		if(pVariable)
		{
			if(pAttributeVariableDeclaration->bIsReadOnly)
			{
				if(pVariable->bIsUsed && !pVariable->uSource)
				{
					if(VariableKey.ShaderType == CEffectInterface::VERTEX_SHADER)
					{
						Parameter.Type    = VariableKey.ElementType;
						Parameter.Usage   = VariableKey.ElementUsage;
						uDestinationCount = VariableKey.uIndex + 1;

						puValue = m_AttributesDefiner.GetShaderParameterKeyCount(Parameter);
						uCurrentCount = 0;
						if(!puValue || !(uCurrentCount = *puValue) || uCurrentCount < uDestinationCount)
						{
							for(j = uCurrentCount; j < uDestinationCount; ++ j)
							{
								ParameterTemp.Type   = Parameter.Type;
								ParameterTemp.Usage  = Parameter.Usage;
								ParameterTemp.uIndex = j;

								m_AttributeAddressMap.Insert(ParameterTemp, m_uAttributeAddress ++);

								m_AttributesDefiner.AppendParameter(Parameter.Type, Parameter.Usage);
							}
						}
					}
					else if(VariableKey.ShaderType == CEffectInterface::FRAGMENT_SHADER)
					{
						Parameter.Type    = VariableKey.ElementType;
						Parameter.Usage   = VariableKey.ElementUsage;
						uDestinationCount = VariableKey.uIndex + 1;

						puValue = m_VaryingDefiner.GetShaderParameterKeyCount(Parameter);
						uCurrentCount = 0;
						if(!puValue || !(uCurrentCount = *puValue) || uCurrentCount < uDestinationCount)
						{
							for(j = uCurrentCount; j < uDestinationCount; ++ j)
							{
								ParameterTemp.Type   = Parameter.Type;
								ParameterTemp.Usage  = Parameter.Usage;
								ParameterTemp.uIndex = j;

								m_VaryingAddressMap.Insert(ParameterTemp, m_uVaryingAddress ++);

								m_VaryingDefiner.AppendParameter(Parameter.Type, Parameter.Usage);
							}
						}

						puValue = m_AttributesDefiner.GetShaderParameterKeyCount(Parameter);
						uCurrentCount = 0;
						if(!puValue || !(uCurrentCount = *puValue) || uCurrentCount < uDestinationCount)
						{
							for(j = uCurrentCount; j < uDestinationCount; ++ j)
							{
								ParameterTemp.Type   = Parameter.Type;
								ParameterTemp.Usage  = Parameter.Usage;
								ParameterTemp.uIndex = j;

								m_AttributeAddressMap.Insert(ParameterTemp, m_uAttributeAddress ++);

								m_AttributesDefiner.AppendParameter(Parameter.Type, Parameter.Usage);
							}
						}
					}

					pVariable->bIsUsed = false;
				}

				if(!pVariable->uHandle)
					pVariable->uHandle = 1;

				m_AttributeVariableMaxInfoMap.Insert(VariableKey, uEffectInfoIndex);
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
			if(VariableKey.ShaderType == CEffectInterface::VERTEX_SHADER)
			{
				Parameter.Type    = VariableKey.ElementType;
				Parameter.Usage   = VariableKey.ElementUsage;
				uDestinationCount = VariableKey.uIndex + 1;

				puValue = m_AttributesDefiner.GetShaderParameterKeyCount(Parameter);
				uCurrentCount = 0;
				if(!puValue || !(uCurrentCount = *puValue) || uCurrentCount < uDestinationCount)
				{
					for(j = uCurrentCount; j < uDestinationCount; ++ j)
					{
						ParameterTemp.Type   = Parameter.Type;
						ParameterTemp.Usage  = Parameter.Usage;
						ParameterTemp.uIndex = j;

						m_AttributeAddressMap.Insert(ParameterTemp, m_uAttributeAddress ++);

						m_AttributesDefiner.AppendParameter(Parameter.Type, Parameter.Usage);
					}
				}
			}
			else if(VariableKey.ShaderType == CEffectInterface::FRAGMENT_SHADER)
			{
				Parameter.Type    = VariableKey.ElementType;
				Parameter.Usage   = VariableKey.ElementUsage;
				uDestinationCount = VariableKey.uIndex + 1;

				puValue = m_VaryingDefiner.GetShaderParameterKeyCount(Parameter);
				uCurrentCount = 0;
				if(!puValue || !(uCurrentCount = *puValue) || uCurrentCount < uDestinationCount)
				{
					for(j = uCurrentCount; j < uDestinationCount; ++ j)
					{
						ParameterTemp.Type   = Parameter.Type;
						ParameterTemp.Usage  = Parameter.Usage;
						ParameterTemp.uIndex = j;

						m_VaryingAddressMap.Insert(ParameterTemp, m_uVaryingAddress ++);

						m_VaryingDefiner.AppendParameter(Parameter.Type, Parameter.Usage);
					}
				}

				puValue = m_AttributesDefiner.GetShaderParameterKeyCount(Parameter);
				uCurrentCount = 0;
				if(!puValue || !(uCurrentCount = *puValue) || uCurrentCount < uDestinationCount)
				{
					for(j = uCurrentCount; j < uDestinationCount; ++ j)
					{
						ParameterTemp.Type   = Parameter.Type;
						ParameterTemp.Usage  = Parameter.Usage;
						ParameterTemp.uIndex = j;

						m_AttributeAddressMap.Insert(ParameterTemp, m_uAttributeAddress ++);

						m_AttributesDefiner.AppendParameter(Parameter.Type, Parameter.Usage);
					}
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

		if(!pVariable && GlobalVariable.ShaderType == CEffectInterface::FRAGMENT_SHADER)
		{
			VariableType = CEffectCode::GetGlobalVariableType(GlobalVariable.GlobalType, GlobalVariable.uDetail);
			if( CEffectCode::IsElementVariable(VariableType) )
			{
				GlobalVariable.ShaderType = CEffectInterface::VERTEX_SHADER;

				pVariable = m_ConstantVariableMap.Get(GlobalVariable);

				if(pVariable)
				{
					if(pConstantVariableDeclaration->bIsReadOnly)
					{
						pElementParameter = m_ConstantVaryingMap.Get(GlobalVariable);

						if(!pElementParameter)
						{
							Parameter.Type    = static_cast<CVertexDeclaration::ELEMENTTYPE>(VariableType);
							Parameter.Usage   = CVertexDeclaration::UNKNOWN;

							puValue = m_VaryingDefiner.GetShaderParameterKeyCount(Parameter);

							ParameterTemp.Type   = Parameter.Type;
							ParameterTemp.Usage  = Parameter.Usage;
							ParameterTemp.uIndex = puValue ? *puValue : 0;

							m_VaryingAddressMap.Insert(ParameterTemp, m_uVaryingAddress ++);

							m_VaryingDefiner.AppendParameter(Parameter.Type, Parameter.Usage);

							m_ConstantVaryingMap.Insert(GlobalVariable, ParameterTemp);
						}
					}
				}
				else
					GlobalVariable.ShaderType = CEffectInterface::FRAGMENT_SHADER;
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
					GlobalVariableKey.ShaderType = GlobalVariable.ShaderType;
					GlobalVariableKey.GlobalType = GlobalVariable.GlobalType;
					GlobalVariableKey.uDetail    = GlobalVariable.uDetail;
					GlobalVariableKey.uCount     = GlobalVariable.uCount;
					GlobalVariableKey.pValue     = GlobalVariable.pValue;
					uDestinationCount            = GlobalVariable.uIndex + 1;

					puValue = m_Parent.m_GlobalVariableCountMap.Get(GlobalVariableKey);
					uCurrentCount = 0;
					if(!puValue || !(uCurrentCount = *puValue) || uCurrentCount < uDestinationCount)
						for(j = uCurrentCount; j < uDestinationCount; ++ j)
							m_Parent.AddGlobalVariable(GlobalVariableKey);

					pVariable->uSource = m_Parent.m_GlobalVariableHandleMap[GlobalVariable];

					pVariable->bIsUsed = false;
				}

				if(!pVariable->uHandle)
					pVariable->uHandle = 1;

				m_ConstantVariableMaxInfoMap.Insert(GlobalVariable, uEffectInfoIndex);
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
				m_TemporaryHandleMap.Insert( Temporary, m_FragmentShaderFunctionDefiner.ApplyVariable(Temporary.VariableType/*, Temporary.uDeclaration, 1) );
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
				Parameter.Type    = pBlendAttribute->Type;
				Parameter.Usage   = pBlendAttribute->Usage;
				uDestinationCount = pBlendAttribute->uIndex + 1;

				puValue = m_AttributesDefiner.GetShaderParameterKeyCount(Parameter);
				uCurrentCount = 0;
				if(!puValue || !(uCurrentCount = *puValue) || uCurrentCount < uDestinationCount)
				{
					for(j = uCurrentCount; j < uDestinationCount; ++ j)
					{
						ParameterTemp.Type   = Parameter.Type;
						ParameterTemp.Usage  = Parameter.Usage;
						ParameterTemp.uIndex = j;

						m_AttributeAddressMap.Insert(ParameterTemp, m_uAttributeAddress ++);

						m_AttributesDefiner.AppendParameter(Parameter.Type, Parameter.Usage);
					}
				}
			}
			else
			{
				VariableKey.ShaderType   = CEffectInterface::FRAGMENT_SHADER;
				VariableKey.ElementType  = pBlendAttribute->Type;
				VariableKey.ElementUsage = pBlendAttribute->Usage;
				VariableKey.uIndex       = pBlendAttribute->uIndex;

				pVariable = m_AttributeVariableMap.Get(VariableKey);
				if(!pVariable)
				{
					VariableKey.ShaderType = CEffectInterface::VERTEX_SHADER;
					pVariable = m_AttributeVariableMap.Get(VariableKey);
					if(!pVariable)
						VariableKey.ShaderType = CEffectInterface::FRAGMENT_SHADER;
				}

				if(pVariable)
				{
					Parameter.Type    = VariableKey.ElementType;
					Parameter.Usage   = VariableKey.ElementUsage;
					uDestinationCount = VariableKey.uIndex + 1;

					if(VariableKey.ShaderType == CEffectInterface::VERTEX_SHADER)
					{
						ParameterTemp.Type   = VariableKey.ElementType;
						ParameterTemp.Usage  = VariableKey.ElementUsage;
						ParameterTemp.uIndex = VariableKey.uIndex;

						puValue = m_AttributeVaryingMap.Get(ParameterTemp);

						if(!puValue)
						{
							m_AttributeVaryingMap.Insert(ParameterTemp, 0);

							puValue = m_VaryingDefiner.GetShaderParameterKeyCount(Parameter);
							uCurrentCount = 0;
							if(!puValue || !(uCurrentCount = *puValue) || uCurrentCount < uDestinationCount)
							{
								for(j = uCurrentCount; j < uDestinationCount; ++ j)
								{
									ParameterTemp.Type   = Parameter.Type;
									ParameterTemp.Usage  = Parameter.Usage;
									ParameterTemp.uIndex = j;

									m_VaryingAddressMap.Insert(ParameterTemp, m_uVaryingAddress ++);

									m_VaryingDefiner.AppendParameter(Parameter.Type, Parameter.Usage);
								}
							}
						}
					}

					if(pVariable->bIsUsed && !pVariable->uSource)
					{
						puValue = m_AttributesDefiner.GetShaderParameterKeyCount(Parameter);
						uCurrentCount = 0;
						if(!puValue || !(uCurrentCount = *puValue) || uCurrentCount < uDestinationCount)
						{
							for(j = uCurrentCount; j < uDestinationCount; ++ j)
							{
								ParameterTemp.Type   = Parameter.Type;
								ParameterTemp.Usage  = Parameter.Usage;
								ParameterTemp.uIndex = j;

								m_AttributeAddressMap.Insert(ParameterTemp, m_uAttributeAddress ++);

								m_AttributesDefiner.AppendParameter(Parameter.Type, Parameter.Usage);
							}
						}

						pVariable->bIsUsed = false;
					}

					if(!pVariable->uHandle)
						pVariable->uHandle = 1;

					m_AttributeVariableMaxInfoMap.Insert(VariableKey, uEffectInfoIndex);
				}
				else
				{
					Parameter.Type    = VariableKey.ElementType;
					Parameter.Usage   = VariableKey.ElementUsage;
					uDestinationCount = VariableKey.uIndex + 1;

					puValue = m_VaryingDefiner.GetShaderParameterKeyCount(Parameter);
					uCurrentCount = 0;
					if(!puValue || !(uCurrentCount = *puValue) || uCurrentCount < uDestinationCount)
					{
						for(j = uCurrentCount; j < uDestinationCount; ++ j)
						{
							ParameterTemp.Type   = Parameter.Type;
							ParameterTemp.Usage  = Parameter.Usage;
							ParameterTemp.uIndex = j;

							m_VaryingAddressMap.Insert(ParameterTemp, m_uVaryingAddress ++);

							m_VaryingDefiner.AppendParameter(Parameter.Type, Parameter.Usage);
						}
					}

					puValue = m_AttributesDefiner.GetShaderParameterKeyCount(Parameter);
					uCurrentCount = 0;
					if(!puValue || !(uCurrentCount = *puValue) || uCurrentCount < uDestinationCount)
					{
						for(j = uCurrentCount; j < uDestinationCount; ++ j)
						{
							ParameterTemp.Type   = Parameter.Type;
							ParameterTemp.Usage  = Parameter.Usage;
							ParameterTemp.uIndex = j;

							m_AttributeAddressMap.Insert(ParameterTemp, m_uAttributeAddress ++);

							m_AttributesDefiner.AppendParameter(Parameter.Type, Parameter.Usage);
						}
					}
				}
			}

			++ pBlendAttribute;
		}

		const CONSTANT* pBlendConstant = pBlendMethod->GetConstants(uConstantCount);
		for(i = 0; i < uConstantCount; ++ i)
		{
			if(pBlendConstant->bIsForce)
			{
				GlobalVariableKey.ShaderType = CEffectInterface::FRAGMENT_SHADER;
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
				}
			}
			else
			{
				GlobalVariable.ShaderType = CEffectInterface::FRAGMENT_SHADER;
				GlobalVariable.GlobalType = pBlendConstant->Type;
				GlobalVariable.uDetail    = pBlendConstant->uDetail;
				GlobalVariable.uIndex     = pBlendConstant->uIndex;
				GlobalVariable.uCount     = pBlendConstant->uCount;
				GlobalVariable.pValue     = pBlendConstant->pValue;

				pVariable = m_ConstantVariableMap.Get(GlobalVariable);

				if(!pVariable)
				{
					VariableType = CEffectCode::GetGlobalVariableType(GlobalVariable.GlobalType, GlobalVariable.uDetail);
					if( CEffectCode::IsElementVariable(VariableType) )
					{
						GlobalVariable.ShaderType = CEffectInterface::VERTEX_SHADER;

						pVariable = m_ConstantVariableMap.Get(GlobalVariable);

						if(pVariable)
						{
							pElementParameter = m_ConstantVaryingMap.Get(GlobalVariable);

							if(!pElementParameter)
							{
								Parameter.Type    = static_cast<CVertexDeclaration::ELEMENTTYPE>(VariableType);
								Parameter.Usage   = CVertexDeclaration::UNKNOWN;

								puValue = m_VaryingDefiner.GetShaderParameterKeyCount(Parameter);

								ParameterTemp.Type   = Parameter.Type;
								ParameterTemp.Usage  = Parameter.Usage;
								ParameterTemp.uIndex = puValue ? *puValue : 0;

								m_VaryingAddressMap.Insert(ParameterTemp, m_uVaryingAddress ++);

								m_VaryingDefiner.AppendParameter(Parameter.Type, Parameter.Usage);

								m_ConstantVaryingMap.Insert(GlobalVariable, ParameterTemp);
							}
						}
						else
							GlobalVariable.ShaderType = CEffectInterface::FRAGMENT_SHADER;
					}
				}

				if(pVariable)
				{
					if(pVariable->bIsUsed && !pVariable->uSource)
					{
						GlobalVariableKey.ShaderType = GlobalVariable.ShaderType;
						GlobalVariableKey.GlobalType = GlobalVariable.GlobalType;
						GlobalVariableKey.uDetail    = GlobalVariable.uDetail;
						GlobalVariableKey.uCount     = GlobalVariable.uCount;
						GlobalVariableKey.pValue     = GlobalVariable.pValue;
						uDestinationCount            = GlobalVariable.uIndex + 1;

						puValue = m_Parent.m_GlobalVariableCountMap.Get(GlobalVariableKey);
						uCurrentCount = 0;
						if(!puValue || !(uCurrentCount = *puValue) || uCurrentCount < uDestinationCount)
							for(j = uCurrentCount; j < uDestinationCount; ++ j)
								m_Parent.AddGlobalVariable(GlobalVariableKey);

						pVariable->uSource = m_Parent.m_GlobalVariableHandleMap[GlobalVariable];

						pVariable->bIsUsed = false;
					}

					if(!pVariable->uHandle)
						pVariable->uHandle = 1;

					m_ConstantVariableMaxInfoMap.Insert(GlobalVariable, uEffectInfoIndex);
				}
			}

			++ pBlendConstant;
		}
	}

	m_bIsBlend = m_bIsBlend || ( m_EffectInfos.GetLength() > 1 && (pBlendMethod != ALCHEMY_NULL) );

	return true;
}

bool CEffectCode::CPassDefiner::AddInfo(CEffectInfoInterface& Info, BLENDMETHOD        BlendMethod)
{
	static CEffectInfoModulation s_Modulation;
	static CEffectInfoAddition s_Addition;

	static CEffectInfoBlendMethodInterface* s_pBlendMethod[TOTAL_BLEND_METHOD] = {
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
}

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

UINT CEffectCode::CPassDefiner::EndDefine()
{
	CShaderParametersDefiner OutputColorDefiner(m_Parent);

	OutputColorDefiner.AppendParameter(CVertexDeclaration::FLOAT4, CVertexDeclaration::COLOR);

	bool bIsBlend = false;

	UINT uVarying = m_VaryingDefiner.EndDefine(),
		uAttributes = m_VertexShaderFunctionDefiner.BeginFunction( m_AttributesDefiner.EndDefine() ), 
		uVertexShaderVarying = m_VertexShaderFunctionDefiner.EndFunction(uVarying), 
		uFragmentShaderVarying = m_FragmentShaderFunctionDefiner.BeginFunction(uVarying),
		uResultColor = m_Parent.GetAddressVariable(m_FragmentShaderFunctionDefiner.EndFunction( OutputColorDefiner.EndDefine() ), 0),
		uDestinationColor = uResultColor,
		uSourceColor1 = 0, 
		uSourceColor2 = 0,
		uSamplerCount = 0, uAttributeCount = 0, uVaryingCount = 0, uConstantCount = 0, uTemporaryCount = 0, uAttributeVariableCount = 0, uConstantVariableCount  = 0, uVariableCount = 0, uTotal, uHandle, uAddress, uDestinationCount, uCurrentCount, i, j;

	PUINT puValue, puSamplers, puAttributes, puConstants, puTemporary;

	LPVARYING pVaryingTemp;

	LPEFFECTINFO pInfo;

	LPSHADERVARIABLE pVariable;

	LPSHADERVARIABLE pAttributeVariables, pConstantVariables;

	GLOBALVARIABLEKEY GlobalVariableKey;

	GLOBALVARIABLE GlobalVariable;

	VARYING Varying;

	//TEMPORARYDECLARATION TemporaryKey;

	TEMPORARYDECLARATION TemporaryDecalaration;

	ATTRIBUTEVARIABLEKEY VariableKey;

	SHADERVARIABLE Variable;

	ELEMENTPARAMETER ParameterTemp;

	const VARIABLETYPE* pTemporaryType;

	const ATTRIBUTE* pBlendAttribute;

	const CONSTANT* pBlendConstant;

	const CEffectInfoInterface::SAMPLER* pSamplers, * pSampler;

	const ELEMENTPARAMETER* pAttributes, * pVarying, * pElementParameter;

	const GLOBALVARIABLE* pConstants;

	const TEMPORARYDECLARATION* pTemporary;

	const ATTRIBUTEVARIABLEDECLARATION* pAttributeVariableDeclaration;

	const CONSTANTVARIABLEDECLARATION* pConstantVariableDeclaration;

	if(m_bIsBlend)
	{
		/*Temporary.ShaderType   = CEffectInterface::FRAGMENT_SHADER;
		Temporary.VariableType = VECTOR4_VARIABLE;

		Temporary.uIndex       = 0;
		puValue = m_TemporaryHandleMap.Get(Temporary);
		if(!puValue)
			m_TemporaryHandleMap.Insert( Temporary, uDestinationColor = m_FragmentShaderFunctionDefiner.ApplyVariable(VECTOR4_VARIABLE) );
		else
			uDestinationColor = *puValue;*/
		TemporaryDecalaration.ShaderType = CEffectInterface::FRAGMENT_SHADER;
		TemporaryDecalaration.VariableType = VECTOR4_VARIABLE;
		uDestinationColor = __PopFreeTemporary(TemporaryDecalaration);
	}
	
	CVector<EFFECTINFO>::CIterator EffectInfoIterator = m_EffectInfos.GetBeginIterator();

	while( CVector<EFFECTINFO>::IsValuable(EffectInfoIterator) )
	{
		pInfo = &(*EffectInfoIterator);

		if(pInfo && pInfo->pEffectInfo)
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

				puSamplers[i] = m_Parent.AddSamplerVariable(pSampler->Declaration, pSampler->uIndex);
			}

			puAttributes = puSamplers + uSamplerCount;
			for(i = 0; i < uAttributeCount; ++ i)
			{
				pElementParameter = &pAttributes[i];

				puValue = m_AttributeMap.Get(*pElementParameter);

				if( !puValue || !(uHandle = *puValue) )
				{
					uHandle = m_Parent.GetAddressVariable(uAttributes, m_AttributeAddressMap[*pElementParameter]);
					m_AttributeMap.Insert(*pElementParameter, uHandle);
				}

				puAttributes[i] = uHandle;
			}

			for(i = 0; i < uVaryingCount; ++ i)
			{
				pElementParameter = &pVarying[i];

				pVaryingTemp = m_VaryingMap.Get(*pElementParameter);

				if(!pVaryingTemp)
				{
					uAddress = m_VaryingAddressMap[*pElementParameter];

					Varying.uVertexShaderHandle   = m_Parent.GetAddressVariable(uVertexShaderVarying, uAddress);
					Varying.uFragmentShaderHandle = m_Parent.GetAddressVariable(uFragmentShaderVarying, uAddress);
					Varying.IsUsed                = false;

					m_VaryingMap.Insert(*pElementParameter, Varying);
				}
				else
				{
					Varying.uVertexShaderHandle   = pVaryingTemp->uVertexShaderHandle;
					Varying.uFragmentShaderHandle = pVaryingTemp->uFragmentShaderHandle;
					Varying.IsUsed                = pVaryingTemp->IsUsed = true;
				}

				pVaryingTemp                        = &m_pVaryingBuffer[i];
				pVaryingTemp->uVertexShaderHandle   = Varying.uVertexShaderHandle;
				pVaryingTemp->uFragmentShaderHandle = Varying.uFragmentShaderHandle;
				pVaryingTemp->IsUsed                = Varying.IsUsed;
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

				if(!pVariable && VariableKey.ShaderType == CEffectInterface::FRAGMENT_SHADER)
				{
					VariableKey.ShaderType = CEffectInterface::VERTEX_SHADER;

					pVariable = m_AttributeVariableMap.Get(VariableKey);

					if(!pVariable)
						VariableKey.ShaderType = CEffectInterface::FRAGMENT_SHADER;
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
									ParameterTemp.Type   = pAttributeVariableDeclaration->ElementType;
									ParameterTemp.Usage  = pAttributeVariableDeclaration->ElementUsage;
									ParameterTemp.uIndex = pAttributeVariableDeclaration->uIndex;

									if(pAttributeVariableDeclaration->ShaderType == CEffectInterface::VERTEX_SHADER)
									{
										puValue = m_AttributeMap.Get(ParameterTemp);

										if( !puValue || !(uHandle = *puValue) )
										{
											uHandle = m_Parent.GetAddressVariable(uAttributes, m_AttributeAddressMap[ParameterTemp]);
											m_AttributeMap.Insert(ParameterTemp, uHandle);
										}

										pVariable->uSource = uHandle;
									}
									else if(pAttributeVariableDeclaration->ShaderType == CEffectInterface::FRAGMENT_SHADER)
									{
										pVaryingTemp = m_VaryingMap.Get(ParameterTemp);

										if(!pVaryingTemp)
										{
											uAddress = m_VaryingAddressMap[ParameterTemp];

											Varying.uVertexShaderHandle   = m_Parent.GetAddressVariable(uVertexShaderVarying, uAddress);
											Varying.uFragmentShaderHandle = m_Parent.GetAddressVariable(uFragmentShaderVarying, uAddress);
											Varying.IsUsed                = true;

											m_VaryingMap.Insert(ParameterTemp, Varying);

											puValue = m_AttributeMap.Get(ParameterTemp);

											if( !puValue || !(uHandle = *puValue) )
											{
												uHandle = m_Parent.GetAddressVariable(uAttributes, m_AttributeAddressMap[ParameterTemp]);
												m_AttributeMap.Insert(ParameterTemp, uHandle);
											}

											m_VertexShaderFunctionDefiner.AppendInstruction(CCompilerInterface::MOV, Varying.uVertexShaderHandle, ALCHEMY_NULL, 0, uHandle, ALCHEMY_NULL, 0, 0, ALCHEMY_NULL, 0);

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
							puValue = m_AttributeVariableMaxInfoMap.Get(VariableKey);
							if(puValue && EffectInfoIterator.GetIndex() == *puValue)
							{
								TemporaryDecalaration.ShaderType   = VariableKey.ShaderType;
								TemporaryDecalaration.VariableType = static_cast<VARIABLETYPE>(VariableKey.ElementType);
								TemporaryDecalaration.uCount       = 1;

								__PushFreeTemporary(TemporaryDecalaration, pVariable->uHandle, false);
							}

							if(VariableKey.ShaderType == CEffectInterface::VERTEX_SHADER && pAttributeVariableDeclaration->ShaderType != VariableKey.ShaderType)
							{
								ParameterTemp.Type   = pAttributeVariableDeclaration->ElementType;
								ParameterTemp.Usage  = pAttributeVariableDeclaration->ElementUsage;
								ParameterTemp.uIndex = pAttributeVariableDeclaration->uIndex;

								pVaryingTemp = m_VaryingMap.Get(ParameterTemp);

								if(pVaryingTemp)
								{
									uHandle = pVariable->uHandle;

									Varying.uVertexShaderHandle   = pVaryingTemp->uVertexShaderHandle;
									Varying.uFragmentShaderHandle = pVaryingTemp->uFragmentShaderHandle;
									Varying.IsUsed                = false;

									pVariable = m_VaryingVariableMap.Get(Varying);

									if(pVariable)
									{
										if(!pVaryingTemp->IsUsed)
										{
											m_VertexShaderFunctionDefiner.AppendInstruction(CCompilerInterface::MOV, pVaryingTemp->uVertexShaderHandle, ALCHEMY_NULL, 0, uHandle, ALCHEMY_NULL, 0, 0, ALCHEMY_NULL, 0);

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
									uAddress = m_VaryingAddressMap[ParameterTemp];

									Varying.uVertexShaderHandle   = m_Parent.GetAddressVariable(uVertexShaderVarying, uAddress);
									Varying.uFragmentShaderHandle = m_Parent.GetAddressVariable(uFragmentShaderVarying, uAddress);
									Varying.IsUsed                = true;

									m_VaryingMap.Insert(ParameterTemp, Varying);

									m_VertexShaderFunctionDefiner.AppendInstruction(CCompilerInterface::MOV, Varying.uVertexShaderHandle, ALCHEMY_NULL, 0, pVariable->uHandle, ALCHEMY_NULL, 0, 0, ALCHEMY_NULL, 0);

									pVaryingTemp = &Varying;

									Variable.bIsUsed = false;
									Variable.uSource = pVaryingTemp->uFragmentShaderHandle;
									Variable.uHandle = pVaryingTemp->uFragmentShaderHandle;

									pVariable = &Variable;
								}

								VariableKey.ShaderType = CEffectInterface::FRAGMENT_SHADER;

								m_AttributeVariableMap.Insert(VariableKey, *pVariable);

								puValue = m_AttributeVariableMaxInfoMap.Get(VariableKey);
								if(!puValue)
								{
									TemporaryDecalaration.ShaderType   = VariableKey.ShaderType;
									TemporaryDecalaration.VariableType = static_cast<VARIABLETYPE>(VariableKey.ElementType);
									TemporaryDecalaration.uCount       = 1;

									__PushFreeTemporary(TemporaryDecalaration, pVariable->uHandle, false);
								}
							}
						}
					}
					else if(pVariable->uHandle)
					{
						puValue = m_AttributeVariableMaxInfoMap.Get(VariableKey);
						if(!puValue || EffectInfoIterator.GetIndex() > *puValue)
							pVariable->uHandle = 0;
						else
						{
							if(pAttributeVariableDeclaration->bIsForce && !pVariable->uSource)
							{
								ParameterTemp.Type   = pAttributeVariableDeclaration->ElementType;
								ParameterTemp.Usage  = pAttributeVariableDeclaration->ElementUsage;
								ParameterTemp.uIndex = pAttributeVariableDeclaration->uIndex;

								if(pAttributeVariableDeclaration->ShaderType == CEffectInterface::VERTEX_SHADER)
								{
									puValue = m_AttributeMap.Get(ParameterTemp);

									if( !puValue || !(uHandle = *puValue) )
									{
										uHandle = m_Parent.GetAddressVariable(uAttributes, m_AttributeAddressMap[ParameterTemp]);
										m_AttributeMap.Insert(ParameterTemp, uHandle);
									}

									pVariable->uSource = uHandle;
								}
								else if(pAttributeVariableDeclaration->ShaderType == CEffectInterface::FRAGMENT_SHADER)
								{
									pVaryingTemp = m_VaryingMap.Get(ParameterTemp);

									if(!pVaryingTemp)
									{
										uAddress = m_VaryingAddressMap[ParameterTemp];

										Varying.uVertexShaderHandle   = m_Parent.GetAddressVariable(uVertexShaderVarying, uAddress);
										Varying.uFragmentShaderHandle = m_Parent.GetAddressVariable(uFragmentShaderVarying, uAddress);
										Varying.IsUsed                = true;

										m_VaryingMap.Insert(ParameterTemp, Varying);

										puValue = m_AttributeMap.Get(ParameterTemp);

										if( !puValue || !(uHandle = *puValue) )
										{
											uHandle = m_Parent.GetAddressVariable(uAttributes, m_AttributeAddressMap[ParameterTemp]);
											m_AttributeMap.Insert(ParameterTemp, uHandle);
										}

										m_VertexShaderFunctionDefiner.AppendInstruction(CCompilerInterface::MOV, Varying.uVertexShaderHandle, ALCHEMY_NULL, 0, uHandle, ALCHEMY_NULL, 0, 0, ALCHEMY_NULL, 0);

										pVaryingTemp = &Varying;
									}

									pVariable->uSource = pVaryingTemp->uFragmentShaderHandle;
								}
							}

							if(!pVariable->bIsUsed)
							{
								TemporaryDecalaration.ShaderType   = VariableKey.ShaderType;
								TemporaryDecalaration.VariableType = static_cast<VARIABLETYPE>(VariableKey.ElementType);

								pVariable->uHandle = __PopFreeTemporary(TemporaryDecalaration);
							}
							else if(VariableKey.ShaderType == CEffectInterface::VERTEX_SHADER && VariableKey.ShaderType != pAttributeVariableDeclaration->ShaderType)
							{
								ParameterTemp.Type   = pAttributeVariableDeclaration->ElementType;
								ParameterTemp.Usage  = pAttributeVariableDeclaration->ElementUsage;
								ParameterTemp.uIndex = pAttributeVariableDeclaration->uIndex;

								puValue = m_AttributeVaryingMap.Get(ParameterTemp);
								if(puValue)
								{
									pVaryingTemp = m_VaryingMap.Get(ParameterTemp);

									if(!pVaryingTemp)
									{
										uAddress = m_VaryingAddressMap[ParameterTemp];

										Varying.uVertexShaderHandle   = m_Parent.GetAddressVariable(uVertexShaderVarying, uAddress);
										Varying.uFragmentShaderHandle = m_Parent.GetAddressVariable(uFragmentShaderVarying, uAddress);
										Varying.IsUsed                = false;

										m_VaryingMap.Insert(ParameterTemp, Varying);

										pVaryingTemp = &Varying;
									}

									TemporaryDecalaration.ShaderType   = CEffectInterface::FRAGMENT_SHADER;
									TemporaryDecalaration.VariableType = static_cast<VARIABLETYPE>(ParameterTemp.Type);

									Variable.bIsUsed = false;
									Variable.uSource = pVaryingTemp->uFragmentShaderHandle;
									Variable.uHandle = __PopFreeTemporary(TemporaryDecalaration);

									m_VaryingVariableMap.Insert(*pVaryingTemp, Variable);

									pVariable = m_VaryingVariableMap.Get(*pVaryingTemp);
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
						ParameterTemp.Type   = pAttributeVariableDeclaration->ElementType;
						ParameterTemp.Usage  = pAttributeVariableDeclaration->ElementUsage;
						ParameterTemp.uIndex = pAttributeVariableDeclaration->uIndex;

						if(pAttributeVariableDeclaration->ShaderType == CEffectInterface::VERTEX_SHADER)
						{
							puValue = m_AttributeMap.Get(ParameterTemp);

							if( !puValue || !(uHandle = *puValue) )
							{
								uHandle = m_Parent.GetAddressVariable(uAttributes, m_AttributeAddressMap[ParameterTemp]);
								m_AttributeMap.Insert(ParameterTemp, uHandle);
							}

							pVariable->uSource = uHandle;
						}
						else if(pAttributeVariableDeclaration->ShaderType == CEffectInterface::FRAGMENT_SHADER)
						{
							pVaryingTemp = m_VaryingMap.Get(ParameterTemp);

							if(!pVaryingTemp)
							{
								uAddress = m_VaryingAddressMap[ParameterTemp];

								Varying.uVertexShaderHandle   = m_Parent.GetAddressVariable(uVertexShaderVarying, uAddress);
								Varying.uFragmentShaderHandle = m_Parent.GetAddressVariable(uFragmentShaderVarying, uAddress);
								Varying.IsUsed                = true;

								m_VaryingMap.Insert(ParameterTemp, Varying);

								puValue = m_AttributeMap.Get(ParameterTemp);

								if( !puValue || !(uHandle = *puValue) )
								{
									uHandle = m_Parent.GetAddressVariable(uAttributes, m_AttributeAddressMap[ParameterTemp]);
									m_AttributeMap.Insert(ParameterTemp, uHandle);
								}

								m_VertexShaderFunctionDefiner.AppendInstruction(CCompilerInterface::MOV, Varying.uVertexShaderHandle, ALCHEMY_NULL, 0, uHandle, ALCHEMY_NULL, 0, 0, ALCHEMY_NULL, 0);

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

				if(!pVariable && GlobalVariable.ShaderType == CEffectInterface::FRAGMENT_SHADER)
				{
					GlobalVariable.ShaderType = CEffectInterface::VERTEX_SHADER;

					pVariable = m_ConstantVariableMap.Get(GlobalVariable);

					if(pVariable)
						pElementParameter = m_ConstantVaryingMap.Get(GlobalVariable);
					else
						pElementParameter = ALCHEMY_NULL;

					if(!pElementParameter)
					{
						GlobalVariable.ShaderType = CEffectInterface::FRAGMENT_SHADER;

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
									GlobalVariableKey.ShaderType = GlobalVariable.ShaderType;
									GlobalVariableKey.GlobalType = GlobalVariable.GlobalType;
									GlobalVariableKey.uDetail    = GlobalVariable.uDetail;
									GlobalVariableKey.uCount     = GlobalVariable.uCount;
									GlobalVariableKey.pValue     = GlobalVariable.pValue;
									uDestinationCount            = GlobalVariable.uIndex + 1;

									puValue = m_Parent.m_GlobalVariableCountMap.Get(GlobalVariableKey);
									uCurrentCount = 0;
									if(!puValue || !(uCurrentCount = *puValue) || uCurrentCount < uDestinationCount)
										for(j = uCurrentCount; j < uDestinationCount; ++ j)
											m_Parent.AddGlobalVariable(GlobalVariableKey);

									pVariable->uSource = m_Parent.m_GlobalVariableHandleMap[GlobalVariable];
								}

								pVariable->uHandle = pVariable->uSource;
							}
						}
						else
						{
							puValue = m_ConstantVariableMaxInfoMap.Get(GlobalVariable);
							if(puValue && EffectInfoIterator.GetIndex() == *puValue)
							{
								TemporaryDecalaration.ShaderType   = GlobalVariable.ShaderType;
								TemporaryDecalaration.VariableType = CEffectCode::GetGlobalVariableType(GlobalVariable.GlobalType, GlobalVariable.uDetail);
								TemporaryDecalaration.uCount       = GlobalVariable.uCount;

								__PushFreeTemporary(TemporaryDecalaration, pVariable->uHandle, false);
							}

							if(GlobalVariable.ShaderType == CEffectInterface::VERTEX_SHADER && GlobalVariable.ShaderType != pConstantVariableDeclaration->ShaderType)
							{
								pVaryingTemp = m_VaryingMap.Get(*pElementParameter);

								if(pVaryingTemp)
								{
									uHandle = pVariable->uHandle;

									Varying.uVertexShaderHandle   = pVaryingTemp->uVertexShaderHandle;
									Varying.uFragmentShaderHandle = pVaryingTemp->uFragmentShaderHandle;
									Varying.IsUsed                = false;

									pVariable = m_VaryingVariableMap.Get(Varying);

									if(pVariable)
									{
										if(!pVaryingTemp->IsUsed)
										{
											m_VertexShaderFunctionDefiner.AppendInstruction(CCompilerInterface::MOV, pVaryingTemp->uVertexShaderHandle, ALCHEMY_NULL, 0, uHandle, ALCHEMY_NULL, 0, 0, ALCHEMY_NULL, 0);

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
									uAddress = m_VaryingAddressMap[*pElementParameter];

									Varying.uVertexShaderHandle   = m_Parent.GetAddressVariable(uVertexShaderVarying, uAddress);
									Varying.uFragmentShaderHandle = m_Parent.GetAddressVariable(uFragmentShaderVarying, uAddress);
									Varying.IsUsed                = true;

									m_VaryingMap.Insert(*pElementParameter, Varying);

									m_VertexShaderFunctionDefiner.AppendInstruction(CCompilerInterface::MOV, Varying.uVertexShaderHandle, ALCHEMY_NULL, 0, pVariable->uHandle, ALCHEMY_NULL, 0, 0, ALCHEMY_NULL, 0);

									pVaryingTemp = &Varying;

									Variable.bIsUsed = false;
									Variable.uSource = pVaryingTemp->uFragmentShaderHandle;
									Variable.uHandle = pVaryingTemp->uFragmentShaderHandle;

									pVariable = &Variable;
								}

								GlobalVariable.ShaderType = CEffectInterface::FRAGMENT_SHADER;

								m_ConstantVariableMap.Insert(GlobalVariable, *pVariable);

								puValue = m_ConstantVariableMaxInfoMap.Get(GlobalVariable);
								if(!puValue)
								{
									TemporaryDecalaration.ShaderType   = GlobalVariable.ShaderType;
									TemporaryDecalaration.VariableType = CEffectCode::GetGlobalVariableType(GlobalVariable.GlobalType, GlobalVariable.uDetail);
									TemporaryDecalaration.uCount       = GlobalVariable.uCount;

									__PushFreeTemporary(TemporaryDecalaration, pVariable->uHandle, false);
								}
							}
						}
					}
					else if(pVariable->uHandle)
					{
						puValue = m_ConstantVariableMaxInfoMap.Get(GlobalVariable);
						if(!puValue || EffectInfoIterator.GetIndex() > *puValue)
							pVariable->uHandle = 0;
						else 
						{
							if(!pVariable->bIsUsed)
							{
								TemporaryDecalaration.ShaderType   = GlobalVariable.ShaderType;
								TemporaryDecalaration.VariableType = CEffectCode::GetGlobalVariableType(GlobalVariable.GlobalType, GlobalVariable.uDetail);

								pVariable->uHandle = __PopFreeTemporary(TemporaryDecalaration);
							}
							else if(GlobalVariable.ShaderType == CEffectInterface::VERTEX_SHADER && GlobalVariable.ShaderType != pConstantVariableDeclaration->ShaderType && pElementParameter)
							{
								pVaryingTemp = m_VaryingMap.Get(*pElementParameter);

								if(!pVaryingTemp)
								{
									uAddress = m_VaryingAddressMap[*pElementParameter];

									Varying.uVertexShaderHandle   = m_Parent.GetAddressVariable(uVertexShaderVarying, uAddress);
									Varying.uFragmentShaderHandle = m_Parent.GetAddressVariable(uFragmentShaderVarying, uAddress);
									Varying.IsUsed                = false;

									m_VaryingMap.Insert(*pElementParameter, Varying);

									pVaryingTemp = &Varying;
								}

								TemporaryDecalaration.ShaderType   = CEffectInterface::FRAGMENT_SHADER;
								TemporaryDecalaration.VariableType = static_cast<VARIABLETYPE>(pElementParameter->Type);

								Variable.bIsUsed = false;
								Variable.uSource = pVaryingTemp->uFragmentShaderHandle;
								Variable.uHandle = __PopFreeTemporary(TemporaryDecalaration);

								//GlobalVariable.ShaderType = CEffectInterface::FRAGMENT_SHADER;

								m_VaryingVariableMap.Insert(*pVaryingTemp, Variable);

								pVariable = m_VaryingVariableMap.Get(*pVaryingTemp);
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
						GlobalVariableKey.ShaderType = GlobalVariable.ShaderType;
						GlobalVariableKey.GlobalType = GlobalVariable.GlobalType;
						GlobalVariableKey.uDetail    = GlobalVariable.uDetail;
						GlobalVariableKey.uCount     = GlobalVariable.uCount;
						GlobalVariableKey.pValue     = GlobalVariable.pValue;
						uDestinationCount            = GlobalVariable.uIndex + 1;

						puValue = m_Parent.m_GlobalVariableCountMap.Get(GlobalVariableKey);
						uCurrentCount = 0;
						if(!puValue || !(uCurrentCount = *puValue) || uCurrentCount < uDestinationCount)
							for(j = uCurrentCount; j < uDestinationCount; ++ j)
								m_Parent.AddGlobalVariable(GlobalVariableKey);

						pVariable->uSource = m_Parent.m_GlobalVariableHandleMap[GlobalVariable];

						pVariable->uHandle = pVariable->uSource;

						m_ConstantVariableMap.Insert(GlobalVariable, *pVariable);
					}
				}

				++ pConstantVariableDeclaration;
			}

			if( bIsBlend && pInfo->pBlendMethod && ALCHEMY_TEST_BIT(pInfo->pEffectInfo->GetEffectInfoFlag(), CEffectInfoInterface::FRAGMENT_SHADER_OUTPUT) )
			{
				if(!uSourceColor1 || !uSourceColor2)
				{
					//Temporary.ShaderType   = CEffectInterface::FRAGMENT_SHADER;
					//Temporary.VariableType = VECTOR4_VARIABLE;

					if(!uSourceColor1)
					{
						//Temporary.uIndex = 0;//1;
						//puValue = m_TemporaryHandleMap.Get(Temporary);
						//if(!puValue)
						//	m_TemporaryHandleMap.Insert( Temporary, uSourceColor1 = m_FragmentShaderFunctionDefiner.ApplyVariable(VECTOR4_VARIABLE, 1) );
						//else
						//	uSourceColor1 = *puValue;
						TemporaryDecalaration.ShaderType = CEffectInterface::FRAGMENT_SHADER;
						TemporaryDecalaration.VariableType = VECTOR4_VARIABLE;
						uSourceColor1 = __PopFreeTemporary(TemporaryDecalaration);
						//__PushFreeTemporary(TemporaryDecalaration, uSourceColor1, true);
					}

					if( !uSourceColor2 && !m_EffectInfos.IsEnd(EffectInfoIterator) )
					{
						//Temporary.uIndex = 2;
						//if(!puValue)
						//	m_TemporaryHandleMap.Insert( Temporary, uSourceColor2 = m_FragmentShaderFunctionDefiner.ApplyVariable(VECTOR4_VARIABLE) );
						//else
						//	uSourceColor2 = *puValue;
						TemporaryDecalaration.ShaderType = CEffectInterface::FRAGMENT_SHADER;
						TemporaryDecalaration.VariableType = VECTOR4_VARIABLE;
						uSourceColor2 = __PopFreeTemporary(TemporaryDecalaration);
						//uSourceColor2 = m_FragmentShaderFunctionDefiner.ApplyVariable(VECTOR4_VARIABLE, 1);
					}
				}

				pInfo->pEffectInfo->ShaderDefineHandler(uSourceColor1, puSamplers, puAttributes, puConstants, m_pVaryingBuffer, puTemporary, pAttributeVariables, pConstantVariables, m_VertexShaderFunctionDefiner, m_FragmentShaderFunctionDefiner, m_EffectCodeUtils);

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
					TemporaryDecalaration.ShaderType   = CEffectInterface::FRAGMENT_SHADER;
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
						ParameterTemp.Type   = pBlendAttribute->Type;
						ParameterTemp.Usage  = pBlendAttribute->Usage;
						ParameterTemp.uIndex = pBlendAttribute->uIndex;

						puValue = m_AttributeMap.Get(ParameterTemp);
						if( !puValue || !(uHandle = *puValue) )
						{
							uHandle = m_Parent.GetAddressVariable(uAttributes, m_AttributeAddressMap[ParameterTemp]);
							m_AttributeMap.Insert(ParameterTemp, uHandle);
						}

						puAttributes[i] = uHandle;
					}
					else
					{
						VariableKey.ShaderType   = CEffectInterface::FRAGMENT_SHADER;
						VariableKey.ElementType  = pBlendAttribute->Type;
						VariableKey.ElementUsage = pBlendAttribute->Usage;
						VariableKey.uIndex       = pBlendAttribute->uIndex;

						pVariable = m_AttributeVariableMap.Get(VariableKey);

						if(!pVariable)
						{
							VariableKey.ShaderType = CEffectInterface::VERTEX_SHADER;

							pVariable = m_AttributeVariableMap.Get(VariableKey);

							if(!pVariable)
								VariableKey.ShaderType = CEffectInterface::FRAGMENT_SHADER;
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
								ParameterTemp.Type   = VariableKey.ElementType;
								ParameterTemp.Usage  = VariableKey.ElementUsage;
								ParameterTemp.uIndex = VariableKey.uIndex;

								pVaryingTemp = m_VaryingMap.Get(ParameterTemp);

								if(!pVaryingTemp)
								{
									uAddress = m_VaryingAddressMap[ParameterTemp];

									Varying.uVertexShaderHandle   = m_Parent.GetAddressVariable(uVertexShaderVarying, uAddress);
									Varying.uFragmentShaderHandle = m_Parent.GetAddressVariable(uFragmentShaderVarying, uAddress);
									Varying.IsUsed                = true;

									m_VaryingMap.Insert(ParameterTemp, Varying);

									puValue = m_AttributeMap.Get(ParameterTemp);

									if( !puValue || !(uHandle = *puValue) )
									{
										uHandle = m_Parent.GetAddressVariable(uAttributes, m_AttributeAddressMap[ParameterTemp]);
										m_AttributeMap.Insert(ParameterTemp, uHandle);
									}

									m_VertexShaderFunctionDefiner.AppendInstruction(CCompilerInterface::MOV, Varying.uVertexShaderHandle, ALCHEMY_NULL, 0, uHandle, ALCHEMY_NULL, 0, 0, ALCHEMY_NULL, 0);

									pVaryingTemp = &Varying;
								}

								pVariable->uSource = pVaryingTemp->uFragmentShaderHandle;
							}

							pVariable->uHandle = pVariable->uSource;
						}
						else
						{
							puValue = m_AttributeVariableMaxInfoMap.Get(VariableKey);
							if(puValue && EffectInfoIterator.GetIndex() == *puValue)
							{
								TemporaryDecalaration.ShaderType   = VariableKey.ShaderType;
								TemporaryDecalaration.VariableType = static_cast<VARIABLETYPE>(VariableKey.ElementType);
								TemporaryDecalaration.uCount       = 1;

								__PushFreeTemporary(TemporaryDecalaration, pVariable->uHandle, false);
							}

							if(VariableKey.ShaderType == CEffectInterface::VERTEX_SHADER)
							{
								ParameterTemp.Type   = VariableKey.ElementType;
								ParameterTemp.Usage  = VariableKey.ElementUsage;
								ParameterTemp.uIndex = VariableKey.uIndex;

								pVaryingTemp = m_VaryingMap.Get(ParameterTemp);

								if(!pVaryingTemp)
								{
									uAddress = m_VaryingAddressMap[ParameterTemp];

									Varying.uVertexShaderHandle   = m_Parent.GetAddressVariable(uVertexShaderVarying, uAddress);
									Varying.uFragmentShaderHandle = m_Parent.GetAddressVariable(uFragmentShaderVarying, uAddress);
									Varying.IsUsed                = true;

									m_VaryingMap.Insert(ParameterTemp, Varying);

									m_VertexShaderFunctionDefiner.AppendInstruction(CCompilerInterface::MOV, Varying.uVertexShaderHandle, ALCHEMY_NULL, 0, pVariable->uHandle, ALCHEMY_NULL, 0, 0, ALCHEMY_NULL, 0);

									pVaryingTemp = &Varying;
								}

								Variable.bIsUsed = false;
								Variable.uSource = pVaryingTemp->uFragmentShaderHandle;
								Variable.uHandle = pVaryingTemp->uFragmentShaderHandle;

								VariableKey.ShaderType = CEffectInterface::FRAGMENT_SHADER;

								m_AttributeVariableMap.Insert(VariableKey, Variable);

								pVariable = &Variable;
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
						GlobalVariable.ShaderType = CEffectInterface::FRAGMENT_SHADER;
						GlobalVariable.GlobalType = pBlendConstant->Type;
						GlobalVariable.uDetail    = pBlendConstant->uDetail;
						GlobalVariable.uCount     = pBlendConstant->uCount;
						GlobalVariable.uIndex     = pBlendConstant->uIndex;
						GlobalVariable.pValue     = pBlendConstant->pValue;

						puConstants[i] = m_Parent.m_GlobalVariableHandleMap[GlobalVariable];
					}
					else
					{
						GlobalVariable.ShaderType = CEffectInterface::FRAGMENT_SHADER;
						GlobalVariable.GlobalType = pBlendConstant->Type;
						GlobalVariable.uDetail    = pBlendConstant->uDetail;
						GlobalVariable.uIndex     = pBlendConstant->uIndex;
						GlobalVariable.uCount     = pBlendConstant->uCount;
						GlobalVariable.pValue     = pBlendConstant->pValue;

						pVariable = m_ConstantVariableMap.Get(GlobalVariable);

						if(!pVariable)
						{
							GlobalVariable.ShaderType = CEffectInterface::VERTEX_SHADER;

							pVariable = m_ConstantVariableMap.Get(GlobalVariable);

							if(pVariable)
								pElementParameter = m_ConstantVaryingMap.Get(GlobalVariable);
							else
								pElementParameter = ALCHEMY_NULL;

							if(!pElementParameter)
							{
								GlobalVariable.ShaderType = CEffectInterface::FRAGMENT_SHADER;
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
								GlobalVariableKey.ShaderType = GlobalVariable.ShaderType;
								GlobalVariableKey.GlobalType = GlobalVariable.GlobalType;
								GlobalVariableKey.uDetail    = GlobalVariable.uDetail;
								GlobalVariableKey.uCount     = GlobalVariable.uCount;
								GlobalVariableKey.pValue     = GlobalVariable.pValue;
								uDestinationCount            = GlobalVariable.uIndex + 1;

								puValue = m_Parent.m_GlobalVariableCountMap.Get(GlobalVariableKey);
								uCurrentCount = 0;
								if(!puValue || !(uCurrentCount = *puValue) || uCurrentCount < uDestinationCount)
									for(j = uCurrentCount; j < uDestinationCount; ++ j)
										m_Parent.AddGlobalVariable(GlobalVariableKey);

								pVariable->uSource = m_Parent.m_GlobalVariableHandleMap[GlobalVariable];
							}

							pVariable->uHandle = pVariable->uSource;
						}
						else
						{
							puValue = m_ConstantVariableMaxInfoMap.Get(GlobalVariable);
							if(puValue && EffectInfoIterator.GetIndex() == *puValue)
							{
								TemporaryDecalaration.ShaderType   = GlobalVariable.ShaderType;
								TemporaryDecalaration.VariableType = CEffectCode::GetGlobalVariableType(GlobalVariable.GlobalType, GlobalVariable.uDetail);
								TemporaryDecalaration.uCount       = 1;

								__PushFreeTemporary(TemporaryDecalaration, pVariable->uHandle, false);
							}

							if(GlobalVariable.ShaderType == CEffectInterface::VERTEX_SHADER)
							{
								pVaryingTemp = m_VaryingMap.Get(*pElementParameter);

								if(pVaryingTemp)
								{
									uHandle = pVariable->uHandle;

									Varying.uVertexShaderHandle   = pVaryingTemp->uVertexShaderHandle;
									Varying.uFragmentShaderHandle = pVaryingTemp->uFragmentShaderHandle;
									Varying.IsUsed                = false;

									pVariable = m_VaryingVariableMap.Get(Varying);

									if(pVariable)
									{
										if(!pVaryingTemp->IsUsed)
										{
											m_VertexShaderFunctionDefiner.AppendInstruction(CCompilerInterface::MOV, pVaryingTemp->uVertexShaderHandle, ALCHEMY_NULL, 0, uHandle, ALCHEMY_NULL, 0, 0, ALCHEMY_NULL, 0);

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
									uAddress = m_VaryingAddressMap[*pElementParameter];

									Varying.uVertexShaderHandle   = m_Parent.GetAddressVariable(uVertexShaderVarying, uAddress);
									Varying.uFragmentShaderHandle = m_Parent.GetAddressVariable(uFragmentShaderVarying, uAddress);
									Varying.IsUsed                = true;

									m_VaryingMap.Insert(*pElementParameter, Varying);

									m_VertexShaderFunctionDefiner.AppendInstruction(CCompilerInterface::MOV, Varying.uVertexShaderHandle, ALCHEMY_NULL, 0, pVariable->uHandle, ALCHEMY_NULL, 0, 0, ALCHEMY_NULL, 0);

									pVaryingTemp = &Varying;

									Variable.bIsUsed = false;
									Variable.uSource = pVaryingTemp->uFragmentShaderHandle;
									Variable.uHandle = pVaryingTemp->uFragmentShaderHandle;

									pVariable = &Variable;
								}

								GlobalVariable.ShaderType = CEffectInterface::FRAGMENT_SHADER;

								m_ConstantVariableMap.Insert(GlobalVariable, *pVariable);
							}
						}

						puConstants[i] = pVariable->uHandle;
					}

					++ pBlendConstant;
				}

				if( m_EffectInfos.IsEnd(EffectInfoIterator) )
					pInfo->pBlendMethod->OutputColorBlendHandler(uResultColor, uDestinationColor, uSourceColor1, puAttributes, puConstants, puTemporary, m_FragmentShaderFunctionDefiner);
				else
				{
					pInfo->pBlendMethod->OutputColorBlendHandler(uSourceColor2, uDestinationColor, uSourceColor1, puAttributes, puConstants, puTemporary, m_FragmentShaderFunctionDefiner);

					ALCHEMY_SWAP(uDestinationColor, uSourceColor2);
				}
			}
			else
			{
				pInfo->pEffectInfo->ShaderDefineHandler(uDestinationColor, puSamplers, puAttributes, puConstants, m_pVaryingBuffer, puTemporary, pAttributeVariables, pConstantVariables, m_VertexShaderFunctionDefiner, m_FragmentShaderFunctionDefiner, m_EffectCodeUtils);

				bIsBlend = true;
			}
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

	m_VaryingVariableMap.Clear();

	m_AttributeVaryingMap.Clear();
	m_ConstantVaryingMap.Clear();

	m_FreeTemporaryMap.Clear();

	m_EffectInfos.Clear();

	m_uAttributeAddress = 0;
	m_uVaryingAddress   = 0;

	m_bIsBlend = false;

	PASS Pass;

	Pass.uVertexShader   = m_VertexShaderFunctionDefiner.EndDefine();
	Pass.uFragmentShader = m_FragmentShaderFunctionDefiner.EndDefine();

	return m_Parent.m_Passes.Add(Pass) + 1;
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

	if(TemporaryDeclaration.ShaderType == CEffectInterface::VERTEX_SHADER)
		return m_VertexShaderFunctionDefiner.ApplyVariable(TemporaryDeclaration.VariableType, 1);
	else if(TemporaryDeclaration.ShaderType == CEffectInterface::FRAGMENT_SHADER)
		return m_FragmentShaderFunctionDefiner.ApplyVariable(TemporaryDeclaration.VariableType, 1);

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
	CVector<TEMPORARY>::CIterator Iteractor = m_FreeTemporaryCache.GetBeginIterator();
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
			if(pTemporaryDeclaration->ShaderType == CEffectInterface::VERTEX_SHADER)
				m_VertexShaderFunctionDefiner.FreeVariable(FreeTemporaryIterator->uHandle);
			else if(pTemporaryDeclaration->ShaderType == CEffectInterface::FRAGMENT_SHADER)
				m_FragmentShaderFunctionDefiner.FreeVariable(FreeTemporaryIterator->uHandle);

			++ FreeTemporaryIterator;
		}

		++ MapIterator;
	}

	m_FreeTemporaryMap.Clear();
}

CEffectCode::CEffectCode(CCompilerInterface& Compiler) :
m_Compiler(Compiler),
m_uVertexShaderGlobalRegisterCount(0),
m_uFragmentShaderGlobalRegisterCount(0),
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
		break;
	case VECTOR2_VARIABLE:
		uCode = 3 ;//(0 << 3) | 1 << 1 | 1
		break;
	case VECTOR3_VARIABLE:
		uCode = 5 ;//(0 << 3) | 1 << 2 | 1
		break;
	case VECTOR4_VARIABLE:
		uCode = 7 ;//(0 << 3) | 1 << 3 | 1
		break;
	case MATRIX2X2_VARIABLE:
		uCode = 11;//(1 << 3) | 1 << 1 | 1
		break;
	case MATRIX3X3_VARIABLE:
		uCode = 21;//(2 << 3) | 1 << 2 | 1
		break;
	case MATRIX4X3_VARIABLE:
		uCode = 29;//(3 << 3) | 1 << 2 | 1
		break;
	case MATRIX4X4_VARIABLE:
		uCode = 31;//(3 << 3) | 1 << 3 | 1
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
		case CRenderMethodInterface::POSITION_OFFSET:
			return VECTOR3_VARIABLE;
			break;

		case CRenderMethodInterface::UV_OFFSET:
			return VECTOR2_VARIABLE;
			break;

		case CRenderMethodInterface::ELAPSED_TIME:
			return VECTOR1_VARIABLE;
			break;
		case CRenderMethodInterface::TOTAL_TIME:
			return VECTOR1_VARIABLE;
			break;

		case CRenderMethodInterface::AMBIENT_LIGHT:
			return VECTOR4_VARIABLE;
			break;
		}

		break;

	case ARRAY_VARIABLE:
		switch(uDetail)
		{
		case CRenderMethodInterface::BONE_MATRIX_ARRAY:
			return MATRIX4X3_VARIABLE;
			break;
		}
		break;

	case MATRIX_VARIABLE:

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
		}

		break;
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
		case CRenderMethod::LIGHT_LOCAL_POSITION:
			return VECTOR3_VARIABLE;
			break;
		case CRenderMethod::LIGHT_LOCAL_DIRECTION:
			return VECTOR3_VARIABLE;
			break;
		case CRenderMethod::LIGHT_WORLD_VIEW_PROJECT_MATRIX:
			return MATRIX4X4_VARIABLE;
			break;
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

const CEffectCode::SAMPLERDECLARATION* CEffectCode::GetSamplerDeclaration(UINT uHandle)const
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

CCompilerInterface::REGISTERTYPE CEffectCode::GetRegisterType(UINT uVariable)const
{
	const VARIABLE* pVariable;
	if(!uVariable || ( pVariable = m_VariablePool.Get(uVariable - 1) ) == ALCHEMY_NULL)
	{
		ALCHEMY_DEBUG_WARNING("CEffectCode::GetRegisterType: Error variable.");

		return CCompilerInterface::NONE;
	}

	if(pVariable->Type == SAMPLER_VARIABLE)
		return CCompilerInterface::SAMPLER;

	const FUNCTION* pFunction;
	if(pVariable->uFunction && ( pFunction = m_FunctionPool.Get(pVariable->uFunction - 1) ) != ALCHEMY_NULL)
	{
		if(pFunction->uParametersVariable == uVariable)
			return CCompilerInterface::PARAMETER;
		else if(pFunction->uResultVariable == uVariable)
			return pVariable->uGlobal ? CCompilerInterface::OUTPUT : CCompilerInterface::RESULT;
		else if(pVariable->uParent)
		{
			UINT uParent = pVariable->uParent;
			const VARIABLE* pPrevious = pVariable, * pTemp = pPrevious->uParent ? m_VariablePool.Get(pPrevious->uParent - 1) : ALCHEMY_NULL;;

			 while(pTemp)
			 {
				 pPrevious = pTemp;

				 pTemp = pPrevious->uParent ? m_VariablePool.Get(pTemp->uParent - 1) : ALCHEMY_NULL;

				 if(pTemp)
					 uParent = pPrevious->uParent;
			 }

			if(uParent == pFunction->uParametersVariable)
				return CCompilerInterface::PARAMETER;
			if(uParent == pFunction->uResultVariable)
			{
				if(pVariable->uGlobal)
				{
					const SHADERFUNCTION* pShaderFunction;
					if(pFunction->uShader && ( pShaderFunction = m_ShaderFunctionPool.Get(pFunction->uShader - 1) ) != ALCHEMY_NULL)
					{
						const SHADERPARAMETER* pShaderParameter;
						if(pShaderFunction->Type == CEffectInterface::VERTEX_SHADER)
						{
							if( ( pShaderParameter = m_ShaderParameterPool.Get(pVariable->uGlobal - 1) ) != ALCHEMY_NULL && pShaderParameter->Usage == CVertexDeclaration::POSITIONT )
								return CCompilerInterface::OUTPUT;
						}
						else if(pShaderFunction->Type == CEffectInterface::FRAGMENT_SHADER)
							return CCompilerInterface::OUTPUT;
					}
				}

				return CCompilerInterface::RESULT;
			}
		}

		return CCompilerInterface::TEMPORARY;
	}

	return CCompilerInterface::GLOBAL;
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

			uByteAlignmentShift = m_Compiler.GetByteAlignmentShift();
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

	m_SamplerDeclarationPool.Clear();

	m_StructDeclarationPool.Clear();
	m_StructPool.Clear();

	m_CodePool.Clear();
	m_FunctionPool.Clear();

	m_ShaderParameterPool.Clear();
	m_ShaderParametersPool.Clear();

	m_ShaderFunctionPool.Clear();
}

UINT CEffectCode::GetAddressVariable(UINT uSource, UINT uAddress)
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
		Variable.uRegister    = uAddress << ALCHEMY_SHIFT(VECTOR1_VARIABLE);
		Variable.uDeclaration = 0;
		Variable.uGlobal      = 0;

		break;

	case VECTOR3_VARIABLE:
		if(uAddress > 2)
		{
			ALCHEMY_DEBUG_WARNING("CEffectCode::AddAddressVariable: Address out of range.");

			return 0;
		}

		Variable.Type         = VECTOR1_VARIABLE;
		Variable.uRegister    = uAddress << ALCHEMY_SHIFT(VECTOR1_VARIABLE);
		Variable.uDeclaration = 0;
		Variable.uGlobal      = 0;

		break;

	case VECTOR4_VARIABLE:
		if(uAddress > 3)
		{
			ALCHEMY_DEBUG_WARNING("CEffectCode::AddAddressVariable: Address out of range.");

			return 0;
		}

		Variable.Type         = VECTOR1_VARIABLE;
		Variable.uRegister    = uAddress << ALCHEMY_SHIFT(VECTOR1_VARIABLE);
		Variable.uDeclaration = 0;
		Variable.uGlobal      = 0;

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
		Variable.uGlobal      = 0;

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
		Variable.uGlobal      = 0;

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
		Variable.uGlobal      = 0;

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
		Variable.uGlobal      = 0;

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
		UINT uShaderParameter = 0, uRegister = 0, uPreviousRegister = 0;
		PUINT puShaderParameters;

		LPFUNCTION pFunction;
		LPSHADERFUNCTION pShaderFunction;

		if( pVariable->uFunction && 
			( pFunction = m_FunctionPool.Get(pVariable->uFunction - 1) )!= ALCHEMY_NULL &&
			pFunction->uShader &&
			( pShaderFunction = m_ShaderFunctionPool.Get(pFunction->uShader - 1) )!= ALCHEMY_NULL &&
			pVariable->uGlobal && 
			( puShaderParameters = m_ShaderParametersPool.Get(pVariable->uGlobal - 1) )!= ALCHEMY_NULL)
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

			uShaderParameter = *puShaderParameters;
		}

		LPSTRUCTDECLARATION pStructDeclaration = ALCHEMY_NULL;

		UINT uParentStructDeclaration = 0, uAddressCount = uAddress + 1, uCurrentShaderParamer = 0, i;

		LPSHADERPARAMETER pShaderParameter;

		for(i = 0; i < uAddressCount; ++ i)
		{
			if(!uStructDeclaration)
			{
				if(uStructDeclaration != uParentStructDeclaration)
				{
					uStructDeclaration = uParentStructDeclaration;

					continue;
				}

				ALCHEMY_DEBUG_WARNING("CEffectCode::AddAddressVariable: Address out of range.");

				return 0;
			}

			uRegister += uPreviousRegister;

			pStructDeclaration = m_StructDeclarationPool.Get(uStructDeclaration - 1);

			if(pStructDeclaration)
			{
				if( ( pStructDeclaration->Type != STRUCT_VARIABLE ||  !( puStruct = m_StructPool.Get(pStructDeclaration->uStruct - 1) ) ) )
				{
					uPreviousRegister = GetRegisterCount(pStructDeclaration->Type);

					if(uShaderParameter)
					{
						pShaderParameter = m_ShaderParameterPool.Get(uShaderParameter - 1);

						if( (pShaderParameter->Usage == CVertexDeclaration::POSITIONT) && (pShaderFunction->Type == CEffectInterface::FRAGMENT_SHADER || bIsVertexShaderOutput) )
							uPreviousRegister = 0;

						uCurrentShaderParamer = uShaderParameter;
						uShaderParameter      = pShaderParameter ? pShaderParameter->uNext : 0;
					}

					uStructDeclaration = pStructDeclaration->uNext;
				}
				else
				{
					uPreviousRegister        = 0;

					uStructDeclaration       = *puStruct;

					uParentStructDeclaration = pStructDeclaration->uNext;
				}
			}
			else
				uStructDeclaration = 0;
		}

		if(pStructDeclaration)
		{
			Variable.Type         = pStructDeclaration->Type;
			Variable.uRegister    = uRegister;
			Variable.uDeclaration = pStructDeclaration->uStruct;
			Variable.uGlobal      = uCurrentShaderParamer;
		}

		break;
	}

	UINT uHandle = m_VariablePool.Add(Variable) + 1;
	m_AddressVariableHandleMap.Insert(AddressVariable, uHandle);

	return uHandle;
}

UINT CEffectCode::AddGlobalVariable(const GLOBALVARIABLEKEY& GlobalVariableKey)
{
	VARIABLETYPE VariableType = GetGlobalVariableType(GlobalVariableKey.GlobalType, GlobalVariableKey.uDetail);
	if(VariableType == UNKNOWN_VARIABLE)
		return 0;

	PUINT puCount = m_GlobalVariableCountMap.Get(GlobalVariableKey);
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
	GlobalVariable.pValue     = GlobalVariableKey.pValue;

	VARIABLE Variable;
	Variable.Type         = VariableType;
	Variable.uDeclaration = 0;
	Variable.uFunction    = 0;
	Variable.uGlobal      = m_GlobalVariablePool.Add(GlobalVariable) + 1;
	Variable.uParent      = 0;

	if(GlobalVariable.ShaderType == CEffectInterface::VERTEX_SHADER)
	{
		Variable.uRegister                  = m_uVertexShaderGlobalRegisterCount;
		m_uVertexShaderGlobalRegisterCount += GlobalVariableKey.uCount * GetRegisterCount(VariableType, 0);
	}
	else if(GlobalVariable.ShaderType == CEffectInterface::FRAGMENT_SHADER)
	{
		Variable.uRegister                    = m_uFragmentShaderGlobalRegisterCount;
		m_uFragmentShaderGlobalRegisterCount += GlobalVariableKey.uCount * GetRegisterCount(VariableType, 0);
	}

	UINT uHandle = m_VariablePool.Add(Variable) + 1;

	m_GlobalVariableHandleMap.Insert(GlobalVariable, uHandle);

	return uHandle;
}

UINT CEffectCode::AddSamplerVariable(const SAMPLERDECLARATION& Sampler, UINT uIndex)
{
	VARIABLE Variable;
	Variable.Type         = SAMPLER_VARIABLE;
	Variable.uRegister    = uIndex;
	Variable.uDeclaration = m_SamplerDeclarationPool.Add(Sampler) + 1;
	Variable.uFunction    = 0;
	Variable.uGlobal      = 0;
	Variable.uParent      = 0;

	return m_VariablePool.Add(Variable) + 1;
}

UINT CEffectCode::AddPass(UINT uVertexShaderFunction, UINT uFragmentShaderFunction)
{
	/////////////////////////////////////////////////////////////
	////////////PASS对接 未完成
	/////////////////////////////////////////////////////////////

	PASS Pass;

	Pass.uVertexShader   = uVertexShaderFunction;
	Pass.uFragmentShader = uFragmentShaderFunction;

	return m_Passes.Add(Pass) + 1;
}

CEffectInterface::EFFECTTYPE CEffectCode::BuildShaderFunctionCode(STRING& Desination, UINT uShaderFunction)
{
	return m_Compiler.BuildShaderFunctionCode(Desination, uShaderFunction, *this);
}

CBuffer& CEffectCode::CreateParameterList(LPGLOBALINPUTHANDLER* ppGlobalInputHandler)
{
	LPGLOBALINPUTHANDLER pGlobalInputHandler = ALCHEMY_NULL;
	UINT uPassCount = m_Passes.GetLength(), uByteAlignmentShift = m_Compiler.GetByteAlignmentShift(), uCurrentCount, i, j;
	const UINT* puUserVariable, * puShaderParameter;

	CRenderMethod::LPPARAMETERLIST pParameterList;

	ALCHEMY_DEBUG_NEW(pParameterList, CRenderMethod::PARAMETERLIST[uPassCount]);

	CRenderMethod::PARAMETERLIST* pCurrentParameterList = pParameterList;
	CRenderMethod::LPPARAMETERINFO pParameterInfo;

	const PASS* pPass;
	const FUNCTION* pVertexShaderFunction, * pFragmentShaderFunction;
	const USERVARIABLE* pUserVariable;
	const GLOBALVARIABLE* pGlobalVariable;

	const SHADERFUNCTION* pShaderFunction;
	const SHADERPARAMETER* pShaderParameter;
	LPELEMENTPARAMETER pVertexElementParameter;

	CEffectInterface::EFFECTTYPE Type;
	CGlobalInputHandler::LPGLOBALINPUT pGlobalInput;

	for(i = 0; i < uPassCount; ++ i)
	{
		pCurrentParameterList->pLightParameters = ALCHEMY_NULL;
		pCurrentParameterList->pTextures = ALCHEMY_NULL;

		pCurrentParameterList->uParameterFlag = 0;
		pCurrentParameterList->uArrayParameterFlag = 0;
		pCurrentParameterList->uMatrixParameterFlag = 0;
		pCurrentParameterList->uCameraParameterFlag = 0;
		pCurrentParameterList->uMaterialParameterFlag = 0;
		pCurrentParameterList->uConstantParameterFlag = 0;

		pCurrentParameterList->puLightParameterFlag = ALCHEMY_NULL;
		pCurrentParameterList->puTextureParameterFlag = ALCHEMY_NULL;

		pCurrentParameterList->uLightCount = 0;
		pCurrentParameterList->uTextureCount = 0;

		pCurrentParameterList->pVertexElementParameters = ALCHEMY_NULL;
		pCurrentParameterList->uVertexElementParameterCount = 0;

		pPass = m_Passes.Get(i);
		if( pPass && 
			(pVertexShaderFunction   = pPass->uVertexShader   ? m_FunctionPool.Get(pPass->uVertexShader   - 1) : ALCHEMY_NULL) != ALCHEMY_NULL &&
			(pFragmentShaderFunction = pPass->uFragmentShader ? m_FunctionPool.Get(pPass->uFragmentShader - 1) : ALCHEMY_NULL) != ALCHEMY_NULL )
		{
			if(pVertexShaderFunction->uShader && ( pShaderFunction = m_ShaderFunctionPool.Get(pVertexShaderFunction->uShader - 1) ) != ALCHEMY_NULL)
			{
				if( pShaderFunction->uParameters && ( puShaderParameter = m_ShaderParametersPool.Get(pShaderFunction->uParameters - 1) ) != ALCHEMY_NULL)
				{
					pShaderParameter = m_ShaderParameterPool.Get(*puShaderParameter - 1);

					while(pShaderParameter)
					{
						++ pCurrentParameterList->uVertexElementParameterCount;
						pShaderParameter = pShaderParameter->uNext ? m_ShaderParameterPool.Get(pShaderParameter->uNext - 1) : ALCHEMY_NULL;
					}

					ALCHEMY_DEBUG_NEW(pCurrentParameterList->pVertexElementParameters, ELEMENTPARAMETER[pCurrentParameterList->uVertexElementParameterCount]);

					pShaderParameter = m_ShaderParameterPool.Get(*puShaderParameter - 1);

					uCurrentCount = 0;
					while(pShaderParameter)
					{
						pVertexElementParameter         = &pCurrentParameterList->pVertexElementParameters[uCurrentCount ++];
						pVertexElementParameter->Type   = pShaderParameter->Type;
						pVertexElementParameter->Usage  = pShaderParameter->Usage;
						pVertexElementParameter->uIndex = pShaderParameter->uIndex;

						pShaderParameter                = pShaderParameter->uNext ? m_ShaderParameterPool.Get(pShaderParameter->uNext - 1) : ALCHEMY_NULL;
					}
				}
			}

			puUserVariable = pVertexShaderFunction->uUserVariables ? m_UserVariablesPool.Get(pVertexShaderFunction->uUserVariables - 1) : ALCHEMY_NULL;
			pUserVariable  = (puUserVariable && *puUserVariable) ? m_UserVariablePool.Get(*puUserVariable - 1) : ALCHEMY_NULL;
			Type = CEffectInterface::VERTEX_SHADER;
			while(pUserVariable)
			{
				switch(pUserVariable->Type)
				{
				case CCompilerInterface::GLOBAL:
					pGlobalVariable = pUserVariable->uDeclaration ? m_GlobalVariablePool.Get(pUserVariable->uDeclaration - 1) : ALCHEMY_NULL;
					if(pGlobalVariable)
					{
						if(pGlobalVariable->pValue)
						{
							if(ppGlobalInputHandler)
							{
								if(!pGlobalInputHandler)
								{
									pGlobalInputHandler = &GLOBALINPUTHANDLER::Create();

									pGlobalInputHandler->m_uPassCount = uPassCount;

									ALCHEMY_DEBUG_NEW(pGlobalInputHandler->m_ppGlobalInput     , CGlobalInputHandler::LPGLOBALINPUT[uPassCount]);
									ALCHEMY_DEBUG_NEW(pGlobalInputHandler->m_puGlobalInputCount, UINT                              [uPassCount]);

									for(j = 0; j < uPassCount; ++ j)
									{
										pGlobalInputHandler->m_ppGlobalInput     [j] = ALCHEMY_NULL;
										pGlobalInputHandler->m_puGlobalInputCount[j] = 0;
									}

									*ppGlobalInputHandler = pGlobalInputHandler;
								}

								++ pGlobalInputHandler->m_puGlobalInputCount[i];
							}
						}
						else
						{
							switch(pGlobalVariable->GlobalType)
							{
							case NORMAL_VARIABLE:
								if(!pGlobalVariable->uIndex && pGlobalVariable->uDetail < CRenderMethodInterface::TOTAL_PARAMETERS)
								{
									pParameterInfo = &pCurrentParameterList->Parameters[pGlobalVariable->uDetail];

									if(ALCHEMY_TEST_BIT(pCurrentParameterList->uParameterFlag, pGlobalVariable->uDetail) && Type != pParameterInfo->Type)
									{
										if(pParameterInfo->Type == CEffectInterface::VERTEX_SHADER)
											pParameterInfo->uBetaIndex = pUserVariable->uRegister >> uByteAlignmentShift;
										else if(pParameterInfo->Type == CEffectInterface::FRAGMENT_SHADER)
											pParameterInfo->uAlphaIndex = pUserVariable->uRegister >> uByteAlignmentShift;

										pParameterInfo->Type = CEffectInterface::PROGRAM;
									}
									else
									{
										pParameterInfo->Type        = Type;
										pParameterInfo->uAlphaIndex = pUserVariable->uRegister >> uByteAlignmentShift;
									}

									ALCHEMY_SET_BIT(pCurrentParameterList->uParameterFlag, pGlobalVariable->uDetail);
								}
								break;

							case ARRAY_VARIABLE:
								if(!pGlobalVariable->uIndex && pGlobalVariable->uDetail < CRenderMethodInterface::TOTAL_ARRAY_PARAMETERS)
								{
									pParameterInfo = &pCurrentParameterList->ArrayParameters[pGlobalVariable->uDetail];

									if(ALCHEMY_TEST_BIT(pCurrentParameterList->uArrayParameterFlag, pGlobalVariable->uDetail) && Type != pParameterInfo->Type)
									{
										if(pParameterInfo->Type == CEffectInterface::VERTEX_SHADER)
											pParameterInfo->uBetaIndex = pUserVariable->uRegister >> uByteAlignmentShift;
										else if(pParameterInfo->Type == CEffectInterface::FRAGMENT_SHADER)
											pParameterInfo->uAlphaIndex = pUserVariable->uRegister >> uByteAlignmentShift;

										pParameterInfo->Type = CEffectInterface::PROGRAM;
									}
									else
									{
										pParameterInfo->Type        = Type;
										pParameterInfo->uAlphaIndex = pUserVariable->uRegister >> uByteAlignmentShift;
									}

									ALCHEMY_SET_BIT(pCurrentParameterList->uArrayParameterFlag, pGlobalVariable->uDetail);
								}
								break;

							case MATRIX_VARIABLE:
								if(!pGlobalVariable->uIndex && pGlobalVariable->uDetail < CRenderMethod::TOTAL_MATRIX_PARAMETERS)
								{
									pParameterInfo = &pCurrentParameterList->MatrixParameters[pGlobalVariable->uDetail];

									if(ALCHEMY_TEST_BIT(pCurrentParameterList->uMatrixParameterFlag, pGlobalVariable->uDetail) && Type != pParameterInfo->Type)
									{
										if(pParameterInfo->Type == CEffectInterface::VERTEX_SHADER)
											pParameterInfo->uBetaIndex = pUserVariable->uRegister >> uByteAlignmentShift;
										else if(pParameterInfo->Type == CEffectInterface::FRAGMENT_SHADER)
											pParameterInfo->uAlphaIndex = pUserVariable->uRegister >> uByteAlignmentShift;

										pParameterInfo->Type = CEffectInterface::PROGRAM;
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
										if(pParameterInfo->Type == CEffectInterface::VERTEX_SHADER)
											pParameterInfo->uBetaIndex = pUserVariable->uRegister >> uByteAlignmentShift;
										else if(pParameterInfo->Type == CEffectInterface::FRAGMENT_SHADER)
											pParameterInfo->uAlphaIndex = pUserVariable->uRegister >> uByteAlignmentShift;

										pParameterInfo->Type = CEffectInterface::PROGRAM;
									}
									else
									{
										pParameterInfo->Type        = Type;
										pParameterInfo->uAlphaIndex = pUserVariable->uRegister >> uByteAlignmentShift;
									}

									ALCHEMY_SET_BIT(pCurrentParameterList->uCameraParameterFlag, pGlobalVariable->uDetail);
								}
								break;
							case MATERIAL_VARIABLE:
								if(!pGlobalVariable->uIndex && pGlobalVariable->uDetail < CRenderMethod::TOTAL_MATERIAL_PARAMETERS)
								{
									pParameterInfo = &pCurrentParameterList->MaterialParameters[pGlobalVariable->uDetail];

									if(ALCHEMY_TEST_BIT(pCurrentParameterList->uMaterialParameterFlag, pGlobalVariable->uDetail) && Type != pParameterInfo->Type)
									{
										if(pParameterInfo->Type == CEffectInterface::VERTEX_SHADER)
											pParameterInfo->uBetaIndex = pUserVariable->uRegister >> uByteAlignmentShift;
										else if(pParameterInfo->Type == CEffectInterface::FRAGMENT_SHADER)
											pParameterInfo->uAlphaIndex = pUserVariable->uRegister >> uByteAlignmentShift;

										pParameterInfo->Type = CEffectInterface::PROGRAM;
									}
									else
									{
										pParameterInfo->Type        = Type;
										pParameterInfo->uAlphaIndex = pUserVariable->uRegister >> uByteAlignmentShift;
									}

									ALCHEMY_SET_BIT(pCurrentParameterList->uMaterialParameterFlag, pGlobalVariable->uDetail);
								}
								break;
							case LIGHT_VARIABLE:
								if(pGlobalVariable->uDetail < CRenderMethod::TOTAL_LIGHT_PARAMETERS)
								{
									uCurrentCount = pGlobalVariable->uIndex + 1;

									pCurrentParameterList->uLightCount = ALCHEMY_MAXINUM(pCurrentParameterList->uLightCount, uCurrentCount);
								}
								break;
							case CONSTANT_VARIABLE:
								if(!pGlobalVariable->uIndex && pGlobalVariable->uDetail < CRenderMethod::TOTAL_CONSTANT_PARAMETERS)
								{
									pParameterInfo = &pCurrentParameterList->ConstantParameters[pGlobalVariable->uDetail];

									if(ALCHEMY_TEST_BIT(pCurrentParameterList->uConstantParameterFlag, pGlobalVariable->uDetail) && Type != pParameterInfo->Type)
									{
										if(pParameterInfo->Type == CEffectInterface::VERTEX_SHADER)
											pParameterInfo->uBetaIndex = pUserVariable->uRegister >> uByteAlignmentShift;
										else if(pParameterInfo->Type == CEffectInterface::FRAGMENT_SHADER)
											pParameterInfo->uAlphaIndex = pUserVariable->uRegister >> uByteAlignmentShift;

										pParameterInfo->Type = CEffectInterface::PROGRAM;
									}
									else
									{
										pParameterInfo->Type        = Type;
										pParameterInfo->uAlphaIndex = pUserVariable->uRegister >> uByteAlignmentShift;
									}

									ALCHEMY_SET_BIT(pCurrentParameterList->uConstantParameterFlag, pGlobalVariable->uDetail);
								}
								break;
							}
						}
					}
					break;

				case CCompilerInterface::SAMPLER:
					++ pCurrentParameterList->uTextureCount;
					break;
				}

				pUserVariable = pUserVariable->uNext ? m_UserVariablePool.Get(pUserVariable->uNext - 1) : ALCHEMY_NULL;

				if(!pUserVariable && Type == CEffectInterface::VERTEX_SHADER)
				{
					Type = CEffectInterface::FRAGMENT_SHADER;

					puUserVariable = pFragmentShaderFunction->uUserVariables ? m_UserVariablesPool.Get(pFragmentShaderFunction->uUserVariables - 1) : ALCHEMY_NULL;
					pUserVariable  = (puUserVariable && *puUserVariable) ? m_UserVariablePool.Get(*puUserVariable - 1) : ALCHEMY_NULL;
				}
			}

			ALCHEMY_DEBUG_NEW(pCurrentParameterList->pLightParameters      , CRenderMethod::PARAMETERINFO       [pCurrentParameterList->uLightCount  ][CRenderMethod::TOTAL_LIGHT_PARAMETERS  ]);
			ALCHEMY_DEBUG_NEW(pCurrentParameterList->pTextureParameters    , CRenderMethod::PARAMETERINFO       [pCurrentParameterList->uTextureCount][CRenderMethod::TOTAL_TEXTURE_PARAMETERS]);

			ALCHEMY_DEBUG_NEW(pCurrentParameterList->pTextures             , CRenderMethod::PARAMETERINFO       [pCurrentParameterList->uTextureCount                                         ]);

			ALCHEMY_DEBUG_NEW(pCurrentParameterList->puLightParameterFlag  , UINT                               [pCurrentParameterList->uLightCount                                           ]);
			ALCHEMY_DEBUG_NEW(pCurrentParameterList->puTextureParameterFlag, UINT                               [pCurrentParameterList->uTextureCount                                         ]);

			for(j = 0; j < pCurrentParameterList->uLightCount; ++ j)
				pCurrentParameterList->puLightParameterFlag[j] = 0;

			for(j = 0; j < pCurrentParameterList->uTextureCount; ++ j)
				pCurrentParameterList->puTextureParameterFlag[j] = 0;

			if(pGlobalInputHandler)
			{
				ALCHEMY_DEBUG_NEW(pGlobalInputHandler->m_ppGlobalInput[i], CGlobalInputHandler::GLOBALINPUT[pGlobalInputHandler->m_puGlobalInputCount[i]]);

				pGlobalInputHandler->m_puGlobalInputCount[i] = 0;
			}

			uCurrentCount = 0;

			puUserVariable = pVertexShaderFunction->uUserVariables ? m_UserVariablesPool.Get(pVertexShaderFunction->uUserVariables - 1) : ALCHEMY_NULL;
			pUserVariable  = (puUserVariable && *puUserVariable) ? m_UserVariablePool.Get(*puUserVariable - 1) : ALCHEMY_NULL;
			Type = CEffectInterface::VERTEX_SHADER;
			while(pUserVariable)
			{
				switch(pUserVariable->Type)
				{
				case CCompilerInterface::GLOBAL:
					pGlobalVariable = pUserVariable->uDeclaration ? m_GlobalVariablePool.Get(pUserVariable->uDeclaration - 1) : ALCHEMY_NULL;
					if(pGlobalVariable)
					{
						if(pGlobalVariable)
						{
							if(pGlobalVariable->pValue)
							{
								if(pGlobalInputHandler)
								{
									pGlobalInput = &pGlobalInputHandler->m_ppGlobalInput[i][pGlobalInputHandler->m_puGlobalInputCount[i] ++];

									if(pGlobalVariable->GlobalType == INPUT_VARIABLE)
										pGlobalInput->uCode = pGlobalVariable->uDetail;
									else
										pGlobalInput->uCode = BuildGlobalInputCode(GetGlobalVariableType(pGlobalVariable->GlobalType, pGlobalVariable->uDetail), 4, 16, false);

									pGlobalInput->ShaderType = pGlobalVariable->ShaderType;
									pGlobalInput->uIndex     = pUserVariable->uRegister >> uByteAlignmentShift;
									pGlobalInput->uCount     = pGlobalVariable->uCount;
									pGlobalInput->pValue     = pGlobalVariable->pValue;
								}
							}
							else
							{
								switch(pGlobalVariable->GlobalType)
								{
								case LIGHT_VARIABLE:
									if(pGlobalVariable->uDetail < CRenderMethod::TOTAL_LIGHT_PARAMETERS)
									{
										pParameterInfo = &pCurrentParameterList->pLightParameters[pGlobalVariable->uIndex][pGlobalVariable->uDetail];

										if(ALCHEMY_TEST_BIT(pCurrentParameterList->puLightParameterFlag[pGlobalVariable->uIndex], pGlobalVariable->uDetail) && Type != pParameterInfo->Type)
										{
											if(pParameterInfo->Type == CEffectInterface::VERTEX_SHADER)
												pParameterInfo->uBetaIndex = pUserVariable->uRegister >> uByteAlignmentShift;
											else if(pParameterInfo->Type == CEffectInterface::FRAGMENT_SHADER)
												pParameterInfo->uAlphaIndex = pUserVariable->uRegister >> uByteAlignmentShift;

											pParameterInfo->Type = CEffectInterface::PROGRAM;
										}
										else
										{
											pParameterInfo->Type   = Type;
											pParameterInfo->uAlphaIndex = pUserVariable->uRegister >> uByteAlignmentShift;
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
											if(pParameterInfo->Type == CEffectInterface::VERTEX_SHADER)
												pParameterInfo->uBetaIndex = pUserVariable->uRegister >> uByteAlignmentShift;
											else if(pParameterInfo->Type == CEffectInterface::FRAGMENT_SHADER)
												pParameterInfo->uAlphaIndex = pUserVariable->uRegister >> uByteAlignmentShift;

											pParameterInfo->Type = CEffectInterface::PROGRAM;
										}
										else
										{
											pParameterInfo->Type = Type;
											pParameterInfo->uAlphaIndex = pUserVariable->uRegister >> uByteAlignmentShift;
										}

										ALCHEMY_SET_BIT(pCurrentParameterList->puTextureParameterFlag[pGlobalVariable->uIndex], pGlobalVariable->uDetail);
									}
									break;
								}
							}
						}
					}
					break;

				case CCompilerInterface::SAMPLER:
					pParameterInfo = &pCurrentParameterList->pTextures[uCurrentCount ++];
					pParameterInfo->Type   = Type;
					pParameterInfo->uAlphaIndex = pUserVariable->uRegister;
					break;
				}

				pUserVariable = pUserVariable->uNext ? m_UserVariablePool.Get(pUserVariable->uNext - 1) : ALCHEMY_NULL;

				if(!pUserVariable && Type == CEffectInterface::VERTEX_SHADER)
				{
					Type = CEffectInterface::FRAGMENT_SHADER;

					puUserVariable = pFragmentShaderFunction->uUserVariables ? m_UserVariablesPool.Get(pFragmentShaderFunction->uUserVariables - 1) : ALCHEMY_NULL;
					pUserVariable  = (puUserVariable && *puUserVariable) ? m_UserVariablePool.Get(*puUserVariable - 1) : ALCHEMY_NULL;
				}
			}
		}

		++ pCurrentParameterList;
	}
	
	CBuffer& Buffer = CRenderMethod::CreateParameterListBuffer(pParameterList, uPassCount);

	CRenderMethod::DestroyParameterList(pParameterList, uPassCount);

	return Buffer;
}