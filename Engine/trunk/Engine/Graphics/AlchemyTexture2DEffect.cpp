#include "AlchemyTexture2DEffect.h"
#include "AlchemyRenderMethod.h"

using namespace alchemy;

const CEffectCode::ELEMENTDECLARATION           CTexture2DEffect::sm_VARYING           [CTexture2DEffect::TOTAL_VARYING            ] = 
{ 
	{CVertexDeclaration::FLOAT4, CVertexDeclaration::TEXCOORD, 0} 
};

const CEffectCode::GLOBALVARIABLE               CTexture2DEffect::sm_CONSTANS          [CTexture2DEffect::TOTAL_CONSTANTS          ] = 
{ 
	{IEffect::VERTEX_SHADER, CEffectCode::CONSTANT_VARIABLE, CRenderMethod::CONSTANT_HALF_ONE, 1, 0, ALCHEMY_NULL} 
};

const CEffectCode::TEMPORARYDECLARATION         CTexture2DEffect::sm_TEMPORARY         [CTexture2DEffect::TOTAL_TEMPORARY          ] = 
{ 
	{IEffect::VERTEX_SHADER, CEffectCode::VECTOR4_VARIABLE, 1}, 
	{IEffect::FRAGMENT_SHADER, CEffectCode::VECTOR2_VARIABLE, 1} 
};

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION CTexture2DEffect::sm_ATTRIBUTE_VARIABLE[CTexture2DEffect::TOTAL_ATTRIBUTE_VARIABLES] = 
{ 
	{IEffect::VERTEX_SHADER, CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITIONT, 0, true, true}, 
	{IEffect::FRAGMENT_SHADER, CVertexDeclaration::FLOAT2, CVertexDeclaration::TEXCOORD, 0, false, false} 
};

CTexture2DEffect::CTexture2DEffect(bool bIsOutputTexture, UINT uSamplerIndex, IEffect::SAMPLERFILTER SamplerFilter) :
m_bIsOutputTexture(bIsOutputTexture)
{
	LPSAMPLER pSampler = &m_Samplers[SAMPLER_TEXTURE];
	pSampler->Type                  = IEffect::FRAGMENT_SHADER;
	pSampler->Declaration.Type      = IEffect::TEXTURE2D;
	pSampler->Declaration.MinFilter = SamplerFilter;
	pSampler->Declaration.MagFilter = SamplerFilter;
	pSampler->Declaration.MipFilter = IEffect::NONE;
	pSampler->Declaration.AddressU  = IEffect::CLAMP;
	pSampler->Declaration.AddressV  = IEffect::CLAMP;
	pSampler->Declaration.AddressW  = IEffect::CLAMP;
	pSampler->uIndex                = uSamplerIndex;
}

CTexture2DEffect::~CTexture2DEffect(void)
{
}

UINT                                            CTexture2DEffect::GetEffectInfoFlag     (                             )
{
	return ALCHEMY_FLAG(IEffectInfo::FRAGMENT_SHADER_OUTPUT) | ALCHEMY_FLAG(IEffectInfo::FRAGMENT_SHADER_INFO);
}

const IEffectInfo::SAMPLER           * CTexture2DEffect::GetSamplers           (UINT& uSamplerCount          )
{
	if(m_bIsOutputTexture)
	{
		uSamplerCount = TOTAL_SAMPLERS;

		return m_Samplers;
	}

	uSamplerCount = 0;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION            * CTexture2DEffect::GetAttributes        (UINT& uAttributeCount        )
{
	uAttributeCount = TOTAL_ATTRIBUTES;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION            * CTexture2DEffect::GetVarying           (UINT& uVaryingCount          )
{
	uVaryingCount = TOTAL_VARYING;

	return sm_VARYING;
}

const CEffectCode::GLOBALVARIABLE              * CTexture2DEffect::GetConstants         (UINT& uConstantCount         )
{
	uConstantCount = TOTAL_CONSTANTS;

	return sm_CONSTANS;
}

const CEffectCode::TEMPORARYDECLARATION        * CTexture2DEffect::GetTemporary         (UINT& uTemporaryCount        )
{
	uTemporaryCount = TOTAL_TEMPORARY;

	return sm_TEMPORARY;
}

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CTexture2DEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
{
	uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLES;

	return sm_ATTRIBUTE_VARIABLE;
}

const CEffectCode::CONSTANTVARIABLEDECLARATION * CTexture2DEffect::GetConstantVariables  (UINT& uConstantVariableCount)
{
	uConstantVariableCount = TOTAL_CONSTANT_VARIABLES;

	return ALCHEMY_NULL;
}

bool CTexture2DEffect::ShaderDefineHandler(
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
	const CEffectCode::VARYING* pTexcoord = &pVarying[VARYING_TEXCOORD];
	if(!pTexcoord->IsUsed)
	{
		static const UINT s_uADDRESS[] = {0, 1, 2, 3};

		UINT uTemp = puTemporary[TEMPORARY_VERTEX_TEXCOORD];

		VertexShaderDefiner.AppendInstruction(ICompiler::MOV, uTemp, pAttributeVariables[ATTRIBUTE_VARIABLE_POSITIONT].uHandle, 0);

		VertexShaderDefiner.AppendInstruction(ICompiler::NEG, uTemp, &(s_uADDRESS[1]), 1, uTemp, &(s_uADDRESS[1]), 1, 0, ALCHEMY_NULL, 0);
		VertexShaderDefiner.AppendInstruction(ICompiler::ADD, uTemp, s_uADDRESS, 2, uTemp, s_uADDRESS, 2, uTemp, &(s_uADDRESS[3]), 1);
		VertexShaderDefiner.AppendInstruction(ICompiler::MUL, uTemp, s_uADDRESS, 2, uTemp, s_uADDRESS, 2, puConstants[CONSTANT_HALF_ONE], s_uADDRESS, 1);

		VertexShaderDefiner.AppendInstruction(ICompiler::MOV, pTexcoord->uVertexShaderHandle, uTemp, 0);

		const CEffectCode::SHADERVARIABLE* pOutputTexcoord = &pAttributeVariables[ATTRIBUTE_VARIABLE_TEXCOORD];

		uTemp = pOutputTexcoord->uHandle ? pOutputTexcoord->uHandle : puTemporary[TEMPORARY_FRAGMENT_TEXCOORD];
		FragmentShaderDefiner.AppendInstruction(ICompiler::DIV, uTemp, s_uADDRESS, 2, pTexcoord->uFragmentShaderHandle, s_uADDRESS, 2, pTexcoord->uFragmentShaderHandle, &(s_uADDRESS[3]), 1);

		if(m_bIsOutputTexture)
		{
			UINT uSampler = puSamplers[SAMPLER_TEXTURE];
			FragmentShaderDefiner.AppendInstruction(ICompiler::TEX, uOutputColor, uTemp, uSampler);
		}
	}

	return true;
}