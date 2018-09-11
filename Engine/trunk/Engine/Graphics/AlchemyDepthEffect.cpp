#include "AlchemyDepthEffect.h"
#include "AlchemyRenderMethod.h"

using namespace alchemy;

const CEffectCode::ELEMENTDECLARATION             CDepthEffect::sm_VARYING           [CDepthEffect::TOTAL_VARYING            ] = 
{ 
	{CVertexDeclaration::FLOAT1, CVertexDeclaration::DEPTH, 0} 
};

//const CEffectCode::GLOBALVARIABLE               CDepthEffect::sm_CONSTANS          [CDepthEffect::TOTAL_CONSTANTS          ] = 
//{ 
//	{IEffect::VERTEX_SHADER, CEffectCode::CAMERA_VARIABLE, CRenderMethod::CAMERA_WORLD_VIEW_PROJECTION_MATRIX, 1, 0, ALCHEMY_NULL}, 
//	{IEffect::FRAGMENT_SHADER, CEffectCode::CAMERA_VARIABLE, CRenderMethod::CAMERA_NEAR, 1, 0, ALCHEMY_NULL}, 
//	{IEffect::FRAGMENT_SHADER, CEffectCode::CAMERA_VARIABLE, CRenderMethod::CAMERA_DISTANCE, 1, 0, ALCHEMY_NULL} 
//};

//const CEffectCode::TEMPORARYDECLARATION         CDepthEffect::sm_TEMPORARY         [CDepthEffect::TOTAL_TEMPORARY          ] = { {IEffect::FRAGMENT_SHADER, CEffectCode::VECTOR1_VARIABLE, 1} };
const CEffectCode::ATTRIBUTEVARIABLEDECLARATION CDepthEffect::sm_ATTRIBUTE_VARIABLES[CDepthEffect::TOTAL_ATTRIBUTE_VARIABLES] = { {IEffect::VERTEX_SHADER, CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION , 0, true, true} };

CDepthEffect::CDepthEffect(PROJECTIONTYPE Type, bool isHalfDepth)
:m_eProjectionType(Type),
m_bIsHalfDepth(isHalfDepth)
{
	CEffectCode::LPGLOBALVARIABLE pConstant;
	if(m_eProjectionType == PERSPECTIVE_TYPE)
	{
		m_uConstantNum = TOTAL_CONSTANTS + 2;
		ALCHEMY_DEBUG_NEW(m_pConstants, CEffectCode::GLOBALVARIABLE[m_uConstantNum]);

		pConstant = &m_pConstants[CONSTANT_WORLD_VIEW_PROJECTION_MATRIX];

		pConstant->ShaderType = IEffect::VERTEX_SHADER;
		pConstant->GlobalType = CEffectCode::CAMERA_VARIABLE;
		pConstant->uDetail	  = CRenderMethod::CAMERA_WORLD_VIEW_PROJECTION_MATRIX;
		pConstant->uCount	  = 1;
		pConstant->uIndex	  = 0;
		pConstant->pValue	  = ALCHEMY_NULL;

		++pConstant;

		pConstant->ShaderType = IEffect::FRAGMENT_SHADER;
		pConstant->GlobalType = CEffectCode::CONSTANT_VARIABLE;
		pConstant->uDetail	  = CRenderMethod::CONSTANT_FLOAT_TO_RGBA_CODE;
		pConstant->uCount	  = 1;
		pConstant->uIndex	  = 0;
		pConstant->pValue	  = ALCHEMY_NULL;

		++pConstant;

		pConstant->ShaderType = IEffect::FRAGMENT_SHADER;
		pConstant->GlobalType = CEffectCode::CAMERA_VARIABLE;
		pConstant->uDetail	  = CRenderMethod::CAMERA_NEAR;
		pConstant->uCount	  = 1;
		pConstant->uIndex	  = 0;
		pConstant->pValue	  = ALCHEMY_NULL;

		++pConstant;

		pConstant->ShaderType = IEffect::FRAGMENT_SHADER;
		pConstant->GlobalType = CEffectCode::CAMERA_VARIABLE;
		pConstant->uDetail	  = CRenderMethod::CAMERA_DISTANCE;
		pConstant->uCount	  = 1;
		pConstant->uIndex	  = 0;
		pConstant->pValue	  = ALCHEMY_NULL;

		m_uTemporaryNum = 2;
		ALCHEMY_DEBUG_NEW(m_pTemporaries, CEffectCode::TEMPORARYDECLARATION[m_uTemporaryNum]);

		m_pTemporaries->ShaderType   = IEffect::FRAGMENT_SHADER;
		m_pTemporaries->VariableType = CEffectCode::VECTOR4_VARIABLE;
		m_pTemporaries->uCount		 = 1;

		m_pTemporaries[1].ShaderType = IEffect::FRAGMENT_SHADER;
		m_pTemporaries[1].VariableType = CEffectCode::VECTOR4_VARIABLE;
		m_pTemporaries[1].uCount = 1;
	}
	else
	{
		if(m_bIsHalfDepth)
		{
			m_uConstantNum = TOTAL_CONSTANTS;

			ALCHEMY_DEBUG_NEW(m_pConstants, CEffectCode::GLOBALVARIABLE[m_uConstantNum]);

			pConstant = m_pConstants;

			pConstant->ShaderType = IEffect::VERTEX_SHADER;
			pConstant->GlobalType = CEffectCode::CAMERA_VARIABLE;
			pConstant->uDetail	  = CRenderMethod::CAMERA_WORLD_VIEW_PROJECTION_MATRIX;
			pConstant->uCount	  = 1;
			pConstant->uIndex	  = 0;
			pConstant->pValue	  = ALCHEMY_NULL;

			++pConstant;

			pConstant->ShaderType = IEffect::FRAGMENT_SHADER;
			pConstant->GlobalType = CEffectCode::CONSTANT_VARIABLE;
			pConstant->uDetail	  = CRenderMethod::CONSTANT_FLOAT_TO_RGBA_CODE;
			pConstant->uCount	  = 1;
			pConstant->uIndex	  = 0;
			pConstant->pValue	  = ALCHEMY_NULL;

			m_uTemporaryNum = 2;
			
			ALCHEMY_DEBUG_NEW(m_pTemporaries, CEffectCode::TEMPORARYDECLARATION[m_uTemporaryNum]);

			m_pTemporaries->ShaderType   = IEffect::FRAGMENT_SHADER;
			m_pTemporaries->VariableType = CEffectCode::VECTOR3_VARIABLE;
			m_pTemporaries->uCount		 = 1;

			m_pTemporaries[1].ShaderType = IEffect::FRAGMENT_SHADER;
			m_pTemporaries[1].VariableType = CEffectCode::VECTOR4_VARIABLE;
			m_pTemporaries[1].uCount = 1;
		}
		else
		{
			m_uConstantNum = TOTAL_CONSTANTS + 1;
			ALCHEMY_DEBUG_NEW(m_pConstants, CEffectCode::GLOBALVARIABLE[m_uConstantNum]);

			pConstant = &m_pConstants[CONSTANT_WORLD_VIEW_PROJECTION_MATRIX];

			pConstant->ShaderType = IEffect::VERTEX_SHADER;
			pConstant->GlobalType = CEffectCode::CAMERA_VARIABLE;
			pConstant->uDetail	  = CRenderMethod::CAMERA_WORLD_VIEW_PROJECTION_MATRIX;
			pConstant->uCount	  = 1;
			pConstant->uIndex	  = 0;
			pConstant->pValue	  = ALCHEMY_NULL;

			++pConstant;

			pConstant->ShaderType = IEffect::FRAGMENT_SHADER;
			pConstant->GlobalType = CEffectCode::CONSTANT_VARIABLE;
			pConstant->uDetail	  = CRenderMethod::CONSTANT_FLOAT_TO_RGBA_CODE;
			pConstant->uCount	  = 1;
			pConstant->uIndex	  = 0;
			pConstant->pValue	  = ALCHEMY_NULL;

			++pConstant;

			pConstant->ShaderType = IEffect::VERTEX_SHADER;
			pConstant->GlobalType = CEffectCode::CONSTANT_VARIABLE;
			pConstant->uDetail	  = CRenderMethod::CONSTANT_TWO;
			pConstant->uCount	  = 1;
			pConstant->uIndex	  = 0;
			pConstant->pValue	  = ALCHEMY_NULL;

			m_uTemporaryNum = 3;
			ALCHEMY_DEBUG_NEW(m_pTemporaries, CEffectCode::TEMPORARYDECLARATION[m_uTemporaryNum]);

			m_pTemporaries->ShaderType   = IEffect::VERTEX_SHADER;
			m_pTemporaries->VariableType = CEffectCode::VECTOR4_VARIABLE;
			m_pTemporaries->uCount		 = 1; 

			m_pTemporaries[1].ShaderType = IEffect::FRAGMENT_SHADER;
			m_pTemporaries[1].VariableType = CEffectCode::VECTOR4_VARIABLE;
			m_pTemporaries[1].uCount = 1;

			m_pTemporaries[2].ShaderType = IEffect::FRAGMENT_SHADER;
			m_pTemporaries[2].VariableType = CEffectCode::VECTOR3_VARIABLE;
			m_pTemporaries[2].uCount = 1;
		}
	}
}

CDepthEffect::~CDepthEffect(void)
{
	if(m_uConstantNum == 1)
	{
		ALCHEMY_DEBUG_DELETE(m_pConstants);
	}
	else
	{
		ALCHEMY_DEBUG_DELETE_ARRAY(m_pConstants);
	}

	if(m_uTemporaryNum == 1)
	{
		ALCHEMY_DEBUG_DELETE(m_pTemporaries);
	}
	else
	{
		ALCHEMY_DEBUG_DELETE_ARRAY(m_pTemporaries);
	}
}

UINT CDepthEffect::GetEffectInfoFlag()
{
	return ALCHEMY_FLAG(IEffectInfo::FRAGMENT_SHADER_OUTPUT);
}

const IEffectInfo::SAMPLER* CDepthEffect::GetSamplers(UINT& uSamplerCount)
{
	uSamplerCount = TOTAL_SAMPLERS;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION* CDepthEffect::GetAttributes(UINT& uAttributeCount)
{
	uAttributeCount = TOTAL_ATTRIBUTES;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION* CDepthEffect::GetVarying (UINT& uVaryingCount)
{
	uVaryingCount = TOTAL_VARYING;

	return sm_VARYING;
}

const CEffectCode::GLOBALVARIABLE* CDepthEffect::GetConstants(UINT& uConstantCount)
{
	uConstantCount = m_uConstantNum;

	return m_pConstants;
}

const CEffectCode::TEMPORARYDECLARATION* CDepthEffect::GetTemporary(UINT& uTemporaryCount)
{
	uTemporaryCount = m_uTemporaryNum;

	return m_pTemporaries;
}

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CDepthEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
{
	uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLES;

	return sm_ATTRIBUTE_VARIABLES;
}

const CEffectCode::CONSTANTVARIABLEDECLARATION * CDepthEffect::GetConstantVariables(UINT& uConstantVariableCount)
{
	uConstantVariableCount = TOTAL_CONSTANT_VARIABLES;

	return ALCHEMY_NULL;
}

bool CDepthEffect::ShaderDefineHandler(	
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
	const CEffectCode::VARYING* pDepth = &pVarying[VARYING_DEPTH];

	UINT x = 0, y = 1, z = 2, w = 3;
	UINT uXSubAddress[] = {0, 0, 0, 0};
	UINT uZSubAddress[] = {2, 2, 2, 2};
	UINT uAddress[] = {0, 1, 2, 3};

	UINT uDepth, uRGBA, uBias, uFloatToRGBACode;

	if(m_eProjectionType == PERSPECTIVE_TYPE)
	{
		VertexShaderDefiner.AppendInstruction(ICompiler::M44, pDepth->uVertexShaderHandle, ALCHEMY_NULL, 0, pAttributeVariables[ATTRIBUTE_VARIABLE_POSITION].uHandle, ALCHEMY_NULL, 0, puConstants[CONSTANT_WORLD_VIEW_PROJECTION_MATRIX], ALCHEMY_NULL, 0);

		uDepth = puTemporary[0];
		FragmentShaderDefiner.AppendInstruction(ICompiler::SUB, uDepth, &x, 1, pDepth->uFragmentShaderHandle, &z, 1, puConstants[TOTAL_CONSTANTS], &x, 1);
		FragmentShaderDefiner.AppendInstruction(ICompiler::DIV, uDepth, &x, 1, uDepth, &x, 1, puConstants[TOTAL_CONSTANTS+1], &x ,1);

		uRGBA = puTemporary[1], uFloatToRGBACode = puConstants[CONSTANT_FLOAT2RGB];
		FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uRGBA, ALCHEMY_NULL,0,  uFloatToRGBACode, ALCHEMY_NULL, 0, uDepth, uXSubAddress, 4);
		FragmentShaderDefiner.AppendInstruction(ICompiler::FRC, uRGBA, uRGBA           , 0                            );

		uBias = puTemporary[0];
		FragmentShaderDefiner.AppendInstruction(ICompiler::DIV, uBias, &uAddress[1], 3, uRGBA, &uAddress[1], 3, uFloatToRGBACode, &uAddress[1], 1);
		FragmentShaderDefiner.AppendInstruction(ICompiler::SUB, uRGBA,  uAddress,    3, uRGBA, uAddress    , 3, uBias           , &uAddress[1]    , 3);
		FragmentShaderDefiner.AppendInstruction(ICompiler::MOV, uOutputColor, uRGBA, 0);
	}
	else
	{
		if(m_bIsHalfDepth)
		{
			VertexShaderDefiner.AppendInstruction(ICompiler::M44, pDepth->uVertexShaderHandle, ALCHEMY_NULL, 0, pAttributeVariables[ATTRIBUTE_VARIABLE_POSITION].uHandle, ALCHEMY_NULL, 0, puConstants[CONSTANT_WORLD_VIEW_PROJECTION_MATRIX], ALCHEMY_NULL, 0);

			uRGBA = puTemporary[1], uFloatToRGBACode = puConstants[CONSTANT_FLOAT2RGB];
			FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uRGBA, ALCHEMY_NULL,0,  uFloatToRGBACode, ALCHEMY_NULL, 0, pDepth->uFragmentShaderHandle, uZSubAddress, 4);
			FragmentShaderDefiner.AppendInstruction(ICompiler::FRC, uRGBA, uRGBA           , 0                            );

			uBias = puTemporary[0];
			FragmentShaderDefiner.AppendInstruction(ICompiler::DIV, uBias, uAddress, 3, uRGBA, &uAddress[1], 3, uFloatToRGBACode, &uAddress[1], 1);
			FragmentShaderDefiner.AppendInstruction(ICompiler::SUB, uRGBA, uAddress,    3, uRGBA, uAddress    , 3, uBias           , uAddress   , 3);
			FragmentShaderDefiner.AppendInstruction(ICompiler::MOV, uOutputColor, uRGBA, 0);
		}
		else
		{
			VertexShaderDefiner.AppendInstruction(ICompiler::M44, puTemporary[0], ALCHEMY_NULL, 0, pAttributeVariables[ATTRIBUTE_VARIABLE_POSITION].uHandle, ALCHEMY_NULL, 0, puConstants[CONSTANT_WORLD_VIEW_PROJECTION_MATRIX], ALCHEMY_NULL, 0);
			
			VertexShaderDefiner.AppendInstruction(ICompiler::ADD, puTemporary[0], &x,  1, puTemporary[0], &w, 1, puTemporary[0], &z, 1);
			VertexShaderDefiner.AppendInstruction(ICompiler::DIV, puTemporary[0], &x,  1, puTemporary[0], &x,  1, puConstants[1], &x, 1);
			VertexShaderDefiner.AppendInstruction(ICompiler::SAT, pDepth->uVertexShaderHandle, ALCHEMY_NULL, 0, puTemporary[0], uXSubAddress, 4,  0, ALCHEMY_NULL, 0);

			uRGBA = puTemporary[2], uFloatToRGBACode = puConstants[CONSTANT_FLOAT2RGB];
			FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uRGBA, ALCHEMY_NULL,0,  uFloatToRGBACode, ALCHEMY_NULL, 0, pDepth->uFragmentShaderHandle, uXSubAddress, 4);
			FragmentShaderDefiner.AppendInstruction(ICompiler::FRC, uRGBA, uRGBA           , 0                            );

			uBias = puTemporary[3];
			FragmentShaderDefiner.AppendInstruction(ICompiler::DIV, uBias, uAddress, 3, uRGBA, &uAddress[1], 3, uFloatToRGBACode, &uAddress[1], 1);
			FragmentShaderDefiner.AppendInstruction(ICompiler::SUB, uRGBA, uAddress,    3, uRGBA, uAddress    , 3, uBias           , uAddress   , 3);
			FragmentShaderDefiner.AppendInstruction(ICompiler::MOV, uOutputColor, uRGBA, 0);
		}
	}

	return true;
}