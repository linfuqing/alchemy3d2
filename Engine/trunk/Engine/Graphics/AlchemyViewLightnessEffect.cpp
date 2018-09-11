#include "AlchemyViewLightnessEffect.h"
#include "AlchemyRenderMethod.h"

using namespace alchemy;

CViewLightnessEffect::CViewLightnessEffect(UINT uLightIndex)
{
	CEffectCode::LPATTRIBUTEVARIABLEDECLARATION pAttributeVariable;
	
	pAttributeVariable               = &m_AttributeVariables[ATTRIBUTE_VARIABLE_NORMAL];
	pAttributeVariable->ShaderType   = IEffect::FRAGMENT_SHADER;
	pAttributeVariable->ElementType  = CVertexDeclaration::FLOAT3;
	pAttributeVariable->ElementUsage = CVertexDeclaration::NORMAL;
	pAttributeVariable->uIndex       = 0;
	pAttributeVariable->bIsReadOnly  = true;
	pAttributeVariable->bIsForce     = true;
	
	pAttributeVariable               = &m_AttributeVariables[ATTRIBUTE_VARIABLE_LIGHTNESS];
	pAttributeVariable->ShaderType   = IEffect::FRAGMENT_SHADER;
	pAttributeVariable->ElementType  = CVertexDeclaration::FLOAT1;
	pAttributeVariable->ElementUsage = CVertexDeclaration::LIGHTNESS;
	pAttributeVariable->uIndex       = uLightIndex;
	pAttributeVariable->bIsReadOnly  = false;
	pAttributeVariable->bIsForce     = false;
	
	CEffectCode::LPCONSTANTVARIABLEDECLARATION pConstantVariable;

	pConstantVariable              = &m_ConstantVariables[CONSTANT_VARIABLE_LIGHT_VIEW_DIRECTION_NEGATE];
	pConstantVariable->ShaderType  = IEffect::FRAGMENT_SHADER;
	pConstantVariable->GlobalType  = CEffectCode::LIGHT_VARIABLE;
	pConstantVariable->uDetail     = CRenderMethod::LIGHT_VIEW_DIRECTION_NEGATE;
	pConstantVariable->uCount      = 1;
	pConstantVariable->uIndex      = uLightIndex;
	pConstantVariable->pValue      = ALCHEMY_NULL;
	pConstantVariable->bIsReadOnly = true;
	pConstantVariable->bIsForce    = true;
}

CViewLightnessEffect::~CViewLightnessEffect(void)
{
}

UINT                                              CViewLightnessEffect::GetEffectInfoFlag    (                            )
{
	return ALCHEMY_FLAG(IEffectInfo::FRAGMENT_SHADER_INFO);
}

const IEffectInfo::SAMPLER             * CViewLightnessEffect::GetSamplers         (UINT& uSamplerCount          )
{
	uSamplerCount = TOTAL_SAMPLERS;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION             * CViewLightnessEffect::GetAttributes       (UINT& uAttributeCount        )
{
	uAttributeCount = TOTAL_ATTRIBUTES;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION            * CViewLightnessEffect::GetVarying           (UINT& uVaryingCount          )
{
	uVaryingCount = TOTAL_VARYING;

	return ALCHEMY_NULL;
}

const CEffectCode::GLOBALVARIABLE              * CViewLightnessEffect::GetConstants         (UINT& uConstantCount         )
{
	uConstantCount = TOTAL_CONSTANTS;

	return ALCHEMY_NULL;
}

const CEffectCode::TEMPORARYDECLARATION        * CViewLightnessEffect::GetTemporary         (UINT& uTemporaryCount        )
{
	uTemporaryCount = TOTAL_TEMPORARY;

	return ALCHEMY_NULL;
}

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CViewLightnessEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
{
	uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLES;

	return m_AttributeVariables;
}

const CEffectCode::CONSTANTVARIABLEDECLARATION * CViewLightnessEffect::GetConstantVariables (UINT& uConstantVariableCount )
{
	uConstantVariableCount = TOTAL_CONSTANT_VARIABLES;

	return m_ConstantVariables;
}

bool CViewLightnessEffect::ShaderDefineHandler(
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
	const CEffectCode::SHADERVARIABLE* pLightness = &pAttributeVariables[ATTRIBUTE_VARIABLE_LIGHTNESS];

	if(pLightness->uHandle && !pLightness->bIsUsed)
	{
		FragmentShaderDefiner.AppendInstruction(ICompiler::DP3, pLightness->uHandle, pAttributeVariables[ATTRIBUTE_VARIABLE_NORMAL].uHandle, pConstantVariables[CONSTANT_VARIABLE_LIGHT_VIEW_DIRECTION_NEGATE].uHandle);
		FragmentShaderDefiner.AppendInstruction(ICompiler::SAT, pLightness->uHandle, pLightness->uHandle                                   , 0                                                                        );
	}

	return true;
}