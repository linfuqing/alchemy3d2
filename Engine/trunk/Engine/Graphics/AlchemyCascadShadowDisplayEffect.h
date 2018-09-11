#pragma once

#include "AlchemyEffectInfoInterface.h"

namespace alchemy
{
	class CCascadShadowDisplayEffect : 
		public EFFECTINFO
	{
		typedef enum
		{
			VARYING_LIGHT_POSITION,

			TOTAL_VARYING
		}VARYINGINDEX;

		typedef enum
		{
			CONSTANT_LIGHT_WORLD_VIEW_PROJECTION_MATRIX = 0,

			CONSTANT_HALF_ONE,

			CONSTANT_CAMERA_NEAR,

			CONSTANT_CAMERA_DISTANCE,

			CONSTANT_EPSILON,

			CONSTANT_ZERO,

			CONSTANT_RGB2FLOAT,

			TOTAL_CONSTANTS
		}CONSTANSINDEX;

		typedef enum
		{
			TEMPORARY_LIGHT_POSITION = 0,
			TEMPORARY_DEPTH,
			TEMPORARY_TEXCOORD_INFO,
			TEMPORARY_RGBVALUE,

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
		CCascadShadowDisplayEffect(const UINT* pSamplerIndices, const FLOAT* pDepthSegment, const UINT* pSpilitCameraIndices,  UINT uSamplerCount, UINT uLightIndex, bool isHalfDepth = true);
		virtual ~CCascadShadowDisplayEffect();

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
		UINT					m_uSamplerNum;
		SAMPLER*				m_pSamplers;

		bool					m_bIsHalfDepth;

		static const CEffectCode::TEMPORARYDECLARATION         sm_TEMPORARY         [TOTAL_TEMPORARY          ];
		static const CEffectCode::ATTRIBUTEVARIABLEDECLARATION sm_ATTRIBUTE_VARIABLE[TOTAL_ATTRIBUTE_VARIABLES];
		static const FLOAT									   sm_fEpsilon;

		UINT*												   m_pExternaConstantInices;
		UINT												   m_uExternConstantNum;
		UINT												   m_uExternalConstantMatrixNum;
		CEffectCode::GLOBALVARIABLE*                           m_pContants;

		CEffectCode::ELEMENTDECLARATION*					   m_pVarying;
		UINT												   m_uExternalVaryNum;

		//CEffectCode::ELEMENTDECLARATION                        m_Varying           [TOTAL_VARYING            ];
		CEffectCode::CONSTANTVARIABLEDECLARATION               m_ConstantVariable  [TOTAL_CONSTANT_VARIABLES ];
	};
}