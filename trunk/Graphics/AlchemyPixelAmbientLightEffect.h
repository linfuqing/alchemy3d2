#pragma once

#include "AlchemyEffectInfoInterface.h"

namespace alchemy
{
	class CPixelAmbientLightEffect :
		public CEffectInfoInterface
	{
		typedef enum
		{
			TOTAL_SAMPLERS = 0
		}SAMPLERINDEX;

		typedef enum
		{
			ATTRIBUTE_POSITION = 0,

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
			CONSTANT_AMBIENT_COLOR,

			TOTAL_CONSTANTS
		}CONSTANSINDEX;

		typedef enum
		{
			TOTAL_TEMPORARY = 0
		}TEMPORARYINDEX;

		typedef enum
		{
			TOTAL_ATTRIBUTE_VARIABLES = 0
		}ATTRIBUTEVARIABLEINDEX;

		typedef enum
		{
			TOTAL_CONSTANT_VARIABLES = 0
		}CONSTANTVARIABLEINDEX;
	public:
		CPixelAmbientLightEffect(void);
		~CPixelAmbientLightEffect(void);

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
		static const CEffectCode::ELEMENTPARAMETER sm_ATTRIBUTES[TOTAL_ATTRIBUTES];
		static const CEffectCode::ELEMENTPARAMETER sm_VARYING   [TOTAL_VARYING   ];

		static const CEffectCode::GLOBALVARIABLE   sm_CONSTANS  [TOTAL_CONSTANTS ];
	};

	ALCHEMY_INLINE UINT                                       CPixelAmbientLightEffect::GetEffectInfoFlag     (                             )
	{
		return ALCHEMY_FLAG(CEffectInfoInterface::FRAGMENT_SHADER_OUTPUT);
	}

	ALCHEMY_INLINE const CEffectInfoInterface::SAMPLER      * CPixelAmbientLightEffect::GetSamplers  (UINT& uSamplerCount  )
	{
		uSamplerCount = TOTAL_SAMPLERS;

		return ALCHEMY_NULL;
	}

	ALCHEMY_INLINE const CEffectCode::ELEMENTPARAMETER* CPixelAmbientLightEffect::GetAttributes(UINT& uAttributeCount)
	{
		uAttributeCount = TOTAL_ATTRIBUTES;

		return sm_ATTRIBUTES;
	}

	ALCHEMY_INLINE const CEffectCode::ELEMENTPARAMETER* CPixelAmbientLightEffect::GetVarying   (UINT& uVaryingCount  )
	{
		uVaryingCount = TOTAL_VARYING;

		return sm_VARYING;
	}

	ALCHEMY_INLINE const CEffectCode::GLOBALVARIABLE        * CPixelAmbientLightEffect::GetConstants (UINT& uConstantCount )
	{
		uConstantCount = TOTAL_CONSTANTS;

		return sm_CONSTANS;
	}

	ALCHEMY_INLINE const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CPixelAmbientLightEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
	{
		uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLES;

		return ALCHEMY_NULL;
	}

	ALCHEMY_INLINE const CEffectCode::CONSTANTVARIABLEDECLARATION * CPixelAmbientLightEffect::GetConstantVariables  (UINT& uConstantVariableCount)
	{
		uConstantVariableCount = TOTAL_CONSTANT_VARIABLES;

		return ALCHEMY_NULL;
	}
}
