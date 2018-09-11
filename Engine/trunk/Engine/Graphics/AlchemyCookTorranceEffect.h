/*
 *  AlchemyCookTorranceEffect.h
 *  IOS
 *
 *  Created by lin fuqing on 12-6-25.
 *  Copyright 2012 AstepGame. All rights reserved.
 *
 */

#pragma once

#include "AlchemyEffectInfoInterface.h"

namespace alchemy
{
	class CCookTorranceEffect :
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
			CONSTANT_ONE = 0,
			CONSTANT_FRESNEL,
			CONSTANT_ROUGHNESS_SQUARE, 
			
			TOTAL_CONSTANTS
		}CONSTANSINDEX;
		
		typedef enum
		{
			TEMPORARY_SPECULAR = 0,
			TEMPORARY_ROUGHNESS,
			TEMPORARY_GEOMETRIC,
			TEMPORARY_FRESNEL_CODE,
			
			TOTAL_TEMPORARY
		}TEMPORARYINDEX;
		
		typedef enum
		{
			ATTRIBUTE_VARIABLE_NORMAL = 0,
			
			ATTRIBUTE_VARIABLE_FACING,
			
			ATTRIBUTE_VARIABLE_LIGHTNESS, 
			
			TOTAL_ATTRIBUTE_VARIABLES
		}ATTRIBUTEVARIABLEINDEX;
		
		typedef enum
		{
			CONSTANT_VARIABLE_CAMERA_LOCAL_DIRECTION_NEGATE = 0,
			
			CONSTANT_VARIABLE_LIGHT_SPECULAR,
			
			CONSTANT_VARIABLE_LIGHT_LOCAL_DIRECTION_NEGATE,
			
			CONSTANT_VARIABLE_MATERIAL_POWER,
			
			TOTAL_CONSTANT_VARIABLES
		}CONSTANTVARIABLEINDEX;
	public:
		CCookTorranceEffect(UINT uLightIndex, const FLOAT* pfFresnel = ALCHEMY_NULL, const FLOAT* pfRoughnessSquare = ALCHEMY_NULL);
		~CCookTorranceEffect(void);
		
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
		static const CEffectCode::TEMPORARYDECLARATION sm_TEMPORARY   [TOTAL_TEMPORARY          ];

		CEffectCode::GLOBALVARIABLE               m_Constants         [TOTAL_CONSTANTS          ];
		CEffectCode::CONSTANTVARIABLEDECLARATION  m_ConstantVariables [TOTAL_CONSTANT_VARIABLES ];
		CEffectCode::ATTRIBUTEVARIABLEDECLARATION m_AttributeVariables[TOTAL_ATTRIBUTE_VARIABLES];
		
		FLOAT m_fFresnel;
		FLOAT m_fRoughnessSquare;
	};
}