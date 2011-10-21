#include "AlchemyHardwareSkinEffect.h"
#include "AlchemyRenderMethod.h"

using namespace alchemy;

const CEffectCode::TEMPORARYDECLARATION CHardwareSkinEffect::sm_TEMPORARY [CHardwareSkinEffect::TOTAL_TEMPORARY] = { {CEffectInterface::VERTEX_SHADER, CEffectCode::VECTOR1_VARIABLE}, {CEffectInterface::VERTEX_SHADER, CEffectCode::VECTOR3_VARIABLE} };


CHardwareSkinEffect::CHardwareSkinEffect(void) :
m_pAttributes(ALCHEMY_NULL),
m_pAttributeVariables(ALCHEMY_NULL),
m_uBonesCount(0),
m_uVertexInfluence(0),
m_uSkinElementParameterCount(0),
m_uBlendAttributeCount(0)
{
}

CHardwareSkinEffect::~CHardwareSkinEffect(void)
{
	Destroy();
}

UINT                                          CHardwareSkinEffect::GetEffectInfoFlag     (                            )
{
	return ALCHEMY_FLAG(CEffectInfoInterface::VERTEX_SHADER_INFO);
}

const CEffectInfoInterface::SAMPLER         * CHardwareSkinEffect::GetSamplers           (UINT& uSamplerCount         )
{
	uSamplerCount = TOTAL_SAMPLERS;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTPARAMETER         * CHardwareSkinEffect::GetAttributes         (UINT& uAttributeCount        )
{
	uAttributeCount = m_uBlendAttributeCount;

	return m_pAttributes;
}

const CEffectCode::ELEMENTPARAMETER         * CHardwareSkinEffect::GetVarying            (UINT& uVaryingCount          )
{
	uVaryingCount = TOTAL_VARYING;

	return ALCHEMY_NULL;
}

const CEffectCode::GLOBALVARIABLE           * CHardwareSkinEffect::GetConstants           (UINT& uConstantCount         )
{
	uConstantCount = 2;

	return m_Constants;
}

const CEffectCode::TEMPORARYDECLARATION     * CHardwareSkinEffect::GetTemporary           (UINT& uTemporaryCount        )
{
	uTemporaryCount = TOTAL_TEMPORARY;

	return sm_TEMPORARY;
}

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CHardwareSkinEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
{
	uAttributeVariableCount = m_uSkinElementParameterCount;

	return m_pAttributeVariables;
}

const CEffectCode::CONSTANTVARIABLEDECLARATION * CHardwareSkinEffect::GetConstantVariables ( UINT& uConstantVariableCount)
{
	uConstantVariableCount = 0;

	return ALCHEMY_NULL;
}

bool CHardwareSkinEffect::Create(UINT uBonesCount, UINT uVertexInfluence, const CEffectCode::ELEMENTPARAMETER* pSkinElementParameters, UINT uSkinElementParameterCount)
{
	if(!uBonesCount || !uVertexInfluence || !pSkinElementParameters || !uSkinElementParameterCount)
		return false;

	Destroy();

	m_uBonesCount                 = uBonesCount;
	m_uVertexInfluence            = uVertexInfluence;
	m_uSkinElementParameterCount  = uSkinElementParameterCount;

	m_uBlendAttributeCount        = ( (uVertexInfluence + 3) >> 2 ) << 1;

	ALCHEMY_DEBUG_NEW(m_pAttributes, CEffectCode::ELEMENTPARAMETER[m_uBlendAttributeCount]);

	CEffectCode::LPELEMENTPARAMETER pAttribute = m_pAttributes;

	UINT uCurrentVertexInflucence = uVertexInfluence & 3;

	if(uCurrentVertexInflucence)
	{
		CVertexDeclaration::ELEMENTTYPE ElementType = static_cast<CVertexDeclaration::ELEMENTTYPE>( uCurrentVertexInflucence * sizeof(FLOAT) );

		pAttribute->Type   = ElementType;
		pAttribute->Usage  = CVertexDeclaration::BLENDINDICES;
		pAttribute->uIndex = 0;

		++ pAttribute;

		pAttribute->Type   = ElementType;
		pAttribute->Usage  = CVertexDeclaration::BLENDWEIGHT;
		pAttribute->uIndex = 0;

		++ pAttribute;
	}

	UINT i, uBlendCount = uVertexInfluence >> 2;
	for(i = 0; i < uBlendCount; ++ i)
	{
		pAttribute->Type   = CVertexDeclaration::FLOAT4;
		pAttribute->Usage  = CVertexDeclaration::BLENDINDICES;
		pAttribute->uIndex = i;

		++ pAttribute;

		pAttribute->Type   = CVertexDeclaration::FLOAT4;
		pAttribute->Usage  = CVertexDeclaration::BLENDWEIGHT;
		pAttribute->uIndex = i;

		++ pAttribute;
	}

	ALCHEMY_DEBUG_NEW(m_pAttributeVariables, CEffectCode::ATTRIBUTEVARIABLEDECLARATION[uSkinElementParameterCount]);

	const CEffectCode::ELEMENTPARAMETER* pSkinElementParameter;

	CEffectCode::LPATTRIBUTEVARIABLEDECLARATION pVariable;

	for(i = 0; i < uSkinElementParameterCount; ++ i)
	{
		pSkinElementParameter = &pSkinElementParameters[i];

		pVariable               = &m_pAttributeVariables[i];
		pVariable->ShaderType   = CEffectInterface::VERTEX_SHADER;
		pVariable->ElementType  = pSkinElementParameter->Type;
		pVariable->ElementUsage = pSkinElementParameter->Usage;
		pVariable->uIndex       = pSkinElementParameter->uIndex;
		pVariable->bIsReadOnly  = false;
		pVariable->bIsForce     = true;
	}

	CEffectCode::LPGLOBALVARIABLE pConstant;

	pConstant = &m_Constants[CONSTANT_BONE_MATRIX_ARRAY];
	pConstant->ShaderType = CEffectInterface::VERTEX_SHADER;
	pConstant->GlobalType = CEffectCode::ARRAY_VARIABLE;
	pConstant->uDetail    = CRenderMethodInterface::BONE_MATRIX_ARRAY;
	pConstant->uCount     = uBonesCount;
	pConstant->uIndex     = 0;
	pConstant->pValue     = ALCHEMY_NULL;

	pConstant = &m_Constants[CONSTANT_ONE];
	pConstant->ShaderType = CEffectInterface::VERTEX_SHADER;
	pConstant->GlobalType = CEffectCode::CONSTANT_VARIABLE;
	pConstant->uDetail    = CRenderMethod::CONSTANT_ONE;
	pConstant->uCount     = 1;
	pConstant->uIndex     = 0;
	pConstant->pValue     = ALCHEMY_NULL;

	return true;
}

bool CHardwareSkinEffect::ShaderDefineHandler(
	UINT uOutputColor, 
	const UINT* puSamplers, 
	const UINT* puAttributes, 
	const UINT* puConstants, 
	const CEffectCode::VARYING* pVarying, 
	const UINT* puTemporary,
	const CEffectCode::SHADERVARIABLE* pAttributeVariables,
	const CEffectCode::SHADERVARIABLE* pConstantVariables,
	CEffectCode::CInstructionSetInterface& VertexShaderDefiner,
	CEffectCode::CInstructionSetInterface& FragmentShaderDefiner,
	CEffectCode::CPassDefiner::CEffectCodeUtils& EffectCodeUtils)
{
	UINT uMaxVertexInfluence = m_uVertexInfluence - 1,
		uCurrentVertexInflucence = m_uVertexInfluence & 3,
		uLastWeight = puTemporary[TEMPORARY_LAST_WEIGHT], 
		uTemp = puTemporary[TEMPORARY_VARIABLE], 
		uBoneMatrixArray = puConstants[CONSTANT_BONE_MATRIX_ARRAY], 
		x = 0,
		uIndexArray,
		uWeightArray,
		uBlendIndex,
		i, 
		j;

	UINT uAddress[] = {0, 1, 2};

	const UINT* puAttribute = puAttributes;

	const CEffectCode::SHADERVARIABLE* pAttributeVariable;

	for(i = 0; i < uMaxVertexInfluence; ++ i)
	{
		if( i > uCurrentVertexInflucence ? !( (i - uCurrentVertexInflucence) & 3 ) : (!i || i == uCurrentVertexInflucence) )
		{
			uBlendIndex = 0;

			uIndexArray  = *(puAttribute ++);
			uWeightArray = *(puAttribute ++);
		}

		if(i)
			VertexShaderDefiner.AppendInstruction(CCompilerInterface::ADD, uLastWeight, &x, 1, uLastWeight , &x, 1, uWeightArray, &uBlendIndex, 1);
		else
			VertexShaderDefiner.AppendInstruction(CCompilerInterface::MOV, uLastWeight, &x, 1, uWeightArray, &i   , 1, 0           , ALCHEMY_NULL, 0);

		for(j = 0; j < m_uSkinElementParameterCount; ++ j)
		{
			pAttributeVariable  = &pAttributeVariables[j];

			if(pAttributeVariable->uHandle)
			{
				VertexShaderDefiner.AppendInstruction(m_pAttributeVariables[j].ElementUsage == CVertexDeclaration::POSITION ? CCompilerInterface::M34 : CCompilerInterface::M33, uTemp, 0, 0, uAddress, 3, pAttributeVariable->uSource, 0, 0, ALCHEMY_NULL, 0, uBoneMatrixArray, uIndexArray, uBlendIndex, ALCHEMY_NULL, 0);
				VertexShaderDefiner.AppendInstruction(CCompilerInterface::MUL, uTemp, uAddress, 3, uTemp, uAddress, 3, uWeightArray, &uBlendIndex, 1);

				if(i)
					VertexShaderDefiner.AppendInstruction(CCompilerInterface::ADD, pAttributeVariable->uHandle, pAttributeVariable->uHandle, uTemp);
				else
					VertexShaderDefiner.AppendInstruction(CCompilerInterface::MOV, pAttributeVariable->uHandle, uTemp, 0);
			}
		}

		++ uBlendIndex;
	}

	if( i > uCurrentVertexInflucence ? !( (i - uCurrentVertexInflucence) & 3 ) : (!i || i == uCurrentVertexInflucence) )
	{
		uBlendIndex = 0;

		uIndexArray  = *(puAttribute ++);
		uWeightArray = *(puAttribute ++);
	}

	if(uMaxVertexInfluence)
		VertexShaderDefiner.AppendInstruction(CCompilerInterface::SUB, uLastWeight, puConstants[CONSTANT_ONE], uLastWeight);
	else
		VertexShaderDefiner.AppendInstruction(CCompilerInterface::MOV, uLastWeight, &x                       , 1          , uWeightArray, &uBlendIndex, 1, 0, ALCHEMY_NULL, 0);

	UINT w = 3;
	for(j = 0; j < m_uSkinElementParameterCount; ++ j)
	{
		pAttributeVariable  = &pAttributeVariables[j];

		if(pAttributeVariable->uHandle)
		{
			if(m_pAttributeVariables[j].ElementUsage == CVertexDeclaration::POSITION)
			{
				VertexShaderDefiner.AppendInstruction(CCompilerInterface::M34, uTemp, 0, 0, uAddress, 3, pAttributeVariable->uSource, 0, 0, ALCHEMY_NULL, 0, uBoneMatrixArray, uIndexArray, uBlendIndex, ALCHEMY_NULL, 0);
				VertexShaderDefiner.AppendInstruction(CCompilerInterface::MUL, uTemp, uTemp, uLastWeight);

				if(i)
					VertexShaderDefiner.AppendInstruction(CCompilerInterface::ADD, pAttributeVariable->uHandle, pAttributeVariable->uHandle, uTemp);
				else
					VertexShaderDefiner.AppendInstruction(CCompilerInterface::MOV, pAttributeVariable->uHandle, uTemp, 0);

				VertexShaderDefiner.AppendInstruction(CCompilerInterface::MOV, pAttributeVariable->uHandle, &w, 1, pAttributeVariable->uSource, &w, 1, 0, ALCHEMY_NULL, 0);
			}
			else
			{
				VertexShaderDefiner.AppendInstruction(CCompilerInterface::M33, uTemp, 0, 0, uAddress, 3, pAttributeVariable->uSource, 0, 0, ALCHEMY_NULL, 0, uBoneMatrixArray, uIndexArray, uBlendIndex, ALCHEMY_NULL, 0);
				VertexShaderDefiner.AppendInstruction(CCompilerInterface::MUL, uTemp, uTemp, uLastWeight);

				if(i)
					VertexShaderDefiner.AppendInstruction(CCompilerInterface::ADD, pAttributeVariable->uHandle, pAttributeVariable->uHandle, uTemp);
				else
					VertexShaderDefiner.AppendInstruction(CCompilerInterface::MOV, pAttributeVariable->uHandle, uTemp, 0);

				VertexShaderDefiner.AppendInstruction(CCompilerInterface::NRM, pAttributeVariable->uHandle, pAttributeVariable->uHandle, 0);
			}
		}
	}

	return true;
}

void CHardwareSkinEffect::Destroy()
{
	ALCHEMY_DEBUG_DELETE_ARRAY(m_pAttributes);
	ALCHEMY_DEBUG_DELETE_ARRAY(m_pAttributeVariables);

	m_uBonesCount = m_uVertexInfluence = m_uSkinElementParameterCount = 0;
}