#include "AlchemyNormalMapEffect.h"

#include "AlchemyRenderMethod.h"

using namespace alchemy;

const CEffectCode::GLOBALVARIABLE               CNormalMapEffect::sm_CONSTANTS         [CNormalMapEffect::TOTAL_CONSTANTS         ] = 
{
	{CEffectInterface::FRAGMENT_SHADER, CEffectCode::CONSTANT_VARIABLE, CRenderMethod::CONSTANT_HALF_ONE, 1, 0, ALCHEMY_NULL}
};

const CEffectCode::TEMPORARYDECLARATION         CNormalMapEffect::sm_TEMPORARY         [CNormalMapEffect::TOTAL_TEMPORARY         ] = 
{ 
	{CEffectInterface::VERTEX_SHADER, CEffectCode::VECTOR3_VARIABLE, 1}
};

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION CNormalMapEffect::sm_ATTRIBUTE_VARIABLES[CNormalMapEffect::TOTAL_ATTRIBUTE_VARIABLES] = 
{ 
	{CEffectInterface::VERTEX_SHADER  , CVertexDeclaration::FLOAT3, CVertexDeclaration::BINORMAL, 0, true , false},
	{CEffectInterface::FRAGMENT_SHADER, CVertexDeclaration::FLOAT3, CVertexDeclaration::NORMAL  , 0, false, false}
};

CNormalMapEffect::CNormalMapEffect(UINT uNormalMapUVIndex, UINT uNormalMapIndex, UINT uNumLights, PUINT puLightIndices)
{
	CEffectCode::LPELEMENTPARAMETER pElementParameter;
	pElementParameter         = &m_Attributes[ATTRIBUTE_NORAML];
	pElementParameter->Type   = CVertexDeclaration::FLOAT3;
	pElementParameter->Usage  = CVertexDeclaration::NORMAL;
	pElementParameter->uIndex = 0;

	pElementParameter         = &m_Attributes[ATTRIBUTE_TANGENT];
	pElementParameter->Type   = CVertexDeclaration::FLOAT3;
	pElementParameter->Usage  = CVertexDeclaration::TANGENT;
	pElementParameter->uIndex = 0;

	pElementParameter         = &m_Attributes[ATTRIBUTE_TEXCOORD];
	pElementParameter->Type   = CVertexDeclaration::FLOAT2;
	pElementParameter->Usage  = CVertexDeclaration::TEXCOORD;
	pElementParameter->uIndex = uNormalMapUVIndex;

	pElementParameter         = &m_Varying[VARYING_TEXCOORD];
	pElementParameter->Type   = CVertexDeclaration::FLOAT2;
	pElementParameter->Usage  = CVertexDeclaration::TEXCOORD;
	pElementParameter->uIndex = uNormalMapUVIndex;

	LPSAMPLER pNormal = &m_Samplers[SAMPLER_NORMAL];
	pNormal->Declaration.Type      = CEffectCode::TEXTURE2D;
	pNormal->Declaration.AddressU  = CEffectCode::WRAP;
	pNormal->Declaration.AddressV  = CEffectCode::WRAP;
	pNormal->Declaration.AddressW  = CEffectCode::WRAP;
	pNormal->Declaration.MinFilter = CEffectCode::LINEAR;
	pNormal->Declaration.MagFilter = CEffectCode::LINEAR;
	pNormal->Declaration.MipFilter = CEffectCode::LINEAR;
	pNormal->uIndex                = uNormalMapIndex;

	m_uConstantVariableCount = (uNumLights + 2) << 1;
	ALCHEMY_DEBUG_NEW(m_pConstantVariables, CEffectCode::CONSTANTVARIABLEDECLARATION[m_uConstantVariableCount]);

	CEffectCode::LPCONSTANTVARIABLEDECLARATION pConstantVariables;
	UINT uIndex = 0, i;

	pConstantVariables              = &m_pConstantVariables[uIndex ++];
	pConstantVariables->ShaderType  = CEffectInterface::VERTEX_SHADER;
	pConstantVariables->GlobalType  = CEffectCode::CAMERA_VARIABLE;
	pConstantVariables->uDetail     = CRenderMethod::CAMERA_LOCAL_DIRECTION;
	pConstantVariables->uCount      = 1;
	pConstantVariables->uIndex      = 0;
	pConstantVariables->bIsReadOnly = false;
	pConstantVariables->bIsForce    = true;
	pConstantVariables->pValue      = ALCHEMY_NULL;

	pConstantVariables              = &m_pConstantVariables[uIndex ++];
	pConstantVariables->ShaderType  = CEffectInterface::VERTEX_SHADER;
	pConstantVariables->GlobalType  = CEffectCode::CAMERA_VARIABLE;
	pConstantVariables->uDetail     = CRenderMethod::CAMERA_LOCAL_DIRECTION_NEGATE;
	pConstantVariables->uCount      = 1;
	pConstantVariables->uIndex      = 0;
	pConstantVariables->bIsReadOnly = false;
	pConstantVariables->bIsForce    = true;
	pConstantVariables->pValue      = ALCHEMY_NULL;

	if(puLightIndices)
	{
		for(i = 0; i < uNumLights; ++ i)
		{
			pConstantVariables = &m_pConstantVariables[uIndex ++];
			pConstantVariables->ShaderType  = CEffectInterface::VERTEX_SHADER;
			pConstantVariables->GlobalType  = CEffectCode::LIGHT_VARIABLE;
			pConstantVariables->uDetail     = CRenderMethod::LIGHT_LOCAL_DIRECTION;
			pConstantVariables->uCount      = 1;
			pConstantVariables->uIndex      = puLightIndices[i];
			pConstantVariables->bIsReadOnly = false;
			pConstantVariables->bIsForce    = true;
			pConstantVariables->pValue      = ALCHEMY_NULL;
		}
	}
	else
	{
		for(i = 0; i < uNumLights; ++ i)
		{
			pConstantVariables = &m_pConstantVariables[uIndex ++];
			pConstantVariables->ShaderType  = CEffectInterface::VERTEX_SHADER;
			pConstantVariables->GlobalType  = CEffectCode::LIGHT_VARIABLE;
			pConstantVariables->uDetail     = CRenderMethod::LIGHT_LOCAL_DIRECTION;
			pConstantVariables->uCount      = 1;
			pConstantVariables->uIndex      = i;
			pConstantVariables->bIsReadOnly = false;
			pConstantVariables->bIsForce    = true;
			pConstantVariables->pValue      = ALCHEMY_NULL;
		}
	}

	pConstantVariables              = &m_pConstantVariables[uIndex ++];
	pConstantVariables->ShaderType  = CEffectInterface::FRAGMENT_SHADER;
	pConstantVariables->GlobalType  = CEffectCode::CAMERA_VARIABLE;
	pConstantVariables->uDetail     = CRenderMethod::CAMERA_LOCAL_DIRECTION;
	pConstantVariables->uCount      = 1;
	pConstantVariables->uIndex      = 0;
	pConstantVariables->bIsReadOnly = false;
	pConstantVariables->bIsForce    = false;
	pConstantVariables->pValue      = ALCHEMY_NULL;

	pConstantVariables              = &m_pConstantVariables[uIndex ++];
	pConstantVariables->ShaderType  = CEffectInterface::FRAGMENT_SHADER;
	pConstantVariables->GlobalType  = CEffectCode::CAMERA_VARIABLE;
	pConstantVariables->uDetail     = CRenderMethod::CAMERA_LOCAL_DIRECTION_NEGATE;
	pConstantVariables->uCount      = 1;
	pConstantVariables->uIndex      = 0;
	pConstantVariables->bIsReadOnly = false;
	pConstantVariables->bIsForce    = false;
	pConstantVariables->pValue      = ALCHEMY_NULL;

	if(puLightIndices)
	{
		for(i = 0; i < uNumLights; ++ i)
		{
			pConstantVariables = &m_pConstantVariables[uIndex ++];
			pConstantVariables->ShaderType  = CEffectInterface::FRAGMENT_SHADER;
			pConstantVariables->GlobalType  = CEffectCode::LIGHT_VARIABLE;
			pConstantVariables->uDetail     = CRenderMethod::LIGHT_LOCAL_DIRECTION;
			pConstantVariables->uCount      = 1;
			pConstantVariables->uIndex      = puLightIndices[i];
			pConstantVariables->bIsReadOnly = false;
			pConstantVariables->bIsForce    = false;
			pConstantVariables->pValue      = ALCHEMY_NULL;
		}
	}
	else
	{
		for(i = 0; i < uNumLights; ++ i)
		{
			pConstantVariables = &m_pConstantVariables[uIndex ++];
			pConstantVariables->ShaderType  = CEffectInterface::FRAGMENT_SHADER;
			pConstantVariables->GlobalType  = CEffectCode::LIGHT_VARIABLE;
			pConstantVariables->uDetail     = CRenderMethod::LIGHT_LOCAL_DIRECTION;
			pConstantVariables->uCount      = 1;
			pConstantVariables->uIndex      = i;
			pConstantVariables->bIsReadOnly = false;
			pConstantVariables->bIsForce    = false;
			pConstantVariables->pValue      = ALCHEMY_NULL;
		}
	}
}

CNormalMapEffect::~CNormalMapEffect(void)
{
	ALCHEMY_DEBUG_DELETE_ARRAY(m_pConstantVariables);
}

UINT                                            CNormalMapEffect::GetEffectInfoFlag     (                             )
{
	return ALCHEMY_FLAG(CEffectInfoInterface::VERTEX_SHADER_INFO) | ALCHEMY_FLAG(CEffectInfoInterface::FRAGMENT_SHADER_INFO);
}

const CEffectInfoInterface::SAMPLER           * CNormalMapEffect::GetSamplers           (UINT& uSamplerCount          )
{
	uSamplerCount = TOTAL_SAMPLERS;

	return m_Samplers;
}

const CEffectCode::ELEMENTPARAMETER            * CNormalMapEffect::GetAttributes        (UINT& uAttributeCount        )
{
	uAttributeCount = TOTAL_ATTRIBUTES;

	return m_Attributes;
}

const CEffectCode::ELEMENTPARAMETER            * CNormalMapEffect::GetVarying           (UINT& uVaryingCount          )
{
	uVaryingCount = TOTAL_VARYING;

	return m_Varying;
}

const CEffectCode::GLOBALVARIABLE              * CNormalMapEffect::GetConstants         (UINT& uConstantCount         )
{
	uConstantCount = TOTAL_CONSTANTS;

	return sm_CONSTANTS;
}

const CEffectCode::TEMPORARYDECLARATION        * CNormalMapEffect::GetTemporary         (UINT& uTemporaryCount        )
{
	uTemporaryCount = TOTAL_TEMPORARY;

	return sm_TEMPORARY;
}

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CNormalMapEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
{
	uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLES;

	return sm_ATTRIBUTE_VARIABLES;
}

const CEffectCode::CONSTANTVARIABLEDECLARATION * CNormalMapEffect::GetConstantVariables  (UINT& uConstantVariableCount)
{
	uConstantVariableCount = m_uConstantVariableCount;

	return m_pConstantVariables;
}

bool CNormalMapEffect::ShaderDefineHandler(
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
	UINT uTangent = puAttributes[ATTRIBUTE_TANGENT], uBitnormal = puTemporary[TEMPORARY_BITNORMAL], uNormal = puAttributes[ATTRIBUTE_NORAML], uValue;
	UINT uAddress[3] = {0, 1, 2};

	const CEffectCode::SHADERVARIABLE* pBinormal = &pAttributeVariables[ATTRIBUTE_VARIABLE_BINORMAL];

	if(pBinormal->uHandle)
		VertexShaderDefiner.AppendInstruction(CCompilerInterface::MOV, uBitnormal, ALCHEMY_NULL, 0, pBinormal->uHandle, ALCHEMY_NULL, 0, 0, ALCHEMY_NULL, 0);
	else
		VertexShaderDefiner.AppendInstruction(CCompilerInterface::CRS, uBitnormal, puAttributes[ATTRIBUTE_TANGENT], puAttributes[ATTRIBUTE_NORAML]);

	const CEffectCode::SHADERVARIABLE* pVariable;
	UINT uConstantVariableCount = m_uConstantVariableCount >> 1, i;
	for(i = 0; i < uConstantVariableCount; ++ i)
	{
		pVariable = &pConstantVariables[i];
		if(pVariable->uHandle)
		{
			uValue = 0;
			VertexShaderDefiner.AppendInstruction(CCompilerInterface::DP3, pVariable->uHandle, &uValue, 1, uTangent  , uAddress, 3, pVariable->uSource, uAddress, 3);
			uValue = 1;
			VertexShaderDefiner.AppendInstruction(CCompilerInterface::DP3, pVariable->uHandle, &uValue, 1, uBitnormal, uAddress, 3, pVariable->uSource, uAddress, 3);
			uValue = 2;
			VertexShaderDefiner.AppendInstruction(CCompilerInterface::DP3, pVariable->uHandle, &uValue, 1, uNormal   , uAddress, 3, pVariable->uSource, uAddress, 3);

			uValue = 3;
			VertexShaderDefiner.AppendInstruction(CCompilerInterface::MOV, pVariable->uHandle, &uValue, 1, pVariable->uSource, &uValue, 1, 0, ALCHEMY_NULL, 0);
		}
	}

	for(i = uConstantVariableCount; i < m_uConstantVariableCount; ++ i)
	{
		pVariable = &pConstantVariables[i];
		if(pVariable->uHandle && pVariable->uSource)
			FragmentShaderDefiner.AppendInstruction(CCompilerInterface::NRM, pVariable->uHandle, pVariable->uSource, 0);
	}

	pVariable = &pAttributeVariables[ATTRIBUTE_VARIABLE_NORAML];
	if(pVariable->uHandle)
	{
		const CEffectCode::VARYING* pTexcoord = &pVarying[VARYING_TEXCOORD];
		if(!pTexcoord->IsUsed)
			VertexShaderDefiner.AppendInstruction(CCompilerInterface::MOV, pTexcoord->uVertexShaderHandle, ALCHEMY_NULL, 0, puAttributes[ATTRIBUTE_TEXCOORD], ALCHEMY_NULL, 0, 0, ALCHEMY_NULL, 0);

		FragmentShaderDefiner.AppendInstruction(CCompilerInterface::TEX, pVariable->uHandle, pTexcoord->uFragmentShaderHandle, puSamplers[SAMPLER_NORMAL]);

		FragmentShaderDefiner.AppendInstruction(CCompilerInterface::SUB, pVariable->uHandle, pVariable->uHandle, puConstants[CONSTANT_HALF_ONE]);
		FragmentShaderDefiner.AppendInstruction(CCompilerInterface::DIV, pVariable->uHandle, pVariable->uHandle, puConstants[CONSTANT_HALF_ONE]);
	}

	return true;
}