#include "AlchemyPositionDepthRGBAMapOutputEffect.h"
#include "AlchemyRenderMethod.h"

using namespace alchemy;

const CEffectCode::ELEMENTDECLARATION            CPositionDepthRGBAMapOutputEffect::sm_VARYING          [CPositionDepthRGBAMapOutputEffect::TOTAL_VARYING          ] = 
{ 
	{CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION, 0}
};

const CEffectCode::GLOBALVARIABLE               CPositionDepthRGBAMapOutputEffect::sm_CONSTANS          [CPositionDepthRGBAMapOutputEffect::TOTAL_CONSTANTS          ] = 
{ 
	{IEffect::VERTEX_SHADER  , CEffectCode::CAMERA_VARIABLE  , CRenderMethod::CAMERA_INVERSE_PROJECTION_MATRIX  , 1, 0, ALCHEMY_NULL}, 
	{IEffect::FRAGMENT_SHADER, CEffectCode::CONSTANT_VARIABLE, CRenderMethod::CONSTANT_ZERO              , 1, 0, ALCHEMY_NULL}, 
	{IEffect::FRAGMENT_SHADER, CEffectCode::CONSTANT_VARIABLE, CRenderMethod::CONSTANT_RGBA_TO_FLOAT_CODE, 1, 0, ALCHEMY_NULL} 
};

const CEffectCode::TEMPORARYDECLARATION         CPositionDepthRGBAMapOutputEffect::sm_TEMPORARY         [CPositionDepthRGBAMapOutputEffect::TOTAL_TEMPORARY          ] = 
{ 
	{IEffect::FRAGMENT_SHADER, CEffectCode::VECTOR4_VARIABLE, 1} 
};

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION CPositionDepthRGBAMapOutputEffect::sm_ATTRIBUTE_VARIABLE[CPositionDepthRGBAMapOutputEffect::TOTAL_ATTRIBUTE_VARIABLES] = 
{ 
	{IEffect::VERTEX_SHADER  , CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION, 0, true , true },
	{IEffect::FRAGMENT_SHADER, CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION, 0, false, false},
	{IEffect::FRAGMENT_SHADER, CVertexDeclaration::FLOAT2, CVertexDeclaration::TEXCOORD, 0, true , true }
};

const CEffectCode::CONSTANTVARIABLEDECLARATION CPositionDepthRGBAMapOutputEffect::sm_CONSTANT_VARIABLE[CPositionDepthRGBAMapOutputEffect::TOTAL_CONSTANT_VARIABLES] = 
{
	{IEffect::FRAGMENT_SHADER, CEffectCode::MATERIAL_VARIABLE, CRenderMethod::MATERIAL_AMBIENT, 1, 0, ALCHEMY_NULL, false, true}
};

CPositionDepthRGBAMapOutputEffect::CPositionDepthRGBAMapOutputEffect(UINT uSamplerIndex, IEffect::SAMPLERFILTER SamplerFilter)
{
	LPSAMPLER pSampler = &m_Samplers[SAMPLER_TEXTURE];
	pSampler->Type                  = IEffect::FRAGMENT_SHADER;
	pSampler->Declaration.Type      = IEffect::TEXTURE2D;
	pSampler->Declaration.MinFilter = SamplerFilter;
	pSampler->Declaration.MagFilter = SamplerFilter;
	pSampler->Declaration.MipFilter = IEffect::NONE;
	pSampler->Declaration.AddressU  = IEffect::CLAMP;
	pSampler->Declaration.AddressV  = IEffect::CLAMP;
	pSampler->Declaration.AddressW  = IEffect::CLAMP;
	pSampler->uIndex                = uSamplerIndex;
}

CPositionDepthRGBAMapOutputEffect::~CPositionDepthRGBAMapOutputEffect(void)
{
}

UINT                                            CPositionDepthRGBAMapOutputEffect::GetEffectInfoFlag     (                             )
{
	return ALCHEMY_FLAG(IEffectInfo::FRAGMENT_SHADER_INFO);
}

const IEffectInfo::SAMPLER           * CPositionDepthRGBAMapOutputEffect::GetSamplers           (UINT& uSamplerCount          )
{
	uSamplerCount = TOTAL_SAMPLERS;

	return m_Samplers;
}

const CEffectCode::ELEMENTDECLARATION            * CPositionDepthRGBAMapOutputEffect::GetAttributes        (UINT& uAttributeCount        )
{
	uAttributeCount = TOTAL_ATTRIBUTES;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION            * CPositionDepthRGBAMapOutputEffect::GetVarying           (UINT& uVaryingCount          )
{
	uVaryingCount = TOTAL_VARYING;

	return sm_VARYING;
}

const CEffectCode::GLOBALVARIABLE              * CPositionDepthRGBAMapOutputEffect::GetConstants         (UINT& uConstantCount         )
{
	uConstantCount = TOTAL_CONSTANTS;

	return sm_CONSTANS;
}

const CEffectCode::TEMPORARYDECLARATION        * CPositionDepthRGBAMapOutputEffect::GetTemporary         (UINT& uTemporaryCount        )
{
	uTemporaryCount = TOTAL_TEMPORARY;

	return sm_TEMPORARY;
}

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CPositionDepthRGBAMapOutputEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
{
	uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLES;

	return sm_ATTRIBUTE_VARIABLE;
}

const CEffectCode::CONSTANTVARIABLEDECLARATION * CPositionDepthRGBAMapOutputEffect::GetConstantVariables  (UINT& uConstantVariableCount)
{
	uConstantVariableCount = TOTAL_CONSTANT_VARIABLES;

	return sm_CONSTANT_VARIABLE;
}

bool CPositionDepthRGBAMapOutputEffect::ShaderDefineHandler(
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
	const CEffectCode::SHADERVARIABLE* pMaterialAmbient = &pConstantVariables[CONSTANT_VARIABLE_MATERIAL_AMBIENT];
	const CEffectCode::SHADERVARIABLE* pDestinationPosition = &pAttributeVariables[ATTRIBUTE_VARIABLE_DESTINATION_POSITION];
	if(pMaterialAmbient->uHandle || pDestinationPosition->uHandle)
	{
		UINT uTemp = puTemporary[TEMPORARY_TEMP], uRGBAToFloatCode = puConstants[CONSTANT_RGBA_TO_FLOAT_CODE];
		FragmentShaderDefiner.AppendInstruction(ICompiler::TEX, uTemp, pAttributeVariables[ATTRIBUTE_VARIABLE_TEXCOORD].uHandle, puSamplers[SAMPLER_TEXTURE]);
		UINT uAddress[] = {0, 1, 2, 3};
		FragmentShaderDefiner.AppendInstruction(ICompiler::DP4, uTemp, uAddress, 1, uTemp, ALCHEMY_NULL, 0, uRGBAToFloatCode, ALCHEMY_NULL, 0);

		if(pMaterialAmbient->uHandle)
		{
			FragmentShaderDefiner.AppendInstruction(ICompiler::SLT, uTemp, &uAddress[1], 1, puConstants[CONSTANT_ZERO], uAddress, 1, uTemp, uAddress, 1);
			FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, pMaterialAmbient->uHandle, ALCHEMY_NULL, 0, pMaterialAmbient->bIsUsed ? pMaterialAmbient->uHandle : pMaterialAmbient->uSource, ALCHEMY_NULL, 0, uTemp, &uAddress[1], 1);
		}

		if(pDestinationPosition->uHandle)
		{
			const CEffectCode::VARYING* pPosition = &pVarying[VARYING_POSITION];
			UINT uSourcePosition = pPosition->uFragmentShaderHandle;
			if(!pPosition->IsUsed)
			{
				VertexShaderDefiner.AppendInstruction(ICompiler::M44, pPosition->uVertexShaderHandle, pAttributeVariables[ATTRIBUTE_VARIABLE_SOURCE_POSITION].uHandle, puConstants[CONSTANT_INVERSE_PROJECTION_MATRIX]);

				FragmentShaderDefiner.AppendInstruction(ICompiler::DIV, pDestinationPosition->uHandle, ALCHEMY_NULL, 0, pPosition->uFragmentShaderHandle, ALCHEMY_NULL, 0, pPosition->uFragmentShaderHandle, &uAddress[3], 1);

				uSourcePosition = pDestinationPosition->uHandle;
			}
			else
				FragmentShaderDefiner.AppendInstruction(ICompiler::MOV, pDestinationPosition->uHandle, &uAddress[3], 1, uRGBAToFloatCode, uAddress, 1, 0, ALCHEMY_NULL, 0);

			FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, pDestinationPosition->uHandle, uAddress, 3, uSourcePosition, uAddress, 3, uTemp, uAddress, 1);
		}
	}

	return true;
}