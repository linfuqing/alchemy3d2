#include "AlchemyBlinnPhongEffect.h"
#include "AlchemyRenderMethod.h"

using namespace alchemy;

const CEffectCode::TEMPORARYDECLARATION         CBlinnPhongEffect::sm_TEMPORARY          [CBlinnPhongEffect::TOTAL_TEMPORARY        ] = 
{
	{IEffect::FRAGMENT_SHADER, CEffectCode::VECTOR3_VARIABLE, 1}, 
};

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION CBlinnPhongEffect::sm_ATTRIBUTE_VARIABLE[CBlinnPhongEffect::TOTAL_ATTRIBUTE_VARIABLES] = 
{
	{IEffect::FRAGMENT_SHADER, CVertexDeclaration::FLOAT3, CVertexDeclaration::NORMAL  , 0, true , true }
};

CBlinnPhongEffect::CBlinnPhongEffect(UINT uLightIndex)
{
	CEffectCode::LPCONSTANTVARIABLEDECLARATION pConstantVariables;

	pConstantVariables              = &m_ConstantVariable[CONSTANT_VARIABLE_CAMERA_LOCAL_DIRECTION_NEGATE];
	pConstantVariables->ShaderType  = IEffect::FRAGMENT_SHADER;
	pConstantVariables->GlobalType  = CEffectCode::CAMERA_VARIABLE;
	pConstantVariables->uDetail     = CRenderMethod::CAMERA_LOCAL_DIRECTION_NEGATE;
	pConstantVariables->uCount      = 1;
	pConstantVariables->uIndex      = 0;
	pConstantVariables->pValue      = ALCHEMY_NULL;
	pConstantVariables->bIsReadOnly = true;
	pConstantVariables->bIsForce    = true;

	pConstantVariables              = &m_ConstantVariable[CONSTANT_VARIABLE_LIGHT_SPECULAR];
	pConstantVariables->ShaderType  = IEffect::FRAGMENT_SHADER;
	pConstantVariables->GlobalType  = CEffectCode::LIGHT_VARIABLE;
	pConstantVariables->uDetail     = CRenderMethod::LIGHT_SPECULAR;
	pConstantVariables->uCount      = 1;
	pConstantVariables->uIndex      = uLightIndex;
	pConstantVariables->pValue      = ALCHEMY_NULL;
	pConstantVariables->bIsReadOnly = true;
	pConstantVariables->bIsForce    = true;

	pConstantVariables              = &m_ConstantVariable[CONSTANT_VARIABLE_LIGHT_LOCAL_DIRECTION_NEGATE];
	pConstantVariables->ShaderType  = IEffect::FRAGMENT_SHADER;
	pConstantVariables->GlobalType  = CEffectCode::LIGHT_VARIABLE;
	pConstantVariables->uDetail     = CRenderMethod::LIGHT_LOCAL_DIRECTION_NEGATE;
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

CBlinnPhongEffect::~CBlinnPhongEffect(void)
{
}

UINT                                              CBlinnPhongEffect::GetEffectInfoFlag    (                            )
{
	return ALCHEMY_FLAG(IEffectInfo::FRAGMENT_SHADER_OUTPUT);
}

const IEffectInfo::SAMPLER             * CBlinnPhongEffect::GetSamplers         (UINT& uSamplerCount          )
{
	uSamplerCount = TOTAL_SAMPLERS;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION             * CBlinnPhongEffect::GetAttributes       (UINT& uAttributeCount        )
{
	uAttributeCount = TOTAL_ATTRIBUTES;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION            * CBlinnPhongEffect::GetVarying           (UINT& uVaryingCount          )
{
	uVaryingCount = TOTAL_VARYING;

	return ALCHEMY_NULL;
}

const CEffectCode::GLOBALVARIABLE              * CBlinnPhongEffect::GetConstants         (UINT& uConstantCount         )
{
	uConstantCount = TOTAL_CONSTANTS;

	return ALCHEMY_NULL;
}

const CEffectCode::TEMPORARYDECLARATION        * CBlinnPhongEffect::GetTemporary         (UINT& uTemporaryCount        )
{
	uTemporaryCount = TOTAL_TEMPORARY;

	return sm_TEMPORARY;
}

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CBlinnPhongEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
{
	uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLES;

	return sm_ATTRIBUTE_VARIABLE;
}

const CEffectCode::CONSTANTVARIABLEDECLARATION * CBlinnPhongEffect::GetConstantVariables (UINT& uConstantVariableCount )
{
	uConstantVariableCount = TOTAL_CONSTANT_VARIABLES;

	return m_ConstantVariable;
}

bool CBlinnPhongEffect::ShaderDefineHandler(
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

	FragmentShaderDefiner.AppendInstruction(ICompiler::ADD, uSpecular, pConstantVariables[CONSTANT_VARIABLE_LIGHT_LOCAL_DIRECTION_NEGATE].uHandle, pConstantVariables[CONSTANT_VARIABLE_CAMERA_LOCAL_DIRECTION_NEGATE].uHandle);
	FragmentShaderDefiner.AppendInstruction(ICompiler::NRM, uSpecular, uSpecular                                                                 , 0                                                                          );

	UINT uAddress[] = {0, 1, 2};
	FragmentShaderDefiner.AppendInstruction(ICompiler::DP3, uSpecular   , uAddress, 1, pAttributeVariables[ATTRIBUTE_VARIABLE_NORMAL].uHandle, uAddress, 3, uSpecular                                                   , uAddress    , 3);

	FragmentShaderDefiner.AppendInstruction(ICompiler::SAT, uSpecular   , uAddress, 1, uSpecular                                             , uAddress, 1, 0                                                           , ALCHEMY_NULL, 0);

	FragmentShaderDefiner.AppendInstruction(ICompiler::POW, uSpecular   , uAddress, 1, uSpecular                                             , uAddress, 1, pConstantVariables[CONSTANT_VARIABLE_MATERIAL_POWER].uHandle, uAddress    , 1);

	UINT uLightSpecular = pConstantVariables[CONSTANT_VARIABLE_LIGHT_SPECULAR].uHandle;
	FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uOutputColor, uAddress, 3, uSpecular                                             , uAddress, 1, uLightSpecular                                              , uAddress    , 3);
	UINT a = 3;
	FragmentShaderDefiner.AppendInstruction(ICompiler::MOV, uOutputColor, &a      , 1, uLightSpecular                                        , &a      , 1, 0                                                           , ALCHEMY_NULL, 0);

	return true;
}