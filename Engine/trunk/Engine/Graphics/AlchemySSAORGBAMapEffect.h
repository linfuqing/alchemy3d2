#pragma once

#include "AlchemyEffectInfoInterface.h"

#include "../Math/AlchemyFloat2.h"

namespace alchemy
{
	class CSSAORGBAMapEffect :
		public EFFECTINFO
	{
		typedef enum
		{
			SAMPLER_DEPTH =  0, 
			SAMPLER_NORMAL,
			SAMPLER_RANDOM,

			TOTAL_SAMPLERS
		}SAMPLERINDEX;

		typedef enum
		{
			TOTAL_ATTRIBUTES = 0
		}ATTRIBUTEINDEX;

		typedef enum
		{
			//VARYING_POSITION = 0,

			TOTAL_VARYING = 0
		}VARYINGINDEX;

		typedef enum
		{
			//CONSTANT_INVERSE_PROJECTION_MATRIX = 0,

			CONSTANT_ZERO = 0,

			CONSTANT_ONE,

			CONSTANT_TWO,

			CONSTANT_RGBA_TO_FLOAT_CODE,

			CONSTANT_CAMERA_DISTANCE,

			CONSTANT_RANDOM_MAP_UV_SCALE,

			CONSTANT_SAMPLE_RADIUS,

			CONSTANT_INVERSE_FOCAL_LENGTH,

			CONSTANT_SCALE,

			CONSTANT_BIAS, 

			CONSTANT_INTENSITY, 

			CONSTANT_ROTATION_DIRECTION

			//TOTAL_CONSTANTS
		}CONSTANSINDEX;

		typedef enum
		{
			//TEMPORARY_VERTEX_TEMP = 0,
			TEMPORARY_DEPTH = 0,
			TEMPORARY_SOURCE_POSITION,
			TEMPORARY_NORMAL,
			TEMPORARY_RANDOM,
			TEMPORARY_RADIUS,
			TEMPORARY_TEXCOORD,
			TEMPORARY_REFLECTION,
			TEMPORARY_DESTINATION_POSITION,
			TEMPORAPY_AO,
			TEMPORARY_RESULT,

			TOTAL_TEMPORARY
		}TEMPORARYINDEX;

		typedef enum
		{
			//ATTRIBUTE_VARIABLE_SOURCE_POSITION = 0,

			ATTRIBUTE_VARIABLE_DESTINATION_POSITION = 0,

			ATTRIBUTE_VARIABLE_NORMAL,

			ATTRIBUTE_VARIABLE_TEXCOORD,

			TOTAL_ATTRIBUTE_VARIABLES
		}ATTRIBUTEVARIABLEINDEX;

		typedef enum
		{
			CONSTANT_VARIABLE_MATERIAL_AMBIENT = 0,

			TOTAL_CONSTANT_VARIABLES
		}CONSTANTVARIABLEINDEX;
	public:
		CSSAORGBAMapEffect(
			UINT uDepthSamplerIndex, 
			UINT uNormalSamplerIndex, 
			UINT uRandomSamplerIndex, 
			UINT uNumDirection,
			UINT uStep,
			const FLOAT2* pRandomMapUVScale = ALCHEMY_NULL, 
			const FLOAT2* pSamplerRadius = ALCHEMY_NULL, 
			const FLOAT2* pInverseFocalLength = ALCHEMY_NULL, 
			const FLOAT* pfScale = ALCHEMY_NULL, 
			const FLOAT* pfBias = ALCHEMY_NULL, 
			const FLOAT* pfIntensity = ALCHEMY_NULL, 
			const FLOAT2* pRotationDirection = ALCHEMY_NULL);
		~CSSAORGBAMapEffect(void);

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
		//static const CEffectCode::ELEMENTDECLARATION           sm_VARYING           [TOTAL_VARYING            ];
		static const CEffectCode::TEMPORARYDECLARATION         sm_TEMPORARY         [TOTAL_TEMPORARY          ];
		static const CEffectCode::ATTRIBUTEVARIABLEDECLARATION sm_ATTRIBUTE_VARIABLE[TOTAL_ATTRIBUTE_VARIABLES];
		static const CEffectCode::CONSTANTVARIABLEDECLARATION  sm_CONSTANT_VARIABLE [TOTAL_CONSTANT_VARIABLES ];

		SAMPLER                                                m_Samplers           [TOTAL_SAMPLERS           ];
		CEffectCode::LPGLOBALVARIABLE                          m_pConstants;

		UINT                                                   m_uNumDirection;
		UINT                                                   m_uStep;

		FLOAT2                                                 m_RandomMapUVScale;
		FLOAT2                                                 m_SamplerRadius;
		FLOAT2                                                 m_InverseFocalLength;
		FLOAT                                                  m_fScale;
		FLOAT                                                  m_fBias;
		FLOAT                                                  m_fIntensity;

		CFloat2*                                               m_pRotationDirection;
	};
}