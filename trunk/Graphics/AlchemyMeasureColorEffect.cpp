#include "AlchemyMeasureColorEffect.h"

using namespace alchemy;

CMeasureColorEffect::CMeasureColorEffect(const FLOAT4* pMeasure)
{
	m_Contans[CONSTANT_MEASURE_VALUE].ShaderType = CEffectInterface::FRAGMENT_SHADER;
	m_Contans[CONSTANT_MEASURE_VALUE].GlobalType = CEffectCode::INPUT_VARIABLE;
	m_Contans[CONSTANT_MEASURE_VALUE].uDetail = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR4_VARIABLE, 1, 1, false);
	m_Contans[CONSTANT_MEASURE_VALUE].uCount = 1;
	m_Contans[CONSTANT_MEASURE_VALUE].uIndex = 0;
	m_Contans[CONSTANT_MEASURE_VALUE].pValue = reinterpret_cast<const VOID*>(pMeasure);
}

CMeasureColorEffect::~CMeasureColorEffect(void)
{

}

UINT                                            CMeasureColorEffect::GetEffectInfoFlag     (                             )
{
	return ALCHEMY_FLAG(CEffectInfoInterface::FRAGMENT_SHADER_OUTPUT);
}

const CEffectInfoInterface::SAMPLER           * CMeasureColorEffect::GetSamplers           (UINT& uSamplerCount          )
{
	uSamplerCount = TOTAL_SAMPLERS;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTPARAMETER            * CMeasureColorEffect::GetAttributes        (UINT& uAttributeCount        )
{
	uAttributeCount = TOTAL_ATTRIBUTES;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTPARAMETER            * CMeasureColorEffect::GetVarying           (UINT& uVaryingCount          )
{
	uVaryingCount = TOTAL_VARYING;

	return ALCHEMY_NULL;
}

const CEffectCode::GLOBALVARIABLE              * CMeasureColorEffect::GetConstants         (UINT& uConstantCount         )
{
	uConstantCount = TOTAL_CONSTANTS;

	return m_Contans;
}

const CEffectCode::TEMPORARYDECLARATION        * CMeasureColorEffect::GetTemporary         (UINT& uTemporaryCount        )
{
	uTemporaryCount = TOTAL_TEMPORARY;

	return ALCHEMY_NULL;
}

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CMeasureColorEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
{
	uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLES;

	return ALCHEMY_NULL;
}

const CEffectCode::CONSTANTVARIABLEDECLARATION * CMeasureColorEffect::GetConstantVariables  (UINT& uConstantVariableCount)
{
	uConstantVariableCount = TOTAL_CONSTANT_VARIABLES;

	return ALCHEMY_NULL;
}

bool CMeasureColorEffect::ShaderDefineHandler(
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
	FragmentShaderDefiner.AppendInstruction( CCompilerInterface::MOV, uOutputColor, ALCHEMY_NULL, 0, puConstants[CONSTANT_MEASURE_VALUE],ALCHEMY_NULL,0, 0, ALCHEMY_NULL, 0 );

	return true;
}