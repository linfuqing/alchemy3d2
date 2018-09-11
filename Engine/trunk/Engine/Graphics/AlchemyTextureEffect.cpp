#include "AlchemyTextureEffect.h"
#include "AlchemyRenderMethod.h"

using namespace alchemy;

CTextureEffect::CTextureEffect(UINT uSamplerIndex, UINT uTexcoordIndex, const IEffect::SAMPLERDECLARATION* pSamplerDeclaration)
{
	const static CVertexDeclaration::ELEMENTTYPE s_ELEMENT_TYPE[IEffect::TOTAL_SAMPLER_TYPE] = {CVertexDeclaration::FLOAT2, CVertexDeclaration::FLOAT3, CVertexDeclaration::FLOAT3};

	CEffectCode::LPATTRIBUTEVARIABLEDECLARATION pTexcoord = &(m_AttributeVariables[ATTRIBUTE_VARIABLE_TEXCOORD]);
	pTexcoord->ShaderType   = IEffect::FRAGMENT_SHADER;
	pTexcoord->ElementType  = pSamplerDeclaration ? s_ELEMENT_TYPE[pSamplerDeclaration->Type] : CVertexDeclaration::FLOAT2;
	pTexcoord->ElementUsage = CVertexDeclaration::TEXCOORD;
	pTexcoord->uIndex       = uTexcoordIndex;
	pTexcoord->bIsReadOnly  = true;
	pTexcoord->bIsForce     = true;

	LPSAMPLER pSampler = &(m_Samplers[SAMPLER_TEXTURE]);
	pSampler->Type = IEffect::FRAGMENT_SHADER;
	if(pSamplerDeclaration)
		pSampler->Declaration = *pSamplerDeclaration;
	else
	{
		pSampler->Declaration.Type      = IEffect::TEXTURE2D;
		pSampler->Declaration.MinFilter = IEffect::LINEAR;
		pSampler->Declaration.MagFilter = IEffect::LINEAR;
		pSampler->Declaration.MipFilter = IEffect::LINEAR;
		pSampler->Declaration.AddressU  = IEffect::WRAP;
		pSampler->Declaration.AddressV  = IEffect::WRAP;
		pSampler->Declaration.AddressW  = IEffect::WRAP;
	}

	pSampler->uIndex = uSamplerIndex;
}

CTextureEffect::~CTextureEffect(void)
{
}

UINT                                            CTextureEffect::GetEffectInfoFlag     (                             )
{
	return ALCHEMY_FLAG(IEffectInfo::FRAGMENT_SHADER_OUTPUT);
}

const IEffectInfo::SAMPLER           * CTextureEffect::GetSamplers           (UINT& uSamplerCount          )
{
	uSamplerCount = TOTAL_SAMPLERS;

	return m_Samplers;
}

const CEffectCode::ELEMENTDECLARATION            * CTextureEffect::GetAttributes        (UINT& uAttributeCount        )
{
	uAttributeCount = TOTAL_ATTRIBUTES;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION            * CTextureEffect::GetVarying           (UINT& uVaryingCount          )
{
	uVaryingCount = TOTAL_VARYING;

	return ALCHEMY_NULL;
}

const CEffectCode::GLOBALVARIABLE              * CTextureEffect::GetConstants         (UINT& uConstantCount         )
{
	uConstantCount = TOTAL_CONSTANTS;

	return ALCHEMY_NULL;
}

const CEffectCode::TEMPORARYDECLARATION        * CTextureEffect::GetTemporary         (UINT& uTemporaryCount        )
{
	uTemporaryCount = TOTAL_TEMPORARY;

	return ALCHEMY_NULL;
}

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CTextureEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
{
	uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLES;

	return m_AttributeVariables;
}

const CEffectCode::CONSTANTVARIABLEDECLARATION * CTextureEffect::GetConstantVariables  (UINT& uConstantVariableCount)
{
	uConstantVariableCount = TOTAL_CONSTANT_VARIABLES;

	return ALCHEMY_NULL;
}

bool CTextureEffect::ShaderDefineHandler(
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
	FragmentShaderDefiner.AppendInstruction(ICompiler::TEX, uOutputColor, pAttributeVariables[ATTRIBUTE_VARIABLE_TEXCOORD].uHandle, puSamplers[SAMPLER_TEXTURE]);

	return true;
}