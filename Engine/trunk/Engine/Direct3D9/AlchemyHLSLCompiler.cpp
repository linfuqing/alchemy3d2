#include "AlchemyHLSLCompiler.h"

#include <d3dx9.h>

#pragma comment(lib,"d3dx9.lib")

using namespace alchemy;

const alchemy::CHAR* CHLSLCompiler::sm_pcINSTRUCTION[ICompiler::TOTAL_INSTRUCTION] = 
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

CHLSLCompiler::CHLSLCompiler(alchemy::UINT uOutputColorCount, alchemy::UINT uConstantBufferSize) :
	m_puOutputRegisters(ALCHEMY_NULL)
{
	m_CompilerInfo.uByteAlignmentShift = 4;
	m_CompilerInfo.uOutputColorCount = uOutputColorCount;
	m_CompilerInfo.uMaxVertexConstantBufferSize = uConstantBufferSize;
	m_CompilerInfo.uMaxFragmentConstantBufferSize = uConstantBufferSize;
	m_CompilerInfo.bIsSupportConstantBuffer = false;
}

CHLSLCompiler::~CHLSLCompiler(void)
{
	ALCHEMY_DEBUG_DELETE_ARRAY(m_puOutputRegisters);
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


bool CHLSLCompiler::BuildShaderFunctionCode(STRING& Destination, IEffect::EFFECTTYPE Type, alchemy::UINT uInstructionIndices, alchemy::UINT uUserVariables, const CEffectCode& Code)
{
	const UINT* puInstructionIndices = Code.GetInstructionIndices(uInstructionIndices);
	const CEffectCode::INSTRUCTIONINDEX* pInstructionIndex = puInstructionIndices ? Code.GetInstructionIndex(*puInstructionIndices) : ALCHEMY_NULL;
	if(pInstructionIndex)
	{
		STRING Tail;

		Destination = Type == IEffect::VERTEX_SHADER ? ALCHEMY_TEXT("vs.2.0\n") : ALCHEMY_TEXT("ps.2.0\n");
		const alchemy::UINT* puUserVariables = Code.GetUserVariables(uUserVariables);
		if(puUserVariables)
		{
			ALCHEMY_DEBUG_DELETE_ARRAY(m_puOutputRegisters);

			m_uTemporaryRegisterCount = 0;
			m_TemporaryRegisterMap.Clear();
			m_FreeTemporaryRegister.Reset();
			m_uFreeTemporaryRegisterCount = 0;

			UINT uMaxOutputColorRegister = m_CompilerInfo.uOutputColorCount << 4;
			const CEffectCode::USERVARIABLE* pUserVariable = Code.GetUserVariable(*puUserVariables);
			UINT uRegister, uNextRegister;
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

							if(uRegister > 1)
								Destination += uRegister - 1;
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
				case ICompiler::OUTPUT:
					if(Type == IEffect::FRAGMENT_SHADER)
					{
						uRegister = (pUserVariable->uRegister > uMaxOutputColorRegister ? (pUserVariable->uRegister % uMaxOutputColorRegister) : pUserVariable->uRegister) >> 4;

						if(!m_puOutputRegisters)
						{
							ALCHEMY_DEBUG_NEW(m_puOutputRegisters, UINT[m_CompilerInfo.uOutputColorCount]);
							for(UINT i = 0; i < m_CompilerInfo.uOutputColorCount; ++ i)
								m_puOutputRegisters[i] = 0;
						}

						uNextRegister = m_puOutputRegisters[uRegister];
						if(uNextRegister)
							-- uNextRegister;
						else
						{
							uNextRegister = m_uTemporaryRegisterCount ++;

							m_puOutputRegisters[uRegister] = m_uTemporaryRegisterCount;
						}

						Tail += sm_pcINSTRUCTION[ICompiler::MOV];
						Tail += ALCHEMY_TEXT(" oC");
						Tail += uRegister;
						Tail += ALCHEMY_TEXT(", ");
						Tail += sm_pcFRAGMENT_SHADER_REGISTER[ICompiler::TEMPORARY];
						Tail += uNextRegister;
						Tail += ALCHEMY_TEXT('\n');
					}
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

				default:
					break;
				}

				pUserVariable = Code.GetUserVariable(pUserVariable->uNext);
			}

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

			Destination += Tail;
			Destination += ALCHEMY_TEXT("\n");
		}

		return true;
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


bool CHLSLCompiler::__AppendInstruction(
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
									  const CEffectCode& EffectCode)
{
	STRING Tail = sm_pcINSTRUCTION[Instruction];

	Tail += ALCHEMY_TEXT(' ');

	m_uIntegerRegisterIndex = 0;

	alchemy::UINT uMinDestiantionAddress = 0, uTemporaryRegister = 0;
	IEffect::EFFECTTYPE ShaderType = IEffect::PROGRAM;
	if(uDestination)
	{
		if(Instruction == ICompiler::M34)
		{
			const CEffectCode::VARIABLE* pVariable = EffectCode.GetVariable(uDestination);
			if(pVariable)
			{
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
					Tail += sm_pcVERTEX_SHADER_REGISTER[ICompiler::TEMPORARY];
				}
				else if(ShaderType == IEffect::FRAGMENT_SHADER)
				{
					Tail += sm_pcFRAGMENT_SHADER_REGISTER[ICompiler::TEMPORARY];
				}
				else
				{
					/////////////////////////////////////////////////////////////
					////////////函数调用 未完成
					/////////////////////////////////////////////////////////////
				}

				if(m_uFreeTemporaryRegisterCount)
					uTemporaryRegister = m_FreeTemporaryRegister[-- m_uFreeTemporaryRegisterCount];
				else
					uTemporaryRegister = m_uTemporaryRegisterCount ++;

				Tail += uTemporaryRegister;
				Tail += ALCHEMY_TEXT(", ");
			}
		}
		else
		{
			__AppendVariable(Tail, Destination, uDestination, uDestinationIndex, uDestinationIndexAddress, puDestinationAddress, uDestinationAddressCount, 0, EffectCode);

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

			Tail += ALCHEMY_TEXT(", ");
		}
	}

	if(Instruction == ICompiler::KIL)
	{
		const CEffectCode::VARIABLE* pVariable = EffectCode.GetVariable(uSource1);
		if(pVariable)
		{
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

			const CHAR* pcRegister;
			if(ShaderType == IEffect::VERTEX_SHADER)
			{
				pcRegister = sm_pcVERTEX_SHADER_REGISTER[ICompiler::TEMPORARY];
			}
			else if(ShaderType == IEffect::FRAGMENT_SHADER)
			{
				pcRegister = sm_pcFRAGMENT_SHADER_REGISTER[ICompiler::TEMPORARY];
			}
			else
			{
				/////////////////////////////////////////////////////////////
				////////////函数调用 未完成
				/////////////////////////////////////////////////////////////
			}

			if(m_uFreeTemporaryRegisterCount)
				uTemporaryRegister = m_FreeTemporaryRegister[-- m_uFreeTemporaryRegisterCount];
			else
				uTemporaryRegister = m_uTemporaryRegisterCount ++;

			Tail += pcRegister;
			Tail += uTemporaryRegister;

			STRING Head = sm_pcINSTRUCTION[ICompiler::MOV];
			Head += ALCHEMY_TEXT(' ');
			Head += pcRegister;
			Head += uTemporaryRegister;
			Head += ALCHEMY_TEXT(", ");

			__AppendVariable(Head, Destination, uSource1, uSourceIndex1, uSourceIndexAddress1, puAddress1, uAddressCount1, uMinDestiantionAddress, EffectCode);

			Head += ALCHEMY_TEXT('\n');

			Destination += Head;
		}
	}
	else
		__AppendVariable(Tail, Destination, uSource1, uSourceIndex1, uSourceIndexAddress1, puAddress1, uAddressCount1, uMinDestiantionAddress, EffectCode);

	if(uSource2)
	{
		Tail += ALCHEMY_TEXT(", ");

		__AppendVariable(Tail, Destination, uSource2, uSourceIndex2, uSourceIndexAddress2, puAddress2, uAddressCount2, uMinDestiantionAddress, EffectCode);
	}

	Destination += Tail;
	Destination += ALCHEMY_TEXT('\n');

	if(Instruction == ICompiler::M34 && uDestination)
	{
		Tail = sm_pcINSTRUCTION[ICompiler::MOV];
		Tail += ALCHEMY_TEXT(' ');
		__AppendVariable(Tail, Destination, uDestination, uDestinationIndex, uDestinationIndexAddress, puDestinationAddress, uDestinationAddressCount, 0, EffectCode);

		Destination += Tail;
		Destination += ALCHEMY_TEXT(", ");

		if(ShaderType == IEffect::VERTEX_SHADER)
		{
			Destination += sm_pcVERTEX_SHADER_REGISTER[ICompiler::TEMPORARY];
		}
		else if(ShaderType == IEffect::FRAGMENT_SHADER)
		{
			Destination += sm_pcFRAGMENT_SHADER_REGISTER[ICompiler::TEMPORARY];
		}
		else
		{
			/////////////////////////////////////////////////////////////
			////////////函数调用 未完成
			/////////////////////////////////////////////////////////////
		}

		UINT uAddress[] = {0, 1, 2, 3};
		__AppendRegister(Destination, uTemporaryRegister << 4, uAddress, uDestinationAddressCount, true);
		Destination += ALCHEMY_TEXT('\n');

		if(m_FreeTemporaryRegister.GetLength() > m_uFreeTemporaryRegisterCount)
			m_FreeTemporaryRegister[m_uFreeTemporaryRegisterCount ++] = uTemporaryRegister;
		else
		{
			m_FreeTemporaryRegister.Add(uTemporaryRegister);

			++ m_uFreeTemporaryRegisterCount;
		}
	}

	return true;
}

void CHLSLCompiler::__AppendVariable(
									 STRING& Destination, 
									 STRING& Head, 
									 alchemy::UINT uVariable,
									 alchemy::UINT uVariableIndex, 
									 alchemy::UINT uIndexAddress, 
									 const alchemy::UINT* puAddress, 
									 alchemy::UINT uAddressCount, 
									 alchemy::UINT uMinDestinationAddress, 
									 const CEffectCode& EffectCode)
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

				if(Type == ICompiler::OUTPUT && ShaderType == IEffect::FRAGMENT_SHADER)
				{
					UINT uCurrentRegister = uRegister >> 4;
					UINT uOutputRegisterIndex = m_CompilerInfo.uOutputColorCount < uCurrentRegister ? uCurrentRegister % m_CompilerInfo.uOutputColorCount : uCurrentRegister;

					if(!m_puOutputRegisters)
					{
						ALCHEMY_DEBUG_NEW(m_puOutputRegisters, UINT[m_CompilerInfo.uOutputColorCount]);
						for(UINT i = 0; i < m_CompilerInfo.uOutputColorCount; ++ i)
							m_puOutputRegisters[i] = 0;
					}

					UINT uNextRegister = m_puOutputRegisters[uOutputRegisterIndex];
					if(uNextRegister)
						-- uNextRegister;
					else
					{
						uNextRegister = m_uTemporaryRegisterCount ++;

						m_puOutputRegisters[uOutputRegisterIndex] = m_uTemporaryRegisterCount;
					}

					uRegister = (uNextRegister << 4) | (~(uCurrentRegister << 4) & uRegister);
				}
				else if(Type == ICompiler::PARAMETER && ShaderType == IEffect::FRAGMENT_SHADER)
				{
					puAddress     = ALCHEMY_NULL;
					uAddressCount = 0;
				}
				else if(Type == ICompiler::TEMPORARY)
				{
					UINT uCurrentRegister = uRegister >> 4;
					const UINT* puRegister = m_TemporaryRegisterMap.Get(uCurrentRegister);
					UINT uNextRegister;
					if(puRegister)
						uNextRegister = *puRegister;
					else
					{
						if(m_uFreeTemporaryRegisterCount)
							uNextRegister = m_FreeTemporaryRegister[-- m_uFreeTemporaryRegisterCount];
						else
							uNextRegister = m_uTemporaryRegisterCount ++;

						m_TemporaryRegisterMap.Insert(uCurrentRegister, uNextRegister);
					}

					uRegister = (uNextRegister << 4) | (~(uCurrentRegister << 4) & uRegister);
				}

				if(uVariableIndex)
				{
					Head += ALCHEMY_TEXT("mova a0.");
					Head += sm_pcREGISTER_ADDRESS[m_uIntegerRegisterIndex];
					Head += ALCHEMY_TEXT(", ");
					__AppendVariable(Head, Head, uVariableIndex, 0, 0, &uIndexAddress, 1, 0, EffectCode);
					Head += ALCHEMY_TEXT('\n');

					__AppendRegister(Destination, uRegister, m_uIntegerRegisterIndex, puAddress, uAddressCount, bIsAppendIndex);

					++ m_uIntegerRegisterIndex;
				}
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