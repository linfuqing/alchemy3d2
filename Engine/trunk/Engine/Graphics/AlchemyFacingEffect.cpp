#include "AlchemyFacingEffect.h"
#include "AlchemyRenderMethod.h"

using namespace alchemy;

CFacingEffect::CFacingEffect(bool bIsVertexShader) :
m_bIsVertexShader(bIsVertexShader)
{
	IEffect::EFFECTTYPE EffectType = bIsVertexShader ? IEffect::VERTEX_SHADER : IEffect::FRAGMENT_SHADER;

	CEffectCode::LPATTRIBUTEVARIABLEDECLARATION pAttributeVariables;
	pAttributeVariables = &m_AttributeVariables[ATTRIBUTE_VARIABLE_FACING];
	pAttributeVariables->ShaderType   = EffectType;
	pAttributeVariables->ElementType  = CVertexDeclaration::FLOAT1;
	pAttributeVariables->ElementUsage = CVertexDeclaration::FACING;
	pAttributeVariables->uIndex       = 0;
	pAttributeVariables->bIsReadOnly  = false;
	pAttributeVariables->bIsForce     = false;

	pAttributeVariables = &m_AttributeVariables[ATTRIBUTE_VARIABLE_NORMAL];
	pAttributeVariables->ShaderType   = EffectType;
	pAttributeVariables->ElementType  = CVertexDeclaration::FLOAT3;
	pAttributeVariables->ElementUsage = CVertexDeclaration::NORMAL;
	pAttributeVariables->uIndex       = 0;
	pAttributeVariables->bIsReadOnly  = true;
	pAttributeVariables->bIsForce     = true;

	CEffectCode::LPCONSTANTVARIABLEDECLARATION pConstantVariables;
	pConstantVariables = &m_ConstantVariables[CONSTANT_VARIABLE_CAMERA_LOCAL_DIRECTION_NEGATE];
	pConstantVariables->ShaderType  = EffectType;
	pConstantVariables->GlobalType  = CEffectCode::CAMERA_VARIABLE;
	pConstantVariables->uDetail     = CRenderMethod::CAMERA_LOCAL_DIRECTION_NEGATE;
	pConstantVariables->uCount      = 1;
	pConstantVariables->uIndex      = 0;
	pConstantVariables->pValue      = ALCHEMY_NULL;
	pConstantVariables->bIsReadOnly = true;
	pConstantVariables->bIsForce    = true;
}

CFacingEffect::~CFacingEffect(void)
{
}

UINT                                              CFacingEffect::GetEffectInfoFlag    (                            )
{
	return m_bIsVertexShader ? ALCHEMY_FLAG(IEffectInfo::VERTEX_SHADER_INFO) : ALCHEMY_FLAG(IEffectInfo::FRAGMENT_SHADER_INFO);
}

const IEffectInfo::SAMPLER             * CFacingEffect::GetSamplers         (UINT& uSamplerCount          )
{
	uSamplerCount = TOTAL_SAMPLERS;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION             * CFacingEffect::GetAttributes       (UINT& uAttributeCount        )
{
	uAttributeCount = TOTAL_ATTRIBUTES;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION            * CFacingEffect::GetVarying           (UINT& uVaryingCount          )
{
	uVaryingCount = TOTAL_VARYING;

	return ALCHEMY_NULL;
}

const CEffectCode::GLOBALVARIABLE              * CFacingEffect::GetConstants         (UINT& uConstantCount         )
{
	uConstantCount = TOTAL_CONSTANTS;

	return ALCHEMY_NULL;
}

const CEffectCode::TEMPORARYDECLARATION        * CFacingEffect::GetTemporary         (UINT& uTemporaryCount        )
{
	uTemporaryCount = TOTAL_TEMPORARY;

	return ALCHEMY_NULL;
}

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CFacingEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
{
	uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLES;

	return m_AttributeVariables;
}

const CEffectCode::CONSTANTVARIABLEDECLARATION * CFacingEffect::GetConstantVariables (UINT& uConstantVariableCount )
{
	uConstantVariableCount = TOTAL_CONSTANT_VARIABLES;

	return m_ConstantVariables;
}

bool CFacingEffect::ShaderDefineHandler(
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
	CEffectCode::IInstructionSet& InstructionSet = m_bIsVertexShader ? VertexShaderDefiner : FragmentShaderDefiner;

	const CEffectCode::SHADERVARIABLE* pFacing = &pAttributeVariables[ATTRIBUTE_VARIABLE_FACING];
	if(pFacing->uHandle)
	{
		InstructionSet.AppendInstruction(ICompiler::DP3, pFacing->uHandle, pAttributeVariables[ATTRIBUTE_VARIABLE_NORMAL].uHandle, pConstantVariables[CONSTANT_VARIABLE_CAMERA_LOCAL_DIRECTION_NEGATE].uHandle);
		InstructionSet.AppendInstruction(ICompiler::SAT, pFacing->uHandle, pFacing->uHandle                                      , 0                                                                          );
	}

	return true;
}