#include "AlchemySkyBox.h"
#include "../Core/AlchemyUtils.h"

using namespace alchemy;

static const CVertexDeclaration::VERTEXELEMENT SKY_BOX_VERTEXELEMENT[]	= { {0,0, CVertexDeclaration::FLOAT4,CVertexDeclaration::POSITION},{1,0, CVertexDeclaration::FLOAT3,CVertexDeclaration::TEXCOORD} /*{0,16, CVertexDeclaration::FLOAT3,CVertexDeclaration::TEXCOORD}*/ };
static const UINT							   SKY_VERTEX_STRIDE		= 28;
static const UINT							   SKY_VERTEX_COUNT			= 8;
static const UINT							   SKY_INDEX_COUNT			= 36;
static const UINT							   SKY_TRIANGLE_COUNT		= 12;

struct SKY_BOX_VERTEX
{
	FLOAT x,y,z,w;
	FLOAT u,v,t;

	SKY_BOX_VERTEX(FLOAT x, FLOAT y,FLOAT z, FLOAT u,FLOAT v, FLOAT t)
	{
		this->x = x;	this->y = y;	this->z = z;	this->w = 1.0f;
		this->u = u;	this->v = v;	this->t = t;
	}
};

CSkyBox::CSkyBox()
:m_pPositionBuffer(ALCHEMY_NULL),
m_pUVBuffer(ALCHEMY_NULL),
m_pIndexBuffer(ALCHEMY_NULL),
m_pEffect(ALCHEMY_NULL),
m_pVertexDeclartion(ALCHEMY_NULL),
m_pSceneManager(ALCHEMY_NULL),
m_pTexture(ALCHEMY_NULL),
m_fWidth(0.0f),
m_fHeight(0.0f),
m_fDepth(0.0f)
{

}

CSkyBox::~CSkyBox()
{
	Destroy();
}

bool CSkyBox::Create(FLOAT fWidth, FLOAT fHeight, FLOAT fDepth, CGraphicsResource::HANDLE Texture)
{
	Destroy();
	
	m_fWidth = fWidth;
	m_fHeight = fHeight;
	m_fDepth = fDepth;
	m_pSceneManager = CSceneManager::GetInstance();

	CRenderSystem& RenderSystem = m_pSceneManager->GetRenderSystem();

	m_pTexture = static_cast<LPTEXTURE>( RenderSystem.GetResource(Texture, CGraphicsResource::TEXTURE) );
	if(!m_pTexture)
	{
		Destroy();
		return false;
	}

	m_pTexture->AddRef();

	m_pVertexDeclartion = RenderSystem.CreateVertexDeclaration( SKY_BOX_VERTEXELEMENT, 2 );


	if(!m_pVertexDeclartion)
	{
		Destroy();
		return false;
	}

	m_pPositionBuffer = RenderSystem.CreateVertexBuffer(SKY_VERTEX_COUNT, 16/*SKY_VERTEX_STRIDE*/, VERTEXBUFFER::WRITE_ONLY);
	if(!m_pPositionBuffer)
	{
		Destroy();
		return false;
	}

	m_pUVBuffer = RenderSystem.CreateVertexBuffer(SKY_VERTEX_COUNT, 12, VERTEXBUFFER::WRITE_ONLY);
	if(!m_pUVBuffer)
	{
		Destroy();
		return false;
	}

	m_pIndexBuffer  = RenderSystem.CreateIndexBuffer( SKY_INDEX_COUNT, INDEXBUFFER::WRITE_ONLY, CIndexBuffer::BIT16);
	if(!m_pIndexBuffer)
	{
		Destroy();
		return false;
	}

	if( false == __ComputeVertexes() )
		return false;

	if( false == __ComputeIndices() )
		return false;

	return __CreateRenderMethod( RenderSystem.GetComplier() );
}

bool CSkyBox::__ComputeVertexes()
{
	FLOAT halfWidth	 = m_fWidth  * 0.5f;
	FLOAT halfHeight = m_fHeight * 0.5f;
	FLOAT halfDepth	 = m_fDepth  * 0.5f;

	CFloat4* pVertexData = reinterpret_cast< CFloat4* >( m_pPositionBuffer->Lock(0) );

	pVertexData[0] = CFloat4(-halfWidth, halfHeight, halfDepth);
	pVertexData[1] = CFloat4( halfWidth, halfHeight, halfDepth);
	pVertexData[2] = CFloat4( halfWidth, halfHeight,-halfDepth);
	pVertexData[3] = CFloat4(-halfWidth, halfHeight,-halfDepth);
							 
	pVertexData[4] = CFloat4(-halfWidth,-halfHeight, halfDepth);
	pVertexData[5] = CFloat4( halfWidth,-halfHeight, halfDepth);
	pVertexData[6] = CFloat4( halfWidth,-halfHeight,-halfDepth);
	pVertexData[7] = CFloat4(-halfWidth,-halfHeight,-halfDepth);

	m_pPositionBuffer->Unlock();


	CFloat3* pUVData = reinterpret_cast< CFloat3* >( m_pUVBuffer->Lock(0) );

	pUVData[0] = CFloat3(-1.0f, 1.0f, 1.0f);
	pUVData[1] = CFloat3( 1.0f, 1.0f, 1.0f);
	pUVData[2] = CFloat3( 1.0f, 1.0f,-1.0f);
	pUVData[3] = CFloat3(-1.0f, 1.0f,-1.0f);

	pUVData[4] = CFloat3(-1.0f,-1.0f, 1.0f);
	pUVData[5] = CFloat3( 1.0f,-1.0f, 1.0f);
	pUVData[6] = CFloat3( 1.0f,-1.0f,-1.0f);
	pUVData[7] = CFloat3(-1.0f,-1.0f,-1.0f);

	m_pUVBuffer->Unlock();

	return true;
}

bool CSkyBox::__ComputeIndices()
{
	UINT16*	pIndexData = reinterpret_cast<UINT16*>( m_pIndexBuffer->Lock(0) );
	UINT16	index = 0;

	//left plane
	pIndexData[index++] = 0;	pIndexData[index++] = 7;	pIndexData[index++] = 2;
	pIndexData[index++] = 0;	pIndexData[index++] = 4;	pIndexData[index++] = 7;

	//right plane
	pIndexData[index++] = 1;	pIndexData[index++] = 6;	pIndexData[index++] = 5;
	pIndexData[index++] = 1;	pIndexData[index++] = 2;	pIndexData[index++] = 6;

	//front plane
	pIndexData[index++] = 2;	pIndexData[index++] = 7;	pIndexData[index++] = 6;
	pIndexData[index++] = 2;	pIndexData[index++] = 3;	pIndexData[index++] = 7;

	//back plane
	pIndexData[index++] = 0;	pIndexData[index++] = 5;	pIndexData[index++] = 4;
	pIndexData[index++] = 0;	pIndexData[index++] = 1;	pIndexData[index++] = 5;

	//top plane
	pIndexData[index++] = 0;	pIndexData[index++] = 2;	pIndexData[index++] = 1;
	pIndexData[index++] = 0;	pIndexData[index++] = 3;	pIndexData[index++] = 2;

	//bottom plane
	pIndexData[index++] = 4;	pIndexData[index++] = 6;	pIndexData[index++] = 7;
	pIndexData[index++] = 4;	pIndexData[index++] = 5;	pIndexData[index++] = 6;

	m_pIndexBuffer->Unlock();

	return true;
}

bool CSkyBox::__CreateRenderMethod(CCompilerInterface& Compiler)
{
	CEffectCode Code(Compiler);
	CEffectCode::CShaderParametersDefiner ShaderParametersDefiner( Code );

	ShaderParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION);
	ShaderParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT3, CVertexDeclaration::TEXCOORD);

	UINT uShaderInput = ShaderParametersDefiner.EndDefine();

	ShaderParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITIONT);
	ShaderParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT3, CVertexDeclaration::TEXCOORD);

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
	UINT uPosition = VertexFunctionDefiner.ApplyVariable( CEffectCode::VECTOR4_VARIABLE, 1 );

	VertexFunctionDefiner.AppendInstruction( CCompilerInterface::M44, uPosition, Code.GetAddressVariable( uVertexShaderInput, 0), uWorldViewProjMatrix);

	UINT subAddress[] = {0, 1, 3, 3};
	VertexFunctionDefiner.AppendInstruction(CCompilerInterface::MOV, uOutputPosition, ALCHEMY_NULL, 0, uPosition, subAddress, 4, 0, ALCHEMY_NULL, 0);

	UINT texCoordAddress[] = {0, 1, 2, 3};
	VertexFunctionDefiner.AppendInstruction( CCompilerInterface::MOV, Code.GetAddressVariable(uVertexShaderOutput, 1), texCoordAddress, 4, Code.GetAddressVariable(uVertexShaderInput, 1), texCoordAddress, 4, 0, ALCHEMY_NULL, 0 );


	CEffectCode::CShaderFunctionDefiner FragmentFunctionDefiner(Code, false);

	ShaderParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT4, CVertexDeclaration::COLOR);

	UINT uTexCoord = Code.GetAddressVariable( FragmentFunctionDefiner.BeginFunction(uShaderVarying), 1), uOutputColor = Code.GetAddressVariable( FragmentFunctionDefiner.EndFunction( ShaderParametersDefiner.EndDefine() ), 0 );

	CEffectCode::SAMPLERDECLARATION SamplerDeclaration;

	SamplerDeclaration.Type     = CEffectCode::TEXTURE_CUBE;
	SamplerDeclaration.AddressU = CEffectCode::CLAMP;
	SamplerDeclaration.AddressV = CEffectCode::CLAMP;
	SamplerDeclaration.AddressW = CEffectCode::CLAMP;

	SamplerDeclaration.MinFilter = CEffectCode::LINEAR;
	SamplerDeclaration.MagFilter = CEffectCode::LINEAR;
	SamplerDeclaration.MipFilter = CEffectCode::LINEAR;

	UINT uSampler = Code.AddSamplerVariable(SamplerDeclaration, 0);
	FragmentFunctionDefiner.AppendInstruction(CCompilerInterface::TEX, uOutputColor, uTexCoord, uSampler );

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
		m_uEffectParameters[EFFECT_PARAMETER_WORLD_VIEW_PROJECTION] = pVariable->uRegister >> ByteAlignmentShift;
	else
	{
		ALCHEMY_DEBUG_WARNING("CSkyBox::Create: Create effect fail.");
		Destroy();
		return false;
	}

	pVariable = Code.GetVariable(uSampler);
	if(pVariable)
		m_uEffectParameters[EFFECT_PARAMETER_TEXTURE] = pVariable->uRegister;
	else
	{
		ALCHEMY_DEBUG_WARNING("CSkyBox::Create: Create effect fail.");
		Destroy();
		return false;
	}

	return true;
}

bool CSkyBox::ApplyForRender()
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

	//RenderQueue.UnlockRenderEntry(Entry);

	return true;
}

void CSkyBox::Render(const CRenderQueue::RENDERENTRY& Entry, UINT uFlag)
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
		const CCamera& Camera = m_pSceneManager->GetCamera();
		const FLOAT* pCameraPosition = Camera.GetWorldMatrix().fPosition[3];

		static CMatrix4x4 s_Matrix;/* (
								   1.0f, 0.0f, 0.0f, 0.0f,
								   0.0f, 1.0f, 0.0f, 0.0f,
								   0.0f, 0.0f, 1.0f, 0.0f,
								   pCameraPosition[0], pCameraPosition[1], pCameraPosition[2], 1.0f);*/
		s_Matrix.Identity();
		s_Matrix._41 = pCameraPosition[0];	s_Matrix._42 = pCameraPosition[1];	s_Matrix._43 = pCameraPosition[2];

		Multiply( s_Matrix, s_Matrix, m_WorldMatrix);
		Multiply( s_Matrix, Camera.GetViewProjectionMatrix(), s_Matrix );

		m_pEffect->SetMatrix(   CEffectInterface::VERTEX_SHADER  , m_uEffectParameters[EFFECT_PARAMETER_WORLD_VIEW_PROJECTION],			s_Matrix                       );
		m_pEffect->SetTexture(  CEffectInterface::FRAGMENT_SHADER, m_uEffectParameters[EFFECT_PARAMETER_TEXTURE]                     ,  m_pTexture->GetResourceHandle() );

		m_pEffect->CommitChange();
	}

	m_pSceneManager->GetRenderSystem().SetStreamSource( m_pPositionBuffer->GetResourceHandle(), 0 );
	m_pSceneManager->GetRenderSystem().SetStreamSource( m_pUVBuffer->GetResourceHandle(), 1 );

	m_pSceneManager->GetRenderSystem().SetIndices( m_pIndexBuffer->GetResourceHandle() );

	CRenderSystem& RenderSystem = m_pSceneManager->GetRenderSystem();
	RenderSystem.RegisterRenderState();
	RenderSystem.SetRenderState(CRenderSystem::SRC_BLEND, CRenderSystem::SRC_ALPHA);
	RenderSystem.SetRenderState(CRenderSystem::DEST_BLEND, CRenderSystem::INV_SRC_ALPHA);
	RenderSystem.SetRenderState(CRenderSystem::DEPTH_FUNC, CRenderSystem::LESS_EQUAL);

	m_pSceneManager->GetRenderSystem().DrawIndexedTriangles(CIndexBuffer::TRIANGLE_LIST, 0, SKY_TRIANGLE_COUNT);

	RenderSystem.RestoreRenderState();
}

void CSkyBox::Destroy()
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

	m_fWidth = 0.0f;
	m_fHeight = 0.0f;
	m_fDepth = 0.0f;
}