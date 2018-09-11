#include "AlchemyAlphaTestBlendMethod.h"

using namespace alchemy;


const CEffectCode::VARIABLETYPE	CAlphaTestBlendMethod::sm_TEMPORARY = CEffectCode::VECTOR1_VARIABLE;

CAlphaTestBlendMethod::CAlphaTestBlendMethod(const FLOAT* pfAlphaRef)
:m_fAlphaRef(0.3f)
{
	pfAlphaRef = pfAlphaRef ? pfAlphaRef : &m_fAlphaRef;
	m_CONSTANT.Type = CEffectCode::INPUT_VARIABLE;
	m_CONSTANT.uDetail = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR1_VARIABLE, 1, 1, false);
	m_CONSTANT.uCount =	1,
	m_CONSTANT.uIndex =	0;
	m_CONSTANT.pValue = pfAlphaRef;
	m_CONSTANT.bIsForce = true;
}

CAlphaTestBlendMethod::~CAlphaTestBlendMethod()
{

}

const CEffectCode::ATTRIBUTE   * CAlphaTestBlendMethod::GetAttributes(UINT& uAttributeCount)
{
	uAttributeCount = 0;
	return ALCHEMY_NULL;
}

const CEffectCode::CONSTANT    * CAlphaTestBlendMethod::GetConstants (UINT& uConstantCount )
{
	uConstantCount = 1;
	return &m_CONSTANT;
}

const CEffectCode::VARIABLETYPE* CAlphaTestBlendMethod::GetTemporary (UINT& uTemporaryCount)
{
	uTemporaryCount = 1;
	return &sm_TEMPORARY;
}

bool CAlphaTestBlendMethod::OutputColorBlendHandler(
							 UINT uOutputColor, 
							 UINT uSourceColor,
							 UINT uDestinationColor,
							 const UINT* puAttributes, 
							 const UINT* puConstants,
							 const UINT* puTemporary,
							 CEffectCode::IInstructionSet& ShaderDefiner)
{
	UINT w = 3, x= 0;
	ShaderDefiner.AppendInstruction(ICompiler::SUB, puTemporary[0], &x, 1, uSourceColor, &w, 1, puConstants[0], &x, 1);
	ShaderDefiner.AppendInstruction(ICompiler::KIL, 0, ALCHEMY_NULL, 0,  puTemporary[0], &x, 1,  0, ALCHEMY_NULL, 0);
	ShaderDefiner.AppendInstruction(ICompiler::MOV, uOutputColor, uSourceColor, 0);

	return true;
}