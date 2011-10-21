#pragma once

#include "AlchemyEffectInfoInterface.h"
#include "AlchemyRenderMethod.h"

namespace alchemy
{
	class COutLineEffect :
		public CEffectInfoInterface
	{
		typedef enum
		{
			TOTAL_SAMPLERS
		}SAMPLERINDEX;

		typedef enum
		{
			ATTRIBUTE_POSITION = 0,
			ATTRIBUTE_NORMAL,
			
			TOTAL_ATTRIBUTES
		}ATTRIBUTEINDEX;

		typedef enum
		{
			VARYING_POSITIONT = 0,
			VARYING_OUTLINE_COLOR,

			TOTAL_VARYING
		}VARYINGINDEX;

		typedef enum
		{
			CONSTANT_WORLD_VIEW_PROJECTION_MATRIX = 0,
			CONSTANT_ZERO,
			CONSTANT_HALF,

			TOTAL_CONSTANTS
		}CONSTANSINDEX;

		typedef enum
		{
			TEMPORARY_POSITION,

			TOTAL_TEMPORARY
		}TEMPORARYINDEX;

		typedef enum
		{
			VARIABLE_POSITION = 0,

			TOTAL_VARIABLE
		}VARIABLEINDEX;

		typedef enum
		{

			TOTAL_CONSTVARIABLE
		}CONSTANTVARIABLEDECLARATIONINDEX;

	public:
		COutLineEffect();
		~COutLineEffect(void);

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
		static const CEffectCode::ELEMENTPARAMETER          	sm_ATTRIBUTES[TOTAL_ATTRIBUTES];
		static const CEffectCode::ELEMENTPARAMETER          	sm_VARYING   [TOTAL_VARYING   ];
		static const CEffectCode::GLOBALVARIABLE            	sm_CONSTANS  [TOTAL_CONSTANTS ];
		static const CEffectCode::ATTRIBUTEVARIABLEDECLARATION	sm_VARIABLE  [TOTAL_VARIABLE  ];
		static const CEffectCode::TEMPORARYDECLARATION			sm_TEMPORARY [TOTAL_TEMPORARY];
	};

	ALCHEMY_INLINE UINT                                         COutLineEffect::GetEffectInfoFlag()
	{
		return 0;
	}

	ALCHEMY_INLINE const CEffectInfoInterface::SAMPLER      * COutLineEffect::GetSamplers  (UINT& uSamplerCount  )
	{
		uSamplerCount = TOTAL_SAMPLERS;

		return ALCHEMY_NULL;
	}

	ALCHEMY_INLINE const CEffectCode::ELEMENTPARAMETER         * COutLineEffect::GetAttributes(UINT& uAttributeCount)
	{
		uAttributeCount = TOTAL_ATTRIBUTES;

		return sm_ATTRIBUTES;
	}

	ALCHEMY_INLINE const CEffectCode::ELEMENTPARAMETER         * COutLineEffect::GetVarying   (UINT& uVaryingCount  )
	{
		uVaryingCount = TOTAL_VARYING;

		return sm_VARYING;
	}

	ALCHEMY_INLINE const CEffectCode::GLOBALVARIABLE           * COutLineEffect::GetConstants (UINT& uConstantCount )
	{
		uConstantCount = TOTAL_CONSTANTS;

		return sm_CONSTANS;
	}

	ALCHEMY_INLINE const CEffectCode::TEMPORARYDECLARATION      * COutLineEffect::GetTemporary (UINT& uTemporaryCount)
	{
		uTemporaryCount = TOTAL_TEMPORARY;

		return sm_TEMPORARY;
	}

	ALCHEMY_INLINE const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* COutLineEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
	{
		uAttributeVariableCount = TOTAL_VARIABLE;

		return sm_VARIABLE;
	}

	ALCHEMY_INLINE const CEffectCode::CONSTANTVARIABLEDECLARATION * COutLineEffect::GetConstantVariables (UINT& uConstantVariableCount )
	{
		uConstantVariableCount = TOTAL_CONSTVARIABLE;

		return ALCHEMY_NULL;
	}
}