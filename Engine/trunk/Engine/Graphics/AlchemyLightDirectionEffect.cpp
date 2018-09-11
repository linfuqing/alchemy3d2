/*
 *  AlchemyLightDirectionEffect.cpp
 *  IOS
 *
 *  Created by lin fuqing on 12-6-7.
 *  Copyright 2012 AstepGame. All rights reserved.
 *
 */

#include "AlchemyLightDirectionEffect.h"

using namespace alchemy;

CLightDirectionEffect::CLightDirectionEffect(UINT uLightIndex, IEffect::EFFECTTYPE EffectType) :
m_EffectType(EffectType)
{
	CEffectCode::LPGLOBALVARIABLE pConstants = &m_Constants[CONSTANTS_LIGHT_LOCAL_POSITION];
	pConstants->ShaderType = EffectType;
	pConstants->GlobalType = CEffectCode::LIGHT_VARIABLE;
	pConstants->uDetail = CRenderMethod::LIGHT_LOCAL_POSITION;
	pConstants->uCount = 1;
	pConstants->uIndex = uLightIndex;
	pConstants->pValue = ALCHEMY_NULL;
	
	CEffectCode::LPATTRIBUTEVARIABLEDECLARATION pAttributeVariable = &m_AttributeVariables[ATTRIBUTE_VARIABLE_POSITION];
	pAttributeVariable->ShaderType   = EffectType;
	pAttributeVariable->ElementType  = CVertexDeclaration::FLOAT4;
	pAttributeVariable->ElementUsage = CVertexDeclaration::POSITION;
	pAttributeVariable->uIndex       = 0;
	pAttributeVariable->bIsReadOnly  = true;
	pAttributeVariable->bIsForce     = true;
	
	CEffectCode::LPCONSTANTVARIABLEDECLARATION pConstantVariable;
	pConstantVariable = &m_ConstantVariables[CONSTANT_VARIABLE_LIGHT_LOCAL_DIRECTION];
	pConstantVariable->ShaderType  = EffectType;
	pConstantVariable->GlobalType  = CEffectCode::LIGHT_VARIABLE;
	pConstantVariable->uDetail     = CRenderMethod::LIGHT_LOCAL_DIRECTION;
	pConstantVariable->uCount      = 1;
	pConstantVariable->uIndex      = uLightIndex;
	pConstantVariable->pValue      = ALCHEMY_NULL;
	pConstantVariable->bIsReadOnly = false;
	pConstantVariable->bIsForce    = false;
	
	pConstantVariable = &m_ConstantVariables[CONSTANT_VARIABLE_LIGHT_LOCAL_DIRECTION_NEGATE];
	pConstantVariable->ShaderType  = EffectType;
	pConstantVariable->GlobalType  = CEffectCode::LIGHT_VARIABLE;
	pConstantVariable->uDetail     = CRenderMethod::LIGHT_LOCAL_DIRECTION_NEGATE;
	pConstantVariable->uCount      = 1;
	pConstantVariable->uIndex      = uLightIndex;
	pConstantVariable->pValue      = ALCHEMY_NULL;
	pConstantVariable->bIsReadOnly = false;
	pConstantVariable->bIsForce    = false;
}

CLightDirectionEffect::~CLightDirectionEffect(void)
{
}

UINT                                              CLightDirectionEffect::GetEffectInfoFlag    (                            )
{
	return m_EffectType == IEffect::VERTEX_SHADER ? ALCHEMY_FLAG(IEffectInfo::VERTEX_SHADER_INFO) : (m_EffectType == IEffect::FRAGMENT_SHADER ? ALCHEMY_FLAG(IEffectInfo::FRAGMENT_SHADER_INFO) : 0);
}

const IEffectInfo::SAMPLER             * CLightDirectionEffect::GetSamplers         (UINT& uSamplerCount          )
{
	uSamplerCount = TOTAL_SAMPLERS;
	
	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION             * CLightDirectionEffect::GetAttributes       (UINT& uAttributeCount        )
{
	uAttributeCount = TOTAL_ATTRIBUTES;
	
	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION            * CLightDirectionEffect::GetVarying           (UINT& uVaryingCount          )
{
	uVaryingCount = TOTAL_VARYING;
	
	return ALCHEMY_NULL;
}

const CEffectCode::GLOBALVARIABLE              * CLightDirectionEffect::GetConstants         (UINT& uConstantCount         )
{
	uConstantCount = TOTAL_CONSTANTS;
	
	return m_Constants;
}

const CEffectCode::TEMPORARYDECLARATION        * CLightDirectionEffect::GetTemporary         (UINT& uTemporaryCount        )
{
	uTemporaryCount = TOTAL_TEMPORARY;
	
	return ALCHEMY_NULL;
}

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CLightDirectionEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
{
	uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLES;
	
	return m_AttributeVariables;
}

const CEffectCode::CONSTANTVARIABLEDECLARATION * CLightDirectionEffect::GetConstantVariables (UINT& uConstantVariableCount )
{
	uConstantVariableCount = TOTAL_CONSTANT_VARIABLES;
	
	return m_ConstantVariables;
}

bool CLightDirectionEffect::ShaderDefineHandler(
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
												 CEffectCode::CPassDefiner::CEffectCodeUtils& EffectCodeUtils)
{
	CEffectCode::IInstructionSet* pInstructionSet = ALCHEMY_NULL;;
	if(m_EffectType == IEffect::VERTEX_SHADER)
		pInstructionSet = &VertexShaderDefiner;
	else if(m_EffectType == IEffect::FRAGMENT_SHADER)
		pInstructionSet = &FragmentShaderDefiner;
	
	if(pInstructionSet)
	{
		UINT uPosition = pAttributeVariables[ATTRIBUTE_VARIABLE_POSITION].uHandle, uLightPosition = puConstants[CONSTANTS_LIGHT_LOCAL_POSITION];
		
		const CEffectCode::SHADERVARIABLE* pLocalDirection = &pConstantVariables[CONSTANT_VARIABLE_LIGHT_LOCAL_DIRECTION];
		if(pLocalDirection->uHandle && !pLocalDirection->bIsUsed)
		{
			UINT uAddrees[] = {0, 1, 2};
			
			pInstructionSet->AppendInstruction(ICompiler::SUB, pLocalDirection->uHandle, uAddrees, 3, uPosition, uAddrees, 3, uLightPosition, uAddrees, 3);
			pInstructionSet->AppendInstruction(ICompiler::NRM, pLocalDirection->uHandle, pLocalDirection->uHandle, 0);
		}
		
		const CEffectCode::SHADERVARIABLE* pLocalDirectionNegate = &pConstantVariables[CONSTANT_VARIABLE_LIGHT_LOCAL_DIRECTION_NEGATE];
		if(pLocalDirectionNegate->uHandle && !pLocalDirectionNegate->bIsUsed)
		{
			UINT uAddrees[] = {0, 1, 2};
			pInstructionSet->AppendInstruction(ICompiler::SUB, pLocalDirectionNegate->uHandle, uAddrees, 3, uLightPosition, uAddrees, 3, uPosition, uAddrees, 3);
			
			pInstructionSet->AppendInstruction(ICompiler::NRM, pLocalDirectionNegate->uHandle, pLocalDirectionNegate->uHandle, 0);
		}
		
		return true;
	}
	
	return false;
}
