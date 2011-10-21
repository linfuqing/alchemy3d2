#include "AlchemyBlinnPhongEffect.h"
#include "AlchemyRenderMethod.h"

using namespace alchemy;

const CEffectCode::GLOBALVARIABLE               CBlinnPhongEffect::sm_CONSTANTS          [CBlinnPhongEffect::TOTAL_CONSTANTS        ] = 
{
	{CEffectInterface::FRAGMENT_SHADER, CEffectCode::CONSTANT_VARIABLE, CRenderMethod::CONSTANT_ONE, 1, 0, ALCHEMY_NULL}
};

const CEffectCode::TEMPORARYDECLARATION         CBlinnPhongEffect::sm_TEMPORARY          [CBlinnPhongEffect::TOTAL_TEMPORARY        ] = 
{
	{CEffectInterface::FRAGMENT_SHADER, CEffectCode::VECTOR3_VARIABLE, 1}, 
};

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION CBlinnPhongEffect::sm_ATTRIBUTE_VARIABLE[CBlinnPhongEffect::TOTAL_ATTRIBUTE_VARIABLES] = 
{
	{CEffectInterface::FRAGMENT_SHADER, CVertexDeclaration::FLOAT3, CVertexDeclaration::NORMAL  , 0, true , true },
	{CEffectInterface::FRAGMENT_SHADER, CVertexDeclaration::FLOAT4, CVertexDeclaration::SPECULAR, 0, false, false}
};

CBlinnPhongEffect::CBlinnPhongEffect(UINT uLightIndex)
{
	CEffectCode::LPCONSTANTVARIABLEDECLARATION pConstantVariables;

	pConstantVariables              = &m_ConstantVariable[CONSTANT_VARIABLE_CAMERA_LOCAL_DIRECTION_NEGATE];
	pConstantVariables->ShaderType  = CEffectInterface::FRAGMENT_SHADER;
	pConstantVariables->GlobalType  = CEffectCode::CAMERA_VARIABLE;
	pConstantVariables->uDetail     = CRenderMethod::CAMERA_LOCAL_DIRECTION_NEGATE;
	pConstantVariables->uCount      = 1;
	pConstantVariables->uIndex      = 0;
	pConstantVariables->pValue      = ALCHEMY_NULL;
	pConstantVariables->bIsReadOnly = true;
	pConstantVariables->bIsForce    = true;

	pConstantVariables              = &m_ConstantVariable[CONSTANT_VARIABLE_LIGHT_SPECULAR];
	pConstantVariables->ShaderType  = CEffectInterface::FRAGMENT_SHADER;
	pConstantVariables->GlobalType  = CEffectCode::LIGHT_VARIABLE;
	pConstantVariables->uDetail     = CRenderMethod::LIGHT_SPECULAR;
	pConstantVariables->uCount      = 1;
	pConstantVariables->uIndex      = uLightIndex;
	pConstantVariables->pValue      = ALCHEMY_NULL;
	pConstantVariables->bIsReadOnly = true;
	pConstantVariables->bIsForce    = true;

	pConstantVariables              = &m_ConstantVariable[CONSTANT_VARIABLE_LIGHT_LOCAL_DIRECTION];
	pConstantVariables->ShaderType  = CEffectInterface::FRAGMENT_SHADER;
	pConstantVariables->GlobalType  = CEffectCode::LIGHT_VARIABLE;
	pConstantVariables->uDetail     = CRenderMethod::LIGHT_LOCAL_DIRECTION;
	pConstantVariables->uCount      = 1;
	pConstantVariables->uIndex      = uLightIndex;
	pConstantVariables->pValue      = ALCHEMY_NULL;
	pConstantVariables->bIsReadOnly = true;
	pConstantVariables->bIsForce    = true;

	pConstantVariables              = &m_ConstantVariable[CONSTANT_VARIABLE_MATERIAL_POWER];
	pConstantVariables->ShaderType  = CEffectInterface::FRAGMENT_SHADER;
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
	return ALCHEMY_FLAG(CEffectInfoInterface::FRAGMENT_SHADER_INFO);
}

const CEffectInfoInterface::SAMPLER             * CBlinnPhongEffect::GetSamplers         (UINT& uSamplerCount          )
{
	uSamplerCount = TOTAL_SAMPLERS;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTPARAMETER             * CBlinnPhongEffect::GetAttributes       (UINT& uAttributeCount        )
{
	uAttributeCount = TOTAL_ATTRIBUTES;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTPARAMETER            * CBlinnPhongEffect::GetVarying           (UINT& uVaryingCount          )
{
	uVaryingCount = TOTAL_VARYING;

	return ALCHEMY_NULL;
}

const CEffectCode::GLOBALVARIABLE              * CBlinnPhongEffect::GetConstants         (UINT& uConstantCount         )
{
	uConstantCount = TOTAL_CONSTANTS;

	return sm_CONSTANTS;
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
	CEffectCode::CInstructionSetInterface& VertexShaderDefiner,
	CEffectCode::CInstructionSetInterface& FragmentShaderDefiner,
	CEffectCode::CPassDefiner::CEffectCodeUtils& EffectCodeUtils)
{
	const CEffectCode::SHADERVARIABLE* pSpecular = &pAttributeVariables[ATTRIBUTE_VARIABLE_SPECULAR];
	if(pSpecular->uHandle)
	{
		UINT uSpecular = puTemporary[TEMPORARY_SPECULAR];

		FragmentShaderDefiner.AppendInstruction(CCompilerInterface::ADD, uSpecular, pConstantVariables[CONSTANT_VARIABLE_LIGHT_LOCAL_DIRECTION].uHandle, pConstantVariables[CONSTANT_VARIABLE_CAMERA_LOCAL_DIRECTION_NEGATE].uHandle);

		FragmentShaderDefiner.AppendInstruction(CCompilerInterface::DP3, uSpecular, pAttributeVariables[ATTRIBUTE_VARIABLE_NORMAL].uHandle             , uSpecular                                                                 );

		FragmentShaderDefiner.AppendInstruction(CCompilerInterface::SAT, uSpecular, uSpecular                                                         , 0                                                                         );

		FragmentShaderDefiner.AppendInstruction(CCompilerInterface::POW, uSpecular, uSpecular                                                         , pConstantVariables[CONSTANT_VARIABLE_MATERIAL_POWER].uHandle               );

		FragmentShaderDefiner.AppendInstruction(CCompilerInterface::MUL, uSpecular, uSpecular                                                         , pConstantVariables[CONSTANT_VARIABLE_LIGHT_SPECULAR].uHandle               );

		UINT uAddress[] = {0, 1, 2, 3};
		if(pSpecular->bIsUsed)
			FragmentShaderDefiner.AppendInstruction(CCompilerInterface::ADD, pSpecular->uHandle, uAddress, 3, pSpecular->uHandle, uAddress, 3, uSpecular, uAddress, 3);
		else
			FragmentShaderDefiner.AppendInstruction(CCompilerInterface::MOV, pSpecular->uHandle, uAddress, 3, uSpecular         , uAddress, 3, 0        , uAddress, 3);

		FragmentShaderDefiner.AppendInstruction(CCompilerInterface::MOV, pSpecular->uHandle, &uAddress[3], 1, puConstants[CONSTANT_ONE], uAddress, 1, 0, ALCHEMY_NULL, 0);
	}

	return true;
}