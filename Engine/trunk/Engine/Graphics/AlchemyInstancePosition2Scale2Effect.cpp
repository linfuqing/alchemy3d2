#include "AlchemyInstancePosition2Scale2Effect.h"

using namespace alchemy;

const CEffectCode::ELEMENTDECLARATION             CInstancePosition2Scale2Effect::sm_VARYING          [CInstancePosition2Scale2Effect::TOTAL_VARYING           ] = 
{ 
	{CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITIONT, 0} 
};

const CEffectCode::TEMPORARYDECLARATION           CInstancePosition2Scale2Effect::sm_TEMPORARY        [CInstancePosition2Scale2Effect::TOTAL_TEMPORARY           ] = 
{
	{IEffect::VERTEX_SHADER, CEffectCode::VECTOR1_VARIABLE, 1}, 
	{IEffect::VERTEX_SHADER, CEffectCode::VECTOR2_VARIABLE, 1}
};

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION CInstancePosition2Scale2Effect::sm_ATTRIBUTE_VARIABLE[CInstancePosition2Scale2Effect::TOTAL_ATTRIBUTE_VARIABLES] = 
{ 
	{IEffect::VERTEX_SHADER, CVertexDeclaration::FLOAT1, CVertexDeclaration::INSTANCEINDEX, 0, true, true}, 
	{IEffect::VERTEX_SHADER, CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION, 0, true, true}, 
	{IEffect::VERTEX_SHADER, CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITIONT, 0, false, false}
};

CInstancePosition2Scale2Effect::CInstancePosition2Scale2Effect(UINT uInstanceCount, const FLOAT4* pTransform)
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

CInstancePosition2Scale2Effect::~CInstancePosition2Scale2Effect(void)
{
}

UINT                                            CInstancePosition2Scale2Effect::GetEffectInfoFlag     (                             )
{
	return ALCHEMY_FLAG(IEffectInfo::VERTEX_SHADER_INFO) | ALCHEMY_FLAG(IEffectInfo::VERTEX_SHADER_OUTPUT);
}

const IEffectInfo::SAMPLER           * CInstancePosition2Scale2Effect::GetSamplers           (UINT& uSamplerCount          )
{
	uSamplerCount = TOTAL_SAMPLERS;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION            * CInstancePosition2Scale2Effect::GetAttributes        (UINT& uAttributeCount        )
{
	uAttributeCount = TOTAL_ATTRIBUTES;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION            * CInstancePosition2Scale2Effect::GetVarying           (UINT& uVaryingCount          )
{
	uVaryingCount = TOTAL_VARYING;

	return sm_VARYING;
}

const CEffectCode::GLOBALVARIABLE              * CInstancePosition2Scale2Effect::GetConstants         (UINT& uConstantCount         )
{
	uConstantCount = TOTAL_CONSTANTS;

	return m_Constants;
}

const CEffectCode::TEMPORARYDECLARATION        * CInstancePosition2Scale2Effect::GetTemporary         (UINT& uTemporaryCount        )
{
	uTemporaryCount = TOTAL_TEMPORARY;

	return sm_TEMPORARY;
}

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CInstancePosition2Scale2Effect::GetAttributeVariables(UINT& uAttributeVariableCount)
{
	uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLES;

	return sm_ATTRIBUTE_VARIABLE;
}

const CEffectCode::CONSTANTVARIABLEDECLARATION * CInstancePosition2Scale2Effect::GetConstantVariables  (UINT& uConstantVariableCount)
{
	uConstantVariableCount = TOTAL_CONSTANT_VARIABLES;

	return ALCHEMY_NULL;
}

bool CInstancePosition2Scale2Effect::ShaderDefineHandler(
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
	const CEffectCode::VARYING* pPositiont = &pVarying[VARYING_POSITIONT];
	if(!pPositiont->IsUsed)
	{
		UINT uTransformIndex = puTemporary[TEMPORARY_TRANSFORM_INDEX];
		VertexShaderDefiner.AppendInstruction(ICompiler::MUL, uTransformIndex, pAttributeVariables[ATTRIBUTE_VARIABLE_INSTANCE_INDEX].uHandle, puConstants[CONSTANT_VECTOR4_SIZE]);

		UINT uTemporayPosition = puTemporary[TEMPORARY_POSITION], uTransform = puConstants[CONSTANT_TRANSFORMS];
		const CEffectCode::SHADERVARIABLE* pPosition = &pAttributeVariables[ATTRIBUTE_VARIABLE_POSITION];

		UINT xy[] = {0, 1};
		UINT zw[] = {2, 3};
		VertexShaderDefiner.AppendInstruction(ICompiler::MUL, 
			uTemporayPosition, 
			0,
			0,
			xy,
			2,
			pPosition->uHandle, 
			0, 
			0, 
			xy,
			2, 
			uTransform, 
			uTransformIndex, 
			0, 
			zw,
			2);

		const CEffectCode::SHADERVARIABLE* pOutputPosition = &pAttributeVariables[ATTRIBUTE_VARIABLE_POSITIONT];
		UINT uOutputPosition = (pOutputPosition->uHandle && !pOutputPosition->bIsUsed) ? pOutputPosition->uHandle : pPositiont->uVertexShaderHandle;

		VertexShaderDefiner.AppendInstruction(ICompiler::ADD, 
			uOutputPosition, 
			0,
			0,
			xy,
			2,
			uTemporayPosition, 
			0, 
			0, 
			xy,
			2, 
			uTransform, 
			uTransformIndex, 
			0, 
			xy,
			2);

		VertexShaderDefiner.AppendInstruction(ICompiler::MOV, 
			uOutputPosition, 
			zw,
			2,
			pPosition->uHandle, 
			zw,
			2, 
			0, 
			ALCHEMY_NULL,
			0);

		if(uOutputPosition != pPositiont->uVertexShaderHandle)
			VertexShaderDefiner.AppendInstruction(ICompiler::MOV, pPositiont->uVertexShaderHandle, uOutputPosition, 0);
	}

	return true;
}