#include "AlchemyBillboardEffect.h"
#include "AlchemyRenderMethod.h"

using namespace alchemy;

const CEffectCode::GLOBALVARIABLE						CBillboardEffect::sm_CONSTANT[TOTAL_CONSTANTS] = 
{ 
	{IEffect::VERTEX_SHADER, CEffectCode::CAMERA_VARIABLE, CRenderMethod::CAMERA_WORLD_VIEW_MATRIX, 1, 0},
	{IEffect::VERTEX_SHADER, CEffectCode::CAMERA_VARIABLE, CRenderMethod::CAMERA_PROJECTION_MATRIX, 1, 0} 
};

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION			CBillboardEffect::sm_ATTRIBUTEVARIABLE[TOTAL_ATTRIBUTE_VARIABLES] = 
{ 
	{IEffect::VERTEX_SHADER, CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION, 0, true, true}, 
	{IEffect::VERTEX_SHADER, CVertexDeclaration::FLOAT3, CVertexDeclaration::BLENDWEIGHT, 0, true, true} 
};

const CEffectCode::TEMPORARYDECLARATION					CBillboardEffect::sm_TEMPORARY[TOTAL_TEMPORARY] = 
{ 
	{IEffect::VERTEX_SHADER, CEffectCode::VECTOR4_VARIABLE,1 } 
};

const CEffectCode::ELEMENTDECLARATION						CBillboardEffect::sm_VARYING[TOTAL_VARYING] = 
{ 
	{CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITIONT, 0} 
};

CBillboardEffect::CBillboardEffect()
{

}

CBillboardEffect::~CBillboardEffect()
{

}

UINT                                             CBillboardEffect::GetEffectInfoFlag    (                             )
{
	return ALCHEMY_FLAG(IEffectInfo::VERTEX_SHADER_OUTPUT);
}

const IEffectInfo::SAMPLER                                  * CBillboardEffect::GetSamplers          (UINT& uSamplerCount          )
{
	uSamplerCount = TOTAL_SAMPLERS;
	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION            * CBillboardEffect::GetAttributes        (UINT& uAttributeCount        )
{
	uAttributeCount = TOTAL_ATTRIBUTES;
	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION            * CBillboardEffect::GetVarying           (UINT& uVaryingCount          )
{
	uVaryingCount = TOTAL_VARYING;
	return sm_VARYING;
}

const CEffectCode::GLOBALVARIABLE              * CBillboardEffect::GetConstants         (UINT& uConstantCount         )
{
	uConstantCount = TOTAL_CONSTANTS;
	return sm_CONSTANT;
}

const CEffectCode::TEMPORARYDECLARATION        * CBillboardEffect::GetTemporary         (UINT& uTemporaryCount        )
{
	uTemporaryCount = TOTAL_TEMPORARY;
	return sm_TEMPORARY;
}

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CBillboardEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
{
	uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLES;
	return sm_ATTRIBUTEVARIABLE;
}

const CEffectCode::CONSTANTVARIABLEDECLARATION * CBillboardEffect::GetConstantVariables (UINT& uConstantVariableCount )
{
	uConstantVariableCount = TOTAL_CONSTANT_VARIABLES;
	return ALCHEMY_NULL;
}

bool CBillboardEffect::ShaderDefineHandler(
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
	UINT uSubAddress[] = {0,1,2,3};
	VertexShaderDefiner.AppendInstruction( ICompiler::M44, puTemporary[TEMPORARY_POSITION], pAttributeVariables[ATTRIBUTE_POSITION].uHandle, puConstants[CONSTANT_WORLD_VIEW_MATRIX]);
	VertexShaderDefiner.AppendInstruction( ICompiler::ADD, puTemporary[TEMPORARY_POSITION], uSubAddress, 3, pAttributeVariables[ATTRIBUTE_BLENDWEIGHT].uHandle, uSubAddress, 3, puTemporary[TEMPORARY_POSITION], uSubAddress, 3);
	VertexShaderDefiner.AppendInstruction( ICompiler::MOV, puTemporary[TEMPORARY_POSITION], &uSubAddress[3], 1, pAttributeVariables[ATTRIBUTE_POSITION].uHandle, &uSubAddress[3], 1, 0, ALCHEMY_NULL, 0);
	VertexShaderDefiner.AppendInstruction( ICompiler::M44, pVarying->uVertexShaderHandle, puTemporary[TEMPORARY_POSITION], puConstants[CONSTANT_PROJECTION_MATRIX]);

	return true;
}