#pragma once
/*
 *  AlchemyLightDirectionEffect.h
 *  IOS
 *
 *  Created by lin fuqing on 12-6-7.
 *  Copyright 2012 AstepGame. All rights reserved.
 *
 */

#include "AlchemyEffectInfoInterface.h"

namespace alchemy
{
	class CLightDirectionEffect :
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
			TOTAL_VARYING = 0
		}VARYINGINDEX;
		
		typedef enum
		{
			CONSTANTS_LIGHT_LOCAL_POSITION = 0,
			
			TOTAL_CONSTANTS
		}CONSTANSINDEX;
		
		typedef enum
		{
			TOTAL_TEMPORARY = 0
		}TEMPORARYINDEX;
		
		typedef enum
		{
			ATTRIBUTE_VARIABLE_POSITION = 0,
			
			TOTAL_ATTRIBUTE_VARIABLES
		}ATTRIBUTEVARIABLEINDEX;
		
		typedef enum
		{
			CONSTANT_VARIABLE_LIGHT_LOCAL_DIRECTION = 0,
			
			CONSTANT_VARIABLE_LIGHT_LOCAL_DIRECTION_NEGATE,
			
			TOTAL_CONSTANT_VARIABLES
		}CONSTANTVARIABLEINDEX;
	public:
		CLightDirectionEffect(UINT uLightIndex, IEffect::EFFECTTYPE EffectType = IEffect::VERTEX_SHADER);
		~CLightDirectionEffect(void);
		
	private:
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
		CEffectCode::GLOBALVARIABLE               m_Constants         [TOTAL_CONSTANTS          ];
		CEffectCode::ATTRIBUTEVARIABLEDECLARATION m_AttributeVariables[TOTAL_ATTRIBUTE_VARIABLES];
		CEffectCode::CONSTANTVARIABLEDECLARATION  m_ConstantVariables [TOTAL_CONSTANT_VARIABLES ];
		
		IEffect::EFFECTTYPE m_EffectType;
	};
}