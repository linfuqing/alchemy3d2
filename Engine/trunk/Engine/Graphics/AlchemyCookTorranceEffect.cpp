/*
 *  AlchemyCookTorranceEffect.cpp
 *  IOS
 *
 *  Created by lin fuqing on 12-6-25.
 *  Copyright 2012 AstepGame. All rights reserved.
 *
 */

#include "AlchemyCookTorranceEffect.h"

using namespace alchemy;

const CEffectCode::TEMPORARYDECLARATION CCookTorranceEffect::sm_TEMPORARY[CCookTorranceEffect::TOTAL_TEMPORARY] = 
{
	{IEffect::FRAGMENT_SHADER, CEffectCode::VECTOR3_VARIABLE, 1}, 
	{IEffect::FRAGMENT_SHADER, CEffectCode::VECTOR1_VARIABLE, 1}, 
	{IEffect::FRAGMENT_SHADER, CEffectCode::VECTOR1_VARIABLE, 1}, 
	{IEffect::FRAGMENT_SHADER, CEffectCode::VECTOR1_VARIABLE, 1}
};

CCookTorranceEffect::CCookTorranceEffect(UINT uLightIndex, const FLOAT* pfFresnel, const FLOAT* pfRoughnessSquare) :
m_fFresnel(0.8f), 
m_fRoughnessSquare(0.1f)
{
	CEffectCode::LPGLOBALVARIABLE pConstant;
	pConstant = &m_Constants[CONSTANT_ONE];
	pConstant->ShaderType  = IEffect::FRAGMENT_SHADER;
	pConstant->GlobalType  = CEffectCode::CONSTANT_VARIABLE;
	pConstant->uDetail     = CRenderMethod::CONSTANT_ONE;
	pConstant->uCount      = 1;
	pConstant->uIndex      = 0;
	pConstant->pValue      = ALCHEMY_NULL;
	
	pfFresnel = pfFresnel ? pfFresnel : &m_fFresnel;
	pConstant = &m_Constants[CONSTANT_FRESNEL];
	pConstant->ShaderType  = IEffect::FRAGMENT_SHADER;
	pConstant->GlobalType  = CEffectCode::INPUT_VARIABLE;
	pConstant->uDetail     = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR1_VARIABLE, 1, 1, false);
	pConstant->uCount      = 1;
	pConstant->uIndex      = 0;
	pConstant->pValue      = pfFresnel;
	
	pfRoughnessSquare = pfRoughnessSquare ? pfRoughnessSquare : &m_fRoughnessSquare;
	pConstant = &m_Constants[CONSTANT_ROUGHNESS_SQUARE];
	pConstant->ShaderType  = IEffect::FRAGMENT_SHADER;
	pConstant->GlobalType  = CEffectCode::INPUT_VARIABLE;
	pConstant->uDetail     = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR1_VARIABLE, 1, 1, false);
	pConstant->uCount      = 1;
	pConstant->uIndex      = 0;
	pConstant->pValue      = pfRoughnessSquare;
	
	CEffectCode::LPATTRIBUTEVARIABLEDECLARATION pAttributeVariable;
	
	pAttributeVariable               = &m_AttributeVariables[ATTRIBUTE_VARIABLE_NORMAL];
	pAttributeVariable->ShaderType   = IEffect::FRAGMENT_SHADER;
	pAttributeVariable->ElementType  = CVertexDeclaration::FLOAT3;
	pAttributeVariable->ElementUsage = CVertexDeclaration::NORMAL;
	pAttributeVariable->uIndex       = 0;
	pAttributeVariable->bIsReadOnly  = true;
	pAttributeVariable->bIsForce     = true;
	
	pAttributeVariable               = &m_AttributeVariables[ATTRIBUTE_VARIABLE_FACING];
	pAttributeVariable->ShaderType   = IEffect::FRAGMENT_SHADER;
	pAttributeVariable->ElementType  = CVertexDeclaration::FLOAT1;
	pAttributeVariable->ElementUsage = CVertexDeclaration::FACING;
	pAttributeVariable->uIndex       = 0;
	pAttributeVariable->bIsReadOnly  = true;
	pAttributeVariable->bIsForce     = false;

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

CCookTorranceEffect::~CCookTorranceEffect(void)
{
}

UINT                                             CCookTorranceEffect::GetEffectInfoFlag    (                            )
{
	return ALCHEMY_FLAG(IEffectInfo::FRAGMENT_SHADER_OUTPUT);
}

const IEffectInfo::SAMPLER                     * CCookTorranceEffect::GetSamplers         (UINT& uSamplerCount          )
{
	uSamplerCount = TOTAL_SAMPLERS;
	
	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION          * CCookTorranceEffect::GetAttributes       (UINT& uAttributeCount        )
{
	uAttributeCount = TOTAL_ATTRIBUTES;
	
	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION          * CCookTorranceEffect::GetVarying           (UINT& uVaryingCount          )
{
	uVaryingCount = TOTAL_VARYING;
	
	return ALCHEMY_NULL;
}

const CEffectCode::GLOBALVARIABLE              * CCookTorranceEffect::GetConstants         (UINT& uConstantCount         )
{
	uConstantCount = TOTAL_CONSTANTS;
	
	return m_Constants;
}

const CEffectCode::TEMPORARYDECLARATION        * CCookTorranceEffect::GetTemporary         (UINT& uTemporaryCount        )
{
	uTemporaryCount = TOTAL_TEMPORARY;
	
	return sm_TEMPORARY;
}

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CCookTorranceEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
{
	uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLES;
	
	return m_AttributeVariables;
}

const CEffectCode::CONSTANTVARIABLEDECLARATION * CCookTorranceEffect::GetConstantVariables (UINT& uConstantVariableCount )
{
	uConstantVariableCount = TOTAL_CONSTANT_VARIABLES;
	
	return m_ConstantVariables;
}

bool CCookTorranceEffect::ShaderDefineHandler(
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
	UINT uSpecular = puTemporary[TEMPORARY_SPECULAR], uLightDirectionNegate = pConstantVariables[CONSTANT_VARIABLE_LIGHT_LOCAL_DIRECTION_NEGATE].uHandle, uCameraDirectionNegate = pConstantVariables[CONSTANT_VARIABLE_CAMERA_LOCAL_DIRECTION_NEGATE].uHandle;
	
	FragmentShaderDefiner.AppendInstruction(ICompiler::ADD, uSpecular, uLightDirectionNegate, uCameraDirectionNegate);
	FragmentShaderDefiner.AppendInstruction(ICompiler::NRM, uSpecular, uSpecular            , 0                     );
	
	UINT uRoughness = puTemporary[TEMPORARY_ROUGHNESS], uNormal = pAttributeVariables[ATTRIBUTE_VARIABLE_NORMAL].uHandle;
	FragmentShaderDefiner.AppendInstruction(ICompiler::DP3, uRoughness , uNormal               , uSpecular);
	UINT uFresnelCoe = puTemporary[TEMPORARY_FRESNEL_CODE];
	FragmentShaderDefiner.AppendInstruction(ICompiler::DP3, uFresnelCoe, uCameraDirectionNegate, uSpecular);
	UINT uAddress[] = {0, 1, 2};
	
	UINT uFacing = pAttributeVariables[ATTRIBUTE_VARIABLE_FACING].uHandle, uFacingAddress = 0;
	if(!uFacing)
	{
		uFacing        = uSpecular;
		uFacingAddress = 1;
		
		FragmentShaderDefiner.AppendInstruction(ICompiler::DP3, uSpecular, &uFacingAddress, 1, uNormal  , uAddress       , 3, uCameraDirectionNegate, uAddress    , 3);
		FragmentShaderDefiner.AppendInstruction(ICompiler::SAT, uSpecular, &uFacingAddress, 1, uSpecular, &uFacingAddress, 1, 0                     , ALCHEMY_NULL, 0);
	}
	
	UINT uGeometric = puTemporary[TEMPORARY_GEOMETRIC];
	FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uGeometric, uAddress, 1, uRoughness, uAddress, 1, uFacing, &uFacingAddress, 1);
	
	FragmentShaderDefiner.AppendInstruction(ICompiler::ADD, uGeometric, uGeometric, uGeometric );
	FragmentShaderDefiner.AppendInstruction(ICompiler::DIV, uGeometric, uGeometric, uFresnelCoe);
	
	UINT uLightness = pAttributeVariables[ATTRIBUTE_VARIABLE_LIGHTNESS].uHandle, uLightnessAddress = 0;
	if(!uLightness)
	{
		uLightness        = uSpecular;
		uLightnessAddress = 2;
		
		FragmentShaderDefiner.AppendInstruction(ICompiler::DP3, uSpecular, &uLightnessAddress, 1, uNormal  , uAddress          , 3, uLightDirectionNegate, uAddress    , 3);
		FragmentShaderDefiner.AppendInstruction(ICompiler::SAT, uSpecular, &uLightnessAddress, 1, uSpecular, &uLightnessAddress, 1, 0                    , ALCHEMY_NULL, 0);
	}
	
	FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uSpecular , uAddress, 1, uRoughness, uAddress, 1, uLightness , &uLightnessAddress, 1);
	FragmentShaderDefiner.AppendInstruction(ICompiler::ADD, uSpecular , uAddress, 1, uSpecular , uAddress, 1, uSpecular  , uAddress          , 1);
	FragmentShaderDefiner.AppendInstruction(ICompiler::DIV, uSpecular , uAddress, 1, uSpecular , uAddress, 1, uFresnelCoe, uAddress          , 1);
	
	FragmentShaderDefiner.AppendInstruction(ICompiler::MIN, uGeometric, uAddress, 1, uGeometric, uAddress, 1, uSpecular  , uAddress          , 1);
	
	UINT uOne = puConstants[CONSTANT_ONE];
	FragmentShaderDefiner.AppendInstruction(ICompiler::MIN, uGeometric, uGeometric, uOne);
	//End Geometric
	
	FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uGeometric, uAddress, 1, uGeometric, uAddress       , 1, uLightness, &uLightnessAddress, 1);
	
	FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uSpecular , uAddress, 1, uFacing   , &uFacingAddress, 1, uLightness, &uLightnessAddress, 1);
	
	FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uRoughness, uRoughness, uRoughness);
	
	FragmentShaderDefiner.AppendInstruction(ICompiler::SUB, uSpecular , &uAddress[1], 1, uRoughness, uAddress    , 1, uOne            , uAddress    , 1);
	UINT uRoughnessSquare = puConstants[CONSTANT_ROUGHNESS_SQUARE];
	FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uSpecular , &uAddress[2], 1, uRoughness, uAddress    , 1, uRoughnessSquare, uAddress    , 1);
	FragmentShaderDefiner.AppendInstruction(ICompiler::DIV, uSpecular , &uAddress[1], 1, uSpecular , &uAddress[1], 1, uSpecular       , &uAddress[2], 1);
	FragmentShaderDefiner.AppendInstruction(ICompiler::EXP, uSpecular , &uAddress[1], 1, uSpecular , &uAddress[1], 1, 0               , ALCHEMY_NULL, 0);
	FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uSpecular , &uAddress[2], 1, uSpecular , &uAddress[2], 1, uRoughness      , uAddress    , 1);
	FragmentShaderDefiner.AppendInstruction(ICompiler::DIV, uRoughness, uAddress    , 1, uSpecular , &uAddress[1], 1, uSpecular       , &uAddress[2], 1);
	//End Roughness
	
	FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uGeometric, uGeometric, uRoughness);
	
	FragmentShaderDefiner.AppendInstruction(ICompiler::SUB, uFresnelCoe, uOne, uFresnelCoe);
	FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uSpecular  , &uAddress[1], 1, uFresnelCoe, uAddress    , 1, uFresnelCoe, uAddress    , 1);
	FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uSpecular  , &uAddress[1], 1, uSpecular  , &uAddress[1], 1, uSpecular  , &uAddress[1], 1);
	FragmentShaderDefiner.AppendInstruction(ICompiler::ADD, uSpecular  , &uAddress[1], 1, uSpecular  , &uAddress[1], 1, uFresnelCoe, uAddress    , 1);
	UINT uFresnal = puConstants[CONSTANT_FRESNEL];
	FragmentShaderDefiner.AppendInstruction(ICompiler::SUB, uSpecular  , &uAddress[2], 1, uOne       , uAddress    , 1, uFresnal   , uAddress    , 1);
	FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uSpecular  , &uAddress[1], 1, uSpecular  , &uAddress[1], 1, uSpecular  , &uAddress[2], 1);
	FragmentShaderDefiner.AppendInstruction(ICompiler::ADD, uFresnelCoe, uAddress    , 1, uFresnal   , uAddress    , 1, uSpecular  , &uAddress[1], 1);
	//End Fresnel Code
	
	FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uGeometric, uGeometric, uFresnelCoe);
	
	FragmentShaderDefiner.AppendInstruction(ICompiler::DIV, uGeometric  , uAddress, 1, uGeometric    , uAddress, 1, uSpecular     , uAddress    , 1);

	UINT uLightSpecular = pConstantVariables[CONSTANT_VARIABLE_LIGHT_SPECULAR].uHandle;
	FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uOutputColor, uAddress, 3, uGeometric    , uAddress, 1, uLightSpecular, uAddress    , 3);
	UINT a = 3;
	FragmentShaderDefiner.AppendInstruction(ICompiler::MOV, uOutputColor, &a      , 1, uLightSpecular, &a      , 1, 0             , ALCHEMY_NULL, 0);
	
	return true;
}