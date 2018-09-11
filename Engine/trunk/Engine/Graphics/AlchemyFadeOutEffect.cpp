#include "AlchemyFadeOutEffect.h"

using namespace alchemy;

/*********************************

CFadeOutParameterHandler

**********************************/

/*********************************

CFadeOutEffect

**********************************/

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION	CFadeOutEffect::sm_ATTRIBUTES[TOTAL_ATTRIBUTE_VARIABLES] = { {IEffect::VERTEX_SHADER,CVertexDeclaration::FLOAT4,CVertexDeclaration::DIFFUSE,0,false,true} };
//const CEffectCode::ELEMENTDECLARATION				CFadeOutEffect::sm_VARYINGS[TOTAL_VARYING] = { {CVertexDeclaration::FLOAT1,CVertexDeclaration::DIFFUSE,0} };
const CEffectCode::TEMPORARYDECLARATION			CFadeOutEffect::sm_TEMPORARY[TOTAL_TEMPORARY] = { {IEffect::VERTEX_SHADER, CEffectCode::VECTOR1_VARIABLE, 1}, {IEffect::VERTEX_SHADER, CEffectCode::VECTOR4_VARIABLE, 1}  };

CFadeOutEffect::CFadeOutEffect(const UINT& ModID, const FLOAT& SystemTime, FLOAT TotalTime, const FLOAT4 TargetColor)
:m_ModID(ModID)
{
	m_TargetColor = TargetColor;

	m_fTotalTime = TotalTime;

	m_ConstantVariables[CONSTANT_SYSTEM_TIME].ShaderType = IEffect::VERTEX_SHADER;
	m_ConstantVariables[CONSTANT_SYSTEM_TIME].GlobalType = CEffectCode::INPUT_VARIABLE;
	m_ConstantVariables[CONSTANT_SYSTEM_TIME].uDetail = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR1_VARIABLE, 1, 1, false);
	m_ConstantVariables[CONSTANT_SYSTEM_TIME].uCount = 1;
	m_ConstantVariables[CONSTANT_SYSTEM_TIME].uIndex = 0;
	m_ConstantVariables[CONSTANT_SYSTEM_TIME].pValue =  reinterpret_cast<const VOID*>(&SystemTime);
	m_ConstantVariables[CONSTANT_SYSTEM_TIME].bIsForce = true;
	m_ConstantVariables[CONSTANT_SYSTEM_TIME].bIsReadOnly = true;

	m_ConstantVariables[CONSTANT_TOTAL_TIME].ShaderType = IEffect::VERTEX_SHADER;
	m_ConstantVariables[CONSTANT_TOTAL_TIME].GlobalType = CEffectCode::INPUT_VARIABLE;
	m_ConstantVariables[CONSTANT_TOTAL_TIME].uDetail = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR1_VARIABLE, 1, 1, false);
	m_ConstantVariables[CONSTANT_TOTAL_TIME].uCount = 1;
	m_ConstantVariables[CONSTANT_TOTAL_TIME].uIndex = 0;
	m_ConstantVariables[CONSTANT_TOTAL_TIME].pValue = reinterpret_cast<const VOID*>(&m_fTotalTime);
	m_ConstantVariables[CONSTANT_TOTAL_TIME].bIsForce = true;
	m_ConstantVariables[CONSTANT_TOTAL_TIME].bIsReadOnly = true;

	m_ConstantVariables[CONSTANT_TARGET_COLOR].ShaderType = IEffect::VERTEX_SHADER;
	m_ConstantVariables[CONSTANT_TARGET_COLOR].GlobalType = CEffectCode::INPUT_VARIABLE;
	m_ConstantVariables[CONSTANT_TARGET_COLOR].uDetail = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR4_VARIABLE, 1, 4, false);
	m_ConstantVariables[CONSTANT_TARGET_COLOR].uCount = 1;
	m_ConstantVariables[CONSTANT_TARGET_COLOR].uIndex = 0;
	m_ConstantVariables[CONSTANT_TARGET_COLOR].pValue = reinterpret_cast<const VOID*>(&m_TargetColor);
	m_ConstantVariables[CONSTANT_TARGET_COLOR].bIsForce = true;
	m_ConstantVariables[CONSTANT_TARGET_COLOR].bIsReadOnly = true;
}

CFadeOutEffect::~CFadeOutEffect()
{

}

UINT CFadeOutEffect::GetEffectInfoFlag()
{
	return ALCHEMY_FLAG( IEffectInfo::VERTEX_SHADER_INFO | IEffectInfo::FRAGMENT_SHADER_INFO | IEffectInfo::VERTEX_SHADER_OUTPUT );
}

const IEffectInfo::SAMPLER* CFadeOutEffect::GetSamplers(UINT& uSamplerCount)
{
	uSamplerCount = TOTAL_SAMPLERS;
	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION* CFadeOutEffect::GetAttributes(UINT& uAttributeCount)
{
	uAttributeCount = TOTAL_ATTRIBUTES;
	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION* CFadeOutEffect::GetVarying(UINT& uVaryingCount)
{
	uVaryingCount = TOTAL_VARYING;
	return ALCHEMY_NULL;
}

const CEffectCode::GLOBALVARIABLE* CFadeOutEffect::GetConstants(UINT& uConstantCount)
{
	uConstantCount = TOTAL_CONSTANTS;
	return ALCHEMY_NULL;
}

const CEffectCode::TEMPORARYDECLARATION* CFadeOutEffect::GetTemporary(UINT& uTemporaryCount)
{
	uTemporaryCount = TOTAL_TEMPORARY;
	return sm_TEMPORARY;
}

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CFadeOutEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
{
	uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLES;
	return sm_ATTRIBUTES;
}

const CEffectCode::CONSTANTVARIABLEDECLARATION * CFadeOutEffect::GetConstantVariables (UINT& uConstantVariableCount )
{
	uConstantVariableCount = TOTAL_CONSTANT_VARIABLES;
	return m_ConstantVariables;
}

bool CFadeOutEffect::ShaderDefineHandler(
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
	UINT uSubAddress[] = {0, 1, 2, 3};

	VertexShaderDefiner.AppendInstruction( ICompiler::SUB, puTemporary[TEMPORARY_CUR_TIME], pConstantVariables[CONSTANT_TOTAL_TIME].uHandle, m_ModID);

	VertexShaderDefiner.AppendInstruction( ICompiler::MUL, puTemporary[TEMPORARY_COLOR], uSubAddress, 4, puTemporary[TEMPORARY_CUR_TIME], uSubAddress, 1,
					pConstantVariables[CONSTANT_TARGET_COLOR].uHandle, uSubAddress, 4);

	if(pAttributeVariables[ATTRIBUTE_DIFFUSE_COLOR].uHandle)
	{
		if(pAttributeVariables[ATTRIBUTE_DIFFUSE_COLOR].bIsUsed)
			VertexShaderDefiner.AppendInstruction( ICompiler::ADD, pAttributeVariables[ATTRIBUTE_DIFFUSE_COLOR].uHandle, uSubAddress, 4, pAttributeVariables[ATTRIBUTE_DIFFUSE_COLOR].uHandle, uSubAddress, 4,puTemporary[TEMPORARY_COLOR], uSubAddress, 4);
		else
			VertexShaderDefiner.AppendInstruction( ICompiler::ADD, pAttributeVariables[ATTRIBUTE_DIFFUSE_COLOR].uHandle, uSubAddress, 4, pAttributeVariables[ATTRIBUTE_DIFFUSE_COLOR].uSource, uSubAddress, 4,puTemporary[TEMPORARY_COLOR], uSubAddress, 4);
	}

	return true;
}