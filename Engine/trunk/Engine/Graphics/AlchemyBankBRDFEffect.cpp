/*
 *  AlchemyBankBRDFEffect.cpp
 *  IOS
 *
 *  Created by lin fuqing on 12-6-20.
 *  Copyright 2012 AstepGame. All rights reserved.
 *
 */

#include "AlchemyBankBRDFEffect.h"

using namespace alchemy;

const CEffectCode::GLOBALVARIABLE       CBankBRDFEffect::sm_CONSTANTS[CBankBRDFEffect::TOTAL_CONSTANTS] = 
{
	{IEffect::FRAGMENT_SHADER, CEffectCode::CONSTANT_VARIABLE, CRenderMethod::CONSTANT_ONE, 1, 0, ALCHEMY_NULL}
};

const CEffectCode::TEMPORARYDECLARATION CBankBRDFEffect::sm_TEMPORARY[CBankBRDFEffect::TOTAL_TEMPORARY] = 
{
	{IEffect::FRAGMENT_SHADER, CEffectCode::VECTOR4_VARIABLE, 1}, 
	{IEffect::FRAGMENT_SHADER, CEffectCode::VECTOR3_VARIABLE, 1}, 
};

CBankBRDFEffect::CBankBRDFEffect(UINT uLightIndex)
{
	CEffectCode::LPATTRIBUTEVARIABLEDECLARATION pAttributeVariable;
	
	pAttributeVariable               = &m_AttributeVariables[ATTRIBUTE_VARIABLE_NORMAL];
	pAttributeVariable->ShaderType   = IEffect::FRAGMENT_SHADER;
	pAttributeVariable->ElementType  = CVertexDeclaration::FLOAT3;
	pAttributeVariable->ElementUsage = CVertexDeclaration::NORMAL;
	pAttributeVariable->uIndex       = 0;
	pAttributeVariable->bIsReadOnly  = true;
	pAttributeVariable->bIsForce     = true;

	pAttributeVariable               = &m_AttributeVariables[ATTRIBUTE_VARIABLE_LIGHTNESS];
	pAttributeVariable->ShaderType   = IEffect::FRAGMENT_SHADER;
	pAttributeVariable->ElementType  = CVertexDeclaration::FLOAT1;
	pAttributeVariable->ElementUsage = CVertexDeclaration::LIGHTNESS;
	pAttributeVariable->uIndex       = uLightIndex;
	pAttributeVariable->bIsReadOnly  = true;
	pAttributeVariable->bIsForce     = false;
	
	CEffectCode::LPCONSTANTVARIABLEDECLARATION pConstantVariable;
	
	pConstantVariable              = &m_ConstantVariables[CONSTANT_VARIABLE_CAMERA_LOCAL_DIRECTION_NEGATE];
	pConstantVariable->ShaderType  = IEffect::FRAGMENT_SHADER;
	pConstantVariable->GlobalType  = CEffectCode::CAMERA_VARIABLE;
	pConstantVariable->uDetail     = CRenderMethod::CAMERA_LOCAL_DIRECTION_NEGATE;
	pConstantVariable->uCount      = 1;
	pConstantVariable->uIndex      = 0;
	pConstantVariable->pValue      = ALCHEMY_NULL;
	pConstantVariable->bIsReadOnly = true;
	pConstantVariable->bIsForce    = true;
	
	pConstantVariable              = &m_ConstantVariables[CONSTANT_VARIABLE_LIGHT_SPECULAR];
	pConstantVariable->ShaderType  = IEffect::FRAGMENT_SHADER;
	pConstantVariable->GlobalType  = CEffectCode::LIGHT_VARIABLE;
	pConstantVariable->uDetail     = CRenderMethod::LIGHT_SPECULAR;
	pConstantVariable->uCount      = 1;
	pConstantVariable->uIndex      = uLightIndex;
	pConstantVariable->pValue      = ALCHEMY_NULL;
	pConstantVariable->bIsReadOnly = true;
	pConstantVariable->bIsForce    = true;
	
	pConstantVariable              = &m_ConstantVariables[CONSTANT_VARIABLE_LIGHT_LOCAL_DIRECTION_NEGATE];
	pConstantVariable->ShaderType  = IEffect::FRAGMENT_SHADER;
	pConstantVariable->GlobalType  = CEffectCode::LIGHT_VARIABLE;
	pConstantVariable->uDetail     = CRenderMethod::LIGHT_LOCAL_DIRECTION_NEGATE;
	pConstantVariable->uCount      = 1;
	pConstantVariable->uIndex      = uLightIndex;
	pConstantVariable->pValue      = ALCHEMY_NULL;
	pConstantVariable->bIsReadOnly = true;
	pConstantVariable->bIsForce    = true;
	
	pConstantVariable              = &m_ConstantVariables[CONSTANT_VARIABLE_MATERIAL_POWER];
	pConstantVariable->ShaderType  = IEffect::FRAGMENT_SHADER;
	pConstantVariable->GlobalType  = CEffectCode::MATERIAL_VARIABLE;
	pConstantVariable->uDetail     = CRenderMethod::MATERIAL_POWER;
	pConstantVariable->uCount      = 1;
	pConstantVariable->uIndex      = 0;
	pConstantVariable->pValue      = ALCHEMY_NULL;
	pConstantVariable->bIsReadOnly = true;
	pConstantVariable->bIsForce    = true;
}

CBankBRDFEffect::~CBankBRDFEffect(void)
{
}

UINT                                              CBankBRDFEffect::GetEffectInfoFlag    (                            )
{
	return ALCHEMY_FLAG(IEffectInfo::FRAGMENT_SHADER_OUTPUT);
}

const IEffectInfo::SAMPLER             * CBankBRDFEffect::GetSamplers         (UINT& uSamplerCount          )
{
	uSamplerCount = TOTAL_SAMPLERS;
	
	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION             * CBankBRDFEffect::GetAttributes       (UINT& uAttributeCount        )
{
	uAttributeCount = TOTAL_ATTRIBUTES;
	
	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION            * CBankBRDFEffect::GetVarying           (UINT& uVaryingCount          )
{
	uVaryingCount = TOTAL_VARYING;
	
	return ALCHEMY_NULL;
}

const CEffectCode::GLOBALVARIABLE              * CBankBRDFEffect::GetConstants         (UINT& uConstantCount         )
{
	uConstantCount = TOTAL_CONSTANTS;
	
	return sm_CONSTANTS;
}

const CEffectCode::TEMPORARYDECLARATION        * CBankBRDFEffect::GetTemporary         (UINT& uTemporaryCount        )
{
	uTemporaryCount = TOTAL_TEMPORARY;
	
	return sm_TEMPORARY;
}

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CBankBRDFEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
{
	uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLES;
	
	return m_AttributeVariables;
}

const CEffectCode::CONSTANTVARIABLEDECLARATION * CBankBRDFEffect::GetConstantVariables (UINT& uConstantVariableCount )
{
	uConstantVariableCount = TOTAL_CONSTANT_VARIABLES;
	
	return m_ConstantVariables;
}

bool CBankBRDFEffect::ShaderDefineHandler(
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
	UINT uTangent = puTemporary[TEMPORARY_TANGENT], uNoraml = pAttributeVariables[ATTRIBUTE_VARIABLE_NORMAL].uHandle, uCameraLocalDirectionNegate = pConstantVariables[CONSTANT_VARIABLE_CAMERA_LOCAL_DIRECTION_NEGATE].uHandle;
	UINT uAddress[] = {0, 1, 2, 3};
	FragmentShaderDefiner.AppendInstruction(ICompiler::CRS, uTangent, uAddress, 3, uNoraml, uAddress, 3, uCameraLocalDirectionNegate, uAddress, 3);
	
	FragmentShaderDefiner.AppendInstruction(ICompiler::NRM, uTangent, uTangent, 0);
	
	UINT uSpecular = puTemporary[TEMPORARY_SPECULAR], uLightLocalDirectionNegate = pConstantVariables[CONSTANT_VARIABLE_LIGHT_LOCAL_DIRECTION_NEGATE].uHandle;
	FragmentShaderDefiner.AppendInstruction(ICompiler::DP3, uSpecular   , uAddress    , 1, uLightLocalDirectionNegate , uAddress    , 3, uTangent                                                    , uAddress    , 3);
	FragmentShaderDefiner.AppendInstruction(ICompiler::DP3, uSpecular   , &uAddress[1], 1, uCameraLocalDirectionNegate, uAddress    , 3, uTangent                                                    , uAddress    , 3);
	
	FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uSpecular   , &uAddress[2], 1, uSpecular                  , uAddress    , 1, uSpecular                                                   , uAddress    , 1);
	FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uSpecular   , &uAddress[3], 1, uSpecular                  , &uAddress[1], 1, uSpecular                                                   , &uAddress[1], 1);
	
	UINT uOne = puConstants[CONSTANT_ONE];
	FragmentShaderDefiner.AppendInstruction(ICompiler::SUB, uSpecular   , &uAddress[2], 1, uOne                       , uAddress    , 1, uSpecular                                                   , &uAddress[2], 1);
	FragmentShaderDefiner.AppendInstruction(ICompiler::SUB, uSpecular   , &uAddress[3], 1, uOne                       , uAddress    , 1, uSpecular                                                   , &uAddress[3], 1);
	
	FragmentShaderDefiner.AppendInstruction(ICompiler::SQT, uSpecular   , &uAddress[2], 1, uSpecular                  , &uAddress[2], 1, 0                                                           , ALCHEMY_NULL, 0);
	FragmentShaderDefiner.AppendInstruction(ICompiler::SQT, uSpecular   , &uAddress[3], 1, uSpecular                  , &uAddress[3], 1, 0                                                           , ALCHEMY_NULL, 0);
	
	FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uSpecular   , uAddress    , 1, uSpecular                  , uAddress    , 1, uSpecular                                                   , &uAddress[1], 1);
	FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uSpecular   , &uAddress[1], 1, uSpecular                  , &uAddress[2], 1, uSpecular                                                   , &uAddress[3], 1);
	
	FragmentShaderDefiner.AppendInstruction(ICompiler::SUB, uSpecular   , uAddress    , 1, uSpecular                  , &uAddress[1], 1, uSpecular                                                   , uAddress    , 1);
	
	FragmentShaderDefiner.AppendInstruction(ICompiler::POW, uSpecular   , uAddress    , 1, uSpecular                  , uAddress    , 1, pConstantVariables[CONSTANT_VARIABLE_MATERIAL_POWER].uHandle, uAddress    , 1);
	
	UINT uLightness = pAttributeVariables[ATTRIBUTE_VARIABLE_LIGHTNESS].uHandle;
	if(uLightness)
		FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uSpecular, uAddress    , 1, uSpecular                 , uAddress    , 1, uLightness, uAddress    , 1);
	else
	{
		FragmentShaderDefiner.AppendInstruction(ICompiler::DP3, uSpecular, &uAddress[1], 1, uLightLocalDirectionNegate, uAddress    , 3, uNoraml   , uAddress    , 3);
		
		FragmentShaderDefiner.AppendInstruction(ICompiler::SAT, uSpecular, &uAddress[1], 1, uSpecular                 , &uAddress[1], 1, 0         , ALCHEMY_NULL, 0);
		
		FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uSpecular, uAddress    , 1, uSpecular                 , uAddress    , 1, uSpecular , &uAddress[1], 1);
	}

	UINT uLightSpecular = pConstantVariables[CONSTANT_VARIABLE_LIGHT_SPECULAR].uHandle;
	FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uOutputColor, uAddress    , 3, uSpecular     , uAddress    , 1, uLightSpecular, uAddress    , 3);
	FragmentShaderDefiner.AppendInstruction(ICompiler::MOV, uOutputColor, &uAddress[3], 1, uLightSpecular, &uAddress[3], 1, 0             , ALCHEMY_NULL, 0);
	
	return true;
}