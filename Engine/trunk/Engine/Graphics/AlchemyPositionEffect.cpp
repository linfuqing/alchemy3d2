#include "AlchemyPositionEffect.h"
#include "AlchemyRenderMethod.h"

using namespace alchemy;

const CEffectCode::ELEMENTDECLARATION             CPositionEffect::sm_VARYING          [CPositionEffect::TOTAL_VARYING           ] = 
{ 
	{CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITIONT, 0} 
};

const CEffectCode::GLOBALVARIABLE               CPositionEffect::sm_CONSTANS         [CPositionEffect::TOTAL_CONSTANTS         ] = 
{ 
	{IEffect::VERTEX_SHADER, CEffectCode::CAMERA_VARIABLE, CRenderMethod::CAMERA_WORLD_VIEW_PROJECTION_MATRIX, 1, 0}
};

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION CPositionEffect::sm_ATTRIBUTE_VARIABLE[CPositionEffect::TOTAL_ATTRIBUTE_VARIABLES] = 
{ 
	{IEffect::VERTEX_SHADER, CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION, 0, true, true}, 
	{IEffect::VERTEX_SHADER, CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITIONT, 0, false, false} 
};

CPositionEffect::CPositionEffect(void)
{
}

CPositionEffect::~CPositionEffect(void)
{
}

UINT                                            CPositionEffect::GetEffectInfoFlag     (                             )
{
	return ALCHEMY_FLAG(IEffectInfo::VERTEX_SHADER_OUTPUT) | ALCHEMY_FLAG(IEffectInfo::VERTEX_SHADER_INFO);
}

const IEffectInfo::SAMPLER           * CPositionEffect::GetSamplers           (UINT& uSamplerCount          )
{
	uSamplerCount = TOTAL_SAMPLERS;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION            * CPositionEffect::GetAttributes        (UINT& uAttributeCount        )
{
	uAttributeCount = TOTAL_ATTRIBUTES;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION            * CPositionEffect::GetVarying           (UINT& uVaryingCount          )
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
	CEffectCode::IInstructionSet& VertexShaderDefiner,
	CEffectCode::IInstructionSet& FragmentShaderDefiner,
	CEffectCode::CPassDefiner::CEffectCodeUtils& EffectCodeUtils)
{
	const CEffectCode::SHADERVARIABLE* pOutputPosition = &pAttributeVariables[ATTRIBUTE_VARIABLE_POSITIONT];

	const CEffectCode::VARYING* pPositiont = &pVarying[VARYING_POSITIONT];
	if(!pPositiont->IsUsed)
	{
		if(pOutputPosition->uHandle && !pOutputPosition->bIsUsed)
		{
			VertexShaderDefiner.AppendInstruction(ICompiler::M44, pOutputPosition->uHandle, pAttributeVariables[ATTRIBUTE_VARIABLE_POSITION].uHandle, puConstants[CONSTANT_WORLD_VIEW_PROJECTION_MATRIX]);

			VertexShaderDefiner.AppendInstruction(ICompiler::MOV, pPositiont->uVertexShaderHandle, pOutputPosition->uHandle, 0);
		}
		else
			VertexShaderDefiner.AppendInstruction(ICompiler::M44, pPositiont->uVertexShaderHandle, pAttributeVariables[ATTRIBUTE_VARIABLE_POSITION].uHandle, puConstants[CONSTANT_WORLD_VIEW_PROJECTION_MATRIX]);
	}

	return true;
}