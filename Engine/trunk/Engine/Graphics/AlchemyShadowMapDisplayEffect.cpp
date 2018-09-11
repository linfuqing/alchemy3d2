#include "AlchemyShadowMapDisplayEffect.h"
#include "AlchemyRenderMethod.h"

using namespace alchemy;

const FLOAT CShadowMapDisplayEffect::sm_fEpsilon = 0.005f;

//const CEffectCode::ELEMENTDECLARATION             CShadowMapDisplayEffect::sm_ATTRIBUTES       [CShadowMapDisplayEffect::TOTAL_ATTRIBUTES        ] = { {CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION , 0} };
const CEffectCode::TEMPORARYDECLARATION         CShadowMapDisplayEffect::sm_TEMPORARY        [CShadowMapDisplayEffect::TOTAL_TEMPORARY         ] = { {IEffect::VERTEX_SHADER, CEffectCode::VECTOR4_VARIABLE, 1}, {IEffect::FRAGMENT_SHADER, CEffectCode::VECTOR3_VARIABLE, 1}, {IEffect::FRAGMENT_SHADER, CEffectCode::VECTOR2_VARIABLE, 1}, {IEffect::FRAGMENT_SHADER, CEffectCode::VECTOR2_VARIABLE, 1}, {IEffect::FRAGMENT_SHADER, CEffectCode::VECTOR1_VARIABLE, 1} };
const CEffectCode::ATTRIBUTEVARIABLEDECLARATION CShadowMapDisplayEffect::sm_ATTRIBUTE_VARIABLE[CShadowMapDisplayEffect::TOTAL_ATTRIBUTE_VARIABLES] = { {IEffect::VERTEX_SHADER, CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION, 0, true, true} };


CShadowMapDisplayEffect::CShadowMapDisplayEffect(UINT uShadowMapIndex, UINT uLightIndex, UINT uCameraIndex) 
{
	LPSAMPLER pSampler              = &m_Samplers[SAMPLER_DEPTH];
	pSampler->Type                  = IEffect::FRAGMENT_SHADER;
	pSampler->Declaration.Type      = IEffect::TEXTURE2D;
	pSampler->Declaration.MinFilter = IEffect::LINEAR;
	pSampler->Declaration.MagFilter = IEffect::LINEAR;
	pSampler->Declaration.MipFilter = IEffect::LINEAR;
	pSampler->Declaration.AddressU  = IEffect::CLAMP;
	pSampler->Declaration.AddressV  = IEffect::CLAMP;
	pSampler->Declaration.AddressW  = IEffect::CLAMP;
	pSampler->uIndex                = uShadowMapIndex;

	CEffectCode::LPELEMENTDECLARATION pVarying;

	pVarying         = &m_Varying[VARYING_POSITION];
	pVarying->Type   = CVertexDeclaration::FLOAT4;
	pVarying->Usage  = CVertexDeclaration::POSITION;
	pVarying->uIndex = 0;

	pVarying         = &m_Varying[VARYING_LIGHT_POSITION];
	pVarying->Type   = CVertexDeclaration::FLOAT4;
	pVarying->Usage  = CVertexDeclaration::TEXCOORD;
	pVarying->uIndex = uShadowMapIndex;

	CEffectCode::LPGLOBALVARIABLE pConstant;

	pConstant             = &m_Contants[CONSTANT_HALF_ONE];

	pConstant->ShaderType = IEffect::VERTEX_SHADER;
	pConstant->GlobalType = CEffectCode::CONSTANT_VARIABLE;
	pConstant->uDetail    = CRenderMethod::CONSTANT_HALF_ONE;
	pConstant->uCount     = 1;
	pConstant->uIndex     = 0;
	pConstant->pValue     = ALCHEMY_NULL;

	pConstant             = &m_Contants[CONSTANT_CAMERA_NEAR];

	pConstant->ShaderType = IEffect::FRAGMENT_SHADER;
	pConstant->GlobalType = CEffectCode::CAMERA_VARIABLE;
	pConstant->uDetail    = CRenderMethod::CAMERA_NEAR;
	pConstant->uCount     = 1;
	pConstant->uIndex     = uCameraIndex;
	pConstant->pValue     = ALCHEMY_NULL;

	pConstant             = &m_Contants[CONSTANT_CAMERA_DISTANCE];

	pConstant->ShaderType = IEffect::FRAGMENT_SHADER;
	pConstant->GlobalType = CEffectCode::CAMERA_VARIABLE;
	pConstant->uDetail    = CRenderMethod::CAMERA_DISTANCE;
	pConstant->uCount     = 1;
	pConstant->uIndex     = uCameraIndex;
	pConstant->pValue     = ALCHEMY_NULL;

	pConstant             = &m_Contants[CONSTANT_LIGHT_WORLD_VIEW_PROJECTION_MATRIX];

	pConstant->ShaderType = IEffect::VERTEX_SHADER;
	pConstant->GlobalType = CEffectCode::CAMERA_VARIABLE;
	pConstant->uDetail    = CRenderMethod::CAMERA_WORLD_VIEW_PROJECTION_MATRIX;
	pConstant->uCount     = 1;
	pConstant->uIndex     = uCameraIndex;
	pConstant->pValue     = ALCHEMY_NULL;

	pConstant             = &m_Contants[CONSTANT_LIGHT_LOCAL_POSITION];

	pConstant->ShaderType = IEffect::FRAGMENT_SHADER;
	pConstant->GlobalType = CEffectCode::LIGHT_VARIABLE;
	pConstant->uDetail    = CRenderMethod::LIGHT_LOCAL_POSITION;
	pConstant->uCount     = 1;
	pConstant->uIndex     = uLightIndex;
	pConstant->pValue     = ALCHEMY_NULL;

	pConstant             = &m_Contants[CONSTANT_LIGHT_LOCAL_DIRECTION];

	pConstant->ShaderType = IEffect::FRAGMENT_SHADER;
	pConstant->GlobalType = CEffectCode::LIGHT_VARIABLE;
	pConstant->uDetail    = CRenderMethod::LIGHT_LOCAL_DIRECTION;
	pConstant->uCount     = 1;
	pConstant->uIndex     = uLightIndex;
	pConstant->pValue     = ALCHEMY_NULL;

	pConstant             = &m_Contants[CONSTANT_LIGHT_COS_THETA];

	pConstant->ShaderType = IEffect::FRAGMENT_SHADER;
	pConstant->GlobalType = CEffectCode::LIGHT_VARIABLE;
	pConstant->uDetail    = CRenderMethod::LIGHT_COS_THETA;
	pConstant->uCount     = 1;
	pConstant->uIndex     = uLightIndex;
	pConstant->pValue     = ALCHEMY_NULL;

	pConstant             = &m_Contants[CONSTANT_EPSILON];

	pConstant->ShaderType = IEffect::FRAGMENT_SHADER;
	pConstant->GlobalType = CEffectCode::INPUT_VARIABLE;
	pConstant->uDetail    = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR1_VARIABLE, 1, 1, false);
	pConstant->uCount     = 1;
	pConstant->uIndex     = 0;
	pConstant->pValue     = reinterpret_cast<const VOID*>(&sm_fEpsilon);

	CEffectCode::LPCONSTANTVARIABLEDECLARATION pConstantVariables;

	pConstantVariables = &m_ConstantVariable[CONSTANT_VARIABLE_LIGHT_DIFFUSE];

	pConstantVariables->ShaderType  = IEffect::FRAGMENT_SHADER;
	pConstantVariables->GlobalType  = CEffectCode::LIGHT_VARIABLE;
	pConstantVariables->uDetail     = CRenderMethod::LIGHT_DIFFUSE;
	pConstantVariables->uCount      = 1;
	pConstantVariables->uIndex      = uLightIndex;
	pConstantVariables->pValue      = ALCHEMY_NULL;
	pConstantVariables->bIsReadOnly = false;
	pConstantVariables->bIsForce    = true;

	pConstantVariables = &m_ConstantVariable[CONSTANT_VARIABLE_LIGHT_SPECULAR];

	pConstantVariables->ShaderType  = IEffect::FRAGMENT_SHADER;
	pConstantVariables->GlobalType  = CEffectCode::LIGHT_VARIABLE;
	pConstantVariables->uDetail     = CRenderMethod::LIGHT_SPECULAR;
	pConstantVariables->uCount      = 1;
	pConstantVariables->uIndex      = uLightIndex;
	pConstantVariables->pValue      = ALCHEMY_NULL;
	pConstantVariables->bIsReadOnly = false;
	pConstantVariables->bIsForce    = true;
}

CShadowMapDisplayEffect::~CShadowMapDisplayEffect(void)
{
}

UINT CShadowMapDisplayEffect::GetEffectInfoFlag()
{
	return ALCHEMY_FLAG(IEffectInfo::FRAGMENT_SHADER_INFO) | ALCHEMY_FLAG(IEffectInfo::FRAGMENT_SHADER_OUTPUT);
}

const IEffectInfo::SAMPLER* CShadowMapDisplayEffect::GetSamplers(UINT& uSamplerCount)
{
	uSamplerCount = TOTAL_SAMPLERS;

	return m_Samplers;
}

const CEffectCode::ELEMENTDECLARATION* CShadowMapDisplayEffect::GetAttributes(UINT& uAttributeCount)
{
	uAttributeCount = TOTAL_ATTRIBUTES;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION* CShadowMapDisplayEffect::GetVarying(UINT& uVaryingCount  )
{
	uVaryingCount = TOTAL_VARYING;

	return m_Varying;
}

const CEffectCode::GLOBALVARIABLE* CShadowMapDisplayEffect::GetConstants(UINT& uConstantCount )
{
	uConstantCount = TOTAL_CONSTANTS;

	return m_Contants;
}

const CEffectCode::TEMPORARYDECLARATION* CShadowMapDisplayEffect::GetTemporary(UINT& uTemporaryCount)
{
	uTemporaryCount = TOTAL_TEMPORARY;

	return sm_TEMPORARY;
}

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CShadowMapDisplayEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
{
	uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLES;

	return sm_ATTRIBUTE_VARIABLE;
}

const CEffectCode::CONSTANTVARIABLEDECLARATION * CShadowMapDisplayEffect::GetConstantVariables(UINT& uConstantVariableCount)
{
	uConstantVariableCount = TOTAL_CONSTANT_VARIABLES;

	return m_ConstantVariable;
}

bool CShadowMapDisplayEffect::ShaderDefineHandler(
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
	UINT uPosition = pAttributeVariables[ATTRIBUTE_VARIABLE_POSITION].uHandle;

	const CEffectCode::VARYING* pPosition = &pVarying[VARYING_POSITION];
	if(!pPosition->IsUsed)
		VertexShaderDefiner.AppendInstruction(ICompiler::MOV, pPosition->uVertexShaderHandle, uPosition, 0);

	static const UINT s_uADDRESS[] = {0, 1, 2, 3};

	UINT uLightPosition = puTemporary[TEMPORARY_LIGHT_POSITION];
	VertexShaderDefiner.AppendInstruction(ICompiler::M44, uLightPosition, uPosition, puConstants[CONSTANT_LIGHT_WORLD_VIEW_PROJECTION_MATRIX]);
	VertexShaderDefiner.AppendInstruction(ICompiler::NEG, uLightPosition, &s_uADDRESS[1], 1, uLightPosition, &s_uADDRESS[1], 1, 0                             , ALCHEMY_NULL  , 0);
	VertexShaderDefiner.AppendInstruction(ICompiler::ADD, uLightPosition, s_uADDRESS    , 2, uLightPosition, s_uADDRESS    , 2, uLightPosition                , &s_uADDRESS[3], 1);
	VertexShaderDefiner.AppendInstruction(ICompiler::MUL, uLightPosition, s_uADDRESS    , 2, uLightPosition, s_uADDRESS    , 2, puConstants[CONSTANT_HALF_ONE], s_uADDRESS    , 1);

	const CEffectCode::VARYING* pLightPosition = &pVarying[VARYING_LIGHT_POSITION];
	VertexShaderDefiner.AppendInstruction(ICompiler::MOV, pLightPosition->uVertexShaderHandle, uLightPosition, 0);

	UINT uLightDirection = puTemporary[TEMPORARY_LIGHT_DIRECTION];
	FragmentShaderDefiner.AppendInstruction(ICompiler::SUB, uLightDirection, s_uADDRESS, 3, pPosition->uFragmentShaderHandle, s_uADDRESS, 3, puConstants[CONSTANT_LIGHT_LOCAL_POSITION], s_uADDRESS, 3);
	FragmentShaderDefiner.AppendInstruction(ICompiler::NRM, uLightDirection, uLightDirection, 0);

	UINT uLightAmount = puTemporary[TEMPORARY_LIGHT_AMOUNT];
	FragmentShaderDefiner.AppendInstruction(ICompiler::DP3, uLightAmount, uLightDirection, puConstants[CONSTANT_LIGHT_LOCAL_DIRECTION]);
	FragmentShaderDefiner.AppendInstruction(ICompiler::SGE, uLightAmount, uLightAmount, puConstants[CONSTANT_LIGHT_COS_THETA]);

	UINT uTexcoord = puTemporary[TEMPORARY_TEXCOORD];
	FragmentShaderDefiner.AppendInstruction(ICompiler::DIV, uTexcoord, s_uADDRESS, 2, pLightPosition->uFragmentShaderHandle, s_uADDRESS, 2, pLightPosition->uFragmentShaderHandle, &s_uADDRESS[3], 1);

	UINT uDepth = puTemporary[TEMPORARY_DEPTH];
	FragmentShaderDefiner.AppendInstruction(ICompiler::TEX, uDepth, s_uADDRESS    , 1, uTexcoord                            , s_uADDRESS    , 2, puSamplers[SAMPLER_DEPTH]            , ALCHEMY_NULL, 0);
	FragmentShaderDefiner.AppendInstruction(ICompiler::ADD, uDepth, s_uADDRESS    , 1, uDepth                               , s_uADDRESS    , 1, puConstants[CONSTANT_EPSILON]        , s_uADDRESS    , 1);
	FragmentShaderDefiner.AppendInstruction(ICompiler::SUB, uDepth, &s_uADDRESS[1], 1, pLightPosition->uFragmentShaderHandle, &s_uADDRESS[2], 1, puConstants[CONSTANT_CAMERA_NEAR]    , s_uADDRESS    , 1);
	FragmentShaderDefiner.AppendInstruction(ICompiler::DIV, uDepth, &s_uADDRESS[1], 1, uDepth                               , &s_uADDRESS[1], 1, puConstants[CONSTANT_CAMERA_DISTANCE], s_uADDRESS    , 1);
	FragmentShaderDefiner.AppendInstruction(ICompiler::SGE, uDepth, s_uADDRESS    , 1, uDepth                               , s_uADDRESS    , 1, uDepth                               , &s_uADDRESS[1], 1);

	const CEffectCode::SHADERVARIABLE* pLightDiffuse = &pConstantVariables[CONSTANT_VARIABLE_LIGHT_DIFFUSE], * pLightSpecular = &pConstantVariables[CONSTANT_VARIABLE_LIGHT_SPECULAR];
	if(pLightDiffuse->uHandle || pLightSpecular->uHandle)
	{
		FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uLightAmount, s_uADDRESS, 1, uLightAmount, s_uADDRESS, 1, uDepth, s_uADDRESS, 1);

		if(pLightDiffuse->uHandle)
			FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, pLightDiffuse->uHandle, pLightDiffuse->bIsUsed ? pLightDiffuse->uHandle : pLightDiffuse->uSource, uLightAmount);

		if(pLightSpecular->uHandle)
			FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, pLightSpecular->uHandle, pLightSpecular->bIsUsed ? pLightSpecular->uHandle : pLightSpecular->uSource, uLightAmount);
	}
	else
		FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uOutputColor, ALCHEMY_NULL, 0, uLightAmount, s_uADDRESS, 1, uDepth, s_uADDRESS, 1);

	return true;
}