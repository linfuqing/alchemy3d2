#include "AlchemyBillboard.h"
#include "AlchemyTextureEffect.h"
#include "../Math/AlchemyFloat2.h"

using namespace alchemy;

static const CVertexDeclaration::VERTEXELEMENT BILLBOARD_VERTEXELEMENT[]	= { {0,0, CVertexDeclaration::FLOAT4,CVertexDeclaration::POSITION}, {1,0, CVertexDeclaration::FLOAT2,CVertexDeclaration::TEXCOORD} };

CBillboard::CBillboard()
:m_pSceneManager(ALCHEMY_NULL),
m_pPositionBuffer(ALCHEMY_NULL),
m_pUVBuffer(ALCHEMY_NULL),
m_pTexture(ALCHEMY_NULL),
m_pEffect(ALCHEMY_NULL),
m_pVertexDeclartion(ALCHEMY_NULL),
m_uEffectParameter(0),
m_uDrawStart(0),
m_uDrawTriangleCount(0)
{
}

CBillboard::~CBillboard()
{
	Destroy();
}

bool CBillboard::Create(FLOAT fSpriteWidth, FLOAT fSpriteHeight, UINT uNumSprite, CGraphicsResource::HANDLE Texture, bool bIsDynamic)
{
	m_pSceneManager = CSceneManager::GetInstance();
	CRenderSystem& RenderSystem = m_pSceneManager->GetRenderSystem();

	m_pTexture = static_cast<LPTEXTURE>( RenderSystem.GetResource(Texture, CGraphicsResource::TEXTURE) );
	if(!m_pTexture)
	{
		Destroy();
		return false;
	}

	m_pTexture->AddRef();

	if(!__InitSprites(fSpriteWidth, fSpriteHeight, uNumSprite, bIsDynamic))
		return false;

	return __InitEffect( RenderSystem.GetComplier() );
}

bool CBillboard::ApplyForRender()
{
	if( !__IsValuable() )
		return false;

	CRenderQueue::RENDERENTRY& Entry = m_pSceneManager->LockRenderEntry(*this);

	Entry.uRenderTarget      = 0;
	Entry.uEffect            = m_pEffect->GetResourceHandle();
	Entry.uRenderPass        = 0;
	Entry.uVertexDeclaration = m_pVertexDeclartion->GetResourceHandle();
	Entry.uModelParameter    = 0;

	Entry.uUserData          = 0;

	m_pSceneManager->UnlockRenderEntry(CSceneManager::EFFECT_NORMAL);

	return true;
}

void CBillboard::Render(const CRenderQueue::RENDERENTRY& Entry, UINT uFlag)
{
	if( !__IsValuable() )
		return;

	if( ALCHEMY_TEST_BIT(uFlag, CRenderQueue::EFFECT) && !m_pEffect->Begin() )
		return;

	if( ALCHEMY_TEST_BIT(uFlag, CRenderQueue::EFFECT_PASS) )
	{
		if( !m_pEffect->BeginPass(Entry.uRenderPass) )
			return;
	}

	if( ALCHEMY_TEST_BIT(uFlag, CRenderQueue::VERTEX_DECLARATION) && ( !Entry.uVertexDeclaration || !m_pSceneManager->GetRenderSystem().SetVertexDeclaration(Entry.uVertexDeclaration) ) )
		return;

	if( ALCHEMY_TEST_BIT(uFlag, CRenderQueue::PARENT) || ALCHEMY_TEST_BIT(uFlag, CRenderQueue::EFFECT_PASS) )
	{
		const CCamera& camera = m_pSceneManager->GetCamera();
		static CMatrix4x4 s_matCamWorld, s_Matrix;
		s_matCamWorld = camera.GetWorldMatrix();

		s_Matrix._11 = s_matCamWorld._11;	s_Matrix._12 = s_matCamWorld._12;	s_Matrix._13 = s_matCamWorld._13;	s_Matrix._14 = 0.0f;
		s_Matrix._21 = s_matCamWorld._21;	s_Matrix._22 = s_matCamWorld._22;	s_Matrix._23 = s_matCamWorld._23;	s_Matrix._24 = 0.0f;
		s_Matrix._31 = s_matCamWorld._31;	s_Matrix._32 = s_matCamWorld._32;	s_Matrix._33 = s_matCamWorld._33;	s_Matrix._34 = 0.0f;
		s_Matrix._41 = m_WorldMatrix._41;	s_Matrix._42 = m_WorldMatrix._42;	s_Matrix._43 = m_WorldMatrix._43;	s_Matrix._44 = 1.0f;

		Multiply(s_Matrix, camera.GetViewProjectionMatrix(), s_Matrix);

		m_pEffect->SetMatrix(   CEffectInterface::VERTEX_SHADER  , m_uEffectParameter,			s_Matrix);
		m_pEffect->SetTexture(  CEffectInterface::FRAGMENT_SHADER, 0,  m_pTexture->GetResourceHandle() );

		m_pEffect->CommitChange();
	}

	m_pSceneManager->GetRenderSystem().SetStreamSource( m_pPositionBuffer->GetResourceHandle(), 0 );
	m_pSceneManager->GetRenderSystem().SetStreamSource( m_pUVBuffer->GetResourceHandle(), 1 );
	m_pSceneManager->GetRenderSystem().SetIndices( m_pIndexBuffer->GetResourceHandle() );


	CRenderSystem& RenderSystem = m_pSceneManager->GetRenderSystem();
	
	RenderSystem.RegisterRenderState();
	RenderSystem.SetRenderState(CRenderSystem::SRC_BLEND, CRenderSystem::SRC_ALPHA);
	RenderSystem.SetRenderState(CRenderSystem::DEST_BLEND, CRenderSystem::INV_SRC_ALPHA);
	
	m_pSceneManager->GetRenderSystem().DrawIndexedTriangles(CIndexBuffer::TRIANGLE_LIST, m_uDrawStart, m_uDrawTriangleCount);

	RenderSystem.RestoreRenderState();
}

void CBillboard::Update()
{
	CSceneNode::Update();
}

void CBillboard::Destroy()
{
	if(m_pPositionBuffer)
	{
		m_pPositionBuffer->Release();

		m_pPositionBuffer = ALCHEMY_NULL;
	}

	if(m_pUVBuffer)
	{
		m_pUVBuffer->Release();

		m_pUVBuffer = ALCHEMY_NULL;
	}

	if(m_pIndexBuffer)
	{
		m_pIndexBuffer->Release();

		m_pIndexBuffer = ALCHEMY_NULL;
	}

	if(m_pVertexDeclartion)
	{
		m_pVertexDeclartion->Release();

		m_pVertexDeclartion = ALCHEMY_NULL;
	}

	if(m_pEffect)
	{
		m_pEffect->Release();

		m_pEffect = ALCHEMY_NULL;
	}

	if(m_pTexture)
	{
		m_pTexture->Release();

		m_pTexture = ALCHEMY_NULL;
	}

	m_pSceneManager = ALCHEMY_NULL;
}

bool CBillboard::__InitSprites(FLOAT fSpriteWidth, FLOAT fSpriteHeight, UINT uNumSprite, bool bIsDynamic)
{
	CRenderSystem& RenderSystem = m_pSceneManager->GetRenderSystem();

	m_pVertexDeclartion = RenderSystem.CreateVertexDeclaration( BILLBOARD_VERTEXELEMENT, 2);
	if(!m_pVertexDeclartion)
	{
		Destroy();
		return false;
	}

	UINT uVertexUsage = bIsDynamic == true ? VERTEXBUFFER::WRITE_ONLY | VERTEXBUFFER::DYNAMIC : VERTEXBUFFER::WRITE_ONLY;

	m_pPositionBuffer = RenderSystem.CreateVertexBuffer( 4*uNumSprite, 16, uVertexUsage );
	if(!m_pPositionBuffer)
	{
		Destroy();
		return false;
	}

	FLOAT fHalfWidth = m_fSpriteWidth/2.0f, fHalfHeight = m_fSpriteHeight/2.0f;
	CFloat4* pPosData = reinterpret_cast<CFloat4*>( m_pPositionBuffer->Lock(0) );

	for (UINT i=0; i< uNumSprite; ++i)
	{
		pPosData[i*4] = CFloat4(-fHalfWidth, fHalfHeight, 0.0f);
		pPosData[i*4+1] = CFloat4( fHalfWidth, fHalfHeight, 0.0f);
		pPosData[i*4+2] = CFloat4( fHalfWidth,-fHalfHeight, 0.0f);
		pPosData[i*4+3] = CFloat4(-fHalfWidth,-fHalfHeight, 0.0f);
	}

	m_pPositionBuffer->Unlock();

	m_pUVBuffer = RenderSystem.CreateVertexBuffer(4*uNumSprite, 8, VERTEXBUFFER::WRITE_ONLY);
	if(!m_pUVBuffer)
	{
		Destroy();
		return false;
	}

	CFloat2* pUVData = reinterpret_cast<CFloat2*>( m_pUVBuffer->Lock(0) );
	for (UINT i=0; i< uNumSprite; ++i)
	{
		pUVData[i*4]   = CFloat2( 0.0f, 0.0f);
		pUVData[i*4+1] = CFloat2( 1.0f, 0.0f);
		pUVData[i*4+2] = CFloat2( 1.0f, 1.0f);
		pUVData[i*4+3] = CFloat2( 0.0f, 1.0f);
	}
	m_pUVBuffer->Unlock();

	UINT uIndicesCount = uNumSprite*6;
	UINT uIndexFormat = INDEXBUFFER::BIT16;

	if(uIndicesCount > 0xffff)
		uIndexFormat = INDEXBUFFER::BIT32;

	m_pIndexBuffer = RenderSystem.CreateIndexBuffer(uIndicesCount, INDEXBUFFER::WRITE_ONLY, (CIndexBuffer::FORMAT)uIndexFormat);
	if(!m_pIndexBuffer)
	{
		Destroy();
		return false;
	}

	m_uDrawStart = 0;
	m_uDrawTriangleCount = uIndicesCount/3;

	UINT16* pIndexData = reinterpret_cast<UINT16*>( m_pIndexBuffer->Lock(0) );
	for (UINT i=0; i< uNumSprite; ++i)
	{
		pIndexData[i*6] = 0;
		pIndexData[i*6+1] = 1;
		pIndexData[i*6+2] = 2;

		pIndexData[i*6+3] = 0;
		pIndexData[i*6+4] = 2;
		pIndexData[i*6+5] = 3;
	}
	m_pIndexBuffer->Unlock();

	return true;
}

bool CBillboard::__InitEffect(CCompilerInterface& Compiler)
{
	CEffectCode Code( m_pSceneManager->GetRenderSystem().GetComplier() );
	CEffectCode::CShaderParametersDefiner ShaderParametersDefiner( Code );

	ShaderParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION);
	ShaderParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT2, CVertexDeclaration::TEXCOORD);

	UINT uShaderInput = ShaderParametersDefiner.EndDefine();

	ShaderParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITIONT);
	ShaderParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT2, CVertexDeclaration::TEXCOORD);

	UINT uShaderVarying = ShaderParametersDefiner.EndDefine();

	CEffectCode::CShaderFunctionDefiner VertexFunctionDefiner(Code, true);

	UINT uVertexShaderInput = VertexFunctionDefiner.BeginFunction( uShaderInput ), uVertexShaderOutput = VertexFunctionDefiner.EndFunction( uShaderVarying );
	UINT uOutputPosition = Code.GetAddressVariable( uVertexShaderOutput, 0 );

	CEffectCode::GLOBALVARIABLEKEY WorldViewProjectionMatrix;
	WorldViewProjectionMatrix.GlobalType = CEffectCode::MATRIX_VARIABLE;
	WorldViewProjectionMatrix.ShaderType = CEffectInterface::VERTEX_SHADER;
	WorldViewProjectionMatrix.uDetail    = CRenderMethod::WORLD_VIEW_PROJECTION_MATRIX;
	WorldViewProjectionMatrix.uCount     = 1;
	WorldViewProjectionMatrix.pValue     = ALCHEMY_NULL;

	UINT uWorldViewProjMatrix = Code.AddGlobalVariable( WorldViewProjectionMatrix );

	VertexFunctionDefiner.AppendInstruction( CCompilerInterface::M44, uOutputPosition, Code.GetAddressVariable( uVertexShaderInput, 0), uWorldViewProjMatrix);

	VertexFunctionDefiner.AppendInstruction( CCompilerInterface::MOV, Code.GetAddressVariable(uVertexShaderOutput, 1), ALCHEMY_NULL, 0, Code.GetAddressVariable(uVertexShaderInput, 1), ALCHEMY_NULL, 0, 0, ALCHEMY_NULL, 0 );

	CEffectCode::CShaderFunctionDefiner FragmentFunctionDefiner(Code, false);

	ShaderParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT4, CVertexDeclaration::COLOR);

	UINT uTexCoord = Code.GetAddressVariable( FragmentFunctionDefiner.BeginFunction(uShaderVarying), 1), uOutputColor = Code.GetAddressVariable( FragmentFunctionDefiner.EndFunction( ShaderParametersDefiner.EndDefine() ), 0 );

	CEffectCode::SAMPLERDECLARATION SamplerDeclaration;

	SamplerDeclaration.Type     = CEffectCode::TEXTURE2D;
	SamplerDeclaration.AddressU = CEffectCode::WRAP;
	SamplerDeclaration.AddressV = CEffectCode::WRAP;
	SamplerDeclaration.AddressW = CEffectCode::WRAP;

	SamplerDeclaration.MinFilter = CEffectCode::LINEAR;
	SamplerDeclaration.MagFilter = CEffectCode::LINEAR;
	SamplerDeclaration.MipFilter = CEffectCode::LINEAR;

	UINT uSampler = Code.AddSamplerVariable(SamplerDeclaration, 0);
	FragmentFunctionDefiner.AppendInstruction( CCompilerInterface::TEX, uOutputColor, uTexCoord, uSampler );

	Code.AddPass( VertexFunctionDefiner.EndDefine(), FragmentFunctionDefiner.EndDefine() );

	m_pEffect = m_pSceneManager->GetRenderSystem().CreateEffect( Code );

	if(!m_pEffect)
	{
		Destroy();
		return false;
	}

	UINT ByteAlignmentShift = Compiler.GetByteAlignmentShift();
	const CEffectCode::VARIABLE* pVariable;

	pVariable = Code.GetVariable(uWorldViewProjMatrix);
	if(pVariable)
		m_uEffectParameter = pVariable->uRegister >> ByteAlignmentShift;
	else
	{
		ALCHEMY_DEBUG_WARNING("CBillboard::Create: Create effect fail.");
		Destroy();
		return false;
	}

	return true;
}