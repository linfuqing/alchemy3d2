#include "AlchemyInstancePosition3Scale1Effect.h"

using namespace alchemy;

const CEffectCode::TEMPORARYDECLARATION           CInstancePosition3Scale1Effect::sm_TEMPORARY        [CInstancePosition3Scale1Effect::TOTAL_TEMPORARY           ] = 
{
	{IEffect::VERTEX_SHADER, CEffectCode::VECTOR1_VARIABLE, 1}
};

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION CInstancePosition3Scale1Effect::sm_ATTRIBUTE_VARIABLE[CInstancePosition3Scale1Effect::TOTAL_ATTRIBUTE_VARIABLES] = 
{ 
	{IEffect::VERTEX_SHADER, CVertexDeclaration::FLOAT1, CVertexDeclaration::INSTANCEINDEX, 0, true, true}, 
	{IEffect::VERTEX_SHADER, CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION, 0, false, true}
};

CInstancePosition3Scale1Effect::CInstancePosition3Scale1Effect(UINT uInstanceCount, const FLOAT4* pTransform)
{
	CEffectCode::LPGLOBALVARIABLE pConstant;

	pConstant = &m_Constants[CONSTANT_TRANSFORMS];
	pConstant->ShaderType = IEffect::VERTEX_SHADER;
	pConstant->GlobalType = CEffectCode::ARRAY_VARIABLE;
	pConstant->uDetail    = IRenderMethod::INSTANCE_TRANSFORM_ARRAY;
	pConstant->uCount     = uInstanceCount;
	pConstant->uIndex     = 0;
	pConstant->pValue     = pTransform;

	pConstant = &m_Constants[CONSTANT_VECTOR4_SIZE];
	pConstant->ShaderType = IEffect::VERTEX_SHADER;
	pConstant->GlobalType = CEffectCode::INPUT_VARIABLE;
	pConstant->uDetail    = CEffectCode::VECTOR4_SIZE;
	pConstant->uCount     = 1;
	pConstant->uIndex     = 0;
	pConstant->pValue     = ALCHEMY_NULL;
}

CInstancePosition3Scale1Effect::CInstancePosition3Scale1Effect(const FLOAT4** ppTransform, UINT uInstanceCount)
{
	CEffectCode::LPGLOBALVARIABLE pConstant;

	pConstant = &m_Constants[CONSTANT_TRANSFORMS];
	pConstant->ShaderType = IEffect::VERTEX_SHADER;
	pConstant->GlobalType = CEffectCode::INPUT_VARIABLE;
	pConstant->uDetail    = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR4_VARIABLE, 4, 4, true);
	pConstant->uCount     = uInstanceCount;
	pConstant->uIndex     = 0;
	pConstant->pValue     = ppTransform;

	pConstant = &m_Constants[CONSTANT_VECTOR4_SIZE];
	pConstant->ShaderType = IEffect::VERTEX_SHADER;
	pConstant->GlobalType = CEffectCode::INPUT_VARIABLE;
	pConstant->uDetail    = CEffectCode::VECTOR4_SIZE;
	pConstant->uCount     = 1;
	pConstant->uIndex     = 0;
	pConstant->pValue     = ALCHEMY_NULL;
}

CInstancePosition3Scale1Effect::~CInstancePosition3Scale1Effect(void)
{
}

UINT                                            CInstancePosition3Scale1Effect::GetEffectInfoFlag     (                             )
{
	return ALCHEMY_FLAG(IEffectInfo::VERTEX_SHADER_INFO);
}

const IEffectInfo::SAMPLER           * CInstancePosition3Scale1Effect::GetSamplers           (UINT& uSamplerCount          )
{
	uSamplerCount = TOTAL_SAMPLERS;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION            * CInstancePosition3Scale1Effect::GetAttributes        (UINT& uAttributeCount        )
{
	uAttributeCount = TOTAL_ATTRIBUTES;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION            * CInstancePosition3Scale1Effect::GetVarying           (UINT& uVaryingCount          )
{
	uVaryingCount = TOTAL_VARYING;

	return ALCHEMY_NULL;
}

const CEffectCode::GLOBALVARIABLE              * CInstancePosition3Scale1Effect::GetConstants         (UINT& uConstantCount         )
{
	uConstantCount = TOTAL_CONSTANTS;

	return m_Constants;
}

const CEffectCode::TEMPORARYDECLARATION        * CInstancePosition3Scale1Effect::GetTemporary         (UINT& uTemporaryCount        )
{
	uTemporaryCount = TOTAL_TEMPORARY;

	return sm_TEMPORARY;
}

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CInstancePosition3Scale1Effect::GetAttributeVariables(UINT& uAttributeVariableCount)
{
	uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLES;

	return sm_ATTRIBUTE_VARIABLE;
}

const CEffectCode::CONSTANTVARIABLEDECLARATION * CInstancePosition3Scale1Effect::GetConstantVariables  (UINT& uConstantVariableCount)
{
	uConstantVariableCount = TOTAL_CONSTANT_VARIABLES;

	return ALCHEMY_NULL;
}

bool CInstancePosition3Scale1Effect::ShaderDefineHandler(
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
	const CEffectCode::SHADERVARIABLE* pPosition = &pAttributeVariables[ATTRIBUTE_VARIABLE_POSITION];

	if(pPosition->uHandle)
	{
		UINT uTransformIndex = puTemporary[TEMPORARY_TRANSFORM_INDEX];
		VertexShaderDefiner.AppendInstruction(ICompiler::MUL, uTransformIndex, pAttributeVariables[ATTRIBUTE_VARIABLE_INSTANCE_INDEX].uHandle, puConstants[CONSTANT_VECTOR4_SIZE]);

		UINT w = 3;
		if(!pPosition->bIsUsed)
			VertexShaderDefiner.AppendInstruction(ICompiler::MOV, pPosition->uHandle, &w, 1, pPosition->uSource, &w, 1, 0, ALCHEMY_NULL, 0);

		UINT uAddress[] = {0, 1, 2};
		VertexShaderDefiner.AppendInstruction(ICompiler::MUL, 
			pPosition->uHandle, 
			0,
			0,
			uAddress,
			3,
			pPosition->bIsUsed ? pPosition->uHandle : pPosition->uSource, 
			0, 
			0, 
			uAddress,
			3, 
			puConstants[CONSTANT_TRANSFORMS], 
			uTransformIndex, 
			0, 
			&w,
			1);

		VertexShaderDefiner.AppendInstruction(ICompiler::ADD, 
			pPosition->uHandle, 
			0,
			0,
			uAddress,
			3,
			pPosition->uHandle, 
			0, 
			0, 
			uAddress,
			3, 
			puConstants[CONSTANT_TRANSFORMS], 
			uTransformIndex, 
			0, 
			uAddress,
			3);
	}

	return true;
}