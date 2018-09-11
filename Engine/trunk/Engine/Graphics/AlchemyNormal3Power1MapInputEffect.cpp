#include "AlchemyNormal3Power1MapInputEffect.h"
#include "AlchemyRenderMethod.h"

using namespace alchemy;

const CEffectCode::ELEMENTDECLARATION           CNormal3Power1MapInputEffect::sm_VARING            [CNormal3Power1MapInputEffect::TOTAL_VARYING            ] = 
{
	{CVertexDeclaration::FLOAT4, CVertexDeclaration::NORMAL, 0}
};

const CEffectCode::GLOBALVARIABLE               CNormal3Power1MapInputEffect::sm_CONSTANS          [CNormal3Power1MapInputEffect::TOTAL_CONSTANTS          ] = 
{ 
	{IEffect::VERTEX_SHADER  , CEffectCode::CAMERA_VARIABLE  , CRenderMethod::CAMERA_WORLD_VIEW_MATRIX  , 1, 0, ALCHEMY_NULL}, 
	//{IEffect::VERTEX_SHADER  , CEffectCode::CAMERA_VARIABLE  , CRenderMethod::CONSTANT_ZERO    , 1, 0, ALCHEMY_NULL}, 
	{IEffect::VERTEX_SHADER  , CEffectCode::MATERIAL_VARIABLE, CRenderMethod::MATERIAL_INVERSE_255_POWER, 1, 0, ALCHEMY_NULL}, 
	{IEffect::FRAGMENT_SHADER, CEffectCode::CONSTANT_VARIABLE, CRenderMethod::CONSTANT_ONE              , 1, 0, ALCHEMY_NULL}, 
	{IEffect::FRAGMENT_SHADER, CEffectCode::CONSTANT_VARIABLE, CRenderMethod::CONSTANT_HALF_ONE         , 1, 0, ALCHEMY_NULL}
};

const CEffectCode::TEMPORARYDECLARATION         CNormal3Power1MapInputEffect::sm_TEMPORARY         [CNormal3Power1MapInputEffect::TOTAL_TEMPORARY          ] = 
{ 
	{IEffect::VERTEX_SHADER  , CEffectCode::VECTOR4_VARIABLE, 1}, 
	{IEffect::FRAGMENT_SHADER, CEffectCode::VECTOR4_VARIABLE, 1} 
};

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION CNormal3Power1MapInputEffect::sm_ATTRIBUTE_VARIABLE[CNormal3Power1MapInputEffect::TOTAL_ATTRIBUTE_VARIABLES] = 
{ 
	{IEffect::VERTEX_SHADER, CVertexDeclaration::FLOAT3, CVertexDeclaration::NORMAL  , 0, true, true}
};

CNormal3Power1MapInputEffect::CNormal3Power1MapInputEffect(void)
{
}

CNormal3Power1MapInputEffect::~CNormal3Power1MapInputEffect(void)
{
}

UINT                                            CNormal3Power1MapInputEffect::GetEffectInfoFlag     (                             )
{
	return ALCHEMY_FLAG(IEffectInfo::FRAGMENT_SHADER_OUTPUT);
}

const IEffectInfo::SAMPLER           * CNormal3Power1MapInputEffect::GetSamplers           (UINT& uSamplerCount          )
{
	uSamplerCount = TOTAL_SAMPLERS;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION            * CNormal3Power1MapInputEffect::GetAttributes        (UINT& uAttributeCount        )
{
	uAttributeCount = TOTAL_ATTRIBUTES;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION            * CNormal3Power1MapInputEffect::GetVarying           (UINT& uVaryingCount          )
{
	uVaryingCount = TOTAL_VARYING;

	return sm_VARING;
}

const CEffectCode::GLOBALVARIABLE              * CNormal3Power1MapInputEffect::GetConstants         (UINT& uConstantCount         )
{
	uConstantCount = TOTAL_CONSTANTS;

	return sm_CONSTANS;
}

const CEffectCode::TEMPORARYDECLARATION        * CNormal3Power1MapInputEffect::GetTemporary         (UINT& uTemporaryCount        )
{
	uTemporaryCount = TOTAL_TEMPORARY;

	return sm_TEMPORARY;
}

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CNormal3Power1MapInputEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
{
	uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLES;

	return sm_ATTRIBUTE_VARIABLE;
}

const CEffectCode::CONSTANTVARIABLEDECLARATION * CNormal3Power1MapInputEffect::GetConstantVariables  (UINT& uConstantVariableCount)
{
	uConstantVariableCount = TOTAL_CONSTANT_VARIABLES;

	return ALCHEMY_NULL;
}

bool CNormal3Power1MapInputEffect::ShaderDefineHandler(
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
	UINT uTemp = puTemporary[TEMPORARY_VERTEX_TEMP], uWorldViewMatrix = puConstants[CONSTANT_WORLD_VIEW_MATRIX];
	const CEffectCode::VARYING* pNormal = &pVarying[VARING_NORMAL];

	UINT uAddress[] = {0, 1, 2};
	UINT w = 3;
	VertexShaderDefiner.AppendInstruction(ICompiler::M33, pNormal->uVertexShaderHandle, uAddress, 3, pAttributeVariables[ATTRIBUTE_VARIABLE_NORMAL].uHandle, uAddress, 3, uWorldViewMatrix                     , ALCHEMY_NULL, 0);
	VertexShaderDefiner.AppendInstruction(ICompiler::MOV, pNormal->uVertexShaderHandle, &w      , 1, puConstants[CONSTANT_MATERIAL_INVERSE_255_POWER]      , uAddress, 1, 0                                    , ALCHEMY_NULL, 0);

	uTemp = puTemporary[TEMPORARY_FRAGMENT_TEMP];
	FragmentShaderDefiner.AppendInstruction(ICompiler::ADD, uTemp                       , uAddress, 3, pNormal->uFragmentShaderHandle                      , uAddress, 3, puConstants[CONSTANT_ONE]            , uAddress    , 1);
	FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uTemp                       , uAddress, 3, uTemp                                               , uAddress, 3, puConstants[CONSTANT_HALF_ONE]       , uAddress    , 1);
	FragmentShaderDefiner.AppendInstruction(ICompiler::MOV, uTemp                       , &w      , 1, pNormal->uFragmentShaderHandle                      , &w      , 1, 0                                    , ALCHEMY_NULL, 0);

	FragmentShaderDefiner.AppendInstruction(ICompiler::MOV, uOutputColor, uTemp, 0);

	return true;
}