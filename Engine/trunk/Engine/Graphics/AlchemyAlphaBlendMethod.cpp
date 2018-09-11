#include "AlchemyAlphaBlendMethod.h"
#include "AlchemyRenderMethod.h"

using namespace alchemy;

const CEffectCode::CONSTANT CAlphaBlendMethod::sm_CONSTANTS[CAlphaBlendMethod::TOTAL_CONSTANTS] = 
{
	{
		CEffectCode::CONSTANT_VARIABLE,
		CRenderMethod::CONSTANT_ONE,
		1,
		0,
		ALCHEMY_NULL,
		true
	}
};

CAlphaBlendMethod::CAlphaBlendMethod(void)
{
}

CAlphaBlendMethod::~CAlphaBlendMethod(void)
{
}

const CEffectCode::ATTRIBUTE   * CAlphaBlendMethod::GetAttributes(UINT& uAttributeCount)
{
	uAttributeCount = 0;

	return ALCHEMY_NULL;
}

const CEffectCode::CONSTANT    * CAlphaBlendMethod::GetConstants (UINT& uConstantCount )
{
	uConstantCount = 1;

	return sm_CONSTANTS;
}

const CEffectCode::VARIABLETYPE* CAlphaBlendMethod::GetTemporary (UINT& uTemporaryCount)
{
	uTemporaryCount = 0;

	return ALCHEMY_NULL;
}

bool CAlphaBlendMethod::OutputColorBlendHandler(		 
	UINT uOutputColor, 	 
	UINT uSourceColor,	
	UINT uDestinationColor,
	const UINT* puAttributes, 
	const UINT* puConstants,
	const UINT* puTemporary,
	CEffectCode::IInstructionSet& ShaderDefiner)
{
	UINT x = 0, w = 3;

	ShaderDefiner.AppendInstruction(ICompiler::MUL, uSourceColor     , ALCHEMY_NULL, 0, uSourceColor             , ALCHEMY_NULL, 0, uDestinationColor, &w, 1);
	ShaderDefiner.AppendInstruction(ICompiler::SUB, uDestinationColor, &w          , 1, puConstants[CONSTANT_ONE], &x          , 1, uDestinationColor, &w, 1);
	ShaderDefiner.AppendInstruction(ICompiler::MUL, uDestinationColor, ALCHEMY_NULL, 0, uDestinationColor        , ALCHEMY_NULL, 0, uDestinationColor, &w, 1);

	ShaderDefiner.AppendInstruction(ICompiler::ADD, uOutputColor, uSourceColor, uDestinationColor);

	return true;
}