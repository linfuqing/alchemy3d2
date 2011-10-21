#pragma once

#include "AlchemyEffectInfoInterface.h"

namespace alchemy
{
	class CAttributeWorldSpaceEffect :
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
			TOTAL_VARYING = 0
		}VARYINGINDEX;

		typedef enum
		{
			CONSTANT_WORLD_MATRIX = 0,

			TOTAL_CONSTANTS
		}CONSTANSINDEX;

		typedef enum
		{
			TOTAL_TEMPORARY = 0
		}TEMPORARYINDEX;

		typedef enum
		{
			ATTRIBUTE_VARIABLE = 0,

			TOTAL_ATTRIBUTE_VARIABLES
		}ATTRIBUTEVARIABLEINDEX;

		typedef enum
		{
			TOTAL_CONSTANT_VARIABLES = 0
		}CONSTANTVARIABLEINDEX;
	public:
		CAttributeWorldSpaceEffect(CVertexDeclaration::ELEMENTUSAGE Usage, bool bIsNoraml, UINT uIndex = 0);
		~CAttributeWorldSpaceEffect(void);

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
		static const CEffectCode::GLOBALVARIABLE   sm_CONSTANS        [TOTAL_CONSTANTS         ];
		CEffectCode::ATTRIBUTEVARIABLEDECLARATION  m_AttributeVariable[TOTAL_ATTRIBUTE_VARIABLES];
	};
}