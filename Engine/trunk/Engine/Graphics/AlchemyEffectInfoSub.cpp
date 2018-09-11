#include "AlchemyEffectInfoSub.h"

using namespace alchemy;

CEffectInfoSub::CEffectInfoSub(BLENDFLAG BlendFlag)
:m_BlendFlag(BlendFlag)
{
}

CEffectInfoSub::~CEffectInfoSub(void)
{
}

const CEffectCode::ATTRIBUTE   * CEffectInfoSub::GetAttributes(UINT& uAttributeCount)
{
	uAttributeCount = 0;

	return ALCHEMY_NULL;
}

const CEffectCode::CONSTANT    * CEffectInfoSub::GetConstants (UINT& uConstantCount )
{
	uConstantCount = 0;

	return ALCHEMY_NULL;
}

const CEffectCode::VARIABLETYPE* CEffectInfoSub::GetTemporary (UINT& uTemporaryCount)
{
	uTemporaryCount = 0;

	return ALCHEMY_NULL;
}

bool CEffectInfoSub::OutputColorBlendHandler(		 
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
			ShaderDefiner.AppendInstruction(ICompiler::SUB, uSourceColor,uSubAddress,3,  uSourceColor,uSubAddress,3, uDestinationColor,uSubAddress,3);
			ShaderDefiner.AppendInstruction(ICompiler::MOV, uOutputColor,uSourceColor,0);
		}
		break;
	case DESTINATION_ALPHA:
		{
			UINT uSubAddress[] = {0,1,2};
			ShaderDefiner.AppendInstruction(ICompiler::SUB, uDestinationColor,uSubAddress,3,  uSourceColor,uSubAddress,3, uDestinationColor,uSubAddress,3);
			ShaderDefiner.AppendInstruction(ICompiler::MOV, uOutputColor,uDestinationColor,0);
		}
		break;
	case BLEND_ALPHA:
		ShaderDefiner.AppendInstruction(ICompiler::SUB, uOutputColor, uSourceColor, uDestinationColor);
		break;
	}

	return true;
}