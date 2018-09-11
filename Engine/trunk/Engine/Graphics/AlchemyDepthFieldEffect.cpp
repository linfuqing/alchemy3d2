#include "AlchemyDepthFieldEffect.h"
#include "AlchemySceneManager.h"

#include <math.h>

using namespace alchemy;

const FLOAT CDepthFieldEffect::sm_NUM_SAMPLERS = 5.0f;

static const FLOAT s_CONSTANT_ONE = 1.0f;

CDepthFieldEffect::CDepthFieldEffect()
:m_pMesh(ALCHEMY_NULL),
m_pEffect(ALCHEMY_NULL),
m_pAlaphaTexture(ALCHEMY_NULL),
m_pSceneManager(ALCHEMY_NULL)
{

}

CDepthFieldEffect::~CDepthFieldEffect()
{
	Destroy();
}

bool CDepthFieldEffect::Create(FLOAT fDistance, FLOAT fBlur)
{
	m_fDistance = fDistance;
	m_fBlur = fBlur;
	CSceneManager* pSceneManager = CSceneManager::GetInstance();
	if(!pSceneManager)
		return false;

	CRenderSystem& RenderSystem = pSceneManager->GetRenderSystem();
	UINT uWidth, uHeight;
	RenderSystem.GetViewport(&uWidth, &uHeight);

	CVertexDeclaration::VERTEXELEMENT VertexElements[7];

	CVertexDeclaration::LPVERTEXELEMENT pVertexElement = VertexElements;
	UINT uOffset = 0;

	pVertexElement->uStream = 0;
	pVertexElement->uOffset = uOffset;
	pVertexElement->Type    = CVertexDeclaration::FLOAT4;
	pVertexElement->Usage   = CVertexDeclaration::POSITIONT;
	uOffset += pVertexElement->Type;

	for (UINT i=0; i<6; ++i)
	{
		++ pVertexElement;
		pVertexElement->uStream = 0;
		pVertexElement->uOffset = uOffset;
		pVertexElement->Type    = CVertexDeclaration::FLOAT2;
		pVertexElement->Usage   = CVertexDeclaration::TEXCOORD;
		uOffset += pVertexElement->Type;
	}

	UINT uVertexUsage = ALCHEMY_FLAG(INDEXBUFFER::WRITE_ONLY);

	m_pMesh = pSceneManager->CreateMesh(4, 6, ALCHEMY_FLAG(INDEXBUFFER::WRITE_ONLY), INDEXBUFFER::BIT16, VertexElements, 7, &uVertexUsage);
	if(!m_pMesh)
	{
		ALCHEMY_DEBUG_WARNING("CDepthFieldEffect::Create: Full screen plane mesh has been created fail.");
		return false;
	}

	CVertex& Vertex = m_pMesh->GetVertex();
	LPVERTEXBUFFER pVertexBuffer = Vertex.GetVertexBuffer(0);
	if(!pVertexBuffer)
	{
		m_pMesh->Release();
		m_pMesh = ALCHEMY_NULL;
		ALCHEMY_DEBUG_WARNING("CDepthFieldEffect::Create: Full screen plane vertex buffer can not be get.");
		return false;
	}

	PFLOAT pfVertices = reinterpret_cast<PFLOAT>( pVertexBuffer->Lock(0) );
	if(!pfVertices)
	{
		m_pMesh->Release();
		m_pMesh = ALCHEMY_NULL;
		ALCHEMY_DEBUG_WARNING("CDepthFieldEffect::Create: Full screen plane vertex buffer can not be locked.");
		return false;
	}

	UINT uIndex = 0;

	FLOAT fHalf = m_fBlur;
	FLOAT fOffOne = fHalf * 0.5f;
	FLOAT fOffTwo = fOffOne * sqrtf( 3.0f );

	FLOAT fWidthMod = 1.0f/uWidth;
	FLOAT fHeightMod = 1.0f/uHeight;

	//vertex 0
	pfVertices[uIndex ++] =  1.0f;
	pfVertices[uIndex ++] =  1.0f;
	pfVertices[uIndex ++] =  0.0f;
	pfVertices[uIndex ++] =  1.0f;

	pfVertices[uIndex ++] =   1.0f;
	pfVertices[uIndex ++] =   0.0f;

	pfVertices[uIndex ++] =   1.0f;
	pfVertices[uIndex ++] =   - fHalf * fHeightMod;

	pfVertices[uIndex ++] =   1.0f - fOffTwo * fWidthMod;
	pfVertices[uIndex ++] =   - fOffOne * fHeightMod;

	pfVertices[uIndex ++] =   1.0f + fOffTwo * fWidthMod;
	pfVertices[uIndex ++] =   - fOffOne * fHeightMod;

	pfVertices[uIndex ++] =   1.0f - fOffTwo * fWidthMod;
	pfVertices[uIndex ++] =   fOffOne * fHeightMod;

	pfVertices[uIndex ++] =   1.0f + fOffTwo * fWidthMod;
	pfVertices[uIndex ++] =   fOffOne * fHeightMod;

	//vertex 1
	pfVertices[uIndex ++] =   1.0f;
	pfVertices[uIndex ++] =   -1.0f;
	pfVertices[uIndex ++] =   0.0f;
	pfVertices[uIndex ++] =   1.0f;

	pfVertices[uIndex ++] =   1.0f;
	pfVertices[uIndex ++] =   1.0f;

	pfVertices[uIndex ++] =   1.0f;
	pfVertices[uIndex ++] =   1.0f - fHalf * fHeightMod;

	pfVertices[uIndex ++] =   1.0f - fOffTwo * fWidthMod;
	pfVertices[uIndex ++] =   1.0f - fOffOne * fHeightMod;

	pfVertices[uIndex ++] =   1.0f + fOffTwo * fWidthMod;
	pfVertices[uIndex ++] =   1.0f - fOffOne * fHeightMod;

	pfVertices[uIndex ++] =   1.0f - fOffTwo * fWidthMod;
	pfVertices[uIndex ++] =   1.0f + fOffOne * fHeightMod;

	pfVertices[uIndex ++] =   1.0f + fOffTwo * fWidthMod;
	pfVertices[uIndex ++] =   1.0f + fOffOne * fHeightMod;

	//vertex 2
	pfVertices[uIndex ++] = - 1.0f;
	pfVertices[uIndex ++] =   1.0f;
	pfVertices[uIndex ++] =   0.0f;
	pfVertices[uIndex ++] =   1.0f;

	pfVertices[uIndex ++] =   0.0f;
	pfVertices[uIndex ++] =   0.0f;

	pfVertices[uIndex ++] =   0.0f;
	pfVertices[uIndex ++] =   - fHalf * fHeightMod;

	pfVertices[uIndex ++] =   - fOffTwo * fWidthMod;
	pfVertices[uIndex ++] =   - fOffOne * fHeightMod;

	pfVertices[uIndex ++] =   fOffTwo * fWidthMod;
	pfVertices[uIndex ++] =   - fOffOne * fHeightMod;

	pfVertices[uIndex ++] =   - fOffTwo * fWidthMod;
	pfVertices[uIndex ++] =   fOffOne * fHeightMod;

	pfVertices[uIndex ++] =     fOffTwo * fWidthMod;
	pfVertices[uIndex ++] =   - fOffOne * fHeightMod;

	//vertex 3
	pfVertices[uIndex ++] =   -1.0f;
	pfVertices[uIndex ++] =   -1.0f;
	pfVertices[uIndex ++] =   0.0f;
	pfVertices[uIndex ++] =   1.0f;

	pfVertices[uIndex ++] =   0.0f;
	pfVertices[uIndex ++] =   1.0f;

	pfVertices[uIndex ++] =   0.0f;
	pfVertices[uIndex ++] =   1.0f - fHalf * fHeightMod;

	pfVertices[uIndex ++] =   - fOffTwo * fWidthMod;
	pfVertices[uIndex ++] =   1.0f - fHalf * fHeightMod;

	pfVertices[uIndex ++] =   fOffTwo * fWidthMod;
	pfVertices[uIndex ++] =   1.0f - fOffOne * fHeightMod;

	pfVertices[uIndex ++] =   fOffTwo * fWidthMod;
	pfVertices[uIndex ++] =   1.0f + fOffOne * fHeightMod;

	pfVertices[uIndex ++] =   -fOffTwo * fWidthMod;
	pfVertices[uIndex ++] =   1.0f + fOffOne * fHeightMod;

	pVertexBuffer->Unlock();

	INDEXBUFFER& IndexBuffer = m_pMesh->GetIndexBuffer();
	PUINT16 puIndices = reinterpret_cast<PUINT16>( IndexBuffer.Lock(0) );

	if(!puIndices)
	{
		m_pMesh->Release();
		m_pMesh = ALCHEMY_NULL;
		ALCHEMY_DEBUG_WARNING("CDepthFieldEffect::Create: Full screen plane index buffer can not be locked.");
		return false;
	}

	puIndices[0] = 0;
	puIndices[1] = 1;
	puIndices[2] = 2;

	puIndices[3] = 1;
	puIndices[4] = 3;
	puIndices[5] = 2;

	IndexBuffer.Unlock();

	CMesh::ATTRIBUTERANGE AttributeRange;
	AttributeRange.uStartIndex = 0;
	AttributeRange.uIndexCount = 6;

	m_pMesh->GetAttributeTable().Add(AttributeRange);

	m_pAlaphaTexture = RenderSystem.CreateTexture(uWidth, uHeight, ALCHEMY_FLAG(TEXTURE::WRITE_ONLY) | ALCHEMY_FLAG(TEXTURE::RENDER_TARGET), TEXTURE::DEFAULT, 1);
	if(!m_pAlaphaTexture)
		return false;

	CEffectCode Code( RenderSystem.GetCompiler() );
	CEffectCode::CShaderParametersDefiner ParametersDefiner(Code);
	ParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITIONT, 0);
	ParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT2, CVertexDeclaration::TEXCOORD, 0);
	ParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT2, CVertexDeclaration::TEXCOORD, 1);
	ParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT2, CVertexDeclaration::TEXCOORD, 2);
	ParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT2, CVertexDeclaration::TEXCOORD, 3);
	ParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT2, CVertexDeclaration::TEXCOORD, 4);
	ParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT2, CVertexDeclaration::TEXCOORD, 5);
	UINT uParameters = ParametersDefiner.EndDefine();

	CEffectCode::CShaderFunctionDefiner VertexShaderDefiner(Code, true);
	UINT uAttribute = VertexShaderDefiner.BeginFunction(uParameters), uVaring = VertexShaderDefiner.EndFunction(uParameters);
	VertexShaderDefiner.AppendInstruction(ICompiler::MOV, Code.GetAddressVariable(uVaring, 0), Code.GetAddressVariable(uAttribute, 0), 0);

	for (UINT i=1; i<=6; ++i)
		VertexShaderDefiner.AppendInstruction(ICompiler::MOV, Code.GetAddressVariable(uVaring, i), ALCHEMY_NULL, 0, Code.GetAddressVariable(uAttribute, i), ALCHEMY_NULL, 0, 0, ALCHEMY_NULL, 0);

	CEffectCode::CShaderFunctionDefiner FragmentShaderDefiner(Code, false);
	uVaring = FragmentShaderDefiner.BeginFunction(uParameters);

	UINT uSampler[2];

	for (UINT i=0; i<2; ++i)
	{
		m_SampleDeclaration[i].Type     = IEffect::TEXTURE2D;
		m_SampleDeclaration[i].AddressU = IEffect::CLAMP;
		m_SampleDeclaration[i].AddressV = IEffect::CLAMP;
		m_SampleDeclaration[i].AddressW = IEffect::CLAMP;
		m_SampleDeclaration[i].MagFilter = IEffect::LINEAR;
		m_SampleDeclaration[i].MipFilter = IEffect::LINEAR;
		m_SampleDeclaration[i].MinFilter = IEffect::LINEAR;

		uSampler[i] = FragmentShaderDefiner.AddSamplerVariable(m_SampleDeclaration[i], i);
	}

	UINT uConst;
	CEffectCode::GLOBALVARIABLE ConstInfo;
	ConstInfo.GlobalType = CEffectCode::INPUT_VARIABLE;
	ConstInfo.ShaderType = IEffect::FRAGMENT_SHADER;
	ConstInfo.uDetail    = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR1_VARIABLE, 1, 1, false);
	ConstInfo.uCount     = 1;
	ConstInfo.uIndex     = 0;
	ConstInfo.pValue     = &s_CONSTANT_ONE;
	uConst = Code.AddGlobalVariable( ConstInfo );

	UINT uNumTexCoord;
	CEffectCode::GLOBALVARIABLE ConstNumTexCoord;
	ConstNumTexCoord.GlobalType = CEffectCode::INPUT_VARIABLE;
	ConstNumTexCoord.ShaderType = IEffect::FRAGMENT_SHADER;
	ConstNumTexCoord.uDetail    = CEffectCode::BuildGlobalInputCode(CEffectCode::VECTOR1_VARIABLE, 1, 1, false);
	ConstNumTexCoord.uCount     = 1;
	ConstNumTexCoord.uIndex     = 0;
	ConstNumTexCoord.pValue     = &sm_NUM_SAMPLERS;
	uNumTexCoord = Code.AddGlobalVariable( ConstNumTexCoord );

	UINT uAlphaInfo = FragmentShaderDefiner.ApplyVariable(CEffectCode::VECTOR4_VARIABLE, 1);
	//UINT uSourceAlpha = FragmentShaderDefiner.ApplyVariable(CEffectCode::VECTOR4_VARIABLE, 1);
	//UINT uDestAlpha = FragmentShaderDefiner.ApplyVariable(CEffectCode::VECTOR4_VARIABLE, 1);
	UINT uSourceColor = FragmentShaderDefiner.ApplyVariable(CEffectCode::VECTOR4_VARIABLE, 1);
	UINT uDestColor = FragmentShaderDefiner.ApplyVariable(CEffectCode::VECTOR4_VARIABLE, 1);
	UINT uCurrentColor = FragmentShaderDefiner.ApplyVariable(CEffectCode::VECTOR4_VARIABLE, 1);
	//UINT uCurrentAlpha = FragmentShaderDefiner.ApplyVariable(CEffectCode::VECTOR4_VARIABLE, 1);

	//UINT uLerpValue = FragmentShaderDefiner.ApplyVariable(CEffectCode::VECTOR1_VARIABLE, 1);
	UINT uLerpColor1 = FragmentShaderDefiner.ApplyVariable(CEffectCode::VECTOR4_VARIABLE, 1);
	UINT uLerpColor2 = FragmentShaderDefiner.ApplyVariable(CEffectCode::VECTOR4_VARIABLE, 1);

	UINT uSubAddress[] = {0, 1, 2, 3};
	FragmentShaderDefiner.AppendInstruction(ICompiler::TEX, uAlphaInfo, uSubAddress, 1,  Code.GetAddressVariable(uVaring, 1), uSubAddress, 2, uSampler[0], ALCHEMY_NULL, 0);
	FragmentShaderDefiner.AppendInstruction(ICompiler::TEX, uSourceColor, Code.GetAddressVariable(uVaring, 1), uSampler[1] );

	FragmentShaderDefiner.AppendInstruction(ICompiler::MOV, uCurrentColor, &uSubAddress[3], 1, uConst, uSubAddress, 1, 0, ALCHEMY_NULL, 0);

	for (UINT i=2; i<=6; ++i)
	{
		FragmentShaderDefiner.AppendInstruction(ICompiler::TEX, uAlphaInfo, &uSubAddress[1], 1, Code.GetAddressVariable(uVaring, i),uSubAddress, 2, uSampler[0], ALCHEMY_NULL, 0);
		FragmentShaderDefiner.AppendInstruction(ICompiler::TEX, uDestColor, Code.GetAddressVariable(uVaring, i), uSampler[1] );

		FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uAlphaInfo, &uSubAddress[2], 1, uAlphaInfo, &uSubAddress[0], 1, uAlphaInfo, &uSubAddress[1], 1);
		FragmentShaderDefiner.AppendInstruction(ICompiler::SAT, uAlphaInfo, &uSubAddress[2], 1,  uAlphaInfo, &uSubAddress[2], 1,  0,ALCHEMY_NULL,0);

		FragmentShaderDefiner.AppendInstruction(ICompiler::SUB, uAlphaInfo, &uSubAddress[3], 1,  uConst, uSubAddress, 1, uAlphaInfo, &uSubAddress[2], 1);
		FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uLerpColor1, uSubAddress, 3, uAlphaInfo, &uSubAddress[3], 1, uSourceColor, uSubAddress, 3);
		FragmentShaderDefiner.AppendInstruction(ICompiler::MUL, uLerpColor2, uSubAddress, 3, uAlphaInfo, &uSubAddress[2], 1, uDestColor, uSubAddress, 3);

		FragmentShaderDefiner.AppendInstruction(ICompiler::ADD, uLerpColor1, uSubAddress, 3, uLerpColor1, uSubAddress, 3, uLerpColor2, uSubAddress, 3);

		if(i == 2)
			FragmentShaderDefiner.AppendInstruction(ICompiler::MOV, uCurrentColor, uSubAddress, 3,  uLerpColor1,uSubAddress,3, 0,ALCHEMY_NULL,0);
		else
			FragmentShaderDefiner.AppendInstruction(ICompiler::ADD, uCurrentColor, uSubAddress, 3,uCurrentColor, uSubAddress, 3, uLerpColor1,uSubAddress,3);
	}

	FragmentShaderDefiner.AppendInstruction(ICompiler::DIV, uCurrentColor, uSubAddress, 3, uCurrentColor, uSubAddress, 3, uNumTexCoord, uSubAddress, 1);
	FragmentShaderDefiner.AppendInstruction(ICompiler::MOV, Code.GetAddressVariable( FragmentShaderDefiner.EndFunction(uParameters),0), uCurrentColor, 0);
	//FragmentShaderDefiner.AppendInstruction(CCompilerInterface::TEX, Code.GetAddressVariable( FragmentShaderDefiner.EndFunction(uParameters),0), Code.GetAddressVariable(uVaring, 1), uSampler[1]);
	Code.AddPass( VertexShaderDefiner.EndDefine(), FragmentShaderDefiner.EndDefine() );

	m_pEffect = RenderSystem.CreateEffect(Code);
	if(!m_pEffect)
	{
		ALCHEMY_DEBUG_WARNING("CDepthFieldEffect::Create: Effect Code fail.");
		return false;
	}

	UINT ByteAlignmentShift = RenderSystem.GetCompiler().GetCompilerInfo().uByteAlignmentShift;
	const CEffectCode::VARIABLE* pVariable;

	pVariable = Code.GetVariable( uConst );
	if(pVariable)
		m_uEffectParameters[EFFECT_CONSTANT_ONE] = pVariable->uRegister >> ByteAlignmentShift;
	else
		return false;

	pVariable = Code.GetVariable( uNumTexCoord );
	if(pVariable)
		m_uEffectParameters[EFFECT_NUM_SAMPLER] = pVariable->uRegister >> ByteAlignmentShift;
	else
		return false;

	m_uEffectParameters[EFFECT_ALPHA_TEXTURE] = 0;
	m_uEffectParameters[EFFECT_SCENE_TEXTURE] = 1;

	m_pSceneManager = pSceneManager;

	return true;
}

void CDepthFieldEffect::Destroy()
{
	if(m_pEffect)
		m_pEffect->Release();

	if(m_pMesh)
		m_pMesh->Release();

	if(m_pAlaphaTexture)
		m_pAlaphaTexture->Release();

	m_pEffect = ALCHEMY_NULL;
	m_pMesh = ALCHEMY_NULL;
	m_pAlaphaTexture = ALCHEMY_NULL;
}

bool CDepthFieldEffect::ApplyForRender()
{
	if( !m_pEffect || !m_pMesh || !m_pAlaphaTexture || !m_pSceneManager)
		return false;

	CRenderQueue::RENDERENTRY& RenderEntry = m_pSceneManager->GetRenderQueue().LockRenderEntry(*this);
	RenderEntry.uRenderTarget              = 0;
	RenderEntry.uEffect                    = m_pEffect->GetResourceHandle();
	RenderEntry.uRenderPass                = 0;
	RenderEntry.uVertexDeclaration         = m_pMesh->GetVertex().GetVertexDeclaration().GetResourceHandle();
	RenderEntry.uModel                     = m_pMesh->GetResourceHandle();
	RenderEntry.uModelParameter            = 0;

	return m_pSceneManager->GetRenderQueue().UnlockRenderEntry( CRenderQueue::FULL_SCREEN, ALCHEMY_FLAG(CRenderQueue::AUTO_SHADER) );
}

void CDepthFieldEffect::Render(const CRenderQueue::RENDERENTRY& Entry, const CRenderQueue::RENDERKEY& Key, UINT uFlag)
{
	if(m_pEffect && m_pSceneManager)
	{
		CGraphicsResource::HANDLE SceneRenderTarget = m_pSceneManager->GetRenderQueue().GetRenderTarget();
		if(SceneRenderTarget == Entry.uRenderTarget)
			return;

		CRenderSystem& RenderSystem = m_pSceneManager->GetRenderSystem();

		if( ALCHEMY_TEST_BIT(uFlag, CRenderQueue::PARENT) || ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_EFFECT) || ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_EFFECT_PASS) )
		{
			if(ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_EFFECT) && Entry.uEffect)
				RenderSystem.SetShader(Entry.uEffect);

			if(ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_EFFECT_PASS) && Entry.uRenderPass)
				RenderSystem.SetShader(Entry.uRenderPass);

			RenderSystem.SetShaderParameters(IEffect::FRAGMENT_SHADER, m_uEffectParameters[EFFECT_CONSTANT_ONE], &s_CONSTANT_ONE, 1);
			RenderSystem.SetShaderParameters(IEffect::FRAGMENT_SHADER, m_uEffectParameters[EFFECT_NUM_SAMPLER], &sm_NUM_SAMPLERS, 1);

			RenderSystem.ResetTexture();
			RenderSystem.SetTexture( IEffect::FRAGMENT_SHADER, m_uEffectParameters[EFFECT_ALPHA_TEXTURE], m_pAlaphaTexture->GetResourceHandle(), &m_SampleDeclaration[0]);
			RenderSystem.SetTexture( IEffect::FRAGMENT_SHADER, m_uEffectParameters[EFFECT_SCENE_TEXTURE], SceneRenderTarget, &m_SampleDeclaration[1]);
		}
		
		static const IRenderState::RENDERSTATEINFO s_RENDER_STATE_INFOS[] = 
		{
			{IRenderState::DEPTH_WRITEE_ENABLE, ALCHEMY_FALSE        }, 
			{IRenderState::DEPTH_FUNC         , CRenderSystem::ALWAYS}
		};

		RenderSystem.SetRenderState(s_RENDER_STATE_INFOS, 2, true);
		m_pMesh->DrawSubset(0, ALCHEMY_TEST_BIT(uFlag, CRenderQueue::MODEL), ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_VERTEX_DECLARATION) );
		RenderSystem.RestoreRenderState();
	}
}