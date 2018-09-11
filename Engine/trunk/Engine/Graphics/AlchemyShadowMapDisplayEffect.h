#pragma once
#include "AlchemyEffectInfoInterface.h"

namespace alchemy
{
	class CShadowMapDisplayEffect :
		public EFFECTINFO
	{
		typedef enum
		{
			SAMPLER_DEPTH =  0, 

			TOTAL_SAMPLERS
		}SAMPLERINDEX;

		typedef enum
		{
			TOTAL_ATTRIBUTES
		}ATTRIBUTEINDEX;

		typedef enum
		{
			VARYING_POSITION = 0,
			VARYING_LIGHT_POSITION,

			TOTAL_VARYING
		}VARYINGINDEX;

		typedef enum
		{
			CONSTANT_LIGHT_WORLD_VIEW_PROJECTION_MATRIX = 0,

			CONSTANT_HALF_ONE,

			CONSTANT_CAMERA_NEAR,

			CONSTANT_CAMERA_DISTANCE,

			CONSTANT_LIGHT_LOCAL_POSITION,

			CONSTANT_LIGHT_LOCAL_DIRECTION,

			CONSTANT_LIGHT_COS_THETA,

			CONSTANT_EPSILON,

			TOTAL_CONSTANTS
		}CONSTANSINDEX;

		typedef enum
		{
			TEMPORARY_LIGHT_POSITION = 0,
			TEMPORARY_LIGHT_DIRECTION,
			TEMPORARY_TEXCOORD,
			TEMPORARY_DEPTH,
			TEMPORARY_LIGHT_AMOUNT,

			TOTAL_TEMPORARY
		}TEMPORARYINDEX;

		typedef enum
		{
			ATTRIBUTE_VARIABLE_POSITION = 0,

			TOTAL_ATTRIBUTE_VARIABLES
		}ATTRIBUTEVARIABLEINDEX;

		typedef enum
		{
			CONSTANT_VARIABLE_LIGHT_DIFFUSE = 0,
			CONSTANT_VARIABLE_LIGHT_SPECULAR,

			TOTAL_CONSTANT_VARIABLES
		}CONSTANTVARIABLEINDEX;

	public:
		CShadowMapDisplayEffect(UINT uShadowMapIndex, UINT uLightIndex, UINT uCameraIndex);
		~CShadowMapDisplayEffect(void);

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
		//static const CEffectCode::ELEMENTDECLARATION             sm_ATTRIBUTES       [TOTAL_ATTRIBUTES        ];
		static const CEffectCode::TEMPORARYDECLARATION         sm_TEMPORARY        [TOTAL_TEMPORARY         ];
		static const CEffectCode::ATTRIBUTEVARIABLEDECLARATION sm_ATTRIBUTE_VARIABLE[TOTAL_ATTRIBUTE_VARIABLES];
		static const FLOAT sm_fEpsilon;

		IEffectInfo::SAMPLER                          m_Samplers          [TOTAL_SAMPLERS          ];
		CEffectCode::ELEMENTDECLARATION                          m_Varying           [TOTAL_VARYING           ];
		CEffectCode::GLOBALVARIABLE                            m_Contants          [TOTAL_CONSTANTS         ];
		CEffectCode::CONSTANTVARIABLEDECLARATION               m_ConstantVariable  [TOTAL_CONSTANT_VARIABLES ];

	};
}