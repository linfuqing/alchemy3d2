#include "AlchemyDistanceInfoEffect.h"
#include "AlchemyRenderMethod.h"

using namespace alchemy;

const FLOAT3 CDistanceInfoEffect::sm_VIEW_CAMERA_DIR = { 0.0f, 0.0f, 1.0f };
const CEffectCode::ATTRIBUTEVARIABLEDECLARATION CDistanceInfoEffect::sm_ATTRIBUTEVARIABLE[CDistanceInfoEffect::TOTAL_ATTRIBUTE_VARIABLE] = { { IEffect::VERTEX_SHADER, CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION, 0, true, true} };
const CEffectCode::TEMPORARYDECLARATION			CDistanceInfoEffect::sm_TEMPRORAYS		  [CDistanceInfoEffect::TOTAL_TEMPORARY]			 = { { IEffect::FRAGMENT_SHADER, CEffectCode::VECTOR4_VARIABLE } };
const CEffectCode::ELEMENTDECLARATION				CDistanceInfoEffect::sm_VARYING[TOTAL_VARYING] = { {CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION, 0} } ;

CDistanceInfoEffect::CDistanceInfoEffect(FLOAT FocusDistance)
:m_fFocusDistance(FocusDistance)
{
	m_Constants[CONSTANT_WORLD_VIEW_MATRIX].ShaderType = IEffect::VERTEX_SHADER;
	m_Constants[CONSTANT_WORLD_VIEW_MATRIX].GlobalType = CEffectCode::CAMERA_VARIABLE;
	m_Constants[CONSTANT_WORLD_VIEW_MATRIX].uDetail = CRenderMethod::CAMERA_WORLD_VIEW_MATRIX;
	m_Constants[CONSTANT_WORLD_VIEW_MATRIX].uCount = 1;
	m_Constants[CONSTANT_WORLD_VIEW_MATRIX].uIndex = 0;
	m_Constants[CONSTANT_WORLD_VIEW_MATRIX].pValue = ALCHEMY_NULL;

	m_Constants[CONSTANT_VIEW_DIR].ShaderType = IEffect::FRAGMENT_SHADER;
	m_Constants[CONSTANT_VIEW_DIR].GlobalType = CEffectCode::INPUT_VARIABLE;
	m_Constants[CONSTANT_VIEW_DIR].uDetail = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR3_VARIABLE, 1, 3, false);
	m_Constants[CONSTANT_VIEW_DIR].uCount = 1;
	m_Constants[CONSTANT_VIEW_DIR].uIndex = 0;
	m_Constants[CONSTANT_VIEW_DIR].pValue = reinterpret_cast<const VOID*>(&sm_VIEW_CAMERA_DIR);

	m_Constants[CONSTANT_FOUCS_DISTANCE].ShaderType = IEffect::FRAGMENT_SHADER;
	m_Constants[CONSTANT_FOUCS_DISTANCE].GlobalType = CEffectCode::INPUT_VARIABLE;
	m_Constants[CONSTANT_FOUCS_DISTANCE].uDetail = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR1_VARIABLE, 1, 1, false);
	m_Constants[CONSTANT_FOUCS_DISTANCE].uCount = 1;
	m_Constants[CONSTANT_FOUCS_DISTANCE].uIndex = 0;
	m_Constants[CONSTANT_FOUCS_DISTANCE].pValue = reinterpret_cast<const VOID*>(&m_fFocusDistance);
}

CDistanceInfoEffect::~CDistanceInfoEffect()
{

}

UINT CDistanceInfoEffect::GetEffectInfoFlag    ()
{
	return ALCHEMY_FLAG( IEffectInfo::FRAGMENT_SHADER_OUTPUT );
}

const IEffectInfo::SAMPLER* CDistanceInfoEffect::GetSamplers          (UINT& uSamplerCount)
{
	uSamplerCount = TOTAL_SAMPLERS;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION            * CDistanceInfoEffect::GetAttributes        (UINT& uAttributeCount        )
{
	uAttributeCount = TOTAL_ATTRIBUTES;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION            * CDistanceInfoEffect::GetVarying           (UINT& uVaryingCount          )
{
	uVaryingCount = TOTAL_VARYING;

	return sm_VARYING;
}

const CEffectCode::GLOBALVARIABLE              * CDistanceInfoEffect::GetConstants         (UINT& uConstantCount         )
{
	uConstantCount = TOTAL_CONSTANTS;

	return m_Constants;
}

const CEffectCode::TEMPORARYDECLARATION        * CDistanceInfoEffect::GetTemporary         (UINT& uTemporaryCount        )
{
	uTemporaryCount = TOTAL_TEMPORARY;

	return sm_TEMPRORAYS;
}

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CDistanceInfoEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
{
	uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLE;

	return sm_ATTRIBUTEVARIABLE;
}

const CEffectCode::CONSTANTVARIABLEDECLARATION * CDistanceInfoEffect::GetConstantVariables (UINT& uConstantVariableCount )
{
	uConstantVariableCount = TOTAL_CONSTANT_VARIABLE;

	return ALCHEMY_NULL;
}

bool CDistanceInfoEffect::ShaderDefineHandler(
								 UINT uOutputColor, 
								 const UINT* puSamplers, 
								 const UINT* puAttributes, 
								 const UINT* puConstants, 
								 const CEffectCode::VARYING* pVarying, 
								 const UINT* puTemporary,
								 const CEffectCode::SHADERVARIABLE* pAttributeVariable,
								 const CEffectCode::SHADERVARIABLE* pConstantVariable,
								 CEffectCode::IInstructionSet& VertexShaderDefiner,
								 CEffectCode::IInstructionSet& FragmentShaderDefiner,
								 CEffectCode::CPassDefiner::CEffectCodeUtils& EffectCodeUtils)
{
	UINT uSubAddress[] = {0,1,2,3};
	VertexShaderDefiner.AppendInstruction( ICompiler::M44, pVarying[VARYING_VIEW_POSITION].uVertexShaderHandle, pAttributeVariable[ ATTRIBUTE_POSITION ].uHandle, puConstants[CONSTANT_WORLD_VIEW_MATRIX] );

	FragmentShaderDefiner.AppendInstruction(ICompiler::MOV, puTemporary[ TEMPORARY_FOCUS_PLANE ], uSubAddress,  3, puConstants[ CONSTANT_VIEW_DIR ], uSubAddress, 3,  0,ALCHEMY_NULL, 0);
	FragmentShaderDefiner.AppendInstruction(ICompiler::MOV, puTemporary[ TEMPORARY_FOCUS_PLANE ], &uSubAddress[3],  1, puConstants[ CONSTANT_FOUCS_DISTANCE ], uSubAddress, 1,  0,ALCHEMY_NULL, 0);

	FragmentShaderDefiner.AppendInstruction(ICompiler::DP4, puTemporary[ TEMPORARY_FOCUS_PLANE ], uSubAddress, 1,  puTemporary[ TEMPORARY_FOCUS_PLANE ],
								uSubAddress, 4, pVarying[VARYING_VIEW_POSITION].uFragmentShaderHandle, uSubAddress, 4);

	//FragmentShaderDefiner.AppendInstruction(CCompilerInterface::MUL, puTemporary[ TEMPORARY_FOCUS_PLANE ], uSubAddress, 1, puTemporary[ TEMPORARY_FOCUS_PLANE ], uSubAddress, 1, puConstants[CONSTANT_HYPERFOCAL_DISTANCE], uSubAddress,1);
	//FragmentShaderDefiner.AppendInstruction(CCompilerInterface::MUL, puTemporary[ TEMPORARY_FOCUS_PLANE ], uSubAddress, 1, puTemporary[ TEMPORARY_FOCUS_PLANE ], uSubAddress, 1, puTemporary[ TEMPORARY_FOCUS_PLANE ], uSubAddress, 1);

	FragmentShaderDefiner.AppendInstruction(ICompiler::SAT, puTemporary[ TEMPORARY_FOCUS_PLANE ], uSubAddress, 1,  puTemporary[ TEMPORARY_FOCUS_PLANE ], uSubAddress, 1,  0, ALCHEMY_NULL, 0);

	FragmentShaderDefiner.AppendInstruction(ICompiler::MOV, uOutputColor, uSubAddress, 4, puTemporary[TEMPORARY_FOCUS_PLANE ],uSubAddress, 1,  0,ALCHEMY_NULL, 0);

	//FragmentShaderDefiner.AppendInstruction(CCompilerInterface::MIN, uOutputColor, uSubAddress, 4, puTemporary[TEMPORARY_FOCUS_PLANE ],uSubAddress, 1, puConstants[ CONSTANT_MAX_BLURFACTOR ], uSubAddress, 1);

	return true;
}