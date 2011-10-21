#include "AlchemyBackgroundEffect.h"

using namespace alchemy;

//const CEffectCode::ELEMENTPARAMETER             CBackgroundEffect::sm_ATTRIBUTES       [CBackgroundEffect::TOTAL_ATTRIBUTES        ] = { {CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION , 0} };
const CEffectCode::ELEMENTPARAMETER             CBackgroundEffect::sm_VARYING          [CBackgroundEffect::TOTAL_VARYING           ] = { {CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITIONT, 0} };
const CEffectCode::GLOBALVARIABLE               CBackgroundEffect::sm_CONSTANS         [CBackgroundEffect::TOTAL_CONSTANTS         ] = { {CEffectInterface::VERTEX_SHADER, CEffectCode::MATRIX_VARIABLE, CRenderMethod::WORLD_VIEW_PROJECTION_MATRIX, 1, 0} };
const CEffectCode::ATTRIBUTEVARIABLEDECLARATION CBackgroundEffect::sm_ATTRIBUTE_VARIABLE[CBackgroundEffect::TOTAL_ATTRIBUTE_VARIABLES] = { {CEffectInterface::VERTEX_SHADER, CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION, 0, true, true} };


const CEffectCode::TEMPORARYDECLARATION			CBackgroundEffect::sm_TEMPRORAYS	    [TOTAL_TEMPORARY							 ] = { {CEffectInterface::VERTEX_SHADER, CEffectCode::VECTOR4_VARIABLE} };

CBackgroundEffect::CBackgroundEffect()
{

}

CBackgroundEffect::~CBackgroundEffect(void)
{
}

bool CBackgroundEffect::ShaderDefineHandler(
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
	const CEffectCode::VARYING* pPositiont = &pVarying[VARYING_POSITIONT];
	const CEffectCode::SHADERVARIABLE* pVariable = &pAttributeVariables[ATTRIBUTE_VARIABLE_POSITION];

	if(pPositiont->IsUsed)
		VertexShaderDefiner.AppendInstruction(CCompilerInterface::MOV, puTemporary[TEMPORARY_POS], pPositiont->uVertexShaderHandle, 0);
	else
		VertexShaderDefiner.AppendInstruction(CCompilerInterface::M44, puTemporary[TEMPORARY_POS],  pVariable->uHandle,  puConstants[CONSTANT_WORLD_VIEW_PROJECTION_MATRIX]);


	UINT z = 2,w = 3;
	VertexShaderDefiner.AppendInstruction(CCompilerInterface::MOV, puTemporary[TEMPORARY_POS], &z, 1, puTemporary[TEMPORARY_POS], &w, 1, 0, ALCHEMY_NULL, 0);

	VertexShaderDefiner.AppendInstruction(CCompilerInterface::MOV, pPositiont->uVertexShaderHandle, puTemporary[TEMPORARY_POS], 0);

	return true;
}