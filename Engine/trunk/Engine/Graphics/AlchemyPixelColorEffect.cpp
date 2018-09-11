#include "AlchemyPixelColorEffect.h"
#include "AlchemyRenderMethod.h"

using namespace alchemy;

CPixelColorEffect::CPixelColorEffect(const FLOAT4* pColor) :
m_Color(0.0f, 0.0f, 0.0f, 1.0f)
{
	pColor = pColor ? pColor : &m_Color;
	m_Constant.ShaderType = IEffect::FRAGMENT_SHADER;
	m_Constant.GlobalType = CEffectCode::INPUT_VARIABLE;
	m_Constant.uDetail = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR4_VARIABLE, 1, 4, false);
	m_Constant.uCount = 1;
	m_Constant.uIndex = 0;
	m_Constant.pValue = pColor;
}

CPixelColorEffect::~CPixelColorEffect()
{

}

UINT                                             CPixelColorEffect::GetEffectInfoFlag    (                             )
{
	return ALCHEMY_FLAG( IEffectInfo::FRAGMENT_SHADER_OUTPUT );
}

const IEffectInfo::SAMPLER                                  * CPixelColorEffect::GetSamplers          (UINT& uSamplerCount          )
{
	uSamplerCount = TOTAL_SAMPLERS;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION            * CPixelColorEffect::GetAttributes        (UINT& uAttributeCount        )
{
	uAttributeCount = TOTAL_ATTRIBUTES;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION            * CPixelColorEffect::GetVarying           (UINT& uVaryingCount          )
{
	uVaryingCount = TOTAL_VARYING;

	return ALCHEMY_NULL;
}

const CEffectCode::GLOBALVARIABLE              * CPixelColorEffect::GetConstants         (UINT& uConstantCount         )
{
	uConstantCount = TOTAL_CONSTANTS;

	return &m_Constant;
}

const CEffectCode::TEMPORARYDECLARATION        * CPixelColorEffect::GetTemporary         (UINT& uTemporaryCount        )
{
	uTemporaryCount = TOTAL_TEMPORARY;

	return ALCHEMY_NULL;
}

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CPixelColorEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
{
	uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLE;

	return ALCHEMY_NULL;
}

const CEffectCode::CONSTANTVARIABLEDECLARATION * CPixelColorEffect::GetConstantVariables (UINT& uConstantVariableCount )
{
	uConstantVariableCount = TOTAL_CONSTANT_VARIABLE;

	return ALCHEMY_NULL;
}

bool CPixelColorEffect::ShaderDefineHandler(
	UINT uOutputColor, 
	const UINT* puSamplers, 
	const UINT* puAttributes, 
	const UINT* puConstants, 
	const CEffectCode::VARYING* pVarying, 
	const UINT* puTemporary,
	const CEffectCode::SHADERVARIABLE* pAttributeVariable,
	const CEffectCode::SHADERVARIABLE* pConstantVariable,
	CEffectCode::IInstructionSet& VertexShaderDefiner,
	CEffectCode::IInstructionSet& FragmentShaderDefiner,
	CEffectCode::CPassDefiner::CEffectCodeUtils& EffectCodeUtils)
{
	FragmentShaderDefiner.AppendInstruction(ICompiler::MOV, uOutputColor, puConstants[CONSTANT_COLOR], 0);

	return true;
}