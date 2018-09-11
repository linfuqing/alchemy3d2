#include "AlchemyFlashStdafx.h"
#include "AlchemyFlashAGALAssembler.h"

using namespace alchemy;

const CHAR* CFlashAGALAssembler::sm_pcAGALInstruction[ICompiler::TOTAL_INSTRUCTION] = 
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
	ALCHEMY_TEXT("kil"),
	ALCHEMY_TEXT("tex"),
	ALCHEMY_TEXT("sge"),
	ALCHEMY_TEXT("slt"),
	ALCHEMY_TEXT("sgn")
};

const CHAR* CFlashAGALAssembler::sm_pcVERTEX_SHADER_REGISTER[ICompiler::TOTAL_REGISTERTYPE]   = 
{
	ALCHEMY_TEXT(""), 
	ALCHEMY_TEXT("vc"), 
	ALCHEMY_TEXT("vt"), 
	ALCHEMY_TEXT("va"), 
	ALCHEMY_TEXT("v"),
	ALCHEMY_TEXT("op"), 
	ALCHEMY_TEXT("") 
};

const CHAR* CFlashAGALAssembler::sm_pcFRAGMENT_SHADER_REGISTER[ICompiler::TOTAL_REGISTERTYPE] = {ALCHEMY_TEXT(""), ALCHEMY_TEXT("fc"), ALCHEMY_TEXT("ft"), ALCHEMY_TEXT("v") , ALCHEMY_TEXT("") , ALCHEMY_TEXT("oc"), ALCHEMY_TEXT("fs")};
const CHAR* CFlashAGALAssembler::sm_pcREGISTER_ADDRESS = ALCHEMY_TEXT("xyzw");


const CHAR* CFlashAGALAssembler::sm_pcSAMPLER_TYPE[IEffect::TOTAL_SAMPLER_TYPE] = 
{
	ALCHEMY_TEXT("2d"), 
	ALCHEMY_TEXT("3d"), 
	ALCHEMY_TEXT("cube")
};
const CHAR* CFlashAGALAssembler::sm_pcSAMPLER_FILTER[IEffect::TOTAL_SAMPLER_FILTER] = 
{
	ALCHEMY_NULL, 
	ALCHEMY_TEXT("nearest"), 
	ALCHEMY_TEXT("linear")
};

const CHAR* CFlashAGALAssembler::sm_pcSAMPLER_MIPMAP_FILTER[IEffect::TOTAL_SAMPLER_FILTER] = 
{
	ALCHEMY_NULL, 
	ALCHEMY_TEXT("mipnearest"), 
	ALCHEMY_TEXT("miplinear")
};

const CHAR* CFlashAGALAssembler::sm_pcSAMPLER_ADDRESS[IEffect::TOTAL_SAMPLER_ADDRESS_MODE] = 
{
	ALCHEMY_TEXT("wrap"),
	ALCHEMY_TEXT("repeat"),
	ALCHEMY_TEXT("clamp")
};

CFlashAGALAssembler::CFlashAGALAssembler(void)
{
}

CFlashAGALAssembler::~CFlashAGALAssembler(void)
{
}

bool CFlashAGALAssembler::ProgramString(CBuffer** ppOutput, IEffect::EFFECTTYPE Type, const CHAR* pcSource)
{
#ifdef AS3_H
	AS3TYPE ByteArray = AS3_CallT(m_CompilerMethod, m_Object, "StrType, IntType", pcSource, Type);
	AS3_Acquire(ByteArray);
	CBuffer& Buffer = CreateBuffer( sizeof(AS3TYPE) );
	Buffer.CopyFrom(&ByteArray, sizeof(AS3TYPE) );
	if(ppOutput)
		*ppOutput = &Buffer;
#endif

	return true;
}

bool CFlashAGALAssembler::BuildShaderFunctionCode(STRING& Desination, IEffect::EFFECTTYPE Type, UINT uUserVariables, UINT uCode, const CEffectCode& Code)
{
	const STRING* pCode = Code.GetCode(uCode);

	if(pCode)
	{
		Desination = *pCode;

		return true;
	}

	return false;
}

bool CFlashAGALAssembler::AppendInstruction(
	STRING& Destination, 
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
	Destination += sm_pcAGALInstruction[Instruction];

	Destination += ALCHEMY_TEXT(' ');

	UINT uMinDestiantionAddress = 0;

	if(uDestination)
	{
		__AppendVariable(Destination, uDestination, uDestinationIndex, uDestinationIndexAddress, puDestinationAddress, uDestinationAddressCount, 0, EffectCode);

		if(uDestinationAddressCount > 1)
		{
			if(puDestinationAddress)
			{
				uMinDestiantionAddress = puDestinationAddress[0];
				for(UINT i = 1; i < uDestinationAddressCount; ++ i)
					uMinDestiantionAddress = ALCHEMY_MININUM(uMinDestiantionAddress, puDestinationAddress[i]);
			}

			const CEffectCode::VARIABLE* pVariable = EffectCode.GetVariable(uDestination);
			UINT uRegister = 0;
			while(pVariable)
			{
				uRegister += pVariable->uRegister;

				pVariable = pVariable->uParent ? EffectCode.GetVariable(pVariable->uParent) : ALCHEMY_NULL;
			}

			uMinDestiantionAddress += (uRegister >> 2) & 3;
		}

		Destination += ALCHEMY_TEXT(' ');
	}

	__AppendVariable(Destination, uSource1, uSourceIndex1, uSourceIndexAddress1, puAddress1, uAddressCount1, uMinDestiantionAddress, EffectCode);

	Destination += ALCHEMY_TEXT(' ');

	__AppendVariable(Destination, uSource2, uSourceIndex2, uSourceIndexAddress2, puAddress2, uAddressCount2, uMinDestiantionAddress, EffectCode);

	Destination += ALCHEMY_TEXT('\n');

	return true;
}

bool CFlashAGALAssembler::ReplaceVariable(
					 STRING& Code, 
					 UINT uSource, 
					 const UINT* puSourceAddress, 
					 UINT uDestiantion, 
					 const UINT* puDestiantionAddress, 
					 UINT uAddressCount, 
					 UINT uMinDestinationAddress, 
					 const CEffectCode& EffectCode, 
					 UINT uCodeFrom, 
					 UINT uCodeLength)
{
	STRING Source, Destination;
	uMinDestinationAddress = uCodeLength < 2 ? 0 : uMinDestinationAddress;
	__AppendVariable(Source     , uSource     , 0, 0, puSourceAddress     , uAddressCount, uMinDestinationAddress, EffectCode);
	__AppendVariable(Destination, uDestiantion, 0, 0, puDestiantionAddress, uAddressCount, uMinDestinationAddress, EffectCode);

	if( Source.GetLength() && Destination.GetLength() )
	{
		UINT uIndex = Code.GetIndexOf(Source, uCodeFrom, uCodeLength), uLength = Source.GetLength(), uFinalIndex = uCodeLength;
		while(uIndex < uCodeLength)
		{
			uFinalIndex = uIndex;
			uIndex = Code.GetIndexOf(Source, uIndex + uLength, uCodeLength);
		}

		if(uFinalIndex < uCodeLength)
		{
			Code.Replace(uFinalIndex, uLength, Destination);

			return true;
		}
	}

	return false; 
}

bool CFlashAGALAssembler::AppendRegisterName(STRING& Destination, UINT uRegister, ICompiler::REGISTERTYPE RegisterType, IEffect::EFFECTTYPE ShaderType)const
{
	if(ShaderType == IEffect::VERTEX_SHADER)
		Destination += sm_pcVERTEX_SHADER_REGISTER[RegisterType];
	else if(ShaderType == IEffect::FRAGMENT_SHADER)
		Destination += sm_pcFRAGMENT_SHADER_REGISTER[RegisterType];
	else 
	{
		///...
	}

	Destination += uRegister >> 4;
	
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

void CFlashAGALAssembler::__AppendVariable(STRING& Destination, UINT uVariable, UINT uVariableIndex, UINT uIndexAddress, const UINT* puAddress, UINT uAddressCount, UINT uMinDestinationAddress, const CEffectCode& EffectCode)
{
	ICompiler::REGISTERTYPE Type = EffectCode.GetRegisterType(uVariable);
	if(Type)
	{
		const CEffectCode::VARIABLE* pVariable = EffectCode.GetVariable(uVariable);
		if(pVariable)
		{
			IEffect::EFFECTTYPE ShaderType = IEffect::PROGRAM;
			const CEffectCode::FUNCTION* pFunction = EffectCode.GetFunction(pVariable->uFunction);
			if(pFunction)
			{
				const CEffectCode::SHADERFUNCTION* pShaderFunction = EffectCode.GetShaderFunction(pFunction->uShader);
				if(pShaderFunction)
					ShaderType = pShaderFunction->Type;
			}
			else
			{
				const CEffectCode::GLOBALVARIABLE* pGlobalVariable = EffectCode.GetGlobalVariable(pVariable->uGlobal);
				if(pGlobalVariable)
					ShaderType = pGlobalVariable->ShaderType;
			}

			if(ShaderType == IEffect::VERTEX_SHADER)
			{
				Destination += sm_pcVERTEX_SHADER_REGISTER[Type];
			}
			else if(ShaderType == IEffect::FRAGMENT_SHADER)
			{
				Destination += sm_pcFRAGMENT_SHADER_REGISTER[Type];
			}
			else
			{
				/////////////////////////////////////////////////////////////
				////////////函数调用 未完成
				/////////////////////////////////////////////////////////////
			}

			if(Type == ICompiler::SAMPLER)
			{
				const IEffect::SAMPLERDECLARATION* pSampler = EffectCode.GetSamplerDeclaration(pVariable->uDeclaration);
				if(pSampler)
				{
					Destination += pVariable->uRegister;

					__AppendSampler(Destination, *pSampler);
				}
			}
			else
			{
				UINT uRegister = 0;
				while(pVariable)
				{
					uRegister += pVariable->uRegister;

					pVariable = EffectCode.GetVariable(pVariable->uParent);
				}

				UINT uAddress[4];
				bool bIsAppendIndex = true;
				if(Type == ICompiler::OUTPUT)
					bIsAppendIndex = false;
				else if(uMinDestinationAddress > 0 && uMinDestinationAddress < 4 && puAddress && uAddressCount && uAddressCount < 3)
				{
					UINT uRegisterCount = uMinDestinationAddress + uAddressCount;

					if(uRegisterCount == 5)
					{
						uAddressCount  = 1;
						uRegisterCount = 4;
					}

					UINT i;
					for(i = 0; i < uMinDestinationAddress; ++ i)
						uAddress[i] = puAddress[0];

					for(i = 0; i < uAddressCount; ++ i)
						uAddress[i + uMinDestinationAddress] = puAddress[i];

					puAddress = uAddress;
					uAddressCount = uRegisterCount;
				}

				Type = EffectCode.GetRegisterType(uVariableIndex);
				if(Type)
				{
					pVariable = EffectCode.GetVariable(uVariableIndex);
					if(pVariable)
					{
						Destination += ALCHEMY_TEXT('[');

						ShaderType = IEffect::PROGRAM;
						const CEffectCode::FUNCTION* pFunction = EffectCode.GetFunction(pVariable->uFunction);
						if(pFunction)
						{
							const CEffectCode::SHADERFUNCTION* pShaderFunction = EffectCode.GetShaderFunction(pFunction->uShader);
							if(pShaderFunction)
								ShaderType = pShaderFunction->Type;
						}

						if(ShaderType == IEffect::VERTEX_SHADER)
						{
							Destination += sm_pcVERTEX_SHADER_REGISTER[Type];
						}
						else if(ShaderType == IEffect::FRAGMENT_SHADER)
						{
							Destination += sm_pcFRAGMENT_SHADER_REGISTER[Type];
						}
						else
						{
							/////////////////////////////////////////////////////////////
							////////////函数调用 未完成
							/////////////////////////////////////////////////////////////
						}

						UINT uIndexRegister = 0;
						while(pVariable)
						{
							uIndexRegister += pVariable->uRegister;

							pVariable = EffectCode.GetVariable(pVariable->uParent);
						}

						
						__AppendRegister(Destination, uRegister, uIndexRegister, uIndexAddress, puAddress, uAddressCount, bIsAppendIndex);
					}
					else
						__AppendRegister(Destination, uRegister, puAddress, uAddressCount, bIsAppendIndex);
				}
				else
					__AppendRegister(Destination, uRegister, puAddress, uAddressCount, bIsAppendIndex);
			}
		}
	}
}

void CFlashAGALAssembler::__AppendSampler(STRING& Destiantion, const IEffect::SAMPLERDECLARATION& Sampler)
{
	//Destiantion += ALCHEMY_TEXT("<cube,linear,clamp,miplinear>");
	Destiantion += ALCHEMY_TEXT('<');
	Destiantion += sm_pcSAMPLER_TYPE[Sampler.Type];
	Destiantion += ALCHEMY_TEXT(", ");
	if(Sampler.MagFilter)
	{
		Destiantion += sm_pcSAMPLER_FILTER[Sampler.MagFilter];
		Destiantion += ALCHEMY_TEXT(", ");
	}

	if(Sampler.MipFilter)
	{
		Destiantion += sm_pcSAMPLER_MIPMAP_FILTER[Sampler.MipFilter];
		Destiantion += ALCHEMY_TEXT(", ");
	}

	Destiantion += sm_pcSAMPLER_ADDRESS[Sampler.AddressU];

	Destiantion += ALCHEMY_TEXT('>');
}

void CFlashAGALAssembler::__AppendRegister(STRING& Destiantion, UINT uRegister, const UINT* puAddress, UINT uAddressCount, bool bIsAppendIndex)
{
	uRegister >>= 2;

	UINT uTemp = uRegister >> 2;

	if(bIsAppendIndex)
		Destiantion += uTemp;

	uTemp = uRegister - (uTemp << 2);

	uAddressCount = ALCHEMY_MININUM(4u, uAddressCount);
	if(uTemp)
	{
		Destiantion += ALCHEMY_TEXT('.');
		if( puAddress && uAddressCount)
		{
			UINT uAddress;
			for(UINT i = 0; i < uAddressCount; ++ i)
			{
				uAddress = puAddress[i] + uTemp;
				if(uAddress < 4)
					Destiantion += sm_pcREGISTER_ADDRESS[uAddress];
			}
		}
		else
			Destiantion += sm_pcREGISTER_ADDRESS[uTemp];
	}
	else if(puAddress && uAddressCount)
	{
		Destiantion += ALCHEMY_TEXT('.');

		UINT uAddress;
		for(UINT i = 0; i < uAddressCount; ++ i)
		{
			uAddress = puAddress[i];
			if(uAddress < 4)
				Destiantion += sm_pcREGISTER_ADDRESS[uAddress];
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
	Destiantion += sm_pcREGISTER_ADDRESS[uTemp ? uTemp : uIndexAddress];

	Destiantion += ALCHEMY_TEXT('+');

	uRegister >>= 2;

	UINT uCurrentRegister = uRegister >> 2;

	Destiantion += uCurrentRegister;

	Destiantion += ALCHEMY_TEXT(']');

	uTemp = uRegister - (uCurrentRegister << 2);

	uAddressCount = ALCHEMY_MININUM(4u, uAddressCount);
	if(uTemp)
	{
		Destiantion += ALCHEMY_TEXT('.');
		if( puAddress && uAddressCount)
		{
			UINT uAddress;
			for(UINT i = 0; i < uAddressCount; ++ i)
			{
				uAddress = puAddress[i] + uTemp;
				if(uAddress < 4)
					Destiantion += sm_pcREGISTER_ADDRESS[uAddress];
			}
		}
		else
			Destiantion += sm_pcREGISTER_ADDRESS[uTemp];
	}
	else if(puAddress && uAddressCount)
	{
		Destiantion += ALCHEMY_TEXT('.');

		UINT uAddress;
		for(UINT i = 0; i < uAddressCount; ++ i)
		{
			uAddress = puAddress[i];
			if(uAddress < 4)
				Destiantion += sm_pcREGISTER_ADDRESS[uAddress];
		}
	}
}