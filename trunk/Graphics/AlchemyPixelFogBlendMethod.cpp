#include "AlchemyPixelFogBlendMethod.h"
#include "AlchemyRenderMethod.h"
#include "../Math/AlchemyMath.h"

using namespace alchemy;

const CEffectCode::ATTRIBUTE						CPixelFogBlendMethod::sm_ATTRIBUTEVARIABLES[TOTAL_ATTRIBUTES] = { {CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION, 0, false} };
const CEffectCode::VARIABLETYPE						CPixelFogBlendMethod::sm_TEMPORARYVARIABLES[TOTAL_TEMPORARY] = { CEffectCode::VECTOR3_VARIABLE, CEffectCode::VECTOR4_VARIABLE, CEffectCode::VECTOR4_VARIABLE  };


CPixelFogBlendMethod::CPixelFogBlendMethod(FOG_TYPE FogType)
:m_FogType(FogType),
m_fFogParameter(0.0f),
m_fDistance(0.0f)
{

}

CPixelFogBlendMethod::~CPixelFogBlendMethod()
{

}

//#include <as3.h>
bool CPixelFogBlendMethod::Create(const FLOAT* pStart, const FLOAT* pEnd, const FLOAT* pDensity)
{
	switch (m_FogType)
	{
	case LINEAR:
		{
			if(!pStart || !pEnd)
				return false;

			m_fStart = *pStart;
			m_fEnd = *pEnd;
			if(m_fEnd <= m_fStart)
				m_fEnd = m_fStart + 1.0f;

			m_fDistance = m_fEnd - m_fStart;

			m_Constans[CONSTANT_PARAMETER1].Type = CEffectCode::INPUT_VARIABLE;
			m_Constans[CONSTANT_PARAMETER1].uDetail = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR1_VARIABLE, 1, 1, false);
			m_Constans[CONSTANT_PARAMETER1].uCount = 1;
			m_Constans[CONSTANT_PARAMETER1].uIndex = 0;
			m_Constans[CONSTANT_PARAMETER1].pValue = &m_fEnd;
			m_Constans[CONSTANT_PARAMETER1].bIsForce = false;

			m_Constans[CONSTANT_PARAMETER2].Type = CEffectCode::INPUT_VARIABLE;
			m_Constans[CONSTANT_PARAMETER2].uDetail = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR1_VARIABLE, 1, 1, false);
			m_Constans[CONSTANT_PARAMETER2].uCount = 1;
			m_Constans[CONSTANT_PARAMETER2].uIndex = 0;
			m_Constans[CONSTANT_PARAMETER2].pValue = &m_fDistance;
			m_Constans[CONSTANT_PARAMETER2].bIsForce = false;
		}
		break;
	case EXP:
	case EXP2:
		{
			if(!pDensity)
				return false;

			m_fFogParameter = *pDensity;

			m_Constans[CONSTANT_PARAMETER1].Type = CEffectCode::INPUT_VARIABLE;
			m_Constans[CONSTANT_PARAMETER1].uDetail = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR1_VARIABLE, 1, 1, false);
			m_Constans[CONSTANT_PARAMETER1].uCount = 1;
			m_Constans[CONSTANT_PARAMETER1].uIndex = 0;
			m_Constans[CONSTANT_PARAMETER1].pValue = &m_fFogParameter;
			m_Constans[CONSTANT_PARAMETER1].bIsForce = false;

			m_Constans[CONSTANT_PARAMETER2].Type = CEffectCode::INPUT_VARIABLE;
			m_Constans[CONSTANT_PARAMETER2].uDetail = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR1_VARIABLE, 1, 1, false);
			m_Constans[CONSTANT_PARAMETER2].uCount = 1;
			m_Constans[CONSTANT_PARAMETER2].uIndex = 0;
			m_Constans[CONSTANT_PARAMETER2].pValue = &g_fE;
			m_Constans[CONSTANT_PARAMETER2].bIsForce = false;
		}
	}

	m_Constans[CONSTANT_LOCAL_CAMERA_POS].Type = CEffectCode::CAMERA_VARIABLE;
	m_Constans[CONSTANT_LOCAL_CAMERA_POS].uDetail = CRenderMethod::CAMERA_LOCAL_POSITION;
	m_Constans[CONSTANT_LOCAL_CAMERA_POS].uCount = 1;
	m_Constans[CONSTANT_LOCAL_CAMERA_POS].uIndex = 0;
	m_Constans[CONSTANT_LOCAL_CAMERA_POS].pValue = ALCHEMY_NULL;
	m_Constans[CONSTANT_LOCAL_CAMERA_POS].bIsForce = false;

	m_Constans[CONSTANT_ONE].Type = CEffectCode::CONSTANT_VARIABLE;
	m_Constans[CONSTANT_ONE].uDetail = CRenderMethod::CONSTANT_ONE;
	m_Constans[CONSTANT_ONE].uCount = 1;
	m_Constans[CONSTANT_ONE].uIndex = 0;
	m_Constans[CONSTANT_ONE].pValue = ALCHEMY_NULL;
	m_Constans[CONSTANT_ONE].bIsForce = false;

	return true;
}

const CEffectCode::ATTRIBUTE   * CPixelFogBlendMethod::GetAttributes(UINT& uAttributeCount)
{
	uAttributeCount = TOTAL_ATTRIBUTES;

	return sm_ATTRIBUTEVARIABLES;
}

const CEffectCode::CONSTANT    * CPixelFogBlendMethod::GetConstants (UINT& uConstantCount )
{
	uConstantCount = TOTAL_CONSTANTS;

	return m_Constans;
}

const CEffectCode::VARIABLETYPE* CPixelFogBlendMethod::GetTemporary (UINT& uTemporaryCount)
{
	uTemporaryCount = TOTAL_TEMPORARY;

	return sm_TEMPORARYVARIABLES;
}

bool CPixelFogBlendMethod::OutputColorBlendHandler(
									 UINT uOutputColor, 
									 UINT uSourceColor,
									 UINT uDestinationColor,
									 const UINT* puAttributes, 
									 const UINT* puConstant,
									 const UINT* puTemporary,
									 CEffectCode::CInstructionSetInterface& ShaderDefiner)
{
	UINT uSubAddress[] = {0, 1, 2, 3};

	ShaderDefiner.AppendInstruction( CCompilerInterface::SUB, puTemporary[TEMPORARY_DISTANCE], uSubAddress,3, puAttributes[ ATTRIBUTE_POSITION ], uSubAddress, 3, puConstant[CONSTANT_LOCAL_CAMERA_POS], uSubAddress, 3);
	ShaderDefiner.AppendInstruction( CCompilerInterface::MUL, puTemporary[TEMPORARY_DISTANCE], uSubAddress, 1, puTemporary[TEMPORARY_DISTANCE], uSubAddress, 1, puTemporary[TEMPORARY_DISTANCE], uSubAddress, 1);
	ShaderDefiner.AppendInstruction( CCompilerInterface::MUL, puTemporary[TEMPORARY_DISTANCE], &uSubAddress[1], 1, puTemporary[TEMPORARY_DISTANCE], &uSubAddress[1], 1, puTemporary[TEMPORARY_DISTANCE], &uSubAddress[1], 1);
	ShaderDefiner.AppendInstruction( CCompilerInterface::MUL, puTemporary[TEMPORARY_DISTANCE], &uSubAddress[2], 1, puTemporary[TEMPORARY_DISTANCE], &uSubAddress[2], 1, puTemporary[TEMPORARY_DISTANCE], &uSubAddress[2], 1);

	ShaderDefiner.AppendInstruction( CCompilerInterface::ADD, puTemporary[TEMPORARY_DISTANCE], uSubAddress, 1, puTemporary[TEMPORARY_DISTANCE], uSubAddress, 1, puTemporary[TEMPORARY_DISTANCE], &uSubAddress[1], 1);
	ShaderDefiner.AppendInstruction( CCompilerInterface::ADD, puTemporary[TEMPORARY_DISTANCE], uSubAddress, 1, puTemporary[TEMPORARY_DISTANCE], uSubAddress, 1, puTemporary[TEMPORARY_DISTANCE], &uSubAddress[2], 1);

	ShaderDefiner.AppendInstruction( CCompilerInterface::SQT, puTemporary[TEMPORARY_DISTANCE], uSubAddress, 1,  puTemporary[ TEMPORARY_DISTANCE ],uSubAddress, 1,  0, ALCHEMY_NULL, 0);

	if(LINEAR == m_FogType)
	{
		ShaderDefiner.AppendInstruction( CCompilerInterface::SUB, puTemporary[TEMPORARY_DISTANCE], &uSubAddress[1], 1, puConstant[CONSTANT_PARAMETER1], uSubAddress, 1, puTemporary[TEMPORARY_DISTANCE], uSubAddress , 1);
		ShaderDefiner.AppendInstruction( CCompilerInterface::DIV, puTemporary[TEMPORARY_DISTANCE], uSubAddress, 1, puTemporary[TEMPORARY_DISTANCE], &uSubAddress[1], 1, puConstant[CONSTANT_PARAMETER2], uSubAddress , 1);
	}
	else if(EXP == m_FogType || EXP2 == m_FogType)
	{
		ShaderDefiner.AppendInstruction( CCompilerInterface::MUL, puTemporary[TEMPORARY_DISTANCE], &uSubAddress[1], 1, puTemporary[TEMPORARY_DISTANCE], &uSubAddress[0], 1, puConstant[CONSTANT_PARAMETER1], uSubAddress, 1);
		if(EXP2 == m_FogType)
			ShaderDefiner.AppendInstruction( CCompilerInterface::MUL, puTemporary[TEMPORARY_DISTANCE], &uSubAddress[1], 1, puTemporary[TEMPORARY_DISTANCE], &uSubAddress[1], 1, puTemporary[TEMPORARY_DISTANCE], &uSubAddress[1], 1 );

		ShaderDefiner.AppendInstruction( CCompilerInterface::POW, puTemporary[TEMPORARY_DISTANCE], uSubAddress, 1, puConstant[CONSTANT_PARAMETER2], uSubAddress, 1, puTemporary[TEMPORARY_DISTANCE], &uSubAddress[1], 1);
		ShaderDefiner.AppendInstruction( CCompilerInterface::DIV, puTemporary[TEMPORARY_DISTANCE], uSubAddress, 1, puConstant[CONSTANT_ONE], uSubAddress, 1 , puTemporary[TEMPORARY_DISTANCE], uSubAddress, 1 );
	}

	ShaderDefiner.AppendInstruction( CCompilerInterface::MUL, puTemporary[TEMPORARY_SCENE_COLOR], uSubAddress, 4,  puTemporary[TEMPORARY_DISTANCE], uSubAddress, 1, uSourceColor, uSubAddress ,4 );
	ShaderDefiner.AppendInstruction(CCompilerInterface::SUB,  puTemporary[TEMPORARY_FOG_COLOR], uSubAddress, 1,  puConstant[CONSTANT_ONE], uSubAddress, 1, puTemporary[TEMPORARY_DISTANCE], uSubAddress, 1);
	ShaderDefiner.AppendInstruction(CCompilerInterface::MUL,  puTemporary[TEMPORARY_FOG_COLOR], uSubAddress, 4,  puTemporary[TEMPORARY_FOG_COLOR], uSubAddress, 1, uDestinationColor, uSubAddress ,4);
	ShaderDefiner.AppendInstruction(CCompilerInterface::ADD, uOutputColor, puTemporary[TEMPORARY_SCENE_COLOR], puTemporary[TEMPORARY_FOG_COLOR]);

	return true;
}