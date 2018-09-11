#pragma once

#include "AlchemyEffectInfoInterface.h"

namespace alchemy
{
	class CSilhouetteEffect :
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
			VARYING_POSITIONT,

			TOTAL_VARYING
		}VARYINGINDEX;

		typedef enum
		{
			CONSTANT_PROJECTION_MATRIX,
			CONSTANT_ZERO,
			
			CONSTANT_SIOUETTE_WIDTH,
			CONSTANT_SIOUETTE_COLOR,

			TOTAL_CONSTANTS
		}CONSTANSINDEX;

		typedef enum
		{
			TEMPORARY_EYE_DIRECTION,
			TEMPORARY_DOT0,
			TEMPORARY_DOT1,
			TEMPORARY_POSITION,

			TOTAL_TEMPORARY
		}TEMPORARYINDEX;

		typedef enum
		{
			ATTRIBUTE_VIEW_POSITION,
			ATTRIBUTE_VIEW_NORMAL0,
			ATTRIBUTE_VIEW_NORMAL1,
			ATTRIBUTE_VIEW_NORMAL2,

			TOTAL_ATTRIBUTE_VARIABLE
		}ATTRIBUTEVARIABLEINDEX;

		typedef enum
		{
			TOTAL_CONSTANT_VARIABLE
		}CONSTANTVARIABLEINDEX;

	public:
		CSilhouetteEffect(FLOAT4* pSiouetteColor, FLOAT* pWidth);
		~CSilhouetteEffect(void);

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
			const CEffectCode::SHADERVARIABLE* pAttributeVariable,
			const CEffectCode::SHADERVARIABLE* pConstantVariable,
			CEffectCode::IInstructionSet& VertexShaderDefiner,
			CEffectCode::IInstructionSet& FragmentShaderDefiner,
			CEffectCode::CPassDefiner::CEffectCodeUtils& EffectCodeUtils);

	private:
		CEffectCode::GLOBALVARIABLE								m_Constans[TOTAL_CONSTANTS];
		static const CEffectCode::ELEMENTDECLARATION				sm_VARYING[TOTAL_VARYING];
		static const CEffectCode::TEMPORARYDECLARATION			sm_TEMPORARYVARIABLE[TOTAL_TEMPORARY];
		static const CEffectCode::ATTRIBUTEVARIABLEDECLARATION  sm_ATTRIBUTEVARIABLE[TOTAL_ATTRIBUTE_VARIABLE];
	};
}