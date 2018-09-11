#include "AlchemySpecularTextureEffect.h"
#include "AlchemyRenderMethod.h"

using namespace alchemy;

CEffectCode::TEMPORARYDECLARATION			CSpecularTextureEffect::sm_TEMPORARY[TOTAL_TEMPORARY]					= { {IEffect::FRAGMENT_SHADER, CEffectCode::VECTOR1_VARIABLE, 1} };
CEffectCode::CONSTANTVARIABLEDECLARATION	CSpecularTextureEffect::sm_CONSTANTVARIABLES[TOTAL_CONSTANT_VARIABLES]	= { {IEffect::FRAGMENT_SHADER, CEffectCode::MATERIAL_VARIABLE, CRenderMethod::MATERIAL_SPECULAR, 1, 0, ALCHEMY_NULL, false, true} };

CSpecularTextureEffect::CSpecularTextureEffect(UINT uChannelIndex, UINT uSamplerIndex, UINT uTexcoordIndex, const IEffect::SAMPLERDECLARATION* pSamplerDeclaration)
:m_uChanndelIndex(uChannelIndex)
{
	CEffectCode::LPATTRIBUTEVARIABLEDECLARATION pTexcoord = &(m_AttributeVariables[ATTRIBUTE_VARIABLE_TEXCOORD]);
	pTexcoord->ShaderType   = IEffect::FRAGMENT_SHADER;
	pTexcoord->ElementType  = CVertexDeclaration::FLOAT2;
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

CSpecularTextureEffect::~CSpecularTextureEffect(void)
{
}

UINT                                            CSpecularTextureEffect::GetEffectInfoFlag     (                             )
{
	return ALCHEMY_FLAG(IEffectInfo::FRAGMENT_SHADER_INFO);
}

const IEffectInfo::SAMPLER           * CSpecularTextureEffect::GetSamplers           (UINT& uSamplerCount          )
{
	uSamplerCount = TOTAL_SAMPLERS;

	return m_Samplers;
}

const CEffectCode::ELEMENTDECLARATION            * CSpecularTextureEffect::GetAttributes        (UINT& uAttributeCount        )
{
	uAttributeCount = TOTAL_ATTRIBUTES;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION            * CSpecularTextureEffect::GetVarying           (UINT& uVaryingCount          )
{
	uVaryingCount = TOTAL_VARYING;

	return ALCHEMY_NULL;
}

const CEffectCode::GLOBALVARIABLE              * CSpecularTextureEffect::GetConstants         (UINT& uConstantCount         )
{
	uConstantCount = TOTAL_CONSTANTS;

	return ALCHEMY_NULL;
}

const CEffectCode::TEMPORARYDECLARATION        * CSpecularTextureEffect::GetTemporary         (UINT& uTemporaryCount        )
{
	uTemporaryCount = TOTAL_TEMPORARY;

	return sm_TEMPORARY;
}

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CSpecularTextureEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
{
	uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLES;

	return m_AttributeVariables;
}

const CEffectCode::CONSTANTVARIABLEDECLARATION * CSpecularTextureEffect::GetConstantVariables  (UINT& uConstantVariableCount)
{
	uConstantVariableCount = TOTAL_CONSTANT_VARIABLES;

	return sm_CONSTANTVARIABLES;
}

bool CSpecularTextureEffect::ShaderDefineHandler(
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
	if( pConstantVariables[CONSTANT_VARIABLE_SPECULAR].uHandle )
	{
		UINT uSubAddress[] = {0, 1, 2, 3};

		UINT uHandle = pConstantVariables[CONSTANT_VARIABLE_SPECULAR].bIsUsed ? pConstantVariables[CONSTANT_VARIABLE_SPECULAR].uHandle : pConstantVariables[CONSTANT_VARIABLE_SPECULAR].uSource;

		FragmentShaderDefiner.AppendInstruction(ICompiler::TEX, puTemporary[TEMPORARY_COLOR], &uSubAddress[m_uChanndelIndex],1, pAttributeVariables[ATTRIBUTE_VARIABLE_TEXCOORD].uHandle, ALCHEMY_NULL, 0,
			puSamplers[SAMPLER_TEXTURE], ALCHEMY_NULL, 0);

		FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, pConstantVariables[CONSTANT_VARIABLE_SPECULAR].uHandle, ALCHEMY_NULL, 0, uHandle, ALCHEMY_NULL, 0, puTemporary[TEMPORARY_COLOR], &uSubAddress[m_uChanndelIndex],1);
	}

	return true;
}