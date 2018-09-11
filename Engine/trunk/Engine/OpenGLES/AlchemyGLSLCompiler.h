#pragma once

#include "../Graphics/AlchemyCompilerInterface.h"
#include "../Graphics/AlchemyEffectCode.h"

namespace alchemy 
{
    class CGLSLCompiler :
    public CSmartClass<ICompiler>
    {
    public:
        CGLSLCompiler();
        ~CGLSLCompiler();
        
        const ICompiler::COMPILERINFO& GetCompilerInfo()const;
        
		UINT GetAddressShift(UINT uVariable, const CEffectCode& EffectCode)const;
        
		bool ProgramString(CBuffer** ppOutput, IEffect::EFFECTTYPE Type, const CHAR* pcSource);
        
		bool BuildShaderFunctionCode(STRING& Desination, IEffect::EFFECTTYPE Type, UINT uUserVariables, UINT uCode, const CEffectCode& Code);
        
		bool AppendInstruction(STRING& Destination, 
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
                               CEffectCode& EffectCode);
        
		bool ReplaceVariable(STRING& Code, 
                             UINT uSource, 
                             const UINT* puSourceAddress, 
                             UINT uDestiantion, 
                             const UINT* puDestiantionAddress, 
                             UINT uAddressCount, 
                             UINT uMinDestinationAddress,  
                             const CEffectCode& EffectCode, 
                             UINT uCodeFrom, 
                             UINT uCodeLength);
		
		bool AppendRegisterName(STRING& Destination, UINT uRegister, ICompiler::REGISTERTYPE RegisterType, IEffect::EFFECTTYPE ShaderType)const;
		
		UINT ComputeRegisterLocation(UINT uRegister, UINT uOffset, UINT uMinRegister, UINT uSamplerCount, UINT uConstantSamplerCount)const;
		
		bool IsInverceInstruction(ICompiler::INSTRUCTION Opcode)const;
		
	private:
		typedef struct
		{
			const CHAR* pcHead;
			const CHAR* pcOpcode;
			const CHAR* pcTail;
			bool        bIsInvert;
		}GLSLINSTRUCTION;
		
		static const GLSLINSTRUCTION sm_INSTRUCTION[ICompiler::TOTAL_INSTRUCTION];
	
		static const CHAR* sm_pcVERTEX_SHADER_REGISTER[ICompiler::TOTAL_REGISTERTYPE];
		static const CHAR* sm_pcFRAGMENT_SHADER_REGISTER[ICompiler::TOTAL_REGISTERTYPE];
		static const CHAR sm_cREGISTER_ADDRESS[5];
		
		static const CHAR* sm_pcSAMPLER_TYPE[IEffect::TOTAL_SAMPLER_TYPE];

		bool __AppendVariable(STRING& Destination, 
							  UINT uVariable, 
							  UINT uVariableIndex, 
							  UINT uIndexAddress, 
							  const UINT* puAddress, 
							  UINT uAddressCount, 
							  UINT uDestinationAddressCount,
							  const CEffectCode& EffectCode, 
							  CEffectCode::VARIABLETYPE* pVariableType = ALCHEMY_NULL);
		
		bool __AppendSampler(STRING& Destiantion, 
							 const IEffect::SAMPLERDECLARATION& Sampler);
		
		void __AppendRegister(STRING& Destiantion, 
							  CEffectCode::VARIABLETYPE VariableType, 
							  UINT uRegister, 
							  const UINT* puAddress, 
							  UINT uAddressCount, 
							  UINT uDestinationAddressCount, 
							  const CHAR* pcRegister, 
							  ICompiler::REGISTERTYPE RegisterType, 
							  UINT uCount, 
							  UINT uIndex, 
							  const CEffectCode& EffectCode);
		
		void __AppendRegister(STRING& Destiantion, 
							  CEffectCode::VARIABLETYPE Type, 
							  UINT uRegister, 
							  const UINT* puAddress, 
							  UINT uAddressCount, 
							  const CHAR* pcRegister, 
							  const STRING& IndexDestiantion,
							  UINT uCount, 
							  UINT uIndex, 
							  const CEffectCode& EffectCode);
		
	private:
		CMap<CEffectCode::VARIABLETYPE, const CHAR*> m_VariableTypeMap;
		IEffect::SAMPLERTYPE m_SamplerType;
		ICompiler::INSTRUCTION m_Opcode;
    };
}