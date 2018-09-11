#include "AlchemyGLSLCompiler.h"

#include "../Graphics/AlchemyEffectCode.h"

using namespace alchemy;

const CGLSLCompiler::GLSLINSTRUCTION CGLSLCompiler::sm_INSTRUCTION[ICompiler::TOTAL_INSTRUCTION] = 
{
	{ALCHEMY_NULL, ALCHEMY_NULL, ALCHEMY_NULL, false},
	{ALCHEMY_NULL, ALCHEMY_TEXT(" + "), ALCHEMY_NULL, false},
	{ALCHEMY_NULL, ALCHEMY_TEXT(" - "), ALCHEMY_NULL, false},
	{ALCHEMY_NULL, ALCHEMY_TEXT(" * "), ALCHEMY_NULL, false},
	{ALCHEMY_NULL, ALCHEMY_TEXT(" / "), ALCHEMY_NULL, false},
	{ALCHEMY_TEXT("1.0 / "), ALCHEMY_NULL, ALCHEMY_NULL, false},
	{ALCHEMY_TEXT("min"), ALCHEMY_TEXT(", "), ALCHEMY_NULL, false},
	{ALCHEMY_TEXT("max"), ALCHEMY_TEXT(", "), ALCHEMY_NULL, false},
	{ALCHEMY_TEXT("fract"), ALCHEMY_NULL, ALCHEMY_NULL, false},
	{ALCHEMY_TEXT("sqrt"), ALCHEMY_NULL, ALCHEMY_NULL, false},
	{ALCHEMY_TEXT("inversesqrt"), ALCHEMY_NULL, ALCHEMY_NULL, false},
	{ALCHEMY_TEXT("pow"), ALCHEMY_TEXT(", "), ALCHEMY_NULL, false},
	{ALCHEMY_TEXT("log"), ALCHEMY_NULL, ALCHEMY_NULL, false},
	{ALCHEMY_TEXT("exp"), ALCHEMY_NULL, ALCHEMY_NULL, false},
	{ALCHEMY_TEXT("normalize"), ALCHEMY_NULL, ALCHEMY_NULL, false},
	{ALCHEMY_TEXT("sin"), ALCHEMY_NULL, ALCHEMY_NULL, false},
	{ALCHEMY_TEXT("cos"), ALCHEMY_NULL, ALCHEMY_NULL, false},
	{ALCHEMY_TEXT("cross"), ALCHEMY_TEXT(","), ALCHEMY_NULL, false},
	{ALCHEMY_TEXT("dot"), ALCHEMY_TEXT(", "), ALCHEMY_NULL, false},
	{ALCHEMY_TEXT("dot"), ALCHEMY_TEXT(", "), ALCHEMY_NULL, false},
	{ALCHEMY_TEXT("abs"), ALCHEMY_NULL, ALCHEMY_NULL, false},
	{ALCHEMY_TEXT("- "), ALCHEMY_NULL, ALCHEMY_NULL, false},
	{ALCHEMY_TEXT("clamp"), ALCHEMY_TEXT(", 0.0, 1.0"), ALCHEMY_NULL, false},
	{ALCHEMY_NULL, ALCHEMY_TEXT(" * "), ALCHEMY_NULL, false},
	{ALCHEMY_NULL, ALCHEMY_TEXT(" * "), ALCHEMY_NULL, false},
	{ALCHEMY_NULL, ALCHEMY_TEXT(" * "), ALCHEMY_NULL, false},
	{ALCHEMY_TEXT("if("), ALCHEMY_TEXT(" < 0.0)\n\t\t discard"), false}, //
	{ALCHEMY_TEXT("texture"), ALCHEMY_TEXT(", "), ALCHEMY_NULL, true},
	{ALCHEMY_NULL, ALCHEMY_TEXT(" >= "), ALCHEMY_TEXT(" ? 1.0 : 0.0"), false},
	{ALCHEMY_NULL, ALCHEMY_TEXT(" < "), ALCHEMY_TEXT(" ? 1.0 : 0.0"), false},
	{ALCHEMY_NULL, ALCHEMY_TEXT(" > "), ALCHEMY_TEXT(" ? 1.0 : 0.0"), false}
};

const CHAR* CGLSLCompiler::sm_pcVERTEX_SHADER_REGISTER[ICompiler::TOTAL_REGISTERTYPE]   = 
{
	ALCHEMY_TEXT(""), 
	ALCHEMY_TEXT("vc"), 
	ALCHEMY_TEXT("vt"), 
	ALCHEMY_TEXT("va"), 
	ALCHEMY_TEXT("v"), 
	ALCHEMY_TEXT("gl_Position"), 
	ALCHEMY_TEXT("")  
};

const CHAR* CGLSLCompiler::sm_pcFRAGMENT_SHADER_REGISTER[ICompiler::TOTAL_REGISTERTYPE] = 
{
	ALCHEMY_TEXT(""), 
	ALCHEMY_TEXT("fc"), 
	ALCHEMY_TEXT("ft"), 
	ALCHEMY_TEXT("v"), 
	ALCHEMY_TEXT(""), 
	ALCHEMY_TEXT("gl_FragColor"), 
	ALCHEMY_TEXT("fs")
};

const CHAR CGLSLCompiler::sm_cREGISTER_ADDRESS[5] = ALCHEMY_TEXT("xyzw");

const CHAR* CGLSLCompiler::sm_pcSAMPLER_TYPE[IEffect::TOTAL_SAMPLER_TYPE] = 
{
	ALCHEMY_TEXT("2D"), 
	ALCHEMY_TEXT("3D"), 
	ALCHEMY_TEXT("Cube")
};

CGLSLCompiler::CGLSLCompiler() : 
m_SamplerType(IEffect::TOTAL_SAMPLER_TYPE)
{
    m_VariableTypeMap.Insert( CEffectCode::VECTOR1_VARIABLE, ALCHEMY_TEXT("float") );
	m_VariableTypeMap.Insert( CEffectCode::VECTOR2_VARIABLE, ALCHEMY_TEXT("vec2") );
	m_VariableTypeMap.Insert( CEffectCode::VECTOR3_VARIABLE, ALCHEMY_TEXT("vec3") );
	m_VariableTypeMap.Insert( CEffectCode::VECTOR4_VARIABLE, ALCHEMY_TEXT("vec4") );
	
	m_VariableTypeMap.Insert( CEffectCode::MATRIX2X2_VARIABLE, ALCHEMY_TEXT("mat2") );
	m_VariableTypeMap.Insert( CEffectCode::MATRIX3X3_VARIABLE, ALCHEMY_TEXT("mat3") );
	m_VariableTypeMap.Insert( CEffectCode::MATRIX4X3_VARIABLE, ALCHEMY_TEXT("mat4") );
	m_VariableTypeMap.Insert( CEffectCode::MATRIX4X4_VARIABLE, ALCHEMY_TEXT("mat4") );
}

CGLSLCompiler::~CGLSLCompiler()
{
    
}

const ICompiler::COMPILERINFO& CGLSLCompiler::GetCompilerInfo()const
{
    static const ICompiler::COMPILERINFO sm_COMPILER_INFO = 
    {
        4,
        1,
		0,
		2048 * 2048,
		false
    };
    
    return sm_COMPILER_INFO;
}

UINT CGLSLCompiler::GetAddressShift(UINT uVariable, const CEffectCode& EffectCode)const
{
    const CEffectCode::VARIABLE* pVarialbe = EffectCode.GetVariable(uVariable);
    if(pVarialbe)
        return (pVarialbe->uRegister & 15) >> 2;
    
    return 0;
}

bool CGLSLCompiler::ProgramString(CBuffer** ppOutput, IEffect::EFFECTTYPE Type, const CHAR* pcSource)
{
	UINT uSize = StringLength(pcSource) * sizeof(CHAR);
	CBuffer& Buffer = CreateBuffer(uSize);
	Buffer.CopyFrom(pcSource, uSize);
	
	if(ppOutput)
		*ppOutput = &Buffer;
	
    return true;
}

typedef struct
{
	UINT uValues[ICompiler::TOTAL_REGISTERTYPE];
}REGISTERADDRESS;

#include <stdio.h>
bool CGLSLCompiler::BuildShaderFunctionCode(STRING& Destination, IEffect::EFFECTTYPE Type, UINT uUserVariables, UINT uCode, const CEffectCode& Code)
{	
	const STRING* pCode = Code.GetCode(uCode);
	if(pCode)
	{
		STRING Head;
		STRING Main;
		
		const CHAR* pcGlobalKeyword = ALCHEMY_NULL, 
		* pcParameterKeyword = ALCHEMY_NULL, 
		* pcResultKeyword = ALCHEMY_NULL, 
		* pcTemporaryKeyword = ALCHEMY_NULL,
		* pcGlobal = ALCHEMY_NULL, 
		* pcParameter = ALCHEMY_NULL, 
		* pcResult = ALCHEMY_NULL, 
		* pcTemporary = ALCHEMY_NULL, 
		* pcSampler = ALCHEMY_NULL;
		if(Type == IEffect::VERTEX_SHADER)
		{
			pcGlobalKeyword           = ALCHEMY_TEXT("uniform");
			pcParameterKeyword        = ALCHEMY_TEXT("attribute");
			pcResultKeyword           = ALCHEMY_TEXT("varying");
			pcTemporaryKeyword        = ALCHEMY_TEXT("\t");
			pcGlobal                  = sm_pcVERTEX_SHADER_REGISTER[ICompiler::GLOBAL];
			pcParameter               = sm_pcVERTEX_SHADER_REGISTER[ICompiler::PARAMETER];
			pcResult                  = sm_pcVERTEX_SHADER_REGISTER[ICompiler::RESULT];
			pcTemporary               = sm_pcVERTEX_SHADER_REGISTER[ICompiler::TEMPORARY];
			pcSampler                 = sm_pcVERTEX_SHADER_REGISTER[ICompiler::SAMPLER];
			
			//Head = ALCHEMY_TEXT("#pragma STDGL invariant(all)\n");
			
			/*Main += ALCHEMY_TEXT('\t');
			Main += m_VariableTypeMap[CEffectCode::VECTOR4_VARIABLE];
			Main += ALCHEMY_TEXT(' ');
			Main += sm_pcVERTEX_SHADER_REGISTER[ICompiler::OUTPUT];
			Main += ALCHEMY_TEXT(";\n");*/
		}
		else if(Type == IEffect::FRAGMENT_SHADER)
		{
			pcGlobalKeyword           = ALCHEMY_TEXT("uniform");
			pcParameterKeyword        = ALCHEMY_TEXT("varying");
			pcResultKeyword           = ALCHEMY_NULL;
			pcTemporaryKeyword        = ALCHEMY_TEXT("\t");
			pcGlobal                  = sm_pcFRAGMENT_SHADER_REGISTER[ICompiler::GLOBAL];
			pcParameter               = sm_pcFRAGMENT_SHADER_REGISTER[ICompiler::PARAMETER];
			pcResult                  = sm_pcFRAGMENT_SHADER_REGISTER[ICompiler::RESULT];
			pcTemporary               = sm_pcFRAGMENT_SHADER_REGISTER[ICompiler::TEMPORARY];
			pcSampler                 = sm_pcFRAGMENT_SHADER_REGISTER[ICompiler::SAMPLER];
			
			Head = ALCHEMY_TEXT("precision lowp float;\n");//ALCHEMY_TEXT("#ifdef GL_FRAGMENT_PRECISION_HIGH\n\tprecision highp float;\n#else \n\tprecision mediump float;\n#endif\n\n");
		}
		
		//Head += pcGlobalKeyword;
//		Head += ALCHEMY_TEXT(' ');
//		Head += m_VariableTypeMap[CEffectCode::VECTOR4_VARIABLE];
//		Head += ALCHEMY_TEXT(' ');
//		Head += pcGlobal;
//		Head += ALCHEMY_TEXT('[');
//		Head += pcMaxGlobalUniformVectors;
//		Head += ALCHEMY_TEXT("];\n");
		
		CMap< UINT, CMap<CEffectCode::VARIABLETYPE, REGISTERADDRESS> > RegisterMap;
		CMap<CEffectCode::VARIABLETYPE, REGISTERADDRESS>* pVariableTypeMap;
		REGISTERADDRESS* pRegister;
		UINT uRegister, uOffset, uMaxGlobalRegister = 0;
		
		const CHAR** ppcVariableType;
		const IEffect::SAMPLERDECLARATION* pSamplerDeclaration;
		
		const UINT* puUserVariable = Code.GetUserVariables(uUserVariables);
		if(puUserVariable)
		{
			const CEffectCode::USERVARIABLE* pUserVariable = Code.GetUserVariable(*puUserVariable);
			while(pUserVariable)
			{
				pVariableTypeMap = RegisterMap.Get(uRegister = 
											 pUserVariable->RegisterType == ICompiler::SAMPLER ? 
											 pUserVariable->uRegister : pUserVariable->uRegister >> 4);
				if(!pVariableTypeMap)
				{
					static CMap<CEffectCode::VARIABLETYPE, REGISTERADDRESS> VariableTypeMap;
					RegisterMap.Insert(uRegister, VariableTypeMap);
					pVariableTypeMap = &RegisterMap[uRegister];
				}
				
				pRegister = pVariableTypeMap->Get(pUserVariable->VariableType);
				if(!pRegister)
				{
					static REGISTERADDRESS RegisterAddress;
					static bool s_bIsInitRegisterAddress = false;
					if(!s_bIsInitRegisterAddress)
					{
						for(UINT i = 0; i < ICompiler::TOTAL_REGISTERTYPE; ++ i)
							RegisterAddress.uValues[i] = 0;
							
						s_bIsInitRegisterAddress = true;
					}
							
					pVariableTypeMap->Insert(pUserVariable->VariableType, RegisterAddress);
					pRegister = &(*pVariableTypeMap)[pUserVariable->VariableType];
				}
				
				uOffset = pUserVariable->RegisterType == ICompiler::SAMPLER ? 0 : (pUserVariable->uRegister >> 2) - (uRegister << 2);
				if( !ALCHEMY_TEST_BIT(pRegister->uValues[pUserVariable->RegisterType], uOffset) )
				{
					ppcVariableType = m_VariableTypeMap.Get(pUserVariable->VariableType);
					if(!ppcVariableType && pUserVariable->RegisterType == ICompiler::SAMPLER)
					{
						pSamplerDeclaration = Code.GetSamplerDeclaration(pUserVariable->uDeclaration);
						if(pSamplerDeclaration)
							ppcVariableType = &sm_pcSAMPLER_TYPE[pSamplerDeclaration->Type];
					}
					
					if(ppcVariableType)
					{
						switch(pUserVariable->RegisterType) 
						{
							case ICompiler::GLOBAL:
								uMaxGlobalRegister = ALCHEMY_MAXINUM(uMaxGlobalRegister, pUserVariable->uRegister + Code.GetRegisterCount(pUserVariable->VariableType) * pUserVariable->uCount);
								break;
							case ICompiler::TEMPORARY:
								Main += pcTemporaryKeyword;
								//Main += ALCHEMY_TEXT(' ');
								Main += *ppcVariableType;
								Main += ALCHEMY_TEXT(' ');
								Main += pcTemporary;
								Main += ALCHEMY_TEXT('_');
								Main += *ppcVariableType;
								Main += ALCHEMY_TEXT('_');
								Main += uRegister;
								
								Main += ALCHEMY_TEXT('_');
								Main += uOffset;
								
								if(pUserVariable->uCount > 1)
								{
									Main += ALCHEMY_TEXT('_');
									Main += pUserVariable->uCount;
									Main += ALCHEMY_TEXT('[');
									Main += pUserVariable->uCount;
									Main += ALCHEMY_TEXT(']');
								}
								Main += ALCHEMY_TEXT(";\n");
								break;
							case ICompiler::PARAMETER:
								Head += pcParameterKeyword;
								Head += ALCHEMY_TEXT(' ');
								Head += *ppcVariableType;
								Head += ALCHEMY_TEXT(' ');
								Head += pcParameter;
								Head += uRegister;
								Head += ALCHEMY_TEXT(";\n");
								break;
							case ICompiler::RESULT:
								Head += pcResultKeyword;
								Head += ALCHEMY_TEXT(' ');
								Head += *ppcVariableType;
								Head += ALCHEMY_TEXT(' ');
								Head += pcResult;
								Head += uRegister;
								Head += ALCHEMY_TEXT(";\n");
								break;
							case ICompiler::SAMPLER:
								Head += ALCHEMY_TEXT("uniform");
								Head += ALCHEMY_TEXT(' ');
								Head += ALCHEMY_TEXT("sampler");
								Head += *ppcVariableType;
								Head += ALCHEMY_TEXT(' ');
								Head += pcSampler;
								Head += uRegister;
								Head += ALCHEMY_TEXT(";\n");
							default:
								break;
						}
						
						ALCHEMY_SET_BIT(pRegister->uValues[pUserVariable->RegisterType], uOffset);
					}
				}
				
				pUserVariable = Code.GetUserVariable(pUserVariable->uNext);
			}
		}
		
		Destination  = Head;
		if(uMaxGlobalRegister)
		{
			Destination += pcGlobalKeyword;
			Destination += ALCHEMY_TEXT(' ');
			Destination += m_VariableTypeMap[CEffectCode::VECTOR4_VARIABLE];
			Destination += ALCHEMY_TEXT(' ');
			Destination += pcGlobal;
			Destination += ALCHEMY_TEXT('[');
			Destination += uMaxGlobalRegister >> 4;
			Destination += ALCHEMY_TEXT("];\n");
		}
		
		Destination += ALCHEMY_TEXT("void main(void)\n{\n");
		Destination += Main;
		Destination += ALCHEMY_TEXT('\n');
		Destination += *pCode;
		
		/*if(Type == IEffect::VERTEX_SHADER)
		{
			Destination += ALCHEMY_TEXT("\tgl_Position = ");
			Destination += m_VariableTypeMap[CEffectCode::VECTOR4_VARIABLE];
			Destination += ALCHEMY_TEXT('(');
			Destination += sm_pcVERTEX_SHADER_REGISTER[ICompiler::OUTPUT];
			Destination += ALCHEMY_TEXT('.');
			Destination += sm_cREGISTER_ADDRESS[0];
			Destination += ALCHEMY_TEXT(", ");
			Destination += sm_pcVERTEX_SHADER_REGISTER[ICompiler::OUTPUT];
			Destination += ALCHEMY_TEXT('.');
			Destination += sm_cREGISTER_ADDRESS[1];
			Destination += ALCHEMY_TEXT(", (");
			Destination += sm_pcVERTEX_SHADER_REGISTER[ICompiler::OUTPUT];
			Destination += ALCHEMY_TEXT('.');
			Destination += sm_cREGISTER_ADDRESS[2];
			Destination += ALCHEMY_TEXT(" + ");
			Destination += sm_pcVERTEX_SHADER_REGISTER[ICompiler::OUTPUT];
			Destination += ALCHEMY_TEXT('.');
			Destination += sm_cREGISTER_ADDRESS[3];
			Destination += ALCHEMY_TEXT(") * 0.5, ");
			Destination += sm_pcVERTEX_SHADER_REGISTER[ICompiler::OUTPUT];
			Destination += ALCHEMY_TEXT('.');
			Destination += sm_cREGISTER_ADDRESS[3];
			Destination += ALCHEMY_TEXT(");\n");
		}*/
		
		Destination += ALCHEMY_TEXT("}\n\n");
		
		printf( static_cast<const CHAR*>(Destination) );
		
		return true;
	}
	
	return false;
}

bool CGLSLCompiler::AppendInstruction(
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
	Destination += ALCHEMY_TEXT('\t');
	
	CEffectCode::VARIABLETYPE DestinationVariableType;
	if( __AppendVariable(Destination, 
						 uDestination, 
						 uDestinationIndex, 
						 uDestinationIndexAddress, 
						 puDestinationAddress, 
						 uDestinationAddressCount, 
						 0, 
						 EffectCode, 
						 &DestinationVariableType) )
		Destination += ALCHEMY_TEXT(" = ");
		
	m_Opcode = Instruction;
	
	const GLSLINSTRUCTION& GLSLInstruction = sm_INSTRUCTION[Instruction];
	
	STRING Temp;
	STRING& CurrentDestination = Instruction == ICompiler::TEX ? Temp : Destination;
	if(GLSLInstruction.pcHead)
	{
		Destination += GLSLInstruction.pcHead;
		
		if(Instruction != ICompiler::KIL)
			CurrentDestination += ALCHEMY_TEXT('(');
	}
	
	if(DestinationVariableType <= CEffectCode::VECTOR4_VARIABLE)
		uDestinationAddressCount = uDestinationAddressCount ? uDestinationAddressCount : ( DestinationVariableType / sizeof(FLOAT) );
	
	if(GLSLInstruction.bIsInvert)
		__AppendVariable(CurrentDestination, 
						 uSource2, 
						 uSourceIndex2, 
						 uSourceIndexAddress2, 
						 puAddress2, 
						 uAddressCount2, 
						 uDestinationAddressCount, 
						 EffectCode);
	else
		__AppendVariable(CurrentDestination, 
						 uSource1, 
						 uSourceIndex1, 
						 uSourceIndexAddress1, 
						 puAddress1, 
						 uAddressCount1, 
						 uDestinationAddressCount, 
						 EffectCode);
	
	CurrentDestination += GLSLInstruction.pcOpcode;
	
	/*if(Instruction == ICompiler::SAT)
	{
		if(uDestinationAddressCount > 1)
		{
			-- uDestinationAddressCount;
			
			const CHAR* pcVariableType = m_VariableTypeMap[DestinationVariableType];
			
			CurrentDestination += pcVariableType;
			CurrentDestination += ALCHEMY_TEXT("(0.0");
			
			for(UINT i = 0; i < uDestinationAddressCount; ++ i)
				CurrentDestination += ALCHEMY_TEXT(", 0.0");
				
			CurrentDestination += ALCHEMY_TEXT("), ");

			CurrentDestination += pcVariableType;
			CurrentDestination += ALCHEMY_TEXT("(1.0");
			
			for(UINT i = 0; i < uDestinationAddressCount; ++ i)
				CurrentDestination += ALCHEMY_TEXT(", 1.0");
				
			CurrentDestination += ALCHEMY_TEXT(")");
		}
		else
			CurrentDestination += ALCHEMY_TEXT("0.0, 1.0");
	}
	else*/
	{
		if(GLSLInstruction.bIsInvert)
			__AppendVariable(CurrentDestination, 
							 uSource1, 
							 uSourceIndex1, 
							 uSourceIndexAddress1, 
							 puAddress1, 
							 uAddressCount1, 
							 uDestinationAddressCount, 
							 EffectCode);
		else
			__AppendVariable(CurrentDestination, 
							 uSource2, 
							 uSourceIndex2, 
							 uSourceIndexAddress2, 
							 puAddress2, 
							 uAddressCount2, 
							 uDestinationAddressCount, 
							 EffectCode);
	}
	
	CurrentDestination += GLSLInstruction.pcTail;
	
	if(GLSLInstruction.pcHead && Instruction != ICompiler::KIL)
		CurrentDestination += ALCHEMY_TEXT(')');
	
	if(Instruction == ICompiler::TEX)
	{
		Destination += sm_pcSAMPLER_TYPE[m_SamplerType]; 
		
		m_SamplerType = IEffect::TOTAL_SAMPLER_TYPE;
		
		Destination += Temp;
		
		if(uDestinationAddressCount && uDestinationAddressCount < 4)
		{
			Destination += ALCHEMY_TEXT('.');
			for(UINT i = 0; i < uDestinationAddressCount; ++ i)
				Destination += sm_cREGISTER_ADDRESS[i];
		}
	}
	
	Destination += ALCHEMY_TEXT(";\n");
	
	//Destination += Temp;
	
	m_Opcode = ICompiler::TOTAL_INSTRUCTION;
	
	return true;
}

bool CGLSLCompiler::ReplaceVariable(
                                    STRING& Code, 
                                    UINT uSource, 
                                    const UINT* puSourceAddress, 
                                    UINT uDestination, 
                                    const UINT* puDestinationAddress, 
                                    UINT uAddressCount, 
                                    UINT uMinDestinationAddress,  
                                    const CEffectCode& EffectCode, 
                                    UINT uCodeFrom, 
                                    UINT uCodeLength)
{
	UINT uFrom = Code.GetIndexOf(ALCHEMY_TEXT('='), uCodeFrom, uCodeLength);
	if(uFrom < uCodeFrom)
	{
		uCodeFrom = uFrom + 1;
		
		uFrom = Code.GetIndexOf(sm_INSTRUCTION[ICompiler::TEX].pcHead, uCodeFrom, uCodeLength);
		if(uFrom < uCodeFrom)
		{
			static const UINT s_uTEXTURE_LENGTH = ALCHEMY_STRING_LENGTH(sm_INSTRUCTION[ICompiler::TEX].pcHead);
			
			uFrom += s_uTEXTURE_LENGTH;
			
#define ALCHEMY_SWITCH_SAMPLER(Type) \
			if(Code[uFrom] == sm_pcSAMPLER_TYPE[Type][0]) \
			{ \
				if( Code.IsEqual(sm_pcSAMPLER_TYPE[Type], uFrom) ) \
				{ \
					m_SamplerType = static_cast<IEffect::SAMPLERTYPE>(Type); \
				} \
			}
			
			for(UINT i = 0; i < IEffect::TOTAL_SAMPLER_TYPE; ++ i)
			{
				ALCHEMY_SWITCH_SAMPLER(i)
				
				if(m_SamplerType != IEffect::TOTAL_SAMPLER_TYPE)
					break;
			}
#undef ALCHEMY_SWITCH_SAMPLER

			uCodeFrom = uFrom + ALCHEMY_STRING_LENGTH(sm_pcSAMPLER_TYPE[m_SamplerType]);
		}
	}
	
	STRING Source, Destination;
	__AppendVariable(Source     , uSource     , 0, 0, puSourceAddress     , uAddressCount, 0, EffectCode);
	__AppendVariable(Destination, uDestination, 0, 0, puDestinationAddress, uAddressCount, 0, EffectCode);
	
	m_SamplerType = IEffect::TOTAL_SAMPLER_TYPE;
	
	if( Source.GetLength() && Destination.GetLength() )
	{
		uFrom = Code.GetIndexOf(Source, uCodeFrom, uCodeLength);
		
		if(uFrom < uCodeLength)
		{
			UINT uSourceLength = Source.GetLength();
			Code.Replace(uFrom, uSourceLength, Destination);
		
			UINT uDestinationLength = Destination.GetLength();
			const CHAR cSourceAddress = Source[uSourceLength - 1], cDestinationAddress = Destination[uDestinationLength - 1];
			for(UINT i = uFrom + uDestinationLength; i < uCodeLength && Code[i] == cSourceAddress; ++ i)
				Code[i] = cDestinationAddress;
		}
			
		return Code.Replace(Source, Destination, uCodeFrom, uCodeLength) != Code.GetLength();
	}
	
	return false; 
}

bool CGLSLCompiler::AppendRegisterName(STRING& Destination, UINT uRegister, ICompiler::REGISTERTYPE RegisterType, IEffect::EFFECTTYPE ShaderType)const
{
	if(ShaderType == IEffect::VERTEX_SHADER)
		Destination += sm_pcVERTEX_SHADER_REGISTER[RegisterType];
	else if(ShaderType == IEffect::FRAGMENT_SHADER)
		Destination += sm_pcFRAGMENT_SHADER_REGISTER[RegisterType];
	else 
	{
		///...
	}

	if(RegisterType != ICompiler::OUTPUT)
	{
		if(RegisterType == ICompiler::SAMPLER)
			Destination += uRegister;
		else
			Destination += uRegister >> 4;
	}
	
	return true;
}

UINT CGLSLCompiler::ComputeRegisterLocation(UINT uRegister, UINT uOffset, UINT uMinRegister, UINT uSamplerCount, UINT uConstantSamplerCount)const
{
	return (uConstantSamplerCount << 2) + ( (uRegister + uOffset) >> 2 );
}

bool CGLSLCompiler::IsInverceInstruction(ICompiler::INSTRUCTION Opcode)const
{
	return sm_INSTRUCTION[Opcode].bIsInvert;
}

bool CGLSLCompiler::__AppendVariable(STRING& Destination, 
									 UINT uVariable, 
									 UINT uVariableIndex, 
									 UINT uIndexAddress, 
									 const UINT* puAddress, 
									 UINT uAddressCount, 
									 UINT uDestinationAddressCount,
									 const CEffectCode& EffectCode, 
									 CEffectCode::VARIABLETYPE* pVariableType)
{
	ICompiler::REGISTERTYPE Type = EffectCode.GetRegisterType(uVariable);
	if(Type)
	{
		const CEffectCode::VARIABLE* pVariable = EffectCode.GetVariable(uVariable);
		if(pVariable)
		{
			if(pVariableType)
				*pVariableType = pVariable->Type;
			
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
			
			const CHAR* pcRegister;
			if(ShaderType == IEffect::VERTEX_SHADER)
			{
				pcRegister = sm_pcVERTEX_SHADER_REGISTER[Type];
			}
			else if(ShaderType == IEffect::FRAGMENT_SHADER)
			{
				pcRegister = sm_pcFRAGMENT_SHADER_REGISTER[Type];
			}
			else
			{
				/////////////////////////////////////////////////////////////
				////////////∫Ø ˝µ˜”√ Œ¥ÕÍ≥…
				/////////////////////////////////////////////////////////////
			}
			
			if(Type == ICompiler::SAMPLER)
			{
				const IEffect::SAMPLERDECLARATION* pSampler = EffectCode.GetSamplerDeclaration(pVariable->uDeclaration);
				if(pSampler)
				{
					Destination += pcRegister;
					Destination += pVariable->uRegister;
					
					return __AppendSampler(Destination, *pSampler);
				}
				
				ALCHEMY_DEBUG_WARNING("CGLSLCompiler::__AppendVariable: Error sampler variable.");
			}
			else
			{
				UINT uRegister = 0, uGlobalIndex = 0;
				CEffectCode::VARIABLETYPE VariableType = CEffectCode::UNKNOWN_VARIABLE;
				while(pVariable && pVariable->Type != CEffectCode::STRUCT_VARIABLE)
				{
					uRegister += pVariable->uRegister;
					
					VariableType = pVariable->Type;
					
					if(pVariable->uParent)
						uGlobalIndex += pVariable->uGlobal;
					
					pVariable = EffectCode.GetVariable(pVariable->uParent);
				}
				
				UINT uCount = 1;
				if(Type == ICompiler::TEMPORARY)
					uCount = EffectCode.GetTemporaryVariableCount(uVariable);
				
				ICompiler::REGISTERTYPE IndexType = EffectCode.GetRegisterType(uVariableIndex);
				if(IndexType)
				{
					pVariable = EffectCode.GetVariable(uVariableIndex);
					if(pVariable)
					{
						//Destination += ALCHEMY_TEXT('[');
						
						ShaderType = IEffect::PROGRAM;
						const CEffectCode::FUNCTION* pFunction = EffectCode.GetFunction(pVariable->uFunction);
						if(pFunction)
						{
							const CEffectCode::SHADERFUNCTION* pShaderFunction = EffectCode.GetShaderFunction(pFunction->uShader);
							if(pShaderFunction)
								ShaderType = pShaderFunction->Type;
						}
						
						const CHAR* pcIndexRegister;
						if(ShaderType == IEffect::VERTEX_SHADER)
						{
							pcIndexRegister = sm_pcVERTEX_SHADER_REGISTER[IndexType];
						}
						else if(ShaderType == IEffect::FRAGMENT_SHADER)
						{
							pcIndexRegister = sm_pcFRAGMENT_SHADER_REGISTER[IndexType];
						}
						else
						{
							/////////////////////////////////////////////////////////////
							////////////∫Ø ˝µ˜”√ Œ¥ÕÍ≥…
							/////////////////////////////////////////////////////////////
						}
						
						UINT uIndexRegister = 0, uIndexGlobalIndex = 0;
						CEffectCode::VARIABLETYPE IndexVariableType = CEffectCode::UNKNOWN_VARIABLE;
						while(pVariable && pVariable->Type != CEffectCode::STRUCT_VARIABLE)
						{
							uIndexRegister += pVariable->uRegister;
							
							IndexVariableType = pVariable->Type;
							
							if(pVariable->uParent)
								uIndexGlobalIndex += pVariable->uGlobal;
							
							pVariable = EffectCode.GetVariable(pVariable->uParent);
						}
						
						STRING IndexDestination;
						__AppendRegister(IndexDestination, 
										 IndexVariableType, 
										 uIndexRegister, 
										 &uIndexAddress, 
										 1, 
										 0, 
										 pcIndexRegister, 
										 IndexType, 
										 IndexType == ICompiler::TEMPORARY ? EffectCode.GetTemporaryVariableCount(uVariable) : 1, 
										 uIndexGlobalIndex, 
										 EffectCode);
						
						__AppendRegister(Destination, 
										 VariableType, 
										 uRegister, 
										 puAddress, 
										 uAddressCount, 
										 pcRegister, 
										 IndexDestination, 
										 uCount, 
										 uGlobalIndex, 
										 EffectCode);
					}
					else
					{
						__AppendRegister(Destination, 
										 VariableType, 
										 uRegister, 
										 puAddress, 
										 uAddressCount, 
										 uDestinationAddressCount, 
										 pcRegister, 
										 Type, 
										 uCount, 
										 uGlobalIndex, 
										 EffectCode);
					}
				}
				else
					__AppendRegister(Destination, 
									 VariableType, 
									 uRegister, 
									 puAddress, 
									 uAddressCount, 
									 uDestinationAddressCount, 
									 pcRegister, 
									 Type, 
									 uCount, 
									 uGlobalIndex, 
									 EffectCode);
				
				return true;
			}
		}
		else
			ALCHEMY_DEBUG_WARNING("CGLSLCompiler::__AppendVariable: Error variable.");

	}
	else
		ALCHEMY_DEBUG_WARNING("CGLSLCompiler::__AppendVariable: Error variable.");
	
	return false;
}

bool CGLSLCompiler::__AppendSampler(STRING& Destiantion, const IEffect::SAMPLERDECLARATION& Sampler)
{
	m_SamplerType = Sampler.Type;
	
	return true;
}

void CGLSLCompiler::__AppendRegister(STRING& Destiantion, 
									 CEffectCode::VARIABLETYPE VariableType, 
									 UINT uRegister, 
									 const UINT* puAddress, 
									 UINT uAddressCount, 
									 UINT uDestinationAddressCount, 
									 const CHAR* pcRegister, 
									 ICompiler::REGISTERTYPE RegisterType, 
									 UINT uCount, 
									 UINT uIndex, 
									 const CEffectCode& EffectCode)
{
	uRegister >>= 2;
	
	UINT uTemp = uRegister >> 2;
	
	STRING Temp;
	STRING& CurrentDestination = m_SamplerType == IEffect::TEXTURE2D ? Temp : Destiantion;
	if(RegisterType == ICompiler::OUTPUT)
		CurrentDestination += pcRegister;
	else
	{
		if(RegisterType == ICompiler::GLOBAL)
		{
			switch (m_Opcode) 
			{
				case ICompiler::M33:
					CurrentDestination += m_VariableTypeMap[CEffectCode::MATRIX3X3_VARIABLE];
					CurrentDestination += ALCHEMY_TEXT('(');
					CurrentDestination += pcRegister;
					CurrentDestination += ALCHEMY_TEXT('[');
					CurrentDestination += uTemp;
					CurrentDestination += ALCHEMY_TEXT("].xyz, ");
					CurrentDestination += pcRegister;
					CurrentDestination += ALCHEMY_TEXT('[');
					CurrentDestination += uTemp + 1;
					CurrentDestination += ALCHEMY_TEXT("].xyz, ");
					CurrentDestination += pcRegister;
					CurrentDestination += ALCHEMY_TEXT('[');
					CurrentDestination += uTemp + 2;
					CurrentDestination += ALCHEMY_TEXT("].xyz)");
					break;
				case ICompiler::M34:
					CurrentDestination += m_VariableTypeMap[CEffectCode::MATRIX4X3_VARIABLE];
					CurrentDestination += ALCHEMY_TEXT("( ");
					CurrentDestination += pcRegister;
					CurrentDestination += ALCHEMY_TEXT('[');
					CurrentDestination += uTemp;
					CurrentDestination += ALCHEMY_TEXT("], ");
					CurrentDestination += pcRegister;
					CurrentDestination += ALCHEMY_TEXT('[');
					CurrentDestination += uTemp + 1;
					CurrentDestination += ALCHEMY_TEXT("], ");
					CurrentDestination += pcRegister;
					CurrentDestination += ALCHEMY_TEXT('[');
					CurrentDestination += uTemp + 2;
					CurrentDestination += ALCHEMY_TEXT("], ");
					CurrentDestination += m_VariableTypeMap[CEffectCode::VECTOR4_VARIABLE];
					CurrentDestination += ALCHEMY_TEXT("(0.0, 0.0, 0.0, 1.0) )");
					break;
				case ICompiler::M44:
					CurrentDestination += m_VariableTypeMap[CEffectCode::MATRIX4X3_VARIABLE];
					CurrentDestination += ALCHEMY_TEXT('(');
					CurrentDestination += pcRegister;
					CurrentDestination += ALCHEMY_TEXT('[');
					CurrentDestination += uTemp;
					CurrentDestination += ALCHEMY_TEXT("], ");
					CurrentDestination += pcRegister;
					CurrentDestination += ALCHEMY_TEXT('[');
					CurrentDestination += uTemp + 1;
					CurrentDestination += ALCHEMY_TEXT("], ");
					CurrentDestination += pcRegister;
					CurrentDestination += ALCHEMY_TEXT('[');
					CurrentDestination += uTemp + 2;
					CurrentDestination += ALCHEMY_TEXT("], ");
					CurrentDestination += pcRegister;
					CurrentDestination += ALCHEMY_TEXT('[');
					CurrentDestination += uTemp + 3;
					CurrentDestination += ALCHEMY_TEXT("])");
					break;
				default:
					CurrentDestination += pcRegister;
					CurrentDestination += ALCHEMY_TEXT('[');
					CurrentDestination += uTemp;
					CurrentDestination += ALCHEMY_TEXT(']');
					break;
			}
		}
		else 
		{
			if(VariableType == CEffectCode::VECTOR1_VARIABLE && uDestinationAddressCount > 1 && m_SamplerType != IEffect::TEXTURE2D)
			{
				CurrentDestination += m_VariableTypeMap[static_cast<CEffectCode::VARIABLETYPE>( uDestinationAddressCount * sizeof(FLOAT) )];
				CurrentDestination += ALCHEMY_TEXT('(');
				
				-- uDestinationAddressCount;
				if(RegisterType == ICompiler::TEMPORARY)
				{
					const CHAR** ppcVariableType = m_VariableTypeMap.Get(VariableType);
					if(ppcVariableType)
					{
						UINT uOffset = uRegister - (uTemp << 2), uRegisterCount = (uIndex < uCount && uCount > 1) ? ( ( uIndex * EffectCode.GetRegisterCount(VariableType) ) >> 2 ) : (uTemp + 1);
						
						if(uRegisterCount < uTemp)
						{
							CurrentDestination += pcRegister;
							CurrentDestination += ALCHEMY_TEXT('_');
							CurrentDestination += *ppcVariableType;
							CurrentDestination += ALCHEMY_TEXT('_');
						
							CurrentDestination += uTemp - uRegisterCount;
							CurrentDestination += ALCHEMY_TEXT('_');
							CurrentDestination += uOffset;
							
							CurrentDestination += ALCHEMY_TEXT('_');
							CurrentDestination += uCount;
							CurrentDestination += ALCHEMY_TEXT('[');
							CurrentDestination += uIndex;
							CurrentDestination += ALCHEMY_TEXT(']');
							
							for(UINT i = 0; i < uDestinationAddressCount; ++ i)
							{
								CurrentDestination += ALCHEMY_TEXT(", ");
								CurrentDestination += pcRegister;
								CurrentDestination += ALCHEMY_TEXT('_');
								CurrentDestination += *ppcVariableType;
								CurrentDestination += ALCHEMY_TEXT('_');
							
								CurrentDestination += uTemp - uRegisterCount;
								CurrentDestination += ALCHEMY_TEXT('_');
								CurrentDestination += uOffset;
								
								CurrentDestination += ALCHEMY_TEXT('_');
								CurrentDestination += uCount;
								CurrentDestination += ALCHEMY_TEXT('[');
								CurrentDestination += uIndex;
								CurrentDestination += ALCHEMY_TEXT(']');
							}
						}
						else 
						{
							CurrentDestination += pcRegister;
							CurrentDestination += ALCHEMY_TEXT('_');
							CurrentDestination += *ppcVariableType;
							CurrentDestination += ALCHEMY_TEXT('_');
							
							CurrentDestination += uTemp;
							
							CurrentDestination += ALCHEMY_TEXT('_');
							CurrentDestination += uOffset;
							
							for(UINT i = 0; i < uDestinationAddressCount; ++ i)
							{
								CurrentDestination += ALCHEMY_TEXT(", ");
								CurrentDestination += pcRegister;
								CurrentDestination += ALCHEMY_TEXT('_');
								CurrentDestination += *ppcVariableType;
								CurrentDestination += ALCHEMY_TEXT('_');
								
								CurrentDestination += uTemp;
								
								CurrentDestination += ALCHEMY_TEXT('_');
								CurrentDestination += uOffset;
							}
						}
					}
					else 
						ALCHEMY_DEBUG_WARNING("CGLSLCompiler::__AppendRegister: Error variable tye.");
				}
				else 
				{
					CurrentDestination += pcRegister;
					CurrentDestination += uTemp;
					
					for(UINT i = 0; i < uDestinationAddressCount; ++ i)
					{
						CurrentDestination += ALCHEMY_TEXT(", ");
						CurrentDestination += pcRegister;
						CurrentDestination += uTemp;
					}
				}
				
				CurrentDestination += ALCHEMY_TEXT(')');
				
				return;
			}
			else if(RegisterType == ICompiler::TEMPORARY)
			{
				const CHAR** ppcVariableType = m_VariableTypeMap.Get(VariableType);
				if(ppcVariableType)
				{
					UINT uOffset = uRegister - (uTemp << 2);
					
					CurrentDestination += pcRegister;
					CurrentDestination += ALCHEMY_TEXT('_');
					CurrentDestination += *ppcVariableType;
					CurrentDestination += ALCHEMY_TEXT('_');
					
					UINT uRegisterCount = (uIndex < uCount && uCount > 1) ? ( ( uIndex * EffectCode.GetRegisterCount(VariableType) ) >> 2 ) : (uTemp + 1);
					if(uRegisterCount < uTemp)
					{
						CurrentDestination += uTemp - uRegisterCount;
						CurrentDestination += ALCHEMY_TEXT('_');
						CurrentDestination += uOffset;
						
						CurrentDestination += ALCHEMY_TEXT('_');
						CurrentDestination += uCount;
						CurrentDestination += ALCHEMY_TEXT('[');
						CurrentDestination += uIndex;
						CurrentDestination += ALCHEMY_TEXT(']');
					}
					else
					{
						CurrentDestination += uTemp;
						
						CurrentDestination += ALCHEMY_TEXT('_');
						CurrentDestination += uOffset;
					}
				}
				else 
					ALCHEMY_DEBUG_WARNING("CGLSLCompiler::__AppendRegister: Error variable tye.");

			}
			else 
			{
				CurrentDestination += pcRegister;
				CurrentDestination += uTemp;
			}
			
			if(VariableType == CEffectCode::VECTOR1_VARIABLE)
				return;
		}
	}
	
	if(m_SamplerType == IEffect::TEXTURE2D)
	{
		Destiantion += m_VariableTypeMap[CEffectCode::VECTOR2_VARIABLE];
		Destiantion += ALCHEMY_TEXT('(');
		Destiantion += Temp;
		Destiantion += ALCHEMY_TEXT('.');
		
		CHAR pcRegisterAddress;
		if(uAddressCount)
		{
			pcRegisterAddress = sm_cREGISTER_ADDRESS[puAddress[0]];
			
			Destiantion += pcRegisterAddress;
			
			if(uAddressCount > 1)
				pcRegisterAddress = sm_cREGISTER_ADDRESS[puAddress[1]];
		}
		else
		{
			Destiantion += sm_cREGISTER_ADDRESS[0];
			
			pcRegisterAddress = sm_cREGISTER_ADDRESS[1];
		}
		
		Destiantion += ALCHEMY_TEXT(", 1.0 - ");
		Destiantion += Temp;
		Destiantion += ALCHEMY_TEXT('.');
		Destiantion += pcRegisterAddress;
		Destiantion += ALCHEMY_TEXT(')');
		
		return;
	}
	
	//Destiantion += Temp;
	
	uTemp = RegisterType == ICompiler::GLOBAL ? ( uRegister - (uTemp << 2) ) : 0;
	
	uAddressCount = ALCHEMY_MININUM(4u, uAddressCount);
	
	UINT uCurrentAddressCount = uAddressCount;
	if(uAddressCount < uDestinationAddressCount && m_SamplerType == IEffect::TOTAL_SAMPLER_TYPE)
	{
		if(!uAddressCount)
		{
			if(RegisterType == ICompiler::GLOBAL)
				return;
			
			uAddressCount = VariableType / sizeof(FLOAT);
			if(uAddressCount == uDestinationAddressCount)
				return;
			
			const UINT uADDRESS[] = {0, 1, 2, 3};
			
			puAddress = uADDRESS;
		}
		
		uCurrentAddressCount = uAddressCount < uDestinationAddressCount ? uDestinationAddressCount : uAddressCount;
	}
	
	/*if(m_Opcode == ICompiler::NEG)
	{
		if(VariableType == CEffectCode::VECTOR1_VARIABLE)
		{
			Destiantion += ALCHEMY_TEXT(" - ");
			Destiantion += Temp;
		}
		else if(uTemp)
		{
			if(puAddress && uCurrentAddressCount)
			{
				Destiantion += m_VariableTypeMap[static_cast<CEffectCode::VARIABLETYPE>( uCurrentAddressCount * sizeof(FLOAT) )];
				Destiantion += ALCHEMY_TEXT('(');
				UINT uAddress = puAddress[0] + uTemp;
				if(uAddress < 4)
				{
					Destiantion += ALCHEMY_TEXT("- ");
					Destiantion += Temp;
					Destiantion += ALCHEMY_TEXT('.');
					Destiantion += sm_cREGISTER_ADDRESS[uAddress];
				}
				
				UINT uMaxAddress = uAddressCount - 1;
				for(UINT i = 1; i < uCurrentAddressCount; ++ i)
				{
					uAddress = puAddress[i < uAddressCount ? i : uMaxAddress] + uTemp;
					if(uAddress < 4)
					{
						Destiantion += ALCHEMY_TEXT(", - ");
						Destiantion += Temp;
						Destiantion += ALCHEMY_TEXT('.');
						Destiantion += sm_cREGISTER_ADDRESS[uAddress];
					}
				}
				
				Destiantion += ALCHEMY_TEXT(')');
			}
			else
			{
				Destiantion += ALCHEMY_TEXT(" - ");
				Destiantion += Temp;
				Destiantion += ALCHEMY_TEXT('.');
				Destiantion += uTemp;
			}
		}
		else if(puAddress && uCurrentAddressCount)
		{
			Destiantion += m_VariableTypeMap[static_cast<CEffectCode::VARIABLETYPE>( uCurrentAddressCount * sizeof(FLOAT) )];
			Destiantion += ALCHEMY_TEXT('(');
			UINT uAddress = puAddress[0] + uTemp;
			if(uAddress < 4)
			{
				Destiantion += ALCHEMY_TEXT("- ");
				Destiantion += Temp;
				Destiantion += ALCHEMY_TEXT('.');
				Destiantion += sm_cREGISTER_ADDRESS[uAddress];
			}
			
			UINT uMaxAddress = uAddressCount - 1;
			for(UINT i = 1; i < uCurrentAddressCount; ++ i)
			{
				uAddress = puAddress[i < uAddressCount ? i : uMaxAddress] + uTemp;
				if(uAddress < 4)
				{
					Destiantion += ALCHEMY_TEXT(", - ");
					Destiantion += Temp;
					Destiantion += ALCHEMY_TEXT('.');
					Destiantion += sm_cREGISTER_ADDRESS[uAddress];
				}
			}
			
			Destiantion += ALCHEMY_TEXT(')');
		}
		
		return;
	}*/
	
	//UINT uCurrentAddressCount = ALCHEMY_MAXINUM(uDestinationAddressCount, uAddressCount);
	if(uTemp)
	{
		Destiantion += ALCHEMY_TEXT('.');
		if(puAddress && uCurrentAddressCount)
		{
			UINT uAddress, uMaxAddress = uAddressCount - 1;
			for(UINT i = 0; i < uCurrentAddressCount; ++ i)
			{
				uAddress = puAddress[i < uAddressCount ? i : uMaxAddress] + uTemp;
				if(uAddress < 4)
					Destiantion += sm_cREGISTER_ADDRESS[uAddress];
			}
		}
		else
			Destiantion += sm_cREGISTER_ADDRESS[uTemp];
	}
	else if(puAddress && uCurrentAddressCount)
	{
		Destiantion += ALCHEMY_TEXT('.');
		
		UINT uAddress, uMaxAddress = uAddressCount - 1;
		for(UINT i = 0; i < uCurrentAddressCount; ++ i)
		{
			uAddress = puAddress[i < uAddressCount ? i : uMaxAddress];
			if(uAddress < 4)
				Destiantion += sm_cREGISTER_ADDRESS[uAddress];
		}
	}
}

void CGLSLCompiler::__AppendRegister(STRING& Destiantion, 
									 CEffectCode::VARIABLETYPE Type, 
									 UINT uRegister, 
									 const UINT* puAddress, 
									 UINT uAddressCount, 
									 const CHAR* pcRegister, 
									 const STRING& IndexDestiantion,
									 UINT uCount, 
									 UINT uIndex, 
									 const CEffectCode& EffectCode)
{
	//Destiantion += ALCHEMY_TEXT('[');
	
	uRegister >>= 2;
	
	UINT /*uCurrentReigster*/uTemp = uRegister >> 2;
	
	//Destiantion += uCurrentRegister;
	
	//Destiantion += ALCHEMY_TEXT(" + ");
	
	//Destiantion += pcIndexRegister;
	
	//uIndexRegister >>= 2;
	
	//UINT uCurrentIndexRegister = uIndexRegister >> 2;
	
	//Destiantion += uCurrentIndexRegister;
	
	//UINT uTemp = uIndexRegister - (uCurrentIndexRegister << 2);
	
	//Destiantion += ALCHEMY_TEXT('.');
	//Destiantion += sm_cREGISTER_ADDRESS[uTemp ? uTemp : uIndexAddress];
	
	//Destiantion += ALCHEMY_TEXT(']');
	//UINT uTemp = uRegister - (uCurrentRegister << 2);
	
	if(uCount > 1)
	{
		const CHAR** ppcVariableType = m_VariableTypeMap.Get(Type);
		if(ppcVariableType)
		{
			uIndex = ALCHEMY_MININUM(uIndex, uCount - 1);
			UINT uRegisterCount = ( uIndex * EffectCode.GetRegisterCount(Type) ) >> 2;
			Destiantion += pcRegister;
			Destiantion += ALCHEMY_TEXT('_');
			Destiantion += *ppcVariableType;
			Destiantion += ALCHEMY_TEXT('_');
			Destiantion += uTemp - uRegisterCount;
			Destiantion += ALCHEMY_TEXT('_');
			Destiantion += uCount;
			Destiantion += ALCHEMY_TEXT('[');
			Destiantion += uIndex;
			Destiantion += ALCHEMY_TEXT(" + ");
			Destiantion += IndexDestiantion;
			Destiantion += ALCHEMY_TEXT(']');
		}
		else 
			ALCHEMY_DEBUG_WARNING("CGLSLCompiler::__AppendRegister: Error variable type.");
	}
	else
	{
		switch (m_Opcode) 
		{
			case ICompiler::M33:
				Destiantion += m_VariableTypeMap[CEffectCode::MATRIX3X3_VARIABLE];
				Destiantion += ALCHEMY_TEXT('(');
				Destiantion += pcRegister;
				Destiantion += ALCHEMY_TEXT('[');
				Destiantion += uTemp;
				Destiantion += ALCHEMY_TEXT(" + int(");
				Destiantion += IndexDestiantion;
				Destiantion += ALCHEMY_TEXT(")].xyz, ");
				Destiantion += pcRegister;
				Destiantion += ALCHEMY_TEXT('[');
				Destiantion += uTemp + 1;
				Destiantion += ALCHEMY_TEXT(" + int(");
				Destiantion += IndexDestiantion;
				Destiantion += ALCHEMY_TEXT(")].xyz, ");
				Destiantion += pcRegister;
				Destiantion += ALCHEMY_TEXT('[');
				Destiantion += uTemp + 2;
				Destiantion += ALCHEMY_TEXT(" + int(");
				Destiantion += IndexDestiantion;
				Destiantion += ALCHEMY_TEXT(")].xyz)");
				break;
			case ICompiler::M34:
				Destiantion += m_VariableTypeMap[CEffectCode::MATRIX4X3_VARIABLE];
				Destiantion += ALCHEMY_TEXT("( ");
				Destiantion += pcRegister;
				Destiantion += ALCHEMY_TEXT('[');
				Destiantion += uTemp;
				Destiantion += ALCHEMY_TEXT(" + int(");
				Destiantion += IndexDestiantion;
				Destiantion += ALCHEMY_TEXT(")], ");
				Destiantion += pcRegister;
				Destiantion += ALCHEMY_TEXT('[');
				Destiantion += uTemp + 1;
				Destiantion += ALCHEMY_TEXT(" + int(");
				Destiantion += IndexDestiantion;
				Destiantion += ALCHEMY_TEXT(")], ");
				Destiantion += pcRegister;
				Destiantion += ALCHEMY_TEXT('[');
				Destiantion += uTemp + 2;
				Destiantion += ALCHEMY_TEXT(" + int(");
				Destiantion += IndexDestiantion;
				Destiantion += ALCHEMY_TEXT(")], ");
				Destiantion += m_VariableTypeMap[CEffectCode::VECTOR4_VARIABLE];
				Destiantion += ALCHEMY_TEXT("(0.0, 0.0, 0.0, 1.0) )");
				break;
			case ICompiler::M44:
				Destiantion += m_VariableTypeMap[CEffectCode::MATRIX4X3_VARIABLE];
				Destiantion += ALCHEMY_TEXT('(');
				Destiantion += pcRegister;
				Destiantion += ALCHEMY_TEXT('[');
				Destiantion += uTemp;
				Destiantion += ALCHEMY_TEXT(" + int(");
				Destiantion += IndexDestiantion;
				Destiantion += ALCHEMY_TEXT(")], ");
				Destiantion += pcRegister;
				Destiantion += ALCHEMY_TEXT('[');
				Destiantion += uTemp + 1;
				Destiantion += ALCHEMY_TEXT(" + int(");
				Destiantion += IndexDestiantion;
				Destiantion += ALCHEMY_TEXT(")], ");
				Destiantion += pcRegister;
				Destiantion += ALCHEMY_TEXT('[');
				Destiantion += uTemp + 2;
				Destiantion += ALCHEMY_TEXT(" + int(");
				Destiantion += IndexDestiantion;
				Destiantion += ALCHEMY_TEXT(")], ");
				Destiantion += pcRegister;
				Destiantion += ALCHEMY_TEXT('[');
				Destiantion += uTemp + 3;
				Destiantion += ALCHEMY_TEXT(" + int(");
				Destiantion += IndexDestiantion;
				Destiantion += ALCHEMY_TEXT(")])");
				break;
			default:
				Destiantion += pcRegister;
				Destiantion += ALCHEMY_TEXT('[');
				Destiantion += uTemp;
				Destiantion += ALCHEMY_TEXT(" + ");
				Destiantion += IndexDestiantion;
				Destiantion += ALCHEMY_TEXT(']');
				break;
		}
	}
	
	uTemp = uRegister - (uTemp << 2);
	
	uAddressCount = ALCHEMY_MININUM(4u, uAddressCount);
	if(uTemp)
	{
		Destiantion += ALCHEMY_TEXT('.');
		if(puAddress && uAddressCount)
		{
			UINT uAddress;
			for(UINT i = 0; i < uAddressCount; ++ i)
			{
				uAddress = puAddress[i] + uTemp;
				if(uAddress < 4)
					Destiantion += sm_cREGISTER_ADDRESS[uAddress];
			}
		}
		else
			Destiantion += sm_cREGISTER_ADDRESS[uTemp];
	}
	else if(puAddress && uAddressCount)
	{
		Destiantion += ALCHEMY_TEXT('.');
		
		UINT uAddress;
		for(UINT i = 0; i < uAddressCount; ++ i)
		{
			uAddress = puAddress[i];
			if(uAddress < 4)
				Destiantion += sm_cREGISTER_ADDRESS[uAddress];
		}
	}	
}