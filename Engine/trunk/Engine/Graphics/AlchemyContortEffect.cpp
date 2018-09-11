#include "AlchemyContortEffect.h"
#include "AlchemySceneManager.h"

using namespace alchemy;

CContortEffect::CContortEffect()
:m_pEffect(ALCHEMY_NULL),
m_uUserData(0)
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

	CRenderSystem& RenderSystem = pSceneManager->GetRenderSystem();

	m_fParameterValues[ EFFECT_SCALE ] = fScale;
	m_fParameterValues[ EFFECT_RADIUS ] = fRadius;

	CEffectCode EffectCode( RenderSystem.GetCompiler() );
	CEffectCode::CShaderParametersDefiner ParametersDefiner(EffectCode);

	ParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITIONT, 0);
	ParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT2, CVertexDeclaration::TEXCOORD, 0);
	UINT uParameters = ParametersDefiner.EndDefine();

	CEffectCode::CShaderFunctionDefiner VertexShaderDefiner(EffectCode, true);
	UINT uAttribute = VertexShaderDefiner.BeginFunction(uParameters), uVaring = VertexShaderDefiner.EndFunction(uParameters);
	VertexShaderDefiner.AppendInstruction(ICompiler::MOV, EffectCode.GetAddressVariable(uVaring, 0), EffectCode.GetAddressVariable(uAttribute, 0), 0);
	VertexShaderDefiner.AppendInstruction(ICompiler::MOV, EffectCode.GetAddressVariable(uVaring, 1), ALCHEMY_NULL, 0, EffectCode.GetAddressVariable(uAttribute, 1), ALCHEMY_NULL, 0, 0, ALCHEMY_NULL, 0);

	CEffectCode::CShaderFunctionDefiner FragmentShaderDefiner(EffectCode, false);

	uVaring = FragmentShaderDefiner.BeginFunction(uParameters);

	IEffect::SAMPLERDECLARATION SampleDeclaration;

	SampleDeclaration.Type     = IEffect::TEXTURE2D;

	SampleDeclaration.AddressU = IEffect::CLAMP;
	SampleDeclaration.AddressV = IEffect::CLAMP;
	SampleDeclaration.AddressW = IEffect::CLAMP;

	SampleDeclaration.MagFilter = IEffect::LINEAR;
	SampleDeclaration.MipFilter = IEffect::LINEAR;
	SampleDeclaration.MinFilter = IEffect::LINEAR;

	UINT uSampler = FragmentShaderDefiner.AddSamplerVariable(SampleDeclaration, 0);

	ParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT4, CVertexDeclaration::COLOR, 0);
	uParameters = ParametersDefiner.EndDefine();

	UINT uConstInfo[3] = {0};

	CEffectCode::GLOBALVARIABLE ConstInfo;

	for (UINT i=0; i<3; ++i)
	{
		ConstInfo.GlobalType = CEffectCode::INPUT_VARIABLE;
		ConstInfo.ShaderType = IEffect::FRAGMENT_SHADER;
		ConstInfo.uDetail    = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR1_VARIABLE, 1, 1, false);
		ConstInfo.uCount     = 1;
		ConstInfo.uIndex     = 0;
		ConstInfo.pValue     = &m_fParameterValues[ i ];
		uConstInfo[i] = EffectCode.AddGlobalVariable( ConstInfo );
	}

	UINT uTexInfo = FragmentShaderDefiner.ApplyVariable( CEffectCode::VECTOR2_VARIABLE, 1 );

	UINT uTexCoord = EffectCode.GetAddressVariable(uVaring, 1);

	UINT uSubAddress[] = {0,1,2,3};

	for (UINT i=0; i<2; ++i)
	{
		FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uTexInfo, &uSubAddress[i], 1, uTexCoord, &uSubAddress[i], 1, uConstInfo[EFFECT_SCALE], uSubAddress, 1);
		FragmentShaderDefiner.AppendInstruction(ICompiler::ADD, uTexInfo, &uSubAddress[i], 1, uTexInfo,  &uSubAddress[i], 1, uConstInfo[EFFECT_TIME], uSubAddress, 1);
		FragmentShaderDefiner.AppendInstruction(ICompiler::SIN, uTexInfo, &uSubAddress[i], 1, uTexInfo,  &uSubAddress[i], 1, 0, ALCHEMY_NULL, 0);
		FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uTexInfo, &uSubAddress[i], 1, uTexInfo,  &uSubAddress[i], 1, uConstInfo[EFFECT_RADIUS], uSubAddress, 1);
		FragmentShaderDefiner.AppendInstruction(ICompiler::ADD, uTexInfo, &uSubAddress[i], 1, uTexInfo,  &uSubAddress[i], 1, uTexCoord, &uSubAddress[i], 1);
	}

	FragmentShaderDefiner.AppendInstruction(ICompiler::TEX, EffectCode.GetAddressVariable(FragmentShaderDefiner.EndFunction(uParameters), 0), uTexInfo, uSampler);

	EffectCode.AddPass( VertexShaderDefiner.EndDefine(), FragmentShaderDefiner.EndDefine() );

	m_pEffect = RenderSystem.CreateEffect(EffectCode);

	m_pRenderMethod = pSceneManager->CreateRenderMethod(EffectCode);

	CSceneManager::IFullScreenDrawer* pFullScreenDrawer = pSceneManager->GetFullScreenDrawer();
	if(pFullScreenDrawer)
		m_uUserData = pFullScreenDrawer->AddUserData(0, m_pRenderMethod, 0, 0, m_pRenderMethod->GetParameterList()[0].pSamplerDeclaration);

	if(!m_uUserData)
		return false;

	UINT ByteAlignmentShift = RenderSystem.GetCompiler().GetCompilerInfo().uByteAlignmentShift;
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

	SetVisible(true);

	return true;
}

void CContortEffect::Destroy()
{
	if(m_pEffect)
	{
		m_pEffect->Release();

		m_pEffect = ALCHEMY_NULL;
	}

	for (UINT i=0; i<EFFECT_PARAMETER_COUNT; ++i)
	{
		m_uEffectParameters[i] = 0;
		m_fParameterValues[i] = 0.0f;
	}
}

bool CContortEffect::ApplyForRender()
{
	if(m_pSceneManager && m_pEffect)
	{
		CSceneManager::IFullScreenDrawer* pFullScreenDrawer = m_pSceneManager->GetFullScreenDrawer();
		if(pFullScreenDrawer)
		{
			CRenderQueue::RENDERENTRY& RenderEntry = m_pSceneManager->GetRenderQueue().LockRenderEntry(*pFullScreenDrawer);
			RenderEntry.uRenderTarget              = 0;
			RenderEntry.uEffect                    = m_pEffect->GetResourceHandle();
			RenderEntry.uRenderPass                = 0;
			RenderEntry.uVertexDeclaration         = 0;
			RenderEntry.uModel                     = 0;
			RenderEntry.uModelParameter            = 0;
			RenderEntry.uUserData                  = m_uUserData;

			return m_pSceneManager->GetRenderQueue().UnlockRenderEntry( CRenderQueue::FULL_SCREEN, ALCHEMY_FLAG(CRenderQueue::AUTO_SHADER) );
		}
	}

	return false;
}