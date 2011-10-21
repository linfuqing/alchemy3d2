#include "AlchemyLightEffect.h"
#include "AlchemyRenderMethod.h"

using namespace alchemy;

const CEffectCode::TEMPORARYDECLARATION         CLightEffect::sm_TEMPORARY           [CLightEffect::TOTAL_TEMPORARY          ] = 
{  
	{CEffectInterface::FRAGMENT_SHADER, CEffectCode::VECTOR4_VARIABLE},
	{CEffectInterface::FRAGMENT_SHADER, CEffectCode::VECTOR4_VARIABLE},
};

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION CLightEffect::sm_ATTRIBUTE_VARIABLES [CLightEffect::TOTAL_ATTRIBUTE_VARIABLES] = 
{  
	{CEffectInterface::FRAGMENT_SHADER, CVertexDeclaration::FLOAT4, CVertexDeclaration::AMBIENT , 0, true, false},
	{CEffectInterface::FRAGMENT_SHADER, CVertexDeclaration::FLOAT4, CVertexDeclaration::DIFFUSE , 0, true, false},
	{CEffectInterface::FRAGMENT_SHADER, CVertexDeclaration::FLOAT4, CVertexDeclaration::SPECULAR, 0, true, false},
};

const CEffectCode::CONSTANTVARIABLEDECLARATION  CLightEffect::sm_CONSTANT_VARIABLES  [CLightEffect::TOTAL_CONSTANT_VARIABLES ] = 
{  
	{CEffectInterface::FRAGMENT_SHADER, CEffectCode::MATERIAL_VARIABLE, CRenderMethod::MATERIAL_AMBIENT , 1, 0, ALCHEMY_NULL, true, true},
	{CEffectInterface::FRAGMENT_SHADER, CEffectCode::MATERIAL_VARIABLE, CRenderMethod::MATERIAL_DIFFUSE , 1, 0, ALCHEMY_NULL, true, true},
	{CEffectInterface::FRAGMENT_SHADER, CEffectCode::MATERIAL_VARIABLE, CRenderMethod::MATERIAL_SPECULAR, 1, 0, ALCHEMY_NULL, true, true},
	{CEffectInterface::FRAGMENT_SHADER, CEffectCode::MATERIAL_VARIABLE, CRenderMethod::MATERIAL_EMISSIVE, 1, 0, ALCHEMY_NULL, true, true} 
};


CLightEffect::CLightEffect(void)
{
}

CLightEffect::~CLightEffect(void)
{
}

UINT                                            CLightEffect::GetEffectInfoFlag     (                             )
{
	return ALCHEMY_FLAG(CEffectInfoInterface::FRAGMENT_SHADER_OUTPUT);
}

const CEffectInfoInterface::SAMPLER           * CLightEffect::GetSamplers           (UINT& uSamplerCount          )
{
	uSamplerCount = TOTAL_SAMPLERS;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTPARAMETER            * CLightEffect::GetAttributes        (UINT& uAttributeCount        )
{
	uAttributeCount = TOTAL_ATTRIBUTES;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTPARAMETER            * CLightEffect::GetVarying           (UINT& uVaryingCount          )
{
	uVaryingCount = TOTAL_VARYING;

	return ALCHEMY_NULL;
}

const CEffectCode::GLOBALVARIABLE              * CLightEffect::GetConstants         (UINT& uConstantCount         )
{
	uConstantCount = TOTAL_CONSTANTS;

	return ALCHEMY_NULL;
}

const CEffectCode::TEMPORARYDECLARATION        * CLightEffect::GetTemporary         (UINT& uTemporaryCount        )
{
	uTemporaryCount = TOTAL_TEMPORARY;

	return sm_TEMPORARY;
}

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CLightEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
{
	uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLES;

	return sm_ATTRIBUTE_VARIABLES;
}

const CEffectCode::CONSTANTVARIABLEDECLARATION * CLightEffect::GetConstantVariables  (UINT& uConstantVariableCount)
{
	uConstantVariableCount = TOTAL_CONSTANT_VARIABLES;

	return sm_CONSTANT_VARIABLES;
}

bool CLightEffect::ShaderDefineHandler(
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
	UINT uColor = puTemporary[TEMPORARY_COLOR], uDestination = puTemporary[TEMPORARY_DESTINATION];
	bool bIsFillDestination = false;

	const CEffectCode::SHADERVARIABLE* pAmibent = &pAttributeVariables[ATTRIBUTE_VARIABLE_AMBIENT];
	if(pAmibent->uHandle)
	{
		if(!bIsFillDestination)
			FragmentShaderDefiner.AppendInstruction(CCompilerInterface::MUL, uDestination, pAmibent->uHandle, pConstantVariables[CONSTANT_VARIABLE_MATERIAL_AMBIENT].uHandle);
		else
		{
			FragmentShaderDefiner.AppendInstruction(CCompilerInterface::MUL, uColor, pAmibent->uHandle, pConstantVariables[CONSTANT_VARIABLE_MATERIAL_AMBIENT].uHandle);
			FragmentShaderDefiner.AppendInstruction(CCompilerInterface::ADD, uDestination, uDestination, uColor);
		}

		bIsFillDestination = true;
	}

	const CEffectCode::SHADERVARIABLE* pDiffuse = &pAttributeVariables[ATTRIBUTE_VARIABLE_DIFFUSE];
	if(pDiffuse->uHandle)
	{
		if(!bIsFillDestination)
			FragmentShaderDefiner.AppendInstruction(CCompilerInterface::MUL, uDestination, pDiffuse->uHandle, pConstantVariables[CONSTANT_VARIABLE_MATERIAL_DIFFUSE].uHandle);
		else
		{
			FragmentShaderDefiner.AppendInstruction(CCompilerInterface::MUL, uColor, pDiffuse->uHandle, pConstantVariables[CONSTANT_VARIABLE_MATERIAL_DIFFUSE].uHandle);
			FragmentShaderDefiner.AppendInstruction(CCompilerInterface::ADD, uDestination, uDestination, uColor);
		}

		bIsFillDestination = true;
	}

	const CEffectCode::SHADERVARIABLE* pSpecular = &pAttributeVariables[ATTRIBUTE_VARIABLE_SPECULAR];
	if(pSpecular->uHandle)
	{
		if(!bIsFillDestination)
			FragmentShaderDefiner.AppendInstruction(CCompilerInterface::MUL, uDestination, pSpecular->uHandle, pConstantVariables[CONSTANT_VARIABLE_MATERIAL_SPECULAR].uHandle);
		else
		{
			FragmentShaderDefiner.AppendInstruction(CCompilerInterface::MUL, uColor, pSpecular->uHandle, pConstantVariables[CONSTANT_VARIABLE_MATERIAL_SPECULAR].uHandle);
			FragmentShaderDefiner.AppendInstruction(CCompilerInterface::ADD, uDestination, uDestination, uColor);
		}

		bIsFillDestination = true;
	}

	if(!bIsFillDestination)
		FragmentShaderDefiner.AppendInstruction(CCompilerInterface::MOV, uOutputColor, pConstantVariables[CONSTANT_VARIABLE_MATERIAL_EMISSIVE].uHandle, 0);
	else
		FragmentShaderDefiner.AppendInstruction(CCompilerInterface::ADD, uOutputColor, uDestination, pConstantVariables[CONSTANT_VARIABLE_MATERIAL_EMISSIVE].uHandle);

	return true;
}