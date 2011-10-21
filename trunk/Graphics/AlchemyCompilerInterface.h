#pragma once

#include "AlchemyEffectInterface.h"
#include "AlchemyVertexDeclaration.h"

#include "../Core/AlchemyBuffer.h"
#include "../Core/AlchemyString.h"

namespace alchemy
{
	class CEffectCode;
	class CCompilerInterface
	{
	public:
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
			IFZ,
			INZ,
			IFE,
			INE,
			IFG,
			IFL,
			IEG,
			IEL,
			ELS,
			EIF,
			REP,
			ERP,
			BRK,
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

		CCompilerInterface(void) {}
		virtual ~CCompilerInterface(void) {}

		virtual bool ProgramString(PVOID* ppOutput, PUINT puOutputLength, CEffectInterface::EFFECTTYPE Type, const CHAR* pcSource, CBuffer** ppCompilationErrors) = 0;

		virtual CEffectInterface::EFFECTTYPE BuildShaderFunctionCode(STRING& Desination, UINT uShaderFunction, const CEffectCode& Code) = 0;

		virtual bool AppendInstruction(
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
			CEffectCode& EffectCode) = 0;

		virtual UINT GetByteAlignmentShift()const = 0;
	};
}
