#pragma once

#include "AlchemyEffectInfoInterface.h"
#include "AlchemyRenderMethod.h"

namespace alchemy
{
	class CMeasureColorEffect :
		public CEffectInfoInterface
	{
		typedef enum
		{
			TOTAL_SAMPLERS
		}SAMPLERINDEX;

		typedef enum
		{
			TOTAL_ATTRIBUTES
		}ATTRIBUTEINDEX;

		typedef enum
		{
			TOTAL_VARYING
		}VARYINGINDEX;

		typedef enum
		{
			CONSTANT_MEASURE_VALUE,

			TOTAL_CONSTANTS
		}CONSTANSINDEX;

		typedef enum
		{
			TOTAL_TEMPORARY = 0
		}TEMPORARYINDEX;

		typedef enum
		{
			TOTAL_ATTRIBUTE_VARIABLES
		}ATTRIBUTEVARIABLEINDEX;

		typedef enum
		{
			TOTAL_CONSTANT_VARIABLES = 0
		}CONSTANTVARIABLEINDEX;
	public:
		CMeasureColorEffect(const FLOAT4* pMeaure);
		~CMeasureColorEffect(void);

		UINT                                             GetEffectInfoFlag    (                             );

		const SAMPLER                                  * GetSamplers          (UINT& uSamplerCount          );

		const CEffectCode::ELEMENTPARAMETER            * GetAttributes        (UINT& uAttributeCount        );
		const CEffectCode::ELEMENTPARAMETER            * GetVarying           (UINT& uVaryingCount          );

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
			const CEffectCode::SHADERVARIABLE* pAttributeVariables,
			const CEffectCode::SHADERVARIABLE* pConstantVariables,
			CEffectCode::CInstructionSetInterface& VertexShaderDefiner,
			CEffectCode::CInstructionSetInterface& FragmentShaderDefiner,
			CEffectCode::CPassDefiner::CEffectCodeUtils& EffectCodeUtils);

	private:
		CEffectCode::GLOBALVARIABLE								m_Contans[ TOTAL_CONSTANTS ];
	};
}