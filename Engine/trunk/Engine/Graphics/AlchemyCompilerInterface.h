#pragma once

#include "AlchemyEffectInterface.h"
#include "AlchemyVertexDeclaration.h"

#include "../Core/AlchemyBuffer.h"
#include "../Core/AlchemyString.h"

namespace alchemy
{
	class CEffectCode;
	ALCHEMY_INTERFACE(ICompiler)
	{
		typedef enum
		{
			MOV,
			ADD,
			SUB,
			MUL,
			DIV,
			RCP,
			MIN,
			MAX,
			FRC,
			SQT,
			RSQ,
			POW,
			LOG,
			EXP,
			NRM,
			SIN,
			COS,
			CRS,
			DP3,
			DP4,
			ABS,
			NEG,
			SAT,
			M33,
			M44,
			M34,
			KIL,
			TEX,
			SGE,
			SLT,
			SGN,

			TOTAL_INSTRUCTION
		}INSTRUCTION;

		typedef enum
		{
			NONE = 0,
			GLOBAL,
			TEMPORARY,
			PARAMETER,
			RESULT,
			OUTPUT,
			SAMPLER,

			TOTAL_REGISTERTYPE
		}REGISTERTYPE;

		typedef enum
		{
			TEXTURE1D,
			TEXTURE2D,
			TEXTURE3D,
			TEXTURE_CUBE,

			TOTAL_SAMPLERTYPE
		}SAMPLERTYPE;

		typedef enum
		{
			NO_FILTER,
			POINT,
			LINEAR,
			ANISOTROPIC,
			PYRAMIDALQUAD,
			GAUSSIANQUAD,
			CONVOLUTIONMONO,

			TOTAL_SAMPLEFILTERTYPE
		}SAMPLERFILTER;

		typedef enum
		{
			WRAP,
			MIRROR,
			CLAMP,
			BORDER,
			MIRROR_ONCE,

			TOTAL_SAMPLERADDRESSTYPE
		}SAMPLERADDRESS;

		typedef struct  
		{
			UINT uByteAlignmentShift;
			UINT uOutputColorCount;
			UINT uMaxVertexConstantBufferSize;
			UINT uMaxFragmentConstantBufferSize;
			bool bIsSupportConstantBuffer;
		}COMPILERINFO;

		virtual const COMPILERINFO& GetCompilerInfo()const = 0;

		virtual UINT GetAddressShift(UINT uVariable, const CEffectCode& EffectCode)const = 0;

		virtual bool ProgramString(CBuffer** ppOutput, IEffect::EFFECTTYPE Type, const CHAR* pcSource) = 0;

		virtual bool BuildShaderFunctionCode(STRING& Desination, IEffect::EFFECTTYPE Type, UINT uInstructionIndices, UINT uUserVariables, const CEffectCode& Code) = 0;
			
		virtual bool AppendRegisterName(STRING& Destination, UINT uRegister, REGISTERTYPE RegisterType, IEffect::EFFECTTYPE ShaderType)const = 0;
		
		virtual UINT ComputeRegisterLocation(UINT uRegister, UINT uOffset, UINT uMinRegister, UINT uSamplerCount, UINT uConstantSamplerCount)const = 0;
		
		virtual bool IsInverceInstruction(INSTRUCTION Opcode)const = 0;
	};
}
