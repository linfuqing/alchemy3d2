#include "AlchemyAttributeViewSpaceEffect.h"
#include "AlchemyRenderMethod.h"

using namespace alchemy;


const CEffectCode::GLOBALVARIABLE CAttributeViewSpaceEffect::sm_CONSTANS[CAttributeViewSpaceEffect::TOTAL_CONSTANTS] = { {CEffectInterface::VERTEX_SHADER, CEffectCode::MATRIX_VARIABLE, CRenderMethod::WORLD_VIEW_MATRIX, 1, 0} };

CAttributeViewSpaceEffect::CAttributeViewSpaceEffect(CVertexDeclaration::ELEMENTUSAGE Usage, bool bIsNoraml, UINT uIndex)
{
	CEffectCode::LPATTRIBUTEVARIABLEDECLARATION pAttribute = &m_AttributeVariable[ATTRIBUTE_VARIABLE];
	pAttribute->ShaderType                                 = CEffectInterface::VERTEX_SHADER;
	pAttribute->ElementType                                = bIsNoraml ? CVertexDeclaration::FLOAT3 : CVertexDeclaration::FLOAT4;
	pAttribute->ElementUsage                               = Usage;
	pAttribute->uIndex                                     = uIndex;
	pAttribute->bIsReadOnly                                = false;
	pAttribute->bIsForce                                   = true;
}

CAttributeViewSpaceEffect::~CAttributeViewSpaceEffect(void)
{
}

UINT                                            CAttributeViewSpaceEffect::GetEffectInfoFlag     (                             )
{
	return ALCHEMY_FLAG(CEffectInfoInterface::VERTEX_SHADER_OUTPUT);
}

const CEffectInfoInterface::SAMPLER           * CAttributeViewSpaceEffect::GetSamplers           (UINT& uSamplerCount          )
{
	uSamplerCount = TOTAL_SAMPLERS;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTPARAMETER            * CAttributeViewSpaceEffect::GetAttributes        (UINT& uAttributeCount        )
{
	uAttributeCount = TOTAL_ATTRIBUTES;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTPARAMETER            * CAttributeViewSpaceEffect::GetVarying           (UINT& uVaryingCount          )
{
	uVaryingCount = TOTAL_VARYING;

	return ALCHEMY_NULL;
}

const CEffectCode::GLOBALVARIABLE              * CAttributeViewSpaceEffect::GetConstants         (UINT& uConstantCount         )
{
	uConstantCount = TOTAL_CONSTANTS;

	return sm_CONSTANS;
}

const CEffectCode::TEMPORARYDECLARATION        * CAttributeViewSpaceEffect::GetTemporary         (UINT& uTemporaryCount        )
{
	uTemporaryCount = TOTAL_TEMPORARY;

	return ALCHEMY_NULL;
}

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CAttributeViewSpaceEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
{
	uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLE;

	return m_AttributeVariable;
}

const CEffectCode::CONSTANTVARIABLEDECLARATION * CAttributeViewSpaceEffect::GetConstantVariables  (UINT& uConstantVariableCount)
{
	uConstantVariableCount = TOTAL_CONSTANT_VARIABLE;

	return ALCHEMY_NULL;
}

bool CAttributeViewSpaceEffect::ShaderDefineHandler(
	UINT uOutputColor, 
	const UINT* puSamplers, 
	const UINT* puAttributes, 
	const UINT* puConstants, 
	const CEffectCode::VARYING* pVarying, 
	const UINT* puTemporary,
	const CEffectCode::SHADERVARIABLE* pAttributeVariable,
	const CEffectCode::SHADERVARIABLE* pConstantVariable,
	CEffectCode::CInstructionSetInterface& VertexShaderDefiner,
	CEffectCode::CInstructionSetInterface& FragmentShaderDefiner,
	CEffectCode::CPassDefiner::CEffectCodeUtils& EffectCodeUtils)
{
	const CEffectCode::SHADERVARIABLE* pOutput = &pAttributeVariable[ATTRIBUTE_VARIABLE];

	if(pOutput->uHandle && !pOutput->bIsUsed)
	{
		if(m_AttributeVariable[ATTRIBUTE_VARIABLE].ElementType == CVertexDeclaration::FLOAT4)
			VertexShaderDefiner.AppendInstruction(CCompilerInterface::M44, pOutput->uHandle, pAttributeVariable[ATTRIBUTE_VARIABLE].uSource, puConstants[CONSTANT_WORLD_VIEW_MATRIX]);
		else
		{
			VertexShaderDefiner.AppendInstruction(CCompilerInterface::M33, pOutput->uHandle, pAttributeVariable[ATTRIBUTE_VARIABLE].uSource, puConstants[CONSTANT_WORLD_VIEW_MATRIX]);

			UINT w = 3;
			VertexShaderDefiner.AppendInstruction(CCompilerInterface::MOV, pOutput->uHandle, &w, 1, pAttributeVariable[ATTRIBUTE_VARIABLE].uSource, &w, 1, 0, ALCHEMY_NULL, 0);
		}
	}

	return true;
}