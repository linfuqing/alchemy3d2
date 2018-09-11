#include "AlchemyNormalOffsetForceTextureEffect.h"
#include "AlchemyRenderMethod.h"

using namespace alchemy;

const CEffectCode::TEMPORARYDECLARATION CNormalOffsetForceTextureEffect::sm_TEMPORARY[CNormalOffsetForceTextureEffect::TOTAL_TEMPORARY] = { {IEffect::FRAGMENT_SHADER, CEffectCode::VECTOR2_VARIABLE, 1} };

CNormalOffsetForceTextureEffect::CNormalOffsetForceTextureEffect(FLOAT& fScale, UINT uUVIndex, UINT uSamplerIndex)
{
	LPSAMPLER pSampler = &m_Samplers[SAMPLER_TEXTURE];
	pSampler->Type                  = IEffect::FRAGMENT_SHADER;
	pSampler->Declaration.Type      = IEffect::TEXTURE2D;
	pSampler->Declaration.MinFilter = IEffect::LINEAR;
	pSampler->Declaration.MagFilter = IEffect::LINEAR;
	pSampler->Declaration.MipFilter = IEffect::NONE;
	pSampler->Declaration.AddressU  = IEffect::CLAMP;
	pSampler->Declaration.AddressV  = IEffect::CLAMP;
	pSampler->Declaration.AddressW  = IEffect::CLAMP;
	pSampler->uIndex                = uSamplerIndex;

	CEffectCode::LPGLOBALVARIABLE pConstant = &m_Constants[CONSTANT_SCALE];
	pConstant->ShaderType = IEffect::FRAGMENT_SHADER;
	pConstant->GlobalType = CEffectCode::INPUT_VARIABLE;
	pConstant->uDetail    = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR1_VARIABLE, 0, 0, false);
	pConstant->uCount     = 1;
	pConstant->uIndex     = 0;
	pConstant->pValue     = &fScale;

	CEffectCode::LPATTRIBUTEVARIABLEDECLARATION pAttributeVariable;

	pAttributeVariable = &m_AttributeVariables[ATTRIBUTE_VARIABLE_TEXCOORD];
	pAttributeVariable->ShaderType   = IEffect::FRAGMENT_SHADER;
	pAttributeVariable->ElementType  = CVertexDeclaration::FLOAT2;
	pAttributeVariable->ElementUsage = CVertexDeclaration::TEXCOORD;
	pAttributeVariable->uIndex       = uUVIndex;
	pAttributeVariable->bIsReadOnly  = true;
	pAttributeVariable->bIsForce     = true;

	pAttributeVariable = &m_AttributeVariables[ATTRIBUTE_VARIABLE_NORMAL];
	pAttributeVariable->ShaderType   = IEffect::FRAGMENT_SHADER;
	pAttributeVariable->ElementType  = CVertexDeclaration::FLOAT3;
	pAttributeVariable->ElementUsage = CVertexDeclaration::NORMAL;
	pAttributeVariable->uIndex       = 0;
	pAttributeVariable->bIsReadOnly  = true;
	pAttributeVariable->bIsForce     = true;
}

CNormalOffsetForceTextureEffect::~CNormalOffsetForceTextureEffect(void)
{
}

UINT                                            CNormalOffsetForceTextureEffect::GetEffectInfoFlag     (                             )
{
	return ALCHEMY_FLAG(IEffectInfo::FRAGMENT_SHADER_OUTPUT);
}

const IEffectInfo::SAMPLER           * CNormalOffsetForceTextureEffect::GetSamplers           (UINT& uSamplerCount          )
{
	uSamplerCount = TOTAL_SAMPLERS;

	return m_Samplers;
}

const CEffectCode::ELEMENTDECLARATION            * CNormalOffsetForceTextureEffect::GetAttributes        (UINT& uAttributeCount        )
{
	uAttributeCount = TOTAL_ATTRIBUTES;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION            * CNormalOffsetForceTextureEffect::GetVarying           (UINT& uVaryingCount          )
{
	uVaryingCount = TOTAL_VARYING;

	return ALCHEMY_NULL;
}

const CEffectCode::GLOBALVARIABLE              * CNormalOffsetForceTextureEffect::GetConstants         (UINT& uConstantCount         )
{
	uConstantCount = TOTAL_CONSTANTS;

	return m_Constants;
}

const CEffectCode::TEMPORARYDECLARATION        * CNormalOffsetForceTextureEffect::GetTemporary         (UINT& uTemporaryCount        )
{
	uTemporaryCount = TOTAL_TEMPORARY;

	return sm_TEMPORARY;
}

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CNormalOffsetForceTextureEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
{
	uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLES;

	return m_AttributeVariables;
}

const CEffectCode::CONSTANTVARIABLEDECLARATION * CNormalOffsetForceTextureEffect::GetConstantVariables  (UINT& uConstantVariableCount)
{
	uConstantVariableCount = TOTAL_CONSTANT_VARIABLES;

	return ALCHEMY_NULL;
}

bool CNormalOffsetForceTextureEffect::ShaderDefineHandler(
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
	UINT uTexcoord = puTemporary[TEMPORARY_TEXCOORD];

	UINT uAddress[] = {0, 1};
	FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uTexcoord, uAddress, 2, pAttributeVariables[ATTRIBUTE_VARIABLE_NORMAL].uHandle, uAddress, 2, puConstants[CONSTANT_SCALE], uAddress, 1);
	FragmentShaderDefiner.AppendInstruction(ICompiler::ADD, uTexcoord, uTexcoord, pAttributeVariables[ATTRIBUTE_VARIABLE_TEXCOORD].uHandle);
	FragmentShaderDefiner.AppendInstruction(ICompiler::TEX, uOutputColor, uTexcoord, puSamplers[SAMPLER_TEXTURE]);

	return true;
}