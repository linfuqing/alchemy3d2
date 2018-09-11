#include "AlchemyFullScreenDrawer.h"

using namespace alchemy;

const IEffect::SAMPLERDECLARATION CFullScreenDrawer::sm_SAMPLER_DECLARATION = 
{
	IEffect::TEXTURE2D,
	IEffect::LINEAR,
	IEffect::LINEAR,
	IEffect::NONE,
	IEffect::CLAMP,
	IEffect::CLAMP,
	IEffect::CLAMP
};

CFullScreenDrawer::CFullScreenDrawer() :
m_pSceneManager(ALCHEMY_NULL),
m_pVertexDeclaration(ALCHEMY_NULL),
m_pVertexBuffer(ALCHEMY_NULL),
m_pIndexBuffer(ALCHEMY_NULL),
m_pEffect(ALCHEMY_NULL)
{
}

CFullScreenDrawer::~CFullScreenDrawer(void)
{
}

bool CFullScreenDrawer::Create()
{
	Destroy();

	m_pSceneManager = CSceneManager::GetInstance();
	if(!m_pSceneManager)
		return false;

	CVertexDeclaration::VERTEXELEMENT VertexElements[2];

	CVertexDeclaration::LPVERTEXELEMENT pVertexElement = VertexElements;
	UINT uOffset = 0, uIndex = 0;

	pVertexElement->uStream = 0;
	pVertexElement->uOffset = uOffset;
	pVertexElement->Type    = CVertexDeclaration::FLOAT4;
	pVertexElement->Usage   = CVertexDeclaration::POSITIONT;
	pVertexElement->uIndex  = uIndex ++;

	uOffset += pVertexElement->Type;

	++ pVertexElement;

	pVertexElement->uStream = 0;
	pVertexElement->uOffset = uOffset;
	pVertexElement->Type    = CVertexDeclaration::FLOAT2;
	pVertexElement->Usage   = CVertexDeclaration::TEXCOORD;
	pVertexElement->uIndex  = uIndex ++;

	uOffset += pVertexElement->Type;

	CRenderSystem& RenderSystem = m_pSceneManager->GetRenderSystem();

	m_pVertexDeclaration = RenderSystem.CreateVertexDeclaration(VertexElements, 2);
	if(!m_pVertexDeclaration)
	{
		Destroy();

		ALCHEMY_DEBUG_WARNING("CFullScreenDrawer::Create: Vertex declaration has been created fail.");

		return false;
	}

	//UINT uVertexUsage = ALCHEMY_FLAG(INDEXBUFFER::WRITE_ONLY);

	m_pVertexBuffer = RenderSystem.CreateVertexBuffer( 4, uOffset, ALCHEMY_FLAG(INDEXBUFFER::WRITE_ONLY) );
	if(!m_pVertexBuffer)
	{
		Destroy();

		ALCHEMY_DEBUG_WARNING("CFullScreenDrawer::Create: Vertex buffer has been created fail.");

		return false;
	}

	PFLOAT pfVertices = reinterpret_cast<PFLOAT>( m_pVertexBuffer->Lock(0) );

	if(!pfVertices)
	{
		Destroy();

		ALCHEMY_DEBUG_WARNING("CFullScreenDrawer::Create: Vertex buffer can not be locked.");

		return false;
	}

	uIndex = 0;

	pfVertices[uIndex ++] = - 1.0f;
	pfVertices[uIndex ++] = - 1.0f;
	pfVertices[uIndex ++] =   1.0f;
	pfVertices[uIndex ++] =   1.0f;

	pfVertices[uIndex ++] =   0.0f;
	pfVertices[uIndex ++] =   1.0f;

	pfVertices[uIndex ++] =   1.0f;
	pfVertices[uIndex ++] = - 1.0f;
	pfVertices[uIndex ++] =   1.0f;
	pfVertices[uIndex ++] =   1.0f;

	pfVertices[uIndex ++] =   1.0f;
	pfVertices[uIndex ++] =   1.0f;

	pfVertices[uIndex ++] = - 1.0f;
	pfVertices[uIndex ++] =   1.0f;
	pfVertices[uIndex ++] =   1.0f;
	pfVertices[uIndex ++] =   1.0f;

	pfVertices[uIndex ++] =   0.0f;
	pfVertices[uIndex ++] =   0.0f;

	pfVertices[uIndex ++] =   1.0f;
	pfVertices[uIndex ++] =   1.0f;
	pfVertices[uIndex ++] =   1.0f;
	pfVertices[uIndex ++] =   1.0f;

	pfVertices[uIndex ++] =   1.0f;
	pfVertices[uIndex ++] =   0.0f;

	m_pVertexBuffer->Unlock();

	m_pIndexBuffer = RenderSystem.CreateIndexBuffer(6, ALCHEMY_FLAG(INDEXBUFFER::WRITE_ONLY), CIndexBuffer::BIT16);
	if(!m_pIndexBuffer)
	{
		Destroy();

		ALCHEMY_DEBUG_WARNING("CFullScreenDrawer::Create: Index buffer has been created fail.");

		return false;
	}

	PUINT16 puIndices = reinterpret_cast<PUINT16>( m_pIndexBuffer->Lock(0) );

	if(!puIndices)
	{
		Destroy();

		ALCHEMY_DEBUG_WARNING("CFullScreenDrawer::Create: Index buffer can not be locked.");

		return false;
	}

	puIndices[0] = 0;
	puIndices[1] = 1;
	puIndices[2] = 2;

	puIndices[3] = 3;
	puIndices[4] = 2;
	puIndices[5] = 1;

	m_pIndexBuffer->Unlock();

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

	UINT uSampler = FragmentShaderDefiner.AddSamplerVariable(sm_SAMPLER_DECLARATION, 0);

	ParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT4, CVertexDeclaration::COLOR, 0);
	uParameters = ParametersDefiner.EndDefine();

	FragmentShaderDefiner.AppendInstruction(ICompiler::TEX, EffectCode.GetAddressVariable(FragmentShaderDefiner.EndFunction(uParameters), 0), EffectCode.GetAddressVariable(uVaring, 1), uSampler);

	EffectCode.AddPass( VertexShaderDefiner.EndDefine(), FragmentShaderDefiner.EndDefine() );

	m_pEffect = RenderSystem.CreateEffect(EffectCode);
	if(!m_pEffect)
	{
		Destroy();

		ALCHEMY_DEBUG_WARNING("CFullScreenDrawer::Create: Effect has been created fail.");

		return false;
	}

	return true;
}

UINT CFullScreenDrawer::AddUserData(CGraphicsResource::HANDLE RenderTexture, IRenderMethod* pRenderMethod, CGraphicsResource::HANDLE Material, UINT uRenderTextureIndex, const IEffect::SAMPLERDECLARATION* pSamplerDeclaration)
{
	if(!m_pSceneManager)
		return 0;

	LPTEXTURE pRenderTexture = static_cast<LPTEXTURE>( m_pSceneManager->GetRenderSystem().GetResource(RenderTexture, CGraphicsResource::TEXTURE) );
	if(pRenderTexture)
	{
		if(!ALCHEMY_TEST_BIT(pRenderTexture->GetUsage(), CTexture::RENDER_TARGET) )
			return 0;
	}
	else
	{
		pRenderTexture = m_pSceneManager->GetRenderQueue().CreateSceneRenderTarget();
		RenderTexture = pRenderTexture->GetResourceHandle();
	}

	USERDATA UserData;
	UserData.RenderTexture       = RenderTexture;
	UserData.pRenderMethod       = pRenderMethod;
	UserData.Material            = Material;
	UserData.uRenderTextureIndex = uRenderTextureIndex;
	
	UserData.pSamplerDeclaration = pSamplerDeclaration ? &m_SamplerDeclarationPool[m_SamplerDeclarationPool.Add(*pSamplerDeclaration)] : &sm_SAMPLER_DECLARATION;

	return m_UserData.Add(UserData) + 1;
}

void CFullScreenDrawer::Render(const CRenderQueue::RENDERENTRY& Entry, const CRenderQueue::RENDERKEY& Key, UINT uFlag)
{
	if( IsValuable() && Entry.uUserData)
	{
		LPUSERDATA pUserData = m_UserData.Get(Entry.uUserData - 1);
		if(pUserData)
		{
			if(pUserData->RenderTexture == Entry.uRenderTarget)
				return;

			CRenderQueue& RenderQueue = m_pSceneManager->GetRenderQueue();
			if( ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_RENDER_TARGET) )
				RenderQueue.SetRenderTarget(Entry.uRenderTarget, 0, false);

			if( ( ALCHEMY_TEST_BIT(uFlag, CRenderQueue::KEY_EFFECT) || ALCHEMY_TEST_BIT(uFlag, CRenderQueue::KEY_EFFECT_PASS) ) && pUserData->pRenderMethod && !pUserData->pRenderMethod->Activate(Key.uRenderPass, Key.uRenderTargetIndex) )
				return;

			CRenderSystem& RenderSystem = m_pSceneManager->GetRenderSystem();
			if( ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_EFFECT) || ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_EFFECT_PASS) || ALCHEMY_TEST_BIT(uFlag, CRenderQueue::PARENT) )
			{
				if(Entry.uEffect || Entry.uRenderPass)
				{
					if( ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_EFFECT) && Entry.uEffect && !RenderSystem.SetShader(Entry.uEffect) )
						return;

					if( ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_EFFECT_PASS) && Entry.uRenderPass && !RenderSystem.SetShader(Entry.uRenderPass) )
						return;
				}
				else
				{
					CShader* pShader, *pAdditionShader;
					m_pEffect->GetShader(0, 0, &pShader, &pAdditionShader);

					if( ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_EFFECT) && pShader && !RenderSystem.SetShader( pShader->GetResourceHandle() ) )
						return;

					if( ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_EFFECT_PASS) && pAdditionShader && !RenderSystem.SetShader( pAdditionShader->GetResourceHandle() ) )
						return;
				}

				RenderSystem.ResetTexture();
				CGraphicsResource::HANDLE RenderTexture = pUserData->RenderTexture ? pUserData->RenderTexture : m_pSceneManager->GetRenderQueue().GetRenderTarget();
				if(pUserData->pRenderMethod)
				{
					CMaterial* pMaterial = static_cast<CMaterial*>( m_pSceneManager->GetResource(pUserData->Material, CGraphicsResource::MATERIAL) );

					if(pMaterial && pMaterial->GetTextures().GetLength() > pUserData->uRenderTextureIndex)
						pMaterial->SetTexture(RenderTexture, pUserData->uRenderTextureIndex);
					else
						RenderSystem.SetTexture(IEffect::FRAGMENT_SHADER, RenderTexture, pUserData->uRenderTextureIndex, pUserData->pSamplerDeclaration);

					pUserData->pRenderMethod->CommitChange(ALCHEMY_NULL, pMaterial);
				}
				else
					RenderSystem.SetTexture(IEffect::FRAGMENT_SHADER, RenderTexture, pUserData->uRenderTextureIndex, pUserData->pSamplerDeclaration);
			}

			static const IRenderState::RENDERSTATEINFO s_RENDER_STATE_INFOS[] = 
			{
				{IRenderState::DEPTH_WRITEE_ENABLE, ALCHEMY_FALSE}, 
				{IRenderState::DEPTH_FUNC, CRenderSystem::ALWAYS}, 
				{CRenderSystem::CULL_MODE, CRenderSystem::CULL_NONE}
			};

			RenderSystem.SetRenderState(s_RENDER_STATE_INFOS, 3, true);
			
			if(Entry.uModel)
			{
				CMesh* pMesh = static_cast<CMesh*>( m_pSceneManager->GetResource(Entry.uModel, CGraphicsResource::MESH) );
				if(pMesh)
					pMesh->DrawSubset( Entry.uModelParameter, ALCHEMY_TEST_BIT(uFlag, CRenderQueue::MODEL), ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_VERTEX_DECLARATION) );
			}
			else
			{
				if( ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_VERTEX_DECLARATION) && !RenderSystem.SetVertexDeclaration( Entry.uVertexDeclaration ? Entry.uVertexDeclaration : m_pVertexDeclaration->GetResourceHandle() ) )
					return;

				if( ALCHEMY_TEST_BIT(uFlag, CRenderQueue::MODEL) )
				{
					RenderSystem.SetStreamSource(m_pVertexBuffer->GetResourceHandle(), 0);
					RenderSystem.SetIndices( m_pIndexBuffer->GetResourceHandle() );
				}

				RenderSystem.DrawIndexedTriangles( CIndexBuffer::TRIANGLE_LIST, 0, m_pIndexBuffer->GetTriangleCount(m_pIndexBuffer->GetMemberCount(), CIndexBuffer::TRIANGLE_LIST) );
			}
			RenderSystem.RestoreRenderState();
		}
	}
}
void CFullScreenDrawer::Destroy()
{
	m_pSceneManager = ALCHEMY_NULL;

	if(m_pVertexDeclaration)
	{
		m_pVertexDeclaration->Release();

		m_pVertexDeclaration = ALCHEMY_NULL;
	}

	if(m_pVertexBuffer)
	{
		m_pVertexBuffer->Release();

		m_pVertexBuffer = ALCHEMY_NULL;
	}

	if(m_pIndexBuffer)
	{
		m_pIndexBuffer->Release();

		m_pIndexBuffer = ALCHEMY_NULL;
	}

	if(m_pEffect)
	{
		m_pEffect->Release();

		m_pEffect = ALCHEMY_NULL;
	}
}