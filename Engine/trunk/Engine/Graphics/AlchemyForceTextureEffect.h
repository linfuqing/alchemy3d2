#pragma once

#include "AlchemyEffectInfoInterface.h"
#include "AlchemyRenderMethod.h"

namespace alchemy
{
	class CForceTextureEffect :
		public EFFECTINFO
	{
		typedef enum
		{
			SAMPLER_TEXTURE =  0, 

			TOTAL_SAMPLERS
		}SAMPLERINDEX;

		typedef enum
		{
			ATTRIBUTE_TEXCOORD = 0,

			TOTAL_ATTRIBUTES
		}ATTRIBUTEINDEX;

		typedef enum
		{
			VARYING_TEXCOORD = 0,

			TOTAL_VARYING
		}VARYINGINDEX;

		typedef enum
		{
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
		CForceTextureEffect(UINT uSamplerIndex = 0, UINT uTexcoordIndex = 0, const IEffect::SAMPLERDECLARATION* pSamplerDeclaration = ALCHEMY_NULL);
		~CForceTextureEffect(void);

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
		SAMPLER                       m_Samplers  [TOTAL_SAMPLERS  ];
		CEffectCode::ELEMENTDECLARATION m_Attributes[TOTAL_ATTRIBUTES];
		CEffectCode::ELEMENTDECLARATION m_Varing    [TOTAL_VARYING   ];
	};
}