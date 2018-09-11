#include "AlchemyTextureBlurEffect.h"

using namespace alchemy;

const CEffectCode::TEMPORARYDECLARATION	CTextureBlurEffect::sm_TEMPORARYS[TOTAL_TEMPORARY] = { {IEffect::FRAGMENT_SHADER, CEffectCode::VECTOR4_VARIABLE, 1}, {IEffect::FRAGMENT_SHADER, CEffectCode::VECTOR4_VARIABLE, 1}, {IEffect::FRAGMENT_SHADER, CEffectCode::VECTOR2_VARIABLE, 1} };

CTextureBlurEffect::CTextureBlurEffect(const FLOAT2* pSamplerOffsets, const FLOAT* pSamplerWeights, UINT uSamplerAttributeNum, UINT uSamplerIndex, UINT uTexcoordIndex, const IEffect::SAMPLERDECLARATION* pSamplerDeclaration)
:m_uSamplerAttributeNum(uSamplerAttributeNum),
m_uConstantNum(0)
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
		pSampler->Declaration.MinFilter = IEffect::POINT;
		pSampler->Declaration.MagFilter = IEffect::POINT;
		pSampler->Declaration.MipFilter = IEffect::NONE;
		pSampler->Declaration.AddressU  = IEffect::CLAMP;
		pSampler->Declaration.AddressV  = IEffect::CLAMP;
		pSampler->Declaration.AddressW  = IEffect::CLAMP;
	}

	pSampler->uIndex = uSamplerIndex;

	if(pSamplerOffsets && pSamplerWeights && m_uSamplerAttributeNum)
	{
		m_uConstantNum = ( (m_uSamplerAttributeNum >> 1) + ( (m_uSamplerAttributeNum & 1) ? 1 : 0 ) ) << 1;
		ALCHEMY_DEBUG_NEW(m_pConstants, CEffectCode::GLOBALVARIABLE[m_uConstantNum]);

		CEffectCode::LPGLOBALVARIABLE pConstant;
		for (UINT i=0; i<m_uConstantNum; i+=2)
		{
			pConstant = &m_pConstants[i];

			pConstant->ShaderType = IEffect::FRAGMENT_SHADER;
			pConstant->GlobalType = CEffectCode::INPUT_VARIABLE;
			pConstant->uDetail    = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR4_VARIABLE, 1, 4, false);
			pConstant->uCount     = 1;
			pConstant->uIndex     = 0;
			pConstant->pValue     = reinterpret_cast<const VOID*>( pSamplerOffsets + i );

			pConstant = &m_pConstants[i+1];
			pConstant->ShaderType = IEffect::FRAGMENT_SHADER;
			pConstant->GlobalType = CEffectCode::INPUT_VARIABLE;
			pConstant->uDetail    = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR4_VARIABLE, 1, 2, false);
			pConstant->uCount     = 1;
			pConstant->uIndex     = 0;
			pConstant->pValue     = reinterpret_cast<const VOID*>( pSamplerWeights + i );
		}
	}
	else
		m_pConstants = ALCHEMY_NULL;
}

CTextureBlurEffect::~CTextureBlurEffect()
{
	ALCHEMY_DEBUG_DELETE_ARRAY(m_pConstants);
}

UINT      CTextureBlurEffect::GetEffectInfoFlag()
{
	return ALCHEMY_FLAG(IEffectInfo::FRAGMENT_SHADER_OUTPUT);
}

const IEffectInfo::SAMPLER* CTextureBlurEffect::GetSamplers(UINT& uSamplerCount)
{
	uSamplerCount = TOTAL_SAMPLERS;
	return m_Samplers;
}

const CEffectCode::ELEMENTDECLARATION* CTextureBlurEffect::GetAttributes(UINT& uAttributeCount)
{
	uAttributeCount = TOTAL_ATTRIBUTES;
	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION* CTextureBlurEffect::GetVarying(UINT& uVaryingCount)
{
	uVaryingCount = TOTAL_VARYING;
	return ALCHEMY_NULL;
}

const CEffectCode::GLOBALVARIABLE* CTextureBlurEffect::GetConstants(UINT& uConstantCount)
{
	uConstantCount = m_uConstantNum;
	return m_pConstants;
}

const CEffectCode::TEMPORARYDECLARATION* CTextureBlurEffect::GetTemporary(UINT& uTemporaryCount)
{
	uTemporaryCount = TOTAL_TEMPORARY;
	return sm_TEMPORARYS;
}

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CTextureBlurEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
{
	uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLES;
	return m_AttributeVariables;
}

const CEffectCode::CONSTANTVARIABLEDECLARATION * CTextureBlurEffect::GetConstantVariables (UINT& uConstantVariableCount )
{
	uConstantVariableCount = TOTAL_CONSTANT_VARIABLES;
	return ALCHEMY_NULL;
}

bool CTextureBlurEffect::ShaderDefineHandler(
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
	if(m_uSamplerAttributeNum < 1)
		return false;

	UINT uUVHandle = pAttributeVariables[ATTRIBUTE_VARIABLE_TEXCOORD].uHandle;
	UINT uTempUV = puTemporary[TEMPORAY_UV];
	UINT uFinalColor = puTemporary[TEMPORARY_FINAL_COLOR], uSourceColor = puTemporary[TEMPORARY_SOURCE_COLOR];
	UINT uLastIndex = m_uSamplerAttributeNum-1;
	UINT uSubAddress[] = {0, 1, 2, 3};
	
	for (UINT i=0; i<m_uSamplerAttributeNum; ++i)
	{
		FragmentShaderDefiner.AppendInstruction(ICompiler::ADD, uTempUV, uSubAddress, 2, uUVHandle, uSubAddress, 2, puConstants[i >> 1], i & 1 ? uSubAddress : &uSubAddress[2], 2);

		if(i == 0)
		{
			FragmentShaderDefiner.AppendInstruction(ICompiler::TEX, uFinalColor, uTempUV, puSamplers[SAMPLER_TEXTURE]);
			FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uFinalColor, ALCHEMY_NULL, 0, uFinalColor, ALCHEMY_NULL, 0, puConstants[ (i >> 1) + 1 ], i & 1 ? uSubAddress : &uSubAddress[2], 2);
		}
		else
		{
			FragmentShaderDefiner.AppendInstruction(ICompiler::TEX, uSourceColor, uTempUV, puSamplers[SAMPLER_TEXTURE]);
			FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uSourceColor, ALCHEMY_NULL, 0, uSourceColor, ALCHEMY_NULL, 0, puConstants[ (i >> 1) + 1 ], i & 1 ? uSubAddress : &uSubAddress[2], 2);
			FragmentShaderDefiner.AppendInstruction(ICompiler::ADD, i == uLastIndex ? uOutputColor : uFinalColor, uSourceColor, uFinalColor);
		}
	}

	return true;
}