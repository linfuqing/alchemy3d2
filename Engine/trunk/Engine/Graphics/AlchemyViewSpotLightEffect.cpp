#include "AlchemyViewSpotLightEffect.h"
#include "AlchemyRenderMethod.h"

using namespace alchemy;

const CEffectCode::TEMPORARYDECLARATION         CViewSpotLightEffect::sm_TEMPORARY          [CViewSpotLightEffect::TOTAL_TEMPORARY        ] = 
{
	{IEffect::FRAGMENT_SHADER, CEffectCode::VECTOR1_VARIABLE, 1}
};

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION CViewSpotLightEffect::sm_ATTRIBUTE_VARIABLES[CViewSpotLightEffect::TOTAL_ATTRIBUTE_VARIABLES] = 
{
	{IEffect::FRAGMENT_SHADER, CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION, 0, true , true}
};

CViewSpotLightEffect::CViewSpotLightEffect(UINT uLightIndex)
{
	CEffectCode::LPGLOBALVARIABLE pConstant;

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

	pConstant              = &m_Constants[CONSTANT_LIGHT_VIEW_POSITION];
	pConstant->ShaderType  = IEffect::FRAGMENT_SHADER;
	pConstant->GlobalType  = CEffectCode::LIGHT_VARIABLE;
	pConstant->uDetail     = CRenderMethod::LIGHT_VIEW_POSITION;
	pConstant->uCount      = 1;
	pConstant->uIndex      = uLightIndex;
	pConstant->pValue      = ALCHEMY_NULL;

	pConstant              = &m_Constants[CONSTANT_LIGHT_VIEW_DIRECTION];
	pConstant->ShaderType  = IEffect::FRAGMENT_SHADER;
	pConstant->GlobalType  = CEffectCode::LIGHT_VARIABLE;
	pConstant->uDetail     = CRenderMethod::LIGHT_VIEW_DIRECTION;
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

	pConstantVariable              = &m_ConstantVariables[CONSTANT_VARIABLE_LIGHT_VIEW_DIRECTION];
	pConstantVariable->ShaderType  = IEffect::FRAGMENT_SHADER;
	pConstantVariable->GlobalType  = CEffectCode::LIGHT_VARIABLE;
	pConstantVariable->uDetail     = CRenderMethod::LIGHT_VIEW_DIRECTION;
	pConstantVariable->uCount      = 1;
	pConstantVariable->uIndex      = uLightIndex;
	pConstantVariable->pValue      = ALCHEMY_NULL;
	pConstantVariable->bIsReadOnly = true;
	pConstantVariable->bIsForce    = true;
}

CViewSpotLightEffect::~CViewSpotLightEffect(void)
{
}

UINT                                              CViewSpotLightEffect::GetEffectInfoFlag    (                            )
{
	return ALCHEMY_FLAG(IEffectInfo::FRAGMENT_SHADER_INFO);
}

const IEffectInfo::SAMPLER             * CViewSpotLightEffect::GetSamplers         (UINT& uSamplerCount          )
{
	uSamplerCount = TOTAL_SAMPLERS;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION             * CViewSpotLightEffect::GetAttributes       (UINT& uAttributeCount        )
{
	uAttributeCount = TOTAL_ATTRIBUTES;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION            * CViewSpotLightEffect::GetVarying           (UINT& uVaryingCount          )
{
	uVaryingCount = TOTAL_VARYING;

	return ALCHEMY_NULL;
}

const CEffectCode::GLOBALVARIABLE              * CViewSpotLightEffect::GetConstants         (UINT& uConstantCount         )
{
	uConstantCount = TOTAL_CONSTANTS;

	return m_Constants;
}

const CEffectCode::TEMPORARYDECLARATION        * CViewSpotLightEffect::GetTemporary         (UINT& uTemporaryCount        )
{
	uTemporaryCount = TOTAL_TEMPORARY;

	return sm_TEMPORARY;
}

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CViewSpotLightEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
{
	uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLES;

	return sm_ATTRIBUTE_VARIABLES;
}

const CEffectCode::CONSTANTVARIABLEDECLARATION * CViewSpotLightEffect::GetConstantVariables (UINT& uConstantVariableCount )
{
	uConstantVariableCount = TOTAL_CONSTANT_VARIABLES;

	return m_ConstantVariables;
}

bool CViewSpotLightEffect::ShaderDefineHandler(
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

		const CEffectCode::SHADERVARIABLE* pLightDirection = &pConstantVariables[CONSTANT_VARIABLE_LIGHT_VIEW_DIRECTION]; 
		UINT uDirection = pLightDirection->uHandle;
		if(uDirection == pLightDirection->uSource)
		{
			uDirection = uLightFalloff;
			FragmentShaderDefiner.AppendInstruction(ICompiler::SUB, uDirection, pAttributeVariables[ATTRIBUTE_VARIABLE_POSITION].uHandle, pConstantVariables[CONSTANT_LIGHT_VIEW_POSITION].uHandle);
			FragmentShaderDefiner.AppendInstruction(ICompiler::NRM, uDirection, uDirection                                              , 0                                                        );
		}

		FragmentShaderDefiner.AppendInstruction(ICompiler::DP3, uLightFalloff, uDirection                                               , puConstants[CONSTANT_LIGHT_VIEW_DIRECTION                     ]);
		FragmentShaderDefiner.AppendInstruction(ICompiler::SUB, uLightFalloff, uLightFalloff                                            , puConstants[CONSTANT_LIGHT_COS_THETA                          ]);
		FragmentShaderDefiner.AppendInstruction(ICompiler::SAT, uLightFalloff, uLightFalloff                                            , 0                                                              );
		FragmentShaderDefiner.AppendInstruction(ICompiler::POW, uLightFalloff, uLightFalloff                                            , puConstants[CONSTANT_LIGHT_FALLOFF                            ]);
		FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uLightFalloff, uLightFalloff                                            , puConstants[CONSTANT_LIGHT_INVERSE_COS_PHI_SUBSTRUCT_COS_THETA]);
		FragmentShaderDefiner.AppendInstruction(ICompiler::SAT, uLightFalloff, uLightFalloff                                            , 0                                                              );

		if(pLightDiffuse->uHandle)
			FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, pLightDiffuse->uHandle , pLightDiffuse->bIsUsed  ? pLightDiffuse->uHandle  : pLightDiffuse->uSource , uLightFalloff);

		if(pLightSpecular->uHandle)
			FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, pLightSpecular->uHandle, pLightSpecular->bIsUsed ? pLightSpecular->uHandle : pLightSpecular->uSource, uLightFalloff);
	}

	return true;
}