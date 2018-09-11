#include "AlchemyVertexColorEffect.h"

using namespace alchemy;

CVertexColorEffect::CVertexColorEffect(UINT uIndex)
{
	CEffectCode::LPELEMENTDECLARATION pVarying = &m_Varying[VARYING_COLOR];
	pVarying->Type   = CVertexDeclaration::FLOAT4;
	pVarying->Usage  = CVertexDeclaration::COLOR;
	pVarying->uIndex = uIndex;
	
	CEffectCode::LPATTRIBUTEVARIABLEDECLARATION pAttributeVariable = &m_AttributeVariables[ATTRIBUTE_VARIABLE_VERTEX_COLOR];
	pAttributeVariable->ShaderType   = IEffect::VERTEX_SHADER;
	pAttributeVariable->ElementType  = CVertexDeclaration::FLOAT4;
	pAttributeVariable->ElementUsage = CVertexDeclaration::COLOR;
	pAttributeVariable->uIndex       = uIndex;
	pAttributeVariable->bIsReadOnly  = true;
	pAttributeVariable->bIsForce     = true;
}

CVertexColorEffect::~CVertexColorEffect()
{

}

UINT                                             CVertexColorEffect::GetEffectInfoFlag    (                             )
{
	return ALCHEMY_FLAG(IEffectInfo::FRAGMENT_SHADER_OUTPUT) | ALCHEMY_FLAG(IEffectInfo::VERTEX_SHADER_INFO);
}

const IEffectInfo::SAMPLER                       * CVertexColorEffect::GetSamplers          (UINT& uSamplerCount          )
{
	uSamplerCount = TOTAL_SAMPLERS;
	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION            * CVertexColorEffect::GetAttributes        (UINT& uAttributeCount        )
{
	uAttributeCount = TOTAL_ATTRIBUTES;
	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION            * CVertexColorEffect::GetVarying           (UINT& uVaryingCount          )
{
	uVaryingCount = TOTAL_VARYING;
	return m_Varying;
}

const CEffectCode::GLOBALVARIABLE              * CVertexColorEffect::GetConstants         (UINT& uConstantCount         )
{
	uConstantCount = TOTAL_CONSTANTS;
	return ALCHEMY_NULL;
}

const CEffectCode::TEMPORARYDECLARATION        * CVertexColorEffect::GetTemporary         (UINT& uTemporaryCount        )
{
	uTemporaryCount = TOTAL_TEMPORARY;
	return ALCHEMY_NULL;
}

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CVertexColorEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
{
	uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLE;
	return m_AttributeVariables;
}

const CEffectCode::CONSTANTVARIABLEDECLARATION * CVertexColorEffect::GetConstantVariables (UINT& uConstantVariableCount )
{
	uConstantVariableCount = TOTAL_CONSTANT_VARIABLE;
	return ALCHEMY_NULL;
}

bool CVertexColorEffect::ShaderDefineHandler(
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
	VertexShaderDefiner.AppendInstruction(ICompiler::MOV, pVarying->uVertexShaderHandle, pAttributeVariable[ATTRIBUTE_VARIABLE_VERTEX_COLOR].uHandle, 0);

	FragmentShaderDefiner.AppendInstruction(ICompiler::MOV, uOutputColor, pVarying->uFragmentShaderHandle, 0);

	return true;
}