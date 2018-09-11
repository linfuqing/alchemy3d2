#include "AlchemyAttributeProjectionEffect.h"
#include "AlchemyRenderMethod.h"

using namespace alchemy;

const CEffectCode::GLOBALVARIABLE CAttributeProjectionEffect::sm_CONSTANS[CAttributeProjectionEffect::TOTAL_CONSTANTS] = 
{ 
	{IEffect::VERTEX_SHADER, CEffectCode::CAMERA_VARIABLE, CRenderMethod::CAMERA_WORLD_VIEW_PROJECTION_MATRIX, 1, 0} 
};

CAttributeProjectionEffect::CAttributeProjectionEffect(CVertexDeclaration::ELEMENTUSAGE Usage, bool bIsNoraml, UINT uIndex)
{
	CEffectCode::LPATTRIBUTEVARIABLEDECLARATION pAttribute = &m_AttributeVariable[ATTRIBUTE_VARIABLE];
	pAttribute->ShaderType                                 = IEffect::VERTEX_SHADER;
	pAttribute->ElementType                                = bIsNoraml ? CVertexDeclaration::FLOAT3 : CVertexDeclaration::FLOAT4;
	pAttribute->ElementUsage                               = Usage;
	pAttribute->uIndex                                     = uIndex;
	pAttribute->bIsReadOnly                                = false;
	pAttribute->bIsForce                                   = true;
}

CAttributeProjectionEffect::~CAttributeProjectionEffect(void)
{
}

UINT                                            CAttributeProjectionEffect::GetEffectInfoFlag     (                             )
{
	return ALCHEMY_FLAG(IEffectInfo::VERTEX_SHADER_OUTPUT);
}

const IEffectInfo::SAMPLER           * CAttributeProjectionEffect::GetSamplers           (UINT& uSamplerCount          )
{
	uSamplerCount = TOTAL_SAMPLERS;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION            * CAttributeProjectionEffect::GetAttributes        (UINT& uAttributeCount        )
{
	uAttributeCount = TOTAL_ATTRIBUTES;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION            * CAttributeProjectionEffect::GetVarying           (UINT& uVaryingCount          )
{
	uVaryingCount = TOTAL_VARYING;

	return ALCHEMY_NULL;
}

const CEffectCode::GLOBALVARIABLE              * CAttributeProjectionEffect::GetConstants         (UINT& uConstantCount         )
{
	uConstantCount = TOTAL_CONSTANTS;

	return sm_CONSTANS;
}

const CEffectCode::TEMPORARYDECLARATION        * CAttributeProjectionEffect::GetTemporary         (UINT& uTemporaryCount        )
{
	uTemporaryCount = TOTAL_TEMPORARY;

	return ALCHEMY_NULL;
}

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CAttributeProjectionEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
{
	uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLES;

	return m_AttributeVariable;
}

const CEffectCode::CONSTANTVARIABLEDECLARATION * CAttributeProjectionEffect::GetConstantVariables  (UINT& uConstantVariableCount)
{
	uConstantVariableCount = TOTAL_CONSTANT_VARIABLES;

	return ALCHEMY_NULL;
}

bool CAttributeProjectionEffect::ShaderDefineHandler(
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
			VertexShaderDefiner.AppendInstruction(ICompiler::M44, pOutput->uHandle, pAttributeVariables[ATTRIBUTE_VARIABLE].uSource, puConstants[CONSTANT_WORLD_VIEW_PROJECTION_MATRIX]);
		else
		{
			VertexShaderDefiner.AppendInstruction(ICompiler::M34, pOutput->uHandle, pAttributeVariables[ATTRIBUTE_VARIABLE].uSource, puConstants[CONSTANT_WORLD_VIEW_PROJECTION_MATRIX]);

			UINT w = 3;
			VertexShaderDefiner.AppendInstruction(ICompiler::MOV, pOutput->uHandle, &w, 1, pAttributeVariables[ATTRIBUTE_VARIABLE].uSource, &w, 1, 0, ALCHEMY_NULL, 0);
		}
	}

	return true;
}