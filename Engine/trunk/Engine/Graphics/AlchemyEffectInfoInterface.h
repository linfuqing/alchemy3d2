#pragma once

#include "AlchemyEffectCode.h"

namespace alchemy
{
	ALCHEMY_INTERFACE(IEffectInfo)
	{
		typedef enum
		{
			VERTEX_SHADER_INFO,
			VERTEX_SHADER_OUTPUT,
			FRAGMENT_SHADER_INFO,
			FRAGMENT_SHADER_OUTPUT
		}EFFECTINFOTYPE;

		typedef struct
		{
			IEffect::EFFECTTYPE Type;
			IEffect::SAMPLERDECLARATION Declaration;
			UINT uIndex;
		}SAMPLER, * LPSAMPLER;

		virtual UINT                                             GetEffectInfoFlag    (                             ) = 0;

		virtual const SAMPLER                                  * GetSamplers          (UINT& uSamplerCount          ) = 0;

		virtual const CEffectCode::ELEMENTDECLARATION            * GetAttributes        (UINT& uAttributeCount        ) = 0;
		virtual const CEffectCode::ELEMENTDECLARATION            * GetVarying           (UINT& uVaryingCount          ) = 0;

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
			CEffectCode::IInstructionSet& VertexShaderDefiner,
			CEffectCode::IInstructionSet& FragmentShaderDefiner,
			CEffectCode::CPassDefiner::CEffectCodeUtils& EffectCodeUtils) = 0;
	};

	typedef CSmartClass<IEffectInfo> EFFECTINFO, * LPEFFECTINFO;
}