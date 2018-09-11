#include "AlchemyPixelAmbientLightEffect.h"

#include "AlchemyRenderMethod.h"

using namespace alchemy;

const CEffectCode::ELEMENTDECLARATION CPixelAmbientLightEffect::sm_ATTRIBUTES[CPixelAmbientLightEffect::TOTAL_ATTRIBUTES] = 
{
	{CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION , 0} 
};

const CEffectCode::ELEMENTDECLARATION CPixelAmbientLightEffect::sm_VARYING   [CPixelAmbientLightEffect::TOTAL_VARYING   ] = 
{ 
	{CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITIONT, 0} 
};

const CEffectCode::GLOBALVARIABLE   CPixelAmbientLightEffect::sm_CONSTANS  [CPixelAmbientLightEffect::TOTAL_CONSTANTS ] = 
{ 
	{IEffect::VERTEX_SHADER, CEffectCode::CAMERA_VARIABLE, CRenderMethod::CAMERA_WORLD_VIEW_PROJECTION_MATRIX, 1, 0, ALCHEMY_NULL}, 
	{IEffect::FRAGMENT_SHADER, CEffectCode::NORMAL_VARIABLE, IRenderMethod::AMBIENT_LIGHT, 1, 0, ALCHEMY_NULL}
};

CPixelAmbientLightEffect::CPixelAmbientLightEffect(void)
{
}

CPixelAmbientLightEffect::~CPixelAmbientLightEffect(void)
{
}

UINT                                       CPixelAmbientLightEffect::GetEffectInfoFlag     (                             )
{
    return ALCHEMY_FLAG(IEffectInfo::FRAGMENT_SHADER_OUTPUT);
}

const IEffectInfo::SAMPLER      * CPixelAmbientLightEffect::GetSamplers  (UINT& uSamplerCount  )
{
    uSamplerCount = TOTAL_SAMPLERS;
    
    return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION* CPixelAmbientLightEffect::GetAttributes(UINT& uAttributeCount)
{
    uAttributeCount = TOTAL_ATTRIBUTES;
    
    return sm_ATTRIBUTES;
}

const CEffectCode::ELEMENTDECLARATION* CPixelAmbientLightEffect::GetVarying   (UINT& uVaryingCount  )
{
    uVaryingCount = TOTAL_VARYING;
    
    return sm_VARYING;
}

const CEffectCode::GLOBALVARIABLE        * CPixelAmbientLightEffect::GetConstants (UINT& uConstantCount )
{
    uConstantCount = TOTAL_CONSTANTS;
    
    return sm_CONSTANS;
}

const CEffectCode::TEMPORARYDECLARATION        * CPixelAmbientLightEffect::GetTemporary         (UINT& uTemporaryCount        )
{
    uTemporaryCount = TOTAL_TEMPORARY;
    
    return ALCHEMY_NULL;
}

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CPixelAmbientLightEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
{
    uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLES;
    
    return ALCHEMY_NULL;
}

const CEffectCode::CONSTANTVARIABLEDECLARATION * CPixelAmbientLightEffect::GetConstantVariables  (UINT& uConstantVariableCount)
{
    uConstantVariableCount = TOTAL_CONSTANT_VARIABLES;
    
    return ALCHEMY_NULL;
}

bool CPixelAmbientLightEffect::ShaderDefineHandler(
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
		VertexShaderDefiner.AppendInstruction(ICompiler::M44, pPositiont->uVertexShaderHandle, puAttributes[ATTRIBUTE_POSITION], puConstants[CONSTANT_WORLD_VIEW_PROJECTION_MATRIX]);

	FragmentShaderDefiner.AppendInstruction(ICompiler::MOV, uOutputColor, puConstants[CONSTANT_AMBIENT_COLOR], 0);

	return true;
}