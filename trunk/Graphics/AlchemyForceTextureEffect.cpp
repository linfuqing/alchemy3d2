#include "AlchemyForceTextureEffect.h"

using namespace alchemy;

CForceTextureEffect::CForceTextureEffect(UINT uSamplerIndex, UINT uTexcoordIndex, const CEffectCode::SAMPLERDECLARATION* pSamplerDeclaration)
{
	const static CVertexDeclaration::ELEMENTTYPE s_ELEMENT_TYPE[CEffectCode::TOTAL_SAMPLER_TYPE] = {CVertexDeclaration::FLOAT2, CVertexDeclaration::FLOAT2, CVertexDeclaration::FLOAT3, CVertexDeclaration::FLOAT3};

	CEffectCode::LPELEMENTPARAMETER pTexcoord = &(m_Attributes[ATTRIBUTE_TEXCOORD]);
	pTexcoord->Type   = pSamplerDeclaration ? s_ELEMENT_TYPE[pSamplerDeclaration->Type] : CVertexDeclaration::FLOAT2;
	pTexcoord->Usage  = CVertexDeclaration::TEXCOORD;
	pTexcoord->uIndex = uTexcoordIndex;

	m_Varing[VARYING_TEXCOORD] = *pTexcoord;

	LPSAMPLER pSampler = &(m_Samplers[SAMPLER_TEXTURE]);
	if(pSamplerDeclaration)
		pSampler->Declaration = *pSamplerDeclaration;
	else
	{
		pSampler->Declaration.Type      = CEffectCode::TEXTURE2D;
		pSampler->Declaration.MinFilter = CEffectCode::LINEAR;
		pSampler->Declaration.MagFilter = CEffectCode::LINEAR;
		pSampler->Declaration.MipFilter = CEffectCode::LINEAR;
		pSampler->Declaration.AddressU  = CEffectCode::WRAP;
		pSampler->Declaration.AddressV  = CEffectCode::WRAP;
		pSampler->Declaration.AddressW  = CEffectCode::WRAP;
	}

	pSampler->uIndex = uSamplerIndex;
}

CForceTextureEffect::~CForceTextureEffect(void)
{
}

UINT                                            CForceTextureEffect::GetEffectInfoFlag     (                             )
{
	return ALCHEMY_FLAG(CEffectInfoInterface::VERTEX_SHADER_INFO) | ALCHEMY_FLAG(CEffectInfoInterface::FRAGMENT_SHADER_OUTPUT);
}

const CEffectInfoInterface::SAMPLER           * CForceTextureEffect::GetSamplers           (UINT& uSamplerCount          )
{
	uSamplerCount = TOTAL_SAMPLERS;

	return m_Samplers;
}

const CEffectCode::ELEMENTPARAMETER            * CForceTextureEffect::GetAttributes        (UINT& uAttributeCount        )
{
	uAttributeCount = TOTAL_ATTRIBUTES;

	return m_Attributes;
}

const CEffectCode::ELEMENTPARAMETER            * CForceTextureEffect::GetVarying           (UINT& uVaryingCount          )
{
	uVaryingCount = TOTAL_VARYING;

	return m_Varing;
}

const CEffectCode::GLOBALVARIABLE              * CForceTextureEffect::GetConstants         (UINT& uConstantCount         )
{
	uConstantCount = TOTAL_CONSTANTS;

	return ALCHEMY_NULL;
}

const CEffectCode::TEMPORARYDECLARATION        * CForceTextureEffect::GetTemporary         (UINT& uTemporaryCount        )
{
	uTemporaryCount = TOTAL_TEMPORARY;

	return ALCHEMY_NULL;
}

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CForceTextureEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
{
	uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLES;

	return ALCHEMY_NULL;
}

const CEffectCode::CONSTANTVARIABLEDECLARATION * CForceTextureEffect::GetConstantVariables  (UINT& uConstantVariableCount)
{
	uConstantVariableCount = TOTAL_CONSTANT_VARIABLES;

	return ALCHEMY_NULL;
}

bool CForceTextureEffect::ShaderDefineHandler(
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
	const CEffectCode::VARYING* pTexcoord = &pVarying[VARYING_TEXCOORD];

	if(!pTexcoord->IsUsed)
		VertexShaderDefiner.AppendInstruction(CCompilerInterface::MOV, pTexcoord->uVertexShaderHandle, ALCHEMY_NULL, 0, puAttributes[ATTRIBUTE_TEXCOORD], ALCHEMY_NULL, 0, 0, ALCHEMY_NULL, 0);

	FragmentShaderDefiner.AppendInstruction(CCompilerInterface::TEX, uOutputColor, pTexcoord->uFragmentShaderHandle, puSamplers[SAMPLER_TEXTURE]);

	return true;
}