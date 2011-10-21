#pragma once

#include "AlchemyEffectInfoInterface.h"

namespace alchemy
{
	class CTexture2DEffect :
		public CEffectInfoInterface
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
			VARYING_TEXCOORD,

			TOTAL_VARYING
		}VARYINGINDEX;

		typedef enum
		{
			CONSTANT_HALF_ONE = 0,

			TOTAL_CONSTANTS
		}CONSTANSINDEX;

		typedef enum
		{
			TEMPORARY_VERTEX_TEXCOORD = 0,
			TEMPORARY_FRAGMENT_TEXCOORD,

			TOTAL_TEMPORARY
		}TEMPORARYINDEX;

		typedef enum
		{
			ATTRIBUTE_VARIABLE_POSITIONT = 0,

			ATTRIBUTE_VARIABLE_TEXCOORD,

			TOTAL_ATTRIBUTE_VARIABLES
		}ATTRIBUTEVARIABLEINDEX;

		typedef enum
		{
			TOTAL_CONSTANT_VARIABLES = 0
		}CONSTANTVARIABLEINDEX;
	public:
		CTexture2DEffect(bool bIsOutputTexture, UINT uSamplerIndex = 0, CEffectCode::SAMPLERFILTER SamplerFilter = CEffectCode::LINEAR);
		~CTexture2DEffect(void);

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
		static const CEffectCode::ELEMENTPARAMETER             sm_VARYING           [TOTAL_VARYING            ];
		static const CEffectCode::GLOBALVARIABLE               sm_CONSTANS          [TOTAL_CONSTANTS          ];
		static const CEffectCode::TEMPORARYDECLARATION         sm_TEMPORARY         [TOTAL_TEMPORARY          ];
		static const CEffectCode::ATTRIBUTEVARIABLEDECLARATION sm_ATTRIBUTE_VARIABLE[TOTAL_ATTRIBUTE_VARIABLES];

		SAMPLER                                                m_Samplers           [TOTAL_SAMPLERS           ];

		bool                                                   m_bIsOutputTexture;
	};
}