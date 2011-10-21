#include "AlchemySkydome.h"
#include "AlchemyMeshManager.h"
#include "AlchemyTextureEffect.h"
#include "AlchemyBackgroundEffect.h"

using namespace alchemy;


CSkyDome::CSkyDome()
:m_pMesh(ALCHEMY_NULL),
m_pTexture(ALCHEMY_NULL),
m_pEffect(ALCHEMY_NULL),
m_ppOffsetTextures(ALCHEMY_NULL),
m_pOffsetTextureParameters(ALCHEMY_NULL),
m_uOffsetTextureCount(0),
m_fUVOffsets(0.0f, 0.0f),
m_fUVOffsetSpeed(0.003f, 0.003f),
m_uMeshData(0)
{

}

CSkyDome::~CSkyDome()
{
	Destroy();
}

bool CSkyDome::Create(FLOAT fRadius, FLOAT fPhiDelta, FLOAT fThetaDelta, 
					  CGraphicsResource::HANDLE textureHandle,CGraphicsResource::HANDLE* pOffsetTextures, UINT uOffsetTexturesNum)
{
	m_pSceneManager = CSceneManager::GetInstance();
	
	m_fUVOffsets.x = m_fUVOffsets.y = 0.0f;

	m_pTexture = static_cast<LPTEXTURE>( m_pSceneManager->GetRenderSystem().GetResource( textureHandle, CGraphicsResource::TEXTURE ) );
	if(!m_pTexture)
	{
		Destroy();
		return false;
	}

	m_pTexture->AddRef();

	CTexture::TYPE textureType = m_pTexture->GetTextureType();

	CVertexDeclaration::VERTEXELEMENT vertexElement[2] = { {0,0, CVertexDeclaration::FLOAT4,CVertexDeclaration::POSITION}, {1,0, CVertexDeclaration::FLOAT3,CVertexDeclaration::TEXCOORD} };
	bool hasOffsetTextures = false;
	if(textureType == CTexture::NORMAL)
	{
		vertexElement[1].Type = CVertexDeclaration::FLOAT2;

		if(pOffsetTextures != ALCHEMY_NULL && uOffsetTexturesNum > 0)
		{
			hasOffsetTextures = true;
			m_uOffsetTextureCount = uOffsetTexturesNum;
			ALCHEMY_DEBUG_NEW( m_ppOffsetTextures, LPTEXTURE[m_uOffsetTextureCount] );
			for (UINT i=0; i<m_uOffsetTextureCount; ++i)
			{
				m_ppOffsetTextures[i] = static_cast<LPTEXTURE>( m_pSceneManager->GetRenderSystem().GetResource( pOffsetTextures[i], CGraphicsResource::TEXTURE ) );
				m_ppOffsetTextures[i]->AddRef();
			}
		}
	}

	UINT vertexUsages[] = {VERTEXBUFFER::WRITE_ONLY, VERTEXBUFFER::WRITE_ONLY};

	m_uMeshData = CMeshManager::GetInstance().CreateSkyDome(fRadius, fPhiDelta, fThetaDelta,vertexElement, vertexUsages, 2, INDEXBUFFER::WRITE_ONLY);

	if(m_uMeshData == 0)
	{
		Destroy();
		return false;
	}

	m_pMesh = m_pSceneManager->CreateMesh( *( CMeshManager::GetInstance().GetMeshData(m_uMeshData) ) );
	if(!m_pMesh)
	{
		Destroy();
		return false;
	}

	//Create Effect
	{
		CEffectCode Code( m_pSceneManager->GetRenderSystem().GetComplier() );

		CEffectCode::CShaderParametersDefiner ShaderParametersDefiner( Code );

		ShaderParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION);
		if(textureType == CTexture::NORMAL)
			ShaderParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT2, CVertexDeclaration::TEXCOORD);
		else
			ShaderParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT3, CVertexDeclaration::TEXCOORD);

		UINT uShaderInput = ShaderParametersDefiner.EndDefine();

		ShaderParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITIONT);
		if(textureType == CTexture::NORMAL)
			ShaderParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT2, CVertexDeclaration::TEXCOORD);
		else
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

		if(textureType == CTexture::NORMAL)
		{
			SamplerDeclaration.Type     = CEffectCode::TEXTURE2D;
			SamplerDeclaration.AddressU = CEffectCode::WRAP;
			SamplerDeclaration.AddressV = CEffectCode::WRAP;
			SamplerDeclaration.AddressW = CEffectCode::WRAP;
		}
		else
		{
			SamplerDeclaration.Type     = CEffectCode::TEXTURE_CUBE;
			SamplerDeclaration.AddressU = CEffectCode::CLAMP;
			SamplerDeclaration.AddressV = CEffectCode::CLAMP;
			SamplerDeclaration.AddressW = CEffectCode::CLAMP;
		}

		SamplerDeclaration.MinFilter = CEffectCode::LINEAR;
		SamplerDeclaration.MagFilter = CEffectCode::LINEAR;
		SamplerDeclaration.MipFilter = CEffectCode::LINEAR;

		UINT uSampler = Code.AddSamplerVariable(SamplerDeclaration, 0);
		UINT* pOffsetSamplers = ALCHEMY_NULL;
		UINT uUVOffset = 0;
		if(hasOffsetTextures)
		{
			UINT uSourceTexColor = FragmentFunctionDefiner.ApplyVariable(CEffectCode::VECTOR4_VARIABLE, 1 );
			FragmentFunctionDefiner.AppendInstruction(CCompilerInterface::TEX, uSourceTexColor, uTexCoord, uSampler );

			CEffectCode::GLOBALVARIABLEKEY UVOffsetKey;
			UVOffsetKey.GlobalType = CEffectCode::NORMAL_VARIABLE;
			UVOffsetKey.ShaderType = CEffectInterface::VERTEX_SHADER;
			UVOffsetKey.uDetail    = CRenderMethodInterface::UV_OFFSET;
			UVOffsetKey.uCount     = 1;
			UVOffsetKey.pValue     = ALCHEMY_NULL;

			uUVOffset = Code.AddGlobalVariable(UVOffsetKey);

			UINT uTmpUVOffset = FragmentFunctionDefiner.ApplyVariable(CEffectCode::VECTOR2_VARIABLE, 1);

			FragmentFunctionDefiner.AppendInstruction(CCompilerInterface::ADD, uTmpUVOffset, ALCHEMY_NULL, 0, uTexCoord, ALCHEMY_NULL, 0, uUVOffset, ALCHEMY_NULL, 0);

			UINT uBlendColor = FragmentFunctionDefiner.ApplyVariable( CEffectCode::VECTOR4_VARIABLE, 1);
			pOffsetSamplers = new UINT[m_uOffsetTextureCount];
			for (UINT i=0; i<m_uOffsetTextureCount; ++i)
			{
				SamplerDeclaration.Type     = CEffectCode::TEXTURE2D;
				SamplerDeclaration.AddressU = CEffectCode::WRAP;
				SamplerDeclaration.AddressV = CEffectCode::WRAP;
				SamplerDeclaration.AddressW = CEffectCode::WRAP;
				SamplerDeclaration.MinFilter = CEffectCode::LINEAR;
				SamplerDeclaration.MagFilter = CEffectCode::LINEAR;
				SamplerDeclaration.MipFilter = CEffectCode::LINEAR;

				pOffsetSamplers[i] = Code.AddSamplerVariable(SamplerDeclaration, i+1);

				FragmentFunctionDefiner.AppendInstruction(CCompilerInterface::TEX, uBlendColor, uTmpUVOffset, pOffsetSamplers[i] );

				UINT subAddress[] = {0,1,2,3};
				FragmentFunctionDefiner.AppendInstruction(CCompilerInterface::MUL, uBlendColor, subAddress, 3, uBlendColor, subAddress,3, uBlendColor, &subAddress[3], 1);
				FragmentFunctionDefiner.AppendInstruction(CCompilerInterface::ADD, uSourceTexColor, uSourceTexColor, uBlendColor);
			}

			FragmentFunctionDefiner.AppendInstruction(CCompilerInterface::MOV, uOutputColor, uSourceTexColor, 0);
		}
		else
			FragmentFunctionDefiner.AppendInstruction(CCompilerInterface::TEX, uOutputColor, uTexCoord, uSampler );

		Code.AddPass( VertexFunctionDefiner.EndDefine(), FragmentFunctionDefiner.EndDefine() );

		m_pEffect = m_pSceneManager->GetRenderSystem().CreateEffect( Code );

		if(!m_pEffect)
		{
			Destroy();
			return false;
		}

		UINT ByteAlignmentShift = m_pSceneManager->GetRenderSystem().GetComplier().GetByteAlignmentShift();
		const CEffectCode::VARIABLE* pVariable;

		pVariable = Code.GetVariable(uWorldViewProjMatrix);
		if(pVariable)
			m_uEffectParameters[EFFECT_PARAMETER_WORLD_VIEW_PROJECTION] = pVariable->uRegister >> ByteAlignmentShift;
		else
		{
			ALCHEMY_DEBUG_WARNING("CSkyDome::Create: Create effect fail.");
			Destroy();
			return false;
		}

		pVariable = Code.GetVariable(uSampler);
		if(pVariable)
			m_uEffectParameters[EFFECT_PARAMETER_TEXTURE] = pVariable->uRegister;
		else
		{
			ALCHEMY_DEBUG_WARNING("CSkyDome::Create: Create effect fail.");
			Destroy();
			return false;
		}

		if(hasOffsetTextures)
		{
			ALCHEMY_DEBUG_NEW( m_pOffsetTextureParameters, UINT[m_uOffsetTextureCount] );
			 
			for (UINT i=0; i<m_uOffsetTextureCount; ++i)
			{
				pVariable = Code.GetVariable( pOffsetSamplers[i] );
				if(pVariable)
					m_pOffsetTextureParameters[i] = pVariable->uRegister;
			}

			ALCHEMY_DEBUG_DELETE_ARRAY(pOffsetSamplers);

			pVariable = Code.GetVariable( uUVOffset );
			if(pVariable)
				m_uEffectParameters[EFFECT_PARAMETER_UV_OFFSET] = pVariable->uRegister >> ByteAlignmentShift;
		}
	}

	return true;
}

void CSkyDome::Update()
{
	CSceneNode::Update();

	m_fUVOffsets += m_fUVOffsetSpeed;
}

bool CSkyDome::ApplyForRender()
{
	if( !__IsValuable() )
		return false;

	CRenderQueue::RENDERENTRY& Entry = m_pSceneManager->LockRenderEntry(*this);

	Entry.uRenderTarget    = 0;
	Entry.uEffect            = m_pEffect->GetResourceHandle();
	Entry.uRenderPass        = 0;
	Entry.uVertexDeclaration = m_pMesh->GetVertex().GetVertexDeclaration().GetResourceHandle();
	Entry.uModel             = m_pMesh->GetResourceHandle();
	Entry.uModelParameter    = 0;

	Entry.uUserData          = 0;

	m_pSceneManager->UnlockRenderEntry(CSceneManager::EFFECT_NORMAL);
	//RenderQueue.UnlockRenderEntry(Entry);

	return true;
}


void CSkyDome::Render(const CRenderQueue::RENDERENTRY& Entry, UINT uFlag)
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

		if(m_pOffsetTextureParameters)
		{
			m_pEffect->SetParameter( CEffectInterface::FRAGMENT_SHADER, m_uEffectParameters[EFFECT_PARAMETER_UV_OFFSET],	static_cast<const FLOAT*>(m_fUVOffsets), 2);
			for (UINT i=0; i<m_uOffsetTextureCount; ++i)
				m_pEffect->SetTexture(  CEffectInterface::FRAGMENT_SHADER, m_pOffsetTextureParameters[i],  m_ppOffsetTextures[i]->GetResourceHandle() );
		}

		m_pEffect->CommitChange();
	}

	CRenderSystem& RenderSystem = m_pSceneManager->GetRenderSystem();
	RenderSystem.RegisterRenderState();
	RenderSystem.SetRenderState(CRenderSystem::SRC_BLEND, CRenderSystem::SRC_ALPHA);
	RenderSystem.SetRenderState(CRenderSystem::DEST_BLEND, CRenderSystem::INV_SRC_ALPHA);
	RenderSystem.SetRenderState(CRenderSystem::DEPTH_FUNC, CRenderSystem::EQUAL);

	m_pMesh->DrawSubset(0, ALCHEMY_TEST_BIT(uFlag, CRenderQueue::MODEL), ALCHEMY_TEST_BIT(uFlag, CRenderQueue::VERTEX_DECLARATION));

	RenderSystem.RestoreRenderState();
}

void CSkyDome::Destroy()
{
	if(m_uMeshData != 0)
	{
		CMeshManager::GetInstance().DestroyMeshData(m_uMeshData);
		m_uMeshData = 0;
	}

	if(m_pEffect != ALCHEMY_NULL)
	{
		m_pEffect->Release();
		m_pEffect = ALCHEMY_NULL;
	}

	if(m_pMesh != ALCHEMY_NULL)
	{
		m_pMesh->Release();
		m_pMesh = ALCHEMY_NULL;
	}

	if(m_pTexture != ALCHEMY_NULL)
	{
		m_pTexture->Release();
		m_pTexture = ALCHEMY_NULL;
	}

	if(m_pOffsetTextureParameters != ALCHEMY_NULL)
	{
		ALCHEMY_DEBUG_DELETE_ARRAY(m_pOffsetTextureParameters);
		m_pOffsetTextureParameters = ALCHEMY_NULL;
	}

	for (UINT i=0; i<m_uOffsetTextureCount; ++i)
		m_ppOffsetTextures[i]->Release();

	ALCHEMY_DEBUG_DELETE_ARRAY( m_ppOffsetTextures );
}