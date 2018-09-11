/*
 *  AlchemyGrassAnimationEffect.h
 *  IOS
 *
 *  Created by lin fuqing on 12-5-28.
 *  Copyright 2012 AstepGame. All rights reserved.
 *
 */

#include "AlchemyEffectInfoInterface.h"

namespace alchemy
{
	class CGrassAnimationEffect :
	public EFFECTINFO
	{
	public:
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
			CONSTANT_OFFSET_DEF,
			
			TOTAL_CONSTANTS
		}CONSTANSINDEX;
		
		typedef enum
		{
			TEMPORARY_OFFSET = 0, 
			
			TOTAL_TEMPORARY
		}TEMPORARYINDEX;
		
		typedef enum
		{
			ATTRIBUTE_VARIABLE_POSITION = 0,
			
			ATTRIBUTE_VARIABLE_TEXCOORD,
			
			TOTAL_ATTRIBUTE_VARIABLES
		}ATTRIBUTEVARIABLEINDEX;
		
		typedef enum
		{
			TOTAL_CONSTANT_VARIABLES = 0
		}CONSTANTVARIABLEINDEX;
	public:
		CGrassAnimationEffect(const FLOAT* pfOffsetRef);
		~CGrassAnimationEffect(void);
		
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
		static const CEffectCode::TEMPORARYDECLARATION         sm_TEMPORARY         [TOTAL_TEMPORARY          ];
		static const CEffectCode::ATTRIBUTEVARIABLEDECLARATION sm_ATTRIBUTE_VARIABLE[TOTAL_ATTRIBUTE_VARIABLES];
		
		CEffectCode::GLOBALVARIABLE                            m_Constants          [TOTAL_CONSTANTS          ];
	};
}