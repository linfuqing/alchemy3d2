#include "AlchemyNormal3Power1MapOutputEffect.h"
#include "AlchemyRenderMethod.h"

using namespace alchemy;

const CEffectCode::GLOBALVARIABLE               CNormal3Power1MapOutputEffect::sm_CONSTANS          [CNormal3Power1MapOutputEffect::TOTAL_CONSTANTS          ] = 
{ 
	{IEffect::FRAGMENT_SHADER, CEffectCode::CONSTANT_VARIABLE, CRenderMethod::CONSTANT_ONE, 1, 0, ALCHEMY_NULL}, 
	{IEffect::FRAGMENT_SHADER, CEffectCode::CONSTANT_VARIABLE, CRenderMethod::CONSTANT_TWO, 1, 0, ALCHEMY_NULL}, 
	{IEffect::FRAGMENT_SHADER, CEffectCode::CONSTANT_VARIABLE, CRenderMethod::CONSTANT_255, 1, 0, ALCHEMY_NULL}, 
};

const CEffectCode::TEMPORARYDECLARATION         CNormal3Power1MapOutputEffect::sm_TEMPORARY         [CNormal3Power1MapOutputEffect::TOTAL_TEMPORARY          ] = 
{ 
	{IEffect::FRAGMENT_SHADER, CEffectCode::VECTOR4_VARIABLE, 1} 
};

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION CNormal3Power1MapOutputEffect::sm_ATTRIBUTE_VARIABLE[CNormal3Power1MapOutputEffect::TOTAL_ATTRIBUTE_VARIABLES] = 
{ 
	{IEffect::FRAGMENT_SHADER, CVertexDeclaration::FLOAT2, CVertexDeclaration::TEXCOORD, 0, true , true },
	{IEffect::FRAGMENT_SHADER, CVertexDeclaration::FLOAT3, CVertexDeclaration::NORMAL  , 0, false, false}
};

const CEffectCode::CONSTANTVARIABLEDECLARATION CNormal3Power1MapOutputEffect::sm_CONSTANT_VARIABLE[CNormal3Power1MapOutputEffect::TOTAL_CONSTANT_VARIABLES] = 
{
	{IEffect::FRAGMENT_SHADER, CEffectCode::MATERIAL_VARIABLE, CRenderMethod::MATERIAL_POWER, 1, 0, ALCHEMY_NULL, false, false}
};

CNormal3Power1MapOutputEffect::CNormal3Power1MapOutputEffect(UINT uSamplerIndex, IEffect::SAMPLERFILTER SamplerFilter)
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

CNormal3Power1MapOutputEffect::~CNormal3Power1MapOutputEffect(void)
{
}

UINT                                            CNormal3Power1MapOutputEffect::GetEffectInfoFlag     (                             )
{
	return ALCHEMY_FLAG(IEffectInfo::FRAGMENT_SHADER_INFO);
}

const IEffectInfo::SAMPLER           * CNormal3Power1MapOutputEffect::GetSamplers           (UINT& uSamplerCount          )
{
	uSamplerCount = TOTAL_SAMPLERS;

	return m_Samplers;
}

const CEffectCode::ELEMENTDECLARATION            * CNormal3Power1MapOutputEffect::GetAttributes        (UINT& uAttributeCount        )
{
	uAttributeCount = TOTAL_ATTRIBUTES;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION            * CNormal3Power1MapOutputEffect::GetVarying           (UINT& uVaryingCount          )
{
	uVaryingCount = TOTAL_VARYING;

	return ALCHEMY_NULL;
}

const CEffectCode::GLOBALVARIABLE              * CNormal3Power1MapOutputEffect::GetConstants         (UINT& uConstantCount         )
{
	uConstantCount = TOTAL_CONSTANTS;

	return sm_CONSTANS;
}

const CEffectCode::TEMPORARYDECLARATION        * CNormal3Power1MapOutputEffect::GetTemporary         (UINT& uTemporaryCount        )
{
	uTemporaryCount = TOTAL_TEMPORARY;

	return sm_TEMPORARY;
}

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CNormal3Power1MapOutputEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
{
	uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLES;

	return sm_ATTRIBUTE_VARIABLE;
}

const CEffectCode::CONSTANTVARIABLEDECLARATION * CNormal3Power1MapOutputEffect::GetConstantVariables  (UINT& uConstantVariableCount)
{
	uConstantVariableCount = TOTAL_CONSTANT_VARIABLES;

	return sm_CONSTANT_VARIABLE;
}

bool CNormal3Power1MapOutputEffect::ShaderDefineHandler(
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
	UINT uTemp = puTemporary[TEMPORARY_TEMP];
	FragmentShaderDefiner.AppendInstruction(ICompiler::TEX, uTemp, pAttributeVariables[ATTRIBUTE_VARIABLE_TEXCOORD].uHandle, puSamplers[SAMPLER_TEXTURE]);

	const CEffectCode::SHADERVARIABLE* pNormal   = &pAttributeVariables[ATTRIBUTE_VARIABLE_NORMAL];
	if(pNormal->uHandle)
	{
		UINT uAddress[] = {0, 1, 2};

		FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, pNormal->uHandle, uAddress, 3, uTemp           , uAddress, 3, puConstants[CONSTANT_TWO], uAddress, 1);
		FragmentShaderDefiner.AppendInstruction(ICompiler::SUB, pNormal->uHandle, uAddress, 3, pNormal->uHandle, uAddress, 3, puConstants[CONSTANT_ONE], uAddress, 1);
	}

	const CEffectCode::SHADERVARIABLE* pMaterialPower   = &pConstantVariables[CONSTANT_VARIABLE_MATERIAL_POWER];
	if(pMaterialPower->uHandle)
	{
		UINT x = 0, w = 3;
		FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, pMaterialPower->uHandle, &x, 1, uTemp, &w, 1, puConstants[CONSTANT_255], &x, 1);
	}

	return true;
}