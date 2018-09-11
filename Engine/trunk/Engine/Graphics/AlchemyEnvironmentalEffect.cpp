#include "AlchemyEnvironmentalEffect.h"
#include "../Math/AlchemyMatrix4x4.h"

using namespace alchemy;

const CEffectCode::ELEMENTDECLARATION             CEnvironmentalEffect::sm_VARYING          [TOTAL_VARYING           ] = { { CVertexDeclaration::FLOAT3, CVertexDeclaration::TEXCOORD, 0 }  };
//const CEffectCode::GLOBALVARIABLE               CEnvironmentalEffect::sm_CONSTANS         [TOTAL_CONSTANTS         ] = { { CEffectInterface::VERTEX_SHADER, CEffectCode::CAMERA_VARIABLE, CRenderMethod::CAMERA_LOCAL_POSITION, 1, 0, ALCHEMY_NULL } };
const CEffectCode::ATTRIBUTEVARIABLEDECLARATION CEnvironmentalEffect::sm_ATTRIBUTE_VARIABLE[TOTAL_ATTRIBUTE_VARIABLES] = { { IEffect::VERTEX_SHADER, CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION, 0, true, true}, { IEffect::VERTEX_SHADER, CVertexDeclaration::FLOAT3, CVertexDeclaration::NORMAL, 0, true, true},   { IEffect::VERTEX_SHADER, CVertexDeclaration::FLOAT3, CVertexDeclaration::TEXCOORD, 0, false, false } };
const CEffectCode::TEMPORARYDECLARATION			CEnvironmentalEffect::sm_TEMPRORAYS		  [TOTAL_TEMPORARY]			 = { { IEffect::VERTEX_SHADER, CEffectCode::VECTOR4_VARIABLE,1 }, { IEffect::VERTEX_SHADER, CEffectCode::VECTOR3_VARIABLE,1 }, { IEffect::VERTEX_SHADER, CEffectCode::VECTOR3_VARIABLE,1 }, { IEffect::VERTEX_SHADER, CEffectCode::VECTOR1_VARIABLE,1 } };


const FLOAT CEnvironmentalEffect::s_MUL_COE = 2.0f;

CEnvironmentalEffect::CEnvironmentalEffect()
{
	m_Contans[CONSTANT_MUL_COE].ShaderType = IEffect::VERTEX_SHADER;
	m_Contans[CONSTANT_MUL_COE].GlobalType = CEffectCode::INPUT_VARIABLE;
	m_Contans[CONSTANT_MUL_COE].uDetail = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR1_VARIABLE, 1, 1, false);
	m_Contans[CONSTANT_MUL_COE].uCount = 1;
	m_Contans[CONSTANT_MUL_COE].uIndex = 0;
	m_Contans[CONSTANT_MUL_COE].pValue = reinterpret_cast<const VOID*>(&s_MUL_COE);


	m_Contans[CONSTANT_WORLD_VIEW_MARTIX].ShaderType = IEffect::VERTEX_SHADER;
	m_Contans[CONSTANT_WORLD_VIEW_MARTIX].GlobalType = CEffectCode::CAMERA_VARIABLE;
	m_Contans[CONSTANT_WORLD_VIEW_MARTIX].uDetail = CRenderMethod::CAMERA_WORLD_VIEW_MATRIX;
	m_Contans[CONSTANT_WORLD_VIEW_MARTIX].uCount = 1;
	m_Contans[CONSTANT_WORLD_VIEW_MARTIX].uIndex = 0;
	m_Contans[CONSTANT_WORLD_VIEW_MARTIX].pValue = ALCHEMY_NULL;

	m_Contans[CONSTANT_CAMERA_WORLD_MATRIX].ShaderType = IEffect::VERTEX_SHADER;
	m_Contans[CONSTANT_CAMERA_WORLD_MATRIX].GlobalType = CEffectCode::CAMERA_VARIABLE;
	m_Contans[CONSTANT_CAMERA_WORLD_MATRIX].uDetail = CRenderMethod::CAMERA_INVERSE_VIEW_MATRIX;
	m_Contans[CONSTANT_CAMERA_WORLD_MATRIX].uCount = 1;
	m_Contans[CONSTANT_CAMERA_WORLD_MATRIX].uIndex = 0;
	m_Contans[CONSTANT_CAMERA_WORLD_MATRIX].pValue = ALCHEMY_NULL;
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
						 CEffectCode::IInstructionSet& VertexShaderDefiner,
						 CEffectCode::IInstructionSet& FragmentShaderDefiner,
						 CEffectCode::CPassDefiner::CEffectCodeUtils& EffectCodeUtils)
{
	UINT subAddress[] = {0, 1, 2};
	VertexShaderDefiner.AppendInstruction( ICompiler::M44, puTemporary[ TEMPORARY_POS], ALCHEMY_NULL, 0, pAttributeVariables[ATTRIBUTE_VARIABLE_POSITION].uHandle, ALCHEMY_NULL, 0, puConstants[CONSTANT_WORLD_VIEW_MARTIX], ALCHEMY_NULL, 0);
	VertexShaderDefiner.AppendInstruction( ICompiler::NEG, puTemporary[ TEMPORARY_POS], ALCHEMY_NULL, 0, puTemporary[TEMPORARY_POS],ALCHEMY_NULL, 0,  0, ALCHEMY_NULL, 0);
	VertexShaderDefiner.AppendInstruction( ICompiler::NRM, puTemporary[ TEMPORARY_POS], subAddress, 3, puTemporary[TEMPORARY_POS], subAddress, 3, 0, ALCHEMY_NULL, 0);
	VertexShaderDefiner.AppendInstruction( ICompiler::MOV, puTemporary[ TEMPORARY_CAMERA_POS_DIR], ALCHEMY_NULL, 0, puTemporary[TEMPORARY_POS], ALCHEMY_NULL, 0,  0, ALCHEMY_NULL, 0);

	VertexShaderDefiner.AppendInstruction( ICompiler::M33, puTemporary[ TEMPORARY_POS_DIR ], subAddress, 3, pAttributeVariables[ ATTRIBUTE_VARIABLE_NORMAL ].uHandle, subAddress, 3, puConstants[CONSTANT_WORLD_VIEW_MARTIX], ALCHEMY_NULL, 0);
	VertexShaderDefiner.AppendInstruction( ICompiler::NRM, puTemporary[ TEMPORARY_POS_DIR ], subAddress, 3, puTemporary[ TEMPORARY_POS_DIR ], subAddress, 3, 0, ALCHEMY_NULL, 0);
	VertexShaderDefiner.AppendInstruction( ICompiler::DP3, puTemporary[ TEMPORARY_NORMAL_DOT ], subAddress, 1, puTemporary[ TEMPORARY_CAMERA_POS_DIR ], subAddress, 3, puTemporary[TEMPORARY_POS_DIR], subAddress, 3 );
	
	VertexShaderDefiner.AppendInstruction( ICompiler::MUL, puTemporary[ TEMPORARY_NORMAL_DOT ], puTemporary[ TEMPORARY_NORMAL_DOT ], puConstants[CONSTANT_MUL_COE] );

	VertexShaderDefiner.AppendInstruction( ICompiler::MUL, puTemporary[ TEMPORARY_POS ], subAddress, 3, puTemporary[ TEMPORARY_NORMAL_DOT ], subAddress, 1, puTemporary[TEMPORARY_POS_DIR], subAddress, 3 );

	
	VertexShaderDefiner.AppendInstruction( ICompiler::SUB, puTemporary[ TEMPORARY_POS_DIR ], ALCHEMY_NULL, 0,  puTemporary[ TEMPORARY_POS ], ALCHEMY_NULL, 0,  puTemporary[ TEMPORARY_CAMERA_POS_DIR], ALCHEMY_NULL ,0);


	VertexShaderDefiner.AppendInstruction( ICompiler::M33, puTemporary[ TEMPORARY_POS_DIR ], subAddress, 3, puTemporary[ TEMPORARY_POS_DIR ], subAddress, 3, puConstants[CONSTANT_CAMERA_WORLD_MATRIX], ALCHEMY_NULL, 0);
	VertexShaderDefiner.AppendInstruction( ICompiler::NRM, puTemporary[ TEMPORARY_POS_DIR ], puTemporary[ TEMPORARY_POS_DIR ], 0 );

	const CEffectCode::SHADERVARIABLE* pOutTexCoord = &pAttributeVariables[ATTRIBUTE_VARIABLE_UVW];
	if( pOutTexCoord->uHandle )
		VertexShaderDefiner.AppendInstruction( ICompiler::MOV, pOutTexCoord->uHandle, ALCHEMY_NULL, 0, puTemporary[ TEMPORARY_POS_DIR ], ALCHEMY_NULL, 0, 0, ALCHEMY_NULL, 0);

	VertexShaderDefiner.AppendInstruction( ICompiler::MOV, pVarying[VARYING_CUBE_UVW].uVertexShaderHandle, ALCHEMY_NULL, 0, puTemporary[ TEMPORARY_POS_DIR ], ALCHEMY_NULL, 0, 0, ALCHEMY_NULL, 0);

	return true;
}