#include "AlchemyFullScreenDrawer.h"
#include "AlchemySceneManager.h"

using namespace alchemy;

CFullScreenDrawer::CFullScreenDrawer(CSceneManager& SceneManager) :
m_SceneManager(SceneManager),
m_pFullScreenPlane(ALCHEMY_NULL),
m_pEffect(ALCHEMY_NULL)
{
}

CFullScreenDrawer::~CFullScreenDrawer(void)
{
}

bool CFullScreenDrawer::Create()
{
	Destroy();

	CVertexDeclaration::VERTEXELEMENT VertexElements[2];

	CVertexDeclaration::LPVERTEXELEMENT pVertexElement = VertexElements;
	UINT uOffset = 0;

	pVertexElement->uStream = 0;
	pVertexElement->uOffset = uOffset;
	pVertexElement->Type    = CVertexDeclaration::FLOAT4;
	pVertexElement->Usage   = CVertexDeclaration::POSITIONT;

	uOffset += pVertexElement->Type;

	++ pVertexElement;

	pVertexElement->uStream = 0;
	pVertexElement->uOffset = uOffset;
	pVertexElement->Type    = CVertexDeclaration::FLOAT2;
	pVertexElement->Usage   = CVertexDeclaration::TEXCOORD;

	uOffset += pVertexElement->Type;

	UINT uVertexUsage = ALCHEMY_FLAG(INDEXBUFFER::WRITE_ONLY);

	m_pFullScreenPlane = m_SceneManager.CreateMesh(4, 6, ALCHEMY_FLAG(INDEXBUFFER::WRITE_ONLY), INDEXBUFFER::BIT16, VertexElements, 2, &uVertexUsage);

	if(!m_pFullScreenPlane)
	{
		ALCHEMY_DEBUG_WARNING("CFullScreenDrawer::Create: Full screen plane mesh has been created fail.");

		return false;
	}

	CVertex& Vertex = m_pFullScreenPlane->GetVertex();
	CVertexBuffer* pVertexBuffer = Vertex.GetVertexBuffer(0);
	if(!pVertexBuffer)
	{
		m_pFullScreenPlane->Release();
		m_pFullScreenPlane = ALCHEMY_NULL;

		ALCHEMY_DEBUG_WARNING("CFullScreenDrawer::Create: Full screen plane vertex buffer can not be get.");

		return false;
	}

	PFLOAT pfVertices = reinterpret_cast<PFLOAT>( pVertexBuffer->Lock(0) );

	if(!pfVertices)
	{
		m_pFullScreenPlane->Release();
		m_pFullScreenPlane = ALCHEMY_NULL;

		ALCHEMY_DEBUG_WARNING("CFullScreenDrawer::Create: Full screen plane vertex buffer can not be locked.");

		return false;
	}

	UINT uIndex = 0;

	pfVertices[uIndex ++] = - 1.0f;
	pfVertices[uIndex ++] = - 1.0f;
	pfVertices[uIndex ++] =   0.0f;
	pfVertices[uIndex ++] =   1.0f;

	pfVertices[uIndex ++] =   0.0f;
	pfVertices[uIndex ++] =   1.0f;

	pfVertices[uIndex ++] =   1.0f;
	pfVertices[uIndex ++] = - 1.0f;
	pfVertices[uIndex ++] =   0.0f;
	pfVertices[uIndex ++] =   1.0f;

	pfVertices[uIndex ++] =   1.0f;
	pfVertices[uIndex ++] =   1.0f;

	pfVertices[uIndex ++] = - 1.0f;
	pfVertices[uIndex ++] =   1.0f;
	pfVertices[uIndex ++] =   0.0f;
	pfVertices[uIndex ++] =   1.0f;

	pfVertices[uIndex ++] =   0.0f;
	pfVertices[uIndex ++] =   0.0f;

	pfVertices[uIndex ++] =   1.0f;
	pfVertices[uIndex ++] =   1.0f;
	pfVertices[uIndex ++] =   0.0f;
	pfVertices[uIndex ++] =   1.0f;

	pfVertices[uIndex ++] =   1.0f;
	pfVertices[uIndex ++] =   0.0f;

	pVertexBuffer->Unlock();

	INDEXBUFFER& IndexBuffer = m_pFullScreenPlane->GetIndexBuffer();
	PUINT16 puIndices = reinterpret_cast<PUINT16>( IndexBuffer.Lock(0) );

	if(!puIndices)
	{
		m_pFullScreenPlane->Release();
		m_pFullScreenPlane = ALCHEMY_NULL;

		ALCHEMY_DEBUG_WARNING("CFullScreenDrawer::Create: Full screen plane index buffer can not be locked.");

		return false;
	}

	puIndices[0] = 0;
	puIndices[1] = 1;
	puIndices[2] = 2;

	puIndices[3] = 2;
	puIndices[4] = 1;
	puIndices[5] = 3;

	IndexBuffer.Unlock();

	CMesh::ATTRIBUTERANGE AttributeRange;
	AttributeRange.uStartIndex = 0;
	AttributeRange.uIndexCount = 6;

	m_pFullScreenPlane->GetAttributeTable().Add(AttributeRange);

	CRenderSystem& RenderSystem = m_SceneManager.GetRenderSystem();

	CEffectCode EffectCode( RenderSystem.GetComplier() );
	CEffectCode::CShaderParametersDefiner ParametersDefiner(EffectCode);

	ParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITIONT);
	ParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT2, CVertexDeclaration::TEXCOORD);
	UINT uParameters = ParametersDefiner.EndDefine();

	CEffectCode::CShaderFunctionDefiner VertexShaderDefiner(EffectCode, true);
	UINT uAttribute = VertexShaderDefiner.BeginFunction(uParameters), uVaring = VertexShaderDefiner.EndFunction(uParameters);
	VertexShaderDefiner.AppendInstruction(CCompilerInterface::MOV, EffectCode.GetAddressVariable(uVaring, 0), EffectCode.GetAddressVariable(uAttribute, 0), 0);
	VertexShaderDefiner.AppendInstruction(CCompilerInterface::MOV, EffectCode.GetAddressVariable(uVaring, 1), ALCHEMY_NULL, 0, EffectCode.GetAddressVariable(uAttribute, 1), ALCHEMY_NULL, 0, 0, ALCHEMY_NULL, 0);

	CEffectCode::CShaderFunctionDefiner FragmentShaderDefiner(EffectCode, false);

	uVaring = FragmentShaderDefiner.BeginFunction(uParameters);

	CEffectCode::SAMPLERDECLARATION SampleDeclaration;

	SampleDeclaration.Type     = CEffectCode::TEXTURE2D;

	SampleDeclaration.AddressU = CEffectCode::CLAMP;
	SampleDeclaration.AddressV = CEffectCode::CLAMP;
	SampleDeclaration.AddressW = CEffectCode::CLAMP;

	SampleDeclaration.MagFilter = CEffectCode::LINEAR;
	SampleDeclaration.MipFilter = CEffectCode::LINEAR;
	SampleDeclaration.MinFilter = CEffectCode::LINEAR;

	UINT uSampler = EffectCode.AddSamplerVariable(SampleDeclaration, 0);

	ParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT4, CVertexDeclaration::COLOR);
	uParameters = ParametersDefiner.EndDefine();

	FragmentShaderDefiner.AppendInstruction(CCompilerInterface::TEX, EffectCode.GetAddressVariable(FragmentShaderDefiner.EndFunction(uParameters), 0), EffectCode.GetAddressVariable(uVaring, 1), uSampler);

	EffectCode.AddPass( VertexShaderDefiner.EndDefine(), FragmentShaderDefiner.EndDefine() );

	m_pEffect = RenderSystem.CreateEffect(EffectCode);

	return true;
}

void CFullScreenDrawer::Draw(CGraphicsResource::HANDLE Texture, UINT uFlag)
{
	if( IsValuable() )
	{
		CRenderSystem& RenderSystem = m_SceneManager.GetRenderSystem();

		if( ALCHEMY_TEST_BIT(uFlag, CRenderQueue::EFFECT) && ALCHEMY_TEST_BIT(uFlag, CRenderQueue::EFFECT_PASS) )
		{
			m_pEffect->Begin();
			m_pEffect->BeginPass(0);

			m_pEffect->SetTexture(CEffectInterface::FRAGMENT_SHADER, 0, Texture);
			m_pEffect->CommitChange();
		}

		RenderSystem.RegisterRenderState();
		RenderSystem.SetRenderState(CRenderSystem::DEPTH_WRITEE_ENABLE, ALCHEMY_FALSE);
		RenderSystem.SetRenderState(CRenderSystem::DEPTH_FUNC, CRenderSystem::ALWAYS);
		RenderSystem.SetRenderState(CRenderSystem::CULL_MODE, CRenderSystem::NONE);
		m_pFullScreenPlane->DrawSubset( 0, ALCHEMY_TEST_BIT(uFlag, CRenderQueue::MODEL), ALCHEMY_TEST_BIT(uFlag, CRenderQueue::VERTEX_DECLARATION) );
		RenderSystem.RestoreRenderState();
	}
}

void CFullScreenDrawer::Destroy()
{
	if(m_pFullScreenPlane)
	{
		m_pFullScreenPlane->Release();

		m_pFullScreenPlane = ALCHEMY_NULL;
	}

	if(m_pEffect)
	{
		m_pEffect->Release();

		m_pFullScreenPlane = ALCHEMY_NULL;
	}
}