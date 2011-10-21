#pragma once

#include "AlchemyEffectInfoInterface.h"
#include "AlchemyRenderMethod.h"

namespace alchemy
{
	class CCartoonEffect :
		public CEffectInfoInterface
	{
		typedef enum
		{
			SAMPLER_GRAY,

			TOTAL_SAMPLERS
		}SAMPLERINDEX;

		typedef enum
		{
			ATTRIBUTE_POSITION = 0,
			ATTRIBUTE_NORMAL = 1,

			TOTAL_ATTRIBUTES
		}ATTRIBUTEINDEX;

		typedef enum
		{
			VARYING_POSITIONT = 0,
			VARYING_CARTOON_UV,

			//VARYING_OUTLINE_COLOR,

			TOTAL_VARYING
		}VARYINGINDEX;

		typedef enum
		{
			CONSTANT_WORLD_VIEW_PROJECTION_MATRIX = 0,
			CONSTANT_LIGHT_POSITION,
			CONSTANT_V_ZERO,

			//CONSTANT_CAMERA_LOCAL_POS,

			TOTAL_CONSTANTS
		}CONSTANSINDEX;

		typedef enum
		{
			TEMPORARY_POSITION,
			TEMPORARY_NORMAL,
			TEMPORARY_LIGHT_DIR,
			TEMPORARY_UV,
			TEMPORARY_COLOR,

			/*TEMPORARY_EYE_DIR,
			TEMPORARY_DOT,*/

			TOTAL_TEMPORARY
		}TEMPORARYINDEX;

		typedef enum
		{
			VARIABLE_POSITION = 0,

			TOTAL_VARIABLE
		}VARIABLEINDEX;

		typedef enum
		{

			TOTAL_CONSTANTVARIABLEDECLARATION
		}CONSTANTVARIABLEDECLARATIONINDEX;

	public:
		CCartoonEffect(UINT sampleIndex,UINT level = 3);
		~CCartoonEffect(void);

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
		static const CEffectCode::ELEMENTPARAMETER				sm_ATTRIBUTES[TOTAL_ATTRIBUTES];
		static const CEffectCode::ELEMENTPARAMETER				sm_VARYING   [TOTAL_VARYING   ];
		static const CEffectCode::GLOBALVARIABLE				sm_CONSTANS  [TOTAL_CONSTANTS ];
		static const CEffectCode::ATTRIBUTEVARIABLEDECLARATION	sm_VARIABLE  [TOTAL_VARIABLE  ];
		static const CEffectCode::TEMPORARYDECLARATION			sm_TEMPORARY [TOTAL_TEMPORARY ];
		//static const CEffectCode::CONSTANTVARIABLEDECLARATION	sm_CONVARIABLE[TOTAL_CONSTANTVARIABLEDECLARATION];

		CEffectInfoInterface::SAMPLER							m_SAMPLER	 [TOTAL_SAMPLERS  ];
		UINT													m_LightLevel;
	};

	ALCHEMY_INLINE UINT                                         CCartoonEffect::GetEffectInfoFlag()
	{
		return 0;
	}

	ALCHEMY_INLINE const CEffectInfoInterface::SAMPLER      * CCartoonEffect::GetSamplers  (UINT& uSamplerCount  )
	{
		uSamplerCount = TOTAL_SAMPLERS;

		return m_SAMPLER;
	}

	ALCHEMY_INLINE const CEffectCode::ELEMENTPARAMETER         * CCartoonEffect::GetAttributes(UINT& uAttributeCount)
	{
		uAttributeCount = TOTAL_ATTRIBUTES;

		return sm_ATTRIBUTES;
	}

	ALCHEMY_INLINE const CEffectCode::ELEMENTPARAMETER         * CCartoonEffect::GetVarying   (UINT& uVaryingCount  )
	{
		uVaryingCount = TOTAL_VARYING;

		return sm_VARYING;
	}

	ALCHEMY_INLINE const CEffectCode::GLOBALVARIABLE           * CCartoonEffect::GetConstants (UINT& uConstantCount )
	{
		uConstantCount = TOTAL_CONSTANTS;

		return sm_CONSTANS;
	}

	ALCHEMY_INLINE const CEffectCode::TEMPORARYDECLARATION      * CCartoonEffect::GetTemporary (UINT& uTemporaryCount)
	{
		uTemporaryCount = TOTAL_TEMPORARY;

		return sm_TEMPORARY;
	}

	ALCHEMY_INLINE const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CCartoonEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
	{
		uAttributeVariableCount = TOTAL_VARIABLE;

		return sm_VARIABLE;
	}

	ALCHEMY_INLINE const CEffectCode::CONSTANTVARIABLEDECLARATION * CCartoonEffect::GetConstantVariables (UINT& uConstantVariableCount )
	{
		uConstantVariableCount = TOTAL_CONSTANTVARIABLEDECLARATION;

		return ALCHEMY_NULL;
	}
}