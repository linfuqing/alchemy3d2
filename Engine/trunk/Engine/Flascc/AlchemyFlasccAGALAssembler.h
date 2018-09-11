#pragma once

#include "../Graphics/AlchemyCompilerInterface.h"
#include "../Graphics/AlchemyEffectCode.h"

namespace alchemy
{
	class CFlasccAGALAssembler :
		public CSmartClass<ICompiler>
	{
	public:
		CFlasccAGALAssembler(void);
		~CFlasccAGALAssembler(void);

		const ICompiler::COMPILERINFO& GetCompilerInfo()const;

		UINT GetAddressShift(UINT uVariable, const CEffectCode& EffectCode)const;

		bool ProgramString(CBuffer** ppOutput, IEffect::EFFECTTYPE Type, const CHAR* pcSource);

		bool BuildShaderFunctionCode(STRING& Destination, IEffect::EFFECTTYPE Type, UINT uInstructionIndices, UINT uUserVariables, const CEffectCode& Code);

		bool AppendRegisterName(STRING& Destination, UINT uRegister, ICompiler::REGISTERTYPE RegisterType, IEffect::EFFECTTYPE ShaderType)const;

		UINT ComputeRegisterLocation(UINT uRegister, UINT uOffset, UINT uMinRegister, UINT uSamplerCount, UINT uConstantSamplerCount)const;

		bool IsInverceInstruction(ICompiler::INSTRUCTION Opcode)const;

	private:
		static const CHAR* sm_pcAGAL_INSTRUCTION[ICompiler::TOTAL_INSTRUCTION];
		static const CHAR* sm_pcVERTEX_SHADER_REGISTER[ICompiler::TOTAL_REGISTERTYPE];
		static const CHAR* sm_pcFRAGMENT_SHADER_REGISTER[ICompiler::TOTAL_REGISTERTYPE];
		static const CHAR* sm_pcREGISTER_ADDRESS;

		static const CHAR* sm_pcSAMPLER_TYPE[IEffect::TOTAL_SAMPLER_TYPE];
		static const CHAR* sm_pcSAMPLER_FILTER[IEffect::TOTAL_SAMPLER_FILTER];
		static const CHAR* sm_pcSAMPLER_MIPMAP_FILTER[IEffect::TOTAL_SAMPLER_FILTER];
		static const CHAR* sm_pcSAMPLER_ADDRESS[IEffect::TOTAL_SAMPLER_ADDRESS_MODE];

		bool __AppendInstruction(
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
			const CEffectCode& EffectCode);

		void __AppendVariable(STRING& Destination, UINT uVariable, UINT uVariableIndex, UINT uIndexAddress, const UINT* puAddress, UINT uAddressCount, UINT uMinDestinationAddress, const CEffectCode& EffectCode);
		void __AppendSampler(STRING& Destiantion, const IEffect::SAMPLERDECLARATION& Sampler);
		void __AppendRegister(STRING& Destiantion, UINT uRegister, const UINT* puAddress, UINT uAddressCount, bool bIsAppendIndex);
		void __AppendRegister(STRING& Destiantion, UINT uRegister, UINT uIndexRegister, UINT uIndexAddress, const UINT* puAddress, UINT uAddressCount, bool bIsAppendIndex);
	};

	ALCHEMY_INLINE const ICompiler::COMPILERINFO& CFlasccAGALAssembler::GetCompilerInfo()const
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

	ALCHEMY_INLINE UINT CFlasccAGALAssembler::GetAddressShift(UINT uVariable, const CEffectCode& EffectCode)const
	{
		const CEffectCode::VARIABLE* pVarialbe = EffectCode.GetVariable(uVariable);
		if(pVarialbe)
			return (pVarialbe->uRegister & 15) >> 2;

		return 0;
	}

	ALCHEMY_INLINE UINT CFlasccAGALAssembler::ComputeRegisterLocation(UINT uRegister, UINT uOffset, UINT uMinRegister, UINT uSamplerCount, UINT uConstantSamplerCount)const
	{
		return uRegister >> 2;
	}

	ALCHEMY_INLINE bool CFlasccAGALAssembler::IsInverceInstruction(ICompiler::INSTRUCTION Opcode)const
	{
		return false;
	}
}