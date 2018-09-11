#include "AlchemySkydome.h"
#include "AlchemyMeshManager.h"
#include "AlchemySceneManager.h"

using namespace alchemy;

const IEffect::SAMPLERDECLARATION CSkyDome::sm_OFFSET_SAMPLER_DECLARATION = 
{
	IEffect::TEXTURE2D,
	IEffect::LINEAR,
	IEffect::LINEAR,
	IEffect::LINEAR,
	IEffect::WRAP,
	IEffect::WRAP,
	IEffect::WRAP
};

CSkyDome::CSkyDome()
:m_pOffsetIndicesNum(ALCHEMY_NULL),
m_uOffsetTextureCount(0),
m_fUVOffsets(0.0f, 0.0f),
m_fUVOffsetSpeed(0.003f, 0.003f),
m_uMeshData(0)
{

}

CSkyDome::~CSkyDome()
{
	ALCHEMY_DEBUG_DELETE(m_pOffsetIndicesNum);

	if(m_uMeshData != 0)
	{
		ALCHEMY_MESH_MANAGER.DestroyMeshData(m_uMeshData);
		m_uMeshData = 0;
	}
}

bool CSkyDome::Create(FLOAT fRadius, FLOAT fPhiDelta, FLOAT fThetaDelta, CGraphicsResource::HANDLE Material,
					  UINT uMainTextureIndex, UINT* pOffsetTextureIndices, UINT uOffsetNum)
{
	Destroy();

	CSceneManager* pSceneManager = CSceneManager::GetInstance();
	
	m_fUVOffsets.x = m_fUVOffsets.y = 0.0f;

	CMaterial* pMaterial = static_cast<CMaterial*>( pSceneManager->GetResource( Material, CGraphicsResource::MATERIAL ) );
	if(!pMaterial)
	{
		ALCHEMY_DEBUG_WARNING("CSkyDome::Create() - Error Material Handle!");
		return false;
	}

	const LPTEXTURE* ppMainTexture = pMaterial->GetTextures().Get(uMainTextureIndex);
	if(!ppMainTexture)
	{
		ALCHEMY_DEBUG_WARNING("CSkyDome::Create() - Error Main Texture Index!");
		return false;
	}

	CTexture::TEXTURETYPE textureType = (*ppMainTexture)->GetTextureType();

	CVertexDeclaration::VERTEXELEMENT vertexElement[2] = { {0,0, CVertexDeclaration::FLOAT4,CVertexDeclaration::POSITION, 0}, {1,0, CVertexDeclaration::FLOAT3,CVertexDeclaration::TEXCOORD, 1} };
	bool hasOffsetTextures = false;
	if(textureType == CTexture::NORMAL)
	{
		vertexElement[1].Type = CVertexDeclaration::FLOAT2;

		if(pOffsetTextureIndices != ALCHEMY_NULL && uOffsetNum > 0)
		{
			hasOffsetTextures = true;
			m_uOffsetTextureCount = uOffsetNum;
		}
	}

	UINT vertexUsages[] = { ALCHEMY_FLAG(VERTEXBUFFER::WRITE_ONLY), ALCHEMY_FLAG(VERTEXBUFFER::WRITE_ONLY)};

	m_uMeshData = ALCHEMY_MESH_MANAGER.CreateSkyDome(fRadius, fPhiDelta, fThetaDelta,vertexElement, vertexUsages, 2, ALCHEMY_FLAG(INDEXBUFFER::WRITE_ONLY));

	if(m_uMeshData == 0)
	{
		Destroy();
		ALCHEMY_DEBUG_WARNING("CSkyDome::Create() - Create Mesh Data Error!");
		return false;
	}

	CMesh* pMesh = pSceneManager->CreateMesh( m_uMeshData );
	if(!pMesh)
	{
		Destroy();
		ALCHEMY_DEBUG_WARNING("CSkyDome::Create() - Create Mesh Error!");
		return false;
	}

	//Create Effect

	CEffectCode Code( pSceneManager->GetRenderSystem().GetCompiler() );

	CEffectCode::CShaderParametersDefiner ShaderParametersDefiner( Code );

	ShaderParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION, 0);
	if(textureType == CTexture::NORMAL)
		ShaderParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT2, CVertexDeclaration::TEXCOORD, 0);
	else
		ShaderParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT3, CVertexDeclaration::TEXCOORD, 0);

	UINT uShaderInput = ShaderParametersDefiner.EndDefine();

	ShaderParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITIONT, 0);
	if(textureType == CTexture::NORMAL)
		ShaderParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT2, CVertexDeclaration::TEXCOORD, 0);
	else
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

	VertexFunctionDefiner.AppendInstruction( ICompiler::MOV, Code.GetAddressVariable(uVertexShaderOutput, 1), ALCHEMY_NULL, 0, Code.GetAddressVariable(uVertexShaderInput, 1), ALCHEMY_NULL, 0, 0, ALCHEMY_NULL, 0 );


	CEffectCode::CShaderFunctionDefiner FragmentFunctionDefiner(Code, false);

	ShaderParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT4, CVertexDeclaration::COLOR, 0);

	UINT uTexCoord = Code.GetAddressVariable( FragmentFunctionDefiner.BeginFunction(uShaderVarying), 1), uOutputColor = Code.GetAddressVariable( FragmentFunctionDefiner.EndFunction( ShaderParametersDefiner.EndDefine() ), 0 );

	IEffect::SAMPLERDECLARATION SamplerDeclaration;

	if(textureType == CTexture::NORMAL)
	{
		SamplerDeclaration.Type     = IEffect::TEXTURE2D;
		SamplerDeclaration.AddressU = IEffect::WRAP;
		SamplerDeclaration.AddressV = IEffect::WRAP;
		SamplerDeclaration.AddressW = IEffect::WRAP;
	}
	else
	{
		SamplerDeclaration.Type     = IEffect::TEXTURE_CUBE;
		SamplerDeclaration.AddressU = IEffect::CLAMP;
		SamplerDeclaration.AddressV = IEffect::CLAMP;
		SamplerDeclaration.AddressW = IEffect::CLAMP;
	}

	SamplerDeclaration.MinFilter = IEffect::LINEAR;
	SamplerDeclaration.MagFilter = IEffect::LINEAR;
	SamplerDeclaration.MipFilter = IEffect::LINEAR;

	UINT uSampler = FragmentFunctionDefiner.AddSamplerVariable(SamplerDeclaration, 0);
	UINT* pOffsetSamplers = ALCHEMY_NULL;
	UINT uUVOffset = 0;
	if(hasOffsetTextures)
	{
		UINT uSourceTexColor = FragmentFunctionDefiner.ApplyVariable(CEffectCode::VECTOR4_VARIABLE, 1 );
		FragmentFunctionDefiner.AppendInstruction(ICompiler::TEX, uSourceTexColor, uTexCoord, uSampler );

		CEffectCode::GLOBALVARIABLE UVOffsetKey;
		UVOffsetKey.GlobalType = CEffectCode::NORMAL_VARIABLE;
		UVOffsetKey.ShaderType = IEffect::FRAGMENT_SHADER;
		UVOffsetKey.uDetail    = IRenderMethod::UV_OFFSET;
		UVOffsetKey.uCount     = 1;
		UVOffsetKey.uIndex     = 0;
		UVOffsetKey.pValue     = ALCHEMY_NULL;

		uUVOffset = Code.AddGlobalVariable(UVOffsetKey);

		UINT uTmpUVOffset = FragmentFunctionDefiner.ApplyVariable(CEffectCode::VECTOR2_VARIABLE, 1);

		FragmentFunctionDefiner.AppendInstruction(ICompiler::ADD, uTmpUVOffset, ALCHEMY_NULL, 0, uTexCoord, ALCHEMY_NULL, 0, uUVOffset, ALCHEMY_NULL, 0);

		UINT uBlendColor = FragmentFunctionDefiner.ApplyVariable( CEffectCode::VECTOR4_VARIABLE, 1);
		ALCHEMY_DEBUG_NEW(pOffsetSamplers,UINT[m_uOffsetTextureCount]);
		for (UINT i=0; i<m_uOffsetTextureCount; ++i)
		{
			pOffsetSamplers[i] = FragmentFunctionDefiner.AddSamplerVariable(sm_OFFSET_SAMPLER_DECLARATION, i+1);

			FragmentFunctionDefiner.AppendInstruction(ICompiler::TEX, uBlendColor, uTmpUVOffset, pOffsetSamplers[i] );

			UINT subAddress[] = {0,1,2,3};
			FragmentFunctionDefiner.AppendInstruction(ICompiler::MUL, uBlendColor, subAddress, 3, uBlendColor, subAddress,3, uBlendColor, &subAddress[3], 1);
			FragmentFunctionDefiner.AppendInstruction(ICompiler::ADD, uSourceTexColor, uSourceTexColor, uBlendColor);
		}

		FragmentFunctionDefiner.AppendInstruction(ICompiler::MOV, uOutputColor, uSourceTexColor, 0);
	}
	else
		FragmentFunctionDefiner.AppendInstruction(ICompiler::TEX, uOutputColor, uTexCoord, uSampler );

	Code.AddPass( VertexFunctionDefiner.EndDefine(), FragmentFunctionDefiner.EndDefine() );

	CEffect* pEffect = pSceneManager->GetRenderSystem().CreateEffect( Code );
	CRenderMethod* pRenderMethod = pSceneManager->CreateRenderMethod(Code);

	ALCHEMY_DEBUG_DELETE_ARRAY(pOffsetSamplers);

	if(!pEffect || !pRenderMethod)
	{
		Destroy();
		ALCHEMY_DEBUG_WARNING("CSkyDome::Create() - Create Effect Error!");
		return false;
	}

	return CStaticObject::Create(pMesh->GetResourceHandle(), 0, pEffect->GetResourceHandle(), pRenderMethod, 0, Material, pSceneManager);
}


void CSkyDome::Update()
{
	m_fUVOffsets += m_fUVOffsetSpeed;

	CSceneNode::Update();
}

void CSkyDome::Render(const CRenderQueue::RENDERENTRY& Entry, const CRenderQueue::RENDERKEY& Key, UINT uFlag)
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

void CSkyDome::Destroy()
{
	CStaticObject::Destroy();

	ALCHEMY_DEBUG_DELETE(m_pOffsetIndicesNum);
	m_uOffsetTextureCount = 0;

	if(m_uMeshData != 0)
	{
		ALCHEMY_MESH_MANAGER.DestroyMeshData(m_uMeshData);
		m_uMeshData = 0;
	}
}