#include "AlchemyHLSLCompiler.h"

#include <d3dx9.h>

#pragma comment(lib,"d3dx9.lib")

using namespace alchemy;

const alchemy::CHAR* CHLSLCompiler::sm_pcInstruction[ICompiler::TOTAL_INSTRUCTION] = 
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
	ALCHEMY_TEXT("m3x3"),
	ALCHEMY_TEXT("m4x4"),
	ALCHEMY_TEXT("m3x4"),
	ALCHEMY_TEXT("texkill"),
	ALCHEMY_TEXT("texld"),
	ALCHEMY_TEXT("sge"),
	ALCHEMY_TEXT("slt"),
	ALCHEMY_TEXT("sgn")
};

const alchemy::CHAR* CHLSLCompiler::sm_pcVERTEX_SHADER_REGISTER[ICompiler::TOTAL_REGISTERTYPE]   = 
{
	ALCHEMY_TEXT(""), 
	ALCHEMY_TEXT("c"), 
	ALCHEMY_TEXT("r"), 
	ALCHEMY_TEXT("v"), 
	ALCHEMY_TEXT("oT"),
	ALCHEMY_TEXT("oPos"), 
	ALCHEMY_TEXT("") 
};

const alchemy::CHAR* CHLSLCompiler::sm_pcFRAGMENT_SHADER_REGISTER[ICompiler::TOTAL_REGISTERTYPE] = 
{
	ALCHEMY_TEXT(""), 
	ALCHEMY_TEXT("c"), 
	ALCHEMY_TEXT("r"),
	ALCHEMY_TEXT("t") ,
	ALCHEMY_TEXT("") , 
	ALCHEMY_TEXT("r"), 
	ALCHEMY_TEXT("s")
};

const alchemy::CHAR* CHLSLCompiler::sm_pcREGISTER_ADDRESS = ALCHEMY_TEXT("xyzw");

const alchemy::CHAR* CHLSLCompiler::sm_pcSAMPLER_TYPE[IEffect::TOTAL_SAMPLER_TYPE] = 
{
	ALCHEMY_TEXT("2d"), 
	ALCHEMY_TEXT("volume"), 
	ALCHEMY_TEXT("cube")
};

CHLSLCompiler::CHLSLCompiler(alchemy::UINT uOutputColorCount, alchemy::UINT uConstantBufferSize)
{
	m_CompilerInfo.uByteAlignmentShift = 4;
	m_CompilerInfo.uOutputColorCount = uOutputColorCount;
	m_CompilerInfo.uMaxVertexConstantBufferSize = uConstantBufferSize;
	m_CompilerInfo.uMaxFragmentConstantBufferSize = uConstantBufferSize;
	m_CompilerInfo.bIsSupportConstantBuffer = false;
}

CHLSLCompiler::~CHLSLCompiler(void)
{
}

bool CHLSLCompiler::ProgramString(CBuffer** ppOutput, IEffect::EFFECTTYPE Type, const alchemy::CHAR* pcSource)
{
	if(!ppOutput)
		return false;

	LPD3DXBUFFER pShader = ALCHEMY_NULL;

#ifdef _UNICODE
	wprintf(pcSource);

	UINT uLength = WideCharToMultiByte( 
		CP_ACP,
		0,
		pcSource,
		- 1,
		ALCHEMY_NULL,
		0,
		ALCHEMY_NULL,
		ALCHEMY_NULL);

	LPSTR pcResult;
	ALCHEMY_DEBUG_NEW(pcResult, char[uLength + 1]);

	WideCharToMultiByte(
		CP_ACP,
		0,
		pcSource,
		- 1,
		pcResult,
		uLength,
		ALCHEMY_NULL,
		ALCHEMY_NULL);

#ifdef _DEBUG
	LPD3DXBUFFER pErrorMsgs = ALCHEMY_NULL;
	if(D3DXAssembleShader(pcResult, uLength, ALCHEMY_NULL, ALCHEMY_NULL, D3DXSHADER_DEBUG, &pShader, &pErrorMsgs) != D3D_OK)
	{
		if(pErrorMsgs)
		{
			ALCHEMY_DEBUG_WARNING( reinterpret_cast<char*>( pErrorMsgs->GetBufferPointer() ) );

			printf( reinterpret_cast<char*>( pErrorMsgs->GetBufferPointer() ) );

			pErrorMsgs->Release();
		}

		return false;
	}
#else
	if(D3DXAssembleShader(pcResult, uLength, ALCHEMY_NULL, ALCHEMY_NULL, 0, &pShader, ALCHEMY_NULL) != D3D_OK)
		return false;
#endif // _DEBUG

	ALCHEMY_DEBUG_DELETE_ARRAY(pcResult);
#else
	printf(pcSource);

#ifdef _DEBUG
	LPD3DXBUFFER pErrorMsgs = ALCHEMY_NULL;
	if(D3DXAssembleShader(pcSource, ALCHEMY_STRING_LENGTH(pcSource), ALCHEMY_NULL, ALCHEMY_NULL, D3DXSHADER_DEBUG, &pShader, &pErrorMsgs) != D3D_OK)
	{
		if(pErrorMsgs)
		{
			ALCHEMY_DEBUG_WARNING( reinterpret_cast<char*>( pErrorMsgs->GetBufferPointer() ) );

			printf( reinterpret_cast<char*>( pErrorMsgs->GetBufferPointer() ) );

			pErrorMsgs->Release();
		}

		return false;
	}
#else
	if(D3DXAssembleShader(pcSource, ALCHEMY_STRING_LENGTH(pcSource), ALCHEMY_NULL, ALCHEMY_NULL, 0, &pShader, ALCHEMY_NULL) != D3D_OK)
		return false;
#endif

#endif

	CBuffer& Buffer = CreateBuffer( sizeof(LPD3DXBUFFER) );
	Buffer.CopyFrom(&pShader);

	*ppOutput = &Buffer;

	return true;
}


bool CHLSLCompiler::BuildShaderFunctionCode(STRING& Destination, IEffect::EFFECTTYPE Type, alchemy::UINT uUserVariables, alchemy::UINT uCode, const CEffectCode& Code)
{
	const STRING* pCode = Code.GetCode(uCode);

	if(pCode)
	{
		STRING Tail;

		Destination = Type == IEffect::VERTEX_SHADER ? ALCHEMY_TEXT("vs.2.0\n") : ALCHEMY_TEXT("ps.2.0\n");
		const alchemy::UINT* puUserVariables = Code.GetUserVariables(uUserVariables);
		if(puUserVariables)
		{
			UINT uMaxOutputColorRegister = m_CompilerInfo.uOutputColorCount << 4;
			const CEffectCode::USERVARIABLE* pUserVariable = Code.GetUserVariable(*puUserVariables);
			UINT uRegister;
			const IEffect::SAMPLERDECLARATION* pSamplerDeclaration;
			while(pUserVariable)
			{
				switch(pUserVariable->RegisterType)
				{
				case ICompiler::PARAMETER:
					uRegister = pUserVariable->uRegister >> 4;

					if(Type == IEffect::VERTEX_SHADER)
					{
						if(uRegister)
						{
							Destination += ALCHEMY_TEXT("dcl_texcoord");

							if(uRegister > 2)
								Destination += uRegister - 2;
						}
						else
							Destination += ALCHEMY_TEXT("dcl_position");

						Destination += ALCHEMY_TEXT(' ');
						Destination += sm_pcVERTEX_SHADER_REGISTER[ICompiler::PARAMETER];
					}
					else
					{
						Destination += ALCHEMY_TEXT("dcl ");
						Destination += sm_pcFRAGMENT_SHADER_REGISTER[ICompiler::PARAMETER];
					}

					Destination += uRegister;
					Destination += ALCHEMY_TEXT('\n');
					break;
				case ICompiler::SAMPLER:
					pSamplerDeclaration = Code.GetSamplerDeclaration(pUserVariable->uDeclaration);
					if(pSamplerDeclaration)
					{
						Destination += ALCHEMY_TEXT("dcl_");
						Destination += sm_pcSAMPLER_TYPE[pSamplerDeclaration->Type];
						Destination += ALCHEMY_TEXT(' ');
						Destination += Type == IEffect::VERTEX_SHADER ? sm_pcVERTEX_SHADER_REGISTER[ICompiler::SAMPLER] : sm_pcFRAGMENT_SHADER_REGISTER[ICompiler::SAMPLER];
						Destination += pUserVariable->uRegister;
						Destination += ALCHEMY_TEXT('\n');
					}
					break;

				case ICompiler::OUTPUT:
					if(Type == IEffect::FRAGMENT_SHADER)
					{
						uRegister = (pUserVariable->uRegister > uMaxOutputColorRegister ? pUserVariable->uRegister - uMaxOutputColorRegister : pUserVariable->uRegister) >> 4;

						Tail += sm_pcInstruction[ICompiler::MOV];
						Tail += ALCHEMY_TEXT(" oC");
						Tail += uRegister;
						Tail += ALCHEMY_TEXT(", ");
						Tail += sm_pcFRAGMENT_SHADER_REGISTER[ICompiler::TEMPORARY];
						Tail += uRegister;
						Tail += ALCHEMY_TEXT('\n');
					}
					break;
				default:
					break;
				}

				pUserVariable = Code.GetUserVariable(pUserVariable->uNext);
			}

			Destination += *pCode;
			Destination += Tail;
			Destination += ALCHEMY_TEXT("\n");
		}

		return true;
	}

	return false;
}

bool CHLSLCompiler::AppendInstruction(
	STRING& Destination, 
	INSTRUCTION Instruction, 
	alchemy::UINT uDestination, 
	alchemy::UINT uDestinationIndex,
	alchemy::UINT uDestinationIndexAddress, 
	const alchemy::UINT* puDestinationAddress, 
	alchemy::UINT uDestinationAddressCount, 
	alchemy::UINT uSource1, 
	alchemy::UINT uSourceIndex1,
	alchemy::UINT uSourceIndexAddress1, 
	const alchemy::UINT* puAddress1, 
	alchemy::UINT uAddressCount1, 
	alchemy::UINT uSource2, 
	alchemy::UINT uSourceIndex2,
	alchemy::UINT uSourceIndexAddress2,
	const alchemy::UINT* puAddress2, 
	alchemy::UINT uAddressCount2, 
	CEffectCode& EffectCode)
{
	bool bIsDestinationIndex = false, bIsSourceIndex1 = false, bIsSourceIndex2 = false;

	ICompiler::REGISTERTYPE Type = EffectCode.GetRegisterType(uDestinationIndex);
	if(Type)
	{
		Destination += ALCHEMY_TEXT("mova a0.x ");
		__AppendVariable(Destination, uDestinationIndex, Type, 0, &uDestinationIndexAddress, 1, 0, EffectCode);
		Destination += ALCHEMY_TEXT('\n');

		bIsDestinationIndex = true;
	}

	Type = EffectCode.GetRegisterType(uSourceIndex1);
	if(Type)
	{
		Destination += ALCHEMY_TEXT("mova a0.y ");
		__AppendVariable(Destination, uSourceIndex1, Type, 0, &uSourceIndexAddress1, 1, 0, EffectCode);
		Destination += ALCHEMY_TEXT('\n');

		bIsSourceIndex1 = true;
	}

	Type = EffectCode.GetRegisterType(uSourceIndex2);
	if(Type)
	{
		Destination += ALCHEMY_TEXT("mova a0.z ");
		__AppendVariable(Destination, uSourceIndex2, Type, 0, &uSourceIndexAddress2, 1, 0, EffectCode);
		Destination += ALCHEMY_TEXT('\n');

		bIsSourceIndex2 = true;
	}

	Destination += sm_pcInstruction[Instruction];

	Destination += ALCHEMY_TEXT(' ');

	alchemy::UINT uMinDestiantionAddress = 0;

	Type = EffectCode.GetRegisterType(uDestination);
	if(Type)
	{
		__AppendVariable(Destination, uDestination, Type, bIsDestinationIndex ? 1 : 0, puDestinationAddress, uDestinationAddressCount, 0, EffectCode);

		if(uDestinationAddressCount > 1)
		{
			if(puDestinationAddress)
			{
				uMinDestiantionAddress = puDestinationAddress[0];
				for(alchemy::UINT i = 1; i < uDestinationAddressCount; ++ i)
					uMinDestiantionAddress = ALCHEMY_MININUM(uMinDestiantionAddress, puDestinationAddress[i]);
			}

			const CEffectCode::VARIABLE* pVariable = EffectCode.GetVariable(uDestination);
			alchemy::UINT uRegister = 0;
			while(pVariable)
			{
				uRegister += pVariable->uRegister;

				pVariable = pVariable->uParent ? EffectCode.GetVariable(pVariable->uParent) : ALCHEMY_NULL;
			}

			uMinDestiantionAddress += (uRegister >> 2) & 3;
		}

		Destination += ALCHEMY_TEXT(", ");
	}

	Type = EffectCode.GetRegisterType(uSource1);
	__AppendVariable(Destination, uSource1, Type, bIsSourceIndex1 ? 2 : 0, puAddress1, uAddressCount1, uMinDestiantionAddress, EffectCode);

	Type = EffectCode.GetRegisterType(uSource2);
	if(Type)
	{
		Destination += ALCHEMY_TEXT(", ");

		__AppendVariable(Destination, uSource2, Type, bIsSourceIndex2 ? 3 : 0, puAddress2, uAddressCount2, uMinDestiantionAddress, EffectCode);
	}

	Destination += ALCHEMY_TEXT('\n');

	return true;
}

bool CHLSLCompiler::ReplaceVariable(
	STRING& Code, 
	alchemy::UINT uSource, 
	const alchemy::UINT* puSourceAddress, 
	alchemy::UINT uDestiantion, 
	const alchemy::UINT* puDestiantionAddress, 
	alchemy::UINT uAddressCount, 
	alchemy::UINT uMinDestinationAddress, 
	const CEffectCode& EffectCode, 
	alchemy::UINT uCodeFrom, 
	alchemy::UINT uCodeLength)
{
	STRING Source, Destination;
	uMinDestinationAddress = uCodeLength < 2 ? 0 : uMinDestinationAddress;
	__AppendVariable(Source     , uSource     , EffectCode.GetRegisterType(uSource)     , 0, puSourceAddress     , uAddressCount, uMinDestinationAddress, EffectCode);
	__AppendVariable(Destination, uDestiantion, EffectCode.GetRegisterType(uDestiantion), 0, puDestiantionAddress, uAddressCount, uMinDestinationAddress, EffectCode);

	if( Source.GetLength() && Destination.GetLength() )
	{
		alchemy::UINT uIndex = Code.GetIndexOf(Source, uCodeFrom, uCodeLength), uLength = Source.GetLength(), uFinalIndex = uCodeLength;
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

bool CHLSLCompiler::AppendRegisterName(STRING& Destination, alchemy::UINT uRegister, ICompiler::REGISTERTYPE RegisterType, IEffect::EFFECTTYPE ShaderType)const
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

void CHLSLCompiler::__AppendVariable(
									 STRING& Destination, 
									 alchemy::UINT uVariable,
									 ICompiler::REGISTERTYPE Type, 
									 alchemy::UINT uVariableIndex, 
									 const alchemy::UINT* puAddress, 
									 alchemy::UINT uAddressCount, 
									 alchemy::UINT uMinDestinationAddress, 
									 const CEffectCode& EffectCode)
{
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
					Destination += pVariable->uRegister;
			}
			else
			{
				alchemy::UINT uRegister = 0;
				while(pVariable)
				{
					uRegister += pVariable->uRegister;

					pVariable = EffectCode.GetVariable(pVariable->uParent);
				}

				alchemy::UINT uAddress[4];
				bool bIsAppendIndex = true;
				if(Type == ICompiler::OUTPUT)
					bIsAppendIndex = ShaderType != IEffect::VERTEX_SHADER;
				else if(uMinDestinationAddress > 0 && uMinDestinationAddress < 4 && puAddress && uAddressCount && uAddressCount < 3)
				{
					alchemy::UINT uRegisterCount = uMinDestinationAddress + uAddressCount;

					if(uRegisterCount == 5)
					{
						uAddressCount  = 1;
						uRegisterCount = 4;
					}

					alchemy::UINT i;
					for(i = 0; i < uMinDestinationAddress; ++ i)
						uAddress[i] = puAddress[0];

					for(i = 0; i < uAddressCount; ++ i)
						uAddress[i + uMinDestinationAddress] = puAddress[i];

					puAddress = uAddress;
					uAddressCount = uRegisterCount;
				}

				if(ShaderType == IEffect::FRAGMENT_SHADER)
				{
					UINT uMaxOutputColorRegister = m_CompilerInfo.uOutputColorCount * 16;
					if(Type == ICompiler::TEMPORARY)
						uRegister = uMaxOutputColorRegister + uRegister;
					else if(Type == ICompiler::OUTPUT && uRegister > uMaxOutputColorRegister)
						uRegister -= uMaxOutputColorRegister;
				}

				if(uVariableIndex)
					__AppendRegister(Destination, uRegister, uVariableIndex - 1, puAddress, uAddressCount, bIsAppendIndex);
				else
					__AppendRegister(Destination, uRegister, puAddress, uAddressCount, bIsAppendIndex);
			}
		}
	}
}

void CHLSLCompiler::__AppendRegister(STRING& Destiantion, alchemy::UINT uRegister, const alchemy::UINT* puAddress, alchemy::UINT uAddressCount, bool bIsAppendIndex)
{
	uRegister >>= 2;

	alchemy::UINT uTemp = uRegister >> 2;

	if(bIsAppendIndex)
		Destiantion += uTemp;

	uTemp = uRegister - (uTemp << 2);

	uAddressCount = ALCHEMY_MININUM(4u, uAddressCount);
	if(uTemp)
	{
		Destiantion += ALCHEMY_TEXT('.');
		if( puAddress && uAddressCount)
		{
			alchemy::UINT uAddress;
			for(alchemy::UINT i = 0; i < uAddressCount; ++ i)
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

		alchemy::UINT uAddress;
		for(alchemy::UINT i = 0; i < uAddressCount; ++ i)
		{
			uAddress = puAddress[i];
			if(uAddress < 4)
				Destiantion += sm_pcREGISTER_ADDRESS[uAddress];
		}
	}
}

void CHLSLCompiler::__AppendRegister(STRING& Destiantion, alchemy::UINT uRegister, alchemy::UINT uVariableIndex, const alchemy::UINT* puAddress, alchemy::UINT uAddressCount, bool bIsAppendIndex)
{
	Destiantion += ALCHEMY_TEXT("[a0.");
	Destiantion += sm_pcREGISTER_ADDRESS[uVariableIndex];

	Destiantion += ALCHEMY_TEXT('+');

	uRegister >>= 2;

	alchemy::UINT uCurrentRegister = uRegister >> 2;

	Destiantion += uCurrentRegister;

	Destiantion += ALCHEMY_TEXT(']');

	uCurrentRegister = uRegister - (uCurrentRegister << 2);

	uAddressCount = ALCHEMY_MININUM(4u, uAddressCount);
	if(uCurrentRegister)
	{
		Destiantion += ALCHEMY_TEXT('.');
		if( puAddress && uAddressCount)
		{
			alchemy::UINT uAddress;
			for(alchemy::UINT i = 0; i < uAddressCount; ++ i)
			{
				uAddress = puAddress[i] + uCurrentRegister;
				if(uAddress < 4)
					Destiantion += sm_pcREGISTER_ADDRESS[uAddress];
			}
		}
		else
			Destiantion += sm_pcREGISTER_ADDRESS[uCurrentRegister];
	}
	else if(puAddress && uAddressCount)
	{
		Destiantion += ALCHEMY_TEXT('.');

		alchemy::UINT uAddress;
		for(alchemy::UINT i = 0; i < uAddressCount; ++ i)
		{
			uAddress = puAddress[i];
			if(uAddress < 4)
				Destiantion += sm_pcREGISTER_ADDRESS[uAddress];
		}
	}
}