#include "AlchemyNormalOffsetTextureEffect.h"
#include "AlchemyRenderMethod.h"

using namespace alchemy;

const CEffectCode::TEMPORARYDECLARATION CNormalOffsetTextureEffect::sm_TEMPORARY[CNormalOffsetTextureEffect::TOTAL_TEMPORARY] = { {CEffectInterface::FRAGMENT_SHADER, CEffectCode::VECTOR2_VARIABLE, 1} };

CNormalOffsetTextureEffect::CNormalOffsetTextureEffect(FLOAT& fScale, UINT uUVIndex, UINT uSamplerIndex)
{
	LPSAMPLER pSampler = &m_Samplers[SAMPLER_TEXTURE];
	pSampler->Declaration.Type      = CEffectCode::TEXTURE2D;
	pSampler->Declaration.MinFilter = CEffectCode::LINEAR;
	pSampler->Declaration.MagFilter = CEffectCode::LINEAR;
	pSampler->Declaration.MipFilter = CEffectCode::LINEAR;
	pSampler->Declaration.AddressU  = CEffectCode::CLAMP;
	pSampler->Declaration.AddressV  = CEffectCode::CLAMP;
	pSampler->Declaration.AddressW  = CEffectCode::CLAMP;
	pSampler->uIndex                = uSamplerIndex;

	CEffectCode::LPGLOBALVARIABLE pConstant = &m_Constants[CONSTANT_SCALE];
	pConstant->ShaderType = CEffectInterface::FRAGMENT_SHADER;
	pConstant->GlobalType = CEffectCode::INPUT_VARIABLE;
	pConstant->uDetail    = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR1_VARIABLE, 0, 0, false);
	pConstant->uCount     = 1;
	pConstant->uIndex     = 0;
	pConstant->pValue     = &fScale;

	CEffectCode::LPATTRIBUTEVARIABLEDECLARATION pAttributeVariable;

	pAttributeVariable = &m_AttributeVariables[ATTRIBUTE_VARIABLE_TEXCOORD];
	pAttributeVariable->ShaderType   = CEffectInterface::FRAGMENT_SHADER;
	pAttributeVariable->ElementType  = CVertexDeclaration::FLOAT2;
	pAttributeVariable->ElementUsage = CVertexDeclaration::TEXCOORD;
	pAttributeVariable->uIndex       = uUVIndex;
	pAttributeVariable->bIsReadOnly  = true;
	pAttributeVariable->bIsForce     = true;

	pAttributeVariable = &m_AttributeVariables[ATTRIBUTE_VARIABLE_NORMAL];
	pAttributeVariable->ShaderType   = CEffectInterface::FRAGMENT_SHADER;
	pAttributeVariable->ElementType  = CVertexDeclaration::FLOAT3;
	pAttributeVariable->ElementUsage = CVertexDeclaration::NORMAL;
	pAttributeVariable->uIndex       = 0;
	pAttributeVariable->bIsReadOnly  = true;
	pAttributeVariable->bIsForce     = true;
}

CNormalOffsetTextureEffect::~CNormalOffsetTextureEffect(void)
{
}

UINT                                            CNormalOffsetTextureEffect::GetEffectInfoFlag     (                             )
{
	return ALCHEMY_FLAG(CEffectInfoInterface::FRAGMENT_SHADER_OUTPUT);
}

const CEffectInfoInterface::SAMPLER           * CNormalOffsetTextureEffect::GetSamplers           (UINT& uSamplerCount          )
{
	uSamplerCount = TOTAL_SAMPLERS;

	return m_Samplers;
}

const CEffectCode::ELEMENTPARAMETER            * CNormalOffsetTextureEffect::GetAttributes        (UINT& uAttributeCount        )
{
	uAttributeCount = TOTAL_ATTRIBUTES;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTPARAMETER            * CNormalOffsetTextureEffect::GetVarying           (UINT& uVaryingCount          )
{
	uVaryingCount = TOTAL_VARYING;

	return ALCHEMY_NULL;
}

const CEffectCode::GLOBALVARIABLE              * CNormalOffsetTextureEffect::GetConstants         (UINT& uConstantCount         )
{
	uConstantCount = TOTAL_CONSTANTS;

	return m_Constants;
}

const CEffectCode::TEMPORARYDECLARATION        * CNormalOffsetTextureEffect::GetTemporary         (UINT& uTemporaryCount        )
{
	uTemporaryCount = TOTAL_TEMPORARY;

	return sm_TEMPORARY;
}

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CNormalOffsetTextureEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
{
	uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLES;

	return m_AttributeVariables;
}

const CEffectCode::CONSTANTVARIABLEDECLARATION * CNormalOffsetTextureEffect::GetConstantVariables  (UINT& uConstantVariableCount)
{
	uConstantVariableCount = TOTAL_CONSTANT_VARIABLES;

	return ALCHEMY_NULL;
}

bool CNormalOffsetTextureEffect::ShaderDefineHandler(
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
	UINT uTexcoord = puTemporary[TEMPORARY_TEXCOORD];

	UINT uAddress[] = {0, 1};
	FragmentShaderDefiner.AppendInstruction(CCompilerInterface::MUL, uTexcoord, uAddress, 2, pAttributeVariables[ATTRIBUTE_VARIABLE_NORMAL].uHandle, uAddress, 2, puConstants[CONSTANT_SCALE], uAddress, 1);
	FragmentShaderDefiner.AppendInstruction(CCompilerInterface::ADD, uTexcoord, uTexcoord, pAttributeVariables[ATTRIBUTE_VARIABLE_TEXCOORD].uHandle);
	FragmentShaderDefiner.AppendInstruction(CCompilerInterface::TEX, uOutputColor, uTexcoord, puSamplers[SAMPLER_TEXTURE]);

	return true;
}