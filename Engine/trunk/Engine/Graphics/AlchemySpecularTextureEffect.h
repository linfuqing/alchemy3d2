#pragma once

#include "AlchemyEffectInfoInterface.h"

namespace alchemy
{
	class CSpecularTextureEffect : 
		public EFFECTINFO
	{
		typedef enum
		{
			SAMPLER_TEXTURE =  0, 

			TOTAL_SAMPLERS
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
			TEMPORARY_COLOR,

			TOTAL_TEMPORARY
		}TEMPORARYINDEX;

		typedef enum
		{
			ATTRIBUTE_VARIABLE_TEXCOORD = 0,

			TOTAL_ATTRIBUTE_VARIABLES
		}ATTRIBUTEVARIABLEINDEX;

		typedef enum
		{
			CONSTANT_VARIABLE_SPECULAR,

			TOTAL_CONSTANT_VARIABLES
		}CONSTANTVARIABLEINDEX;

	public:
		CSpecularTextureEffect(UINT uChannelIndex = 3, UINT uSamplerIndex = 0, UINT uTexcoordIndex = 0, const IEffect::SAMPLERDECLARATION* pSamplerDeclaration = ALCHEMY_NULL);
		virtual ~CSpecularTextureEffect();

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
		UINT												m_uChanndelIndex;
		SAMPLER												m_Samplers          [TOTAL_SAMPLERS          ];
		CEffectCode::ATTRIBUTEVARIABLEDECLARATION			m_AttributeVariables[TOTAL_ATTRIBUTE_VARIABLES];


		static CEffectCode::TEMPORARYDECLARATION			sm_TEMPORARY[TOTAL_TEMPORARY];
		static CEffectCode::CONSTANTVARIABLEDECLARATION		sm_CONSTANTVARIABLES[TOTAL_CONSTANT_VARIABLES];
	};
}