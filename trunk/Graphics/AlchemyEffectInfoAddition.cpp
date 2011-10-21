#include "AlchemyEffectInfoAddition.h"

using namespace alchemy;

CEffectInfoAddition::CEffectInfoAddition(void)
{
}

CEffectInfoAddition::~CEffectInfoAddition(void)
{
}

const CEffectCode::ATTRIBUTE   * CEffectInfoAddition::GetAttributes(UINT& uAttributeCount)
{
	uAttributeCount = 0;

	return ALCHEMY_NULL;
}

const CEffectCode::CONSTANT    * CEffectInfoAddition::GetConstants (UINT& uConstantCount )
{
	uConstantCount = 0;

	return ALCHEMY_NULL;
}

const CEffectCode::VARIABLETYPE* CEffectInfoAddition::GetTemporary (UINT& uTemporaryCount)
{
	uTemporaryCount = 0;

	return ALCHEMY_NULL;
}

bool CEffectInfoAddition::OutputColorBlendHandler(		 
	UINT uOutputColor, 	 
	UINT uSourceColor,	
	UINT uDestinationColor,
	const UINT* puAttributes, 
	const UINT* puConstant,
	const UINT* puTemporary,
	CEffectCode::CInstructionSetInterface& ShaderDefiner)
{
	return ShaderDefiner.AppendInstruction(CCompilerInterface::ADD, uOutputColor, uSourceColor, uDestinationColor);
}