/*
 *  AlchemyGrassAnimationEffect.cpp
 *  IOS
 *
 *  Created by lin fuqing on 12-5-28.
 *  Copyright 2012 AstepGame. All rights reserved.
 *
 */

#include "AlchemyGrassAnimationEffect.h"

using namespace alchemy;

const CEffectCode::TEMPORARYDECLARATION           CGrassAnimationEffect::sm_TEMPORARY        [CGrassAnimationEffect::TOTAL_TEMPORARY           ] = 
{
	{IEffect::VERTEX_SHADER, CEffectCode::VECTOR1_VARIABLE, 1}
};

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION CGrassAnimationEffect::sm_ATTRIBUTE_VARIABLE[CGrassAnimationEffect::TOTAL_ATTRIBUTE_VARIABLES] = 
{ 
	{IEffect::VERTEX_SHADER, CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION, 0, false, true}, 
	{IEffect::VERTEX_SHADER, CVertexDeclaration::FLOAT2, CVertexDeclaration::TEXCOORD, 0, true , true}
};

CGrassAnimationEffect::CGrassAnimationEffect(const FLOAT* pfOffsetRef)
{
	CEffectCode::LPGLOBALVARIABLE pConstant;
	
	pConstant = &m_Constants[CONSTANT_ONE];
	pConstant->ShaderType = IEffect::VERTEX_SHADER;
	pConstant->GlobalType = CEffectCode::CONSTANT_VARIABLE;
	pConstant->uDetail    = CRenderMethod::CONSTANT_ONE;
	pConstant->uCount     = 1;
	pConstant->uIndex     = 0;
	pConstant->pValue     = ALCHEMY_NULL;
	
	pConstant = &m_Constants[CONSTANT_OFFSET_DEF];
	pConstant->ShaderType = IEffect::VERTEX_SHADER;
	pConstant->GlobalType = CEffectCode::INPUT_VARIABLE;
	pConstant->uDetail    = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR1_VARIABLE, 1, 1, false);
	pConstant->uCount     = 1;
	pConstant->uIndex     = 0;
	pConstant->pValue     = pfOffsetRef;
}

CGrassAnimationEffect::~CGrassAnimationEffect(void)
{
}

UINT                                            CGrassAnimationEffect::GetEffectInfoFlag     (                             )
{
	return ALCHEMY_FLAG(IEffectInfo::VERTEX_SHADER_INFO);
}

const IEffectInfo::SAMPLER           * CGrassAnimationEffect::GetSamplers           (UINT& uSamplerCount          )
{
	uSamplerCount = TOTAL_SAMPLERS;
	
	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION            * CGrassAnimationEffect::GetAttributes        (UINT& uAttributeCount        )
{
	uAttributeCount = TOTAL_ATTRIBUTES;
	
	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION            * CGrassAnimationEffect::GetVarying           (UINT& uVaryingCount          )
{
	uVaryingCount = TOTAL_VARYING;
	
	return ALCHEMY_NULL;
}

const CEffectCode::GLOBALVARIABLE              * CGrassAnimationEffect::GetConstants         (UINT& uConstantCount         )
{
	uConstantCount = TOTAL_CONSTANTS;
	
	return m_Constants;
}

const CEffectCode::TEMPORARYDECLARATION        * CGrassAnimationEffect::GetTemporary         (UINT& uTemporaryCount        )
{
	uTemporaryCount = TOTAL_TEMPORARY;
	
	return sm_TEMPORARY;
}

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CGrassAnimationEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
{
	uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLES;
	
	return sm_ATTRIBUTE_VARIABLE;
}

const CEffectCode::CONSTANTVARIABLEDECLARATION * CGrassAnimationEffect::GetConstantVariables  (UINT& uConstantVariableCount)
{
	uConstantVariableCount = TOTAL_CONSTANT_VARIABLES;
	
	return ALCHEMY_NULL;
}

bool CGrassAnimationEffect::ShaderDefineHandler(
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
	const CEffectCode::SHADERVARIABLE* pPosition = &pAttributeVariables[ATTRIBUTE_VARIABLE_POSITION];
	
	if(pPosition->uHandle)
	{
		UINT uOffset = puTemporary[TEMPORARY_OFFSET], uTexcoord = pAttributeVariables[ATTRIBUTE_VARIABLE_TEXCOORD].uHandle, uOffsetRef = puConstants[CONSTANT_OFFSET_DEF], x = 0, v = 1;
		VertexShaderDefiner.AppendInstruction(ICompiler::SUB, uOffset, &x, 1, puConstants[CONSTANT_ONE], &x, 1, uTexcoord, &v, 1);
		VertexShaderDefiner.AppendInstruction(ICompiler::MUL, uOffset, uOffset, uOffsetRef);
		
		if(!pPosition->bIsUsed)
			VertexShaderDefiner.AppendInstruction(ICompiler::MOV, pPosition->uHandle, pPosition->uSource, 0);
		
		VertexShaderDefiner.AppendInstruction(ICompiler::ADD, 
											  pPosition->uHandle, 
											  &x,
											  1,
											  pPosition->uHandle, 
											  &x,
											  1, 
											  uOffset, 
											  &x,
											  1);
	}
	
	return true;
}