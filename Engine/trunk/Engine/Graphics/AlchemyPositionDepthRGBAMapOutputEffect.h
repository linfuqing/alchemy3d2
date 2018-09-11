#pragma once

#include "AlchemyEffectInfoInterface.h"

namespace alchemy
{
	class CPositionDepthRGBAMapOutputEffect :
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
			VARYING_POSITION = 0,

			TOTAL_VARYING
		}VARYINGINDEX;

		typedef enum
		{
			CONSTANT_INVERSE_PROJECTION_MATRIX = 0,

			CONSTANT_ZERO,

			CONSTANT_RGBA_TO_FLOAT_CODE,

			TOTAL_CONSTANTS
		}CONSTANSINDEX;

		typedef enum
		{
			//TEMPORARY_VERTEX_TEMP = 0,
			TEMPORARY_TEMP = 0,

			TOTAL_TEMPORARY
		}TEMPORARYINDEX;

		typedef enum
		{
			ATTRIBUTE_VARIABLE_SOURCE_POSITION = 0,

			ATTRIBUTE_VARIABLE_DESTINATION_POSITION,

			ATTRIBUTE_VARIABLE_TEXCOORD,

			TOTAL_ATTRIBUTE_VARIABLES
		}ATTRIBUTEVARIABLEINDEX;

		typedef enum
		{
			CONSTANT_VARIABLE_MATERIAL_AMBIENT = 0,

			TOTAL_CONSTANT_VARIABLES
		}CONSTANTVARIABLEINDEX;
	public:
		CPositionDepthRGBAMapOutputEffect(UINT uSamplerIndex, IEffect::SAMPLERFILTER SamplerFilter);
		~CPositionDepthRGBAMapOutputEffect(void);

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
		static const CEffectCode::ELEMENTDECLARATION           sm_VARYING           [TOTAL_VARYING            ];
		static const CEffectCode::GLOBALVARIABLE               sm_CONSTANS          [TOTAL_CONSTANTS          ];
		static const CEffectCode::TEMPORARYDECLARATION         sm_TEMPORARY         [TOTAL_TEMPORARY          ];
		static const CEffectCode::ATTRIBUTEVARIABLEDECLARATION sm_ATTRIBUTE_VARIABLE[TOTAL_ATTRIBUTE_VARIABLES];
		static const CEffectCode::CONSTANTVARIABLEDECLARATION  sm_CONSTANT_VARIABLE [TOTAL_CONSTANT_VARIABLES ];

		SAMPLER                                                m_Samplers           [TOTAL_SAMPLERS           ];
	};
}