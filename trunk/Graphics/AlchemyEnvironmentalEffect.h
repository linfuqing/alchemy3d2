#pragma once

#include "AlchemyEffectInfoInterface.h"
#include "AlchemyRenderMethod.h"

namespace alchemy
{
	class CMatrix4x4;
	class CEnvironmentalEffect :
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
			VARYING_CUBE_UVW = 0,

			TOTAL_VARYING
		}VARYINGINDEX;

		typedef enum
		{
			CONSTANT_MUL_COE,
			CONSTANT_WORLD_VIEW_MARTIX,
			CONSTANT_INVERT_VIEW_MATRIX,
			TOTAL_CONSTANTS
		}CONSTANSINDEX;

		typedef enum
		{
			TEMPORARY_POS,
			TEMPORARY_POS_DIR,
			TEMPORARY_CAMERA_POS_DIR,
			TEMPORARY_NORMAL_DOT,

			TOTAL_TEMPORARY
		}TEMPORARYINDEX;

		typedef enum
		{
			ATTRIBUTE_VARIABLE_POSITION,
			ATTRIBUTE_VARIABLE_NORMAL,
			ATTRIBUTE_VARIABLE_UVW,

			TOTAL_ATTRIBUTE_VARIABLES
		}ATTRIBUTEVARIABLEINDEX;

		typedef enum
		{
			TOTAL_CONSTANT_VARIABLES = 0
		}CONSTANTVARIABLEINDEX;
	public:
		CEnvironmentalEffect(const CMatrix4x4* pInvertViewMatrix);
		~CEnvironmentalEffect(void);

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
		static const FLOAT s_MUL_COE;
		static const CEffectCode::ELEMENTPARAMETER             sm_VARYING          [TOTAL_VARYING           ];
		//static const CEffectCode::GLOBALVARIABLE               sm_CONSTANS         [TOTAL_CONSTANTS         ];
		static const CEffectCode::ATTRIBUTEVARIABLEDECLARATION sm_ATTRIBUTE_VARIABLE[TOTAL_ATTRIBUTE_VARIABLES];
		static const CEffectCode::TEMPORARYDECLARATION		   sm_TEMPRORAYS	   [TOTAL_TEMPORARY];

		CEffectCode::GLOBALVARIABLE								m_Contans[ TOTAL_CONSTANTS ];
	};

	ALCHEMY_INLINE UINT                                            CEnvironmentalEffect::GetEffectInfoFlag     (                             )
	{
		return ALCHEMY_FLAG(CEffectInfoInterface::VERTEX_SHADER_INFO);
	}

	ALCHEMY_INLINE const CEffectInfoInterface::SAMPLER           * CEnvironmentalEffect::GetSamplers           (UINT& uSamplerCount          )
	{
		uSamplerCount = TOTAL_SAMPLERS;

		return ALCHEMY_NULL;
	}

	ALCHEMY_INLINE const CEffectCode::ELEMENTPARAMETER            * CEnvironmentalEffect::GetAttributes        (UINT& uAttributeCount        )
	{
		uAttributeCount = TOTAL_ATTRIBUTES;

		return ALCHEMY_NULL;
	}

	ALCHEMY_INLINE const CEffectCode::ELEMENTPARAMETER            * CEnvironmentalEffect::GetVarying           (UINT& uVaryingCount          )
	{
		uVaryingCount = TOTAL_VARYING;

		return sm_VARYING;
	}

	ALCHEMY_INLINE const CEffectCode::GLOBALVARIABLE              * CEnvironmentalEffect::GetConstants         (UINT& uConstantCount         )
	{
		uConstantCount = TOTAL_CONSTANTS;

		return m_Contans;
	}

	ALCHEMY_INLINE const CEffectCode::TEMPORARYDECLARATION        * CEnvironmentalEffect::GetTemporary         (UINT& uTemporaryCount        )
	{
		uTemporaryCount = TOTAL_TEMPORARY;

		return sm_TEMPRORAYS;
	}

	ALCHEMY_INLINE const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CEnvironmentalEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
	{
		uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLES;

		return sm_ATTRIBUTE_VARIABLE;
	}

	ALCHEMY_INLINE const CEffectCode::CONSTANTVARIABLEDECLARATION * CEnvironmentalEffect::GetConstantVariables  (UINT& uConstantVariableCount)
	{
		uConstantVariableCount = TOTAL_CONSTANT_VARIABLES;

		return ALCHEMY_NULL;
	}
}