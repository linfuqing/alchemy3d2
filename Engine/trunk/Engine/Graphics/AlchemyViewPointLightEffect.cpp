#include "AlchemyViewPointLightEffect.h"
#include "AlchemyRenderMethod.h"

using namespace alchemy;

const CEffectCode::TEMPORARYDECLARATION         CViewPointLightEffect::sm_TEMPORARY          [CViewPointLightEffect::TOTAL_TEMPORARY        ] = 
{
	{IEffect::FRAGMENT_SHADER, CEffectCode::VECTOR3_VARIABLE, 1}, 
	{IEffect::FRAGMENT_SHADER, CEffectCode::VECTOR1_VARIABLE, 1}, 
};

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION CViewPointLightEffect::sm_ATTRIBUTE_VARIABLES[CViewPointLightEffect::TOTAL_ATTRIBUTE_VARIABLES] = 
{
	{IEffect::FRAGMENT_SHADER, CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION, 0, true , true},
};

CViewPointLightEffect::CViewPointLightEffect(UINT uLightIndex)
{
	CEffectCode::LPGLOBALVARIABLE pConstant;

	pConstant              = &m_Constants[CONSTANT_LIGHT_VIEW_POSITION];
	pConstant->ShaderType  = IEffect::FRAGMENT_SHADER;
	pConstant->GlobalType  = CEffectCode::LIGHT_VARIABLE;
	pConstant->uDetail     = CRenderMethod::LIGHT_VIEW_POSITION;
	pConstant->uCount      = 1;
	pConstant->uIndex      = uLightIndex;
	pConstant->pValue      = ALCHEMY_NULL;

	pConstant              = &m_Constants[CONSTANT_LIGHT_ATTENUATION0];
	pConstant->ShaderType  = IEffect::FRAGMENT_SHADER;
	pConstant->GlobalType  = CEffectCode::LIGHT_VARIABLE;
	pConstant->uDetail     = CRenderMethod::LIGHT_ATTENUATION0;
	pConstant->uCount      = 1;
	pConstant->uIndex      = uLightIndex;
	pConstant->pValue      = ALCHEMY_NULL;

	pConstant              = &m_Constants[CONSTANT_LIGHT_ATTENUATION1];
	pConstant->ShaderType  = IEffect::FRAGMENT_SHADER;
	pConstant->GlobalType  = CEffectCode::LIGHT_VARIABLE;
	pConstant->uDetail     = CRenderMethod::LIGHT_ATTENUATION1;
	pConstant->uCount      = 1;
	pConstant->uIndex      = uLightIndex;
	pConstant->pValue      = ALCHEMY_NULL;

	pConstant              = &m_Constants[CONSTANT_LIGHT_ATTENUATION2];
	pConstant->ShaderType  = IEffect::FRAGMENT_SHADER;
	pConstant->GlobalType  = CEffectCode::LIGHT_VARIABLE;
	pConstant->uDetail     = CRenderMethod::LIGHT_ATTENUATION2;
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
	pConstantVariable->bIsReadOnly = false;
	pConstantVariable->bIsForce    = true;

	pConstantVariable              = &m_ConstantVariables[CONSTANT_VARIABLE_LIGHT_SPECULAR];
	pConstantVariable->ShaderType  = IEffect::FRAGMENT_SHADER;
	pConstantVariable->GlobalType  = CEffectCode::LIGHT_VARIABLE;
	pConstantVariable->uDetail     = CRenderMethod::LIGHT_SPECULAR;
	pConstantVariable->uCount      = 1;
	pConstantVariable->uIndex      = uLightIndex;
	pConstantVariable->pValue      = ALCHEMY_NULL;
	pConstantVariable->bIsReadOnly = false;
	pConstantVariable->bIsForce    = true;

	pConstantVariable              = &m_ConstantVariables[CONSTANT_VARIABLE_LIGHT_VIEW_DIRECTION];
	pConstantVariable->ShaderType  = IEffect::FRAGMENT_SHADER;
	pConstantVariable->GlobalType  = CEffectCode::LIGHT_VARIABLE;
	pConstantVariable->uDetail     = CRenderMethod::LIGHT_VIEW_DIRECTION;
	pConstantVariable->uCount      = 1;
	pConstantVariable->uIndex      = uLightIndex;
	pConstantVariable->pValue      = ALCHEMY_NULL;
	pConstantVariable->bIsReadOnly = false;
	pConstantVariable->bIsForce    = false;

	pConstantVariable              = &m_ConstantVariables[CONSTANT_VARIABLE_LIGHT_VIEW_DIRECTION_NEGATE];
	pConstantVariable->ShaderType  = IEffect::FRAGMENT_SHADER;
	pConstantVariable->GlobalType  = CEffectCode::LIGHT_VARIABLE;
	pConstantVariable->uDetail     = CRenderMethod::LIGHT_VIEW_DIRECTION_NEGATE;
	pConstantVariable->uCount      = 1;
	pConstantVariable->uIndex      = uLightIndex;
	pConstantVariable->pValue      = ALCHEMY_NULL;
	pConstantVariable->bIsReadOnly = false;
	pConstantVariable->bIsForce    = false;
}

CViewPointLightEffect::~CViewPointLightEffect(void)
{
}

UINT                                              CViewPointLightEffect::GetEffectInfoFlag    (                            )
{
	return ALCHEMY_FLAG(IEffectInfo::FRAGMENT_SHADER_INFO);
}

const IEffectInfo::SAMPLER             * CViewPointLightEffect::GetSamplers         (UINT& uSamplerCount          )
{
	uSamplerCount = TOTAL_SAMPLERS;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION             * CViewPointLightEffect::GetAttributes       (UINT& uAttributeCount        )
{
	uAttributeCount = TOTAL_ATTRIBUTES;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION            * CViewPointLightEffect::GetVarying           (UINT& uVaryingCount          )
{
	uVaryingCount = TOTAL_VARYING;

	return ALCHEMY_NULL;
}

const CEffectCode::GLOBALVARIABLE              * CViewPointLightEffect::GetConstants         (UINT& uConstantCount         )
{
	uConstantCount = TOTAL_CONSTANTS;

	return m_Constants;
}

const CEffectCode::TEMPORARYDECLARATION        * CViewPointLightEffect::GetTemporary         (UINT& uTemporaryCount        )
{
	uTemporaryCount = TOTAL_TEMPORARY;

	return sm_TEMPORARY;
}

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CViewPointLightEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
{
	uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLES;

	return sm_ATTRIBUTE_VARIABLES;
}

const CEffectCode::CONSTANTVARIABLEDECLARATION * CViewPointLightEffect::GetConstantVariables (UINT& uConstantVariableCount )
{
	uConstantVariableCount = TOTAL_CONSTANT_VARIABLES;

	return m_ConstantVariables;
}

bool CViewPointLightEffect::ShaderDefineHandler(
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
		UINT uLightDistance = puTemporary[TEMPORARY_LIGHT_DISTANCE];
		UINT uAddress[] = {0, 1, 2};
		FragmentShaderDefiner.AppendInstruction(ICompiler::SUB, uLightDistance, uAddress, 3, pAttributeVariables[ATTRIBUTE_VARIABLE_POSITION].uHandle, uAddress, 3, puConstants[CONSTANT_LIGHT_VIEW_POSITION], uAddress, 3);

		const CEffectCode::SHADERVARIABLE* pLightDirection = &pConstantVariables[CONSTANT_VARIABLE_LIGHT_VIEW_DIRECTION]; 
		const CEffectCode::SHADERVARIABLE* pLightDirectionNegate = &pConstantVariables[CONSTANT_VARIABLE_LIGHT_VIEW_DIRECTION_NEGATE];
		if(pLightDirection->uHandle && !pLightDirection->bIsUsed)
		{
			FragmentShaderDefiner.AppendInstruction(ICompiler::NRM, pLightDirection->uHandle, uLightDistance, 0);

			if(pLightDirectionNegate->uHandle && !pLightDirectionNegate->bIsUsed)
				FragmentShaderDefiner.AppendInstruction(ICompiler::NEG, pLightDirectionNegate->uHandle, pLightDirection->uHandle, 0);
		}
		else if(pLightDirectionNegate->uHandle && !pLightDirectionNegate->bIsUsed)
		{
			FragmentShaderDefiner.AppendInstruction(ICompiler::NRM, pLightDirectionNegate->uHandle, uLightDistance                , 0);
			FragmentShaderDefiner.AppendInstruction(ICompiler::NEG, pLightDirectionNegate->uHandle, pLightDirectionNegate->uHandle, 0);
		}

		FragmentShaderDefiner.AppendInstruction(ICompiler::DP3, uLightDistance, uAddress, 1, uLightDistance, uAddress, 3, uLightDistance, uAddress, 3);

		UINT uLightAttenuation = puTemporary[TEMPORARY_LIGHT_ATTENUATION];
		FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uLightAttenuation, uAddress, 1, uLightDistance, uAddress, 1, puConstants[CONSTANT_LIGHT_ATTENUATION2], uAddress, 1);

		FragmentShaderDefiner.AppendInstruction(ICompiler::SQT, uLightDistance, uAddress, 1, uLightDistance, uAddress, 1, 0                                       , uAddress, 1);
		FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uLightDistance, uAddress, 1, uLightDistance, uAddress, 1, puConstants[CONSTANT_LIGHT_ATTENUATION1], uAddress, 1);

		FragmentShaderDefiner.AppendInstruction(ICompiler::ADD, uLightAttenuation, uAddress, 1, uLightAttenuation, uAddress, 1, uLightDistance, uAddress, 1);
		
		FragmentShaderDefiner.AppendInstruction(ICompiler::ADD, uLightAttenuation, uLightAttenuation, puConstants[CONSTANT_LIGHT_ATTENUATION0]);

		FragmentShaderDefiner.AppendInstruction(ICompiler::RCP, uLightAttenuation, uLightAttenuation, 0);

		if(pLightDiffuse->uHandle)
			FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, pLightDiffuse->uHandle, pLightDiffuse->bIsUsed ? pLightDiffuse->uHandle : pLightDiffuse->uSource, uLightAttenuation);

		if(pLightSpecular->uHandle)
			FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, pLightSpecular->uHandle, pLightSpecular->bIsUsed ? pLightSpecular->uHandle : pLightSpecular->uSource, uLightAttenuation);
	}

	return true;
}