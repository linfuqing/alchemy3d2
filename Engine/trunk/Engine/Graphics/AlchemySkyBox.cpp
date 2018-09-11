#include "AlchemySkyBox.h"
#include "AlchemySceneManager.h"

#include "../Core/AlchemyUtils.h"

using namespace alchemy;

const IEffect::SAMPLERDECLARATION CSkyBox::sm_SAMPLER_DECLARATION = 
{
	IEffect::TEXTURE_CUBE,
	IEffect::LINEAR,
	IEffect::LINEAR,
	IEffect::NONE,
	IEffect::CLAMP,
	IEffect::CLAMP,
	IEffect::CLAMP
};

static const CVertexDeclaration::VERTEXELEMENT SKY_BOX_VERTEXELEMENT[]	= { {0,0, CVertexDeclaration::FLOAT4,CVertexDeclaration::POSITION, 0},{1,0, CVertexDeclaration::FLOAT3,CVertexDeclaration::TEXCOORD, 1} /*{0,16, CVertexDeclaration::FLOAT3,CVertexDeclaration::TEXCOORD}*/ };
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
:m_fWidth(0.0f),
m_fHeight(0.0f),
m_fDepth(0.0f),
m_uCubeSamplerIndex(0)
{
}

CSkyBox::~CSkyBox()
{
	
}

bool CSkyBox::Create(FLOAT fWidth, FLOAT fHeight, FLOAT fDepth, CGraphicsResource::HANDLE Material, UINT uCubeTextureIndex)
{
	Destroy();

	m_fWidth = fWidth;
	m_fHeight = fHeight;
	m_fDepth = fDepth;

	m_uCubeSamplerIndex = uCubeTextureIndex;

	CSceneManager* pSceneManager = CSceneManager::GetInstance();

	CRenderSystem& RenderSystem = pSceneManager->GetRenderSystem();

	PUINT pVertexUsage;
	ALCHEMY_DEBUG_NEW(pVertexUsage, UINT[2]);
	for (UINT i=0; i<2; ++i)
		pVertexUsage[i] = ALCHEMY_FLAG( INDEXBUFFER::WRITE_ONLY );

	CMesh* pMesh = pSceneManager->CreateMesh(SKY_VERTEX_COUNT, SKY_INDEX_COUNT, ALCHEMY_FLAG(INDEXBUFFER::WRITE_ONLY), INDEXBUFFER::BIT16,
		SKY_BOX_VERTEXELEMENT, 2, pVertexUsage);

	CMesh::ATTRIBUTERANGE Attribute;
	Attribute.uStartIndex = 0;
	Attribute.uIndexCount = SKY_INDEX_COUNT;
	pMesh->GetAttributeTable().Add(Attribute);

	ALCHEMY_DEBUG_DELETE_ARRAY(pVertexUsage);

	if(!pMesh)
		return false;

	if( false == __ComputeVertexes(*pMesh) )
		return false;

	if( false == __ComputeIndices(*pMesh) )
		return false;

	CEffectCode Code( RenderSystem.GetCompiler() );
	if( false == __BuildEffectCode(Code) )
		return false;

	CEffect* pEffect = RenderSystem.CreateEffect(Code);
	CRenderMethod* pRenderMethod = pSceneManager->CreateRenderMethod(Code);

	if(!pEffect && !pRenderMethod)
		return false;

	return CStaticObject::Create(pMesh->GetResourceHandle(), 0, pEffect->GetResourceHandle(), pRenderMethod, 0, Material, pSceneManager);
}

bool CSkyBox::__ComputeVertexes(CMesh& Mesh)
{
	CVertex& Vertex = Mesh.GetVertex();
	LPVERTEXBUFFER pPositionBuffer = Vertex.GetVertexBuffer(0);
	LPVERTEXBUFFER pUVBuffer = Vertex.GetVertexBuffer(1);

	FLOAT halfWidth	 = m_fWidth  * 0.5f;
	FLOAT halfHeight = m_fHeight * 0.5f;
	FLOAT halfDepth	 = m_fDepth  * 0.5f;

	CFloat4* pVertexData = reinterpret_cast< CFloat4* >( pPositionBuffer->Lock(0) );

	pVertexData[0] = CFloat4(-halfWidth, halfHeight, halfDepth);
	pVertexData[1] = CFloat4( halfWidth, halfHeight, halfDepth);
	pVertexData[2] = CFloat4( halfWidth, halfHeight,-halfDepth);
	pVertexData[3] = CFloat4(-halfWidth, halfHeight,-halfDepth);
							 
	pVertexData[4] = CFloat4(-halfWidth,-halfHeight, halfDepth);
	pVertexData[5] = CFloat4( halfWidth,-halfHeight, halfDepth);
	pVertexData[6] = CFloat4( halfWidth,-halfHeight,-halfDepth);
	pVertexData[7] = CFloat4(-halfWidth,-halfHeight,-halfDepth);

	pPositionBuffer->Unlock();


	CFloat3* pUVData = reinterpret_cast< CFloat3* >( pUVBuffer->Lock(0) );

	pUVData[0] = CFloat3(-1.0f, 1.0f, 1.0f);
	pUVData[1] = CFloat3( 1.0f, 1.0f, 1.0f);
	pUVData[2] = CFloat3( 1.0f, 1.0f,-1.0f);
	pUVData[3] = CFloat3(-1.0f, 1.0f,-1.0f);

	pUVData[4] = CFloat3(-1.0f,-1.0f, 1.0f);
	pUVData[5] = CFloat3( 1.0f,-1.0f, 1.0f);
	pUVData[6] = CFloat3( 1.0f,-1.0f,-1.0f);
	pUVData[7] = CFloat3(-1.0f,-1.0f,-1.0f);

	pUVBuffer->Unlock();

	return true;
}

bool CSkyBox::__ComputeIndices(CMesh& Mesh)
{
	INDEXBUFFER& IndexBuffer = Mesh.GetIndexBuffer();
	UINT16*	pIndexData = reinterpret_cast<UINT16*>( IndexBuffer.Lock(0) );
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

	IndexBuffer.Unlock();

	return true;
}

bool CSkyBox::__BuildEffectCode(CEffectCode& Code)
{
	CEffectCode::CShaderParametersDefiner ShaderParametersDefiner( Code );

	ShaderParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION, 0);
	ShaderParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT3, CVertexDeclaration::TEXCOORD, 0);

	UINT uShaderInput = ShaderParametersDefiner.EndDefine();

	ShaderParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITIONT, 0);
	ShaderParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT3, CVertexDeclaration::TEXCOORD, 0);

	UINT uShaderVarying = ShaderParametersDefiner.EndDefine();

	CEffectCode::CShaderFunctionDefiner VertexFunctionDefiner(Code, true);

	UINT uVertexShaderInput = VertexFunctionDefiner.BeginFunction( uShaderInput ), uVertexShaderOutput = VertexFunctionDefiner.EndFunction( uShaderVarying );
	UINT uOutputPosition = Code.GetAddressVariable( uVertexShaderOutput, 0 );

	CEffectCode::GLOBALVARIABLE WorldViewProjectionMatrix;
	WorldViewProjectionMatrix.GlobalType = CEffectCode::CAMERA_VARIABLE;
	WorldViewProjectionMatrix.ShaderType = IEffect::VERTEX_SHADER;
	WorldViewProjectionMatrix.uDetail    = CRenderMethod::CAMERA_WORLD_VIEW_PROJECTION_MATRIX;
	WorldViewProjectionMatrix.uCount     = 1;
	WorldViewProjectionMatrix.uIndex     = 0;
	WorldViewProjectionMatrix.pValue     = ALCHEMY_NULL;

	UINT uWorldViewProjMatrix = Code.AddGlobalVariable( WorldViewProjectionMatrix );
	UINT uPosition = VertexFunctionDefiner.ApplyVariable( CEffectCode::VECTOR4_VARIABLE, 1 );

	VertexFunctionDefiner.AppendInstruction( ICompiler::M44, uPosition, Code.GetAddressVariable( uVertexShaderInput, 0), uWorldViewProjMatrix);

	UINT subAddress[] = {0, 1, 3, 3};
	VertexFunctionDefiner.AppendInstruction(ICompiler::MOV, uOutputPosition, ALCHEMY_NULL, 0, uPosition, subAddress, 4, 0, ALCHEMY_NULL, 0);

	VertexFunctionDefiner.AppendInstruction( ICompiler::MOV, Code.GetAddressVariable(uVertexShaderOutput, 1), ALCHEMY_NULL, 0, Code.GetAddressVariable(uVertexShaderInput, 1), ALCHEMY_NULL, 0, 0, ALCHEMY_NULL, 0);

	CEffectCode::CShaderFunctionDefiner FragmentFunctionDefiner(Code, false);

	ShaderParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT4, CVertexDeclaration::COLOR, 0);

	UINT uTexCoord = Code.GetAddressVariable( FragmentFunctionDefiner.BeginFunction(uShaderVarying), 1), uOutputColor = Code.GetAddressVariable( FragmentFunctionDefiner.EndFunction( ShaderParametersDefiner.EndDefine() ), 0 );

	UINT uSampler = FragmentFunctionDefiner.AddSamplerVariable(sm_SAMPLER_DECLARATION, m_uCubeSamplerIndex);
	FragmentFunctionDefiner.AppendInstruction(ICompiler::TEX, uOutputColor, uTexCoord, uSampler );

	Code.AddPass( VertexFunctionDefiner.EndDefine(), FragmentFunctionDefiner.EndDefine() );

	return true;
}

void CSkyBox::Render(const CRenderQueue::RENDERENTRY& Entry, const CRenderQueue::RENDERKEY& Key, UINT uFlag)
{
	CMesh* pMesh = GetMesh();
	IRenderMethod* pRenderMethod = GetRenderMethod();
	CSceneManager* pSceneManager = GetSceneManager();
	if(!pMesh || !pRenderMethod || !pSceneManager)
		return;

	CRenderQueue& RenderQueue = pSceneManager->GetRenderQueue();
	CRenderSystem& RenderSystem = pSceneManager->GetRenderSystem();

	CMaterial* pMaterial = GetMaterial();
	if(pMaterial && pRenderMethod)
	{
		UINT uTextureCount;
		const UINT* pTextureAlphaIndexes = pRenderMethod->GetTextureInfo(Key.uRenderPass, Key.uRenderTargetIndex, uTextureCount);
		if(pTextureAlphaIndexes)
		{
			const LPTEXTURE* ppTexture;
			for(UINT i = 0; i < uTextureCount; ++ i)
			{
				ppTexture = pMaterial->GetTextures().Get( pTextureAlphaIndexes[i] );

				if( ppTexture && *ppTexture && ALCHEMY_TEST_BIT( (*ppTexture)->GetUsage(), CTexture::RENDER_TARGET ) && (*ppTexture)->GetResourceHandle() == RenderQueue.GetRenderTarget() )
					return;
			}
		}
	}

	if(ALCHEMY_TEST_BIT(uFlag, CRenderQueue::PARENT) || ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_EFFECT) || ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_EFFECT_PASS) )
	{
		if( ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_EFFECT) && Entry.uEffect )
			RenderSystem.SetShader(Entry.uEffect);

		if( ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_EFFECT_PASS) && Entry.uRenderPass )
			RenderSystem.SetShader(Entry.uRenderPass);
	}

	if(pRenderMethod && !pRenderMethod->Activate(Key.uRenderPass, Key.uRenderTargetIndex) )
		return;

	const CCamera* pCamera = pSceneManager->GetCamera(0);
	const MATRIX4X4& CameraWorldMatrix = pCamera->GetWorldMatrix();

	Copy(m_SkyMatrix, GetWorldMatrix());
	m_SkyMatrix._41 = CameraWorldMatrix._41;
	m_SkyMatrix._42 = CameraWorldMatrix._42;
	m_SkyMatrix._43 = CameraWorldMatrix._43;

	RenderSystem.ResetTexture();

	static const IRenderState::RENDERSTATEINFO s_RENDER_STATE_INFOS[] = 
	{
		{IRenderState::DEPTH_WRITEE_ENABLE, ALCHEMY_FALSE}, 
		{IRenderState::DEPTH_FUNC, CRenderSystem::LESS_EQUAL}, 
		{CRenderSystem::CULL_MODE, CRenderSystem::CULL_NONE}
	};

	RenderSystem.RegisterRenderState();

	RenderSystem.SetRenderState(s_RENDER_STATE_INFOS, 3, true);

	if(pRenderMethod)
		pRenderMethod->CommitChange( &m_SkyMatrix, pMaterial);

	if( ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_VERTEX_DECLARATION) && ( !Entry.uVertexDeclaration || !RenderSystem.SetVertexDeclaration(Entry.uVertexDeclaration) ) )
		return;

	pMesh->DrawSubset( Entry.uModelParameter, ALCHEMY_TEST_BIT(uFlag, CRenderQueue::MODEL), ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_VERTEX_DECLARATION) && !Entry.uVertexDeclaration);

	RenderSystem.RestoreRenderState();
}