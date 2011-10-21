#include "AlchemyWaterNormalMapEffect.h"
#include "AlchemyRenderMethod.h"

using namespace alchemy;

const CEffectCode::TEMPORARYDECLARATION         CWaterNormalMapEffect::sm_TEMPORARY          [CWaterNormalMapEffect::TOTAL_TEMPORARY         ] = 
{ 
	{CEffectInterface::VERTEX_SHADER  , CEffectCode::VECTOR3_VARIABLE, 1},
	{CEffectInterface::FRAGMENT_SHADER, CEffectCode::VECTOR3_VARIABLE, 1}
};

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION CWaterNormalMapEffect::sm_ATTRIBUTE_VARIABLES[CWaterNormalMapEffect::TOTAL_ATTRIBUTE_VARIABLES] = 
{ 
	{CEffectInterface::VERTEX_SHADER  , CVertexDeclaration::FLOAT3, CVertexDeclaration::BINORMAL, 0, true , false},
	{CEffectInterface::FRAGMENT_SHADER, CVertexDeclaration::FLOAT3, CVertexDeclaration::NORMAL  , 0, false, false}
};

const CEffectCode::CONSTANTVARIABLEDECLARATION  CWaterNormalMapEffect::sm_CONSTANT_VARIABLES [CWaterNormalMapEffect::TOTAL_CONSTANT_VARIABLES ] = 
{ 
	{CEffectInterface::VERTEX_SHADER  , CEffectCode::CAMERA_VARIABLE, CRenderMethod::CAMERA_LOCAL_DIRECTION       , 1, 0, ALCHEMY_NULL, false , true},
	{CEffectInterface::VERTEX_SHADER  , CEffectCode::CAMERA_VARIABLE, CRenderMethod::CAMERA_LOCAL_DIRECTION_NEGATE, 1, 0, ALCHEMY_NULL, false , true},
	{CEffectInterface::FRAGMENT_SHADER, CEffectCode::CAMERA_VARIABLE, CRenderMethod::CAMERA_LOCAL_DIRECTION       , 1, 0, ALCHEMY_NULL, false , false},
	{CEffectInterface::FRAGMENT_SHADER, CEffectCode::CAMERA_VARIABLE, CRenderMethod::CAMERA_LOCAL_DIRECTION_NEGATE, 1, 0, ALCHEMY_NULL, false , false}
};

CWaterNormalMapEffect::CWaterNormalMapEffect(FLOAT2& Translation0, FLOAT2& Translation1, FLOAT2& Translation2, FLOAT2& Translation3, UINT uNormalMapIndex)
{
	CEffectCode::LPGLOBALVARIABLE pConstant;
	pConstant = &m_Constants[CONSTANT_TRANSLATION0];
	pConstant->ShaderType = CEffectInterface::VERTEX_SHADER;
	pConstant->GlobalType = CEffectCode::INPUT_VARIABLE;
	pConstant->uDetail    = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR2_VARIABLE, 0, 0, false);
	pConstant->uCount     = 1;
	pConstant->uIndex     = 0;
	pConstant->pValue     = &Translation0;

	pConstant = &m_Constants[CONSTANT_TRANSLATION1];
	pConstant->ShaderType = CEffectInterface::VERTEX_SHADER;
	pConstant->GlobalType = CEffectCode::INPUT_VARIABLE;
	pConstant->uDetail    = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR2_VARIABLE, 0, 0, false);
	pConstant->uCount     = 1;
	pConstant->uIndex     = 0;
	pConstant->pValue     = &Translation1;

	pConstant = &m_Constants[CONSTANT_TRANSLATION2];
	pConstant->ShaderType = CEffectInterface::VERTEX_SHADER;
	pConstant->GlobalType = CEffectCode::INPUT_VARIABLE;
	pConstant->uDetail    = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR2_VARIABLE, 0, 0, false);
	pConstant->uCount     = 1;
	pConstant->uIndex     = 0;
	pConstant->pValue     = &Translation2;

	pConstant = &m_Constants[CONSTANT_TRANSLATION3];
	pConstant->ShaderType = CEffectInterface::VERTEX_SHADER;
	pConstant->GlobalType = CEffectCode::INPUT_VARIABLE;
	pConstant->uDetail    = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR2_VARIABLE, 0, 0, false);
	pConstant->uCount     = 1;
	pConstant->uIndex     = 0;
	pConstant->pValue     = &Translation3;

	pConstant = &m_Constants[CONSTANT_TWO];
	pConstant->ShaderType = CEffectInterface::FRAGMENT_SHADER;
	pConstant->GlobalType = CEffectCode::CONSTANT_VARIABLE;
	pConstant->uDetail    = CRenderMethod::CONSTANT_TWO;
	pConstant->uCount     = 1;
	pConstant->uIndex     = 0;
	pConstant->pValue     = ALCHEMY_NULL;

	CEffectCode::LPELEMENTPARAMETER pElementParameter;
	pElementParameter         = &m_Attributes[ATTRIBUTE_NORAML];
	pElementParameter->Type   = CVertexDeclaration::FLOAT3;
	pElementParameter->Usage  = CVertexDeclaration::NORMAL;
	pElementParameter->uIndex = 0;

	pElementParameter         = &m_Attributes[ATTRIBUTE_TANGENT];
	pElementParameter->Type   = CVertexDeclaration::FLOAT3;
	pElementParameter->Usage  = CVertexDeclaration::TANGENT;
	pElementParameter->uIndex = 0;

	pElementParameter         = &m_Attributes[ATTRIBUTE_TEXCOORD];
	pElementParameter->Type   = CVertexDeclaration::FLOAT2;
	pElementParameter->Usage  = CVertexDeclaration::TEXCOORD;
	pElementParameter->uIndex = 0;

	pElementParameter         = &m_Varying[VARYING_TEXCOORD0];
	pElementParameter->Type   = CVertexDeclaration::FLOAT2;
	pElementParameter->Usage  = CVertexDeclaration::TEXCOORD;
	pElementParameter->uIndex = 0;

	pElementParameter         = &m_Varying[VARYING_TEXCOORD1];
	pElementParameter->Type   = CVertexDeclaration::FLOAT2;
	pElementParameter->Usage  = CVertexDeclaration::TEXCOORD;
	pElementParameter->uIndex = 1;

	pElementParameter         = &m_Varying[VARYING_TEXCOORD2];
	pElementParameter->Type   = CVertexDeclaration::FLOAT2;
	pElementParameter->Usage  = CVertexDeclaration::TEXCOORD;
	pElementParameter->uIndex = 2;

	pElementParameter         = &m_Varying[VARYING_TEXCOORD3];
	pElementParameter->Type   = CVertexDeclaration::FLOAT2;
	pElementParameter->Usage  = CVertexDeclaration::TEXCOORD;
	pElementParameter->uIndex = 3;

	LPSAMPLER pNormal = &m_Samplers[SAMPLER_NORMAL];
	pNormal->Declaration.Type      = CEffectCode::TEXTURE2D;
	pNormal->Declaration.AddressU  = CEffectCode::WRAP;
	pNormal->Declaration.AddressV  = CEffectCode::WRAP;
	pNormal->Declaration.AddressW  = CEffectCode::WRAP;
	pNormal->Declaration.MinFilter = CEffectCode::LINEAR;
	pNormal->Declaration.MagFilter = CEffectCode::LINEAR;
	pNormal->Declaration.MipFilter = CEffectCode::LINEAR;
	pNormal->uIndex                = uNormalMapIndex;
}

CWaterNormalMapEffect::~CWaterNormalMapEffect(void)
{
}


UINT                                            CWaterNormalMapEffect::GetEffectInfoFlag     (                             )
{
	return ALCHEMY_FLAG(CEffectInfoInterface::VERTEX_SHADER_INFO) | ALCHEMY_FLAG(CEffectInfoInterface::FRAGMENT_SHADER_INFO);
}

const CEffectInfoInterface::SAMPLER           * CWaterNormalMapEffect::GetSamplers           (UINT& uSamplerCount          )
{
	uSamplerCount = TOTAL_SAMPLERS;

	return m_Samplers;
}

const CEffectCode::ELEMENTPARAMETER            * CWaterNormalMapEffect::GetAttributes        (UINT& uAttributeCount        )
{
	uAttributeCount = TOTAL_ATTRIBUTES;

	return m_Attributes;
}

const CEffectCode::ELEMENTPARAMETER            * CWaterNormalMapEffect::GetVarying           (UINT& uVaryingCount          )
{
	uVaryingCount = TOTAL_VARYING;

	return m_Varying;
}

const CEffectCode::GLOBALVARIABLE              * CWaterNormalMapEffect::GetConstants         (UINT& uConstantCount         )
{
	uConstantCount = TOTAL_CONSTANTS;

	return m_Constants;
}

const CEffectCode::TEMPORARYDECLARATION        * CWaterNormalMapEffect::GetTemporary         (UINT& uTemporaryCount        )
{
	uTemporaryCount = TOTAL_TEMPORARY;

	return sm_TEMPORARY;
}

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CWaterNormalMapEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
{
	uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLES;

	return sm_ATTRIBUTE_VARIABLES;
}

const CEffectCode::CONSTANTVARIABLEDECLARATION * CWaterNormalMapEffect::GetConstantVariables  (UINT& uConstantVariableCount)
{
	uConstantVariableCount = TOTAL_CONSTANT_VARIABLES;

	return sm_CONSTANT_VARIABLES;
}

bool CWaterNormalMapEffect::ShaderDefineHandler(
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
	UINT uTangent = puAttributes[ATTRIBUTE_TANGENT], uBitnormal = puTemporary[TEMPORARY_BITNORMAL], uNormal = puAttributes[ATTRIBUTE_NORAML], uValue;
	UINT uAddress[3] = {0, 1, 2};

	const CEffectCode::SHADERVARIABLE* pBinormal = &pAttributeVariables[ATTRIBUTE_VARIABLE_BINORMAL];

	if(pBinormal->uHandle)
		VertexShaderDefiner.AppendInstruction(CCompilerInterface::MOV, uBitnormal, ALCHEMY_NULL, 0, pBinormal->uHandle, ALCHEMY_NULL, 0, 0, ALCHEMY_NULL, 0);
	else
		VertexShaderDefiner.AppendInstruction(CCompilerInterface::CRS, uBitnormal, puAttributes[ATTRIBUTE_TANGENT], puAttributes[ATTRIBUTE_NORAML]);

	const CEffectCode::SHADERVARIABLE* pVariable;
	UINT uConstantVariableCount = TOTAL_CONSTANT_VARIABLES >> 1, i;
	for(i = 0; i < uConstantVariableCount; ++ i)
	{
		pVariable = &pConstantVariables[i];
		if(pVariable->uHandle)
		{
			uValue = 0;
			VertexShaderDefiner.AppendInstruction(CCompilerInterface::DP3, pVariable->uHandle, &uValue, 1, uTangent  , uAddress, 3, pVariable->uSource, uAddress, 3);
			uValue = 1;
			VertexShaderDefiner.AppendInstruction(CCompilerInterface::DP3, pVariable->uHandle, &uValue, 1, uBitnormal, uAddress, 3, pVariable->uSource, uAddress, 3);
			uValue = 2;
			VertexShaderDefiner.AppendInstruction(CCompilerInterface::DP3, pVariable->uHandle, &uValue, 1, uNormal   , uAddress, 3, pVariable->uSource, uAddress, 3);

			uValue = 3;
			VertexShaderDefiner.AppendInstruction(CCompilerInterface::MOV, pVariable->uHandle, &uValue, 1, pVariable->uSource, &uValue, 1, 0, ALCHEMY_NULL, 0);
		}
	}

	for(i = uConstantVariableCount; i < TOTAL_CONSTANT_VARIABLES; ++ i)
	{
		pVariable = &pConstantVariables[i];
		if(pVariable->uHandle && pVariable->uSource)
			FragmentShaderDefiner.AppendInstruction(CCompilerInterface::NRM, pVariable->uHandle, pVariable->uSource, 0);
	}

	pVariable = &pAttributeVariables[ATTRIBUTE_VARIABLE_NORAML];
	if(pVariable->uHandle)
	{
		const CEffectCode::VARYING* pTexcoord0 = &pVarying[VARYING_TEXCOORD0];
		if(!pTexcoord0->IsUsed)
			VertexShaderDefiner.AppendInstruction(CCompilerInterface::ADD, pTexcoord0->uVertexShaderHandle, ALCHEMY_NULL, 0, puAttributes[ATTRIBUTE_TEXCOORD], ALCHEMY_NULL, 0, puConstants[CONSTANT_TRANSLATION0], ALCHEMY_NULL, 0);

		const CEffectCode::VARYING* pTexcoord1 = &pVarying[VARYING_TEXCOORD1];
		if(!pTexcoord1->IsUsed)
			VertexShaderDefiner.AppendInstruction(CCompilerInterface::ADD, pTexcoord1->uVertexShaderHandle, ALCHEMY_NULL, 0, puAttributes[ATTRIBUTE_TEXCOORD], ALCHEMY_NULL, 0, puConstants[CONSTANT_TRANSLATION1], ALCHEMY_NULL, 0);

		const CEffectCode::VARYING* pTexcoord2 = &pVarying[VARYING_TEXCOORD2];
		if(!pTexcoord2->IsUsed)
			VertexShaderDefiner.AppendInstruction(CCompilerInterface::ADD, pTexcoord2->uVertexShaderHandle, ALCHEMY_NULL, 0, puAttributes[ATTRIBUTE_TEXCOORD], ALCHEMY_NULL, 0, puConstants[CONSTANT_TRANSLATION2], ALCHEMY_NULL, 0);

		const CEffectCode::VARYING* pTexcoord3 = &pVarying[VARYING_TEXCOORD3];
		if(!pTexcoord3->IsUsed)
			VertexShaderDefiner.AppendInstruction(CCompilerInterface::ADD, pTexcoord3->uVertexShaderHandle, ALCHEMY_NULL, 0, puAttributes[ATTRIBUTE_TEXCOORD], ALCHEMY_NULL, 0, puConstants[CONSTANT_TRANSLATION3], ALCHEMY_NULL, 0);

		FragmentShaderDefiner.AppendInstruction(CCompilerInterface::TEX, pVariable->uHandle, pTexcoord0->uFragmentShaderHandle, puSamplers[SAMPLER_NORMAL]);

		UINT uNormal = puTemporary[TEMPORARY_NORMAL];
		FragmentShaderDefiner.AppendInstruction(CCompilerInterface::TEX, uNormal           , pTexcoord1->uFragmentShaderHandle, puSamplers[SAMPLER_NORMAL]);
		FragmentShaderDefiner.AppendInstruction(CCompilerInterface::ADD, pVariable->uHandle, pVariable->uHandle               , uNormal                   );
		
		FragmentShaderDefiner.AppendInstruction(CCompilerInterface::TEX, uNormal           , pTexcoord2->uFragmentShaderHandle, puSamplers[SAMPLER_NORMAL]);
		FragmentShaderDefiner.AppendInstruction(CCompilerInterface::ADD, pVariable->uHandle, pVariable->uHandle               , uNormal                   );

		FragmentShaderDefiner.AppendInstruction(CCompilerInterface::TEX, uNormal           , pTexcoord3->uFragmentShaderHandle, puSamplers[SAMPLER_NORMAL]);
		FragmentShaderDefiner.AppendInstruction(CCompilerInterface::ADD, pVariable->uHandle, pVariable->uHandle               , uNormal                   );

		FragmentShaderDefiner.AppendInstruction(CCompilerInterface::SUB, pVariable->uHandle, pVariable->uHandle               , puConstants[CONSTANT_TWO] );
		FragmentShaderDefiner.AppendInstruction(CCompilerInterface::DIV, pVariable->uHandle, pVariable->uHandle               , puConstants[CONSTANT_TWO] );

		//FragmentShaderDefiner.AppendInstruction(CCompilerInterface::NRM, pVariable->uHandle, pVariable->uHandle               , 0                         );
	}

	return true;
}