#include "AlchemyViewBlinnPhongEffect.h"
#include "AlchemyRenderMethod.h"

using namespace alchemy;

const CEffectCode::GLOBALVARIABLE               CViewBlinnPhongEffect::sm_CONSTANTS          [CViewBlinnPhongEffect::TOTAL_CONSTANTS        ] = 
{
	{IEffect::FRAGMENT_SHADER, CEffectCode::CONSTANT_VARIABLE, CRenderMethod::CONSTANT_ONE, 1, 0, ALCHEMY_NULL}
};

const CEffectCode::TEMPORARYDECLARATION         CViewBlinnPhongEffect::sm_TEMPORARY          [CViewBlinnPhongEffect::TOTAL_TEMPORARY        ] = 
{
	{IEffect::FRAGMENT_SHADER, CEffectCode::VECTOR3_VARIABLE, 1}, 
};

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION CViewBlinnPhongEffect::sm_ATTRIBUTE_VARIABLE[CViewBlinnPhongEffect::TOTAL_ATTRIBUTE_VARIABLES] = 
{
	{IEffect::FRAGMENT_SHADER, CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION, 0, true , false}, 
	{IEffect::FRAGMENT_SHADER, CVertexDeclaration::FLOAT3, CVertexDeclaration::NORMAL  , 0, true , true }
};

CViewBlinnPhongEffect::CViewBlinnPhongEffect(UINT uLightIndex)
{
	CEffectCode::LPCONSTANTVARIABLEDECLARATION pConstantVariables;

	pConstantVariables              = &m_ConstantVariable[CONSTANT_VARIABLE_LIGHT_SPECULAR];
	pConstantVariables->ShaderType  = IEffect::FRAGMENT_SHADER;
	pConstantVariables->GlobalType  = CEffectCode::LIGHT_VARIABLE;
	pConstantVariables->uDetail     = CRenderMethod::LIGHT_SPECULAR;
	pConstantVariables->uCount      = 1;
	pConstantVariables->uIndex      = uLightIndex;
	pConstantVariables->pValue      = ALCHEMY_NULL;
	pConstantVariables->bIsReadOnly = true;
	pConstantVariables->bIsForce    = true;

	pConstantVariables              = &m_ConstantVariable[CONSTANT_VARIABLE_LIGHT_VIEW_DIRECTION_NEGATE];
	pConstantVariables->ShaderType  = IEffect::FRAGMENT_SHADER;
	pConstantVariables->GlobalType  = CEffectCode::LIGHT_VARIABLE;
	pConstantVariables->uDetail     = CRenderMethod::LIGHT_VIEW_DIRECTION_NEGATE;
	pConstantVariables->uCount      = 1;
	pConstantVariables->uIndex      = uLightIndex;
	pConstantVariables->pValue      = ALCHEMY_NULL;
	pConstantVariables->bIsReadOnly = true;
	pConstantVariables->bIsForce    = true;

	pConstantVariables              = &m_ConstantVariable[CONSTANT_VARIABLE_MATERIAL_POWER];
	pConstantVariables->ShaderType  = IEffect::FRAGMENT_SHADER;
	pConstantVariables->GlobalType  = CEffectCode::MATERIAL_VARIABLE;
	pConstantVariables->uDetail     = CRenderMethod::MATERIAL_POWER;
	pConstantVariables->uCount      = 1;
	pConstantVariables->uIndex      = 0;
	pConstantVariables->pValue      = ALCHEMY_NULL;
	pConstantVariables->bIsReadOnly = true;
	pConstantVariables->bIsForce    = true;
}

CViewBlinnPhongEffect::~CViewBlinnPhongEffect(void)
{
}

UINT                                              CViewBlinnPhongEffect::GetEffectInfoFlag    (                            )
{
	return ALCHEMY_FLAG(IEffectInfo::FRAGMENT_SHADER_OUTPUT);
}

const IEffectInfo::SAMPLER             * CViewBlinnPhongEffect::GetSamplers         (UINT& uSamplerCount          )
{
	uSamplerCount = TOTAL_SAMPLERS;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION             * CViewBlinnPhongEffect::GetAttributes       (UINT& uAttributeCount        )
{
	uAttributeCount = TOTAL_ATTRIBUTES;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION            * CViewBlinnPhongEffect::GetVarying           (UINT& uVaryingCount          )
{
	uVaryingCount = TOTAL_VARYING;

	return ALCHEMY_NULL;
}

const CEffectCode::GLOBALVARIABLE              * CViewBlinnPhongEffect::GetConstants         (UINT& uConstantCount         )
{
	uConstantCount = TOTAL_CONSTANTS;

	return sm_CONSTANTS;
}

const CEffectCode::TEMPORARYDECLARATION        * CViewBlinnPhongEffect::GetTemporary         (UINT& uTemporaryCount        )
{
	uTemporaryCount = TOTAL_TEMPORARY;

	return sm_TEMPORARY;
}

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CViewBlinnPhongEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
{
	uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLES;

	return sm_ATTRIBUTE_VARIABLE;
}

const CEffectCode::CONSTANTVARIABLEDECLARATION * CViewBlinnPhongEffect::GetConstantVariables (UINT& uConstantVariableCount )
{
	uConstantVariableCount = TOTAL_CONSTANT_VARIABLES;

	return m_ConstantVariable;
}

bool CViewBlinnPhongEffect::ShaderDefineHandler(
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
	UINT uSpecular = puTemporary[TEMPORARY_SPECULAR];

	const CEffectCode::SHADERVARIABLE* pPosition = &pAttributeVariables[ATTRIBUTE_VARIABLE_POSITION];

	UINT uAddress[] = {0, 1, 2, 3};
	if(pPosition->uHandle)
	{
		FragmentShaderDefiner.AppendInstruction(ICompiler::NRM, uSpecular, uAddress, 3, pPosition->uHandle, uAddress, 3, 0, ALCHEMY_NULL, 0);
		FragmentShaderDefiner.AppendInstruction(ICompiler::SUB, uSpecular, pConstantVariables[CONSTANT_VARIABLE_LIGHT_VIEW_DIRECTION_NEGATE].uHandle, uSpecular);
	}
	else
	{
		FragmentShaderDefiner.AppendInstruction(ICompiler::MOV, uSpecular, pConstantVariables[CONSTANT_VARIABLE_LIGHT_VIEW_DIRECTION_NEGATE].uHandle, 0);

		FragmentShaderDefiner.AppendInstruction(ICompiler::SUB, uSpecular, &uAddress[2], 1, uSpecular, &uAddress[2], 1, puConstants[CONSTANT_ONE], uAddress, 1);
	}

	FragmentShaderDefiner.AppendInstruction(ICompiler::DP3, uSpecular   , uAddress, 1, pAttributeVariables[ATTRIBUTE_VARIABLE_NORMAL].uHandle, uAddress, 3, uSpecular                                                   , uAddress, 3);

	FragmentShaderDefiner.AppendInstruction(ICompiler::SAT, uSpecular   , uAddress, 1, uSpecular                                             , uAddress, 1, 0                                                           , uAddress, 1);

	FragmentShaderDefiner.AppendInstruction(ICompiler::POW, uSpecular   , uAddress, 1, uSpecular                                             , uAddress, 1, pConstantVariables[CONSTANT_VARIABLE_MATERIAL_POWER].uHandle, uAddress, 1);

	FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uOutputColor, uAddress, 4, uSpecular                                             , uAddress, 1, pConstantVariables[CONSTANT_VARIABLE_LIGHT_SPECULAR].uHandle, uAddress, 4);

	return true;
}