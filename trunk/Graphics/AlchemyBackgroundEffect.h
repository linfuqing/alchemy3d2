#pragma once

#include "AlchemyEffectInfoInterface.h"
#include "AlchemyRenderMethod.h"

namespace alchemy
{
	class CBackgroundEffect :
		public CEffectInfoInterface
	{
		typedef enum
		{
			TOTAL_SAMPLERS
		}SAMPLERINDEX;

		typedef enum
		{
			//ATTRIBUTE_POSITION = 0,

			TOTAL_ATTRIBUTES
		}ATTRIBUTEINDEX;

		typedef enum
		{
			VARYING_POSITIONT = 0,

			TOTAL_VARYING
		}VARYINGINDEX;

		typedef enum
		{
			CONSTANT_WORLD_VIEW_PROJECTION_MATRIX = 0,
			TOTAL_CONSTANTS
		}CONSTANSINDEX;

		typedef enum
		{
			TEMPORARY_POS = 0,

			TOTAL_TEMPORARY
		}TEMPORARYINDEX;

		typedef enum
		{
			ATTRIBUTE_VARIABLE_POSITION = 0,

			TOTAL_ATTRIBUTE_VARIABLES
		}ATTRIBUTEVARIABLEINDEX;

		typedef enum
		{
			TOTAL_CONSTANT_VARIABLES = 0
		}CONSTANTVARIABLEINDEX;
	public:
		CBackgroundEffect();
		~CBackgroundEffect(void);

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
		//static const CEffectCode::ELEMENTPARAMETER             sm_ATTRIBUTES       [TOTAL_ATTRIBUTES        ];
		static const CEffectCode::ELEMENTPARAMETER             sm_VARYING          [TOTAL_VARYING           ];
		static const CEffectCode::GLOBALVARIABLE               sm_CONSTANS         [TOTAL_CONSTANTS         ];
		static const CEffectCode::ATTRIBUTEVARIABLEDECLARATION sm_ATTRIBUTE_VARIABLE[TOTAL_ATTRIBUTE_VARIABLES];
		static const CEffectCode::TEMPORARYDECLARATION		   sm_TEMPRORAYS	   [TOTAL_TEMPORARY];
	};

	ALCHEMY_INLINE UINT                                            CBackgroundEffect::GetEffectInfoFlag     (                             )
	{
		return ALCHEMY_FLAG(CEffectInfoInterface::VERTEX_SHADER_OUTPUT);
	}

	ALCHEMY_INLINE const CEffectInfoInterface::SAMPLER           * CBackgroundEffect::GetSamplers           (UINT& uSamplerCount          )
	{
		uSamplerCount = TOTAL_SAMPLERS;

		return ALCHEMY_NULL;
	}

	ALCHEMY_INLINE const CEffectCode::ELEMENTPARAMETER            * CBackgroundEffect::GetAttributes        (UINT& uAttributeCount        )
	{
		uAttributeCount = TOTAL_ATTRIBUTES;

		return ALCHEMY_NULL;
	}

	ALCHEMY_INLINE const CEffectCode::ELEMENTPARAMETER            * CBackgroundEffect::GetVarying           (UINT& uVaryingCount          )
	{
		uVaryingCount = TOTAL_VARYING;

		return sm_VARYING;
	}

	ALCHEMY_INLINE const CEffectCode::GLOBALVARIABLE              * CBackgroundEffect::GetConstants         (UINT& uConstantCount         )
	{
		uConstantCount = TOTAL_CONSTANTS;

		return sm_CONSTANS;
	}

	ALCHEMY_INLINE const CEffectCode::TEMPORARYDECLARATION        * CBackgroundEffect::GetTemporary         (UINT& uTemporaryCount        )
	{
		uTemporaryCount = TOTAL_TEMPORARY;

		return sm_TEMPRORAYS;
	}

	ALCHEMY_INLINE const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CBackgroundEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
	{
		uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLES;

		return sm_ATTRIBUTE_VARIABLE;
	}

	ALCHEMY_INLINE const CEffectCode::CONSTANTVARIABLEDECLARATION * CBackgroundEffect::GetConstantVariables  (UINT& uConstantVariableCount)
	{
		uConstantVariableCount = TOTAL_CONSTANT_VARIABLES;

		return ALCHEMY_NULL;
	}
}