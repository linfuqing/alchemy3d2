#include "AlchemyAttributeWorldSpaceEffect.h"
#include "AlchemyRenderMethod.h"

using namespace alchemy;

const CEffectCode::GLOBALVARIABLE CAttributeWorldSpaceEffect::sm_CONSTANS[CAttributeWorldSpaceEffect::TOTAL_CONSTANTS] = 
{ 
	{IEffect::VERTEX_SHADER, CEffectCode::NORMAL_VARIABLE, CRenderMethod::WORLD_MATRIX, 1, 0} 
};

CAttributeWorldSpaceEffect::CAttributeWorldSpaceEffect(CVertexDeclaration::ELEMENTUSAGE Usage, bool bIsNoraml, UINT uIndex)
{
	CEffectCode::LPATTRIBUTEVARIABLEDECLARATION pAttribute = &m_AttributeVariable[ATTRIBUTE_VARIABLE];
	pAttribute->ShaderType                                 = IEffect::VERTEX_SHADER;
	pAttribute->ElementType                                = bIsNoraml ? CVertexDeclaration::FLOAT3 : CVertexDeclaration::FLOAT4;
	pAttribute->ElementUsage                               = Usage;
	pAttribute->uIndex                                     = uIndex;
	pAttribute->bIsReadOnly                                = false;
	pAttribute->bIsForce                                   = true;
}

CAttributeWorldSpaceEffect::~CAttributeWorldSpaceEffect(void)
{
}

UINT                                            CAttributeWorldSpaceEffect::GetEffectInfoFlag     (                             )
{
	return ALCHEMY_FLAG(IEffectInfo::VERTEX_SHADER_OUTPUT);
}

const IEffectInfo::SAMPLER           * CAttributeWorldSpaceEffect::GetSamplers           (UINT& uSamplerCount          )
{
	uSamplerCount = TOTAL_SAMPLERS;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION            * CAttributeWorldSpaceEffect::GetAttributes        (UINT& uAttributeCount        )
{
	uAttributeCount = TOTAL_ATTRIBUTES;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION            * CAttributeWorldSpaceEffect::GetVarying           (UINT& uVaryingCount          )
{
	uVaryingCount = TOTAL_VARYING;

	return ALCHEMY_NULL;
}

const CEffectCode::GLOBALVARIABLE              * CAttributeWorldSpaceEffect::GetConstants         (UINT& uConstantCount         )
{
	uConstantCount = TOTAL_CONSTANTS;

	return sm_CONSTANS;
}

const CEffectCode::TEMPORARYDECLARATION        * CAttributeWorldSpaceEffect::GetTemporary         (UINT& uTemporaryCount        )
{
	uTemporaryCount = TOTAL_TEMPORARY;

	return ALCHEMY_NULL;
}

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CAttributeWorldSpaceEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
{
	uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLES;

	return m_AttributeVariable;
}

const CEffectCode::CONSTANTVARIABLEDECLARATION * CAttributeWorldSpaceEffect::GetConstantVariables  (UINT& uConstantVariableCount)
{
	uConstantVariableCount = TOTAL_CONSTANT_VARIABLES;

	return ALCHEMY_NULL;
}

bool CAttributeWorldSpaceEffect::ShaderDefineHandler(
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
	const CEffectCode::SHADERVARIABLE* pOutput = &pAttributeVariables[ATTRIBUTE_VARIABLE];

	if(pOutput->uHandle && !pOutput->bIsUsed)
	{
		if(m_AttributeVariable[ATTRIBUTE_VARIABLE].ElementType == CVertexDeclaration::FLOAT4)
			VertexShaderDefiner.AppendInstruction(ICompiler::M44, pOutput->uHandle, pAttributeVariables[ATTRIBUTE_VARIABLE].uSource, puConstants[CONSTANT_WORLD_MATRIX]);
		else
		{
			VertexShaderDefiner.AppendInstruction(ICompiler::M33, pOutput->uHandle, pAttributeVariables[ATTRIBUTE_VARIABLE].uSource, puConstants[CONSTANT_WORLD_MATRIX]);

			UINT w = 3;
			VertexShaderDefiner.AppendInstruction(ICompiler::MOV, pOutput->uHandle, &w, 1, pAttributeVariables[ATTRIBUTE_VARIABLE].uSource, &w, 1, 0, ALCHEMY_NULL, 0);
		}
	}

	return true;
}