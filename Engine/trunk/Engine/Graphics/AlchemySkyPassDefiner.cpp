#include "AlchemySkyPassDefiner.h"
#include "AlchemyRenderMethod.h"

using namespace alchemy;

CEffectCode::ELEMENTDECLARATION             CSkyPassDefiner::CSkyEffect::sm_VARYING          [CSkyPassDefiner::CSkyEffect::TOTAL_VARYING           ] = 
{ 
	{CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITIONT, 0} 
};

CEffectCode::GLOBALVARIABLE               CSkyPassDefiner::CSkyEffect::sm_CONSTANS         [CSkyPassDefiner::CSkyEffect::TOTAL_CONSTANTS         ] = 
{ 
	{IEffect::VERTEX_SHADER, CEffectCode::CAMERA_VARIABLE, CRenderMethod::CAMERA_WORLD_VIEW_PROJECTION_MATRIX, 1, 0}
};

CEffectCode::ATTRIBUTEVARIABLEDECLARATION CSkyPassDefiner::CSkyEffect::sm_ATTRIBUTE_VARIABLE[CSkyPassDefiner::CSkyEffect::TOTAL_ATTRIBUTE_VARIABLES] = 
{ 
	{IEffect::VERTEX_SHADER, CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION, 0, true, true} 
};

CEffectCode::TEMPORARYDECLARATION			CSkyPassDefiner::CSkyEffect::sm_TEMPRORAYS	    [CSkyPassDefiner::CSkyEffect::TOTAL_TEMPORARY] = { {IEffect::VERTEX_SHADER, CEffectCode::VECTOR4_VARIABLE} };

CSkyPassDefiner::CSkyEffect::CSkyEffect()
:m_pConstant(sm_CONSTANS),
m_uConstantNum(TOTAL_CONSTANTS)
{

}

CSkyPassDefiner::CSkyEffect::~CSkyEffect()
{
	if(m_pConstant != sm_CONSTANS)
	{
		ALCHEMY_DEBUG_DELETE_ARRAY(m_pConstant);
	}
}

void CSkyPassDefiner::CSkyEffect::SetOffset(const FLOAT& ZOffset)
{
	if(m_pConstant != sm_CONSTANS)
	{
		ALCHEMY_DEBUG_DELETE_ARRAY(m_pConstant);
	}

	m_uConstantNum = 2;
	ALCHEMY_DEBUG_NEW(m_pConstant, CEffectCode::GLOBALVARIABLE[m_uConstantNum]);
	CEffectCode::GLOBALVARIABLE* pConstant = &m_pConstant[ CONSTANT_WORLD_VIEW_PROJECTION_MATRIX ];
	pConstant->ShaderType = IEffect::VERTEX_SHADER;
	pConstant->GlobalType = CEffectCode::CAMERA_VARIABLE;
	pConstant->uDetail	  = CRenderMethod::CAMERA_WORLD_VIEW_PROJECTION_MATRIX;
	pConstant->uIndex	  = 0;
	pConstant->uCount	  = 1;
	pConstant->pValue	  = ALCHEMY_NULL;

	++pConstant;
	pConstant->ShaderType = IEffect::VERTEX_SHADER;
	pConstant->GlobalType = CEffectCode::INPUT_VARIABLE;
	pConstant->uDetail	  = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR1_VARIABLE);
	pConstant->uIndex	  = 0;
	pConstant->uCount	  = 1;
	pConstant->pValue	  = reinterpret_cast<const void*>(&ZOffset);
}

UINT                                             CSkyPassDefiner::CSkyEffect::GetEffectInfoFlag    (                             )
{
	return ALCHEMY_FLAG(IEffectInfo::VERTEX_SHADER_OUTPUT);
}

const IEffectInfo::SAMPLER                                  * CSkyPassDefiner::CSkyEffect::GetSamplers          (UINT& uSamplerCount          )
{
	uSamplerCount = TOTAL_SAMPLERS;
	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION            * CSkyPassDefiner::CSkyEffect::GetAttributes        (UINT& uAttributeCount        )
{
	uAttributeCount = TOTAL_ATTRIBUTES;
	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION            * CSkyPassDefiner::CSkyEffect::GetVarying           (UINT& uVaryingCount          )
{
	uVaryingCount = TOTAL_VARYING;
	return sm_VARYING;
}

const CEffectCode::GLOBALVARIABLE              * CSkyPassDefiner::CSkyEffect::GetConstants         (UINT& uConstantCount         )
{
	uConstantCount =  m_uConstantNum;
	return m_pConstant;
}

const CEffectCode::TEMPORARYDECLARATION        * CSkyPassDefiner::CSkyEffect::GetTemporary         (UINT& uTemporaryCount        )
{
	uTemporaryCount = TOTAL_TEMPORARY;
	return sm_TEMPRORAYS;
}

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CSkyPassDefiner::CSkyEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
{
	uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLES;
	return sm_ATTRIBUTE_VARIABLE;
}

const CEffectCode::CONSTANTVARIABLEDECLARATION * CSkyPassDefiner::CSkyEffect::GetConstantVariables (UINT& uConstantVariableCount )
{
	uConstantVariableCount = TOTAL_CONSTANT_VARIABLES;
	return ALCHEMY_NULL;
}

bool CSkyPassDefiner::CSkyEffect::ShaderDefineHandler(
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
	const CEffectCode::SHADERVARIABLE* pVariable = &pAttributeVariables[ATTRIBUTE_VARIABLE_POSITION];

	UINT uSubAddress[] = {0, 1, 2, 3};
	VertexShaderDefiner.AppendInstruction(ICompiler::M44, puTemporary[TEMPORARY_POS], pVariable->uHandle,  puConstants[CONSTANT_WORLD_VIEW_PROJECTION_MATRIX]);

	UINT z = 2,w = 3;
	if(m_uConstantNum == 1)
		VertexShaderDefiner.AppendInstruction(ICompiler::MOV, puTemporary[TEMPORARY_POS], &z, 1, puTemporary[TEMPORARY_POS], &w, 1, 0, ALCHEMY_NULL, 0);
	else
		VertexShaderDefiner.AppendInstruction(ICompiler::ADD, puTemporary[TEMPORARY_POS], &z, 1, puTemporary[TEMPORARY_POS], &w, 1, puConstants[CONSTANT_WORLD_VIEW_PROJECTION_MATRIX+1], uSubAddress, 1);

	VertexShaderDefiner.AppendInstruction(ICompiler::MOV, pPositiont->uVertexShaderHandle, puTemporary[TEMPORARY_POS], 0);

	return true;
}

typedef CSmartClass<CSkyPassDefiner::CSkyEffect>	SKYEFFECT;

CSkyPassDefiner::CSkyPassDefiner( CEffectCode& Code, const FLOAT* pZOffset )
:CPassDefiner(Code)
{
	ALCHEMY_DEBUG_NEW(m_pSkyEffect, SKYEFFECT);
	if(pZOffset)
		m_pSkyEffect->SetOffset(*pZOffset);
	AddInfo(*m_pSkyEffect, 0);
}

CSkyPassDefiner::~CSkyPassDefiner()
{
	ALCHEMY_DEBUG_DELETE(m_pSkyEffect);
}