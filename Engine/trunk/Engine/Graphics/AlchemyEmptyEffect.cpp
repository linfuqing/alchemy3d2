#include "AlchemyEmptyEffect.h"

using namespace alchemy;

CEmptyEffect::CEmptyEffect()
{

}

CEmptyEffect::~CEmptyEffect()
{

}

UINT                                             CEmptyEffect::GetEffectInfoFlag    (                             )
{
	return ALCHEMY_FLAG(IEffectInfo::FRAGMENT_SHADER_OUTPUT);
}

const IEffectInfo::SAMPLER                                  * CEmptyEffect::GetSamplers          (UINT& uSamplerCount          )
{
	uSamplerCount = TOTAL_SAMPLERS;
	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION            * CEmptyEffect::GetAttributes        (UINT& uAttributeCount        )
{
	uAttributeCount = TOTAL_ATTRIBUTES;
	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION            * CEmptyEffect::GetVarying           (UINT& uVaryingCount          )
{
	uVaryingCount = TOTAL_VARYING;
	return ALCHEMY_NULL;
}

const CEffectCode::GLOBALVARIABLE              * CEmptyEffect::GetConstants         (UINT& uConstantCount         )
{
	uConstantCount = TOTAL_CONSTANTS;
	return ALCHEMY_NULL;
}

const CEffectCode::TEMPORARYDECLARATION        * CEmptyEffect::GetTemporary         (UINT& uTemporaryCount        )
{
	uTemporaryCount = TOTAL_CONSTANTS;
	return ALCHEMY_NULL;
}

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CEmptyEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
{
	uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLE;
	return ALCHEMY_NULL;
}

const CEffectCode::CONSTANTVARIABLEDECLARATION * CEmptyEffect::GetConstantVariables (UINT& uConstantVariableCount )
{
	uConstantVariableCount = TOTAL_CONSTANT_VARIABLE;
	return ALCHEMY_NULL;
}

bool CEmptyEffect::ShaderDefineHandler(
						 UINT uOutputColor, 
						 const UINT* puSamplers, 
						 const UINT* puAttributes, 
						 const UINT* puConstants, 
						 const CEffectCode::VARYING* pVarying, 
						 const UINT* puTemporary,
						 const CEffectCode::SHADERVARIABLE* pAttributeVariable,
						 const CEffectCode::SHADERVARIABLE* pConstantVariable,
						 CEffectCode::IInstructionSet& VertexShaderDefiner,
						 CEffectCode::IInstructionSet& FragmentShaderDefiner,
						 CEffectCode::CPassDefiner::CEffectCodeUtils& EffectCodeUtils)
{
	return true;
	/*UINT w = 3;
	FragmentShaderDefiner.AppendInstruction(CCompilerInterface::KIL, 0, ALCHEMY_NULL, 0,  uOutputColor, &w, 1,  0, ALCHEMY_NULL, 0);*/
}