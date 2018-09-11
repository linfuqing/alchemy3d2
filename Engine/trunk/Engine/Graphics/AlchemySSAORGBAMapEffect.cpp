#include "AlchemySSAORGBAMapEffect.h"
#include "AlchemyRenderMethod.h"

#include "../Math/AlchemyMath.h"

#include <math.h>

using namespace alchemy;

//const CEffectCode::ELEMENTDECLARATION            CSSAORGBAMapEffect::sm_VARYING          [CSSAORGBAMapEffect::TOTAL_VARYING          ] = 
//{ 
//	{CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION, 0}
//};

const CEffectCode::TEMPORARYDECLARATION         CSSAORGBAMapEffect::sm_TEMPORARY         [CSSAORGBAMapEffect::TOTAL_TEMPORARY          ] = 
{ 
	{IEffect::FRAGMENT_SHADER, CEffectCode::VECTOR1_VARIABLE, 1}, 
	{IEffect::FRAGMENT_SHADER, CEffectCode::VECTOR4_VARIABLE, 1}, 
	{IEffect::FRAGMENT_SHADER, CEffectCode::VECTOR3_VARIABLE, 1}, 
	{IEffect::FRAGMENT_SHADER, CEffectCode::VECTOR3_VARIABLE, 1}, 
	{IEffect::FRAGMENT_SHADER, CEffectCode::VECTOR2_VARIABLE, 1}, 
	{IEffect::FRAGMENT_SHADER, CEffectCode::VECTOR2_VARIABLE, 1}, 
	{IEffect::FRAGMENT_SHADER, CEffectCode::VECTOR2_VARIABLE, 1}, 
	{IEffect::FRAGMENT_SHADER, CEffectCode::VECTOR4_VARIABLE, 1}, 
	{IEffect::FRAGMENT_SHADER, CEffectCode::VECTOR1_VARIABLE, 1}, 
	{IEffect::FRAGMENT_SHADER, CEffectCode::VECTOR1_VARIABLE, 1}
};

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION CSSAORGBAMapEffect::sm_ATTRIBUTE_VARIABLE[CSSAORGBAMapEffect::TOTAL_ATTRIBUTE_VARIABLES] = 
{ 
	//{IEffect::VERTEX_SHADER  , CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION, 0, true , true },
	{IEffect::FRAGMENT_SHADER, CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION, 0, false, false},
	{IEffect::FRAGMENT_SHADER, CVertexDeclaration::FLOAT3, CVertexDeclaration::NORMAL  , 0, false, false},
	{IEffect::FRAGMENT_SHADER, CVertexDeclaration::FLOAT2, CVertexDeclaration::TEXCOORD, 0, true , true }
};

const CEffectCode::CONSTANTVARIABLEDECLARATION CSSAORGBAMapEffect::sm_CONSTANT_VARIABLE[CSSAORGBAMapEffect::TOTAL_CONSTANT_VARIABLES] = 
{
	{IEffect::FRAGMENT_SHADER, CEffectCode::MATERIAL_VARIABLE, CRenderMethod::MATERIAL_AMBIENT, 1, 0, ALCHEMY_NULL, false, true}
};

CSSAORGBAMapEffect::CSSAORGBAMapEffect(
									   UINT uDepthSamplerIndex, 
									   UINT uNormalSamplerIndex, 
									   UINT uRandomSamplerIndex, 
									   UINT uNumDirection, 
									   UINT uStep, 
									   const FLOAT2* pRandomMapUVScale, 
									   const FLOAT2* pSamplerRadius, 
									   const FLOAT2* pInverseFocalLength, 
									   const FLOAT* pfScale, 
									   const FLOAT* pfBias, 
									   const FLOAT* pfIntensity, 
									   const FLOAT2* pRotationDirection) :
m_pConstants(ALCHEMY_NULL),
m_pRotationDirection(ALCHEMY_NULL)
{
	ALCHEMY_DEBUG_ASSERT(uNumDirection, "CSSAORGBAMapEffect::CSSAORGBAMapEffect: Direction number can not be 0.");

	LPSAMPLER pSampler;

	pSampler = &m_Samplers[SAMPLER_DEPTH];
	pSampler->Type                  = IEffect::FRAGMENT_SHADER;
	pSampler->Declaration.Type      = IEffect::TEXTURE2D;
	pSampler->Declaration.MinFilter = IEffect::POINT;
	pSampler->Declaration.MagFilter = IEffect::POINT;
	pSampler->Declaration.MipFilter = IEffect::POINT;
	pSampler->Declaration.AddressU  = IEffect::CLAMP;
	pSampler->Declaration.AddressV  = IEffect::CLAMP;
	pSampler->Declaration.AddressW  = IEffect::CLAMP;
	pSampler->uIndex                = uDepthSamplerIndex;

	pSampler = &m_Samplers[SAMPLER_NORMAL];
	pSampler->Type                  = IEffect::FRAGMENT_SHADER;
	pSampler->Declaration.Type      = IEffect::TEXTURE2D;
	pSampler->Declaration.MinFilter = IEffect::POINT;
	pSampler->Declaration.MagFilter = IEffect::POINT;
	pSampler->Declaration.MipFilter = IEffect::POINT;
	pSampler->Declaration.AddressU  = IEffect::CLAMP;
	pSampler->Declaration.AddressV  = IEffect::CLAMP;
	pSampler->Declaration.AddressW  = IEffect::CLAMP;
	pSampler->uIndex                = uNormalSamplerIndex;

	pSampler = &m_Samplers[SAMPLER_RANDOM];
	pSampler->Type                  = IEffect::FRAGMENT_SHADER;
	pSampler->Declaration.Type      = IEffect::TEXTURE2D;
	pSampler->Declaration.MinFilter = IEffect::POINT;
	pSampler->Declaration.MagFilter = IEffect::POINT;
	pSampler->Declaration.MipFilter = IEffect::POINT;
	pSampler->Declaration.AddressU  = IEffect::WRAP;
	pSampler->Declaration.AddressV  = IEffect::WRAP;
	pSampler->Declaration.AddressW  = IEffect::WRAP;
	pSampler->uIndex                = uRandomSamplerIndex;

	ALCHEMY_DEBUG_NEW(m_pConstants, CEffectCode::GLOBALVARIABLE[CONSTANT_ROTATION_DIRECTION + uNumDirection]);

	CEffectCode::LPGLOBALVARIABLE pGlobalVariable;

	/*pGlobalVariable = &m_pConstants[CONSTANT_INVERSE_PROJECTION_MATRIX];
	pGlobalVariable->ShaderType = IEffect::VERTEX_SHADER;
	pGlobalVariable->GlobalType = CEffectCode::CAMERA_VARIABLE;
	pGlobalVariable->uDetail    = CRenderMethod::CAMERA_INVERSE_PROJECTION_MATRIX;
	pGlobalVariable->uCount     = 1;
	pGlobalVariable->uIndex     = 0;
	pGlobalVariable->pValue     = ALCHEMY_NULL;*/

	pGlobalVariable = &m_pConstants[CONSTANT_ZERO];
	pGlobalVariable->ShaderType = IEffect::FRAGMENT_SHADER;
	pGlobalVariable->GlobalType = CEffectCode::CONSTANT_VARIABLE;
	pGlobalVariable->uDetail    = CRenderMethod::CONSTANT_ZERO;
	pGlobalVariable->uCount     = 1;
	pGlobalVariable->uIndex     = 0;
	pGlobalVariable->pValue     = ALCHEMY_NULL;

	pGlobalVariable = &m_pConstants[CONSTANT_ONE];
	pGlobalVariable->ShaderType = IEffect::FRAGMENT_SHADER;
	pGlobalVariable->GlobalType = CEffectCode::CONSTANT_VARIABLE;
	pGlobalVariable->uDetail    = CRenderMethod::CONSTANT_ONE;
	pGlobalVariable->uCount     = 1;
	pGlobalVariable->uIndex     = 0;
	pGlobalVariable->pValue     = ALCHEMY_NULL;

	pGlobalVariable = &m_pConstants[CONSTANT_TWO];
	pGlobalVariable->ShaderType = IEffect::FRAGMENT_SHADER;
	pGlobalVariable->GlobalType = CEffectCode::CONSTANT_VARIABLE;
	pGlobalVariable->uDetail    = CRenderMethod::CONSTANT_TWO;
	pGlobalVariable->uCount     = 1;
	pGlobalVariable->uIndex     = 0;
	pGlobalVariable->pValue     = ALCHEMY_NULL;

	pGlobalVariable = &m_pConstants[CONSTANT_RGBA_TO_FLOAT_CODE];
	pGlobalVariable->ShaderType = IEffect::FRAGMENT_SHADER;
	pGlobalVariable->GlobalType = CEffectCode::CONSTANT_VARIABLE;
	pGlobalVariable->uDetail    = CRenderMethod::CONSTANT_RGBA_TO_FLOAT_CODE;
	pGlobalVariable->uCount     = 1;
	pGlobalVariable->uIndex     = 0;
	pGlobalVariable->pValue     = ALCHEMY_NULL;

	pGlobalVariable = &m_pConstants[CONSTANT_CAMERA_DISTANCE];
	pGlobalVariable->ShaderType = IEffect::FRAGMENT_SHADER;
	pGlobalVariable->GlobalType = CEffectCode::CAMERA_VARIABLE;
	pGlobalVariable->uDetail    = CRenderMethod::CAMERA_DISTANCE;
	pGlobalVariable->uCount     = 1;
	pGlobalVariable->uIndex     = 0;
	pGlobalVariable->pValue     = ALCHEMY_NULL;

	pRandomMapUVScale = pRandomMapUVScale ? pRandomMapUVScale : &m_RandomMapUVScale;
	pGlobalVariable = &m_pConstants[CONSTANT_RANDOM_MAP_UV_SCALE];
	pGlobalVariable->ShaderType = IEffect::FRAGMENT_SHADER;
	pGlobalVariable->GlobalType = CEffectCode::INPUT_VARIABLE;
	pGlobalVariable->uDetail    = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR2_VARIABLE, 1, 2, false);
	pGlobalVariable->uCount     = 1;
	pGlobalVariable->uIndex     = 0;
	pGlobalVariable->pValue     = pRandomMapUVScale;

	pSamplerRadius = pSamplerRadius ? pSamplerRadius : &m_SamplerRadius;
	pGlobalVariable = &m_pConstants[CONSTANT_SAMPLE_RADIUS];
	pGlobalVariable->ShaderType = IEffect::FRAGMENT_SHADER;
	pGlobalVariable->GlobalType = CEffectCode::INPUT_VARIABLE;
	pGlobalVariable->uDetail    = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR2_VARIABLE, 1, 2, false);
	pGlobalVariable->uCount     = 1;
	pGlobalVariable->uIndex     = 0;
	pGlobalVariable->pValue     = pSamplerRadius->fValue;

	pInverseFocalLength = pInverseFocalLength ? pInverseFocalLength : &m_InverseFocalLength;
	pGlobalVariable = &m_pConstants[CONSTANT_INVERSE_FOCAL_LENGTH];
	pGlobalVariable->ShaderType = IEffect::FRAGMENT_SHADER;
	pGlobalVariable->GlobalType = CEffectCode::INPUT_VARIABLE;
	pGlobalVariable->uDetail    = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR2_VARIABLE, 1, 2, false);
	pGlobalVariable->uCount     = 1;
	pGlobalVariable->uIndex     = 0;
	pGlobalVariable->pValue     = pInverseFocalLength->fValue;

	pfScale = pfScale ? pfScale : &m_fScale;
	pGlobalVariable = &m_pConstants[CONSTANT_SCALE];
	pGlobalVariable->ShaderType = IEffect::FRAGMENT_SHADER;
	pGlobalVariable->GlobalType = CEffectCode::INPUT_VARIABLE;
	pGlobalVariable->uDetail    = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR1_VARIABLE, 1, 1, false);
	pGlobalVariable->uCount     = 1;
	pGlobalVariable->uIndex     = 0;
	pGlobalVariable->pValue     = pfScale;

	pfBias = pfBias ? pfBias : &m_fBias;
	pGlobalVariable = &m_pConstants[CONSTANT_BIAS];
	pGlobalVariable->ShaderType = IEffect::FRAGMENT_SHADER;
	pGlobalVariable->GlobalType = CEffectCode::INPUT_VARIABLE;
	pGlobalVariable->uDetail    = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR1_VARIABLE, 1, 1, false);
	pGlobalVariable->uCount     = 1;
	pGlobalVariable->uIndex     = 0;
	pGlobalVariable->pValue     = pfBias;

	pfIntensity = pfIntensity ? pfIntensity : &m_fIntensity;
	pGlobalVariable = &m_pConstants[CONSTANT_INTENSITY];
	pGlobalVariable->ShaderType = IEffect::FRAGMENT_SHADER;
	pGlobalVariable->GlobalType = CEffectCode::INPUT_VARIABLE;
	pGlobalVariable->uDetail    = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR1_VARIABLE, 1, 1, false);
	pGlobalVariable->uCount     = 1;
	pGlobalVariable->uIndex     = 0;
	pGlobalVariable->pValue     = pfIntensity;

	if(pRotationDirection)
	{
		for(UINT i = 0; i < uNumDirection; ++ i)
		{
			pGlobalVariable = &m_pConstants[CONSTANT_ROTATION_DIRECTION + i];
			pGlobalVariable->ShaderType = IEffect::FRAGMENT_SHADER;
			pGlobalVariable->GlobalType = CEffectCode::INPUT_VARIABLE;
			pGlobalVariable->uDetail    = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR2_VARIABLE, 1, 2, false);
			pGlobalVariable->uCount     = 1;
			pGlobalVariable->uIndex     = 0;
			pGlobalVariable->pValue     = pRotationDirection->fValue;

			++ pRotationDirection;
		}
	}
	else
	{
		FLOAT fAlpha = g_fTWO_PI / uNumDirection, fAngle = 0.0f;
		ALCHEMY_DEBUG_NEW(m_pRotationDirection, CFloat2[uNumDirection]);
		for(UINT i = 0; i < uNumDirection; ++ i)
		{
			pRotationDirection = &( m_pRotationDirection[i] = CFloat2( cosf(fAngle), sinf(fAngle) ) );

			pGlobalVariable = &m_pConstants[CONSTANT_ROTATION_DIRECTION + i];
			pGlobalVariable->ShaderType = IEffect::FRAGMENT_SHADER;
			pGlobalVariable->GlobalType = CEffectCode::INPUT_VARIABLE;
			pGlobalVariable->uDetail    = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR2_VARIABLE, 1, 2, false);
			pGlobalVariable->uCount     = 1;
			pGlobalVariable->uIndex     = 0;
			pGlobalVariable->pValue     = pRotationDirection->fValue;

			fAngle += fAlpha;
		}
	}

	m_uNumDirection = uNumDirection;

	m_uStep         = uStep;
}

CSSAORGBAMapEffect::~CSSAORGBAMapEffect(void)
{
	ALCHEMY_DEBUG_DELETE_ARRAY(m_pConstants);

	ALCHEMY_DEBUG_DELETE_ARRAY(m_pRotationDirection);
}

UINT                                            CSSAORGBAMapEffect::GetEffectInfoFlag     (                             )
{
	return ALCHEMY_FLAG(IEffectInfo::FRAGMENT_SHADER_INFO) | ALCHEMY_FLAG(IEffectInfo::FRAGMENT_SHADER_OUTPUT);;
}

const IEffectInfo::SAMPLER           * CSSAORGBAMapEffect::GetSamplers           (UINT& uSamplerCount          )
{
	uSamplerCount = TOTAL_SAMPLERS;

	return m_Samplers;
}

const CEffectCode::ELEMENTDECLARATION            * CSSAORGBAMapEffect::GetAttributes        (UINT& uAttributeCount        )
{
	uAttributeCount = TOTAL_ATTRIBUTES;

	return ALCHEMY_NULL;
}

const CEffectCode::ELEMENTDECLARATION            * CSSAORGBAMapEffect::GetVarying           (UINT& uVaryingCount          )
{
	uVaryingCount = TOTAL_VARYING;

	return ALCHEMY_NULL;//sm_VARYING;
}

const CEffectCode::GLOBALVARIABLE              * CSSAORGBAMapEffect::GetConstants         (UINT& uConstantCount         )
{
	uConstantCount = CONSTANT_ROTATION_DIRECTION + m_uNumDirection;

	return m_pConstants;
}

const CEffectCode::TEMPORARYDECLARATION        * CSSAORGBAMapEffect::GetTemporary         (UINT& uTemporaryCount        )
{
	uTemporaryCount = TOTAL_TEMPORARY;

	return sm_TEMPORARY;
}

const CEffectCode::ATTRIBUTEVARIABLEDECLARATION* CSSAORGBAMapEffect::GetAttributeVariables(UINT& uAttributeVariableCount)
{
	uAttributeVariableCount = TOTAL_ATTRIBUTE_VARIABLES;

	return sm_ATTRIBUTE_VARIABLE;
}

const CEffectCode::CONSTANTVARIABLEDECLARATION * CSSAORGBAMapEffect::GetConstantVariables  (UINT& uConstantVariableCount)
{
	uConstantVariableCount = TOTAL_CONSTANT_VARIABLES;

	return sm_CONSTANT_VARIABLE;
}

bool CSSAORGBAMapEffect::ShaderDefineHandler(
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
	UINT uNormal = puTemporary[TEMPORARY_NORMAL], 
		uRandom = puTemporary[TEMPORARY_RANDOM], 
		uTexcoord = pAttributeVariables[ATTRIBUTE_VARIABLE_TEXCOORD].uHandle, 
		uOne = puConstants[CONSTANT_ONE], 
		uTwo = puConstants[CONSTANT_TWO];

	FragmentShaderDefiner.AppendInstruction(ICompiler::TEX, uNormal, uTexcoord, puSamplers[SAMPLER_NORMAL]);
	FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uNormal, uNormal  , uTwo                      );
	FragmentShaderDefiner.AppendInstruction(ICompiler::SUB, uNormal, uNormal  , uOne                      );

	UINT uDeltaUV = puTemporary[TEMPORARY_TEXCOORD], uRandomMapUVScale = puConstants[CONSTANT_RANDOM_MAP_UV_SCALE];
	FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uDeltaUV, uTexcoord, uRandomMapUVScale         );
	FragmentShaderDefiner.AppendInstruction(ICompiler::TEX, uRandom , uDeltaUV , puSamplers[SAMPLER_RANDOM]);
	FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uRandom , uRandom  , uTwo                      );
	FragmentShaderDefiner.AppendInstruction(ICompiler::SUB, uRandom , uRandom  , uOne                      );
	//FragmentShaderDefiner.AppendInstruction(ICompiler::NRM, uRandom , uRandom  , 0                         );

	const CEffectCode::SHADERVARIABLE* pNormal   = &pAttributeVariables[ATTRIBUTE_VARIABLE_NORMAL];
	if(pNormal->uHandle)
		FragmentShaderDefiner.AppendInstruction(ICompiler::MOV, pNormal->uHandle, uNormal, 0);

	UINT uSourcePositionTemp = puTemporary[TEMPORARY_SOURCE_POSITION], uDepthSampler = puSamplers[SAMPLER_DEPTH];
	FragmentShaderDefiner.AppendInstruction(ICompiler::TEX, uSourcePositionTemp, uTexcoord, uDepthSampler);
	UINT uDepth = puTemporary[TEMPORARY_DEPTH], uRGBAToFloatCode = puConstants[CONSTANT_RGBA_TO_FLOAT_CODE];
	FragmentShaderDefiner.AppendInstruction(ICompiler::DP4, uDepth, uSourcePositionTemp, uRGBAToFloatCode);

	UINT uCameraDistance = puConstants[CONSTANT_CAMERA_DISTANCE], uInverseFocalLength = puConstants[CONSTANT_INVERSE_FOCAL_LENGTH];
	UINT uAddress[] = {0, 1, 2, 3};
	FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uSourcePositionTemp, &uAddress[2], 1, uDepth, uAddress, 1, uCameraDistance, uAddress, 1);

	FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uSourcePositionTemp, uAddress, 2, uTexcoord          , uAddress, 2, uTwo, uAddress, 1);
	FragmentShaderDefiner.AppendInstruction(ICompiler::SUB, uSourcePositionTemp, uAddress, 2, uSourcePositionTemp, uAddress, 2, uOne, uAddress, 1);

	FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uSourcePositionTemp, uAddress, 2, uSourcePositionTemp, uAddress, 2, uInverseFocalLength, uAddress    , 2);

	FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uSourcePositionTemp, uAddress, 2, uSourcePositionTemp, uAddress, 2, uSourcePositionTemp, &uAddress[2], 1);

	/*const CEffectCode::VARYING* pPosition = &pVarying[VARYING_POSITION];
	UINT uSourcePosition = pPosition->uFragmentShaderHandle;
	UINT uAddress[] = {0, 1, 2, 3};
	if(!pPosition->IsUsed)
	{
		VertexShaderDefiner.AppendInstruction(ICompiler::M44, pPosition->uVertexShaderHandle, pAttributeVariables[ATTRIBUTE_VARIABLE_SOURCE_POSITION].uHandle, puConstants[CONSTANT_INVERSE_PROJECTION_MATRIX]);

		FragmentShaderDefiner.AppendInstruction(ICompiler::DIV, uSourcePositionTemp, ALCHEMY_NULL, 0, pPosition->uFragmentShaderHandle, ALCHEMY_NULL, 0, pPosition->uFragmentShaderHandle, &uAddress[3], 1);

		uSourcePosition = uSourcePositionTemp;
	}
	else
		FragmentShaderDefiner.AppendInstruction(ICompiler::MOV, uSourcePositionTemp, &uAddress[3], 1, uOne, uAddress, 1, 0, ALCHEMY_NULL, 0);

	FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uSourcePositionTemp, uAddress, 3, uSourcePosition, uAddress, 3, uDepth, uAddress, 1);*/
	const CEffectCode::SHADERVARIABLE* pDestinationPosition = &pAttributeVariables[ATTRIBUTE_VARIABLE_DESTINATION_POSITION];
	if(pDestinationPosition->uHandle)
		FragmentShaderDefiner.AppendInstruction(ICompiler::MOV, pDestinationPosition->uHandle, uSourcePositionTemp, 0);

	//UINT uZero = puConstants[CONSTANT_ZERO], uIntensity = puTemporary[TEMPORARY_INTENSITY];
	//FragmentShaderDefiner.AppendInstruction(ICompiler::SLT, uIntensity, uZero     , uDepth);
	//FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uIntensity, uIntensity, puConstants[CONSTANT_INTENSITY]);

	UINT uRadius = puTemporary[TEMPORARY_RADIUS];
	FragmentShaderDefiner.AppendInstruction(ICompiler::DIV, uRadius, uAddress, 2, puConstants[CONSTANT_SAMPLE_RADIUS], uAddress, 2, uSourcePositionTemp, &uAddress[2], 1);
	FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uRadius, uAddress, 2, uRadius, uAddress, 2, uRandom, &uAddress[2], 1);

	UINT uReflection = puTemporary[TEMPORARY_REFLECTION], 
		uDestinationPositonTemp = puTemporary[TEMPORARY_DESTINATION_POSITION], 
		uAO = puTemporary[TEMPORAPY_AO], 
		uResult = puTemporary[TEMPORARY_RESULT], 
		uZero = puConstants[CONSTANT_ZERO], 
		uScale = puConstants[CONSTANT_SCALE], 
		uBias = puConstants[CONSTANT_BIAS], 
		uDirection,
		i,
		j;

	for(i = 0; i < m_uNumDirection; ++ i)
	{
		uDirection = puConstants[CONSTANT_ROTATION_DIRECTION + i];
		FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uDeltaUV   , uAddress    , 2, uDirection , uAddress, 2, uRandom    , uAddress    , 1);
		FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uReflection, uAddress    , 2, uDirection , uAddress, 2, uRandom    , &uAddress[1], 1);

		FragmentShaderDefiner.AppendInstruction(ICompiler::SUB, uDeltaUV   , uAddress    , 1, uDeltaUV   , uAddress, 1, uReflection, &uAddress[1], 1);
		FragmentShaderDefiner.AppendInstruction(ICompiler::ADD, uDeltaUV   , &uAddress[1], 1, uReflection, uAddress, 1, uDeltaUV   , &uAddress[1], 1);

		/*FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uReflection, uAddress    , 2, uRandom    , uAddress, 2, uDeltaUV   , uAddress    , 2);
		FragmentShaderDefiner.AppendInstruction(ICompiler::ADD, uReflection, uAddress    , 2, uReflection, uAddress, 1, uReflection, &uAddress[1], 1);
		FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uReflection, uAddress    , 2, uReflection, uAddress, 2, uRandom    , uAddress    , 2);

		FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uReflection, uReflection, uTwo);

		FragmentShaderDefiner.AppendInstruction(ICompiler::SUB, uReflection, uAddress, 2, uDeltaUV, uAddress, 2, uReflection, uAddress, 2);*/

		FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uReflection/*uDeltaUV*/, uDeltaUV/*uReflection*/, uRadius);

		//FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uDeltaUV, uAddress, 2, uDeltaUV, uAddress, 2, uRandom, &uAddress[2], 1);

		for(j = 0; j < m_uStep; ++ j)
		{
			FragmentShaderDefiner.AppendInstruction(ICompiler::ADD, uDeltaUV, uReflection/*uDeltaUV*/, j ? uDeltaUV : uTexcoord);

			FragmentShaderDefiner.AppendInstruction(ICompiler::TEX, uDestinationPositonTemp, uDeltaUV, uDepthSampler);

			FragmentShaderDefiner.AppendInstruction(ICompiler::DP4, uDepth, uDestinationPositonTemp, uRGBAToFloatCode);

			FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uDestinationPositonTemp, &uAddress[2], 1, uDepth, uAddress, 1, uCameraDistance, uAddress, 1);

			FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uDestinationPositonTemp, uAddress, 2, uDeltaUV               , uAddress, 2, uTwo, uAddress, 1);
			FragmentShaderDefiner.AppendInstruction(ICompiler::SUB, uDestinationPositonTemp, uAddress, 2, uDestinationPositonTemp, uAddress, 2, uOne, uAddress, 1);

			FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uDestinationPositonTemp, uAddress, 2, uDestinationPositonTemp, uAddress, 2, uInverseFocalLength, uAddress, 2);

			FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uDestinationPositonTemp, uAddress, 2, uDestinationPositonTemp, uAddress, 2, uDestinationPositonTemp, &uAddress[2], 1);

			FragmentShaderDefiner.AppendInstruction(ICompiler::SUB, uDestinationPositonTemp, uDestinationPositonTemp, uSourcePositionTemp);

			FragmentShaderDefiner.AppendInstruction(ICompiler::DP3, uDepth, uAddress, 1, uDestinationPositonTemp, uAddress, 3, uDestinationPositonTemp, uAddress, 3);

			FragmentShaderDefiner.AppendInstruction(ICompiler::RSQ, uDepth, uDepth, 0);

			FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uDestinationPositonTemp, uDestinationPositonTemp, uDepth);

			FragmentShaderDefiner.AppendInstruction(ICompiler::DP3, uAO, uAddress, 1, uNormal, uAddress, 3, uDestinationPositonTemp, uAddress, 3);

			FragmentShaderDefiner.AppendInstruction(ICompiler::ADD, uAO, uAO, uBias);

			FragmentShaderDefiner.AppendInstruction(ICompiler::MAX, uAO, uAO, uZero);

			FragmentShaderDefiner.AppendInstruction(ICompiler::RCP, uDepth, uDepth, 0);

			FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uDepth, uDepth, uScale);

			FragmentShaderDefiner.AppendInstruction(ICompiler::ADD, uDepth, uDepth, uOne);

			if(i || j)
			{
				FragmentShaderDefiner.AppendInstruction(ICompiler::DIV, uAO, uAO, uDepth);

				FragmentShaderDefiner.AppendInstruction(ICompiler::ADD, uResult, uResult, uAO);
			}
			else
				FragmentShaderDefiner.AppendInstruction(ICompiler::DIV, uResult, uAO, uDepth);
		}
	}

	FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uResult, uResult, puConstants[CONSTANT_INTENSITY]);
	FragmentShaderDefiner.AppendInstruction(ICompiler::SAT, uResult, uResult, 0                              );
	//FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uResult, uResult, uIntensity                     );

	const CEffectCode::SHADERVARIABLE* pMaterialAmbient = &pConstantVariables[CONSTANT_VARIABLE_MATERIAL_AMBIENT];
	if(pMaterialAmbient->uHandle)
	{
		FragmentShaderDefiner.AppendInstruction(ICompiler::SUB, uResult, uOne, uResult);
		FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, pMaterialAmbient->uHandle, pMaterialAmbient->bIsUsed ? pMaterialAmbient->uHandle : pMaterialAmbient->uSource, uResult);
		FragmentShaderDefiner.AppendInstruction(ICompiler::MOV, uOutputColor, uResult, 0);
	}
	else
		FragmentShaderDefiner.AppendInstruction(ICompiler::SUB, uOutputColor, uOne, uResult);

	return true;
}