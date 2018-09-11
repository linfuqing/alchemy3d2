#include "AlchemyCascadShadowDisplayEffect.h"

using namespace alchemy;

const FLOAT CCascadShadowDisplayEffect::sm_fEpsilon = 0.005f;

const CEffectCode::TEMPORARYDECLARATION         CCascadShadowDisplayEffect::sm_TEMPORARY        [CCascadShadowDisplayEffect::TOTAL_TEMPORARY         ] = { {IEffect::VERTEX_SHADER, CEffectCode::VECTOR4_VARIABLE, 1}, {IEffect::FRAGMENT_SHADER, CEffectCode::VECTOR4_VARIABLE, 1}, {IEffect::FRAGMENT_SHADER, CEffectCode::VECTOR4_VARIABLE, 1}, {IEffect::FRAGMENT_SHADER, CEffectCode::VECTOR4_VARIABLE, 1}};
const CEffectCode::ATTRIBUTEVARIABLEDECLARATION CCascadShadowDisplayEffect::sm_ATTRIBUTE_VARIABLE[CCascadShadowDisplayEffect::TOTAL_ATTRIBUTE_VARIABLES] = { {IEffect::VERTEX_SHADER, CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION, 0, true, true} };

CCascadShadowDisplayEffect::CCascadShadowDisplayEffect(const UINT* pSamplerIndices, const FLOAT* pDepthSegment, const UINT* pSpilitCameraIndices, UINT uSamplerCount, UINT uLightIndex, bool isHalfDepth)
:m_bIsHalfDepth(isHalfDepth)
{
	ALCHEMY_DEBUG_ASSERT(pSamplerIndices && uSamplerCount, "CCascadShadowDisplay::CCascadShadowDisplay() - error arguments");

	ALCHEMY_DEBUG_NEW(m_pSamplers, SAMPLER[uSamplerCount]);
	m_uSamplerNum = uSamplerCount;

	LPSAMPLER pSampler;
	UINT i;

	for (i=0; i<uSamplerCount; ++i)
	{
		pSampler						= &m_pSamplers[i];
		pSampler->Type                  = IEffect::FRAGMENT_SHADER;
		pSampler->Declaration.Type      = IEffect::TEXTURE2D;
		pSampler->Declaration.MinFilter = IEffect::POINT;
		pSampler->Declaration.MagFilter = IEffect::POINT;
		pSampler->Declaration.MipFilter = IEffect::NONE;
		pSampler->Declaration.AddressU  = IEffect::CLAMP;
		pSampler->Declaration.AddressV  = IEffect::CLAMP;
		pSampler->Declaration.AddressW  = IEffect::CLAMP;
		pSampler->uIndex                = pSamplerIndices[i];
	}

	m_uExternalVaryNum = uSamplerCount;

	ALCHEMY_DEBUG_NEW(m_pVarying, CEffectCode::ELEMENTDECLARATION[TOTAL_VARYING+m_uExternalVaryNum]);

	CEffectCode::LPELEMENTDECLARATION pVarying;

	pVarying         = &m_pVarying[VARYING_LIGHT_POSITION];
	pVarying->Type   = CVertexDeclaration::FLOAT1;
	pVarying->Usage  = CVertexDeclaration::TEXCOORD;
	pVarying->uIndex = pSamplerIndices[0];

	for (i=0; i<m_uExternalVaryNum; ++i)
	{
		pVarying = &m_pVarying[TOTAL_VARYING+i];

		pVarying->Type   = CVertexDeclaration::FLOAT3;
		pVarying->Usage  = CVertexDeclaration::TEXCOORD;
		pVarying->uIndex = i == (m_uExternalVaryNum-1) ?  pSamplerIndices[i]+1 : pSamplerIndices[i+1];
	}

	if( (uSamplerCount & 3) == 0)
		m_uExternConstantNum = uSamplerCount/4;
	else
		m_uExternConstantNum = uSamplerCount/4 + 1;

	m_uExternalConstantMatrixNum = uSamplerCount;

	if(m_bIsHalfDepth)
	{
		ALCHEMY_DEBUG_NEW(m_pContants, CEffectCode::GLOBALVARIABLE[TOTAL_CONSTANTS+m_uExternConstantNum+m_uExternalConstantMatrixNum]);
	}
	else
	{
		ALCHEMY_DEBUG_NEW(m_pContants, CEffectCode::GLOBALVARIABLE[TOTAL_CONSTANTS+m_uExternConstantNum+m_uExternalConstantMatrixNum+1]);
	}

	CEffectCode::LPGLOBALVARIABLE pConstant;

	pConstant             = &m_pContants[CONSTANT_LIGHT_WORLD_VIEW_PROJECTION_MATRIX];

	pConstant->ShaderType = IEffect::VERTEX_SHADER;
	pConstant->GlobalType = CEffectCode::CAMERA_VARIABLE;
	pConstant->uDetail    = CRenderMethod::CAMERA_WORLD_VIEW_PROJECTION_MATRIX;
	pConstant->uCount     = 1;
	pConstant->uIndex     = 0;
	pConstant->pValue     = ALCHEMY_NULL;

	pConstant             = &m_pContants[CONSTANT_HALF_ONE];

	pConstant->ShaderType = IEffect::VERTEX_SHADER;
	pConstant->GlobalType = CEffectCode::CONSTANT_VARIABLE;
	pConstant->uDetail    = CRenderMethod::CONSTANT_HALF_ONE;
	pConstant->uCount     = 1;
	pConstant->uIndex     = 0;
	pConstant->pValue     = ALCHEMY_NULL;

	pConstant             = &m_pContants[CONSTANT_CAMERA_NEAR];

	pConstant->ShaderType = IEffect::FRAGMENT_SHADER;
	pConstant->GlobalType = CEffectCode::CAMERA_VARIABLE;
	pConstant->uDetail    = CRenderMethod::CAMERA_NEAR;
	pConstant->uCount     = 1;
	pConstant->uIndex     = 0;
	pConstant->pValue     = ALCHEMY_NULL;

	pConstant             = &m_pContants[CONSTANT_CAMERA_DISTANCE];

	pConstant->ShaderType = IEffect::FRAGMENT_SHADER;
	pConstant->GlobalType = CEffectCode::CAMERA_VARIABLE;
	pConstant->uDetail    = CRenderMethod::CAMERA_DISTANCE;
	pConstant->uCount     = 1;
	pConstant->uIndex     = 0;
	pConstant->pValue     = ALCHEMY_NULL;

	pConstant             = &m_pContants[CONSTANT_EPSILON];

	pConstant->ShaderType = IEffect::FRAGMENT_SHADER;
	pConstant->GlobalType = CEffectCode::INPUT_VARIABLE;
	pConstant->uDetail    = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR1_VARIABLE, 1, 1, false);
	pConstant->uCount     = 1;
	pConstant->uIndex     = 0;
	pConstant->pValue     = reinterpret_cast<const VOID*>(&sm_fEpsilon);

	pConstant             = &m_pContants[CONSTANT_ZERO];

	pConstant->ShaderType = IEffect::FRAGMENT_SHADER;
	pConstant->GlobalType = CEffectCode::CONSTANT_VARIABLE;
	pConstant->uDetail    = CRenderMethod::CONSTANT_ZERO;
	pConstant->uCount     = 1;
	pConstant->uIndex     = 0;
	pConstant->pValue     = ALCHEMY_NULL;

	pConstant             = &m_pContants[CONSTANT_RGB2FLOAT];

	pConstant->ShaderType = IEffect::FRAGMENT_SHADER;
	pConstant->GlobalType = CEffectCode::CONSTANT_VARIABLE;
	pConstant->uDetail    = CRenderMethod::CONSTANT_RGBA_TO_FLOAT_CODE;
	pConstant->uCount     = 1;
	pConstant->uIndex     = 0;
	pConstant->pValue     = ALCHEMY_NULL;

	ALCHEMY_DEBUG_NEW(m_pExternaConstantInices, UINT[uSamplerCount]);

	CEffectCode::VARIABLETYPE type;
	if(m_uExternConstantNum == 1)
	{
		pConstant = &m_pContants[TOTAL_CONSTANTS];

		switch (uSamplerCount)
		{
		case 1:
			type = CEffectCode::VECTOR1_VARIABLE;
			break;
		case 2:
			type = CEffectCode::VECTOR2_VARIABLE;
			break;
		case 3:
			type = CEffectCode::VECTOR3_VARIABLE;
			break;
		case 4:
			type = CEffectCode::VECTOR4_VARIABLE;
			break;
		}

		pConstant->ShaderType = IEffect::FRAGMENT_SHADER;
		pConstant->GlobalType = CEffectCode::INPUT_VARIABLE;
		pConstant->uDetail    = CEffectCode::BuildGlobalInputCode(type, 1, uSamplerCount, false);
		pConstant->uCount     = 1;
		pConstant->uIndex     = 0;
		pConstant->pValue     = reinterpret_cast<const VOID*>(pDepthSegment);

		for(i=0; i<uSamplerCount; ++i)
			m_pExternaConstantInices[i] = i;
	}
	else
	{
		for(i=0; i<uSamplerCount; ++i)
			m_pExternaConstantInices[i] = i & 3;

		--m_uExternConstantNum;
		UINT uLastNum;
		for (i=0; i<=m_uExternConstantNum; ++i)
		{
			pConstant = &m_pContants[TOTAL_CONSTANTS+i];

			if(i == m_uExternConstantNum)
			{
				uLastNum = m_uExternConstantNum & 3;

				switch (uLastNum)
				{
				case 1:
					type = CEffectCode::VECTOR1_VARIABLE;
					break;
				case 2:
					type = CEffectCode::VECTOR2_VARIABLE;
					break;
				case 3:
					type = CEffectCode::VECTOR3_VARIABLE;
					break;
				case 0:
					type = CEffectCode::VECTOR4_VARIABLE;
					break;
				}

				pConstant->ShaderType = IEffect::FRAGMENT_SHADER;
				pConstant->GlobalType = CEffectCode::INPUT_VARIABLE;
				pConstant->uDetail    = CEffectCode::BuildGlobalInputCode(type, 1, uLastNum, false);
				pConstant->uCount     = 1;
				pConstant->uIndex     = 0;
				pConstant->pValue     = reinterpret_cast<const VOID*>( pDepthSegment + i*sizeof(FLOAT4) );
			}
			else
			{
				pConstant->ShaderType = IEffect::FRAGMENT_SHADER;
				pConstant->GlobalType = CEffectCode::INPUT_VARIABLE;
				pConstant->uDetail    = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR4_VARIABLE, 1, 4, false);
				pConstant->uCount     = 1;
				pConstant->uIndex     = 0;
				pConstant->pValue     = reinterpret_cast<const VOID*>( pDepthSegment + i*sizeof(FLOAT4) );
			}
		}

		++m_uExternConstantNum;
	}

	for (i=0; i<uSamplerCount; ++i)
	{
		pConstant = &m_pContants[ TOTAL_CONSTANTS + m_uExternConstantNum + i ];
		pConstant->ShaderType = IEffect::VERTEX_SHADER;
		pConstant->GlobalType = CEffectCode::CAMERA_VARIABLE;
		pConstant->uDetail    = CRenderMethod::CAMERA_WORLD_VIEW_PROJECTION_MATRIX;
		pConstant->uCount     = 1;
		pConstant->uIndex     = pSpilitCameraIndices[i];
		pConstant->pValue     = ALCHEMY_NULL;
	}

	if(!m_bIsHalfDepth)
	{
		pConstant = &m_pContants[ TOTAL_CONSTANTS + m_uExternConstantNum + m_uExternalConstantMatrixNum ];
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

CCascadShadowDisplayEffect::~CCascadShadowDisplayEffect()
{
	ALCHEMY_DEBUG_DELETE_ARRAY(m_pSamplers);
	ALCHEMY_DEBUG_DELETE_ARRAY(m_pContants);
	ALCHEMY_DEBUG_DELETE_ARRAY(m_pVarying);
}

UINT                                             CCascadShadowDisplayEffect::GetEffectInfoFlag    (                             )
{
	return ALCHEMY_FLAG(IEffectInfo::FRAGMENT_SHADER_INFO) | ALCHEMY_FLAG(IEffectInfo::FRAGMENT_SHADER_OUTPUT);
}

const IEffectInfo::SAMPLER                                  * CCascadShadowDisplayEffect::GetSamplers          (UINT& uSamplerCount          )
{
	uSamplerCount = m_uSamplerNum;
	return m_pSamplers;
}

const CEffectCode::ELEMENTDECLARATION            * CCascadShadowDisplayEffect::GetAttributes        (UINT& uAttributeCount        )
{
	uAttributeCount = 0;
	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION            * CCascadShadowDisplayEffect::GetVarying           (UINT& uVaryingCount          )
{
	uVaryingCount = TOTAL_VARYING+m_uExternalVaryNum;
	return m_pVarying;
}

const CEffectCode::GLOBALVARIABLE              * CCascadShadowDisplayEffect::GetConstants         (UINT& uConstantCount         )
{
	uConstantCount = TOTAL_CONSTANTS+m_uExternConstantNum+m_uExternalConstantMatrixNum;
	if(!m_bIsHalfDepth)
		++uConstantCount;

	return m_pContants;
}

const CEffectCode::TEMPORARYDECLARATION        * CCascadShadowDisplayEffect::GetTemporary         (UINT& uTemporaryCount        )
{
	uTemporaryCount = TOTAL_TEMPORARY;
	return sm_TEMPORARY;
}

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CCascadShadowDisplayEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
{
	uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLES;
	return sm_ATTRIBUTE_VARIABLE;
}

const CEffectCode::CONSTANTVARIABLEDECLARATION * CCascadShadowDisplayEffect::GetConstantVariables (UINT& uConstantVariableCount )
{
	uConstantVariableCount = TOTAL_CONSTANT_VARIABLES;
	return m_ConstantVariable;
}

bool CCascadShadowDisplayEffect::ShaderDefineHandler(
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

	const CEffectCode::VARYING* pLightPosition = &pVarying[VARYING_LIGHT_POSITION];

	UINT zAddress[] = {2, 2, 2, 2};
	VertexShaderDefiner.AppendInstruction(ICompiler::MOV, pLightPosition->uVertexShaderHandle, ALCHEMY_NULL, 0,  uLightPosition, zAddress, 4,  0, ALCHEMY_NULL, 0);

	for (UINT i=0; i<m_uSamplerNum; ++i)
	{
		VertexShaderDefiner.AppendInstruction(ICompiler::M44, uLightPosition, uPosition, puConstants[TOTAL_CONSTANTS+m_uExternConstantNum+i]);
		VertexShaderDefiner.AppendInstruction(ICompiler::NEG, uLightPosition, &s_uADDRESS[1], 1, uLightPosition, &s_uADDRESS[1], 1, 0                             , ALCHEMY_NULL  , 0);
		VertexShaderDefiner.AppendInstruction(ICompiler::ADD, uLightPosition, s_uADDRESS    , 2, uLightPosition, s_uADDRESS    , 2, uLightPosition                , &s_uADDRESS[3], 1);
		VertexShaderDefiner.AppendInstruction(ICompiler::MUL, uLightPosition, s_uADDRESS    , 2, uLightPosition, s_uADDRESS    , 2, puConstants[CONSTANT_HALF_ONE], s_uADDRESS    , 1);
		if(!m_bIsHalfDepth)
		{
			VertexShaderDefiner.AppendInstruction(ICompiler::ADD, uLightPosition, &w,  1, uLightPosition, &w,  1, uLightPosition, &z, 1);
			VertexShaderDefiner.AppendInstruction(ICompiler::DIV, uLightPosition, &z,  1, uLightPosition, &w,  1, puConstants[TOTAL_CONSTANTS+m_uExternalConstantMatrixNum+m_uExternConstantNum], &x, 1);
		}

		VertexShaderDefiner.AppendInstruction(ICompiler::MOV, pVarying[TOTAL_VARYING + i ].uVertexShaderHandle, ALCHEMY_NULL, 0,   uLightPosition, s_uADDRESS, 3,  0, ALCHEMY_NULL, 0);
	}

	UINT uLightPositionDepth = puTemporary[TEMPORARY_TEXCOORD_INFO];
	FragmentShaderDefiner.AppendInstruction(ICompiler::SUB, uLightPositionDepth, &z, 1, pLightPosition->uFragmentShaderHandle, s_uADDRESS, 1, puConstants[CONSTANT_CAMERA_NEAR], &x, 1);
	FragmentShaderDefiner.AppendInstruction(ICompiler::DIV, uLightPositionDepth, &z, 1, uLightPositionDepth, &z, 1, puConstants[CONSTANT_CAMERA_DISTANCE], &x, 1);

	UINT uDepth = puTemporary[TEMPORARY_DEPTH];
	for (UINT i=0; i<m_uSamplerNum; ++i)
	{
		if(i == 0)
		{
			FragmentShaderDefiner.AppendInstruction(ICompiler::MOV, uDepth, &s_uADDRESS[1], 1, pVarying[TOTAL_VARYING].uFragmentShaderHandle, &s_uADDRESS[2], 1,  0, ALCHEMY_NULL, 0);
			FragmentShaderDefiner.AppendInstruction(ICompiler::TEX, puTemporary[TEMPORARY_RGBVALUE ], ALCHEMY_NULL, 0, pVarying[TOTAL_VARYING].uFragmentShaderHandle, s_uADDRESS , 2, puSamplers[0], ALCHEMY_NULL, 0);
			FragmentShaderDefiner.AppendInstruction(ICompiler::DP4, uDepth,  s_uADDRESS,    1, puTemporary[TEMPORARY_RGBVALUE ], ALCHEMY_NULL, 0, puConstants[CONSTANT_RGB2FLOAT], ALCHEMY_NULL, 0);

			FragmentShaderDefiner.AppendInstruction(ICompiler::SGE, uDepth, &s_uADDRESS[2], 1, uLightPositionDepth, &z, 1, puConstants[CONSTANT_ZERO], s_uADDRESS, 1);
			FragmentShaderDefiner.AppendInstruction(ICompiler::SLT, uDepth, &s_uADDRESS[3], 1, uLightPositionDepth, &z, 1, puConstants[TOTAL_CONSTANTS], &s_uADDRESS[ m_pExternaConstantInices[0] ], 1);
			FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uDepth, &s_uADDRESS[2], 1, uDepth, &s_uADDRESS[2], 1, uDepth, &s_uADDRESS[3], 1);

			FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uDepth, s_uADDRESS, 2,  uDepth, s_uADDRESS, 2, uDepth, &s_uADDRESS[2], 1);
		}
		else
		{

			FragmentShaderDefiner.AppendInstruction(ICompiler::MOV, puTemporary[TEMPORARY_TEXCOORD_INFO], &s_uADDRESS[1], 1, pVarying[TOTAL_VARYING+i].uFragmentShaderHandle, &s_uADDRESS[2], 1,  0, ALCHEMY_NULL, 0);
			FragmentShaderDefiner.AppendInstruction(ICompiler::TEX, puTemporary[TEMPORARY_RGBVALUE], ALCHEMY_NULL, 0, pVarying[TOTAL_VARYING+i].uFragmentShaderHandle, s_uADDRESS , 2, puSamplers[i], ALCHEMY_NULL, 0);
			FragmentShaderDefiner.AppendInstruction(ICompiler::DP4, puTemporary[TEMPORARY_TEXCOORD_INFO],  s_uADDRESS,    1, puTemporary[TEMPORARY_RGBVALUE ], ALCHEMY_NULL, 0, puConstants[CONSTANT_RGB2FLOAT], ALCHEMY_NULL, 0);

			FragmentShaderDefiner.AppendInstruction(ICompiler::SGE, uDepth, &s_uADDRESS[2], 1, uLightPositionDepth, &z, 1, puConstants[ TOTAL_CONSTANTS + ( ( (i-1) >> 2) & 3) ], &s_uADDRESS[ m_pExternaConstantInices[i-1] ], 1);
			FragmentShaderDefiner.AppendInstruction(ICompiler::SLT, uDepth, &s_uADDRESS[3], 1, uLightPositionDepth, &z, 1, puConstants[ TOTAL_CONSTANTS + ( (i >> 2) & 3) ], &s_uADDRESS[ m_pExternaConstantInices[i] ], 1);
			FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uDepth, &s_uADDRESS[2], 1, uDepth, &s_uADDRESS[2], 1, uDepth, &s_uADDRESS[3], 1);

			FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, puTemporary[TEMPORARY_TEXCOORD_INFO], s_uADDRESS, 2, puTemporary[TEMPORARY_TEXCOORD_INFO], s_uADDRESS, 2, uDepth, &s_uADDRESS[2], 1);

			FragmentShaderDefiner.AppendInstruction(ICompiler::ADD, uDepth, s_uADDRESS    , 2, uDepth, s_uADDRESS    , 2, puTemporary[TEMPORARY_TEXCOORD_INFO], s_uADDRESS, 2);
		}
	}

	FragmentShaderDefiner.AppendInstruction(ICompiler::ADD, uDepth, s_uADDRESS    , 1, uDepth                               , s_uADDRESS    , 1, puConstants[CONSTANT_EPSILON]        , s_uADDRESS    , 1);
	FragmentShaderDefiner.AppendInstruction(ICompiler::SGE, uDepth, s_uADDRESS    , 1, uDepth                               , s_uADDRESS    , 1, uDepth                               , &s_uADDRESS[1], 1);

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