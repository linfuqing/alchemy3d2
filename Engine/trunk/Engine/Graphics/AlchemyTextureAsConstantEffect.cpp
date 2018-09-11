#include "AlchemyTextureAsConstantEffect.h"
#include "AlchemyRenderMethod.h"

using namespace alchemy;

CTextureAsConstantEffect::CTextureAsConstantEffect(CEffectCode::GLOBALVARIABLETYPE Type, UINT uDetail, UINT uSamplerIndex, UINT uTexcoordIndex, const IEffect::SAMPLERDECLARATION* pSamplerDeclaration)
{
	const static CVertexDeclaration::ELEMENTTYPE s_ELEMENT_TYPE[IEffect::TOTAL_SAMPLER_TYPE] = {CVertexDeclaration::FLOAT2, CVertexDeclaration::FLOAT3, CVertexDeclaration::FLOAT3};

	CEffectCode::LPATTRIBUTEVARIABLEDECLARATION pAttributeVarialbe = &(m_AttributeVariables[ATTRIBUTE_VARIABLE_TEXCOORD]);
	pAttributeVarialbe->ShaderType   = IEffect::FRAGMENT_SHADER;
	pAttributeVarialbe->ElementType  = pSamplerDeclaration ? s_ELEMENT_TYPE[pSamplerDeclaration->Type] : CVertexDeclaration::FLOAT2;
	pAttributeVarialbe->ElementUsage = CVertexDeclaration::TEXCOORD;
	pAttributeVarialbe->uIndex       = uTexcoordIndex;
	pAttributeVarialbe->bIsReadOnly  = true;
	pAttributeVarialbe->bIsForce     = true;

	CEffectCode::LPCONSTANTVARIABLEDECLARATION pConstantVariable = &(m_ConstantVariables[CONSTANT_VARIABLE_OUTPUT]);
	pConstantVariable->ShaderType  = IEffect::FRAGMENT_SHADER;
	pConstantVariable->GlobalType  = Type;
	pConstantVariable->uDetail     = uDetail;
	pConstantVariable->uCount      = 1;
	pConstantVariable->uIndex      = 0;
	pConstantVariable->pValue      = ALCHEMY_NULL;
	pConstantVariable->bIsReadOnly = false;
	pConstantVariable->bIsForce    = false;

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

CTextureAsConstantEffect::~CTextureAsConstantEffect(void)
{
}

UINT                                            CTextureAsConstantEffect::GetEffectInfoFlag     (                             )
{
	return ALCHEMY_FLAG(IEffectInfo::FRAGMENT_SHADER_INFO);
}

const IEffectInfo::SAMPLER           * CTextureAsConstantEffect::GetSamplers           (UINT& uSamplerCount          )
{
	uSamplerCount = TOTAL_SAMPLERS;

	return m_Samplers;
}

const CEffectCode::ELEMENTDECLARATION            * CTextureAsConstantEffect::GetAttributes        (UINT& uAttributeCount        )
{
	uAttributeCount = TOTAL_ATTRIBUTES;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION            * CTextureAsConstantEffect::GetVarying           (UINT& uVaryingCount          )
{
	uVaryingCount = TOTAL_VARYING;

	return ALCHEMY_NULL;
}

const CEffectCode::GLOBALVARIABLE              * CTextureAsConstantEffect::GetConstants         (UINT& uConstantCount         )
{
	uConstantCount = TOTAL_CONSTANTS;

	return ALCHEMY_NULL;
}

const CEffectCode::TEMPORARYDECLARATION        * CTextureAsConstantEffect::GetTemporary         (UINT& uTemporaryCount        )
{
	uTemporaryCount = TOTAL_TEMPORARY;

	return ALCHEMY_NULL;
}

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CTextureAsConstantEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
{
	uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLES;

	return m_AttributeVariables;
}

const CEffectCode::CONSTANTVARIABLEDECLARATION * CTextureAsConstantEffect::GetConstantVariables  (UINT& uConstantVariableCount)
{
	uConstantVariableCount = TOTAL_CONSTANT_VARIABLES;

	return m_ConstantVariables;
}

bool CTextureAsConstantEffect::ShaderDefineHandler(
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
	UINT uOutput = pConstantVariables[CONSTANT_VARIABLE_OUTPUT].uHandle;

	if(uOutput)
		FragmentShaderDefiner.AppendInstruction(ICompiler::TEX, uOutput, pAttributeVariables[ATTRIBUTE_VARIABLE_TEXCOORD].uHandle, puSamplers[SAMPLER_TEXTURE]);

	return true;
}