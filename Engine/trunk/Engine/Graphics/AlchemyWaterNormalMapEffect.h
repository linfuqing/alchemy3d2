#pragma once

#include "AlchemyEffectInfoInterface.h"
#include "../Math/AlchemyFloat2.h"

namespace alchemy
{
	class CWaterNormalMapEffect:
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
			VARYING_TEXCOORD0 = 0,
			VARYING_TEXCOORD1,
			VARYING_TEXCOORD2,
			VARYING_TEXCOORD3,

			TOTAL_VARYING
		}VARYINGINDEX;

		typedef enum
		{
			CONSTANT_TRANSLATION0 = 0,
			CONSTANT_TRANSLATION1,
			CONSTANT_TRANSLATION2,
			CONSTANT_TRANSLATION3,

			CONSTANT_VERTEX_TWO,
			CONSTANT_FRAGMENT_TWO,

			TOTAL_CONSTANTS
		}CONSTANSINDEX;

		typedef enum
		{
			TEMPORARY_VERTEX = 0,
			TEMPORARY_SOURCE,
			TEMPORARY_FRAGMENT,

			TOTAL_TEMPORARY
		}TEMPORARYINDEX;

		typedef enum
		{
			ATTRIBUTE_VARIABLE_BINORMAL = 0,
			ATTRIBUTE_VARIABLE_NORAML,

			TOTAL_ATTRIBUTE_VARIABLES
		}ATTRIBUTEVARIABLEINDEX;

		typedef enum
		{
			CONSTANT_VARIABLES_VERTEX_SHADER_CAMERA_LOCAL_DIRECTION = 0,
			CONSTANT_VARIABLES_VERTEX_SHADER_CAMERA_LOCAL_DIRECTION_NEGATE,
			CONSTANT_VARIABLES_FRAGMENT_SHADER_CAMERA_LOCAL_DIRECTION,
			CONSTANT_VARIABLES_FRAGMENT_SHADER_CAMERA_LOCAL_DIRECTION_NEGATE,

			TOTAL_CONSTANT_VARIABLES
		}CONSTANTVARIABLEINDEX;
	public:
		CWaterNormalMapEffect(FLOAT2& Translation0, FLOAT2& Translation1, FLOAT2& Translation2, FLOAT2& Translation3, UINT uNormalMapIndex = 0);
		~CWaterNormalMapEffect(void);

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
		static const CEffectCode::TEMPORARYDECLARATION         sm_TEMPORARY          [TOTAL_TEMPORARY          ];
		static const CEffectCode::ATTRIBUTEVARIABLEDECLARATION sm_ATTRIBUTE_VARIABLES[TOTAL_ATTRIBUTE_VARIABLES];
		static const CEffectCode::CONSTANTVARIABLEDECLARATION  sm_CONSTANT_VARIABLES [TOTAL_CONSTANT_VARIABLES ];

		SAMPLER                                                m_Samplers            [TOTAL_SAMPLERS           ];
		CEffectCode::GLOBALVARIABLE                            m_Constants           [TOTAL_CONSTANTS          ];
		CEffectCode::ELEMENTDECLARATION                          m_Attributes          [TOTAL_ATTRIBUTES         ];
		CEffectCode::ELEMENTDECLARATION                          m_Varying             [TOTAL_VARYING            ];
	};
}