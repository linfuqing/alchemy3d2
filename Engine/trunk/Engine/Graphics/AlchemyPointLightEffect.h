#pragma once

#include "AlchemyEffectInfoInterface.h"

namespace alchemy
{
	class CPointLightEffect :
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
			TOTAL_VARYING = 0
		}VARYINGINDEX;

		typedef enum
		{
			CONSTANT_LIGHT_ATTENUATION0 = 0,
			CONSTANT_LIGHT_ATTENUATION1,
			CONSTANT_LIGHT_ATTENUATION2,

			CONSTANT_LIGHT_LOCAL_POSITION,

			TOTAL_CONSTANTS
		}CONSTANSINDEX;

		typedef enum
		{
			TEMPORARY_LIGHT_DISTANCE = 0,
			TEMPORARY_LIGHT_ATTENUATION,

			TOTAL_TEMPORARY
		}TEMPORARYINDEX;

		typedef enum
		{
			ATTRIBUTE_VARIABLE_POSITION = 0,

			TOTAL_ATTRIBUTE_VARIABLES
		}ATTRIBUTEVARIABLEINDEX;

		typedef enum
		{
			CONSTANT_VARIABLE_LIGHT_DIFFUSE = 0,

			CONSTANT_VARIABLE_LIGHT_SPECULAR,

			CONSTANT_VARIABLE_LIGHT_LOCAL_DIRECTION,

			CONSTANT_VARIABLE_LIGHT_LOCAL_DIRECTION_NEGATE,

			TOTAL_CONSTANT_VARIABLES
		}CONSTANTVARIABLEINDEX;
	public:
		CPointLightEffect(UINT uLightIndex);
		~CPointLightEffect(void);

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
			const CEffectCode::SHADERVARIABLE* pAttributeVariables,
			const CEffectCode::SHADERVARIABLE* pConstantVariables,
			CEffectCode::IInstructionSet& VertexShaderDefiner,
			CEffectCode::IInstructionSet& FragmentShaderDefiner,
			CEffectCode::CPassDefiner::CEffectCodeUtils& EffectCodeUtils);

	private:
		static const CEffectCode::TEMPORARYDECLARATION         sm_TEMPORARY          [TOTAL_TEMPORARY          ];
		static const CEffectCode::ATTRIBUTEVARIABLEDECLARATION sm_ATTRIBUTE_VARIABLES[TOTAL_ATTRIBUTE_VARIABLES];

		CEffectCode::GLOBALVARIABLE                            m_Constants           [TOTAL_CONSTANTS          ];
		CEffectCode::CONSTANTVARIABLEDECLARATION               m_ConstantVariables   [TOTAL_CONSTANT_VARIABLES ];
	};
}