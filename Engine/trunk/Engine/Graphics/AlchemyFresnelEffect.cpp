#include "AlchemyFresnelEffect.h"
#include "AlchemyRenderMethod.h"

using namespace alchemy;

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION CFresnelEffect::sm_ATTRIBUTE_VARIABLES[CFresnelEffect::TOTAL_ATTRIBUTE_VARIABLES] = { {IEffect::FRAGMENT_SHADER, CVertexDeclaration::FLOAT1, CVertexDeclaration::FACING, 0, true, true} };
const CEffectCode::TEMPORARYDECLARATION         CFresnelEffect::sm_TEMPORARY          [CFresnelEffect::TOTAL_TEMPORARY          ] = { {IEffect::FRAGMENT_SHADER, CEffectCode::VECTOR1_VARIABLE, 1} };

CFresnelEffect::CFresnelEffect(void) :
m_fBias(0.2f),
m_fPow(5.0f),
m_fInverseBias(0.8f)
{
	CEffectCode::LPGLOBALVARIABLE pConstant;
	pConstant             = &m_Constants[CONSTANT_ONE];
	pConstant->ShaderType = IEffect::FRAGMENT_SHADER;
	pConstant->GlobalType = CEffectCode::CONSTANT_VARIABLE;
	pConstant->uDetail    = CRenderMethod::CONSTANT_ONE;
	pConstant->uCount     = 1;
	pConstant->uIndex     = 0;
	pConstant->pValue     = ALCHEMY_NULL;

	pConstant             = &m_Constants[CONSTANT_FRESNEL_BIAS];
	pConstant->ShaderType = IEffect::FRAGMENT_SHADER;
	pConstant->GlobalType = CEffectCode::INPUT_VARIABLE;
	pConstant->uDetail    = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR1_VARIABLE, 0, 1, false);
	pConstant->uCount     = 1;
	pConstant->uIndex     = 0;
	pConstant->pValue     = &m_fBias;

	pConstant             = &m_Constants[CONSTANT_FRESNEL_POW];
	pConstant->ShaderType = IEffect::FRAGMENT_SHADER;
	pConstant->GlobalType = CEffectCode::INPUT_VARIABLE;
	pConstant->uDetail    = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR1_VARIABLE, 0, 1, false);
	pConstant->uCount     = 1;
	pConstant->uIndex     = 0;
	pConstant->pValue     = &m_fPow;

	pConstant             = &m_Constants[CONSTANT_INVERSE_BIAS];
	pConstant->ShaderType = IEffect::FRAGMENT_SHADER;
	pConstant->GlobalType = CEffectCode::INPUT_VARIABLE;
	pConstant->uDetail    = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR1_VARIABLE, 0, 1, false);
	pConstant->uCount     = 1;
	pConstant->uIndex     = 0;
	pConstant->pValue     = &m_fInverseBias;
}

CFresnelEffect::~CFresnelEffect(void)
{
}

UINT                                            CFresnelEffect::GetEffectInfoFlag     (                             )
{
	return ALCHEMY_FLAG(IEffectInfo::FRAGMENT_SHADER_OUTPUT);
}

const IEffectInfo::SAMPLER           * CFresnelEffect::GetSamplers           (UINT& uSamplerCount          )
{
	uSamplerCount = TOTAL_SAMPLERS;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION            * CFresnelEffect::GetAttributes        (UINT& uAttributeCount        )
{
	uAttributeCount = TOTAL_ATTRIBUTES;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION            * CFresnelEffect::GetVarying           (UINT& uVaryingCount          )
{
	uVaryingCount = TOTAL_VARYING;

	return ALCHEMY_NULL;
}

const CEffectCode::GLOBALVARIABLE              * CFresnelEffect::GetConstants         (UINT& uConstantCount         )
{
	uConstantCount = TOTAL_CONSTANTS;

	return m_Constants;
}

const CEffectCode::TEMPORARYDECLARATION        * CFresnelEffect::GetTemporary         (UINT& uTemporaryCount        )
{
	uTemporaryCount = TOTAL_TEMPORARY;

	return sm_TEMPORARY;
}

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CFresnelEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
{
	uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLES;

	return sm_ATTRIBUTE_VARIABLES;
}

const CEffectCode::CONSTANTVARIABLEDECLARATION * CFresnelEffect::GetConstantVariables  (UINT& uConstantVariableCount)
{
	uConstantVariableCount = TOTAL_CONSTANT_VARIABLES;

	return ALCHEMY_NULL;
}

bool CFresnelEffect::ShaderDefineHandler(
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
	UINT uReflection = puTemporary[TEMPORARY_REFLECTION];

	UINT uOne = puConstants[CONSTANT_ONE];
	FragmentShaderDefiner.AppendInstruction(ICompiler::SUB, uReflection, uOne, pAttributeVariables[ATTRIBUTE_VARIABLE_FACING].uHandle);

	FragmentShaderDefiner.AppendInstruction(ICompiler::POW, uReflection, uReflection, puConstants[CONSTANT_FRESNEL_POW]);

	FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uReflection, puConstants[CONSTANT_INVERSE_BIAS], uReflection);

	FragmentShaderDefiner.AppendInstruction(ICompiler::ADD, uReflection, puConstants[CONSTANT_FRESNEL_BIAS], uReflection);

	FragmentShaderDefiner.AppendInstruction(ICompiler::SAT, uOutputColor, uReflection, 0);

	return true;
}