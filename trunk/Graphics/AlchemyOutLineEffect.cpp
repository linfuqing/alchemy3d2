#include "AlchemyOutLineEffect.h"

using namespace alchemy;

//const CEffectInfoInterface::SAMPLER       CTextureEffect::sm_SAMPLERS  [CTextureEffect::TOTAL_SAMPLERS  ] = { { {CEffectCode::TEXTURE2D, CEffectCode::LINEAR, CEffectCode::LINEAR, CEffectCode::LINEAR, CEffectCode::WRAP, CEffectCode::WRAP, CEffectCode::WRAP}, 0} };
const CEffectCode::ELEMENTPARAMETER          	COutLineEffect::sm_ATTRIBUTES[COutLineEffect::TOTAL_ATTRIBUTES] = { {CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION , 0}, {CVertexDeclaration::FLOAT3, CVertexDeclaration::NORMAL, 0} };
const CEffectCode::ELEMENTPARAMETER          	COutLineEffect::sm_VARYING   [COutLineEffect::TOTAL_VARYING   ] = { {CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITIONT, 0}, {CVertexDeclaration::FLOAT4, CVertexDeclaration::COLOR, 0} };
const CEffectCode::GLOBALVARIABLE            	COutLineEffect::sm_CONSTANS  [COutLineEffect::TOTAL_CONSTANTS ] = { {CEffectInterface::VERTEX_SHADER, CEffectCode::MATRIX_VARIABLE, CRenderMethod::WORLD_VIEW_PROJECTION_MATRIX, 1, 0},  {CEffectInterface::VERTEX_SHADER, CEffectCode::CONSTANT_VARIABLE, CRenderMethod::CONSTANT_ZERO, 1, 0}, {CEffectInterface::VERTEX_SHADER, CEffectCode::CONSTANT_VARIABLE, CRenderMethod::CONSTANT_HALF_ONE, 1, 0} };
const CEffectCode::ATTRIBUTEVARIABLEDECLARATION COutLineEffect::sm_VARIABLE  [COutLineEffect::TOTAL_VARIABLE  ] = { {CEffectInterface::VERTEX_SHADER, CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION, 0, true, false} };

const CEffectCode::TEMPORARYDECLARATION			COutLineEffect::sm_TEMPORARY [COutLineEffect::TOTAL_TEMPORARY ] = { {CEffectInterface::VERTEX_SHADER, CEffectCode::VECTOR4_VARIABLE, 1} };

COutLineEffect::COutLineEffect()
{
	
}

COutLineEffect::~COutLineEffect(void)
{
}

bool COutLineEffect::ShaderDefineHandler(
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
	const CEffectCode::SHADERVARIABLE* pVariable = &pAttributeVariables[VARIABLE_POSITION];


	UINT subAddress[] = {0,1,2,3};

	VertexShaderDefiner.AppendInstruction(CCompilerInterface::MUL, puTemporary[TEMPORARY_POSITION], subAddress, 4, puAttributes[ATTRIBUTE_NORMAL], subAddress, 3, puConstants[CONSTANT_HALF], subAddress, 1);
	VertexShaderDefiner.AppendInstruction(CCompilerInterface::ADD, puTemporary[TEMPORARY_POSITION], subAddress, 3, puAttributes[ATTRIBUTE_POSITION], subAddress, 3, puTemporary[TEMPORARY_POSITION], subAddress, 3);
	VertexShaderDefiner.AppendInstruction(CCompilerInterface::MOV, puTemporary[TEMPORARY_POSITION], &subAddress[3], 1, puAttributes[ATTRIBUTE_NORMAL], &subAddress[3], 1, 0, ALCHEMY_NULL, 0);


	if(!pPositiont->IsUsed)
	{
		UINT uPosition = 0;

		if(pVariable->bIsUsed)
			uPosition = pVariable->uHandle;

		uPosition = uPosition ? uPosition : puTemporary[TEMPORARY_POSITION];

		VertexShaderDefiner.AppendInstruction(CCompilerInterface::M44, pPositiont->uVertexShaderHandle, uPosition, puConstants[CONSTANT_WORLD_VIEW_PROJECTION_MATRIX] );
	}

	VertexShaderDefiner.AppendInstruction(CCompilerInterface::MOV, pVarying[VARYING_OUTLINE_COLOR].uVertexShaderHandle, subAddress, 4, puConstants[CONSTANT_ZERO], subAddress, 1, 0, ALCHEMY_NULL, 0);

	FragmentShaderDefiner.AppendInstruction(CCompilerInterface::MOV, uOutputColor, pVarying[VARYING_OUTLINE_COLOR].uFragmentShaderHandle,0);

	return true;
}