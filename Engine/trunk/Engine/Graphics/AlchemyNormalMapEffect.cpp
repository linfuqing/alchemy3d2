#include "AlchemyNormalMapEffect.h"

#include "AlchemyRenderMethod.h"

using namespace alchemy;

const CEffectCode::GLOBALVARIABLE               CNormalMapEffect::sm_CONSTANTS         [CNormalMapEffect::TOTAL_CONSTANTS         ] = 
{
	{IEffect::FRAGMENT_SHADER, CEffectCode::CONSTANT_VARIABLE, CRenderMethod::CONSTANT_HALF_ONE, 1, 0, ALCHEMY_NULL}
};

const CEffectCode::TEMPORARYDECLARATION         CNormalMapEffect::sm_TEMPORARY         [CNormalMapEffect::TOTAL_TEMPORARY         ] = 
{ 
	{IEffect::VERTEX_SHADER, CEffectCode::VECTOR3_VARIABLE, 1},
	{IEffect::VERTEX_SHADER, CEffectCode::VECTOR3_VARIABLE, 1}
};

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION CNormalMapEffect::sm_ATTRIBUTE_VARIABLES[CNormalMapEffect::TOTAL_ATTRIBUTE_VARIABLES] = 
{ 
	{IEffect::VERTEX_SHADER  , CVertexDeclaration::FLOAT3, CVertexDeclaration::BINORMAL, 0, true , false},
	{IEffect::FRAGMENT_SHADER, CVertexDeclaration::FLOAT3, CVertexDeclaration::NORMAL  , 0, false, false}
};

CNormalMapEffect::CNormalMapEffect(UINT uNormalMapUVIndex, UINT uNormalMapIndex, UINT uNumLights, PUINT puLightIndices)
{
	CEffectCode::LPELEMENTDECLARATION pElement;
	pElement         = &m_Attributes[ATTRIBUTE_NORAML];
	pElement->Type   = CVertexDeclaration::FLOAT3;
	pElement->Usage  = CVertexDeclaration::NORMAL;
	pElement->uIndex = 0;

	pElement         = &m_Attributes[ATTRIBUTE_TANGENT];
	pElement->Type   = CVertexDeclaration::FLOAT3;
	pElement->Usage  = CVertexDeclaration::TANGENT;
	pElement->uIndex = 0;

	pElement         = &m_Attributes[ATTRIBUTE_TEXCOORD];
	pElement->Type   = CVertexDeclaration::FLOAT2;
	pElement->Usage  = CVertexDeclaration::TEXCOORD;
	pElement->uIndex = uNormalMapUVIndex;

	pElement         = &m_Varying[VARYING_TEXCOORD];
	pElement->Type   = CVertexDeclaration::FLOAT2;
	pElement->Usage  = CVertexDeclaration::TEXCOORD;
	pElement->uIndex = uNormalMapUVIndex;

	LPSAMPLER pNormal = &m_Samplers[SAMPLER_NORMAL];
	pNormal->Type                  = IEffect::FRAGMENT_SHADER;
	pNormal->Declaration.Type      = IEffect::TEXTURE2D;
	pNormal->Declaration.AddressU  = IEffect::WRAP;
	pNormal->Declaration.AddressV  = IEffect::WRAP;
	pNormal->Declaration.AddressW  = IEffect::WRAP;
	pNormal->Declaration.MinFilter = IEffect::LINEAR;
	pNormal->Declaration.MagFilter = IEffect::LINEAR;
	pNormal->Declaration.MipFilter = IEffect::LINEAR;
	pNormal->uIndex                = uNormalMapIndex;

	m_uConstantVariableCount = ( (uNumLights << 1) + 2 ) << 1;
	ALCHEMY_DEBUG_NEW(m_pConstantVariables, CEffectCode::CONSTANTVARIABLEDECLARATION[m_uConstantVariableCount]);

	CEffectCode::LPCONSTANTVARIABLEDECLARATION pConstantVariables;
	UINT uIndex = 0, i;

	pConstantVariables              = &m_pConstantVariables[uIndex ++];
	pConstantVariables->ShaderType  = IEffect::VERTEX_SHADER;
	pConstantVariables->GlobalType  = CEffectCode::CAMERA_VARIABLE;
	pConstantVariables->uDetail     = CRenderMethod::CAMERA_LOCAL_DIRECTION;
	pConstantVariables->uCount      = 1;
	pConstantVariables->uIndex      = 0;
	pConstantVariables->bIsReadOnly = false;
	pConstantVariables->bIsForce    = true;
	pConstantVariables->pValue      = ALCHEMY_NULL;

	pConstantVariables              = &m_pConstantVariables[uIndex ++];
	pConstantVariables->ShaderType  = IEffect::VERTEX_SHADER;
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
			pConstantVariables->ShaderType  = IEffect::VERTEX_SHADER;
			pConstantVariables->GlobalType  = CEffectCode::LIGHT_VARIABLE;
			pConstantVariables->uDetail     = CRenderMethod::LIGHT_LOCAL_DIRECTION;
			pConstantVariables->uCount      = 1;
			pConstantVariables->uIndex      = puLightIndices[i];
			pConstantVariables->bIsReadOnly = false;
			pConstantVariables->bIsForce    = true;
			pConstantVariables->pValue      = ALCHEMY_NULL;

			pConstantVariables = &m_pConstantVariables[uIndex ++];
			pConstantVariables->ShaderType  = IEffect::VERTEX_SHADER;
			pConstantVariables->GlobalType  = CEffectCode::LIGHT_VARIABLE;
			pConstantVariables->uDetail     = CRenderMethod::LIGHT_LOCAL_DIRECTION_NEGATE;
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
			pConstantVariables->ShaderType  = IEffect::VERTEX_SHADER;
			pConstantVariables->GlobalType  = CEffectCode::LIGHT_VARIABLE;
			pConstantVariables->uDetail     = CRenderMethod::LIGHT_LOCAL_DIRECTION;
			pConstantVariables->uCount      = 1;
			pConstantVariables->uIndex      = i;
			pConstantVariables->bIsReadOnly = false;
			pConstantVariables->bIsForce    = true;
			pConstantVariables->pValue      = ALCHEMY_NULL;

			pConstantVariables = &m_pConstantVariables[uIndex ++];
			pConstantVariables->ShaderType  = IEffect::VERTEX_SHADER;
			pConstantVariables->GlobalType  = CEffectCode::LIGHT_VARIABLE;
			pConstantVariables->uDetail     = CRenderMethod::LIGHT_LOCAL_DIRECTION_NEGATE;
			pConstantVariables->uCount      = 1;
			pConstantVariables->uIndex      = i;
			pConstantVariables->bIsReadOnly = false;
			pConstantVariables->bIsForce    = true;
			pConstantVariables->pValue      = ALCHEMY_NULL;
		}
	}

	pConstantVariables              = &m_pConstantVariables[uIndex ++];
	pConstantVariables->ShaderType  = IEffect::FRAGMENT_SHADER;
	pConstantVariables->GlobalType  = CEffectCode::CAMERA_VARIABLE;
	pConstantVariables->uDetail     = CRenderMethod::CAMERA_LOCAL_DIRECTION;
	pConstantVariables->uCount      = 1;
	pConstantVariables->uIndex      = 0;
	pConstantVariables->bIsReadOnly = false;
	pConstantVariables->bIsForce    = false;
	pConstantVariables->pValue      = ALCHEMY_NULL;

	pConstantVariables              = &m_pConstantVariables[uIndex ++];
	pConstantVariables->ShaderType  = IEffect::FRAGMENT_SHADER;
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
			pConstantVariables->ShaderType  = IEffect::FRAGMENT_SHADER;
			pConstantVariables->GlobalType  = CEffectCode::LIGHT_VARIABLE;
			pConstantVariables->uDetail     = CRenderMethod::LIGHT_LOCAL_DIRECTION;
			pConstantVariables->uCount      = 1;
			pConstantVariables->uIndex      = puLightIndices[i];
			pConstantVariables->bIsReadOnly = false;
			pConstantVariables->bIsForce    = false;
			pConstantVariables->pValue      = ALCHEMY_NULL;

			pConstantVariables = &m_pConstantVariables[uIndex ++];
			pConstantVariables->ShaderType  = IEffect::FRAGMENT_SHADER;
			pConstantVariables->GlobalType  = CEffectCode::LIGHT_VARIABLE;
			pConstantVariables->uDetail     = CRenderMethod::LIGHT_LOCAL_DIRECTION_NEGATE;
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
			pConstantVariables->ShaderType  = IEffect::FRAGMENT_SHADER;
			pConstantVariables->GlobalType  = CEffectCode::LIGHT_VARIABLE;
			pConstantVariables->uDetail     = CRenderMethod::LIGHT_LOCAL_DIRECTION;
			pConstantVariables->uCount      = 1;
			pConstantVariables->uIndex      = i;
			pConstantVariables->bIsReadOnly = false;
			pConstantVariables->bIsForce    = false;
			pConstantVariables->pValue      = ALCHEMY_NULL;

			pConstantVariables = &m_pConstantVariables[uIndex ++];
			pConstantVariables->ShaderType  = IEffect::FRAGMENT_SHADER;
			pConstantVariables->GlobalType  = CEffectCode::LIGHT_VARIABLE;
			pConstantVariables->uDetail     = CRenderMethod::LIGHT_LOCAL_DIRECTION_NEGATE;
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
	return ALCHEMY_FLAG(IEffectInfo::VERTEX_SHADER_INFO) | ALCHEMY_FLAG(IEffectInfo::FRAGMENT_SHADER_INFO);
}

const IEffectInfo::SAMPLER           * CNormalMapEffect::GetSamplers           (UINT& uSamplerCount          )
{
	uSamplerCount = TOTAL_SAMPLERS;

	return m_Samplers;
}

const CEffectCode::ELEMENTDECLARATION            * CNormalMapEffect::GetAttributes        (UINT& uAttributeCount        )
{
	uAttributeCount = TOTAL_ATTRIBUTES;

	return m_Attributes;
}

const CEffectCode::ELEMENTDECLARATION            * CNormalMapEffect::GetVarying           (UINT& uVaryingCount          )
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
	CEffectCode::IInstructionSet& VertexShaderDefiner,
	CEffectCode::IInstructionSet& FragmentShaderDefiner,
	CEffectCode::CPassDefiner::CEffectCodeUtils& EffectCodeUtils)
{
	UINT uTangent = puAttributes[ATTRIBUTE_TANGENT], uBitnormal = puTemporary[TEMPORARY_BITNORMAL], uNormal = puAttributes[ATTRIBUTE_NORAML], uSourceTemp = puTemporary[TEMPORARY_SOURCE], uValue, uSource;
	UINT uAddress[3] = {0, 1, 2};

	const CEffectCode::SHADERVARIABLE* pBinormal = &pAttributeVariables[ATTRIBUTE_VARIABLE_BINORMAL];

	if(pBinormal->uHandle)
		VertexShaderDefiner.AppendInstruction(ICompiler::MOV, uBitnormal, ALCHEMY_NULL, 0, pBinormal->uHandle, ALCHEMY_NULL, 0, 0, ALCHEMY_NULL, 0);
	else
		VertexShaderDefiner.AppendInstruction(ICompiler::CRS, uBitnormal, puAttributes[ATTRIBUTE_TANGENT], puAttributes[ATTRIBUTE_NORAML]);

	const CEffectCode::SHADERVARIABLE* pVariable;
	UINT uConstantVariableCount = m_uConstantVariableCount >> 1, i;
	for(i = 0; i < uConstantVariableCount; ++ i)
	{
		pVariable = &pConstantVariables[i];
		if(pVariable->uHandle)
		{
			uSource = pVariable->uSource;
			if(pVariable->bIsUsed)
			{
				uSource = uSourceTemp;
				VertexShaderDefiner.AppendInstruction(ICompiler::MOV, uSource, uAddress, 3, pVariable->uHandle, uAddress, 3, 0, ALCHEMY_NULL, 0);
			}

			uValue = 0;
			VertexShaderDefiner.AppendInstruction(ICompiler::DP3, pVariable->uHandle, &uValue, 1, uTangent  , uAddress, 3, uSource, uAddress, 3);
			uValue = 1;
			VertexShaderDefiner.AppendInstruction(ICompiler::DP3, pVariable->uHandle, &uValue, 1, uBitnormal, uAddress, 3, uSource, uAddress, 3);
			uValue = 2;
			VertexShaderDefiner.AppendInstruction(ICompiler::DP3, pVariable->uHandle, &uValue, 1, uNormal   , uAddress, 3, uSource, uAddress, 3);
		}
	}

	for(i = uConstantVariableCount; i < m_uConstantVariableCount; ++ i)
	{
		pVariable = &pConstantVariables[i];
		if(pVariable->uHandle && pVariable->uSource)
			FragmentShaderDefiner.AppendInstruction(ICompiler::NRM, pVariable->uHandle, pVariable->uSource, 0);
	}

	pVariable = &pAttributeVariables[ATTRIBUTE_VARIABLE_NORAML];
	if(pVariable->uHandle)
	{
		const CEffectCode::VARYING* pTexcoord = &pVarying[VARYING_TEXCOORD];
		if(!pTexcoord->IsUsed)
			VertexShaderDefiner.AppendInstruction(ICompiler::MOV, pTexcoord->uVertexShaderHandle, ALCHEMY_NULL, 0, puAttributes[ATTRIBUTE_TEXCOORD], ALCHEMY_NULL, 0, 0, ALCHEMY_NULL, 0);

		FragmentShaderDefiner.AppendInstruction(ICompiler::TEX, pVariable->uHandle, pTexcoord->uFragmentShaderHandle, puSamplers[SAMPLER_NORMAL]);

		FragmentShaderDefiner.AppendInstruction(ICompiler::SUB, pVariable->uHandle, pVariable->uHandle, puConstants[CONSTANT_HALF_ONE]);
		FragmentShaderDefiner.AppendInstruction(ICompiler::DIV, pVariable->uHandle, pVariable->uHandle, puConstants[CONSTANT_HALF_ONE]);
	}

	return true;
}