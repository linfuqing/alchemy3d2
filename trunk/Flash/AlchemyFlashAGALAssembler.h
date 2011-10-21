#pragma once

#include "../Graphics/AlchemyCompilerInterface.h"
#include "AlchemyFlashObject.h"
#include "../Graphics/AlchemyEffectCode.h"

namespace alchemy
{
	class CFlashAGALAssembler :
		public CCompilerInterface, public CFlashObject
	{
	public:
		CFlashAGALAssembler(void);
		~CFlashAGALAssembler(void);

		bool ProgramString(PVOID* ppOutput, PUINT puLengthOutput, CEffectInterface::EFFECTTYPE Type, const CHAR* pcSource, CBuffer** ppCompilationErrors);

		CEffectInterface::EFFECTTYPE BuildShaderFunctionCode(STRING& Desination, UINT uShaderFunction, const CEffectCode& Code);

		bool AppendInstruction(
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
			CEffectCode& EffectCode);

		UINT GetByteAlignmentShift()const;

#ifdef AS3_H
		AS3TYPE Init();
		void Fill(AS3TYPE Object, AS3TYPE MethodArray);
#endif

	private:
		static const CHAR sm_cAGALInstruction[CCompilerInterface::TOTAL_INSTRUCTION][4];
		static const CHAR sm_cVERTEX_SHADER_REGISTER[CCompilerInterface::TOTAL_REGISTERTYPE][3];
		static const CHAR sm_cFRAGMENT_SHADER_REGISTER[CCompilerInterface::TOTAL_REGISTERTYPE][3];
		static const CHAR sm_cREGISTER_ADDRESS[5];

		static const CHAR sm_cSAMPLER_TYPE[CEffectCode::TOTAL_SAMPLER_TYPE][5];
		static const CHAR sm_cSAMPLER_FILTER[CEffectCode::TOTAL_SAMPLER_FILTER][8];
		static const CHAR sm_cSAMPLER_ADDRESS[CEffectCode::TOTAL_SAMPLER_ADDRESS_MODE][7];

		void __AppendSampler(STRING& Destiantion, const CEffectCode::SAMPLERDECLARATION& Sampler);
		void __AppendRegister(STRING& Destiantion, UINT uRegister, const UINT* puAddress, UINT uAddressCount, bool bIsAppendIndex);
		void __AppendRegister(STRING& Destiantion, UINT uRegister, UINT uIndexRegister, UINT uIndexAddress, const UINT* puAddress, UINT uAddressCount, bool bIsAppendIndex);
#ifdef AS3_H
		AS3TYPE m_CompilerMethod;
#endif
	};

	ALCHEMY_INLINE UINT CFlashAGALAssembler::GetByteAlignmentShift()const
	{
		return 4;
	}
}