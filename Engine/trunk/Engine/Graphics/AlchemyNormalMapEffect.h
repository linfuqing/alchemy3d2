#pragma once

#include "AlchemyEffectInfoInterface.h"

namespace alchemy
{
	class CNormalMapEffect :
		public EFFECTINFO
	{
		typedef enum
		{
			SAMPLER_NORMAL =  0, 

			TOTAL_SAMPLERS
		}SAMPLERINDEX;

		typedef enum
		{
			ATTRIBUTE_NORAML = 0,
			ATTRIBUTE_TANGENT,
			ATTRIBUTE_TEXCOORD,

			TOTAL_ATTRIBUTES
		}ATTRIBUTEINDEX;

		typedef enum
		{
			VARYING_TEXCOORD = 0,

			TOTAL_VARYING
		}VARYINGINDEX;

		typedef enum
		{
			CONSTANT_HALF_ONE = 0,

			TOTAL_CONSTANTS
		}CONSTANSINDEX;

		typedef enum
		{
			TEMPORARY_BITNORMAL = 0,
			TEMPORARY_SOURCE,

			TOTAL_TEMPORARY
		}TEMPORARYINDEX;

		typedef enum
		{
			ATTRIBUTE_VARIABLE_BINORMAL = 0,
			ATTRIBUTE_VARIABLE_NORAML,

			TOTAL_ATTRIBUTE_VARIABLES
		}ATTRIBUTEVARIABLEINDEX;
	public:
		CNormalMapEffect(UINT uNormalMapUVIndex, UINT uNormalMapIndex, UINT uNumLights = 0, PUINT puLightIndices = ALCHEMY_NULL);
		~CNormalMapEffect(void);

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
		static const CEffectCode::GLOBALVARIABLE               sm_CONSTANTS          [TOTAL_CONSTANTS          ];
		static const CEffectCode::TEMPORARYDECLARATION         sm_TEMPORARY          [TOTAL_TEMPORARY          ];
		static const CEffectCode::ATTRIBUTEVARIABLEDECLARATION sm_ATTRIBUTE_VARIABLES[TOTAL_ATTRIBUTE_VARIABLES];

		SAMPLER                                                m_Samplers            [TOTAL_SAMPLERS           ];
		CEffectCode::ELEMENTDECLARATION                          m_Attributes          [TOTAL_ATTRIBUTES         ];
		CEffectCode::ELEMENTDECLARATION                          m_Varying             [TOTAL_VARYING            ];

		CEffectCode::LPCONSTANTVARIABLEDECLARATION             m_pConstantVariables;

		UINT m_uConstantVariableCount;
	};
}