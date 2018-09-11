#include "AlchemyFlasccStdafx.h"
#include "AlchemyFlasccAGALAssembler.h"

using namespace alchemy;

const CHAR* CFlasccAGALAssembler::sm_pcAGAL_INSTRUCTION[ICompiler::TOTAL_INSTRUCTION] = 
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

const CHAR* CFlasccAGALAssembler::sm_pcVERTEX_SHADER_REGISTER[ICompiler::TOTAL_REGISTERTYPE]   = 
{
	ALCHEMY_TEXT(""), 
	ALCHEMY_TEXT("vc"), 
	ALCHEMY_TEXT("vt"), 
	ALCHEMY_TEXT("va"), 
	ALCHEMY_TEXT("v"),
	ALCHEMY_TEXT("op"), 
	ALCHEMY_TEXT("") 
};

const CHAR* CFlasccAGALAssembler::sm_pcFRAGMENT_SHADER_REGISTER[ICompiler::TOTAL_REGISTERTYPE] = {ALCHEMY_TEXT(""), ALCHEMY_TEXT("fc"), ALCHEMY_TEXT("ft"), ALCHEMY_TEXT("v") , ALCHEMY_TEXT("") , ALCHEMY_TEXT("oc"), ALCHEMY_TEXT("fs")};
const CHAR* CFlasccAGALAssembler::sm_pcREGISTER_ADDRESS = ALCHEMY_TEXT("xyzw");


const CHAR* CFlasccAGALAssembler::sm_pcSAMPLER_TYPE[IEffect::TOTAL_SAMPLER_TYPE] = 
{
	ALCHEMY_TEXT("2d"), 
	ALCHEMY_TEXT("3d"), 
	ALCHEMY_TEXT("cube")
};
const CHAR* CFlasccAGALAssembler::sm_pcSAMPLER_FILTER[IEffect::TOTAL_SAMPLER_FILTER] = 
{
	ALCHEMY_NULL, 
	ALCHEMY_TEXT("nearest"), 
	ALCHEMY_TEXT("linear")
};

const CHAR* CFlasccAGALAssembler::sm_pcSAMPLER_MIPMAP_FILTER[IEffect::TOTAL_SAMPLER_FILTER] = 
{
	ALCHEMY_NULL, 
	ALCHEMY_TEXT("mipnearest"), 
	ALCHEMY_TEXT("miplinear")
};

const CHAR* CFlasccAGALAssembler::sm_pcSAMPLER_ADDRESS[IEffect::TOTAL_SAMPLER_ADDRESS_MODE] = 
{
	ALCHEMY_TEXT("wrap"),
	ALCHEMY_TEXT("repeat"),
	ALCHEMY_TEXT("clamp")
};

CFlasccAGALAssembler::CFlasccAGALAssembler(void)
{
}

CFlasccAGALAssembler::~CFlasccAGALAssembler(void)
{
}

bool CFlasccAGALAssembler::ProgramString(CBuffer** ppOutput, IEffect::EFFECTTYPE Type, const CHAR* pcSource)
{
#ifdef AS3_H
	UINT uCodeIndex;
	if(Type < IEffect::PROGRAM)
	{
		inline_as3(
			"import alchemy.core.AlchemyGraphicsManager;\n"
			"%0 = AlchemyGraphicsManager.instance.compileAGALShaderSource(%1, %2, %3);\n"
			: "=r"(uCodeIndex) : "r"(Type == IEffect::VERTEX_SHADER ? true : false), "r"(pcSource), "r"( ALCHEMY_STRING_LENGTH(pcSource) )
			);

		CBuffer& Buffer = CreateBuffer( sizeof(UINT) );
		Buffer.CopyFrom(&uCodeIndex);
		if(ppOutput)
			*ppOutput = &Buffer;

		return true;
	}

	return false;
#endif

	return true;
}

bool CFlasccAGALAssembler::BuildShaderFunctionCode(STRING& Destination, IEffect::EFFECTTYPE Type, UINT uInstructionIndices, UINT uUserVariables, const CEffectCode& Code)
{
	const UINT* puInstructionIndices = Code.GetInstructionIndices(uInstructionIndices);
	const CEffectCode::INSTRUCTIONINDEX* pInstructionIndex = puInstructionIndices ? Code.GetInstructionIndex(*puInstructionIndices) : ALCHEMY_NULL;
	if(pInstructionIndex)
	{
		UINT uDestinationAddress[4], uSourceAddress1[4], uSourceAddress2[4];
		UINT uDestinationAddressCount, uSourceAddressCount1, uSourceAddressCount2;
		const UINT* puDestinationAddress, * puSourceAddress1, * puSourceAddress2;
		const CSmartPointer<CBuffer>* pAddress;
		const CEffectCode::INSTRUCTION* pInstruction;
		while(pInstructionIndex)
		{
			pInstruction = Code.GetInstruction(pInstructionIndex->uCurrentInstruction);
			if(pInstruction)
			{
				pAddress = Code.GetBuffer(pInstruction->uDestinationAddressInfo);
				if(pAddress)
				{
					puDestinationAddress     = reinterpret_cast<const UINT*>( (*pAddress)->GetBufferPointer() );
					uDestinationAddressCount = (*pAddress)->GetBufferSize() / sizeof(UINT);
				}
				else
				{
					if(pInstruction->uDestinationAddress == ~0)
					{
						puDestinationAddress     = ALCHEMY_NULL;
						uDestinationAddressCount = 0;
					}
					else
					{
						CEffectCode::UnserializeAddress(pInstruction->uDestinationAddress & 3, static_cast<PUINT>(uDestinationAddress), &uDestinationAddressCount);
						puDestinationAddress = uDestinationAddress;
					}
				}

				pAddress = Code.GetBuffer(pInstruction->uSourceAddressInfo1);
				if(pAddress)
				{
					puSourceAddress1     = reinterpret_cast<const UINT*>( (*pAddress)->GetBufferPointer() );
					uSourceAddressCount1 = (*pAddress)->GetBufferSize() / sizeof(UINT);
				}
				else
				{
					if(pInstruction->uSourceAddress1 == ~0)
					{
						puSourceAddress1     = ALCHEMY_NULL;
						uSourceAddressCount1 = 0;
					}
					else
					{
						CEffectCode::UnserializeAddress(pInstruction->uSourceAddress1 & 3, static_cast<PUINT>(uSourceAddress1), &uSourceAddressCount1);
						puSourceAddress1 = uSourceAddress1;
					}
				}

				pAddress = Code.GetBuffer(pInstruction->uSourceAddressInfo2);
				if(pAddress)
				{
					puSourceAddress2     = reinterpret_cast<const UINT*>( (*pAddress)->GetBufferPointer() );
					uSourceAddressCount2 = (*pAddress)->GetBufferSize() / sizeof(UINT);
				}
				else
				{
					if(pInstruction->uSourceAddress2 == ~0)
					{
						puSourceAddress2     = ALCHEMY_NULL;
						uSourceAddressCount2 = 0;
					}
					else
					{
						CEffectCode::UnserializeAddress(pInstruction->uSourceAddress2 & 3, static_cast<PUINT>(uSourceAddress2), &uSourceAddressCount2);
						puSourceAddress2 = uSourceAddress2;
					}
				}

				__AppendInstruction(
					Destination, 
					pInstruction->Opcode, 
					pInstruction->uDestination, 
					pInstruction->uDestinationIndex, 
					pInstruction->uDestinationIndexAddress, 
					puDestinationAddress, 
					uDestinationAddressCount, 
					pInstruction->uSource1, 
					pInstruction->uSourceIndex1,
					pInstruction->uSourceIndexAddress1, 
					puSourceAddress1, 
					uSourceAddressCount1, 
					pInstruction->uSource2, 
					pInstruction->uSourceIndex2,
					pInstruction->uSourceIndexAddress2, 
					puSourceAddress2, 
					uSourceAddressCount2, 
					Code);
			}

			pInstructionIndex = Code.GetInstructionIndex(pInstructionIndex->uNextIndex);
		}

		return true;
	}

	return false;
}

bool CFlasccAGALAssembler::AppendRegisterName(STRING& Destination, UINT uRegister, ICompiler::REGISTERTYPE RegisterType, IEffect::EFFECTTYPE ShaderType)const
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

bool CFlasccAGALAssembler::__AppendInstruction(
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
	const CEffectCode& EffectCode)
{
	Destination += sm_pcAGAL_INSTRUCTION[Instruction];

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

void CFlasccAGALAssembler::__AppendVariable(STRING& Destination, UINT uVariable, UINT uVariableIndex, UINT uIndexAddress, const UINT* puAddress, UINT uAddressCount, UINT uMinDestinationAddress, const CEffectCode& EffectCode)
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

void CFlasccAGALAssembler::__AppendSampler(STRING& Destiantion, const IEffect::SAMPLERDECLARATION& Sampler)
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

void CFlasccAGALAssembler::__AppendRegister(STRING& Destiantion, UINT uRegister, const UINT* puAddress, UINT uAddressCount, bool bIsAppendIndex)
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

void CFlasccAGALAssembler::__AppendRegister(STRING& Destiantion, UINT uRegister, UINT uIndexRegister, UINT uIndexAddress, const UINT* puAddress, UINT uAddressCount, bool bIsAppendIndex)
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