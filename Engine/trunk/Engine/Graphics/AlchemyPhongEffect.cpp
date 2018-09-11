#include "AlchemyPhongEffect.h"
#include "AlchemyRenderMethod.h"

using namespace alchemy;

const CEffectCode::GLOBALVARIABLE       CPhongEffect::sm_CONSTANTS[CPhongEffect::TOTAL_CONSTANTS] = 
{
	{IEffect::FRAGMENT_SHADER, CEffectCode::CONSTANT_VARIABLE, CRenderMethod::CONSTANT_TWO, 1, 0, ALCHEMY_NULL}, 
};

const CEffectCode::TEMPORARYDECLARATION CPhongEffect::sm_TEMPORARY[CPhongEffect::TOTAL_TEMPORARY] = 
{
	{IEffect::FRAGMENT_SHADER, CEffectCode::VECTOR3_VARIABLE, 1}, 
};

CPhongEffect::CPhongEffect(UINT uLightIndex)
{
	CEffectCode::LPATTRIBUTEVARIABLEDECLARATION pAttributeVariable;

	pAttributeVariable = &m_AttributeVariables[ATTRIBUTE_VARIABLE_NORMAL];
	pAttributeVariable->ShaderType  = IEffect::FRAGMENT_SHADER;
	pAttributeVariable->ElementType = CVertexDeclaration::FLOAT3;
	pAttributeVariable->ElementUsage = CVertexDeclaration::NORMAL;
	pAttributeVariable->uIndex       = 0;
	pAttributeVariable->bIsReadOnly  = true;
	pAttributeVariable->bIsForce     = true;

	/*pAttributeVariable = &m_AttributeVariables[ATTRIBUTE_VARIABLE_LIGHTNESS];
	pAttributeVariable->ShaderType  = IEffect::FRAGMENT_SHADER;
	pAttributeVariable->ElementType = CVertexDeclaration::FLOAT1;
	pAttributeVariable->ElementUsage = CVertexDeclaration::LIGHTNESS;
	pAttributeVariable->uIndex       = 0;
	pAttributeVariable->bIsReadOnly  = true;
	pAttributeVariable->bIsForce     = false;*/

	CEffectCode::LPCONSTANTVARIABLEDECLARATION pConstantVariable;

	pConstantVariable              = &m_ConstantVariables[CONSTANT_VARIABLE_CAMERA_LOCAL_DIRECTION_NEGATE];
	pConstantVariable->ShaderType  = IEffect::FRAGMENT_SHADER;
	pConstantVariable->GlobalType  = CEffectCode::CAMERA_VARIABLE;
	pConstantVariable->uDetail     = CRenderMethod::CAMERA_LOCAL_DIRECTION_NEGATE;
	pConstantVariable->uCount      = 1;
	pConstantVariable->uIndex      = 0;
	pConstantVariable->pValue      = ALCHEMY_NULL;
	pConstantVariable->bIsReadOnly = true;
	pConstantVariable->bIsForce    = true;

	pConstantVariable              = &m_ConstantVariables[CONSTANT_VARIABLE_LIGHT_SPECULAR];
	pConstantVariable->ShaderType  = IEffect::FRAGMENT_SHADER;
	pConstantVariable->GlobalType  = CEffectCode::LIGHT_VARIABLE;
	pConstantVariable->uDetail     = CRenderMethod::LIGHT_SPECULAR;
	pConstantVariable->uCount      = 1;
	pConstantVariable->uIndex      = uLightIndex;
	pConstantVariable->pValue      = ALCHEMY_NULL;
	pConstantVariable->bIsReadOnly = true;
	pConstantVariable->bIsForce    = true;

	pConstantVariable              = &m_ConstantVariables[CONSTANT_VARIABLE_LIGHT_LOCAL_DIRECTION];
	pConstantVariable->ShaderType  = IEffect::FRAGMENT_SHADER;
	pConstantVariable->GlobalType  = CEffectCode::LIGHT_VARIABLE;
	pConstantVariable->uDetail     = CRenderMethod::LIGHT_LOCAL_DIRECTION;
	pConstantVariable->uCount      = 1;
	pConstantVariable->uIndex      = uLightIndex;
	pConstantVariable->pValue      = ALCHEMY_NULL;
	pConstantVariable->bIsReadOnly = true;
	pConstantVariable->bIsForce    = true;

	pConstantVariable              = &m_ConstantVariables[CONSTANT_VARIABLE_MATERIAL_POWER];
	pConstantVariable->ShaderType  = IEffect::FRAGMENT_SHADER;
	pConstantVariable->GlobalType  = CEffectCode::MATERIAL_VARIABLE;
	pConstantVariable->uDetail     = CRenderMethod::MATERIAL_POWER;
	pConstantVariable->uCount      = 1;
	pConstantVariable->uIndex      = 0;
	pConstantVariable->pValue      = ALCHEMY_NULL;
	pConstantVariable->bIsReadOnly = true;
	pConstantVariable->bIsForce    = true;
}

CPhongEffect::~CPhongEffect(void)
{
}

UINT                                              CPhongEffect::GetEffectInfoFlag    (                            )
{
	return ALCHEMY_FLAG(IEffectInfo::FRAGMENT_SHADER_OUTPUT);
}

const IEffectInfo::SAMPLER             * CPhongEffect::GetSamplers         (UINT& uSamplerCount          )
{
	uSamplerCount = TOTAL_SAMPLERS;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION             * CPhongEffect::GetAttributes       (UINT& uAttributeCount        )
{
	uAttributeCount = TOTAL_ATTRIBUTES;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION            * CPhongEffect::GetVarying           (UINT& uVaryingCount          )
{
	uVaryingCount = TOTAL_VARYING;

	return ALCHEMY_NULL;
}

const CEffectCode::GLOBALVARIABLE              * CPhongEffect::GetConstants         (UINT& uConstantCount         )
{
	uConstantCount = TOTAL_CONSTANTS;

	return sm_CONSTANTS;
}

const CEffectCode::TEMPORARYDECLARATION        * CPhongEffect::GetTemporary         (UINT& uTemporaryCount        )
{
	uTemporaryCount = TOTAL_TEMPORARY;

	return sm_TEMPORARY;
}

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CPhongEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
{
	uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLES;

	return m_AttributeVariables;
}

const CEffectCode::CONSTANTVARIABLEDECLARATION * CPhongEffect::GetConstantVariables (UINT& uConstantVariableCount )
{
	uConstantVariableCount = TOTAL_CONSTANT_VARIABLES;

	return m_ConstantVariables;
}

bool CPhongEffect::ShaderDefineHandler(
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
	UINT uSpecular = puTemporary[TEMPORARY_SPECULAR], /*uLightness = uSpecular, */uNormal = pAttributeVariables[ATTRIBUTE_VARIABLE_NORMAL].uHandle, uLightDirection = pConstantVariables[CONSTANT_VARIABLE_LIGHT_LOCAL_DIRECTION].uHandle;
	UINT uAddress[] = {0, 1, 2, 3};
	FragmentShaderDefiner.AppendInstruction(ICompiler::DP3, uSpecular, uAddress, 1, uNormal, uAddress, 3, uLightDirection, uAddress, 3);
	FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uSpecular, uAddress, 3, uNormal, uAddress, 3, uSpecular      , uAddress, 1);
	
	FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uSpecular, puConstants[CONSTANT_TWO], uSpecular );
	FragmentShaderDefiner.AppendInstruction(ICompiler::SUB, uSpecular, uLightDirection          , uSpecular);

	FragmentShaderDefiner.AppendInstruction(ICompiler::DP3, uSpecular   , uAddress, 1, uSpecular, uAddress, 3, pConstantVariables[CONSTANT_VARIABLE_CAMERA_LOCAL_DIRECTION_NEGATE].uHandle, uAddress    , 3);

	FragmentShaderDefiner.AppendInstruction(ICompiler::SAT, uSpecular   , uAddress, 1, uSpecular, uAddress, 1, 0                                                                          , ALCHEMY_NULL, 0);

	FragmentShaderDefiner.AppendInstruction(ICompiler::POW, uSpecular   , uAddress, 1, uSpecular, uAddress, 1, pConstantVariables[CONSTANT_VARIABLE_MATERIAL_POWER].uHandle               , uAddress    , 1);

	FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uOutputColor, uAddress, 4, uSpecular, uAddress, 1, pConstantVariables[CONSTANT_VARIABLE_LIGHT_SPECULAR].uHandle               , uAddress    , 4);

	return true;
}