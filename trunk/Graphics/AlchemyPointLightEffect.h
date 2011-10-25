#pragma once

#include "AlchemyEffectInfoInterface.h"

namespace alchemy
{
	class CPointLightEffect :
		public CEffectInfoInterface
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
			TOTAL_VARYING = 0
		}VARYINGINDEX;

		typedef enum
		{
			CONSTANT_ONE = 0,

			TOTAL_CONSTANTS
		}CONSTANSINDEX;

		typedef enum
		{
			TEMPORARY_SPECULAR = 0,

			TOTAL_TEMPORARY
		}TEMPORARYINDEX;

		typedef enum
		{
			ATTRIBUTE_VARIABLE_NORMAL = 0,
			ATTRIBUTE_VARIABLE_SPECULAR,

			TOTAL_ATTRIBUTE_VARIABLES
		}ATTRIBUTEVARIABLEINDEX;

		typedef enum
		{
			CONSTANT_VARIABLE_CAMERA_LOCAL_DIRECTION_NEGATE = 0,

			CONSTANT_VARIABLE_LIGHT_SPECULAR,

			TOTAL_CONSTANT_VARIABLES
		}CONSTANTVARIABLEINDEX;
	public:
		CPointLightEffect(void);
		~CPointLightEffect(void);

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
		static const CEffectCode::GLOBALVARIABLE               sm_CONSTANTS         [TOTAL_CONSTANTS         ];
		static const CEffectCode::TEMPORARYDECLARATION         sm_TEMPORARY         [TOTAL_TEMPORARY         ];
		static const CEffectCode::ATTRIBUTEVARIABLEDECLARATION sm_ATTRIBUTE_VARIABLE[TOTAL_ATTRIBUTE_VARIABLES];

		CEffectCode::CONSTANTVARIABLEDECLARATION               m_ConstantVariable   [TOTAL_CONSTANT_VARIABLES ];
	};
}