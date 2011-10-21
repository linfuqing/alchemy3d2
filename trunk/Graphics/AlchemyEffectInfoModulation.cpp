#include "AlchemyEffectInfoModulation.h"

using namespace alchemy;

CEffectInfoModulation::CEffectInfoModulation(void)
{
}

CEffectInfoModulation::~CEffectInfoModulation(void)
{
}

const CEffectCode::ATTRIBUTE   * CEffectInfoModulation::GetAttributes(UINT& uAttributeCount)
{
	uAttributeCount = 0;

	return ALCHEMY_NULL;
}

const CEffectCode::CONSTANT    * CEffectInfoModulation::GetConstants (UINT& uConstantCount )
{
	uConstantCount = 0;

	return ALCHEMY_NULL;
}

const CEffectCode::VARIABLETYPE* CEffectInfoModulation::GetTemporary (UINT& uTemporaryCount)
{
	uTemporaryCount = 0;

	return ALCHEMY_NULL;
}

bool CEffectInfoModulation::OutputColorBlendHandler(
	UINT uOutputColor, 	
	UINT uSourceColor,	
	UINT uDestinationColor, 
	const UINT* puAttributes, 
	const UINT* puConstant,
	const UINT* puTemporary, 
	CEffectCode::CInstructionSetInterface& ShaderDefiner)
{
	return ShaderDefiner.AppendInstruction(CCompilerInterface::MUL, uOutputColor, uSourceColor, uDestinationColor);
}