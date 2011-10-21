#pragma once

#include "AlchemyEffectCode.h"
#include "AlchemyRenderMethodInterface.h"

namespace alchemy
{
	class CEffectInfoInterface
	{
	public:
		typedef enum
		{
			VERTEX_SHADER_INFO,
			VERTEX_SHADER_OUTPUT,
			FRAGMENT_SHADER_INFO,
			FRAGMENT_SHADER_OUTPUT
		}EFFECTINFOTYPE;

		typedef struct
		{
			CEffectCode::SAMPLERDECLARATION Declaration;
			UINT uIndex;
		}SAMPLER, * LPSAMPLER;

		CEffectInfoInterface(void) {}
		virtual ~CEffectInfoInterface(void) {}

		virtual UINT                                             GetEffectInfoFlag    (                             ) = 0;

		virtual const SAMPLER                                  * GetSamplers          (UINT& uSamplerCount          ) = 0;

		virtual const CEffectCode::ELEMENTPARAMETER            * GetAttributes        (UINT& uAttributeCount        ) = 0;
		virtual const CEffectCode::ELEMENTPARAMETER            * GetVarying           (UINT& uVaryingCount          ) = 0;

		virtual const CEffectCode::GLOBALVARIABLE              * GetConstants         (UINT& uConstantCount         ) = 0;

		virtual const CEffectCode::TEMPORARYDECLARATION        * GetTemporary         (UINT& uTemporaryCount        ) = 0;

		virtual const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* GetAttributeVariables(UINT& uAttributeVariableCount) = 0;

		virtual const CEffectCode::CONSTANTVARIABLEDECLARATION * GetConstantVariables (UINT& uConstantVariableCount ) = 0;

		virtual bool ShaderDefineHandler(
			UINT uOutputColor, 
			const UINT* puSamplers, 
			const UINT* puAttributes, 
			const UINT* puConstants, 
			const CEffectCode::VARYING* pVarying, 
			const UINT* puTemporary,
			const CEffectCode::SHADERVARIABLE* pAttributeVariables,
			const CEffectCode::SHADERVARIABLE* pConstantVariables,
			CEffectCode::CInstructionSetInterface& VertexShaderDefiner,
			CEffectCode::CInstructionSetInterface& FragmentShaderDefiner,
			CEffectCode::CPassDefiner::CEffectCodeUtils& EffectCodeUtils) = 0;
	};
}