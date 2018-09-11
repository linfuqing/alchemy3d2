#include "AlchemyWaterNormalMapEffect.h"
#include "AlchemyRenderMethod.h"

using namespace alchemy;

const CEffectCode::TEMPORARYDECLARATION         CWaterNormalMapEffect::sm_TEMPORARY          [CWaterNormalMapEffect::TOTAL_TEMPORARY         ] = 
{ 
	{IEffect::VERTEX_SHADER  , CEffectCode::VECTOR3_VARIABLE, 1},
	{IEffect::VERTEX_SHADER  , CEffectCode::VECTOR3_VARIABLE, 1},
	{IEffect::FRAGMENT_SHADER, CEffectCode::VECTOR3_VARIABLE, 1}
};

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION CWaterNormalMapEffect::sm_ATTRIBUTE_VARIABLES[CWaterNormalMapEffect::TOTAL_ATTRIBUTE_VARIABLES] = 
{ 
	{IEffect::VERTEX_SHADER  , CVertexDeclaration::FLOAT3, CVertexDeclaration::BINORMAL, 0, true , false},
	{IEffect::FRAGMENT_SHADER, CVertexDeclaration::FLOAT3, CVertexDeclaration::NORMAL  , 0, false, false}
};

const CEffectCode::CONSTANTVARIABLEDECLARATION  CWaterNormalMapEffect::sm_CONSTANT_VARIABLES [CWaterNormalMapEffect::TOTAL_CONSTANT_VARIABLES ] = 
{ 
	{IEffect::VERTEX_SHADER  , CEffectCode::CAMERA_VARIABLE, CRenderMethod::CAMERA_LOCAL_DIRECTION       , 1, 0, ALCHEMY_NULL, false , true},
	{IEffect::VERTEX_SHADER  , CEffectCode::CAMERA_VARIABLE, CRenderMethod::CAMERA_LOCAL_DIRECTION_NEGATE, 1, 0, ALCHEMY_NULL, false , true},
	{IEffect::FRAGMENT_SHADER, CEffectCode::CAMERA_VARIABLE, CRenderMethod::CAMERA_LOCAL_DIRECTION       , 1, 0, ALCHEMY_NULL, false , false},
	{IEffect::FRAGMENT_SHADER, CEffectCode::CAMERA_VARIABLE, CRenderMethod::CAMERA_LOCAL_DIRECTION_NEGATE, 1, 0, ALCHEMY_NULL, false , false}
};

CWaterNormalMapEffect::CWaterNormalMapEffect(FLOAT2& Translation0, FLOAT2& Translation1, FLOAT2& Translation2, FLOAT2& Translation3, UINT uNormalMapIndex)
{
	CEffectCode::LPGLOBALVARIABLE pConstant;
	pConstant = &m_Constants[CONSTANT_TRANSLATION0];
	pConstant->ShaderType = IEffect::VERTEX_SHADER;
	pConstant->GlobalType = CEffectCode::INPUT_VARIABLE;
	pConstant->uDetail    = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR2_VARIABLE, 0, 0, false);
	pConstant->uCount     = 1;
	pConstant->uIndex     = 0;
	pConstant->pValue     = &Translation0;

	pConstant = &m_Constants[CONSTANT_TRANSLATION1];
	pConstant->ShaderType = IEffect::VERTEX_SHADER;
	pConstant->GlobalType = CEffectCode::INPUT_VARIABLE;
	pConstant->uDetail    = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR2_VARIABLE, 0, 0, false);
	pConstant->uCount     = 1;
	pConstant->uIndex     = 0;
	pConstant->pValue     = &Translation1;

	pConstant = &m_Constants[CONSTANT_TRANSLATION2];
	pConstant->ShaderType = IEffect::VERTEX_SHADER;
	pConstant->GlobalType = CEffectCode::INPUT_VARIABLE;
	pConstant->uDetail    = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR2_VARIABLE, 0, 0, false);
	pConstant->uCount     = 1;
	pConstant->uIndex     = 0;
	pConstant->pValue     = &Translation2;

	pConstant = &m_Constants[CONSTANT_TRANSLATION3];
	pConstant->ShaderType = IEffect::VERTEX_SHADER;
	pConstant->GlobalType = CEffectCode::INPUT_VARIABLE;
	pConstant->uDetail    = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR2_VARIABLE, 0, 0, false);
	pConstant->uCount     = 1;
	pConstant->uIndex     = 0;
	pConstant->pValue     = &Translation3;

	pConstant = &m_Constants[CONSTANT_VERTEX_TWO];
	pConstant->ShaderType = IEffect::VERTEX_SHADER;
	pConstant->GlobalType = CEffectCode::CONSTANT_VARIABLE;
	pConstant->uDetail    = CRenderMethod::CONSTANT_TWO;
	pConstant->uCount     = 1;
	pConstant->uIndex     = 0;
	pConstant->pValue     = ALCHEMY_NULL;

	pConstant = &m_Constants[CONSTANT_FRAGMENT_TWO];
	pConstant->ShaderType = IEffect::FRAGMENT_SHADER;
	pConstant->GlobalType = CEffectCode::CONSTANT_VARIABLE;
	pConstant->uDetail    = CRenderMethod::CONSTANT_TWO;
	pConstant->uCount     = 1;
	pConstant->uIndex     = 0;
	pConstant->pValue     = ALCHEMY_NULL;

	CEffectCode::LPELEMENTDECLARATION pElement;
	pElement         = &m_Attributes[ATTRIBUTE_NORAML];
	pElement->Type   = CVertexDeclaration::FLOAT3;
	pElement->Usage  = CVertexDeclaration::NORMAL;
	pElement->uIndex = 0;

	pElement         = &m_Attributes[ATTRIBUTE_TANGENT];
	pElement->Type   = CVertexDeclaration::FLOAT3;
	pElement->Usage  = CVertexDeclaration::TANGENT;
	pElement->uIndex = 0;

	pElement         = &m_Attributes[ATTRIBUTE_TEXCOORD];
	pElement->Type   = CVertexDeclaration::FLOAT2;
	pElement->Usage  = CVertexDeclaration::TEXCOORD;
	pElement->uIndex = 0;

	pElement         = &m_Varying[VARYING_TEXCOORD0];
	pElement->Type   = CVertexDeclaration::FLOAT2;
	pElement->Usage  = CVertexDeclaration::TEXCOORD;
	pElement->uIndex = 0;

	pElement         = &m_Varying[VARYING_TEXCOORD1];
	pElement->Type   = CVertexDeclaration::FLOAT2;
	pElement->Usage  = CVertexDeclaration::TEXCOORD;
	pElement->uIndex = 1;

	pElement         = &m_Varying[VARYING_TEXCOORD2];
	pElement->Type   = CVertexDeclaration::FLOAT2;
	pElement->Usage  = CVertexDeclaration::TEXCOORD;
	pElement->uIndex = 2;

	pElement         = &m_Varying[VARYING_TEXCOORD3];
	pElement->Type   = CVertexDeclaration::FLOAT2;
	pElement->Usage  = CVertexDeclaration::TEXCOORD;
	pElement->uIndex = 3;

	LPSAMPLER pNormal = &m_Samplers[SAMPLER_NORMAL];
	pNormal->Type                  = IEffect::FRAGMENT_SHADER;
	pNormal->Declaration.Type      = IEffect::TEXTURE2D;
	pNormal->Declaration.AddressU  = IEffect::WRAP;
	pNormal->Declaration.AddressV  = IEffect::WRAP;
	pNormal->Declaration.AddressW  = IEffect::WRAP;
	pNormal->Declaration.MinFilter = IEffect::LINEAR;
	pNormal->Declaration.MagFilter = IEffect::LINEAR;
	pNormal->Declaration.MipFilter = IEffect::LINEAR;
	pNormal->uIndex                = uNormalMapIndex;
}

CWaterNormalMapEffect::~CWaterNormalMapEffect(void)
{
}


UINT                                            CWaterNormalMapEffect::GetEffectInfoFlag     (                             )
{
	return ALCHEMY_FLAG(IEffectInfo::VERTEX_SHADER_INFO) | ALCHEMY_FLAG(IEffectInfo::FRAGMENT_SHADER_INFO);
}

const IEffectInfo::SAMPLER           * CWaterNormalMapEffect::GetSamplers           (UINT& uSamplerCount          )
{
	uSamplerCount = TOTAL_SAMPLERS;

	return m_Samplers;
}

const CEffectCode::ELEMENTDECLARATION            * CWaterNormalMapEffect::GetAttributes        (UINT& uAttributeCount        )
{
	uAttributeCount = TOTAL_ATTRIBUTES;

	return m_Attributes;
}

const CEffectCode::ELEMENTDECLARATION            * CWaterNormalMapEffect::GetVarying           (UINT& uVaryingCount          )
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
	CEffectCode::IInstructionSet& VertexShaderDefiner,
	CEffectCode::IInstructionSet& FragmentShaderDefiner,
	CEffectCode::CPassDefiner::CEffectCodeUtils& EffectCodeUtils)
{
	UINT uTangent = puAttributes[ATTRIBUTE_TANGENT], uTemp = puTemporary[TEMPORARY_VERTEX], uNormal = puAttributes[ATTRIBUTE_NORAML], uSourceTemp = puTemporary[TEMPORARY_SOURCE], uValue, uSource;
	UINT uAddress[3] = {0, 1, 2};

	const CEffectCode::SHADERVARIABLE* pBinormal = &pAttributeVariables[ATTRIBUTE_VARIABLE_BINORMAL];

	if(pBinormal->uHandle)
		VertexShaderDefiner.AppendInstruction(ICompiler::MOV, uTemp, ALCHEMY_NULL, 0, pBinormal->uHandle, ALCHEMY_NULL, 0, 0, ALCHEMY_NULL, 0);
	else
		VertexShaderDefiner.AppendInstruction(ICompiler::CRS, uTemp, puAttributes[ATTRIBUTE_TANGENT], puAttributes[ATTRIBUTE_NORAML]);

	const CEffectCode::SHADERVARIABLE* pVariable;
	UINT uConstantVariableCount = TOTAL_CONSTANT_VARIABLES >> 1, i;
	for(i = 0; i < uConstantVariableCount; ++ i)
	{
		pVariable = &pConstantVariables[i];
		if(pVariable->uHandle)
		{
			uSource = pVariable->uSource;
			if(pVariable->bIsUsed)
			{
				uSource = uSourceTemp;
				VertexShaderDefiner.AppendInstruction(ICompiler::MOV, uSource, uAddress, 3, pVariable->uHandle, uAddress, 3, 0, ALCHEMY_NULL, 0);
			}

			uValue = 0;
			VertexShaderDefiner.AppendInstruction(ICompiler::DP3, pVariable->uHandle, &uValue, 1, uTangent  , uAddress, 3, uSource, uAddress, 3);
			uValue = 1;
			VertexShaderDefiner.AppendInstruction(ICompiler::DP3, pVariable->uHandle, &uValue, 1, uTemp     , uAddress, 3, uSource, uAddress, 3);
			uValue = 2;
			VertexShaderDefiner.AppendInstruction(ICompiler::DP3, pVariable->uHandle, &uValue, 1, uNormal   , uAddress, 3, uSource, uAddress, 3);
		}
	}

	for(i = uConstantVariableCount; i < TOTAL_CONSTANT_VARIABLES; ++ i)
	{
		pVariable = &pConstantVariables[i];
		if(pVariable->uHandle && pVariable->uSource)
			FragmentShaderDefiner.AppendInstruction(ICompiler::NRM, pVariable->uHandle, pVariable->uSource, 0);
	}

	pVariable = &pAttributeVariables[ATTRIBUTE_VARIABLE_NORAML];
	if(pVariable->uHandle)
	{
		UINT uTexcoord = puAttributes[ATTRIBUTE_TEXCOORD], uTwo = puConstants[CONSTANT_VERTEX_TWO];
		const CEffectCode::VARYING* pTexcoord0 = &pVarying[VARYING_TEXCOORD0];
		if(!pTexcoord0->IsUsed)
			VertexShaderDefiner.AppendInstruction(ICompiler::ADD, pTexcoord0->uVertexShaderHandle, ALCHEMY_NULL, 0, uTexcoord, ALCHEMY_NULL, 0, puConstants[CONSTANT_TRANSLATION0], uAddress, 2);

		VertexShaderDefiner.AppendInstruction(ICompiler::MUL, uTemp, uAddress, 2, uTexcoord, uAddress, 2, uTwo, uAddress, 1);

		const CEffectCode::VARYING* pTexcoord1 = &pVarying[VARYING_TEXCOORD1];
		if(!pTexcoord1->IsUsed)
			VertexShaderDefiner.AppendInstruction(ICompiler::ADD, pTexcoord1->uVertexShaderHandle, ALCHEMY_NULL, 0, uTemp, uAddress, 2, puConstants[CONSTANT_TRANSLATION1], uAddress, 2);

		VertexShaderDefiner.AppendInstruction(ICompiler::MUL, uTemp, uAddress, 2, uTemp, uAddress, 2, uTwo, uAddress, 1);

		const CEffectCode::VARYING* pTexcoord2 = &pVarying[VARYING_TEXCOORD2];
		if(!pTexcoord2->IsUsed)
			VertexShaderDefiner.AppendInstruction(ICompiler::ADD, pTexcoord2->uVertexShaderHandle, ALCHEMY_NULL, 0, uTemp, uAddress, 2, puConstants[CONSTANT_TRANSLATION2], uAddress, 2);

		VertexShaderDefiner.AppendInstruction(ICompiler::MUL, uTemp, uAddress, 2, uTemp, uAddress, 2, uTwo, uAddress, 1);

		const CEffectCode::VARYING* pTexcoord3 = &pVarying[VARYING_TEXCOORD3];
		if(!pTexcoord3->IsUsed)
			VertexShaderDefiner.AppendInstruction(ICompiler::ADD, pTexcoord3->uVertexShaderHandle, ALCHEMY_NULL, 0, uTemp, uAddress, 2, puConstants[CONSTANT_TRANSLATION3], uAddress, 2);

		UINT uSampler = puSamplers[SAMPLER_NORMAL];
		FragmentShaderDefiner.AppendInstruction(ICompiler::TEX, pVariable->uHandle, pTexcoord0->uFragmentShaderHandle, uSampler);

		uNormal = puTemporary[TEMPORARY_FRAGMENT];
		FragmentShaderDefiner.AppendInstruction(ICompiler::TEX, uNormal           , pTexcoord1->uFragmentShaderHandle, uSampler);
		FragmentShaderDefiner.AppendInstruction(ICompiler::ADD, pVariable->uHandle, pVariable->uHandle               , uNormal );
		
		FragmentShaderDefiner.AppendInstruction(ICompiler::TEX, uNormal           , pTexcoord2->uFragmentShaderHandle, uSampler);
		FragmentShaderDefiner.AppendInstruction(ICompiler::ADD, pVariable->uHandle, pVariable->uHandle               , uNormal );

		FragmentShaderDefiner.AppendInstruction(ICompiler::TEX, uNormal           , pTexcoord3->uFragmentShaderHandle, uSampler);
		FragmentShaderDefiner.AppendInstruction(ICompiler::ADD, pVariable->uHandle, pVariable->uHandle               , uNormal );

		uTwo = puConstants[CONSTANT_FRAGMENT_TWO];
		FragmentShaderDefiner.AppendInstruction(ICompiler::SUB, pVariable->uHandle, pVariable->uHandle               , uTwo    );
		FragmentShaderDefiner.AppendInstruction(ICompiler::DIV, pVariable->uHandle, pVariable->uHandle               , uTwo    );

		//FragmentShaderDefiner.AppendInstruction(CCompilerInterface::NRM, pVariable->uHandle, pVariable->uHandle               , 0                         );
	}

	return true;
}