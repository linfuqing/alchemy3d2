#pragma once

#include "AlchemyEffectInfoInterface.h"

namespace alchemy
{
	class CDistanceInfoEffect : 
		public CEffectInfoInterface
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
			VARYING_VIEW_POSITION,

			TOTAL_VARYING
		}VARYINGINDEX;

		typedef enum
		{
			CONSTANT_WORLD_VIEW_MATRIX,

			CONSTANT_VIEW_DIR,
			CONSTANT_FOUCS_DISTANCE,

			TOTAL_CONSTANTS
		}CONSTANSINDEX;

		typedef enum
		{
			TEMPORARY_FOCUS_PLANE,

			TOTAL_TEMPORARY
		}TEMPORARYINDEX;

		typedef enum
		{
			ATTRIBUTE_POSITION,

			TOTAL_ATTRIBUTE_VARIABLE
		}ATTRIBUTEVARIABLEINDEX;

		typedef enum
		{
			TOTAL_CONSTANT_VARIABLE = 0
		}CONSTANTVARIABLEINDEX;

	public:
		CDistanceInfoEffect();
		virtual ~CDistanceInfoEffect();

		bool Create(const FLOAT& FocusDistance);

		virtual UINT                                             GetEffectInfoFlag    (                             );

		virtual const SAMPLER                                  * GetSamplers          (UINT& uSamplerCount          );

		virtual const CEffectCode::ELEMENTPARAMETER            * GetAttributes        (UINT& uAttributeCount        );
		virtual const CEffectCode::ELEMENTPARAMETER            * GetVarying           (UINT& uVaryingCount          );

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
			const CEffectCode::SHADERVARIABLE* pAttributeVariable,
			const CEffectCode::SHADERVARIABLE* pConstantVariable,
			CEffectCode::CInstructionSetInterface& VertexShaderDefiner,
			CEffectCode::CInstructionSetInterface& FragmentShaderDefiner,
			CEffectCode::CPassDefiner::CEffectCodeUtils& EffectCodeUtils);

	private:
		CEffectCode::GLOBALVARIABLE								m_Constants[TOTAL_CONSTANTS];

		static const FLOAT3 sm_VIEW_CAMERA_DIR;
		static const CEffectCode::TEMPORARYDECLARATION			sm_TEMPRORAYS	   [TOTAL_TEMPORARY];
		static const CEffectCode::ATTRIBUTEVARIABLEDECLARATION	sm_ATTRIBUTEVARIABLE[TOTAL_ATTRIBUTE_VARIABLE];
		static const CEffectCode::ELEMENTPARAMETER				sm_VARYING[TOTAL_VARYING];
	};
}