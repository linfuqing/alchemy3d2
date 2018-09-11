#include "AlchemyVolumeLightShow.h"
#include "AlchemySceneManager.h"


#include "AlchemyPositionEffect.h"
#include "AlchemyPosition2DEffect.h"
#include "AlchemyPixelColorEffect.h"
#include "AlchemyTextureEffect.h"
#include "AlchemyEffectInfoAddition.h"

using namespace alchemy;

static const FLOAT4 BLACK_COLOR = {0.0f, 0.0f, 0.0f, 1.0f};

CVolumeLightShow::CVolumeLightShow()
:m_pEffect(ALCHEMY_NULL),
m_pRenderMethod(ALCHEMY_NULL),
m_OccTexture(0),
m_pLight(ALCHEMY_NULL),
m_pMaterial(ALCHEMY_NULL),
m_bIsBeginRender(false),
m_OffScreenTexture(0)
{
	m_uUserData[0] = m_uUserData[1] = 0;
}

CVolumeLightShow::~CVolumeLightShow()
{
	
}

bool CVolumeLightShow::Create(CGraphicsResource::HANDLE OccTexture, CGraphicsResource::HANDLE Light, UINT uNumSamples, FLOAT fDensity, 
							  FLOAT fExposure, FLOAT fDecay, FLOAT fScatterWeight  )
{
	Destroy();

	m_pSceneManager = CSceneManager::GetInstance();
	CRenderSystem& RenderSystem = m_pSceneManager->GetRenderSystem();


	m_fScatterWeight = fScatterWeight;
	m_ConstantData[ SAMPLES_NUM_INDEX ] = (FLOAT)uNumSamples;
	m_ConstantData[ DENSITY_INDEX ]		= fDensity;
	m_ConstantData[ EXPOSURE_INDEX ]	= fExposure;
	m_ConstantData[ DECAY_INDEX ]		= fDecay;

	m_fSkip = (1.0f/(FLOAT)uNumSamples)*fDensity;

	m_pLight = static_cast<CLight*>( m_pSceneManager->GetResource(Light, CGraphicsResource::LIGHT) );//->GetLight( uLightIndex );
	if( !m_pLight )
		return false;

	LPTEXTURE pTexture = RenderSystem.CreateRenderTargetView(CTexture::DEFAULT, 0);
	if(!pTexture)
	{
		ALCHEMY_DEBUG_WARNING("CVolumeLightShow::Create() - RenderSystem.CreateRenderTargetView() - error");
		return false;
	}

	m_OffScreenTexture = pTexture->GetResourceHandle();

	pTexture = static_cast<LPTEXTURE>( RenderSystem.GetResource(OccTexture, CGraphicsResource::TEXTURE) );
	if(!pTexture)
	{
		ALCHEMY_DEBUG_WARNING("CVolumeLightShow::Create() - RenderSystem.CreateRenderTargetView() - error");
		return false;
	}

	m_OccTexture = pTexture->GetResourceHandle();

	UINT i;
	m_pMaterial = m_pSceneManager->CreateMaterial();
	m_pMaterial->AddTexture( m_OccTexture );

	CEffectCode Code( m_pSceneManager->GetRenderSystem().GetCompiler() );
	CEffectCode::CShaderParametersDefiner ParametersDefiner(Code);

	ParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITIONT, 0);
	ParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT2, CVertexDeclaration::TEXCOORD, 0);
	UINT uParameters = ParametersDefiner.EndDefine();

	CEffectCode::CShaderFunctionDefiner VertexShaderDefiner(Code, true);
	UINT uAttribute = VertexShaderDefiner.BeginFunction(uParameters), uVaring = VertexShaderDefiner.EndFunction(uParameters);
	VertexShaderDefiner.AppendInstruction(ICompiler::MOV, Code.GetAddressVariable(uVaring, 0), Code.GetAddressVariable(uAttribute, 0), 0);
	VertexShaderDefiner.AppendInstruction(ICompiler::MOV, Code.GetAddressVariable(uVaring, 1), ALCHEMY_NULL, 0,
		Code.GetAddressVariable(uAttribute, 1), ALCHEMY_NULL,0,  0, ALCHEMY_NULL, 0);

	CEffectCode::CShaderFunctionDefiner FragmentShaderDefiner(Code, false);

	uVaring = FragmentShaderDefiner.BeginFunction(uParameters);
	ParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT4, CVertexDeclaration::COLOR, 0);
	UINT uOutputColor = Code.GetAddressVariable( FragmentShaderDefiner.EndFunction( ParametersDefiner.EndDefine() ), 0);

	IEffect::SAMPLERDECLARATION SampleDeclaration;
	SampleDeclaration.Type     = IEffect::TEXTURE2D;

	SampleDeclaration.AddressU = IEffect::CLAMP;
	SampleDeclaration.AddressV = IEffect::CLAMP;
	SampleDeclaration.AddressW = IEffect::CLAMP;

	SampleDeclaration.MagFilter = IEffect::LINEAR;
	SampleDeclaration.MipFilter = IEffect::LINEAR;
	SampleDeclaration.MinFilter = IEffect::LINEAR;

	UINT Samplere = FragmentShaderDefiner.AddSamplerVariable(SampleDeclaration, 0);

	CEffectCode::GLOBALVARIABLE ConstInfo;
	ConstInfo.GlobalType = CEffectCode::INPUT_VARIABLE;
	ConstInfo.ShaderType = IEffect::FRAGMENT_SHADER;
	ConstInfo.uDetail    = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR4_VARIABLE, 1, 4, false);
	ConstInfo.uCount     = 1;
	ConstInfo.uIndex     = 0;
	ConstInfo.pValue     = m_ConstantData;
	UINT uLightInfo = Code.AddGlobalVariable( ConstInfo );

	/*ConstInfo.GlobalType = CEffectCode::INPUT_VARIABLE;
	ConstInfo.ShaderType = IEffect::FRAGMENT_SHADER;
	ConstInfo.uDetail    = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR1_VARIABLE, 1, 1, false);
	ConstInfo.uCount     = 1;
	ConstInfo.uIndex     = 0;
	ConstInfo.pValue     = &m_fScatterWeight;
	UINT uLightWeight = Code.AddGlobalVariable( ConstInfo );*/

	ConstInfo.GlobalType = CEffectCode::INPUT_VARIABLE;
	ConstInfo.ShaderType = IEffect::FRAGMENT_SHADER;
	ConstInfo.uDetail    = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR2_VARIABLE, 1, 2, false);
	ConstInfo.uCount     = 1;
	ConstInfo.uIndex     = 0;
	ConstInfo.pValue     = &m_ScreenLightPos;
	UINT uScreenLightPos = Code.AddGlobalVariable( ConstInfo );

	ConstInfo.GlobalType = CEffectCode::INPUT_VARIABLE;
	ConstInfo.ShaderType = IEffect::FRAGMENT_SHADER;
	ConstInfo.uDetail    = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR1_VARIABLE, 1, 1, false);
	ConstInfo.uCount     = 1;
	ConstInfo.uIndex     = 0;
	ConstInfo.pValue     = &m_fSkip;
	UINT uSkipInfo = Code.AddGlobalVariable( ConstInfo );

	UINT* pNumInfo;
	FLOAT fVal = 1.0f;
	for (i=0; i<uNumSamples; ++i)
	{
		fVal *= m_fScatterWeight;
		m_fNumSampleInfoes.Add( fVal );
		fVal *= fDecay;
	}

	ALCHEMY_DEBUG_NEW(pNumInfo, UINT[uNumSamples]);

	for (i=0; i<uNumSamples; ++i)
	{
		ConstInfo.GlobalType = CEffectCode::INPUT_VARIABLE;
		ConstInfo.ShaderType = IEffect::FRAGMENT_SHADER;
		ConstInfo.uDetail    = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR1_VARIABLE, 1, 1, false);
		ConstInfo.uCount     = 1;
		ConstInfo.uIndex     = 0;
		ConstInfo.pValue     = &m_fNumSampleInfoes[i];
		pNumInfo[i] = Code.AddGlobalVariable( ConstInfo );
	}
	
	UINT uTexCoord = Code.GetAddressVariable(uVaring, 1);

	UINT uSubAddress[] = {0, 1, 2, 3};

	UINT uOccVariable		= FragmentShaderDefiner.ApplyVariable(CEffectCode::VECTOR4_VARIABLE, 1);
	UINT uTempVariable	= FragmentShaderDefiner.ApplyVariable(CEffectCode::VECTOR4_VARIABLE, 1);
	UINT uSamplerVariable	= FragmentShaderDefiner.ApplyVariable(CEffectCode::VECTOR4_VARIABLE, 1);

	FragmentShaderDefiner.AppendInstruction( ICompiler::TEX, uOccVariable, uTexCoord, Samplere);

	FragmentShaderDefiner.AppendInstruction( ICompiler::MOV, uTempVariable, &uSubAddress[2], 2,
		uTexCoord, uSubAddress, 2,  0, ALCHEMY_NULL, 0);

	FragmentShaderDefiner.AppendInstruction( ICompiler::SUB, uTempVariable, uSubAddress, 2,
		uTexCoord, uSubAddress, 2, uScreenLightPos, uSubAddress, 2);

	FragmentShaderDefiner.AppendInstruction( ICompiler::MUL, uTempVariable, uSubAddress, 2,
		uTempVariable, uSubAddress, 2, uSkipInfo, uSubAddress, 1);

	for (i=0; i<uNumSamples; ++i)
	{
		FragmentShaderDefiner.AppendInstruction(ICompiler::SUB, uTempVariable, &uSubAddress[2], 2,
			uTempVariable, &uSubAddress[2], 2, uTempVariable, uSubAddress, 2);

		FragmentShaderDefiner.AppendInstruction( ICompiler::TEX, uSamplerVariable, ALCHEMY_NULL, 0,
			uTempVariable, &uSubAddress[2], 2, Samplere,ALCHEMY_NULL, 0);

		FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uSamplerVariable, ALCHEMY_NULL, 0,
			uSamplerVariable, ALCHEMY_NULL, 0, pNumInfo[i], uSubAddress, 1);

		FragmentShaderDefiner.AppendInstruction(ICompiler::ADD, uOccVariable, uSubAddress, 3,
			uSamplerVariable, uSubAddress, 3, uOccVariable, uSubAddress, 3);
	}

	FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uOccVariable, uSubAddress, 3,
		uOccVariable, uSubAddress, 3, uLightInfo, &uSubAddress[EXPOSURE_INDEX], 1);

	FragmentShaderDefiner.AppendInstruction(ICompiler::MOV, uOutputColor, uOccVariable, 0);

	Code.AddPass( VertexShaderDefiner.EndDefine(), FragmentShaderDefiner.EndDefine() );

	CEffectCode::CPassDefiner Pass(Code);
	CPosition2DEffect Pos2DEffect;
	CTextureEffect	  TextureEffect;
	CTextureEffect	  Texture1Effect(1);
	CEffectInfoAddition	Add(CEffectInfoAddition::DESTINATION_ALPHA);

	Pass.AddInfo(Pos2DEffect, ALCHEMY_NULL);
	Pass.AddInfo(TextureEffect, ALCHEMY_NULL);
	Pass.AddInfo(Texture1Effect, &Add);
	Pass.EndDefine();

	m_pEffect = m_pSceneManager->GetRenderSystem().CreateEffect(Code);
	m_pRenderMethod = m_pSceneManager->CreateRenderMethod(Code);

	if(!m_pEffect || !m_pRenderMethod)
	{
		ALCHEMY_DEBUG_DELETE_ARRAY(pNumInfo);
		return false;
	}

	ALCHEMY_DEBUG_DELETE_ARRAY(pNumInfo);

	CSceneManager::IFullScreenDrawer* pFullScreenDrawer = m_pSceneManager->GetFullScreenDrawer();
	if(pFullScreenDrawer)
		m_uUserData[0] = pFullScreenDrawer->AddUserData( m_OccTexture, m_pRenderMethod, m_pMaterial->GetResourceHandle(), 0, ALCHEMY_NULL);

	if(!m_uUserData[0])
		return false;

	m_uUserData[1] = pFullScreenDrawer->AddUserData( 0, m_pRenderMethod, m_pMaterial->GetResourceHandle(), 0, ALCHEMY_NULL);
	if(!m_uUserData[1])
		return false;

	m_pMaterial->AddTexture(m_OffScreenTexture);

	return true;
}

void CVolumeLightShow::Destroy()
{
	if(m_pEffect)
	{
		m_pEffect->Release();
		m_pEffect = ALCHEMY_NULL;
	}

	if(m_pRenderMethod)
	{
		m_pRenderMethod->Release();
		m_pRenderMethod = ALCHEMY_NULL;
	}

	if(!m_pSceneManager)
		m_pSceneManager = CSceneManager::GetInstance();

	CRenderSystem& RenderSystem = m_pSceneManager->GetRenderSystem();
	LPTEXTURE pTexture;

	if(m_OffScreenTexture)
	{
		pTexture = static_cast<LPTEXTURE>( RenderSystem.GetResource(m_OffScreenTexture, CGraphicsResource::TEXTURE) );
		if(pTexture)
			pTexture->Release();
	}

	if(m_OccTexture)
	{
		pTexture = static_cast<LPTEXTURE>( RenderSystem.GetResource(m_OccTexture, CGraphicsResource::TEXTURE) );
		if(pTexture)
			pTexture->Release();
	}

	if(m_pMaterial)
	{
		m_pMaterial->Release();
		m_pMaterial = ALCHEMY_NULL;
	}

	m_pSceneManager = ALCHEMY_NULL;
}

bool CVolumeLightShow::ApplyForRender()
{
	if(m_pSceneManager && m_pEffect)
	{
		m_bIsBeginRender = true;

		CSceneManager::IFullScreenDrawer* pFullScreenDrawer = m_pSceneManager->GetFullScreenDrawer();

		//compute light position
		CRenderQueue::RENDERENTRY& ComputeEntry = m_pSceneManager->GetRenderQueue().LockRenderEntry(*this);
		ComputeEntry.uRenderTarget              = 0;
		ComputeEntry.uEffect                    = 0;
		ComputeEntry.uRenderPass                = 0;
		ComputeEntry.uVertexDeclaration         = 0;
		ComputeEntry.uModel                     = 0;
		ComputeEntry.uModelParameter            = 0;
		ComputeEntry.uUserData                  = 0;
		m_pSceneManager->GetRenderQueue().UnlockRenderEntry( CRenderQueue::BEFOREHAND, 0);

		//Post Process
		CRenderQueue::RENDERENTRY& FullRenderEntry = m_pSceneManager->GetRenderQueue().LockRenderEntry( *pFullScreenDrawer );
		FullRenderEntry.uRenderTarget              = m_OffScreenTexture;
		FullRenderEntry.uEffect                    = m_pEffect->GetResourceHandle();
		FullRenderEntry.uRenderPass                = 0;
		FullRenderEntry.uVertexDeclaration         = 0;
		FullRenderEntry.uModel                     = 0;
		FullRenderEntry.uModelParameter            = 0;
		FullRenderEntry.uUserData                  = m_uUserData[0];
		m_pSceneManager->GetRenderQueue().UnlockRenderEntry( CRenderQueue::FULL_SCREEN );

		//Add
		CRenderQueue::RENDERENTRY& SceneRenderEntry = m_pSceneManager->GetRenderQueue().LockRenderEntry( *pFullScreenDrawer );
		SceneRenderEntry.uRenderTarget              = 0;
		SceneRenderEntry.uEffect                    = m_pEffect->GetResourceHandle();
		SceneRenderEntry.uRenderPass                = 1;
		SceneRenderEntry.uVertexDeclaration         = 0;
		SceneRenderEntry.uModel                     = 0;
		SceneRenderEntry.uModelParameter            = 0;
		SceneRenderEntry.uUserData                  = m_uUserData[1];
		m_pSceneManager->GetRenderQueue().UnlockRenderEntry( CRenderQueue::FULL_SCREEN );

		return true;
	}

	return false;
}

void CVolumeLightShow::Render(const CRenderQueue::RENDERENTRY& Entry, const CRenderQueue::RENDERKEY& Key, UINT uFlag)
{
	if(m_pLight && m_pSceneManager)
	{
		static FLOAT s_fOldDensity = m_fScatterWeight;
		CCamera& Camera = m_pSceneManager->GetSceneCamera();

		m_ScreenLightPos =  m_pLight->Position;
		m_ScreenLightPos.Transform( Camera.GetViewProjectionMatrix() );

		if(m_ScreenLightPos.w != 0.0f)
		{
			m_ScreenLightPos.x /= m_ScreenLightPos.w;
			m_ScreenLightPos.y /= m_ScreenLightPos.w;
			m_ScreenLightPos.x = (m_ScreenLightPos.x + 1.0f)*0.5f;
			m_ScreenLightPos.y = (1.0f - m_ScreenLightPos.y)*0.5f;
			m_fScatterWeight = s_fOldDensity;
		}
		else
		{
			m_ScreenLightPos.x = -1.0f;
			m_ScreenLightPos.y = -1.0f;
			m_fScatterWeight = 0.0f;
		}
	}
}