#include "AlchemyDepthEffect.h"
#include "AlchemyRenderMethod.h"

using namespace alchemy;

const CEffectCode::ELEMENTPARAMETER             CDepthEffect::sm_VARYING           [CDepthEffect::TOTAL_VARYING            ] = { {CVertexDeclaration::FLOAT1, CVertexDeclaration::DEPTH, 0} };
const CEffectCode::GLOBALVARIABLE               CDepthEffect::sm_CONSTANS          [CDepthEffect::TOTAL_CONSTANTS          ] = { {CEffectInterface::VERTEX_SHADER, CEffectCode::MATRIX_VARIABLE, CRenderMethod::WORLD_VIEW_PROJECTION_MATRIX, 1, 0, ALCHEMY_NULL}, {CEffectInterface::FRAGMENT_SHADER, CEffectCode::CAMERA_VARIABLE, CRenderMethod::CAMERA_NEAR, 1, 0, ALCHEMY_NULL}, {CEffectInterface::FRAGMENT_SHADER, CEffectCode::CAMERA_VARIABLE, CRenderMethod::CAMERA_DISTANCE, 1, 0, ALCHEMY_NULL} };
const CEffectCode::TEMPORARYDECLARATION         CDepthEffect::sm_TEMPORARY         [CDepthEffect::TOTAL_TEMPORARY          ] = { {CEffectInterface::FRAGMENT_SHADER, CEffectCode::VECTOR1_VARIABLE, 1} };
const CEffectCode::ATTRIBUTEVARIABLEDECLARATION CDepthEffect::sm_ATTRIBUTE_VARIABLES[CDepthEffect::TOTAL_ATTRIBUTE_VARIABLES] = { {CEffectInterface::VERTEX_SHADER, CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION , 0, true, true} };

CDepthEffect::CDepthEffect(void)
{
}

CDepthEffect::~CDepthEffect(void)
{
}

UINT CDepthEffect::GetEffectInfoFlag()
{
	return ALCHEMY_FLAG(CEffectInfoInterface::FRAGMENT_SHADER_OUTPUT);
}

const CEffectInfoInterface::SAMPLER* CDepthEffect::GetSamplers(UINT& uSamplerCount)
{
	uSamplerCount = TOTAL_SAMPLERS;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTPARAMETER* CDepthEffect::GetAttributes(UINT& uAttributeCount)
{
	uAttributeCount = TOTAL_ATTRIBUTES;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTPARAMETER* CDepthEffect::GetVarying (UINT& uVaryingCount)
{
	uVaryingCount = TOTAL_VARYING;

	return sm_VARYING;
}

const CEffectCode::GLOBALVARIABLE* CDepthEffect::GetConstants(UINT& uConstantCount)
{
	uConstantCount = TOTAL_CONSTANTS;

	return sm_CONSTANS;
}

const CEffectCode::TEMPORARYDECLARATION* CDepthEffect::GetTemporary(UINT& uTemporaryCount)
{
	uTemporaryCount = TOTAL_TEMPORARY;

	return sm_TEMPORARY;
}

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CDepthEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
{
	uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLES;

	return sm_ATTRIBUTE_VARIABLES;
}

const CEffectCode::CONSTANTVARIABLEDECLARATION * CDepthEffect::GetConstantVariables(UINT& uConstantVariableCount)
{
	uConstantVariableCount = TOTAL_CONSTANT_VARIABLES;

	return ALCHEMY_NULL;
}

bool CDepthEffect::ShaderDefineHandler(	
	UINT uOutputColor, 
	const UINT* puSamplers, 
	const UINT* puAttributes, 
	const UINT* puConstants, 
	const CEffectCode::VARYING* pVarying, 
	const UINT* puTemporary,
	const CEffectCode::SHADERVARIABLE* pAttributeVariables,
	const CEffectCode::SHADERVARIABLE* pConstantVariables,
	CEffectCode::CInstructionSetInterface& VertexShaderDefiner,
	CEffectCode::CInstructionSetInterface& FragmentShaderDefiner,
	CEffectCode::CPassDefiner::CEffectCodeUtils& EffectCodeUtils)
{
	const CEffectCode::VARYING* pDepth = &pVarying[VARYING_DEPTH];
	VertexShaderDefiner.AppendInstruction(CCompilerInterface::M44, pDepth->uVertexShaderHandle, ALCHEMY_NULL, 0, pAttributeVariables[ATTRIBUTE_VARIABLE_POSITION].uHandle, ALCHEMY_NULL, 0, puConstants[CONSTANT_WORLD_VIEW_PROJECTION_MATRIX], ALCHEMY_NULL, 0);

	UINT x = 0, z = 2, uDepth = puTemporary[TEMPORARY_DEPTH];
	FragmentShaderDefiner.AppendInstruction(CCompilerInterface::SUB, uDepth, &x, 1, pDepth->uFragmentShaderHandle, &z, 1, puConstants[CONSTANT_CAMERA_NEAR], &x, 1);
	FragmentShaderDefiner.AppendInstruction(CCompilerInterface::DIV, uDepth      , uDepth, puConstants[CONSTANT_CAMERA_DISTANTCE]);
	FragmentShaderDefiner.AppendInstruction(CCompilerInterface::SAT, uOutputColor, uDepth, 0);

	return true;
}