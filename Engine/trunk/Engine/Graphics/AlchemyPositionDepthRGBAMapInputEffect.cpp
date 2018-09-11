#include "AlchemyPositionDepthRGBAMapInputEffect.h"
#include "AlchemyRenderMethod.h"

using namespace alchemy;

const CEffectCode::ELEMENTDECLARATION           CPositionDepthRGBAMapInputEffect::sm_VARING            [CPositionDepthRGBAMapInputEffect::TOTAL_VARYING            ] = 
{
	{CVertexDeclaration::FLOAT1, CVertexDeclaration::DEPTH, 0}
};

const CEffectCode::GLOBALVARIABLE               CPositionDepthRGBAMapInputEffect::sm_CONSTANS          [CPositionDepthRGBAMapInputEffect::TOTAL_CONSTANTS          ] = 
{ 
	{IEffect::VERTEX_SHADER  , CEffectCode::CAMERA_VARIABLE  , CRenderMethod::CAMERA_DISTANCE            , 1, 0, ALCHEMY_NULL}, 
	{IEffect::FRAGMENT_SHADER, CEffectCode::CONSTANT_VARIABLE, CRenderMethod::CONSTANT_FLOAT_TO_RGBA_CODE, 1, 0, ALCHEMY_NULL}
};

const CEffectCode::TEMPORARYDECLARATION         CPositionDepthRGBAMapInputEffect::sm_TEMPORARY         [CPositionDepthRGBAMapInputEffect::TOTAL_TEMPORARY          ] = 
{ 
	{IEffect::FRAGMENT_SHADER, CEffectCode::VECTOR4_VARIABLE, 1}, 
	{IEffect::FRAGMENT_SHADER, CEffectCode::VECTOR3_VARIABLE, 1} 
};

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION CPositionDepthRGBAMapInputEffect::sm_ATTRIBUTE_VARIABLE[CPositionDepthRGBAMapInputEffect::TOTAL_ATTRIBUTE_VARIABLES] = 
{ 
	{IEffect::VERTEX_SHADER  , CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITIONT, 0, true , true }
};


CPositionDepthRGBAMapInputEffect::CPositionDepthRGBAMapInputEffect(void)
{
}

CPositionDepthRGBAMapInputEffect::~CPositionDepthRGBAMapInputEffect(void)
{
}

UINT                                            CPositionDepthRGBAMapInputEffect::GetEffectInfoFlag     (                             )
{
	return ALCHEMY_FLAG(IEffectInfo::FRAGMENT_SHADER_OUTPUT);
}

const IEffectInfo::SAMPLER           * CPositionDepthRGBAMapInputEffect::GetSamplers           (UINT& uSamplerCount          )
{
	uSamplerCount = TOTAL_SAMPLERS;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION            * CPositionDepthRGBAMapInputEffect::GetAttributes        (UINT& uAttributeCount        )
{
	uAttributeCount = TOTAL_ATTRIBUTES;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION            * CPositionDepthRGBAMapInputEffect::GetVarying           (UINT& uVaryingCount          )
{
	uVaryingCount = TOTAL_VARYING;

	return sm_VARING;
}

const CEffectCode::GLOBALVARIABLE              * CPositionDepthRGBAMapInputEffect::GetConstants         (UINT& uConstantCount         )
{
	uConstantCount = TOTAL_CONSTANTS;

	return sm_CONSTANS;
}

const CEffectCode::TEMPORARYDECLARATION        * CPositionDepthRGBAMapInputEffect::GetTemporary         (UINT& uTemporaryCount        )
{
	uTemporaryCount = TOTAL_TEMPORARY;

	return sm_TEMPORARY;
}

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CPositionDepthRGBAMapInputEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
{
	uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLES;

	return sm_ATTRIBUTE_VARIABLE;
}

const CEffectCode::CONSTANTVARIABLEDECLARATION * CPositionDepthRGBAMapInputEffect::GetConstantVariables  (UINT& uConstantVariableCount)
{
	uConstantVariableCount = TOTAL_CONSTANT_VARIABLES;

	return ALCHEMY_NULL;
}

bool CPositionDepthRGBAMapInputEffect::ShaderDefineHandler(
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
	UINT uAddress[] = {0, 1, 2, 3};

	const CEffectCode::VARYING* pDepth = &pVarying[VARYING_DEPTH];
	if(!pDepth->IsUsed)
		VertexShaderDefiner.AppendInstruction(ICompiler::DIV, pDepth->uVertexShaderHandle, ALCHEMY_NULL, 0, pAttributeVariables[ATTRIBUTE_VARIABLE_POSITIONT].uHandle, &uAddress[3], 1, puConstants[CONSTANT_CAMERA_DISTANCE], uAddress, 1);

	UINT uRGBA = puTemporary[TEMPORARY_RGBA], uFloatToRGBACode = puConstants[CONSTANT_FLOAT_TO_RGBA_CODE];
	FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uRGBA, uFloatToRGBACode, pDepth->uFragmentShaderHandle);
	FragmentShaderDefiner.AppendInstruction(ICompiler::FRC, uRGBA, uRGBA           , 0                            );

	UINT uBias = puTemporary[TEMPORARY_BIAS];
	FragmentShaderDefiner.AppendInstruction(ICompiler::DIV, uBias, uAddress, 3, uRGBA, &uAddress[1], 3, uFloatToRGBACode, &uAddress[1], 1);
	FragmentShaderDefiner.AppendInstruction(ICompiler::SUB, uRGBA, uAddress, 3, uRGBA, uAddress    , 3, uBias           , uAddress    , 3);
	FragmentShaderDefiner.AppendInstruction(ICompiler::MOV, uOutputColor, uRGBA, 0);

	return true;
}