#include "AlchemyFlashStdafx.h"
#include "AlchemyFlashAGALAssembler.h"

using namespace alchemy;

const CHAR CFlashAGALAssembler::sm_cAGALInstruction[CCompilerInterface::TOTAL_INSTRUCTION][4] = 
{
	ALCHEMY_TEXT("mov"),
	ALCHEMY_TEXT("add"),
	ALCHEMY_TEXT("sub"),
	ALCHEMY_TEXT("mul"),
	ALCHEMY_TEXT("div"),
	ALCHEMY_TEXT("rcp"),
	ALCHEMY_TEXT("min"),
	ALCHEMY_TEXT("max"),
	ALCHEMY_TEXT("frc"),
	ALCHEMY_TEXT("sqt"),
	ALCHEMY_TEXT("rsq"),
	ALCHEMY_TEXT("pow"),
	ALCHEMY_TEXT("log"),
	ALCHEMY_TEXT("exp"),
	ALCHEMY_TEXT("nrm"),
	ALCHEMY_TEXT("sin"),
	ALCHEMY_TEXT("cos"),
	ALCHEMY_TEXT("crs"),
	ALCHEMY_TEXT("dp3"),
	ALCHEMY_TEXT("dp4"),
	ALCHEMY_TEXT("abs"),
	ALCHEMY_TEXT("neg"),
	ALCHEMY_TEXT("sat"),
	ALCHEMY_TEXT("m33"),
	ALCHEMY_TEXT("m44"),
	ALCHEMY_TEXT("m34"),
	ALCHEMY_TEXT("ifz"),
	ALCHEMY_TEXT("inz"),
	ALCHEMY_TEXT("ife"),
	ALCHEMY_TEXT("ine"),
	ALCHEMY_TEXT("ifg"),
	ALCHEMY_TEXT("ifl"),
	ALCHEMY_TEXT("ieg"),
	ALCHEMY_TEXT("iel"),
	ALCHEMY_TEXT("els"),
	ALCHEMY_TEXT("eif"),
	ALCHEMY_TEXT("rep"),
	ALCHEMY_TEXT("erp"),
	ALCHEMY_TEXT("brk"),
	ALCHEMY_TEXT("kil"),
	ALCHEMY_TEXT("tex"),
	ALCHEMY_TEXT("sge"),
	ALCHEMY_TEXT("slt"),
	ALCHEMY_TEXT("sgn")
};

const CHAR CFlashAGALAssembler::sm_cVERTEX_SHADER_REGISTER[CCompilerInterface::TOTAL_REGISTERTYPE][3]   = {ALCHEMY_TEXT(""), ALCHEMY_TEXT("vc"), ALCHEMY_TEXT("vt"), ALCHEMY_TEXT("va"), ALCHEMY_TEXT("v"), ALCHEMY_TEXT("op"), ALCHEMY_TEXT("")  };
const CHAR CFlashAGALAssembler::sm_cFRAGMENT_SHADER_REGISTER[CCompilerInterface::TOTAL_REGISTERTYPE][3] = {ALCHEMY_TEXT(""), ALCHEMY_TEXT("fc"), ALCHEMY_TEXT("ft"), ALCHEMY_TEXT("v") , ALCHEMY_TEXT("") , ALCHEMY_TEXT("oc"), ALCHEMY_TEXT("fs")};
const CHAR CFlashAGALAssembler::sm_cREGISTER_ADDRESS[5] = ALCHEMY_TEXT("xyzw");


const CHAR CFlashAGALAssembler::sm_cSAMPLER_TYPE[CEffectCode::TOTAL_SAMPLER_TYPE][5] = {ALCHEMY_TEXT(""), ALCHEMY_TEXT("2d"), ALCHEMY_TEXT("3d"), ALCHEMY_TEXT("cube")};
const CHAR CFlashAGALAssembler::sm_cSAMPLER_FILTER[CEffectCode::TOTAL_SAMPLER_FILTER][8] = {ALCHEMY_TEXT("nearest"), ALCHEMY_TEXT(""), ALCHEMY_TEXT("linear"), ALCHEMY_TEXT(""), ALCHEMY_TEXT(""), ALCHEMY_TEXT(""), ALCHEMY_TEXT("")};
const CHAR CFlashAGALAssembler::sm_cSAMPLER_ADDRESS[CEffectCode::TOTAL_SAMPLER_ADDRESS_MODE][7] = {ALCHEMY_TEXT("wrap"),ALCHEMY_TEXT(""),ALCHEMY_TEXT("clamp"),ALCHEMY_TEXT(""),ALCHEMY_TEXT("")};

CFlashAGALAssembler::CFlashAGALAssembler(void)
{
}

CFlashAGALAssembler::~CFlashAGALAssembler(void)
{
}

bool CFlashAGALAssembler::ProgramString(PVOID* ppOutput, PUINT puOutputLength, CEffectInterface::EFFECTTYPE Type, const CHAR* pcSource, CBuffer** ppCompilationErrors)
{
#ifdef AS3_H
	AS3TYPE ByteArray = AS3_CallT(m_CompilerMethod, m_Object, "StrType, IntType", pcSource, Type);
	AS3_Acquire(ByteArray);
	*ppOutput = reinterpret_cast<PVOID>(ByteArray);
#endif

	if(puOutputLength)
		(*puOutputLength) = 0;

	return true;
}

CEffectInterface::EFFECTTYPE CFlashAGALAssembler::BuildShaderFunctionCode(STRING& Desination, UINT uShaderFunction, const CEffectCode& Code)
{
	const CEffectCode::FUNCTION* pFunction;

	if(uShaderFunction && ( pFunction = Code.GetFunction(uShaderFunction) ) != ALCHEMY_NULL)
	{
		const CEffectCode::SHADERFUNCTION* pShaderFunction;

		if(pFunction->uShader && ( pShaderFunction = Code.GetShaderFunction(pFunction->uShader) ) != ALCHEMY_NULL && pShaderFunction->Type != CEffectInterface::PROGRAM)
		{
			const STRING* pDesination;

			if(pFunction->uCode && ( pDesination = Code.GetCode(pFunction->uCode) ) != ALCHEMY_NULL)
			{
				Desination = *pDesination;

				return pShaderFunction->Type;
			}
			else
				ALCHEMY_DEBUG_WARNING("CFlashEffect::__BuildShaderCode: Error code.");
		}
		else
			ALCHEMY_DEBUG_WARNING("CFlashEffect::__BuildShaderCode: Error shader function.");
	}
	else
		ALCHEMY_DEBUG_WARNING("CFlashEffect::__BuildShaderCode: Error function.");

	return CEffectInterface::PROGRAM;
}

bool CFlashAGALAssembler::AppendInstruction(
	STRING& Destination, 
	CEffectInterface::EFFECTTYPE Type, 
	INSTRUCTION Instruction, 
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
	UINT uAddressCount2, 
	CEffectCode& EffectCode)
{
	UINT uDestinationRegister = 0, uDestinationIndexRegister = 0, uRegister1 = 0, uIndexRegister1 = 0, uRegister2 = 0, uIndexRegister2 = 0;
	REGISTERTYPE DestinationType, DestinationIndexType, SourceType1, SourceIndexType1, SourceType2, SourceIndexType2;
	const CEffectCode::VARIABLE* pVariable;
	const CEffectCode::SAMPLERDECLARATION* pSampler;

	if(uDestination && ( pVariable = EffectCode.GetVariable(uDestination) ) != ALCHEMY_NULL)
	{
		DestinationType = EffectCode.GetRegisterType(uDestination);

		if(DestinationType != CCompilerInterface::TEMPORARY && DestinationType != CCompilerInterface::RESULT && DestinationType != CCompilerInterface::OUTPUT)
		{
			ALCHEMY_DEBUG_WARNING("CFlashAGALAssembler::AppendInstruction: Destination must be temporary, result or output variable.");

			return false;
		}

		while(pVariable)
		{
			uDestinationRegister += pVariable->uRegister;

			pVariable = pVariable->uParent ? EffectCode.GetVariable(pVariable->uParent) : ALCHEMY_NULL;
		}

		if(uDestinationIndex && ( pVariable = EffectCode.GetVariable(uDestinationIndex) ) != ALCHEMY_NULL)
		{
			DestinationIndexType = EffectCode.GetRegisterType(uDestinationIndex);

			if(DestinationIndexType == CCompilerInterface::SAMPLER)
				DestinationIndexType = NONE;
			else
			{
				while(pVariable)
				{
					uDestinationIndexRegister += pVariable->uRegister;

					pVariable = pVariable->uParent ? EffectCode.GetVariable(pVariable->uParent) : ALCHEMY_NULL;
				}
			}
		}
		else
			DestinationIndexType = CCompilerInterface::NONE;
	}
	else
	{
		DestinationType = CCompilerInterface::NONE;

		return false;
	}

	if(uSource1 && ( pVariable = EffectCode.GetVariable(uSource1) ) != ALCHEMY_NULL)
	{
		SourceType1 = EffectCode.GetRegisterType(uSource1);

		if(SourceType1 == CCompilerInterface::SAMPLER)
		{
			ALCHEMY_DEBUG_WARNING("CFlashAGALAssembler::AppendInstruction: Source 1 can not be sampler.");

			return false;
		}

		while(pVariable)
		{
			uRegister1 += pVariable->uRegister;

			pVariable = pVariable->uParent ? EffectCode.GetVariable(pVariable->uParent) : ALCHEMY_NULL;
		}

		if(uSourceIndex1 && ( pVariable = EffectCode.GetVariable(uSourceIndex1) ) != ALCHEMY_NULL)
		{
			SourceIndexType1 = EffectCode.GetRegisterType(uSourceIndex1);

			if(SourceIndexType1 == CCompilerInterface::SAMPLER)
				SourceIndexType1 = NONE;
			else
			{
				while(pVariable)
				{
					uIndexRegister1 += pVariable->uRegister;

					pVariable = pVariable->uParent ? EffectCode.GetVariable(pVariable->uParent) : ALCHEMY_NULL;
				}
			}
		}
		else
			SourceIndexType1 = CCompilerInterface::NONE;
	}
	else
	{
		ALCHEMY_DEBUG_WARNING("CFlashAGALAssembler::AppendInstruction: Source 1 can not be none.");

		return false;
	}

	if(uSource2 && ( pVariable = EffectCode.GetVariable(uSource2) ) != ALCHEMY_NULL)
	{
		if(pVariable->Type == CEffectCode::SAMPLER_VARIABLE)
		{
			uRegister2  = pVariable->uRegister;
			SourceType2 = CCompilerInterface::SAMPLER;

			pSampler    = EffectCode.GetSamplerDeclaration(pVariable->uDeclaration);

			if(!pSampler)
			{
				ALCHEMY_DEBUG_WARNING("CFlashAGALAssembler::AppendInstruction: Error sampler.");

				return false;
			}

			SourceIndexType2 = CCompilerInterface::NONE;
		}
		else
		{
			SourceType2 = EffectCode.GetRegisterType(uSource2);

			while(pVariable)
			{
				uRegister2 += pVariable->uRegister;

				pVariable = pVariable->uParent ? EffectCode.GetVariable(pVariable->uParent) : ALCHEMY_NULL;
			}

			if(uSourceIndex2 && ( pVariable = EffectCode.GetVariable(uSourceIndex2) ) != ALCHEMY_NULL)
			{
				SourceIndexType2 = EffectCode.GetRegisterType(uSourceIndex2);

				if(SourceIndexType2 == CCompilerInterface::SAMPLER)
					SourceIndexType2 = NONE;
				else
				{
					while(pVariable)
					{
						uIndexRegister2 += pVariable->uRegister;

						pVariable = pVariable->uParent ? EffectCode.GetVariable(pVariable->uParent) : ALCHEMY_NULL;
					}
				}
			}
			else
				SourceIndexType2 = CCompilerInterface::NONE;
		}
	}
	else
	{
		SourceType2 = CCompilerInterface::NONE;

		SourceIndexType2 = CCompilerInterface::NONE;
	}

	if(!SourceType1)
	{
		ALCHEMY_DEBUG_WARNING("CFlashAGALAssembler::AppendInstruction: Source type 1 must not be none.");

		return false;
	}

	const CHAR* pcDestinationRegister, *pcDestinationIndexRegister, * pcRegister1, * pcIndexRegister1, * pcRegister2, * pcIndexRegister2;
	if(Type == CEffectInterface::VERTEX_SHADER)
	{
		pcDestinationRegister      = sm_cVERTEX_SHADER_REGISTER[DestinationType     ];
		pcDestinationIndexRegister = sm_cVERTEX_SHADER_REGISTER[DestinationIndexType];
		pcRegister1                = sm_cVERTEX_SHADER_REGISTER[SourceType1         ];
		pcIndexRegister1           = sm_cVERTEX_SHADER_REGISTER[SourceIndexType1    ];
		pcRegister2                = sm_cVERTEX_SHADER_REGISTER[SourceType2         ];
		pcIndexRegister2           = sm_cVERTEX_SHADER_REGISTER[SourceIndexType2    ];
	}
	else if(Type == CEffectInterface::FRAGMENT_SHADER)
	{
		pcDestinationRegister      = sm_cFRAGMENT_SHADER_REGISTER[DestinationType     ];
		pcDestinationIndexRegister = sm_cFRAGMENT_SHADER_REGISTER[DestinationIndexType];
		pcRegister1                = sm_cFRAGMENT_SHADER_REGISTER[SourceType1         ];
		pcIndexRegister1           = sm_cFRAGMENT_SHADER_REGISTER[SourceIndexType1    ];
		pcRegister2                = sm_cFRAGMENT_SHADER_REGISTER[SourceType2         ];
		pcIndexRegister2           = sm_cFRAGMENT_SHADER_REGISTER[SourceIndexType2    ];
	}
	else
	{
		/////////////////////////////////////////////////////////////
		////////////函数调用 未完成
		/////////////////////////////////////////////////////////////
	}

	Destination += sm_cAGALInstruction[Instruction];

	Destination += ALCHEMY_TEXT(' ');

	Destination += pcDestinationRegister;

	bool bIsAppendIndex;
	
	bIsAppendIndex = DestinationType != CCompilerInterface::OUTPUT;

	if(DestinationType)
	{
		if(DestinationIndexType == CCompilerInterface::NONE)
			__AppendRegister(Destination, uDestinationRegister, puDestinationAddress, uDestinationAddressCount, bIsAppendIndex);
		else
		{
			Destination += ALCHEMY_TEXT('[');
			Destination += pcDestinationIndexRegister;

			__AppendRegister(Destination, uDestinationRegister, uDestinationIndexRegister, uDestinationIndexAddress, puDestinationAddress, uDestinationAddressCount, bIsAppendIndex);
		}

		Destination += ALCHEMY_TEXT(' ');

		Destination += pcRegister1;
	}

	UINT uAddress[4], uRegister = (uDestinationRegister >> 2) & 3, uRegisterCount = uRegister + uDestinationAddressCount;

	bIsAppendIndex = SourceType1 != CCompilerInterface::OUTPUT;

	if(uDestinationAddressCount == uAddressCount1 && uRegister && uRegisterCount < 5)
	{
		UINT i;
		for(i = 0; i < uRegister; ++ i)
			uAddress[i] = puAddress1[0];

		for(i = 0; i < uDestinationAddressCount; i ++)
			uAddress[i + uRegister] = puAddress1[i < uAddressCount1 ? i : 0];

		puAddress1 = uAddress;
		uAddressCount1 = uRegisterCount;
	}

	if(SourceIndexType1 == CCompilerInterface::NONE)
		__AppendRegister(Destination, uRegister1, puAddress1, uAddressCount1, bIsAppendIndex);
	else
	{
		Destination += ALCHEMY_TEXT('[');
		Destination += pcIndexRegister1;

		__AppendRegister(Destination, uRegister1, uIndexRegister1, uSourceIndexAddress1, puAddress1, uAddressCount1, bIsAppendIndex);
	}


	if(SourceType2)
	{
		Destination += ALCHEMY_TEXT(' ');

		Destination += pcRegister2;

		if(SourceType2 == CCompilerInterface::SAMPLER)
		{
			Destination += uRegister2;

			__AppendSampler(Destination, *pSampler);
		}
		else
		{
			bIsAppendIndex = SourceType2 != CCompilerInterface::OUTPUT;

			if(uDestinationAddressCount == uAddressCount2 && uRegister && uRegisterCount < 5)
			{
				UINT i;
				for(i = 0; i < uRegister; ++ i)
					uAddress[i] = puAddress2[0];

				for(i = 0; i < uDestinationAddressCount; ++ i)
					uAddress[i + uRegister] = puAddress2[i < uAddressCount2 ? i : 0];

				puAddress2 = uAddress;
				uAddressCount2 = uRegisterCount;
			}

			if(SourceIndexType2 == CCompilerInterface::NONE)
				__AppendRegister(Destination, uRegister2, puAddress2, uAddressCount2, true);
			else
			{
				Destination += ALCHEMY_TEXT('[');
				Destination += pcIndexRegister2;

				__AppendRegister(Destination, uRegister2, uIndexRegister2, uSourceIndexAddress2, puAddress2, uAddressCount2, bIsAppendIndex);
			}
		}
	}

	Destination += ALCHEMY_TEXT('\n');

	return true;
}

#ifdef AS3_H
AS3TYPE CFlashAGALAssembler::Init()
{

}

void CFlashAGALAssembler::Fill(AS3TYPE Object, AS3TYPE MethodArray)
{
	m_Object = Object;

	AS3_ArrayValue(MethodArray, "AS3ValType", &m_CompilerMethod);
}
#endif

void CFlashAGALAssembler::__AppendSampler(STRING& Destiantion, const CEffectCode::SAMPLERDECLARATION& Sampler)
{
	//Destiantion += ALCHEMY_TEXT("<cube,linear,clamp,miplinear>");
	Destiantion += ALCHEMY_TEXT( "<" );
	Destiantion += sm_cSAMPLER_TYPE[ Sampler.Type ];
	Destiantion += ALCHEMY_TEXT( "," );
	Destiantion += sm_cSAMPLER_FILTER[Sampler.MagFilter];
	Destiantion += ALCHEMY_TEXT( "," );
	Destiantion += sm_cSAMPLER_ADDRESS[Sampler.AddressU];
	
	Destiantion += ALCHEMY_TEXT( ">" );
}

void CFlashAGALAssembler::__AppendRegister(STRING& Destiantion, UINT uRegister, const UINT* puAddress, UINT uAddressCount, bool bIsAppendIndex)
{
	uRegister >>= 2;

	UINT uTemp = uRegister >> 2;

	if(bIsAppendIndex)
		Destiantion += uTemp;

	uTemp = uRegister - (uTemp << 2);

	if(uTemp)
	{
		Destiantion += ALCHEMY_TEXT('.');
		if( puAddress && uAddressCount && uAddressCount < 5)
		{
			UINT uAddress;
			for(UINT i = 0; i < uAddressCount; ++ i)
			{
				uAddress = puAddress[i] + uTemp;
				if(uAddress < 5)
					Destiantion += sm_cREGISTER_ADDRESS[puAddress[i] + uTemp];
			}
		}
		else
			Destiantion += sm_cREGISTER_ADDRESS[uTemp];
	}
	else if(puAddress && uAddressCount && uAddressCount < 5)
	{
		Destiantion += ALCHEMY_TEXT('.');

		UINT uAddress;
		for(UINT i = 0; i < uAddressCount; ++ i)
		{
			uAddress = puAddress[i];
			if(uAddress < 5)
				Destiantion += sm_cREGISTER_ADDRESS[puAddress[i]];
		}
	}
}

void CFlashAGALAssembler::__AppendRegister(STRING& Destiantion, UINT uRegister, UINT uIndexRegister, UINT uIndexAddress, const UINT* puAddress, UINT uAddressCount, bool bIsAppendIndex)
{
	uIndexRegister >>= 2;

	UINT uCurrentIndexRegister = uIndexRegister >> 2;

	Destiantion += uCurrentIndexRegister;

	UINT uTemp = uIndexRegister - (uCurrentIndexRegister << 2);

	Destiantion += ALCHEMY_TEXT('.');
	Destiantion += sm_cREGISTER_ADDRESS[uTemp ? uTemp : uIndexAddress];

	Destiantion += ALCHEMY_TEXT('+');

	uRegister >>= 2;

	UINT uCurrentRegister = uRegister >> 2;

	Destiantion += uCurrentRegister;

	Destiantion += ALCHEMY_TEXT(']');

	uTemp = uRegister - (uCurrentRegister << 2);

	if(uTemp)
	{
		Destiantion += ALCHEMY_TEXT('.');
		if( puAddress && uAddressCount && uAddressCount < 5)
		{
			UINT uAddress;
			for(UINT i = 0; i < uAddressCount; ++ i)
			{
				uAddress = puAddress[i] + uTemp;
				if(uAddress < 5)
					Destiantion += sm_cREGISTER_ADDRESS[puAddress[i] + uTemp];
			}
		}
		else
			Destiantion += sm_cREGISTER_ADDRESS[uTemp];
	}
	else if(puAddress && uAddressCount && uAddressCount < 5)
	{
		Destiantion += ALCHEMY_TEXT('.');

		UINT uAddress;
		for(UINT i = 0; i < uAddressCount; ++ i)
		{
			uAddress = puAddress[i];
			if(uAddress < 5)
				Destiantion += sm_cREGISTER_ADDRESS[puAddress[i]];
		}
	}
}