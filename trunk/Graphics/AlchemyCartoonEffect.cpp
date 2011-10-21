#include "AlchemyCartoonEffect.h"
#include "AlchemyRenderMethod.h"

using namespace alchemy;


const CEffectCode::ELEMENTPARAMETER				CCartoonEffect::sm_ATTRIBUTES[TOTAL_ATTRIBUTES] = {  {CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION , 0}, {CVertexDeclaration::FLOAT3, CVertexDeclaration::NORMAL, 0} };
const CEffectCode::ELEMENTPARAMETER				CCartoonEffect::sm_VARYING   [TOTAL_VARYING   ] = {  {CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITIONT, 0}, {CVertexDeclaration::FLOAT2, CVertexDeclaration::COLOR, 0}/*, {CVertexDeclaration::FLOAT4, CVertexDeclaration::COLOR, 0}*/ };
const CEffectCode::ATTRIBUTEVARIABLEDECLARATION CCartoonEffect::sm_VARIABLE  [TOTAL_VARIABLE  ] = {  {CEffectInterface::VERTEX_SHADER, CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION, 0, true, false} };
const CEffectCode::TEMPORARYDECLARATION			CCartoonEffect::sm_TEMPORARY [TOTAL_TEMPORARY ] = {  {CEffectInterface::VERTEX_SHADER, CEffectCode::VECTOR4_VARIABLE, 1}, {CEffectInterface::VERTEX_SHADER, CEffectCode::VECTOR3_VARIABLE, 1}, {CEffectInterface::VERTEX_SHADER, CEffectCode::VECTOR3_VARIABLE, 1}, {CEffectInterface::VERTEX_SHADER, CEffectCode::VECTOR2_VARIABLE, 1},{CEffectInterface::FRAGMENT_SHADER, CEffectCode::VECTOR4_VARIABLE, 1}/*,  {CEffectInterface::VERTEX_SHADER, CEffectCode::VECTOR3_VARIABLE}, {CEffectInterface::VERTEX_SHADER, CEffectCode::VECTOR1_VARIABLE}*/ };
const CEffectCode::GLOBALVARIABLE				CCartoonEffect::sm_CONSTANS  [TOTAL_CONSTANTS ] = {  {CEffectInterface::VERTEX_SHADER, CEffectCode::MATRIX_VARIABLE, CRenderMethod::WORLD_VIEW_PROJECTION_MATRIX, 1, 0},  {CEffectInterface::VERTEX_SHADER, CEffectCode::LIGHT_VARIABLE, CRenderMethod::LIGHT_LOCAL_POSITION, 1, 0},{CEffectInterface::VERTEX_SHADER, CEffectCode::CONSTANT_VARIABLE, CRenderMethod::CONSTANT_ZERO, 1, 0}/*,{CEffectInterface::VERTEX_SHADER, CEffectCode::CAMERA_VARIABLE, CRenderMethod::CAMERA_LOCAL_POSITION, 1, 0}*/ };

CCartoonEffect::CCartoonEffect(UINT sampleIndex,UINT level)
:m_LightLevel(level)
{
	m_SAMPLER[SAMPLER_GRAY].uIndex = sampleIndex;
	m_SAMPLER[SAMPLER_GRAY].Declaration.Type = CEffectCode::TEXTURE2D;
	m_SAMPLER[SAMPLER_GRAY].Declaration.MipFilter = CEffectCode::NONE;
	m_SAMPLER[SAMPLER_GRAY].Declaration.MinFilter = CEffectCode::NONE;
	m_SAMPLER[SAMPLER_GRAY].Declaration.MagFilter = CEffectCode::NONE;
	m_SAMPLER[SAMPLER_GRAY].Declaration.AddressU = CEffectCode::CLAMP;
	m_SAMPLER[SAMPLER_GRAY].Declaration.AddressV = CEffectCode::CLAMP;
	m_SAMPLER[SAMPLER_GRAY].Declaration.AddressW = CEffectCode::CLAMP;
}

CCartoonEffect::~CCartoonEffect(void)
{

}

//#include <as3.h>
bool CCartoonEffect::ShaderDefineHandler(
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
	{
		const CEffectCode::SHADERVARIABLE* pVariable = &pAttributeVariables[VARIABLE_POSITION];

		UINT uPosition = 0;
		if(pVariable->bIsUsed)
			uPosition = pVariable->uHandle;

		uPosition = uPosition ? uPosition : puAttributes[ATTRIBUTE_POSITION];
		VertexShaderDefiner.AppendInstruction(CCompilerInterface::M44, pPositiont->uVertexShaderHandle, uPosition, puConstants[CONSTANT_WORLD_VIEW_PROJECTION_MATRIX]);
	}

	VertexShaderDefiner.AppendInstruction(CCompilerInterface::MOV,	puTemporary[TEMPORARY_NORMAL],		puAttributes[ATTRIBUTE_NORMAL],		0);
	VertexShaderDefiner.AppendInstruction(CCompilerInterface::MOV,	puTemporary[TEMPORARY_POSITION],	puAttributes[ATTRIBUTE_POSITION],		0);

	/*VertexShaderDefiner.AppendInstruction(CCompilerInterface::M33,	puTemporary[TEMPORARY_NORMAL],	puAttributes[ATTRIBUTE_NORMAL],		puConstants[CONSTANT_WORLD_MATRIX]);
	VertexShaderDefiner.AppendInstruction(CCompilerInterface::NRM,	puTemporary[TEMPORARY_NORMAL],	puTemporary[TEMPORARY_NORMAL],		0);
	VertexShaderDefiner.AppendInstruction(CCompilerInterface::M44,	puTemporary[TEMPORARY_POSITION], puAttributes[ATTRIBUTE_POSITION],	puConstants[CONSTANT_WORLD_MATRIX]);*/

	UINT subAddress[] = {0,1,2,3};

	VertexShaderDefiner.AppendInstruction(CCompilerInterface::SUB, puTemporary[TEMPORARY_LIGHT_DIR], puConstants[CONSTANT_LIGHT_POSITION], puTemporary[TEMPORARY_POSITION]);

	VertexShaderDefiner.AppendInstruction(CCompilerInterface::NRM, puTemporary[TEMPORARY_LIGHT_DIR],puTemporary[TEMPORARY_LIGHT_DIR], 0);

	VertexShaderDefiner.AppendInstruction(CCompilerInterface::MOV, puTemporary[TEMPORARY_UV], ALCHEMY_NULL, 0,  puConstants[CONSTANT_V_ZERO], ALCHEMY_NULL,0,  0, ALCHEMY_NULL,0);

	VertexShaderDefiner.AppendInstruction(CCompilerInterface::DP3, puTemporary[TEMPORARY_UV], subAddress, 1, 
		puTemporary[TEMPORARY_NORMAL], subAddress, 3, puTemporary[TEMPORARY_LIGHT_DIR], subAddress, 3);

	VertexShaderDefiner.AppendInstruction(CCompilerInterface::SAT,	puTemporary[TEMPORARY_UV], subAddress, 1, 
		puTemporary[TEMPORARY_UV], subAddress, 1, 0, ALCHEMY_NULL, 0);


	VertexShaderDefiner.AppendInstruction(CCompilerInterface::MOV, pVarying[VARYING_CARTOON_UV].uVertexShaderHandle,  ALCHEMY_NULL, 0, puTemporary[TEMPORARY_UV],  ALCHEMY_NULL, 0, 0,  ALCHEMY_NULL, 0);



	/*VertexShaderDefiner.AppendInstruction(CCompilerInterface::SUB, puTemporary[TEMPORARY_EYE_DIR],  puConstants[CONSTANT_CAMERA_LOCAL_POS], puAttributes[ATTRIBUTE_POSITION]);
	VertexShaderDefiner.AppendInstruction(CCompilerInterface::NRM, puTemporary[TEMPORARY_EYE_DIR],	puTemporary[TEMPORARY_EYE_DIR],			0);
	VertexShaderDefiner.AppendInstruction(CCompilerInterface::DP3, puTemporary[TEMPORARY_DOT],		puTemporary[TEMPORARY_EYE_DIR],			puAttributes[ATTRIBUTE_NORMAL]);

	VertexShaderDefiner.AppendInstruction(CCompilerInterface::SLT, pVarying[VARYING_OUTLINE_COLOR].uVertexShaderHandle, subAddress, 4, puConstants[CONSTANT_V_ZERO], subAddress, 1, puTemporary[TEMPORARY_DOT], subAddress, 1);
	*/

	FragmentShaderDefiner.AppendInstruction(CCompilerInterface::TEX, puTemporary[TEMPORARY_COLOR],	pVarying[VARYING_CARTOON_UV].uFragmentShaderHandle,	puSamplers[SAMPLER_GRAY]);
	//FragmentShaderDefiner.AppendInstruction(CCompilerInterface::MUL, puTemporary[TEMPORARY_COLOR],  puTemporary[TEMPORARY_COLOR], pVarying[VARYING_OUTLINE_COLOR].uFragmentShaderHandle);

	FragmentShaderDefiner.AppendInstruction(CCompilerInterface::MOV, uOutputColor, ALCHEMY_NULL, 0,	puTemporary[TEMPORARY_COLOR], ALCHEMY_NULL, 0, 0, ALCHEMY_NULL, 0);

	return true;
}