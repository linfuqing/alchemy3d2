#include "AlchemyContortEffect.h"
#include "AlchemySceneManager.h"

using namespace alchemy;

CContortEffect::CContortEffect()
:m_pEffect(ALCHEMY_NULL)
{
	for (UINT i=0; i<EFFECT_PARAMETER_COUNT; ++i)
	{
		m_uEffectParameters[i] = 0;
		m_fParameterValues[i] = 0.0f;
	}
}

CContortEffect::~CContortEffect()
{
	Destroy();
}

bool CContortEffect::Create(FLOAT fScale, FLOAT fRadius)
{
	CSceneManager* pSceneManager = CSceneManager::GetInstance();
	if(ALCHEMY_NULL == pSceneManager)
		return false;

	SetFullScreenDrawer( &pSceneManager->GetFullScreenDrawer() );

	CRenderSystem& RenderSystem = pSceneManager->GetRenderSystem();

	m_fParameterValues[ EFFECT_SCALE ] = fScale;
	m_fParameterValues[ EFFECT_RADIUS ] = fRadius;

	CEffectCode EffectCode( RenderSystem.GetComplier() );
	CEffectCode::CShaderParametersDefiner ParametersDefiner(EffectCode);

	ParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITIONT);
	ParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT2, CVertexDeclaration::TEXCOORD);
	UINT uParameters = ParametersDefiner.EndDefine();

	CEffectCode::CShaderFunctionDefiner VertexShaderDefiner(EffectCode, true);
	UINT uAttribute = VertexShaderDefiner.BeginFunction(uParameters), uVaring = VertexShaderDefiner.EndFunction(uParameters);
	VertexShaderDefiner.AppendInstruction(CCompilerInterface::MOV, EffectCode.GetAddressVariable(uVaring, 0), EffectCode.GetAddressVariable(uAttribute, 0), 0);
	VertexShaderDefiner.AppendInstruction(CCompilerInterface::MOV, EffectCode.GetAddressVariable(uVaring, 1), ALCHEMY_NULL, 0, EffectCode.GetAddressVariable(uAttribute, 1), ALCHEMY_NULL, 0, 0, ALCHEMY_NULL, 0);

	CEffectCode::CShaderFunctionDefiner FragmentShaderDefiner(EffectCode, false);

	uVaring = FragmentShaderDefiner.BeginFunction(uParameters);

	CEffectCode::SAMPLERDECLARATION SampleDeclaration;

	SampleDeclaration.Type     = CEffectCode::TEXTURE2D;

	SampleDeclaration.AddressU = CEffectCode::CLAMP;
	SampleDeclaration.AddressV = CEffectCode::CLAMP;
	SampleDeclaration.AddressW = CEffectCode::CLAMP;

	SampleDeclaration.MagFilter = CEffectCode::LINEAR;
	SampleDeclaration.MipFilter = CEffectCode::LINEAR;
	SampleDeclaration.MinFilter = CEffectCode::LINEAR;

	UINT uSampler = EffectCode.AddSamplerVariable(SampleDeclaration, 0);

	ParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT4, CVertexDeclaration::COLOR);
	uParameters = ParametersDefiner.EndDefine();

	UINT uConstInfo[3] = {0};

	CEffectCode::GLOBALVARIABLEKEY ConstInfo;

	for (UINT i=0; i<3; ++i)
	{
		ConstInfo.GlobalType = CEffectCode::INPUT_VARIABLE;
		ConstInfo.ShaderType = CEffectInterface::FRAGMENT_SHADER;
		ConstInfo.uDetail    = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR1_VARIABLE, 1, 1, false);
		ConstInfo.uCount     = 1;
		ConstInfo.pValue     = &m_fParameterValues[ i ];
		uConstInfo[i] = EffectCode.AddGlobalVariable( ConstInfo );
	}

	UINT uTexInfo = FragmentShaderDefiner.ApplyVariable( CEffectCode::VECTOR2_VARIABLE, 1 );

	UINT uTexCoord = EffectCode.GetAddressVariable(uVaring, 1);

	UINT uSubAddress[] = {0,1,2,3};

	for (UINT i=0; i<2; ++i)
	{
		FragmentShaderDefiner.AppendInstruction(CCompilerInterface::MUL, uTexInfo, &uSubAddress[i], 1, uTexCoord, &uSubAddress[i], 1, uConstInfo[EFFECT_SCALE], uSubAddress, 1);
		FragmentShaderDefiner.AppendInstruction(CCompilerInterface::ADD, uTexInfo, &uSubAddress[i], 1, uTexInfo,  &uSubAddress[i], 1, uConstInfo[EFFECT_TIME], uSubAddress, 1);
		FragmentShaderDefiner.AppendInstruction(CCompilerInterface::SIN, uTexInfo, &uSubAddress[i], 1, uTexInfo,  &uSubAddress[i], 1, 0, ALCHEMY_NULL, 0);
		FragmentShaderDefiner.AppendInstruction(CCompilerInterface::MUL, uTexInfo, &uSubAddress[i], 1, uTexInfo,  &uSubAddress[i], 1, uConstInfo[EFFECT_RADIUS], uSubAddress, 1);
		FragmentShaderDefiner.AppendInstruction(CCompilerInterface::ADD, uTexInfo, &uSubAddress[i], 1, uTexInfo,  &uSubAddress[i], 1, uTexCoord, &uSubAddress[i], 1);
	}

	FragmentShaderDefiner.AppendInstruction(CCompilerInterface::TEX, EffectCode.GetAddressVariable(FragmentShaderDefiner.EndFunction(uParameters), 0), uTexInfo, uSampler);

	EffectCode.AddPass( VertexShaderDefiner.EndDefine(), FragmentShaderDefiner.EndDefine() );

	m_pEffect = RenderSystem.CreateEffect(EffectCode);

	UINT ByteAlignmentShift = RenderSystem.GetComplier().GetByteAlignmentShift();
	const CEffectCode::VARIABLE* pVariable;
	for (UINT i=0; i<EFFECT_PARAMETER_COUNT; ++i)
	{
		pVariable = EffectCode.GetVariable( uConstInfo[i] );
		if(pVariable)
			m_uEffectParameters[i] = pVariable->uRegister >> ByteAlignmentShift;
		else
			return false;
	}

	m_pSceneManager = pSceneManager;

	return true;
}

void CContortEffect::Destroy()
{
	if(m_pEffect)
		m_pEffect->Release();

	m_pEffect = ALCHEMY_NULL;

	for (UINT i=0; i<EFFECT_PARAMETER_COUNT; ++i)
	{
		m_uEffectParameters[i] = 0;
		m_fParameterValues[i] = 0.0f;
	}
}

CGraphicsResource::HANDLE CContortEffect::GetEffectHandle()
{
	return m_pEffect == ALCHEMY_NULL ? 0 : m_pEffect->GetResourceHandle();
}

void CContortEffect::OnEffectUpdate( UINT uFlag, CGraphicsResource::HANDLE uSceneTarget)
{
	if(m_pEffect && m_pSceneManager && m_pEffect)
	{
		m_fParameterValues[EFFECT_TIME] = m_pSceneManager->GetTime();

		if( ALCHEMY_TEST_BIT(uFlag, CRenderQueue::EFFECT) && !m_pEffect->Begin() )
			return;
		
		if( ALCHEMY_TEST_BIT(uFlag, CRenderQueue::EFFECT_PASS) && !m_pEffect->BeginPass(0) )
			return;

		for (UINT i=0; i<EFFECT_PARAMETER_COUNT; ++i)
			m_pEffect->SetParameter(CEffectInterface::FRAGMENT_SHADER, m_uEffectParameters[i], &m_fParameterValues[i], 1 );

		m_pEffect->SetTexture(CEffectInterface::FRAGMENT_SHADER, 0, uSceneTarget);
		m_pEffect->CommitChange();

		ALCHEMY_CLEAR_BIT(uFlag, CRenderQueue::EFFECT);

		GetFullScreenDrawer()->Draw( uSceneTarget, uFlag );

		m_pEffect->EndPass();
		m_pEffect->End();
	}
}