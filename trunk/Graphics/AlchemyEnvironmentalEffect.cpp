#include "AlchemyEnvironmentalEffect.h"
#include "../Math/AlchemyMatrix4x4.h"

using namespace alchemy;

const CEffectCode::ELEMENTPARAMETER             CEnvironmentalEffect::sm_VARYING          [TOTAL_VARYING           ] = { { CVertexDeclaration::FLOAT3, CVertexDeclaration::TEXCOORD, 0 }  };
//const CEffectCode::GLOBALVARIABLE               CEnvironmentalEffect::sm_CONSTANS         [TOTAL_CONSTANTS         ] = { { CEffectInterface::VERTEX_SHADER, CEffectCode::CAMERA_VARIABLE, CRenderMethod::CAMERA_LOCAL_POSITION, 1, 0, ALCHEMY_NULL } };
const CEffectCode::ATTRIBUTEVARIABLEDECLARATION CEnvironmentalEffect::sm_ATTRIBUTE_VARIABLE[TOTAL_ATTRIBUTE_VARIABLES] = { { CEffectInterface::VERTEX_SHADER, CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION, 0, true, true}, { CEffectInterface::VERTEX_SHADER, CVertexDeclaration::FLOAT3, CVertexDeclaration::NORMAL, 0, true, true},   { CEffectInterface::VERTEX_SHADER, CVertexDeclaration::FLOAT3, CVertexDeclaration::TEXCOORD, 0, false, false } };
const CEffectCode::TEMPORARYDECLARATION			CEnvironmentalEffect::sm_TEMPRORAYS		  [TOTAL_TEMPORARY]			 = { { CEffectInterface::VERTEX_SHADER, CEffectCode::VECTOR4_VARIABLE }, { CEffectInterface::VERTEX_SHADER, CEffectCode::VECTOR3_VARIABLE }, { CEffectInterface::VERTEX_SHADER, CEffectCode::VECTOR3_VARIABLE }, { CEffectInterface::VERTEX_SHADER, CEffectCode::VECTOR1_VARIABLE } };


const FLOAT CEnvironmentalEffect::s_MUL_COE = 2.0f;

CEnvironmentalEffect::CEnvironmentalEffect(const CMatrix4x4* pInvertViewMatrix)
{
	m_Contans[CONSTANT_MUL_COE].ShaderType = CEffectInterface::VERTEX_SHADER;
	m_Contans[CONSTANT_MUL_COE].GlobalType = CEffectCode::INPUT_VARIABLE;
	m_Contans[CONSTANT_MUL_COE].uDetail = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR1_VARIABLE, 1, 1, false);
	m_Contans[CONSTANT_MUL_COE].uCount = 1;
	m_Contans[CONSTANT_MUL_COE].uIndex = 0;
	m_Contans[CONSTANT_MUL_COE].pValue = reinterpret_cast<const VOID*>(&s_MUL_COE);


	m_Contans[CONSTANT_WORLD_VIEW_MARTIX].ShaderType = CEffectInterface::VERTEX_SHADER;
	m_Contans[CONSTANT_WORLD_VIEW_MARTIX].GlobalType = CEffectCode::MATRIX_VARIABLE;
	m_Contans[CONSTANT_WORLD_VIEW_MARTIX].uDetail = CRenderMethod::WORLD_VIEW_MATRIX;
	m_Contans[CONSTANT_WORLD_VIEW_MARTIX].uCount = 1;
	m_Contans[CONSTANT_WORLD_VIEW_MARTIX].uIndex = 0;
	m_Contans[CONSTANT_WORLD_VIEW_MARTIX].pValue = ALCHEMY_NULL;

	m_Contans[CONSTANT_INVERT_VIEW_MATRIX].ShaderType = CEffectInterface::VERTEX_SHADER;
	m_Contans[CONSTANT_INVERT_VIEW_MATRIX].GlobalType = CEffectCode::INPUT_VARIABLE;
	m_Contans[CONSTANT_INVERT_VIEW_MATRIX].uDetail = CEffectCode::BuildGlobalInputCode(CEffectCode::MATRIX4X4_VARIABLE, 4, 16, false);
	m_Contans[CONSTANT_INVERT_VIEW_MATRIX].uCount = 1;
	m_Contans[CONSTANT_INVERT_VIEW_MATRIX].uIndex = 0;
	m_Contans[CONSTANT_INVERT_VIEW_MATRIX].pValue = pInvertViewMatrix;
}

CEnvironmentalEffect::~CEnvironmentalEffect()
{

}

bool CEnvironmentalEffect::ShaderDefineHandler(
						 UINT uOutputColor, 
						 const UINT* puSamplers, 
						 const UINT* puAttributes, 
						 const UINT* puConstants, 
						 const CEffectCode::VARYING* pVarying, 
						 const UINT* puTemporary,
						 const CEffectCode::SHADERVARIABLE* pAttributeVariables,
						 const CEffectCode::SHADERVARIABLE* pConstantVariables,
						 CEffectCode::CInstructionSetInterface& VertexShaderDefiner,
						 CEffectCode::CInstructionSetInterface& FragmentShaderDefiner,
						 CEffectCode::CPassDefiner::CEffectCodeUtils& EffectCodeUtils)
{
	UINT subAddress[] = {0, 1, 2};
	VertexShaderDefiner.AppendInstruction( CCompilerInterface::M44, puTemporary[ TEMPORARY_POS], ALCHEMY_NULL, 0, pAttributeVariables[ATTRIBUTE_VARIABLE_POSITION].uHandle, ALCHEMY_NULL, 0, puConstants[CONSTANT_WORLD_VIEW_MARTIX], ALCHEMY_NULL, 0);
	VertexShaderDefiner.AppendInstruction( CCompilerInterface::NEG, puTemporary[ TEMPORARY_POS], ALCHEMY_NULL, 0, puTemporary[TEMPORARY_POS],ALCHEMY_NULL, 0,  0, ALCHEMY_NULL, 0);
	VertexShaderDefiner.AppendInstruction( CCompilerInterface::NRM, puTemporary[ TEMPORARY_POS], subAddress, 3, puTemporary[TEMPORARY_POS], subAddress, 3, 0, ALCHEMY_NULL, 0);
	VertexShaderDefiner.AppendInstruction( CCompilerInterface::MOV, puTemporary[ TEMPORARY_CAMERA_POS_DIR], ALCHEMY_NULL, 0, puTemporary[TEMPORARY_POS], ALCHEMY_NULL, 0,  0, ALCHEMY_NULL, 0);

	VertexShaderDefiner.AppendInstruction( CCompilerInterface::M33, puTemporary[ TEMPORARY_POS_DIR ], subAddress, 3, pAttributeVariables[ ATTRIBUTE_VARIABLE_NORMAL ].uHandle, subAddress, 3, puConstants[CONSTANT_WORLD_VIEW_MARTIX], ALCHEMY_NULL, 0);
	VertexShaderDefiner.AppendInstruction( CCompilerInterface::NRM, puTemporary[ TEMPORARY_POS_DIR ], subAddress, 3, puTemporary[ TEMPORARY_POS_DIR ], subAddress, 3, 0, ALCHEMY_NULL, 0);
	VertexShaderDefiner.AppendInstruction( CCompilerInterface::DP3, puTemporary[ TEMPORARY_NORMAL_DOT ], subAddress, 1, puTemporary[ TEMPORARY_CAMERA_POS_DIR ], subAddress, 3, puTemporary[TEMPORARY_POS_DIR], subAddress, 3 );
	
	VertexShaderDefiner.AppendInstruction( CCompilerInterface::MUL, puTemporary[ TEMPORARY_NORMAL_DOT ], puTemporary[ TEMPORARY_NORMAL_DOT ], puConstants[CONSTANT_MUL_COE] );

	VertexShaderDefiner.AppendInstruction( CCompilerInterface::MUL, puTemporary[ TEMPORARY_POS ], subAddress, 3, puTemporary[ TEMPORARY_NORMAL_DOT ], subAddress, 1, puTemporary[TEMPORARY_POS_DIR], subAddress, 3 );

	
	VertexShaderDefiner.AppendInstruction( CCompilerInterface::SUB, puTemporary[ TEMPORARY_POS_DIR ], ALCHEMY_NULL, 0,  puTemporary[ TEMPORARY_POS ], ALCHEMY_NULL, 0,  puTemporary[ TEMPORARY_CAMERA_POS_DIR], ALCHEMY_NULL ,0);


	VertexShaderDefiner.AppendInstruction( CCompilerInterface::M33, puTemporary[ TEMPORARY_POS_DIR ], subAddress, 3, puTemporary[ TEMPORARY_POS_DIR ], subAddress, 3, puConstants[CONSTANT_INVERT_VIEW_MATRIX], ALCHEMY_NULL, 0);
	VertexShaderDefiner.AppendInstruction( CCompilerInterface::NRM, puTemporary[ TEMPORARY_POS_DIR ], puTemporary[ TEMPORARY_POS_DIR ], 0 );

	const CEffectCode::SHADERVARIABLE* pOutTexCoord = &pAttributeVariables[ATTRIBUTE_VARIABLE_UVW];
	if( pOutTexCoord->uHandle )
		VertexShaderDefiner.AppendInstruction( CCompilerInterface::MOV, pOutTexCoord->uHandle, ALCHEMY_NULL, 0, puTemporary[ TEMPORARY_POS_DIR ], ALCHEMY_NULL, 0, 0, ALCHEMY_NULL, 0);

	VertexShaderDefiner.AppendInstruction( CCompilerInterface::MOV, pVarying[VARYING_CUBE_UVW].uVertexShaderHandle, ALCHEMY_NULL, 0, puTemporary[ TEMPORARY_POS_DIR ], ALCHEMY_NULL, 0, 0, ALCHEMY_NULL, 0);

	return true;
}