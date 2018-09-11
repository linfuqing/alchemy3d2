#include "AlchemyWater.h"
#include "AlchemySceneManager.h"
//#include "AlchemyViewportRenderTargetListener.h"
#include "AlchemyAttributeWorldSpaceEffect.h"
#include "AlchemyTextureEffect.h"
#include "AlchemyNormalOffsetForceTextureEffect.h"
#include "AlchemyFacingEffect.h"
#include "AlchemyAlphaBlendMethod.h"
//#include "AlchemyEffectInfoAddition.h"
#include "AlchemyEffectInfoModulation.h"
//#include "AlchemyBlinnPhongEffect.h"
//#include "AlchemyWaterBlendMethod.h"

using namespace alchemy;

CWater::CRefractionMapRenderer::CRefractionMapRenderer(CWater& Water) :
m_Water(Water)
{
}

CWater::CRefractionMapRenderer::~CRefractionMapRenderer(void)
{
}

bool CWater::CRefractionMapRenderer::ApplyForRender()
{
	CSceneManager* pSceneManager = m_Water.GetSceneManager();
	if(!pSceneManager || !m_Water.m_pRefractionMap)
		return false;

	CRenderQueue& RenderQueue = pSceneManager->GetRenderQueue();
	CRenderQueue::RENDERENTRY& RenderEntry = RenderQueue.LockRenderEntry(*this);
	RenderEntry.uRenderTarget   = m_Water.m_pRefractionMap->GetResourceHandle();
	RenderEntry.uEffect         = 0;
	RenderEntry.uRenderPass     = 0;
	RenderEntry.uModel          = 0;
	RenderEntry.uModelParameter = 0;
	RenderEntry.uUserData       = 0;

	return RenderQueue.UnlockRenderEntry( CRenderQueue::BEFOREHAND, ALCHEMY_FLAG(CRenderQueue::AUTO_SHADER) );
}

void CWater::CRefractionMapRenderer::Render(const CRenderQueue::RENDERENTRY& Entry, const CRenderQueue::RENDERKEY& Key, UINT uFlag)
{
	CSceneManager* pSceneManager = m_Water.GetSceneManager();
	if(!pSceneManager || !m_Water.m_pRefractionMap)
		return;

	//CRenderSystem& RenderSystem = pSceneManager->GetRenderSystem();
	//RenderSystem.SetRenderState(CRenderSystem::COLOR_WRITE_ENABLE, ALCHEMY_FLAG(CRenderSystem::RED) | ALCHEMY_FLAG(CRenderSystem::GREEN) | ALCHEMY_FLAG(CRenderSystem::BLUE) );

	CRenderQueue& RenderQueue = pSceneManager->GetRenderQueue();
	if( ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_RENDER_TARGET) )
	{
		RenderQueue.SetRenderTarget(m_Water.m_pRefractionMap->GetResourceHandle(), 0);

		pSceneManager->RenderScene( ALCHEMY_FLAG(CRenderSystem::TARGET) | ALCHEMY_FLAG(CRenderSystem::ZBUFFER) | ALCHEMY_FLAG(CRenderSystem::STENCIL) );
	}
	else
		pSceneManager->RenderScene(0);

	//RenderSystem.SetRenderState(CRenderSystem::COLOR_WRITE_ENABLE, ALCHEMY_FLAG(CRenderSystem::ALPHA) | ALCHEMY_FLAG(CRenderSystem::RED) | ALCHEMY_FLAG(CRenderSystem::GREEN) | ALCHEMY_FLAG(CRenderSystem::BLUE) );
}

CWater::CRefractionMapMaskRenderer::CRefractionMapMaskRenderer(CWater& Water) :
m_Water(Water),
m_pEffect(ALCHEMY_NULL),
m_pVertexDeclaration(ALCHEMY_NULL),
m_uWorldViewProjectionMatrix(0),
m_uZero(0)
{
}

CWater::CRefractionMapMaskRenderer::~CRefractionMapMaskRenderer(void)
{
}

bool CWater::CRefractionMapMaskRenderer::Create()
{
	Destroy();

	CSceneManager* pSceneManager = m_Water.GetSceneManager();
	if(!pSceneManager)
		return false;

	CMesh* pPlaneMesh = m_Water.GetPlaneMesh();
	if(!pPlaneMesh)
		return false;

	CRenderSystem& RenderSystem = pSceneManager->GetRenderSystem();
	ICompiler& Compiler = RenderSystem.GetCompiler();
	CEffectCode EffectCode(Compiler);
	CEffectCode::CShaderParametersDefiner ShaderParametersDefiner(EffectCode);
	ShaderParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION, 0);
	UINT uPosition = ShaderParametersDefiner.EndDefine();

	ShaderParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITIONT, 0);
	UINT uPositiont = ShaderParametersDefiner.EndDefine();

	CEffectCode::GLOBALVARIABLE Constant;
	const CEffectCode::VARIABLE* pVarialbe;

	CEffectCode::CShaderFunctionDefiner VertexShader(EffectCode, true);
	Constant.ShaderType = IEffect::VERTEX_SHADER;
	Constant.GlobalType = CEffectCode::CAMERA_VARIABLE;
	Constant.uDetail    = CRenderMethod::CAMERA_VIEW_PROJECTION_MATRIX;
	Constant.uCount     = 1;
	Constant.uIndex     = 0;
	Constant.pValue     = ALCHEMY_NULL;
	m_uWorldViewProjectionMatrix = EffectCode.AddGlobalVariable(Constant);
	VertexShader.AppendInstruction(ICompiler::M44, EffectCode.GetAddressVariable(VertexShader.EndFunction(uPositiont), 0), EffectCode.GetAddressVariable(VertexShader.BeginFunction(uPosition), 0), m_uWorldViewProjectionMatrix);
	pVarialbe = EffectCode.GetVariable(m_uWorldViewProjectionMatrix);
	if(!pVarialbe)
	{
		ALCHEMY_DEBUG_WARNING("CWater::CRefractionMapMaskRenderer::Create: Variable world view projection can not be get.");

		return false;
	}

	m_uWorldViewProjectionMatrix = Compiler.ComputeRegisterLocation(pVarialbe->uRegister, 0, 0, 0, 0);

	ShaderParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT4, CVertexDeclaration::COLOR, 0);
	UINT uColor = ShaderParametersDefiner.EndDefine();

	CEffectCode::CShaderFunctionDefiner FragmentShader(EffectCode, false);
	FragmentShader.BeginFunction(uPositiont);

	Constant.ShaderType = IEffect::FRAGMENT_SHADER;
	Constant.GlobalType = CEffectCode::CONSTANT_VARIABLE;
	Constant.uDetail    = CRenderMethod::CONSTANT_ZERO;
	Constant.uCount     = 1;
	Constant.uIndex     = 0;
	Constant.pValue     = ALCHEMY_NULL;
	m_uZero = EffectCode.AddGlobalVariable(Constant);
	FragmentShader.AppendInstruction(ICompiler::MOV, EffectCode.GetAddressVariable(FragmentShader.EndFunction(uColor), 0), m_uZero, 0);
	pVarialbe = EffectCode.GetVariable(m_uZero);
	if(!pVarialbe)
	{
		ALCHEMY_DEBUG_WARNING("CWater::CRefractionMapMaskRenderer::Create: Variable zero can not be get.");

		return false;
	}
	m_uZero = EffectCode.GetCompiler().ComputeRegisterLocation(pVarialbe->uRegister, EffectCode.GetRegisterCount(CEffectCode::MATRIX4X4_VARIABLE), 0, 0, 0);

	EffectCode.AddPass( VertexShader.EndDefine(), FragmentShader.EndDefine() );

	m_pEffect = RenderSystem.CreateEffect(EffectCode);

	if(!m_pEffect)
	{
		ALCHEMY_DEBUG_WARNING("CWater::CRefractionMapMaskRenderer::Create: Effect has been created fail.");

		Destroy();

		return false;
	}

	CVertexDeclaration::ELEMENT VertexElement;
	VertexElement.Type        = CVertexDeclaration::FLOAT4;
	VertexElement.Usage       = CVertexDeclaration::POSITION;
	VertexElement.uUsageIndex = 0;
	VertexElement.uIndex      = 0;

	m_pVertexDeclaration = pPlaneMesh->GetVertex().GetVertexDeclaration().Clone(&VertexElement, 1);
	if(!m_pVertexDeclaration)
	{
		ALCHEMY_DEBUG_WARNING("CWater::CRefractionMapMaskRenderer::Create: Vertex declaration has been created fail.");

		Destroy();

		return false;
	}

	return true;
}

bool CWater::CRefractionMapMaskRenderer::ApplyForRender()
{
	CSceneManager* pSceneManager = m_Water.GetSceneManager();
	CMesh* pPlaneMesh = m_Water.GetPlaneMesh();
	if(!pSceneManager || !pPlaneMesh || !m_Water.m_pRefractionMap || !m_pEffect || !m_pVertexDeclaration)
		return false;

	CRenderQueue& RenderQueue = pSceneManager->GetRenderQueue();
	CRenderQueue::RENDERENTRY& RenderEntry = RenderQueue.LockRenderEntry(*this);
	RenderEntry.uRenderTarget      = m_Water.m_pRefractionMap->GetResourceHandle();
	RenderEntry.uEffect            = m_pEffect->GetResourceHandle();
	RenderEntry.uRenderPass        = 0;
	RenderEntry.uVertexDeclaration = m_pVertexDeclaration->GetResourceHandle();
	RenderEntry.uModel             = pPlaneMesh->GetResourceHandle();
	RenderEntry.uModelParameter    = 0;
	RenderEntry.uUserData          = 0;

	return RenderQueue.UnlockRenderEntry( CRenderQueue::BEFOREHAND, ALCHEMY_FLAG(CRenderQueue::AUTO_SHADER) );
}

void CWater::CRefractionMapMaskRenderer::Render(const CRenderQueue::RENDERENTRY& Entry, const CRenderQueue::RENDERKEY& Key, UINT uFlag)
{
	CSceneManager* pSceneManager = m_Water.GetSceneManager();
	CMesh* pPlaneMesh = m_Water.GetPlaneMesh();
	if(!pSceneManager || !pPlaneMesh || !m_Water.m_pRefractionMap || !m_pEffect || !m_pVertexDeclaration)
		return;

	CRenderSystem& RenderSystem = pSceneManager->GetRenderSystem();
	SetRenderState(RenderSystem, IRenderState::COLOR_WRITE_ENABLE, ALCHEMY_FLAG(IRenderState::ALPHA) );

	CRenderQueue& RenderQueue = pSceneManager->GetRenderQueue();
	if( ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_RENDER_TARGET) )
	{
		RenderQueue.SetRenderTarget(m_Water.m_pRefractionMap->GetResourceHandle(), 0);
		RenderSystem.Clear( pSceneManager->GetBackgroundColor(), ALCHEMY_FLAG(CRenderSystem::TARGET) | ALCHEMY_FLAG(CRenderSystem::ZBUFFER) | ALCHEMY_FLAG(CRenderSystem::STENCIL) );
	}

	if( ALCHEMY_TEST_BIT(uFlag, CRenderQueue::PARENT) || ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_EFFECT) || ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_EFFECT_PASS) )
	{
		if(ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_EFFECT) && Entry.uEffect)
			RenderSystem.SetShader(Entry.uEffect);

		if(ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_EFFECT_PASS) && Entry.uRenderPass)
			RenderSystem.SetShader(Entry.uRenderPass);

		RenderSystem.ResetTexture();

		static CMatrix4x4 WorldViewProjectionMatrix;
		Copy( WorldViewProjectionMatrix, m_Water.GetWorldMatrix() );
		WorldViewProjectionMatrix *= pSceneManager->GetSceneCamera().GetViewProjectionMatrix();

		SetShaderMatrix(RenderSystem, IEffect::VERTEX_SHADER, WorldViewProjectionMatrix, m_uWorldViewProjectionMatrix);

		const FLOAT fZERO = 0;
		RenderSystem.SetShaderParameters(IEffect::FRAGMENT_SHADER, m_uZero, &fZERO, 1);
	}

	if( ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_VERTEX_DECLARATION) )
		RenderSystem.SetVertexDeclaration( m_pVertexDeclaration->GetResourceHandle() );

	pPlaneMesh->DrawSubset(0, ALCHEMY_TEST_BIT(uFlag, CRenderQueue::MODEL), false);

	SetRenderState(RenderSystem, IRenderState::COLOR_WRITE_ENABLE, ALCHEMY_FLAG(IRenderState::ALPHA) | ALCHEMY_FLAG(IRenderState::RED) | ALCHEMY_FLAG(IRenderState::GREEN) | ALCHEMY_FLAG(IRenderState::BLUE) );
}

void CWater::CRefractionMapMaskRenderer::Destroy()
{
	if(m_pEffect)
	{
		m_pEffect->Release();
		m_pEffect = ALCHEMY_NULL;
	}

	if(m_pVertexDeclaration)
	{
		m_pVertexDeclaration->Release();
		m_pVertexDeclaration = ALCHEMY_NULL;
	}
}

CWater::CWater(void) :
m_pRefractionMap(ALCHEMY_NULL),
//m_pRefractionMapInfo(ALCHEMY_NULL),
m_fRefractionScale(0.02f),
m_NormalMapTranslationScale(0.01f, 0.0f),
m_RefractionMapRenderer(*this),
m_RefractionMapMaskRenderer(*this)
{
}

CWater::~CWater(void)
{
	//ALCHEMY_DEBUG_DELETE(m_pRefractionMapInfo);
}

bool CWater::Create(
					FLOAT fWidth, 
					FLOAT fHeight, 
					UINT uWidthSegment /* = 1 */, 
					UINT uHeightSegment /* = 1 */, 
					UINT uReflectionMap,
					INT nRefractionMapShift /* = 0 */, 
					UINT uNormalMap,
					CGraphicsResource::HANDLE Material /* = 0 */, 
					const CEffectCode::CPassDefiner::EFFECTINFO* pEffectInfos /* = ALCHEMY_NULL */, 
					UINT uEffectInfoCount)
{
	__Destroy();

	if( !CSceneManager::GetInstance() )
	{
		ALCHEMY_DEBUG_WARNING("CWater::Create:  Water can not be created without a activate scene manager.");

		return false;
	}

	CSceneManager& SceneManager = *CSceneManager::GetInstance();

	CMaterial* pMaterial = Material ? static_cast<CMaterial*>( SceneManager.GetResource(Material, CGraphicsResource::MATERIAL) ) : SceneManager.CreateMaterial();

	if(!pMaterial)
	{
		__Destroy();

		ALCHEMY_DEBUG_WARNING("CWater::Create:  Material has been created fail.");

		return false;
	}

	if(Material)
		pMaterial->AddRef();

	CRenderSystem& RenderSystem = SceneManager.GetRenderSystem();

	const LPTEXTURE* ppNormalMap = ALCHEMY_NULL;
	
	if(uNormalMap)
	{
		ppNormalMap = pMaterial->GetTextures().Get(uNormalMap - 1);
		if(!ppNormalMap)
		{
			LPTEXTURE pTexture = static_cast<LPTEXTURE>( RenderSystem.GetResource(uNormalMap, CGraphicsResource::TEXTURE) );

			if(pTexture)
			{
				ppNormalMap  = &pTexture;
				pMaterial->AddTexture( pTexture->GetResourceHandle() );
				uNormalMap = pMaterial->GetTextures().GetLength();
			}
		}
	}

	//ALCHEMY_DEBUG_NEW( m_pRefractionMapInfo, CViewportTextureInfo(RenderSystem) );

	//m_pRefractionMapInfo->SetShift(nRefractionMapShift);

	//m_pRefractionMap = SceneManager.GetRenderSystem().CreateTexture(*m_pRefractionMapInfo, ALCHEMY_FLAG(TEXTURE::WRITE_ONLY) | ALCHEMY_FLAG(CTexture::RENDER_TARGET), CTexture::R8G8B8A8, 1);

	//ALCHEMY_VIEWPORT_RENDER_TARGET_LISTENER.AddEventListener(m_pRefractionMap, RenderSystem, 0, CTexture::R8G8B8A8, nRefractionMapShift);
	m_pRefractionMap = RenderSystem.CreateRenderTargetView(CTexture::DEFAULT, nRefractionMapShift);
	if(!m_pRefractionMap)
	{
		__Destroy();

		ALCHEMY_DEBUG_WARNING("CWater::Create:  Refraction map has been created fail.");

		return false;
	}

	IEffect::SAMPLERDECLARATION SamplerDeclaration;
	SamplerDeclaration.Type      = IEffect::TEXTURE2D;
	SamplerDeclaration.MinFilter = IEffect::LINEAR;
	SamplerDeclaration.MagFilter = IEffect::LINEAR;
	SamplerDeclaration.MipFilter = IEffect::NONE;
	SamplerDeclaration.AddressU  = IEffect::CLAMP;
	SamplerDeclaration.AddressV  = IEffect::CLAMP;
	SamplerDeclaration.AddressW  = IEffect::CLAMP;
	CTextureEffect TextureEffect( pMaterial->GetTextures().GetLength(), 0,  &SamplerDeclaration);
	CNormalOffsetForceTextureEffect NormalOffsetTextureEffect( m_fRefractionScale, 0, pMaterial->GetTextures().GetLength() );

	pMaterial->AddTexture( m_pRefractionMap->GetResourceHandle() );

	CEffectCode::CPassDefiner::LPEFFECTINFO pCurrentEffectInfos;

	UINT uCurrentEffectInfoCount = uEffectInfoCount;
	uEffectInfoCount += 4/*5*/;

	ALCHEMY_DEBUG_NEW(pCurrentEffectInfos, CEffectCode::CPassDefiner::EFFECTINFO[uEffectInfoCount]);

	CEffectCode::CPassDefiner::LPEFFECTINFO pEffectInfo = pCurrentEffectInfos;

	CFacingEffect FacingEffect(false);

	pEffectInfo->pEffectInfo     = &FacingEffect;
	pEffectInfo->pBlendMethod    = ALCHEMY_NULL;
	pEffectInfo->uInputColorKey  = 0;
	pEffectInfo->uSourceColorKey = 0;
	pEffectInfo->uOutputColorKey = 0;

	++ pEffectInfo;

	/*CEffectInfoModulation EffectInfoModulation;
	pEffectInfo->pEffectInfo     = &m_FresnelEffect;
	pEffectInfo->pBlendMethod    = &EffectInfoModulation;
	pEffectInfo->uInputColorKey  = 0;
	pEffectInfo->uSourceColorKey = 0;
	pEffectInfo->uOutputColorKey = 0;

	++ pEffectInfo;*/

	pEffectInfo->pEffectInfo     = &TextureEffect;
	pEffectInfo->pBlendMethod    = ALCHEMY_NULL;
	pEffectInfo->uInputColorKey  = 0;
	pEffectInfo->uSourceColorKey = 0;
	pEffectInfo->uOutputColorKey = 1;

	++ pEffectInfo;

	CAlphaBlendMethod AlphaBlendMethod;

	pEffectInfo->pEffectInfo     = &NormalOffsetTextureEffect;
	pEffectInfo->pBlendMethod    = &AlphaBlendMethod;
	pEffectInfo->uInputColorKey  = 1;
	pEffectInfo->uSourceColorKey = 0;
	pEffectInfo->uOutputColorKey = 1;

	++ pEffectInfo;

	//CEffectInfoAddition EffectInfoAddition;
	pEffectInfo->pEffectInfo     = ALCHEMY_NULL;
	pEffectInfo->pBlendMethod    = &m_WaterBlendMethod;//&EffectInfoAddition;
	pEffectInfo->uInputColorKey  = 1;
	pEffectInfo->uSourceColorKey = 0;
	pEffectInfo->uOutputColorKey = 0;

	++ pEffectInfo;

	for(UINT i = 0; i < uCurrentEffectInfoCount; ++ i)
		pEffectInfo[i] = pEffectInfos[i];

	bool bResult = CMirror::Create(fWidth, fHeight, uWidthSegment, uHeightSegment, uReflectionMap, uNormalMap, pMaterial->GetResourceHandle(), pCurrentEffectInfos, uEffectInfoCount, true);

	ALCHEMY_DEBUG_DELETE_ARRAY(pCurrentEffectInfos);

	return bResult && m_RefractionMapMaskRenderer.Create();
}

void CWater::Update()
{
	CSceneManager* pSceneManager = CMirror::GetSceneManager();
	if(pSceneManager)
	{
		FLOAT fTime = pSceneManager->GetTime();

		m_Translation3.x = m_NormalMapTranslationScale.x * fTime;
		m_Translation3.y = m_NormalMapTranslationScale.y * fTime;

		m_Translation3.x = m_Translation3.x - static_cast<INT>(m_Translation3.x);
		m_Translation3.y = m_Translation3.y - static_cast<INT>(m_Translation3.y);

		m_Translation2  = m_Translation3;
		m_Translation2 *= 2.0f;
		m_Translation1  = m_Translation3;
		m_Translation1 *= 4.0f;
		m_Translation0  = m_Translation2;
	}

	CSceneNode::Update();
}

bool CWater::ApplyForRender(UINT uRenderPass, UINT uRenderList)
{
	return CMirror::ApplyForRender(uRenderPass, uRenderList) && m_RefractionMapRenderer.ApplyForRender() && m_RefractionMapMaskRenderer.ApplyForRender();
}

void CWater::Render(const CRenderQueue::RENDERENTRY& Entry, const CRenderQueue::RENDERKEY& Key, UINT uFlag)
{
	CSceneManager* pSceneManager = GetSceneManager();
	if( m_pRefractionMap && pSceneManager && m_pRefractionMap->GetResourceHandle() == pSceneManager->GetRenderQueue().GetRenderTarget() )
		return;

	CMirror::Render(Entry, Key, uFlag);
	/*CSceneManager* pSceneManager = CMirror::GetSceneManager();
	if(pSceneManager && m_pRefractionMap)
	{
		CGraphicsResource::HANDLE RenderTarget = pSceneManager->GetRenderQueue().GetRenderTarget();

		pSceneManager->GetRenderQueue().SetRenderTarget( m_pRefractionMap->GetResourceHandle() );

		pSceneManager->SetCamera( pSceneManager->GetSceneCamera() );

		pSceneManager->RenderScene( ALCHEMY_FLAG(CRenderSystem::TARGET) | ALCHEMY_FLAG(CRenderSystem::ZBUFFER) );

		pSceneManager->GetRenderQueue().SetRenderTarget(RenderTarget);
	}

	CMirror::Render(Entry, uFlag);*/
}

void CWater::Destroy()
{
	__Destroy();

	CMirror::Destroy();
}

void CWater::__Destroy()
{
	if(m_pRefractionMap)
	{
		m_pRefractionMap->Release();

		m_pRefractionMap = ALCHEMY_NULL;
	}

	//ALCHEMY_DEBUG_DELETE(m_pRefractionMapInfo);

	m_RefractionMapMaskRenderer.Destroy();
}