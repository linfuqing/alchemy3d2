#include "AlchemyEffectInfoModulation.h"

using namespace alchemy;

CEffectInfoModulation::CEffectInfoModulation(BLENDFLAG BlendFlag)
:m_BlendFlag(BlendFlag)
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
	CEffectCode::IInstructionSet& ShaderDefiner)
{
	switch(m_BlendFlag)
	{
	case SOURCE_ALPHA:
		{
			UINT uSubAddress[] = {0,1,2};
			ShaderDefiner.AppendInstruction(ICompiler::MUL, uSourceColor,uSubAddress,3,  uSourceColor,uSubAddress,3, uDestinationColor,uSubAddress,3);
			ShaderDefiner.AppendInstruction(ICompiler::MOV, uOutputColor,uSourceColor,0);
		}
		break;
	case DESTINATION_ALPHA:
		{
			UINT uSubAddress[] = {0,1,2};
			ShaderDefiner.AppendInstruction(ICompiler::MUL, uDestinationColor,uSubAddress,3,  uSourceColor,uSubAddress,3, uDestinationColor,uSubAddress,3);
			ShaderDefiner.AppendInstruction(ICompiler::MOV, uOutputColor,uDestinationColor,0);
		}
		break;
	case BLEND_ALPHA:
		ShaderDefiner.AppendInstruction(ICompiler::MUL, uOutputColor, uSourceColor, uDestinationColor);
		break;
	}

	return true;
}