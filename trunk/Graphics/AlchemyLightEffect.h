#pragma once

#include "AlchemyEffectInfoInterface.h"

namespace alchemy
{
	class CLightEffect :
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
			TOTAL_CONSTANTS = 0
		}CONSTANSINDEX;

		typedef enum
		{
			TEMPORARY_COLOR = 0,
			TEMPORARY_DESTINATION,

			TOTAL_TEMPORARY
		}TEMPORARYINDEX;

		typedef enum
		{
			ATTRIBUTE_VARIABLE_AMBIENT = 0,
			ATTRIBUTE_VARIABLE_DIFFUSE,
			ATTRIBUTE_VARIABLE_SPECULAR,

			TOTAL_ATTRIBUTE_VARIABLES
		}ATTRIBUTEVARIABLEINDEX;

		typedef enum
		{
			CONSTANT_VARIABLE_MATERIAL_AMBIENT = 0,
			CONSTANT_VARIABLE_MATERIAL_DIFFUSE,
			CONSTANT_VARIABLE_MATERIAL_SPECULAR,

			CONSTANT_VARIABLE_MATERIAL_EMISSIVE,

			TOTAL_CONSTANT_VARIABLES
		}CONSTANTVARIABLEINDEX;
	public:
		CLightEffect(void);
		~CLightEffect(void);

		UINT                                          GetEffectInfoFlag();

		const SAMPLER                               * GetSamplers  (UINT& uSamplerCount  );

		const CEffectCode::ELEMENTPARAMETER         * GetAttributes(UINT& uAttributeCount);
		const CEffectCode::ELEMENTPARAMETER         * GetVarying   (UINT& uVaryingCount  );

		const CEffectCode::GLOBALVARIABLE           * GetConstants (UINT& uConstantCount );

		const CEffectCode::TEMPORARYDECLARATION     * GetTemporary (UINT& uTemporaryCount);

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
		static const CEffectCode::TEMPORARYDECLARATION         sm_TEMPORARY          [TOTAL_TEMPORARY          ];
		static const CEffectCode::ATTRIBUTEVARIABLEDECLARATION sm_ATTRIBUTE_VARIABLES[TOTAL_ATTRIBUTE_VARIABLES];
		static const CEffectCode::CONSTANTVARIABLEDECLARATION  sm_CONSTANT_VARIABLES [TOTAL_CONSTANT_VARIABLES ];
	};
}