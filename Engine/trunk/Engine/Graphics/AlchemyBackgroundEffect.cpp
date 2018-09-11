#include "AlchemyBackgroundEffect.h"

using namespace alchemy;

//const CEffectCode::ELEMENTDECLARATION             CBackgroundEffect::sm_ATTRIBUTES       [CBackgroundEffect::TOTAL_ATTRIBUTES        ] = { {CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION , 0} };
const CEffectCode::ELEMENTDECLARATION             CBackgroundEffect::sm_VARYING          [CBackgroundEffect::TOTAL_VARYING           ] = 
{ 
	{CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITIONT, 0} 
};

const CEffectCode::GLOBALVARIABLE               CBackgroundEffect::sm_CONSTANS         [CBackgroundEffect::TOTAL_CONSTANTS         ] = 
{ 
	{IEffect::VERTEX_SHADER, CEffectCode::NORMAL_VARIABLE, CRenderMethod::WORLD_MATRIX, 1, 0} ,
	{IEffect::VERTEX_SHADER, CEffectCode::CAMERA_VARIABLE, CRenderMethod::CAMERA_VIEW_PROJECTION_MATRIX, 1, 0} ,
	{IEffect::VERTEX_SHADER, CEffectCode::CAMERA_VARIABLE, CRenderMethod::CAMERA_POSITION, 1, 0} 
};

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION CBackgroundEffect::sm_ATTRIBUTE_VARIABLE[CBackgroundEffect::TOTAL_ATTRIBUTE_VARIABLES] = 
{ 
	{IEffect::VERTEX_SHADER, CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION, 0, true, true} 
};


const CEffectCode::TEMPORARYDECLARATION			CBackgroundEffect::sm_TEMPRORAYS	    [TOTAL_TEMPORARY							 ] = { {IEffect::VERTEX_SHADER, CEffectCode::VECTOR4_VARIABLE} };

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
	CEffectCode::IInstructionSet& VertexShaderDefiner,
	CEffectCode::IInstructionSet& FragmentShaderDefiner,
	CEffectCode::CPassDefiner::CEffectCodeUtils& EffectCodeUtils)
{
	const CEffectCode::VARYING* pPositiont = &pVarying[VARYING_POSITIONT];
	const CEffectCode::SHADERVARIABLE* pVariable = &pAttributeVariables[ATTRIBUTE_VARIABLE_POSITION];

	UINT uSubAddress[] = {0, 1, 2, 3};
	VertexShaderDefiner.AppendInstruction(ICompiler::M33, puTemporary[TEMPORARY_POS], uSubAddress, 3, pVariable->uHandle, uSubAddress, 3, puConstants[CONSTANT_WORLD_MATRIX], ALCHEMY_NULL, 0);
	VertexShaderDefiner.AppendInstruction(ICompiler::MOV, puTemporary[TEMPORARY_POS], &uSubAddress[3], 1, pVariable->uHandle, &uSubAddress[3], 1, 0, ALCHEMY_NULL, 0);

	VertexShaderDefiner.AppendInstruction(ICompiler::ADD, puTemporary[TEMPORARY_POS], uSubAddress, 3, puTemporary[TEMPORARY_POS], uSubAddress, 3, puConstants[CONSTANT_CAMEERA_POSITION], uSubAddress, 3);
	VertexShaderDefiner.AppendInstruction(ICompiler::M44, puTemporary[TEMPORARY_POS], puTemporary[TEMPORARY_POS],  puConstants[CONSTANT_VIEW_PROJECTION_MATRIX]);

	//UINT z = 2,w = 3;
	//VertexShaderDefiner.AppendInstruction(ICompiler::MOV, puTemporary[TEMPORARY_POS], &z, 1, puTemporary[TEMPORARY_POS], &w, 1, 0, ALCHEMY_NULL, 0);

	VertexShaderDefiner.AppendInstruction(ICompiler::MOV, pPositiont->uVertexShaderHandle, puTemporary[TEMPORARY_POS], 0);

	return true;
}