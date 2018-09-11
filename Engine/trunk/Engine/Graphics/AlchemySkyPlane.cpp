#include "AlchemySkyPlane.h"
#include "AlchemySceneManager.h"

using namespace alchemy;

const IEffect::SAMPLERDECLARATION CSkyPlane::sm_SAMPLER_DECLARATION = 
{
	IEffect::TEXTURE_CUBE,
	IEffect::LINEAR,
	IEffect::LINEAR,
	IEffect::LINEAR,
	IEffect::WRAP,
	IEffect::WRAP,
	IEffect::WRAP
};

CSkyPlane::CSkyPlane(void) :
m_UVOffsetSpeed(0.1f, 0.1f),
m_UVOffset(0.0f, 0.0f),
m_Emissive(0.0f, 0.0f, 0.0f, 1.0f)
{
}

CSkyPlane::~CSkyPlane(void)
{

}

bool CSkyPlane::Create(
					   FLOAT fLength,
					   FLOAT fWidth,
					   FLOAT fHeight,
					   CGraphicsResource::HANDLE Texture, 
					   bool bIsAmbientLight,
					   bool bIsEmissive,
					   bool bIsUVOffset)
{
	Destroy();

	CSceneManager* pSceneManager = CSceneManager::GetInstance();

	if(!pSceneManager)
	{
		ALCHEMY_DEBUG_WARNING("CSkyPlane::Create:  Sky plane can not be created without a activate scene manager.");

		return false;
	}

	m_UVOffset.x = m_UVOffset.y = 0.0f;

	static const CVertexDeclaration::VERTEXELEMENT s_VERTEX_ELEMENTS[2] = { {0, 0, CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION, 0}, {0, 16, CVertexDeclaration::FLOAT2, CVertexDeclaration::TEXCOORD, 1} };

	CFloat3 PositonOffset(0.0f, fHeight, 0.0f);
	UINT uVertexUsage = ALCHEMY_FLAG(VERTEXBUFFER::WRITE_ONLY), uIndexUsage = ALCHEMY_FLAG(INDEXBUFFER::WRITE_ONLY);

	CMesh* pMesh = ALCHEMY_NULL;
	UINT uMeshData = ALCHEMY_MESH_MANAGER.CreatePlane(fLength, fWidth, 1, 1, 1, 1, true, s_VERTEX_ELEMENTS, 2, &PositonOffset, &uVertexUsage, uIndexUsage);
	if(uMeshData)
	{
		const CMeshManager::MESHDATA* pMeshData = ALCHEMY_MESH_MANAGER.GetMeshData(uMeshData);

		if(pMeshData)
			pMesh = pSceneManager->CreateMesh(uMeshData);
		
		ALCHEMY_MESH_MANAGER.DestroyMeshData(uMeshData);
	}

	CRenderSystem& RenderSystem = pSceneManager->GetRenderSystem();
	if(pMesh)
	{
		CTexture* pTexture = static_cast<CTexture*>( RenderSystem.GetResource(Texture, CGraphicsResource::TEXTURE) );

		if(pTexture)
		{
			CMaterial* pMaterial = pSceneManager->CreateMaterial();

			if(pMaterial)
			{
				pMaterial->AddTexture(pTexture->GetResourceHandle());

				CEffectCode EffectCode( RenderSystem.GetCompiler() );

				CEffectCode::CShaderParametersDefiner ShaderParametersDefiner(EffectCode);

				ShaderParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION, 0);
				ShaderParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT2, CVertexDeclaration::TEXCOORD, 0);

				UINT uShaderInput = ShaderParametersDefiner.EndDefine();

				ShaderParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITIONT, 0);
				ShaderParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT2, CVertexDeclaration::TEXCOORD, 0);

				UINT uShaderVarying = ShaderParametersDefiner.EndDefine();

				CEffectCode::CShaderFunctionDefiner VertexFunctionDefiner(EffectCode, true);

				UINT uVertexShaderInput = VertexFunctionDefiner.BeginFunction(uShaderInput), uVertexShaderOutput = VertexFunctionDefiner.EndFunction(uShaderVarying), uOutputPosition = EffectCode.GetAddressVariable(uVertexShaderOutput, 0);

				CEffectCode::GLOBALVARIABLE WorldViewProjectionMatrixKey;
				WorldViewProjectionMatrixKey.GlobalType = CEffectCode::CAMERA_VARIABLE;
				WorldViewProjectionMatrixKey.ShaderType = IEffect::VERTEX_SHADER;
				WorldViewProjectionMatrixKey.uDetail    = CRenderMethod::CAMERA_WORLD_VIEW_PROJECTION_MATRIX;
				WorldViewProjectionMatrixKey.uCount     = 1;
				WorldViewProjectionMatrixKey.uIndex     = 0;
				WorldViewProjectionMatrixKey.pValue     = ALCHEMY_NULL;

				UINT uWorldViewProjectionMatrix = EffectCode.AddGlobalVariable(WorldViewProjectionMatrixKey);

				UINT uPosition = VertexFunctionDefiner.ApplyVariable(CEffectCode::VECTOR4_VARIABLE, 1);

				VertexFunctionDefiner.AppendInstruction(ICompiler::M44, uPosition, EffectCode.GetAddressVariable(uVertexShaderInput, 0), uWorldViewProjectionMatrix);

				const static UINT s_uADDRESS[] = {0, 1, 3, 3};

				VertexFunctionDefiner.AppendInstruction(ICompiler::MOV, uOutputPosition, ALCHEMY_NULL, 0, uPosition, s_uADDRESS, 4, 0, ALCHEMY_NULL, 0);

				UINT uUVOffset = 0;
				if(bIsUVOffset)
				{
					CEffectCode::GLOBALVARIABLE UVOffsetKey;
					UVOffsetKey.GlobalType = CEffectCode::NORMAL_VARIABLE;
					UVOffsetKey.ShaderType = IEffect::VERTEX_SHADER;
					UVOffsetKey.uDetail    = IRenderMethod::UV_OFFSET;
					UVOffsetKey.uCount     = 1;
					UVOffsetKey.uIndex     = 0;
					UVOffsetKey.pValue     = ALCHEMY_NULL;

					uUVOffset = EffectCode.AddGlobalVariable(UVOffsetKey);

					VertexFunctionDefiner.AppendInstruction(ICompiler::ADD, EffectCode.GetAddressVariable(uVertexShaderOutput, 1), ALCHEMY_NULL, 0, EffectCode.GetAddressVariable(uVertexShaderInput, 1), ALCHEMY_NULL, 0, uUVOffset, ALCHEMY_NULL, 0);
				}
				else
					VertexFunctionDefiner.AppendInstruction(ICompiler::MOV, EffectCode.GetAddressVariable(uVertexShaderOutput, 1), ALCHEMY_NULL, 0, EffectCode.GetAddressVariable(uVertexShaderInput, 1), ALCHEMY_NULL, 0, 0        , ALCHEMY_NULL, 0);

				ShaderParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT4, CVertexDeclaration::COLOR, 0);

				CEffectCode::CShaderFunctionDefiner FragmentFunctionDefiner(EffectCode, false);

				UINT uTexcoord = EffectCode.GetAddressVariable(FragmentFunctionDefiner.BeginFunction(uShaderVarying), 1), 
					uOutputColor = EffectCode.GetAddressVariable(FragmentFunctionDefiner.EndFunction( ShaderParametersDefiner.EndDefine() ), 0);

				UINT uSampler = FragmentFunctionDefiner.AddSamplerVariable(sm_SAMPLER_DECLARATION, 0);

				UINT uAmbientLight = 0, uEmissive = 0;
				if(bIsAmbientLight || bIsEmissive)
				{
					UINT uTemp = FragmentFunctionDefiner.ApplyVariable(CEffectCode::VECTOR4_VARIABLE, 1);
					FragmentFunctionDefiner.AppendInstruction(ICompiler::TEX, uTemp, uTexcoord, uSampler);

					if(bIsAmbientLight)
					{
						CEffectCode::GLOBALVARIABLE AmbientLightKey;
						AmbientLightKey.GlobalType = CEffectCode::NORMAL_VARIABLE;
						AmbientLightKey.ShaderType = IEffect::FRAGMENT_SHADER;
						AmbientLightKey.uDetail    = IRenderMethod::AMBIENT_LIGHT;
						AmbientLightKey.uCount     = 1;
						AmbientLightKey.uIndex     = 0;
						AmbientLightKey.pValue     = ALCHEMY_NULL;

						uAmbientLight = EffectCode.AddGlobalVariable(AmbientLightKey);

						FragmentFunctionDefiner.AppendInstruction(ICompiler::MUL, bIsEmissive ? uTemp : uOutputColor, uTemp, uAmbientLight);
					}

					if(bIsEmissive)
					{
						CEffectCode::GLOBALVARIABLE EmissiveKey;
						EmissiveKey.GlobalType = CEffectCode::MATERIAL_VARIABLE;
						EmissiveKey.ShaderType = IEffect::FRAGMENT_SHADER;
						EmissiveKey.uDetail    = CRenderMethod::MATERIAL_EMISSIVE;
						EmissiveKey.uCount     = 1;
						EmissiveKey.uIndex     = 0;
						EmissiveKey.pValue     = ALCHEMY_NULL;

						uEmissive = EffectCode.AddGlobalVariable(EmissiveKey);

						FragmentFunctionDefiner.AppendInstruction(ICompiler::ADD, uOutputColor, uTemp, uEmissive);
					}
				}
				else
					FragmentFunctionDefiner.AppendInstruction(ICompiler::TEX, uOutputColor, uTexcoord, uSampler);

				EffectCode.AddPass( VertexFunctionDefiner.EndDefine(), FragmentFunctionDefiner.EndDefine() );

				CEffect* pEffect = pSceneManager->GetRenderSystem().CreateEffect(EffectCode);
				CRenderMethod* pRenderMethod = pSceneManager->CreateRenderMethod(EffectCode);

				return CStaticObject::Create(pMesh->GetResourceHandle(), 0, pEffect->GetResourceHandle(), pRenderMethod, 0, pMaterial->GetResourceHandle(), pSceneManager);
			}
		}
		else
		{
			ALCHEMY_DEBUG_WARNING("CSkyPlane::Create: Error texture handle.");

			return false;
		}
	}

	ALCHEMY_DEBUG_WARNING("CSkyPlane::Create: create mesh fail.");

	return false;
}

void CSkyPlane::Update()
{
	CStaticObject::Update();

	m_UVOffset += m_UVOffsetSpeed;
}

void CSkyPlane::Render(const CRenderQueue::RENDERENTRY& Entry, const CRenderQueue::RENDERKEY& Key, UINT uFlag)
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