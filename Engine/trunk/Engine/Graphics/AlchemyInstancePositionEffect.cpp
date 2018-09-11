/*
 *  AlchemyInstancePositionEffect.cpp
 *  IOS
 *
 *  Created by lin fuqing on 12-5-17.
 *  Copyright 2012 AstepGame. All rights reserved.
 *
 */

#include "AlchemyInstancePositionEffect.h"
//#include "AlchemyRenderMethod.h"

using namespace alchemy;

const CEffectCode::TEMPORARYDECLARATION           CInstancePositionEffect::sm_TEMPORARY        [CInstancePositionEffect::TOTAL_TEMPORARY           ] = 
{
	{IEffect::VERTEX_SHADER, CEffectCode::VECTOR1_VARIABLE, 1}
};

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION CInstancePositionEffect::sm_ATTRIBUTE_VARIABLE[CInstancePositionEffect::TOTAL_ATTRIBUTE_VARIABLES] = 
{ 
	{IEffect::VERTEX_SHADER, CVertexDeclaration::FLOAT1, CVertexDeclaration::INSTANCEINDEX, 0, true, true}, 
	{IEffect::VERTEX_SHADER, CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION, 0, false, true}
};

CInstancePositionEffect::CInstancePositionEffect(UINT uInstanceCount, const MATRIX4X4* pMatrices)
{
	CEffectCode::LPGLOBALVARIABLE pConstant;
	
	pConstant = &m_Constants[CONSTANT_MATRICES];
	pConstant->ShaderType = IEffect::VERTEX_SHADER;
	pConstant->GlobalType = CEffectCode::ARRAY_VARIABLE;
	pConstant->uDetail    = IRenderMethod::INSTANCE_TRANSFORM_ARRAY;
	pConstant->uCount     = uInstanceCount * 3;
	pConstant->uIndex     = 0;
	pConstant->pValue     = pMatrices;
	
	pConstant = &m_Constants[CONSTANT_MATRIX4X4_SIZE];
	pConstant->ShaderType = IEffect::VERTEX_SHADER;
	pConstant->GlobalType = CEffectCode::INPUT_VARIABLE;
	pConstant->uDetail    = CEffectCode::MATRIX4X3_SIZE;
	pConstant->uCount     = 1;
	pConstant->uIndex     = 0;
	pConstant->pValue     = ALCHEMY_NULL;
}

CInstancePositionEffect::CInstancePositionEffect(const MATRIX4X4** ppMatrices, UINT uInstanceCount)
{
	CEffectCode::LPGLOBALVARIABLE pConstant;
	
	pConstant = &m_Constants[CONSTANT_MATRICES];
	pConstant->ShaderType = IEffect::VERTEX_SHADER;
	pConstant->GlobalType = CEffectCode::INPUT_VARIABLE;
	pConstant->uDetail    = CEffectCode::BuildGlobalInputCode(CEffectCode::MATRIX4X3_VARIABLE, 4, 12, true);
	pConstant->uCount     = uInstanceCount;
	pConstant->uIndex     = 0;
	pConstant->pValue     = ppMatrices;
	
	pConstant = &m_Constants[CONSTANT_MATRIX4X4_SIZE];
	pConstant->ShaderType = IEffect::VERTEX_SHADER;
	pConstant->GlobalType = CEffectCode::INPUT_VARIABLE;
	pConstant->uDetail    = CEffectCode::MATRIX4X3_SIZE;
	pConstant->uCount     = 1;
	pConstant->uIndex     = 0;
	pConstant->pValue     = ALCHEMY_NULL;
}

CInstancePositionEffect::~CInstancePositionEffect(void)
{
}

UINT                                            CInstancePositionEffect::GetEffectInfoFlag     (                             )
{
	return ALCHEMY_FLAG(IEffectInfo::VERTEX_SHADER_INFO);
}

const IEffectInfo::SAMPLER           * CInstancePositionEffect::GetSamplers           (UINT& uSamplerCount          )
{
	uSamplerCount = TOTAL_SAMPLERS;
	
	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION            * CInstancePositionEffect::GetAttributes        (UINT& uAttributeCount        )
{
	uAttributeCount = TOTAL_ATTRIBUTES;
	
	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION            * CInstancePositionEffect::GetVarying           (UINT& uVaryingCount          )
{
	uVaryingCount = TOTAL_VARYING;
	
	return ALCHEMY_NULL;
}

const CEffectCode::GLOBALVARIABLE              * CInstancePositionEffect::GetConstants         (UINT& uConstantCount         )
{
	uConstantCount = TOTAL_CONSTANTS;
	
	return m_Constants;
}

const CEffectCode::TEMPORARYDECLARATION        * CInstancePositionEffect::GetTemporary         (UINT& uTemporaryCount        )
{
	uTemporaryCount = TOTAL_TEMPORARY;
	
	return sm_TEMPORARY;
}

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CInstancePositionEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
{
	uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLES;
	
	return sm_ATTRIBUTE_VARIABLE;
}

const CEffectCode::CONSTANTVARIABLEDECLARATION * CInstancePositionEffect::GetConstantVariables  (UINT& uConstantVariableCount)
{
	uConstantVariableCount = TOTAL_CONSTANT_VARIABLES;
	
	return ALCHEMY_NULL;
}

bool CInstancePositionEffect::ShaderDefineHandler(
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
		UINT uMatrixIndex = puTemporary[TEMPORARY_MATRIX_INDEX];
		VertexShaderDefiner.AppendInstruction(ICompiler::MUL, uMatrixIndex, pAttributeVariables[ATTRIBUTE_VARIABLE_INSTANCE_INDEX].uHandle, puConstants[CONSTANT_MATRIX4X4_SIZE]);
	
		UINT w = 3;
		if(!pPosition->bIsUsed)
			VertexShaderDefiner.AppendInstruction(ICompiler::MOV, pPosition->uHandle, &w, 1, pPosition->uSource, &w, 1 , 0, ALCHEMY_NULL, 0);

		UINT uAddress[] = {0, 1, 2};
		VertexShaderDefiner.AppendInstruction(ICompiler::M34, 
											  pPosition->uHandle, 
											  0,
											  0,
											  uAddress,
											  3,
											  pPosition->bIsUsed ? pPosition->uHandle : pPosition->uSource, 
											  0, 
											  0, 
											  ALCHEMY_NULL,
											  0, 
											  puConstants[CONSTANT_MATRICES], 
											  uMatrixIndex, 
											  0, 
											  ALCHEMY_NULL,
											  0);
	}
	
	return true;
}