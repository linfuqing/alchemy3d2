#pragma once

#include "AlchemyEffectInfoInterface.h"

#include "../Math/AlchemyFloat2.h"

namespace alchemy
{
	class CTextureBlurEffect :
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
			TEMPORARY_SOURCE_COLOR,
			TEMPORARY_FINAL_COLOR,
			TEMPORAY_UV,

			TOTAL_TEMPORARY
		}TEMPORARYINDEX;

		typedef enum
		{
			ATTRIBUTE_VARIABLE_TEXCOORD = 0,

			TOTAL_ATTRIBUTE_VARIABLES
		}ATTRIBUTEVARIABLEINDEX;

		typedef enum
		{
			TOTAL_CONSTANT_VARIABLES = 0
		}CONSTANTVARIABLEINDEX;
	public:
		CTextureBlurEffect(const FLOAT2* pSamplerOffsets, const FLOAT* pSamplerWeights, UINT uSamplerAttributeNum, UINT uSamplerIndex = 0, UINT uTexcoordIndex = 0, const IEffect::SAMPLERDECLARATION* pSamplerDeclaration = ALCHEMY_NULL);
		virtual ~CTextureBlurEffect();

		virtual UINT                                             GetEffectInfoFlag    (                             );

		virtual const SAMPLER                                  * GetSamplers          (UINT& uSamplerCount          );

		virtual const CEffectCode::ELEMENTDECLARATION            * GetAttributes        (UINT& uAttributeCount        );
		virtual const CEffectCode::ELEMENTDECLARATION            * GetVarying           (UINT& uVaryingCount          );

		virtual const CEffectCode::GLOBALVARIABLE              * GetConstants         (UINT& uConstantCount         );

		virtual const CEffectCode::TEMPORARYDECLARATION        * GetTemporary         (UINT& uTemporaryCount        );

		virtual const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* GetAttributeVariables(UINT& uAttributeVariableCount);

		virtual const CEffectCode::CONSTANTVARIABLEDECLARATION * GetConstantVariables (UINT& uConstantVariableCount );

		virtual bool ShaderDefineHandler(
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
		UINT											m_uSamplerAttributeNum;
		UINT											m_uConstantNum;
		CEffectCode::GLOBALVARIABLE*					m_pConstants;

		SAMPLER											m_Samplers          [TOTAL_SAMPLERS          ];
		CEffectCode::ATTRIBUTEVARIABLEDECLARATION		m_AttributeVariables[TOTAL_ATTRIBUTE_VARIABLES];

		static const CEffectCode::TEMPORARYDECLARATION	sm_TEMPORARYS		[TOTAL_TEMPORARY];
	};
}