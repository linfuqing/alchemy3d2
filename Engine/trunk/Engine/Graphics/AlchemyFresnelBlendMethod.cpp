#include "AlchemyFresnelBlendMethod.h"
#include "AlchemyRenderMethod.h"

using namespace alchemy;

const CEffectCode::ATTRIBUTE    CFresnelBlendMethod::sm_ATTRIBUTES[TOTAL_ATTRIBUTES] = { {CVertexDeclaration::FLOAT1, CVertexDeclaration::FACING, 0, false} };
const CEffectCode::VARIABLETYPE CFresnelBlendMethod::sm_TEMPORARY [TOTAL_TEMPORARY ] = {CEffectCode::VECTOR1_VARIABLE};

CFresnelBlendMethod::CFresnelBlendMethod(void) :
m_fBias(0.2f),
m_fPow(5.0f),
m_fInverseBias(0.8f)
{
	CEffectCode::LPCONSTANT pConstant;

	pConstant           = &m_Constants[CONSTANT_ONE];
	pConstant->Type     = CEffectCode::CONSTANT_VARIABLE;
	pConstant->uDetail  = CRenderMethod::CONSTANT_ONE;
	pConstant->uCount   = 1;
	pConstant->uIndex   = 0;
	pConstant->pValue   = ALCHEMY_NULL;
	pConstant->bIsForce = true;

	pConstant           = &m_Constants[CONSTANT_FRESNEL_BIAS];
	pConstant->Type     = CEffectCode::INPUT_VARIABLE;
	pConstant->uDetail  = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR1_VARIABLE, 0, 1, false);
	pConstant->uCount   = 1;
	pConstant->uIndex   = 0;
	pConstant->pValue   = &m_fBias;
	pConstant->bIsForce = true;

	pConstant           = &m_Constants[CONSTANT_FRESNEL_POW];
	pConstant->Type     = CEffectCode::INPUT_VARIABLE;
	pConstant->uDetail  = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR1_VARIABLE, 0, 1, false);
	pConstant->uCount   = 1;
	pConstant->uIndex   = 0;
	pConstant->pValue   = &m_fPow;
	pConstant->bIsForce = true;

	pConstant           = &m_Constants[CONSTANT_INVERSE_BIAS];
	pConstant->Type     = CEffectCode::INPUT_VARIABLE;
	pConstant->uDetail  = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR1_VARIABLE, 0, 1, false);
	pConstant->uCount   = 1;
	pConstant->uIndex   = 0;
	pConstant->pValue   = &m_fInverseBias;
	pConstant->bIsForce = true;
}

CFresnelBlendMethod::~CFresnelBlendMethod(void)
{
}

const CEffectCode::ATTRIBUTE   * CFresnelBlendMethod::GetAttributes(UINT& uAttributeCount)
{
	uAttributeCount = TOTAL_ATTRIBUTES;

	return sm_ATTRIBUTES;
}

const CEffectCode::CONSTANT    * CFresnelBlendMethod::GetConstants (UINT& uConstantCount )
{
	uConstantCount = TOTAL_CONSTANTS;

	return m_Constants;
}

const CEffectCode::VARIABLETYPE* CFresnelBlendMethod::GetTemporary (UINT& uTemporaryCount)
{
	uTemporaryCount = TOTAL_TEMPORARY;

	return sm_TEMPORARY;
}

bool CFresnelBlendMethod::OutputColorBlendHandler(		 
	UINT uOutputColor, 	 
	UINT uSourceColor,	
	UINT uDestinationColor,
	const UINT* puAttributes, 
	const UINT* puConstant,
	const UINT* puTemporary,
	CEffectCode::IInstructionSet& ShaderDefiner)
{
	UINT uReflection = puTemporary[TEMPORARY_REFLECTION];

	UINT uOne = puConstant[CONSTANT_ONE];
	ShaderDefiner.AppendInstruction(ICompiler::SUB, uReflection, uOne, puAttributes[ATTRIBUTE_FACING]);

	ShaderDefiner.AppendInstruction(ICompiler::POW, uReflection, uReflection, puConstant[CONSTANT_FRESNEL_POW]);

	ShaderDefiner.AppendInstruction(ICompiler::MUL, uReflection, puConstant[CONSTANT_INVERSE_BIAS], uReflection);

	ShaderDefiner.AppendInstruction(ICompiler::ADD, uReflection, puConstant[CONSTANT_FRESNEL_BIAS], uReflection);

	ShaderDefiner.AppendInstruction(ICompiler::SAT, uReflection, uReflection, 0);

	ShaderDefiner.AppendInstruction(ICompiler::MUL, uSourceColor, uSourceColor, uReflection);

	ShaderDefiner.AppendInstruction(ICompiler::SUB, uReflection, uOne, uReflection);

	ShaderDefiner.AppendInstruction(ICompiler::MUL, uDestinationColor, uDestinationColor, uReflection);

	ShaderDefiner.AppendInstruction(ICompiler::ADD, uOutputColor, uSourceColor, uDestinationColor);

	return true;
}