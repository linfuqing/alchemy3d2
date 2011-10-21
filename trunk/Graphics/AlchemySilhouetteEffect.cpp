#include "AlchemySilhouetteEffect.h"
#include "AlchemyRenderMethod.h"

using namespace alchemy;

const CEffectCode::ELEMENTPARAMETER					CSilhouetteEffect::sm_VARYING[ CSilhouetteEffect::TOTAL_VARYING ]						= { { CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITIONT,0 } };
const CEffectCode::TEMPORARYDECLARATION				CSilhouetteEffect::sm_TEMPORARYVARIABLE[ CSilhouetteEffect::TOTAL_TEMPORARY ]			= { { CEffectInterface::VERTEX_SHADER, CEffectCode::VECTOR3_VARIABLE }, { CEffectInterface::VERTEX_SHADER, CEffectCode::VECTOR1_VARIABLE }, { CEffectInterface::VERTEX_SHADER, CEffectCode::VECTOR1_VARIABLE },  { CEffectInterface::VERTEX_SHADER, CEffectCode::VECTOR4_VARIABLE } };
const CEffectCode::ATTRIBUTEVARIABLEDECLARATION		CSilhouetteEffect::sm_ATTRIBUTEVARIABLE[ CSilhouetteEffect::TOTAL_ATTRIBUTE_VARIABLE ]	= { { CEffectInterface::VERTEX_SHADER, CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION, 0, true, true},{ CEffectInterface::VERTEX_SHADER, CVertexDeclaration::FLOAT3, CVertexDeclaration::NORMAL, 0, true, true},{ CEffectInterface::VERTEX_SHADER, CVertexDeclaration::FLOAT3, CVertexDeclaration::NORMAL, 1, true, true},{ CEffectInterface::VERTEX_SHADER, CVertexDeclaration::FLOAT3, CVertexDeclaration::NORMAL, 2, true, true}  };

CSilhouetteEffect::CSilhouetteEffect(FLOAT4* pSiouetteColor, FLOAT* pWidth)
{
	m_Constans[CONSTANT_PROJECTION_MATRIX].ShaderType = CEffectInterface::VERTEX_SHADER;
	m_Constans[CONSTANT_PROJECTION_MATRIX].GlobalType = CEffectCode::MATRIX_VARIABLE;
	m_Constans[CONSTANT_PROJECTION_MATRIX].uDetail = CRenderMethod::PROJECTION_MATRIX;
	m_Constans[CONSTANT_PROJECTION_MATRIX].uCount = 1;
	m_Constans[CONSTANT_PROJECTION_MATRIX].uIndex = 0;
	m_Constans[CONSTANT_PROJECTION_MATRIX].pValue = ALCHEMY_NULL;

	m_Constans[CONSTANT_ZERO].ShaderType = CEffectInterface::VERTEX_SHADER;
	m_Constans[CONSTANT_ZERO].GlobalType = CEffectCode::CONSTANT_VARIABLE;
	m_Constans[CONSTANT_ZERO].uDetail = CRenderMethod::CONSTANT_ZERO;
	m_Constans[CONSTANT_ZERO].uCount = 1;
	m_Constans[CONSTANT_ZERO].uIndex = 0;
	m_Constans[CONSTANT_ZERO].pValue = ALCHEMY_NULL;

	m_Constans[CONSTANT_SIOUETTE_WIDTH].ShaderType = CEffectInterface::VERTEX_SHADER;
	m_Constans[CONSTANT_SIOUETTE_WIDTH].GlobalType = CEffectCode::INPUT_VARIABLE;
	m_Constans[CONSTANT_SIOUETTE_WIDTH].uDetail = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR1_VARIABLE, 1, 1, false);
	m_Constans[CONSTANT_SIOUETTE_WIDTH].uCount = 1;
	m_Constans[CONSTANT_SIOUETTE_WIDTH].uIndex = 0;
	m_Constans[CONSTANT_SIOUETTE_WIDTH].pValue = pWidth;

	m_Constans[CONSTANT_SIOUETTE_COLOR].ShaderType = CEffectInterface::FRAGMENT_SHADER;
	m_Constans[CONSTANT_SIOUETTE_COLOR].GlobalType = CEffectCode::INPUT_VARIABLE;
	m_Constans[CONSTANT_SIOUETTE_COLOR].uDetail = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR4_VARIABLE, 1, 4, false);
	m_Constans[CONSTANT_SIOUETTE_COLOR].uCount = 1;
	m_Constans[CONSTANT_SIOUETTE_COLOR].uIndex = 0;
	m_Constans[CONSTANT_SIOUETTE_COLOR].pValue = pSiouetteColor;
}

CSilhouetteEffect::~CSilhouetteEffect(void)
{

}

UINT CSilhouetteEffect::GetEffectInfoFlag()
{
	return ALCHEMY_FLAG( CEffectInfoInterface::VERTEX_SHADER_OUTPUT ) | ALCHEMY_FLAG( CEffectInfoInterface::FRAGMENT_SHADER_OUTPUT );
}

const CEffectInfoInterface::SAMPLER* CSilhouetteEffect::GetSamplers(UINT& uSamplerCount)
{
	uSamplerCount = TOTAL_SAMPLERS;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTPARAMETER* CSilhouetteEffect::GetAttributes(UINT& uAttributeCount)
{
	uAttributeCount = TOTAL_ATTRIBUTES;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTPARAMETER* CSilhouetteEffect::GetVarying(UINT& uVaryingCount)
{
	uVaryingCount = TOTAL_VARYING;

	return sm_VARYING;
}

const CEffectCode::GLOBALVARIABLE* CSilhouetteEffect::GetConstants(UINT& uConstantCount)
{
	uConstantCount = TOTAL_CONSTANTS;

	return m_Constans;
}

const CEffectCode::TEMPORARYDECLARATION* CSilhouetteEffect::GetTemporary(UINT& uTemporaryCount)
{
	uTemporaryCount = TOTAL_TEMPORARY;

	return sm_TEMPORARYVARIABLE;
}

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CSilhouetteEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
{
	uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLE;

	return sm_ATTRIBUTEVARIABLE;
}

const CEffectCode::CONSTANTVARIABLEDECLARATION * CSilhouetteEffect::GetConstantVariables (UINT& uConstantVariableCount )
{
	uConstantVariableCount = TOTAL_CONSTANT_VARIABLE;

	return ALCHEMY_NULL;
}

bool CSilhouetteEffect::ShaderDefineHandler(
						 UINT uOutputColor, 
						 const UINT* puSamplers, 
						 const UINT* puAttributes, 
						 const UINT* puConstants, 
						 const CEffectCode::VARYING* pVarying, 
						 const UINT* puTemporary,
						 const CEffectCode::SHADERVARIABLE* pAttributeVariable,
						 const CEffectCode::SHADERVARIABLE* pConstantVariable,
						 CEffectCode::CInstructionSetInterface& VertexShaderDefiner,
						 CEffectCode::CInstructionSetInterface& FragmentShaderDefiner,
						 CEffectCode::CPassDefiner::CEffectCodeUtils& EffectCodeUtils)
{
	UINT uSubAddress[4] = {0, 1, 2, 3};

	VertexShaderDefiner.AppendInstruction( CCompilerInterface::NRM, puTemporary[ TEMPORARY_EYE_DIRECTION ], uSubAddress, 3, pAttributeVariable[ ATTRIBUTE_VIEW_POSITION ].uHandle, uSubAddress, 3, 0, ALCHEMY_NULL, 0 );

	VertexShaderDefiner.AppendInstruction( CCompilerInterface::DP3, puTemporary[ TEMPORARY_DOT0 ], pAttributeVariable[ ATTRIBUTE_VIEW_NORMAL1  ].uHandle, puTemporary[ TEMPORARY_EYE_DIRECTION ]);

	VertexShaderDefiner.AppendInstruction( CCompilerInterface::DP3, puTemporary[ TEMPORARY_DOT1 ], pAttributeVariable[ ATTRIBUTE_VIEW_NORMAL2  ].uHandle, puTemporary[ TEMPORARY_EYE_DIRECTION ]);

	VertexShaderDefiner.AppendInstruction(CCompilerInterface::MUL, puTemporary[TEMPORARY_DOT0],  puTemporary[TEMPORARY_DOT0], puTemporary[TEMPORARY_DOT1] );

	VertexShaderDefiner.AppendInstruction(CCompilerInterface::SLT, puTemporary[TEMPORARY_DOT0], puTemporary[TEMPORARY_DOT0], puConstants[CONSTANT_ZERO] );

	VertexShaderDefiner.AppendInstruction(CCompilerInterface::MUL, puTemporary[TEMPORARY_DOT0], puTemporary[TEMPORARY_DOT0], puConstants[CONSTANT_SIOUETTE_WIDTH]);

	VertexShaderDefiner.AppendInstruction(CCompilerInterface::MUL, puTemporary[TEMPORARY_EYE_DIRECTION], puTemporary[TEMPORARY_DOT0], pAttributeVariable[ATTRIBUTE_VIEW_NORMAL0].uHandle);

	//VertexShaderDefiner.AppendInstruction(CCompilerInterface::MUL, puTemporary[TEMPORARY_EYE_DIRECTION], puConstants[CONSTANT_SIOUETTE_WIDTH], pAttributeVariable[ATTRIBUTE_VIEW_NORMAL0].uHandle);

	VertexShaderDefiner.AppendInstruction(CCompilerInterface::ADD, puTemporary[TEMPORARY_POSITION], uSubAddress, 3, pAttributeVariable[ ATTRIBUTE_VIEW_POSITION ].uHandle, uSubAddress, 3, puTemporary[TEMPORARY_EYE_DIRECTION], uSubAddress, 3);

	VertexShaderDefiner.AppendInstruction(CCompilerInterface::MOV, puTemporary[TEMPORARY_POSITION], &uSubAddress[3], 1, pAttributeVariable[ ATTRIBUTE_VIEW_POSITION ].uHandle, &uSubAddress[3], 1, 0, ALCHEMY_NULL, 0);
	VertexShaderDefiner.AppendInstruction( CCompilerInterface::M44, pVarying[VARYING_POSITIONT].uVertexShaderHandle, puTemporary[TEMPORARY_POSITION], puConstants[CONSTANT_PROJECTION_MATRIX]);

	FragmentShaderDefiner.AppendInstruction( CCompilerInterface::MOV, uOutputColor, puConstants[CONSTANT_SIOUETTE_COLOR], 0 );

	return true;
}