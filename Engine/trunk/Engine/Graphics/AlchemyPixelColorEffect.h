#pragma once

#include "AlchemyEffectInfoInterface.h"

namespace alchemy
{
	class CPixelColorEffect : 
		public EFFECTINFO
	{
		typedef enum
		{
			TOTAL_SAMPLERS = 0
		}SAMPLERINDEX;

		typedef enum
		{
			TOTAL_ATTRIBUTES = 0
		}ATTRIBUTEINDEX;

		typedef enum
		{
			TOTAL_VARYING
		}VARYINGINDEX;

		typedef enum
		{
			CONSTANT_COLOR,
			TOTAL_CONSTANTS
		}CONSTANSINDEX;

		typedef enum
		{
			TOTAL_TEMPORARY
		}TEMPORARYINDEX;

		typedef enum
		{
			TOTAL_ATTRIBUTE_VARIABLE
		}ATTRIBUTEVARIABLEINDEX;

		typedef enum
		{
			TOTAL_CONSTANT_VARIABLE
		}CONSTANTVARIABLEINDEX;

	public:
		CPixelColorEffect(const FLOAT4* pColor = ALCHEMY_NULL);
		virtual ~CPixelColorEffect();

		UINT                                             GetEffectInfoFlag    (                             );

		const SAMPLER                                  * GetSamplers          (UINT& uSamplerCount          );

		const CEffectCode::ELEMENTDECLARATION            * GetAttributes        (UINT& uAttributeCount        );
		const CEffectCode::ELEMENTDECLARATION            * GetVarying           (UINT& uVaryingCount          );

		const CEffectCode::GLOBALVARIABLE              * GetConstants         (UINT& uConstantCount         );

		const CEffectCode::TEMPORARYDECLARATION        * GetTemporary         (UINT& uTemporaryCount        );

		const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* GetAttributeVariables(UINT& uAttributeVariableCount);

		const CEffectCode::CONSTANTVARIABLEDECLARATION * GetConstantVariables (UINT& uConstantVariableCount );

		bool ShaderDefineHandler(
			UINT uOutputColor, 
			const UINT* puSamplers, 
			const UINT* puAttributes, 
			const UINT* puConstants, 
			const CEffectCode::VARYING* pVarying, 
			const UINT* puTemporary,
			const CEffectCode::SHADERVARIABLE* pAttributeVariable,
			const CEffectCode::SHADERVARIABLE* pConstantVariable,
			CEffectCode::IInstructionSet& VertexShaderDefiner,
			CEffectCode::IInstructionSet& FragmentShaderDefiner,
			CEffectCode::CPassDefiner::CEffectCodeUtils& EffectCodeUtils);

	private:
		CEffectCode::GLOBALVARIABLE m_Constant;
		
		CFloat4 m_Color;
	};
}