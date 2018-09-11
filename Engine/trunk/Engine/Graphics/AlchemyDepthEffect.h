#pragma once

#include "AlchemyEffectInfoInterface.h"

namespace alchemy
{
	class CDepthEffect :
		public EFFECTINFO
	{
		typedef enum
		{
			TOTAL_SAMPLERS = 0
		}SAMPLERINDEX;

		typedef enum
		{
			TOTAL_ATTRIBUTES = 0
		}ATTRIBUTEINDEX;

		typedef enum
		{
			VARYING_DEPTH,

			TOTAL_VARYING
		}VARYINGINDEX;

		typedef enum
		{
			CONSTANT_WORLD_VIEW_PROJECTION_MATRIX = 0,

			CONSTANT_FLOAT2RGB,
			//CONSTANT_CAMERA_NEAR,
			//CONSTANT_CAMERA_DISTANTCE,

			TOTAL_CONSTANTS
		}CONSTANSINDEX;

		typedef enum
		{
			//TEMPORARY_DEPTH,

			TOTAL_TEMPORARY
		}TEMPORARYINDEX;

		typedef enum
		{
			ATTRIBUTE_VARIABLE_POSITION = 0,

			TOTAL_ATTRIBUTE_VARIABLES
		}ATTRIBUTEVARIABLEINDEX;

		typedef enum
		{
			TOTAL_CONSTANT_VARIABLES = 0
		}CONSTANTVARIABLEINDEX;
	public:
		typedef enum
		{
			PERSPECTIVE_TYPE,
			ORTHOGGONAL_TYPE
		}PROJECTIONTYPE;

		CDepthEffect(PROJECTIONTYPE Type = PERSPECTIVE_TYPE, bool isHalfDepth = true);
		~CDepthEffect(void);

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
		PROJECTIONTYPE								m_eProjectionType;
		bool										m_bIsHalfDepth;

		UINT										m_uConstantNum;
		CEffectCode::GLOBALVARIABLE*				m_pConstants;

		UINT										m_uTemporaryNum;
		CEffectCode::TEMPORARYDECLARATION*			m_pTemporaries;

		static const CEffectCode::ELEMENTDECLARATION            sm_VARYING           [TOTAL_VARYING            ];
		static const CEffectCode::ATTRIBUTEVARIABLEDECLARATION	sm_ATTRIBUTE_VARIABLES[TOTAL_ATTRIBUTE_VARIABLES];
	};
}