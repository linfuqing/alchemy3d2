#include "AlchemyEffectInfoAddition.h"

using namespace alchemy;

CEffectInfoAddition::CEffectInfoAddition(BLENDFLAG BlendFlag)
:m_BlendFlag(BlendFlag)
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
	CEffectCode::IInstructionSet& ShaderDefiner)
{
	switch(m_BlendFlag)
	{
	case SOURCE_ALPHA:
		{
			UINT uSubAddress[] = {0,1,2};
			ShaderDefiner.AppendInstruction(ICompiler::ADD, uSourceColor,uSubAddress,3,  uSourceColor,uSubAddress,3, uDestinationColor,uSubAddress,3);
			ShaderDefiner.AppendInstruction(ICompiler::MOV, uOutputColor,uSourceColor,0);
		}
		break;
	case DESTINATION_ALPHA:
		{
			UINT uSubAddress[] = {0,1,2};
			ShaderDefiner.AppendInstruction(ICompiler::ADD, uDestinationColor,uSubAddress,3,  uSourceColor,uSubAddress,3, uDestinationColor,uSubAddress,3);
			ShaderDefiner.AppendInstruction(ICompiler::MOV, uOutputColor,uDestinationColor,0);
		}
		break;
	case BLEND_ALPHA:
		ShaderDefiner.AppendInstruction(ICompiler::ADD, uOutputColor, uSourceColor, uDestinationColor);
		break;
	}

	return true;
}