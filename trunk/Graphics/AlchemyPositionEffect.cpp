#include "AlchemyPositionEffect.h"
#include "AlchemyRenderMethod.h"

using namespace alchemy;

const CEffectCode::ELEMENTPARAMETER             CPositionEffect::sm_VARYING          [CPositionEffect::TOTAL_VARYING           ] = { {CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITIONT, 0} };
const CEffectCode::GLOBALVARIABLE               CPositionEffect::sm_CONSTANS         [CPositionEffect::TOTAL_CONSTANTS         ] = { {CEffectInterface::VERTEX_SHADER, CEffectCode::MATRIX_VARIABLE, CRenderMethod::WORLD_VIEW_PROJECTION_MATRIX, 1, 0} };
const CEffectCode::ATTRIBUTEVARIABLEDECLARATION CPositionEffect::sm_ATTRIBUTE_VARIABLE[CPositionEffect::TOTAL_ATTRIBUTE_VARIABLES] = { {CEffectInterface::VERTEX_SHADER, CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION, 0, true, true}, {CEffectInterface::VERTEX_SHADER, CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITIONT, 0, false, false} };

CPositionEffect::CPositionEffect(void)
{
}

CPositionEffect::~CPositionEffect(void)
{
}

UINT                                            CPositionEffect::GetEffectInfoFlag     (                             )
{
	return ALCHEMY_FLAG(CEffectInfoInterface::VERTEX_SHADER_OUTPUT);
}

const CEffectInfoInterface::SAMPLER           * CPositionEffect::GetSamplers           (UINT& uSamplerCount          )
{
	uSamplerCount = TOTAL_SAMPLERS;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTPARAMETER            * CPositionEffect::GetAttributes        (UINT& uAttributeCount        )
{
	uAttributeCount = TOTAL_ATTRIBUTES;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTPARAMETER            * CPositionEffect::GetVarying           (UINT& uVaryingCount          )
{
	uVaryingCount = TOTAL_VARYING;

	return sm_VARYING;
}

const CEffectCode::GLOBALVARIABLE              * CPositionEffect::GetConstants         (UINT& uConstantCount         )
{
	uConstantCount = TOTAL_CONSTANTS;

	return sm_CONSTANS;
}

const CEffectCode::TEMPORARYDECLARATION        * CPositionEffect::GetTemporary         (UINT& uTemporaryCount        )
{
	uTemporaryCount = TOTAL_TEMPORARY;

	return ALCHEMY_NULL;
}

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CPositionEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
{
	uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLES;

	return sm_ATTRIBUTE_VARIABLE;
}

const CEffectCode::CONSTANTVARIABLEDECLARATION * CPositionEffect::GetConstantVariables  (UINT& uConstantVariableCount)
{
	uConstantVariableCount = TOTAL_CONSTANT_VARIABLES;

	return ALCHEMY_NULL;
}

bool CPositionEffect::ShaderDefineHandler(
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
	const CEffectCode::SHADERVARIABLE* pOutputPosition = &pAttributeVariables[ATTRIBUTE_VARIABLE_POSITIONT];

	const CEffectCode::VARYING* pPositiont = &pVarying[VARYING_POSITIONT];
	if(!pPositiont->IsUsed)
	{
		if(pOutputPosition->uHandle && !pOutputPosition->bIsUsed)
		{
			VertexShaderDefiner.AppendInstruction(CCompilerInterface::M44, pOutputPosition->uHandle, pAttributeVariables[ATTRIBUTE_VARIABLE_POSITION].uHandle, puConstants[CONSTANT_WORLD_VIEW_PROJECTION_MATRIX]);

			VertexShaderDefiner.AppendInstruction(CCompilerInterface::MOV, pPositiont->uVertexShaderHandle, pOutputPosition->uHandle, 0);
		}
		else
			VertexShaderDefiner.AppendInstruction(CCompilerInterface::M44, pPositiont->uVertexShaderHandle, pAttributeVariables[ATTRIBUTE_VARIABLE_POSITION].uHandle, puConstants[CONSTANT_WORLD_VIEW_PROJECTION_MATRIX]);
	}

	return true;
}