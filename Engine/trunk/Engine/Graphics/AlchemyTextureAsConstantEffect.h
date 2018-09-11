#pragma once

#include "AlchemyEffectInfoInterface.h"

namespace alchemy
{
	class CTextureAsConstantEffect :
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
			TOTAL_TEMPORARY = 0
		}TEMPORARYINDEX;

		typedef enum
		{
			ATTRIBUTE_VARIABLE_TEXCOORD = 0,

			TOTAL_ATTRIBUTE_VARIABLES
		}ATTRIBUTEVARIABLEINDEX;

		typedef enum
		{
			CONSTANT_VARIABLE_OUTPUT = 0,

			TOTAL_CONSTANT_VARIABLES
		}CONSTANTVARIABLEINDEX;
	public:
		CTextureAsConstantEffect(CEffectCode::GLOBALVARIABLETYPE Type, UINT uDetail, UINT uSamplerIndex, UINT uTexcoordIndex, const IEffect::SAMPLERDECLARATION* pSamplerDeclaration);
		~CTextureAsConstantEffect(void);

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
		SAMPLER                                   m_Samplers          [TOTAL_SAMPLERS           ];
		CEffectCode::ATTRIBUTEVARIABLEDECLARATION m_AttributeVariables[TOTAL_ATTRIBUTE_VARIABLES];
		CEffectCode::CONSTANTVARIABLEDECLARATION  m_ConstantVariables [TOTAL_CONSTANT_VARIABLES ];
	};
}
