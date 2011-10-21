#include "AlchemyPixelAmbientLightEffect.h"

#include "AlchemyRenderMethod.h"

using namespace alchemy;

const CEffectCode::ELEMENTPARAMETER CPixelAmbientLightEffect::sm_ATTRIBUTES[CPixelAmbientLightEffect::TOTAL_ATTRIBUTES] = { {CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION , 0} };
const CEffectCode::ELEMENTPARAMETER CPixelAmbientLightEffect::sm_VARYING   [CPixelAmbientLightEffect::TOTAL_VARYING   ] = { {CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITIONT, 0} };
const CEffectCode::GLOBALVARIABLE   CPixelAmbientLightEffect::sm_CONSTANS  [CPixelAmbientLightEffect::TOTAL_CONSTANTS ] = { {CEffectInterface::VERTEX_SHADER, CEffectCode::MATRIX_VARIABLE, CRenderMethod::WORLD_VIEW_PROJECTION_MATRIX, 1, 0, ALCHEMY_NULL}, {CEffectInterface::FRAGMENT_SHADER, CEffectCode::NORMAL_VARIABLE, CRenderMethodInterface::AMBIENT_LIGHT, 1, 0, ALCHEMY_NULL} };

CPixelAmbientLightEffect::CPixelAmbientLightEffect(void)
{
}

CPixelAmbientLightEffect::~CPixelAmbientLightEffect(void)
{
}

bool CPixelAmbientLightEffect::ShaderDefineHandler(
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
	if(!pPositiont->IsUsed)
		VertexShaderDefiner.AppendInstruction(CCompilerInterface::M44, pPositiont->uVertexShaderHandle, puAttributes[ATTRIBUTE_POSITION], puConstants[CONSTANT_WORLD_VIEW_PROJECTION_MATRIX]);

	FragmentShaderDefiner.AppendInstruction(CCompilerInterface::MOV, uOutputColor, puConstants[CONSTANT_AMBIENT_COLOR], 0);

	return true;
}