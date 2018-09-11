#include "AlchemyDirectionalShadowDisplayEffect.h"

using namespace alchemy;

const FLOAT CDirectionalShadowDisplayEffect::sm_fEpsilon = 0.00005f;

const CEffectCode::TEMPORARYDECLARATION         CDirectionalShadowDisplayEffect::sm_TEMPORARY        [CDirectionalShadowDisplayEffect::TOTAL_TEMPORARY         ] = { {IEffect::VERTEX_SHADER, CEffectCode::VECTOR4_VARIABLE, 1}, {IEffect::FRAGMENT_SHADER, CEffectCode::VECTOR2_VARIABLE, 1},  {IEffect::FRAGMENT_SHADER, CEffectCode::VECTOR4_VARIABLE, 1} };
const CEffectCode::ATTRIBUTEVARIABLEDECLARATION CDirectionalShadowDisplayEffect::sm_ATTRIBUTE_VARIABLE[CDirectionalShadowDisplayEffect::TOTAL_ATTRIBUTE_VARIABLES] = { {IEffect::VERTEX_SHADER, CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION, 0, true, true} };

CDirectionalShadowDisplayEffect::CDirectionalShadowDisplayEffect(UINT uShadowMapIndex, UINT uLightIndex, UINT uCameraIndex, bool isHalfDepth)
:m_bIsHalfDepth(isHalfDepth)
{
	LPSAMPLER pSampler              = &m_Samplers[SAMPLER_DEPTH];
	pSampler->Type                  = IEffect::FRAGMENT_SHADER;
	pSampler->Declaration.Type      = IEffect::TEXTURE2D;
	pSampler->Declaration.MinFilter = IEffect::POINT;
	pSampler->Declaration.MagFilter = IEffect::POINT;
	pSampler->Declaration.MipFilter = IEffect::NONE;
	pSampler->Declaration.AddressU  = IEffect::CLAMP;
	pSampler->Declaration.AddressV  = IEffect::CLAMP;
	pSampler->Declaration.AddressW  = IEffect::CLAMP;
	pSampler->uIndex                = uShadowMapIndex;

	CEffectCode::LPELEMENTDECLARATION pVarying;

	pVarying         = &m_Varying[VARYING_LIGHT_POSITION];
	pVarying->Type   = CVertexDeclaration::FLOAT4;
	pVarying->Usage  = CVertexDeclaration::TEXCOORD;
	pVarying->uIndex = uShadowMapIndex;

	m_uConstantNum = TOTAL_CONSTANTS;
	if(!isHalfDepth)
		++m_uConstantNum;

	ALCHEMY_DEBUG_NEW(m_pConstants,CEffectCode::GLOBALVARIABLE[ m_uConstantNum ]);

	CEffectCode::LPGLOBALVARIABLE pConstant;

	pConstant             = &m_pConstants[CONSTANT_HALF_ONE];

	pConstant->ShaderType = IEffect::VERTEX_SHADER;
	pConstant->GlobalType = CEffectCode::CONSTANT_VARIABLE;
	pConstant->uDetail    = CRenderMethod::CONSTANT_HALF_ONE;
	pConstant->uCount     = 1;
	pConstant->uIndex     = 0;
	pConstant->pValue     = ALCHEMY_NULL;

	pConstant             = &m_pConstants[CONSTANT_LIGHT_WORLD_VIEW_PROJECTION_MATRIX];

	pConstant->ShaderType = IEffect::VERTEX_SHADER;
	pConstant->GlobalType = CEffectCode::CAMERA_VARIABLE;
	pConstant->uDetail    = CRenderMethod::CAMERA_WORLD_VIEW_PROJECTION_MATRIX;
	pConstant->uCount     = 1;
	pConstant->uIndex     = uCameraIndex;
	pConstant->pValue     = ALCHEMY_NULL;


	pConstant             = &m_pConstants[CONSTANT_EPSILON];

	pConstant->ShaderType = IEffect::FRAGMENT_SHADER;
	pConstant->GlobalType = CEffectCode::INPUT_VARIABLE;
	pConstant->uDetail    = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR1_VARIABLE, 1, 1, false);
	pConstant->uCount     = 1;
	pConstant->uIndex     = 0;
	pConstant->pValue     = reinterpret_cast<const VOID*>(&sm_fEpsilon);

	pConstant             = &m_pConstants[CONSTANT_RGB2FLOAT];

	pConstant->ShaderType = IEffect::FRAGMENT_SHADER;
	pConstant->GlobalType = CEffectCode::CONSTANT_VARIABLE;
	pConstant->uDetail    = CRenderMethod::CONSTANT_RGBA_TO_FLOAT_CODE;
	pConstant->uCount     = 1;
	pConstant->uIndex     = 0;
	pConstant->pValue     = ALCHEMY_NULL;


	if(!m_bIsHalfDepth)
	{
		pConstant             = &m_pConstants[TOTAL_CONSTANTS];
		pConstant->ShaderType = IEffect::VERTEX_SHADER;
		pConstant->GlobalType = CEffectCode::CONSTANT_VARIABLE;
		pConstant->uDetail    = CRenderMethod::CONSTANT_ONE;
		pConstant->uCount     = 1;
		pConstant->uIndex     = 0;
		pConstant->pValue     = ALCHEMY_NULL;
	}

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

CDirectionalShadowDisplayEffect::~CDirectionalShadowDisplayEffect()
{
	ALCHEMY_DEBUG_DELETE_ARRAY(m_pConstants);
}

UINT                                             CDirectionalShadowDisplayEffect::GetEffectInfoFlag    (                             )
{
	return ALCHEMY_FLAG(IEffectInfo::FRAGMENT_SHADER_INFO) | ALCHEMY_FLAG(IEffectInfo::FRAGMENT_SHADER_OUTPUT);
}

const IEffectInfo::SAMPLER                                  * CDirectionalShadowDisplayEffect::GetSamplers          (UINT& uSamplerCount          )
{
	uSamplerCount = TOTAL_SAMPLERS;

	return m_Samplers;
}

const CEffectCode::ELEMENTDECLARATION            * CDirectionalShadowDisplayEffect::GetAttributes        (UINT& uAttributeCount        )
{
	uAttributeCount = TOTAL_ATTRIBUTES;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION            * CDirectionalShadowDisplayEffect::GetVarying           (UINT& uVaryingCount          )
{
	uVaryingCount = TOTAL_VARYING;

	return m_Varying;
}

const CEffectCode::GLOBALVARIABLE              * CDirectionalShadowDisplayEffect::GetConstants         (UINT& uConstantCount         )
{
	uConstantCount = m_uConstantNum;

	return m_pConstants;
}

const CEffectCode::TEMPORARYDECLARATION        * CDirectionalShadowDisplayEffect::GetTemporary         (UINT& uTemporaryCount        )
{
	uTemporaryCount = TOTAL_TEMPORARY;

	return sm_TEMPORARY;
}

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CDirectionalShadowDisplayEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
{
	uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLES;

	return sm_ATTRIBUTE_VARIABLE;
}

const CEffectCode::CONSTANTVARIABLEDECLARATION * CDirectionalShadowDisplayEffect::GetConstantVariables (UINT& uConstantVariableCount )
{
	uConstantVariableCount = TOTAL_CONSTANT_VARIABLES;

	return m_ConstantVariable;
}

bool CDirectionalShadowDisplayEffect::ShaderDefineHandler(
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

	static const UINT s_uADDRESS[] = {0, 1, 2, 3};
	UINT x = 0, z = 2, w = 3;

	UINT uLightPosition = puTemporary[TEMPORARY_LIGHT_POSITION];
	VertexShaderDefiner.AppendInstruction(ICompiler::M44, uLightPosition, uPosition, puConstants[CONSTANT_LIGHT_WORLD_VIEW_PROJECTION_MATRIX]);
	VertexShaderDefiner.AppendInstruction(ICompiler::NEG, uLightPosition, &s_uADDRESS[1], 1, uLightPosition, &s_uADDRESS[1], 1, 0                             , ALCHEMY_NULL  , 0);
	VertexShaderDefiner.AppendInstruction(ICompiler::ADD, uLightPosition, s_uADDRESS    , 2, uLightPosition, s_uADDRESS    , 2, uLightPosition                , &s_uADDRESS[3], 1);
	VertexShaderDefiner.AppendInstruction(ICompiler::MUL, uLightPosition, s_uADDRESS    , 2, uLightPosition, s_uADDRESS    , 2, puConstants[CONSTANT_HALF_ONE], s_uADDRESS    , 1);

	if(!m_bIsHalfDepth)
	{
		VertexShaderDefiner.AppendInstruction(ICompiler::ADD, uLightPosition, &w,  1, uLightPosition, &w,  1, uLightPosition, &z, 1);
		VertexShaderDefiner.AppendInstruction(ICompiler::DIV, uLightPosition, &z,  1, uLightPosition, &w,  1, puConstants[TOTAL_CONSTANTS], &x, 1);
	}

	const CEffectCode::VARYING* pLightPosition = &pVarying[VARYING_LIGHT_POSITION];
	VertexShaderDefiner.AppendInstruction(ICompiler::MOV, pLightPosition->uVertexShaderHandle, uLightPosition, 0);

	UINT uDepth = puTemporary[TEMPORARY_DEPTH];
	FragmentShaderDefiner.AppendInstruction(ICompiler::TEX, puTemporary[TEMPORARY_RGBCOLOR], ALCHEMY_NULL, 0, pLightPosition->uFragmentShaderHandle, s_uADDRESS    , 2, puSamplers[SAMPLER_DEPTH]            , ALCHEMY_NULL, 0);
	FragmentShaderDefiner.AppendInstruction(ICompiler::DP4, uDepth,  s_uADDRESS,    1, puTemporary[TEMPORARY_RGBCOLOR], ALCHEMY_NULL, 0, puConstants[CONSTANT_RGB2FLOAT], ALCHEMY_NULL, 0);
	
	FragmentShaderDefiner.AppendInstruction(ICompiler::ADD, uDepth, s_uADDRESS    , 1, uDepth                               , s_uADDRESS    , 1, puConstants[CONSTANT_EPSILON]        , s_uADDRESS    , 1);
	FragmentShaderDefiner.AppendInstruction(ICompiler::SGE, uDepth, s_uADDRESS    , 1, uDepth                               , s_uADDRESS    , 1, pLightPosition->uFragmentShaderHandle, &z           ,1);

	const CEffectCode::SHADERVARIABLE* pLightDiffuse = &pConstantVariables[CONSTANT_VARIABLE_LIGHT_DIFFUSE], * pLightSpecular = &pConstantVariables[CONSTANT_VARIABLE_LIGHT_SPECULAR];

	UINT uSubAddress[] = {0, 0, 0, 0};
	if(pLightDiffuse->uHandle || pLightSpecular->uHandle)
	{
		if(pLightDiffuse->uHandle)
			FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, pLightDiffuse->uHandle, ALCHEMY_NULL, 0, pLightDiffuse->bIsUsed ? pLightDiffuse->uHandle : pLightDiffuse->uSource,
			ALCHEMY_NULL, 0, uDepth, uSubAddress, 4);

		if(pLightSpecular->uHandle)
			FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, pLightSpecular->uHandle, ALCHEMY_NULL, 0, pLightSpecular->bIsUsed ? pLightSpecular->uHandle : pLightSpecular->uSource,
			ALCHEMY_NULL, 0, uDepth, uSubAddress, 4);
	}
	else
		FragmentShaderDefiner.AppendInstruction(ICompiler::MOV, uOutputColor, ALCHEMY_NULL, 0, uDepth, uSubAddress, 4, 0, ALCHEMY_NULL, 0);

	return true;
}