#include "AlchemyWaterBlendMethod.h"

using namespace alchemy;

const CEffectCode::ATTRIBUTE    CWaterBlendMethod::sm_ATTRIBUTES[CWaterBlendMethod::TOTAL_ATTRIBUTES] = 
{ 
	{CVertexDeclaration::FLOAT4, CVertexDeclaration::TEXCOORD, 0, false}, 
	{CVertexDeclaration::FLOAT1, CVertexDeclaration::FACING  , 0, false} 
};

const CEffectCode::VARIABLETYPE CWaterBlendMethod::sm_TEMPORARY [CWaterBlendMethod::TOTAL_TEMPORARY ] = 
{ 
	CEffectCode::VECTOR4_VARIABLE,
	CEffectCode::VECTOR1_VARIABLE
};

CWaterBlendMethod::CWaterBlendMethod(const FLOAT* pfBias, 
									 const FLOAT* pfPow, 
									 const FLOAT* pfDeep,
									 const FLOAT4* pColor, 
									 const FLOAT4* pDeepColor) :
m_fBias(0.2f),
m_fPow(5.0f),
m_fDeep(100.0f),
m_fInverseBias(0.8f), 
m_Color(0.0f, 0.1f, 0.15f, 1.0f),
m_DeepColor(0.0f, 0.1f, 0.125f, 1.0f)
{
	CEffectCode::LPCONSTANT pConstant;
	pConstant           = &m_Constants[CONSTANT_ONE];
	pConstant->Type     = CEffectCode::CONSTANT_VARIABLE;
	pConstant->uDetail  = CRenderMethod::CONSTANT_ONE;
	pConstant->uCount   = 1;
	pConstant->uIndex   = 0;
	pConstant->pValue   = ALCHEMY_NULL;
	pConstant->bIsForce = true;

	pfBias              = pfBias ? pfBias : &m_fBias;
	pConstant           = &m_Constants[CONSTANT_FRESNEL_BIAS];
	pConstant->Type     = CEffectCode::INPUT_VARIABLE;
	pConstant->uDetail  = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR1_VARIABLE, 0, 1, false);
	pConstant->uCount   = 1;
	pConstant->uIndex   = 0;
	pConstant->pValue   = pfBias;
	pConstant->bIsForce = true;

	pfPow               = pfPow ? pfPow : &m_fPow;
	pConstant           = &m_Constants[CONSTANT_FRESNEL_POW];
	pConstant->Type     = CEffectCode::INPUT_VARIABLE;
	pConstant->uDetail  = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR1_VARIABLE, 0, 1, false);
	pConstant->uCount   = 1;
	pConstant->uIndex   = 0;
	pConstant->pValue   = pfPow;
	pConstant->bIsForce = true;

	//m_fInverseBias      = 1 - *pfBias;
	pConstant           = &m_Constants[CONSTANT_INVERSE_BIAS];
	pConstant->Type     = CEffectCode::INPUT_VARIABLE;
	pConstant->uDetail  = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR1_VARIABLE, 0, 1, false);
	pConstant->uCount   = 1;
	pConstant->uIndex   = 0;
	pConstant->pValue   = ALCHEMY_NULL;//&m_fInverseBias;
	pConstant->bIsForce = true;

	pfDeep = pfDeep ? pfDeep : &m_fDeep;
	pConstant             = &m_Constants[CONSTANT_DEEP];
	pConstant->Type     = CEffectCode::INPUT_VARIABLE;
	pConstant->uDetail  = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR1_VARIABLE, 0, 1, false);
	pConstant->uCount   = 1;
	pConstant->uIndex   = 0;
	pConstant->pValue   = pfDeep;
	pConstant->bIsForce = true;

	pColor = pColor ? pColor : &m_Color;
	pConstant             = &m_Constants[CONSTANT_COLOR];
	pConstant->Type     = CEffectCode::INPUT_VARIABLE;
	pConstant->uDetail  = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR4_VARIABLE, 0, 1, false);
	pConstant->uCount   = 1;
	pConstant->uIndex   = 0;
	pConstant->pValue   = pColor;
	pConstant->bIsForce = true;

	pDeepColor = pDeepColor ? pDeepColor : &m_DeepColor;
	pConstant           = &m_Constants[CONSTANT_DEEP_COLOR];
	pConstant->Type     = CEffectCode::INPUT_VARIABLE;
	pConstant->uDetail  = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR4_VARIABLE, 0, 1, false);
	pConstant->uCount   = 1;
	pConstant->uIndex   = 0;
	pConstant->pValue   = pDeepColor;
	pConstant->bIsForce = true;
}

CWaterBlendMethod::~CWaterBlendMethod(void)
{
}

const CEffectCode::ATTRIBUTE   * CWaterBlendMethod::GetAttributes(UINT& uAttributeCount)
{
	uAttributeCount = TOTAL_ATTRIBUTES;

	return sm_ATTRIBUTES;
}

const CEffectCode::CONSTANT    * CWaterBlendMethod::GetConstants (UINT& uConstantCount )
{
	uConstantCount = TOTAL_CONSTANTS;

	return m_Constants;
}

const CEffectCode::VARIABLETYPE* CWaterBlendMethod::GetTemporary (UINT& uTemporaryCount)
{
	uTemporaryCount = TOTAL_TEMPORARY;

	return sm_TEMPORARY;
}

bool CWaterBlendMethod::OutputColorBlendHandler(
	UINT uOutputColor, 	
	UINT uSourceColor,	
	UINT uDestinationColor, 
	const UINT* puAttributes, 
	const UINT* puConstants,
	const UINT* puTemporary, 
	CEffectCode::IInstructionSet& ShaderDefiner)
{
	UINT uColor = puTemporary[TEMPORARY_COLOR], uReflection = puTemporary[TEMPORARY_REFLECTION], uOne = puConstants[CONSTANT_ONE], uFacing = puAttributes[ATTRIBUTE_FACING], x = 0, w = 3;
	ShaderDefiner.AppendInstruction(ICompiler::DIV, uReflection, &x, 1, puConstants[CONSTANT_DEEP], &x, 1, puAttributes[ATTRIBUTE_POSITION], &w, 1);
	ShaderDefiner.AppendInstruction(ICompiler::SAT, uReflection, uReflection, 0);
	ShaderDefiner.AppendInstruction(ICompiler::MUL, uDestinationColor, uDestinationColor, uReflection);
	ShaderDefiner.AppendInstruction(ICompiler::SUB, uReflection, uOne, uReflection);
	ShaderDefiner.AppendInstruction(ICompiler::MUL, uColor, puConstants[CONSTANT_DEEP_COLOR], uReflection);

	ShaderDefiner.AppendInstruction(ICompiler::ADD, uDestinationColor, uDestinationColor, uColor);

	ShaderDefiner.AppendInstruction(ICompiler::MUL, uColor, puConstants[CONSTANT_COLOR] ,uFacing);

	ShaderDefiner.AppendInstruction(ICompiler::SUB, uReflection, uOne, uFacing);

	ShaderDefiner.AppendInstruction(ICompiler::MUL, uDestinationColor, uDestinationColor, uReflection);

	ShaderDefiner.AppendInstruction(ICompiler::ADD, uDestinationColor, uDestinationColor, uColor);

	ShaderDefiner.AppendInstruction(ICompiler::POW, uReflection, uReflection, puConstants[CONSTANT_FRESNEL_POW]);

	UINT uFresnelBias = puConstants[CONSTANT_FRESNEL_BIAS], uInverseBias = puConstants[CONSTANT_INVERSE_BIAS];

	ShaderDefiner.AppendInstruction(ICompiler::SUB, uInverseBias, uOne, uFresnelBias);

	ShaderDefiner.AppendInstruction(ICompiler::MUL, uReflection, uInverseBias, uReflection);

	ShaderDefiner.AppendInstruction(ICompiler::ADD, uReflection, uFresnelBias, uReflection);

	ShaderDefiner.AppendInstruction(ICompiler::SAT, uReflection, uReflection, 0);

	ShaderDefiner.AppendInstruction(ICompiler::MUL, uSourceColor, uSourceColor, uReflection);

	ShaderDefiner.AppendInstruction(ICompiler::ADD, uOutputColor, uSourceColor, uDestinationColor);

	return true;
}