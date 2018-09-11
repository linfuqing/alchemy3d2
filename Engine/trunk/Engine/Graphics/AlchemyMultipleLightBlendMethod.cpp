#include "AlchemyMultipleLightBlendMethod.h"
#include "AlchemyRenderMethod.h"

using namespace alchemy;

const CEffectCode::CONSTANT     CMultipleLightBlendMethod::sm_CONSTANTS[CMultipleLightBlendMethod::TOTAL_CONSTANTS] = 
{
	{CEffectCode::MATERIAL_VARIABLE, CRenderMethod::MATERIAL_AMBIENT , 1, 0, ALCHEMY_NULL, false}, 
	{CEffectCode::MATERIAL_VARIABLE, CRenderMethod::MATERIAL_DIFFUSE , 1, 0, ALCHEMY_NULL, false}, 
	{CEffectCode::MATERIAL_VARIABLE, CRenderMethod::MATERIAL_SPECULAR, 1, 0, ALCHEMY_NULL, false}, 
	{CEffectCode::MATERIAL_VARIABLE, CRenderMethod::MATERIAL_EMISSIVE, 1, 0, ALCHEMY_NULL, false}
};

const CEffectCode::VARIABLETYPE CMultipleLightBlendMethod::sm_TEMPORARY[CMultipleLightBlendMethod::TOTAL_TEMPORARY] = 
{
	CEffectCode::VECTOR4_VARIABLE,
	CEffectCode::VECTOR4_VARIABLE
};

CMultipleLightBlendMethod::CMultipleLightBlendMethod(bool bIsWriteOverDiffuse, bool bIsWriteOverSpecular) :
m_bIsWriteOverDiffuse(bIsWriteOverDiffuse),
m_bIsWriteOverSpecular(bIsWriteOverSpecular)
{
}

CMultipleLightBlendMethod::~CMultipleLightBlendMethod(void)
{
}

const CEffectCode::ATTRIBUTE   * CMultipleLightBlendMethod::GetAttributes(UINT& uAttributeCount)
{
	uAttributeCount = TOTAL_ATTRIBUTES;

	return ALCHEMY_NULL;
}

const CEffectCode::CONSTANT    * CMultipleLightBlendMethod::GetConstants (UINT& uConstantCount )
{
	uConstantCount = TOTAL_CONSTANTS;

	return sm_CONSTANTS;
}

const CEffectCode::VARIABLETYPE* CMultipleLightBlendMethod::GetTemporary (UINT& uTemporaryCount)
{
	uTemporaryCount = 0;
	
	if(!m_bIsWriteOverDiffuse)
		++ uTemporaryCount;
		
	if(!m_bIsWriteOverSpecular)
		++ uTemporaryCount;

	return sm_TEMPORARY;
}

bool CMultipleLightBlendMethod::OutputColorBlendHandler(
	UINT uOutputColor, 	 
	UINT uSourceColor,	
	UINT uDestinationColor,
	const UINT* puAttributes, 
	const UINT* puConstant,
	const UINT* puTemporary,
	CEffectCode::IInstructionSet& ShaderDefiner)
{
	UINT uCurrentTemporary = 0, uDiffuse = uSourceColor;
	if(!m_bIsWriteOverDiffuse)
		uDiffuse = puTemporary[uCurrentTemporary ++];
		
	ShaderDefiner.AppendInstruction(ICompiler::MUL, uDiffuse, uSourceColor, puConstant[CONSTANT_MATERIAL_DIFFUSE]);

	UINT uSpecular = uDestinationColor;
	if(!m_bIsWriteOverSpecular)
		uSpecular = puTemporary[uCurrentTemporary ++];
		
	ShaderDefiner.AppendInstruction(ICompiler::MUL, uSpecular, uDestinationColor, puConstant[CONSTANT_MATERIAL_SPECULAR]);
	ShaderDefiner.AppendInstruction(ICompiler::ADD, uDiffuse, uDiffuse, uSpecular);

	ShaderDefiner.AppendInstruction(ICompiler::MUL, uSpecular, uDiffuse, puConstant[CONSTANT_MATERIAL_AMBIENT]);

	ShaderDefiner.AppendInstruction(ICompiler::ADD, uOutputColor, uSpecular, puConstant[CONSTANT_MATERIAL_EMISSIVE]);

	return true;
}