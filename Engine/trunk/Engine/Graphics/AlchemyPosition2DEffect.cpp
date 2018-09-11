#include "AlchemyPosition2DEffect.h"

using namespace alchemy;

const CEffectCode::ELEMENTDECLARATION           CPosition2DEffect::sm_ATTRIBUTES         [CPosition2DEffect::TOTAL_ATTRIBUTES         ] = 
{ 
	{CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITIONT, 0}
};

const CEffectCode::ELEMENTDECLARATION           CPosition2DEffect::sm_VARYING            [CPosition2DEffect::TOTAL_VARYING            ] = 
{ 
	{CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITIONT, 0}
};

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION CPosition2DEffect::sm_ATTRIBUTE_VARIABLES[CPosition2DEffect::TOTAL_ATTRIBUTE_VARIABLES] = 
{ 
	{IEffect::VERTEX_SHADER, CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION, 0, false, false}
};

CPosition2DEffect::CPosition2DEffect(void)
{
}

CPosition2DEffect::~CPosition2DEffect(void)
{
}

UINT                                            CPosition2DEffect::GetEffectInfoFlag     (                             )
{
	return ALCHEMY_FLAG(IEffectInfo::VERTEX_SHADER_OUTPUT);
}

const IEffectInfo::SAMPLER           * CPosition2DEffect::GetSamplers           (UINT& uSamplerCount          )
{
	uSamplerCount = TOTAL_SAMPLERS;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION            * CPosition2DEffect::GetAttributes        (UINT& uAttributeCount        )
{
	uAttributeCount = TOTAL_ATTRIBUTES;

	return sm_ATTRIBUTES;
}

const CEffectCode::ELEMENTDECLARATION            * CPosition2DEffect::GetVarying           (UINT& uVaryingCount          )
{
	uVaryingCount = TOTAL_VARYING;

	return sm_VARYING;
}

const CEffectCode::GLOBALVARIABLE              * CPosition2DEffect::GetConstants         (UINT& uConstantCount         )
{
	uConstantCount = TOTAL_CONSTANTS;

	return ALCHEMY_NULL;
}

const CEffectCode::TEMPORARYDECLARATION        * CPosition2DEffect::GetTemporary         (UINT& uTemporaryCount        )
{
	uTemporaryCount = TOTAL_TEMPORARY;

	return ALCHEMY_NULL;
}

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CPosition2DEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
{
	uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLES;

	return sm_ATTRIBUTE_VARIABLES;
}

const CEffectCode::CONSTANTVARIABLEDECLARATION * CPosition2DEffect::GetConstantVariables  (UINT& uConstantVariableCount)
{
	uConstantVariableCount = TOTAL_CONSTANT_VARIABLES;

	return ALCHEMY_NULL;
}

bool CPosition2DEffect::ShaderDefineHandler(
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
	UINT uPositiont = puAttributes[ATTRIBUTE_POSITIONT];

	const CEffectCode::VARYING* pPositiont = &pVarying[VARYING_POSITIONT];
	if(!pPositiont->IsUsed)
		VertexShaderDefiner.AppendInstruction(ICompiler::MOV, pPositiont->uVertexShaderHandle, uPositiont, 0);

	const CEffectCode::SHADERVARIABLE* pPosition = &pAttributeVariables[ATTRIBUTE_VARIABLE_POSITION];
	if(pPosition->uHandle && !pPosition->bIsUsed)
		VertexShaderDefiner.AppendInstruction(ICompiler::MOV, pPosition->uHandle, uPositiont, 0);

	return true;
}