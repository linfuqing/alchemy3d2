#pragma once

#include "../Graphics/AlchemyCompilerInterface.h"
#include "../Graphics/AlchemyEffectCode.h"

namespace alchemy
{
	class CHLSLCompiler : 
		public CSmartClass<ICompiler>
	{
	public:
		CHLSLCompiler(UINT uOutputColorCount, UINT uConstantBufferSize);
		~CHLSLCompiler(void);

		const COMPILERINFO& GetCompilerInfo()const;

		UINT GetAddressShift(UINT uVariable, const CEffectCode& EffectCode)const;

		bool ProgramString(CBuffer** ppOutput, IEffect::EFFECTTYPE Type, const CHAR* pcSource);

		bool BuildShaderFunctionCode(STRING& Destination, IEffect::EFFECTTYPE Type, UINT uInstructionIndices, UINT uUserVariables, const CEffectCode& Code);

		bool AppendRegisterName(STRING& Destination, UINT uRegister, REGISTERTYPE RegisterType, IEffect::EFFECTTYPE ShaderType)const;

		UINT ComputeRegisterLocation(UINT uRegister, UINT uOffset, UINT uMinRegister, UINT uSamplerCount, UINT uConstantSamplerCount)const;

		bool IsInverceInstruction(INSTRUCTION Opcode)const;

	private:
		static const CHAR* sm_pcINSTRUCTION[ICompiler::TOTAL_INSTRUCTION];
		static const CHAR* sm_pcVERTEX_SHADER_REGISTER[ICompiler::TOTAL_REGISTERTYPE];
		static const CHAR* sm_pcFRAGMENT_SHADER_REGISTER[ICompiler::TOTAL_REGISTERTYPE];
		static const CHAR* sm_pcREGISTER_ADDRESS;

		static const CHAR* sm_pcSAMPLER_TYPE[IEffect::TOTAL_SAMPLER_TYPE];

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

		void __AppendVariable(STRING& Destination, STRING& Head, UINT uVariable, UINT uVariableIndex, UINT uIndexAddress, const UINT* puAddress, UINT uAddressCount, UINT uMinDestinationAddress, const CEffectCode& EffectCode);
		void __AppendRegister(STRING& Destiantion, UINT uRegister, const UINT* puAddress, UINT uAddressCount, bool bIsAppendIndex);
		void __AppendRegister(STRING& Destiantion, UINT uRegister, UINT uVariableIndex, const UINT* puAddress, UINT uAddressCount, bool bIsAppendIndex);

		COMPILERINFO m_CompilerInfo;

		UINT m_uIntegerRegisterIndex;

		PUINT m_puOutputRegisters;

		UINT m_uTemporaryRegisterCount;
		CMap<UINT, UINT> m_TemporaryRegisterMap;
		CVector<UINT> m_FreeTemporaryRegister;
		UINT m_uFreeTemporaryRegisterCount;
	};

	ALCHEMY_INLINE	const ICompiler::COMPILERINFO& CHLSLCompiler::GetCompilerInfo()const
	{
		return m_CompilerInfo;
	}

	ALCHEMY_INLINE UINT CHLSLCompiler::GetAddressShift(UINT uVariable, const CEffectCode& EffectCode)const
	{
		const CEffectCode::VARIABLE* pVarialbe = EffectCode.GetVariable(uVariable);
		if(pVarialbe)
			return (pVarialbe->uRegister & 15) >> 2;

		return 0;
	}

	ALCHEMY_INLINE UINT CHLSLCompiler::ComputeRegisterLocation(UINT uRegister, UINT uOffset, UINT uMinRegister, UINT uSamplerCount, UINT uConstantSamplerCount)const
	{
		return uRegister >> 4;
	}

	ALCHEMY_INLINE bool CHLSLCompiler::IsInverceInstruction(ICompiler::INSTRUCTION Opcode)const
	{
		return false;
	}
}