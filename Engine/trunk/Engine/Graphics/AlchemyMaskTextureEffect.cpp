#include "AlchemyMaskTextureEffect.h"
#include "AlchemyRenderMethod.h"

using namespace alchemy;
const CEffectCode::TEMPORARYDECLARATION CMaskTextureEffect::sm_TEMPORARY[TOTAL_TEMPORARY] = { {IEffect::FRAGMENT_SHADER, CEffectCode::VECTOR4_VARIABLE, 1} };

CMaskTextureEffect::CMaskTextureEffect(UINT uChannelIndex, FLOAT fMaskRef, UINT uSamplerIndex, UINT uTexcoordIndex, const IEffect::SAMPLERDECLARATION* pSamplerDeclaration)
:m_uChannelIndex(uChannelIndex),
m_fMaskReference(fMaskRef)
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

	m_Constant.ShaderType = IEffect::FRAGMENT_SHADER;
	m_Constant.uIndex     = 0;
	m_Constant.uCount     = 1;
	m_Constant.GlobalType = CEffectCode::INPUT_VARIABLE;
	m_Constant.uDetail    = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR1_VARIABLE, 1, 1);
	m_Constant.pValue     = reinterpret_cast<const void*>(&m_fMaskReference);
}

CMaskTextureEffect::~CMaskTextureEffect(void)
{
}

UINT                                            CMaskTextureEffect::GetEffectInfoFlag     (                             )
{
	return ALCHEMY_FLAG(IEffectInfo::FRAGMENT_SHADER_OUTPUT);
}

const IEffectInfo::SAMPLER           * CMaskTextureEffect::GetSamplers           (UINT& uSamplerCount          )
{
	uSamplerCount = TOTAL_SAMPLERS;

	return m_Samplers;
}

const CEffectCode::ELEMENTDECLARATION            * CMaskTextureEffect::GetAttributes        (UINT& uAttributeCount        )
{
	uAttributeCount = TOTAL_ATTRIBUTES;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION            * CMaskTextureEffect::GetVarying           (UINT& uVaryingCount          )
{
	uVaryingCount = TOTAL_VARYING;

	return ALCHEMY_NULL;
}

const CEffectCode::GLOBALVARIABLE              * CMaskTextureEffect::GetConstants         (UINT& uConstantCount         )
{
	uConstantCount = TOTAL_CONSTANTS;

	return &m_Constant;
}

const CEffectCode::TEMPORARYDECLARATION        * CMaskTextureEffect::GetTemporary         (UINT& uTemporaryCount        )
{
	uTemporaryCount = TOTAL_TEMPORARY;

	return sm_TEMPORARY;
}

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CMaskTextureEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
{
	uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLES;

	return m_AttributeVariables;
}

const CEffectCode::CONSTANTVARIABLEDECLARATION * CMaskTextureEffect::GetConstantVariables  (UINT& uConstantVariableCount)
{
	uConstantVariableCount = TOTAL_CONSTANT_VARIABLES;

	return ALCHEMY_NULL;
}

bool CMaskTextureEffect::ShaderDefineHandler(
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
	UINT uSubAddress[] = {0, 1, 2, 3};
	FragmentShaderDefiner.AppendInstruction(ICompiler::TEX, puTemporary[TEMPORARY_COLOR], pAttributeVariables[ATTRIBUTE_VARIABLE_TEXCOORD].uHandle, puSamplers[SAMPLER_TEXTURE]);
	FragmentShaderDefiner.AppendInstruction(ICompiler::SGE, puTemporary[TEMPORARY_COLOR], &uSubAddress[ m_uChannelIndex ], 1, puTemporary[TEMPORARY_COLOR], &uSubAddress[ m_uChannelIndex ], 1, puConstants[CONSTANT_MASK_REFERENCE], uSubAddress, 1);

	UINT* pIndexes;
	ALCHEMY_DEBUG_NEW(pIndexes, UINT[4]);
	for (UINT i=0; i<4; ++i)
		pIndexes[i] = m_uChannelIndex;

	FragmentShaderDefiner.AppendInstruction(ICompiler::MOV, uOutputColor, ALCHEMY_NULL, 0, puTemporary[TEMPORARY_COLOR], pIndexes, 4, 0, ALCHEMY_NULL, 0);

	ALCHEMY_DEBUG_DELETE_ARRAY(pIndexes);

	return true;
}