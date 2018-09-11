#include "AlchemyBidirectionalBlendMethod.h"
#include "AlchemyRenderMethod.h"

using namespace alchemy;

const CEffectCode::VARIABLETYPE CBidirectionalBlendMethod::sm_TEMPORARY[CBidirectionalBlendMethod::TOTAL_TEMPORARY] = 
{
	CEffectCode::VECTOR1_VARIABLE
};

const CEffectCode::CONSTANT		CBidirectionalBlendMethod::sm_CONSTANTS[CBidirectionalBlendMethod::TOTAL_CONSTANT] = 
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

CBidirectionalBlendMethod::CBidirectionalBlendMethod(BLENDMETHOD BlendMethod, UINT uChannelIndex)
:m_eBlendMethod(BlendMethod),
m_uChannelIndex(uChannelIndex)
{

}

CBidirectionalBlendMethod::~CBidirectionalBlendMethod()
{

}

const CEffectCode::ATTRIBUTE   * CBidirectionalBlendMethod::GetAttributes(UINT& uAttributeCount)
{
	uAttributeCount = 0;
	return ALCHEMY_NULL;
}

const CEffectCode::CONSTANT    * CBidirectionalBlendMethod::GetConstants (UINT& uConstantCount )
{
	uConstantCount = (m_eBlendMethod != POSITIVE ? 1 : 0);
	if(m_eBlendMethod == POSITIVE)
	{
		uConstantCount = 0;
		return ALCHEMY_NULL;
	}
	else
	{
		uConstantCount = TOTAL_CONSTANT;
		return sm_CONSTANTS;
	}
}

const CEffectCode::VARIABLETYPE* CBidirectionalBlendMethod::GetTemporary (UINT& uTemporaryCount)
{
	uTemporaryCount = (m_eBlendMethod != POSITIVE ? 1 : 0);
	if(m_eBlendMethod == POSITIVE)
	{
		uTemporaryCount = 0;
		return ALCHEMY_NULL;
	}
	else
	{
		uTemporaryCount = TOTAL_TEMPORARY;
		return sm_TEMPORARY;
	}
}

bool CBidirectionalBlendMethod::OutputColorBlendHandler(
							 UINT uOutputColor, 
							 UINT uSourceColor,
							 UINT uDestinationColor,
							 const UINT* puAttributes, 
							 const UINT* puConstants,
							 const UINT* puTemporary,
							 CEffectCode::IInstructionSet& ShaderDefiner)
{
	UINT uSubAddress[] = {0, 1, 2, 3};
	if(m_eBlendMethod == POSITIVE)
	{
		ShaderDefiner.AppendInstruction(ICompiler::MUL, uDestinationColor, uSubAddress, 3, uDestinationColor, uSubAddress, 3, uSourceColor, &uSubAddress[m_uChannelIndex], 1);
		ShaderDefiner.AppendInstruction(ICompiler::MOV, uOutputColor, ALCHEMY_NULL, 0, uDestinationColor, ALCHEMY_NULL, 0,  0, ALCHEMY_NULL, 0);
	}
	else
	{
		ShaderDefiner.AppendInstruction(ICompiler::SUB, puTemporary[TEMPORARY_COLOR], uSubAddress, 1, puConstants[CONSTANT_ONE], uSubAddress, 1, uSourceColor, &uSubAddress[m_uChannelIndex], 1);
		ShaderDefiner.AppendInstruction(ICompiler::MUL, uDestinationColor, uSubAddress, 3, uDestinationColor, uSubAddress, 3, puTemporary[TEMPORARY_COLOR], uSubAddress, 1);
		ShaderDefiner.AppendInstruction(ICompiler::MOV, uOutputColor, ALCHEMY_NULL, 0, uDestinationColor, ALCHEMY_NULL, 0, 0, ALCHEMY_NULL, 0);
	}

	return true;
}