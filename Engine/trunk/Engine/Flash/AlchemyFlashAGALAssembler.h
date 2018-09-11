#pragma once

#include "AlchemyFlashObject.h"

#include "../Graphics/AlchemyCompilerInterface.h"
#include "../Graphics/AlchemyEffectCode.h"

namespace alchemy
{
	class CFlashAGALAssembler :
		public CSmartClass<ICompiler>, public CFlashObject
	{
	public:
		CFlashAGALAssembler(void);
		~CFlashAGALAssembler(void);

		const ICompiler::COMPILERINFO& GetCompilerInfo()const;

		UINT GetAddressShift(UINT uVariable, const CEffectCode& EffectCode)const;

		bool ProgramString(CBuffer** ppOutput, IEffect::EFFECTTYPE Type, const CHAR* pcSource);

		bool BuildShaderFunctionCode(STRING& Desination, IEffect::EFFECTTYPE Type, UINT uUserVariables, UINT uCode, const CEffectCode& Code);

		bool AppendInstruction(
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
			CEffectCode& EffectCode);

		bool ReplaceVariable(
			STRING& Code, 
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

#ifdef AS3_H
		AS3TYPE Init();
		void Fill(AS3TYPE Object, AS3TYPE MethodArray);
#endif

	private:
		static const CHAR* sm_pcAGALInstruction[ICompiler::TOTAL_INSTRUCTION];
		static const CHAR* sm_pcVERTEX_SHADER_REGISTER[ICompiler::TOTAL_REGISTERTYPE];
		static const CHAR* sm_pcFRAGMENT_SHADER_REGISTER[ICompiler::TOTAL_REGISTERTYPE];
		static const CHAR* sm_pcREGISTER_ADDRESS;

		static const CHAR* sm_pcSAMPLER_TYPE[IEffect::TOTAL_SAMPLER_TYPE];
		static const CHAR* sm_pcSAMPLER_FILTER[IEffect::TOTAL_SAMPLER_FILTER];
		static const CHAR* sm_pcSAMPLER_MIPMAP_FILTER[IEffect::TOTAL_SAMPLER_FILTER];
		static const CHAR* sm_pcSAMPLER_ADDRESS[IEffect::TOTAL_SAMPLER_ADDRESS_MODE];

		void __AppendVariable(STRING& Destination, UINT uVariable, UINT uVariableIndex, UINT uIndexAddress, const UINT* puAddress, UINT uAddressCount, UINT uMinDestinationAddress, const CEffectCode& EffectCode);
		void __AppendSampler(STRING& Destiantion, const IEffect::SAMPLERDECLARATION& Sampler);
		void __AppendRegister(STRING& Destiantion, UINT uRegister, const UINT* puAddress, UINT uAddressCount, bool bIsAppendIndex);
		void __AppendRegister(STRING& Destiantion, UINT uRegister, UINT uIndexRegister, UINT uIndexAddress, const UINT* puAddress, UINT uAddressCount, bool bIsAppendIndex);
#ifdef AS3_H
		AS3TYPE m_CompilerMethod;
#endif
	};

	ALCHEMY_INLINE const ICompiler::COMPILERINFO& CFlashAGALAssembler::GetCompilerInfo()const
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

	ALCHEMY_INLINE UINT CFlashAGALAssembler::GetAddressShift(UINT uVariable, const CEffectCode& EffectCode)const
	{
		const CEffectCode::VARIABLE* pVarialbe = EffectCode.GetVariable(uVariable);
		if(pVarialbe)
			return (pVarialbe->uRegister & 15) >> 2;

		return 0;
	}
	
	ALCHEMY_INLINE UINT CFlashAGALAssembler::ComputeRegisterLocation(UINT uRegister, UINT uOffset, UINT uMinRegister, UINT uSamplerCount, UINT uConstantSamplerCount)const
	{
		return uRegister >> 2;
	}
	
	ALCHEMY_INLINE bool CFlashAGALAssembler::IsInverceInstruction(ICompiler::INSTRUCTION Opcode)const
	{
		return false;
	}
}