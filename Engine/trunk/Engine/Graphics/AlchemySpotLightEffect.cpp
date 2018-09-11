#include "AlchemySpotLightEffect.h"
#include "AlchemyRenderMethod.h"

using namespace alchemy;

const CEffectCode::TEMPORARYDECLARATION         CSpotLightEffect::sm_TEMPORARY          [CSpotLightEffect::TOTAL_TEMPORARY        ] = 
{
	{IEffect::FRAGMENT_SHADER, CEffectCode::VECTOR1_VARIABLE, 1}, 
	{IEffect::FRAGMENT_SHADER, CEffectCode::VECTOR3_VARIABLE, 1}
};

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION CSpotLightEffect::sm_ATTRIBUTE_VARIABLES[CSpotLightEffect::TOTAL_ATTRIBUTE_VARIABLES] = 
{
	{IEffect::FRAGMENT_SHADER, CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION, 0, true , true}
};

CSpotLightEffect::CSpotLightEffect(UINT uLightIndex)
{
	CEffectCode::LPGLOBALVARIABLE pConstant;
	
	pConstant              = &m_Constants[CONSTANT_ZERO];
	pConstant->ShaderType  = IEffect::FRAGMENT_SHADER;
	pConstant->GlobalType  = CEffectCode::CONSTANT_VARIABLE;
	pConstant->uDetail     = CRenderMethod::CONSTANT_ZERO;
	pConstant->uCount      = 1;
	pConstant->uIndex      = 0;
	pConstant->pValue      = ALCHEMY_NULL;
	
	pConstant              = &m_Constants[CONSTANT_ONE];
	pConstant->ShaderType  = IEffect::FRAGMENT_SHADER;
	pConstant->GlobalType  = CEffectCode::CONSTANT_VARIABLE;
	pConstant->uDetail     = CRenderMethod::CONSTANT_ONE;
	pConstant->uCount      = 1;
	pConstant->uIndex      = 0;
	pConstant->pValue      = ALCHEMY_NULL;

	pConstant              = &m_Constants[CONSTANT_LIGHT_FALLOFF];
	pConstant->ShaderType  = IEffect::FRAGMENT_SHADER;
	pConstant->GlobalType  = CEffectCode::LIGHT_VARIABLE;
	pConstant->uDetail     = CRenderMethod::LIGHT_FALLOFF;
	pConstant->uCount      = 1;
	pConstant->uIndex      = uLightIndex;
	pConstant->pValue      = ALCHEMY_NULL;

	pConstant              = &m_Constants[CONSTANT_LIGHT_COS_THETA];
	pConstant->ShaderType  = IEffect::FRAGMENT_SHADER;
	pConstant->GlobalType  = CEffectCode::LIGHT_VARIABLE;
	pConstant->uDetail     = CRenderMethod::LIGHT_COS_THETA;
	pConstant->uCount      = 1;
	pConstant->uIndex      = uLightIndex;
	pConstant->pValue      = ALCHEMY_NULL;

	pConstant              = &m_Constants[CONSTANT_LIGHT_INVERSE_COS_PHI_SUBSTRUCT_COS_THETA];
	pConstant->ShaderType  = IEffect::FRAGMENT_SHADER;
	pConstant->GlobalType  = CEffectCode::LIGHT_VARIABLE;
	pConstant->uDetail     = CRenderMethod::LIGHT_INVERSE_COS_PHI_SUBSTRUCT_COS_THETA;
	pConstant->uCount      = 1;
	pConstant->uIndex      = uLightIndex;
	pConstant->pValue      = ALCHEMY_NULL;

	pConstant              = &m_Constants[CONSTANT_LIGHT_LOCAL_POSITION];
	pConstant->ShaderType  = IEffect::FRAGMENT_SHADER;
	pConstant->GlobalType  = CEffectCode::LIGHT_VARIABLE;
	pConstant->uDetail     = CRenderMethod::LIGHT_LOCAL_POSITION;
	pConstant->uCount      = 1;
	pConstant->uIndex      = uLightIndex;
	pConstant->pValue      = ALCHEMY_NULL;

	pConstant              = &m_Constants[CONSTANT_LIGHT_LOCAL_DIRECTION];
	pConstant->ShaderType  = IEffect::FRAGMENT_SHADER;
	pConstant->GlobalType  = CEffectCode::LIGHT_VARIABLE;
	pConstant->uDetail     = CRenderMethod::LIGHT_LOCAL_DIRECTION;
	pConstant->uCount      = 1;
	pConstant->uIndex      = uLightIndex;
	pConstant->pValue      = ALCHEMY_NULL;

	CEffectCode::LPCONSTANTVARIABLEDECLARATION pConstantVariable;

	pConstantVariable              = &m_ConstantVariables[CONSTANT_VARIABLE_LIGHT_DIFFUSE];
	pConstantVariable->ShaderType  = IEffect::FRAGMENT_SHADER;
	pConstantVariable->GlobalType  = CEffectCode::LIGHT_VARIABLE;
	pConstantVariable->uDetail     = CRenderMethod::LIGHT_DIFFUSE;
	pConstantVariable->uCount      = 1;
	pConstantVariable->uIndex      = uLightIndex;
	pConstantVariable->pValue      = ALCHEMY_NULL;
	pConstantVariable->bIsReadOnly = true;
	pConstantVariable->bIsForce    = true;

	pConstantVariable              = &m_ConstantVariables[CONSTANT_VARIABLE_LIGHT_SPECULAR];
	pConstantVariable->ShaderType  = IEffect::FRAGMENT_SHADER;
	pConstantVariable->GlobalType  = CEffectCode::LIGHT_VARIABLE;
	pConstantVariable->uDetail     = CRenderMethod::LIGHT_SPECULAR;
	pConstantVariable->uCount      = 1;
	pConstantVariable->uIndex      = uLightIndex;
	pConstantVariable->pValue      = ALCHEMY_NULL;
	pConstantVariable->bIsReadOnly = true;
	pConstantVariable->bIsForce    = true;

	pConstantVariable              = &m_ConstantVariables[CONSTANT_VARIABLE_LIGHT_LOCAL_DIRECTION];
	pConstantVariable->ShaderType  = IEffect::FRAGMENT_SHADER;
	pConstantVariable->GlobalType  = CEffectCode::LIGHT_VARIABLE;
	pConstantVariable->uDetail     = CRenderMethod::LIGHT_LOCAL_DIRECTION;
	pConstantVariable->uCount      = 1;
	pConstantVariable->uIndex      = uLightIndex;
	pConstantVariable->pValue      = ALCHEMY_NULL;
	pConstantVariable->bIsReadOnly = true;
	pConstantVariable->bIsForce    = true;
}

CSpotLightEffect::~CSpotLightEffect(void)
{
}

UINT                                              CSpotLightEffect::GetEffectInfoFlag    (                            )
{
	return ALCHEMY_FLAG(IEffectInfo::FRAGMENT_SHADER_INFO);
}

const IEffectInfo::SAMPLER             * CSpotLightEffect::GetSamplers         (UINT& uSamplerCount          )
{
	uSamplerCount = TOTAL_SAMPLERS;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION             * CSpotLightEffect::GetAttributes       (UINT& uAttributeCount        )
{
	uAttributeCount = TOTAL_ATTRIBUTES;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION            * CSpotLightEffect::GetVarying           (UINT& uVaryingCount          )
{
	uVaryingCount = TOTAL_VARYING;

	return ALCHEMY_NULL;
}

const CEffectCode::GLOBALVARIABLE              * CSpotLightEffect::GetConstants         (UINT& uConstantCount         )
{
	uConstantCount = TOTAL_CONSTANTS;

	return m_Constants;
}

const CEffectCode::TEMPORARYDECLARATION        * CSpotLightEffect::GetTemporary         (UINT& uTemporaryCount        )
{
	uTemporaryCount = TOTAL_TEMPORARY;

	return sm_TEMPORARY;
}

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CSpotLightEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
{
	uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLES;

	return sm_ATTRIBUTE_VARIABLES;
}

const CEffectCode::CONSTANTVARIABLEDECLARATION * CSpotLightEffect::GetConstantVariables (UINT& uConstantVariableCount )
{
	uConstantVariableCount = TOTAL_CONSTANT_VARIABLES;

	return m_ConstantVariables;
}

bool CSpotLightEffect::ShaderDefineHandler(
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
	const CEffectCode::SHADERVARIABLE* pLightDiffuse = &pConstantVariables[CONSTANT_VARIABLE_LIGHT_DIFFUSE], *pLightSpecular = &pConstantVariables[CONSTANT_VARIABLE_LIGHT_SPECULAR];
	if(pLightDiffuse->uHandle || pLightSpecular->uHandle)
	{

		UINT uLightFalloff = puTemporary[TEMPORARY_LIGHT_FALLOFF];
		
		const CEffectCode::SHADERVARIABLE* pLightDirection = &pConstantVariables[CONSTANT_VARIABLE_LIGHT_LOCAL_DIRECTION]; 
		UINT uDirection = pLightDirection->uHandle;
		if(uDirection == pLightDirection->uSource)
		{
			uDirection = puTemporary[TEMPORARY_LIGHT_DIRECTION];
			UINT uAddress[] = {0, 1, 2};
			FragmentShaderDefiner.AppendInstruction(ICompiler::SUB, uDirection, uAddress, 3, pAttributeVariables[ATTRIBUTE_VARIABLE_POSITION].uHandle, uAddress, 3, puConstants[CONSTANT_LIGHT_LOCAL_POSITION], uAddress, 3);
			FragmentShaderDefiner.AppendInstruction(ICompiler::NRM, uDirection, uDirection, 0);
		}

		FragmentShaderDefiner.AppendInstruction(ICompiler::DP3, uLightFalloff, uDirection   , puConstants[CONSTANT_LIGHT_LOCAL_DIRECTION]);
		FragmentShaderDefiner.AppendInstruction(ICompiler::SUB, uLightFalloff, uLightFalloff, puConstants[CONSTANT_LIGHT_COS_THETA      ]);
		FragmentShaderDefiner.AppendInstruction(ICompiler::MAX, uLightFalloff, uLightFalloff, puConstants[CONSTANT_ZERO                 ]);
		UINT x = 0;
		uDirection = puTemporary[TEMPORARY_LIGHT_DIRECTION];
		FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uDirection, &x, 1, uLightFalloff, &x, 1, puConstants[CONSTANT_LIGHT_INVERSE_COS_PHI_SUBSTRUCT_COS_THETA], &x, 1);
		UINT y = 1, uOne = puConstants[CONSTANT_ONE];
		FragmentShaderDefiner.AppendInstruction(ICompiler::SLT, uDirection, &y, 1, uOne      , &x, 1, uDirection, &x, 1);
		UINT z = 2;
		FragmentShaderDefiner.AppendInstruction(ICompiler::SUB, uDirection, &z, 1, uOne      , &x, 1, uDirection, &y, 1);
		FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uDirection, &x, 1, uDirection, &x, 1, uDirection, &z, 1);
		
		FragmentShaderDefiner.AppendInstruction(ICompiler::POW, uLightFalloff, uLightFalloff, puConstants[CONSTANT_LIGHT_FALLOFF]);
		
		FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uLightFalloff, &x, 1, uLightFalloff, &x, 1, uDirection, &x, 1);
		FragmentShaderDefiner.AppendInstruction(ICompiler::ADD, uLightFalloff, &x, 1, uLightFalloff, &x, 1, uDirection, &y, 1);

		if(pLightDiffuse->uHandle)
			FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, pLightDiffuse->uHandle , pLightDiffuse->bIsUsed  ? pLightDiffuse->uHandle  : pLightDiffuse->uSource , uLightFalloff);

		if(pLightSpecular->uHandle)
			FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, pLightSpecular->uHandle, pLightSpecular->bIsUsed ? pLightSpecular->uHandle : pLightSpecular->uSource, uLightFalloff);
	}

	return true;
}