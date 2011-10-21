#include "AlchemySkyPlane.h"

using namespace alchemy;

CSkyPlane::CSkyPlane(void) :
m_pSceneManager(ALCHEMY_NULL),
m_pMesh(ALCHEMY_NULL),
m_pTexture(ALCHEMY_NULL),
m_pEffect(ALCHEMY_NULL),
m_bIsAmbientLight(false),
m_bIsEmissive(false),
m_bIsUVOffset(false),
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
					   CCompilerInterface& Compiler, 
					   bool bIsAmbientLight,
					   bool bIsEmissive,
					   bool bIsUVOffset)
{
	Destroy();

	if( !CSceneManager::GetInstance() )
	{
		ALCHEMY_DEBUG_WARNING("CSkyPlane::Create:  Sky plane can not be created without a activate scene manager.");

		return false;
	}

	CSceneManager& SceneManager = *CSceneManager::GetInstance();

	m_UVOffset.x = m_UVOffset.y = 0.0f;

	static const CVertexDeclaration::VERTEXELEMENT s_VERTEX_ELEMENTS[2] = { {0, 0, CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION}, {0, 16, CVertexDeclaration::FLOAT2, CVertexDeclaration::TEXCOORD} };

	CFloat3 PositonOffset(0.0f, fHeight, 0.0f);
	UINT uVertexUsage = ALCHEMY_FLAG(VERTEXBUFFER::WRITE_ONLY), uIndexUsage = ALCHEMY_FLAG(INDEXBUFFER::WRITE_ONLY);

	UINT uMeshData = ALCHEMY_MESH_MANAGER.CreatePlane(fLength, fWidth, 1, 1, 1, 1, true, s_VERTEX_ELEMENTS, 2, &PositonOffset, &uVertexUsage, uIndexUsage);
	if(uMeshData)
	{
		const CMeshManager::MESHDATA* pMeshData = ALCHEMY_MESH_MANAGER.GetMeshData(uMeshData);

		if(pMeshData)
			m_pMesh = SceneManager.CreateMesh(*pMeshData);
		
		ALCHEMY_MESH_MANAGER.DestroyMeshData(uMeshData);
	}

	if(m_pMesh)
	{
		m_pTexture = static_cast<CTexture*>( SceneManager.GetRenderSystem().GetResource(Texture, CGraphicsResource::TEXTURE) );

		if(m_pTexture)
		{
			CEffectCode EffectCode(Compiler);

			CEffectCode::CShaderParametersDefiner ShaderParametersDefiner(EffectCode);

			ShaderParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION);
			ShaderParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT2, CVertexDeclaration::TEXCOORD);

			UINT uShaderInput = ShaderParametersDefiner.EndDefine();

			ShaderParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITIONT);
			ShaderParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT2, CVertexDeclaration::TEXCOORD);

			UINT uShaderVarying = ShaderParametersDefiner.EndDefine();

			CEffectCode::CShaderFunctionDefiner VertexFunctionDefiner(EffectCode, true);

			UINT uVertexShaderInput = VertexFunctionDefiner.BeginFunction(uShaderInput), uVertexShaderOutput = VertexFunctionDefiner.EndFunction(uShaderVarying), uOutputPosition = EffectCode.GetAddressVariable(uVertexShaderOutput, 0);

			CEffectCode::GLOBALVARIABLEKEY WorldViewProjectionMatrixKey;
			WorldViewProjectionMatrixKey.GlobalType = CEffectCode::MATRIX_VARIABLE;
			WorldViewProjectionMatrixKey.ShaderType = CEffectInterface::VERTEX_SHADER;
			WorldViewProjectionMatrixKey.uDetail    = CRenderMethod::WORLD_VIEW_PROJECTION_MATRIX;
			WorldViewProjectionMatrixKey.uCount     = 1;
			WorldViewProjectionMatrixKey.pValue     = ALCHEMY_NULL;

			UINT uWorldViewProjectionMatrix = EffectCode.AddGlobalVariable(WorldViewProjectionMatrixKey);

			UINT uPosition = VertexFunctionDefiner.ApplyVariable(CEffectCode::VECTOR4_VARIABLE, 1);

			VertexFunctionDefiner.AppendInstruction(CCompilerInterface::M44, uPosition, EffectCode.GetAddressVariable(uVertexShaderInput, 0), uWorldViewProjectionMatrix);

			const static UINT s_uADDRESS[] = {0, 1, 3, 3};

			VertexFunctionDefiner.AppendInstruction(CCompilerInterface::MOV, uOutputPosition, ALCHEMY_NULL, 0, uPosition, s_uADDRESS, 4, 0, ALCHEMY_NULL, 0);

			UINT uUVOffset = 0;
			if(bIsUVOffset)
			{
				CEffectCode::GLOBALVARIABLEKEY UVOffsetKey;
				UVOffsetKey.GlobalType = CEffectCode::NORMAL_VARIABLE;
				UVOffsetKey.ShaderType = CEffectInterface::VERTEX_SHADER;
				UVOffsetKey.uDetail    = CRenderMethodInterface::UV_OFFSET;
				UVOffsetKey.uCount     = 1;
				UVOffsetKey.pValue     = ALCHEMY_NULL;

				uUVOffset = EffectCode.AddGlobalVariable(UVOffsetKey);

				VertexFunctionDefiner.AppendInstruction(CCompilerInterface::ADD, EffectCode.GetAddressVariable(uVertexShaderOutput, 1), ALCHEMY_NULL, 0, EffectCode.GetAddressVariable(uVertexShaderInput, 1), ALCHEMY_NULL, 0, uUVOffset, ALCHEMY_NULL, 0);
			}
			else
				VertexFunctionDefiner.AppendInstruction(CCompilerInterface::MOV, EffectCode.GetAddressVariable(uVertexShaderOutput, 1), ALCHEMY_NULL, 0, EffectCode.GetAddressVariable(uVertexShaderInput, 1), ALCHEMY_NULL, 0, 0        , ALCHEMY_NULL, 0);

			ShaderParametersDefiner.AppendParameter(CVertexDeclaration::FLOAT4, CVertexDeclaration::COLOR);

			CEffectCode::CShaderFunctionDefiner FragmentFunctionDefiner(EffectCode, false);

			UINT uTexcoord = EffectCode.GetAddressVariable(FragmentFunctionDefiner.BeginFunction(uShaderVarying), 1), 
				uOutputColor = EffectCode.GetAddressVariable(FragmentFunctionDefiner.EndFunction( ShaderParametersDefiner.EndDefine() ), 0);

			CEffectCode::SAMPLERDECLARATION SamplerDeclaration;

			SamplerDeclaration.Type     = CEffectCode::TEXTURE2D;
			SamplerDeclaration.AddressU = CEffectCode::WRAP;
			SamplerDeclaration.AddressV = CEffectCode::WRAP;
			SamplerDeclaration.AddressW = CEffectCode::WRAP;
			SamplerDeclaration.MinFilter = CEffectCode::LINEAR;
			SamplerDeclaration.MagFilter = CEffectCode::LINEAR;
			SamplerDeclaration.MipFilter = CEffectCode::LINEAR;

			UINT uSampler = EffectCode.AddSamplerVariable(SamplerDeclaration, 0);

			UINT uAmbientLight = 0, uEmissive = 0;
			if(bIsAmbientLight || bIsEmissive)
			{
				UINT uTemp = FragmentFunctionDefiner.ApplyVariable(CEffectCode::VECTOR4_VARIABLE, 1);
				FragmentFunctionDefiner.AppendInstruction(CCompilerInterface::TEX, uTemp, uTexcoord, uSampler);

				if(bIsAmbientLight)
				{
					CEffectCode::GLOBALVARIABLEKEY AmbientLightKey;
					AmbientLightKey.GlobalType = CEffectCode::NORMAL_VARIABLE;
					AmbientLightKey.ShaderType = CEffectInterface::FRAGMENT_SHADER;
					AmbientLightKey.uDetail    = CRenderMethodInterface::AMBIENT_LIGHT;
					AmbientLightKey.uCount     = 1;
					AmbientLightKey.pValue     = ALCHEMY_NULL;

					uAmbientLight = EffectCode.AddGlobalVariable(AmbientLightKey);

					FragmentFunctionDefiner.AppendInstruction(CCompilerInterface::MUL, bIsEmissive ? uTemp : uOutputColor, uTemp, uAmbientLight);
				}

				if(bIsEmissive)
				{
					CEffectCode::GLOBALVARIABLEKEY EmissiveKey;
					EmissiveKey.GlobalType = CEffectCode::MATERIAL_VARIABLE;
					EmissiveKey.ShaderType = CEffectInterface::FRAGMENT_SHADER;
					EmissiveKey.uDetail    = CRenderMethod::MATERIAL_EMISSIVE;
					EmissiveKey.uCount     = 1;
					EmissiveKey.pValue     = ALCHEMY_NULL;

					uEmissive = EffectCode.AddGlobalVariable(EmissiveKey);

					FragmentFunctionDefiner.AppendInstruction(CCompilerInterface::ADD, uOutputColor, uTemp, uEmissive);
				}
			}
			else
				FragmentFunctionDefiner.AppendInstruction(CCompilerInterface::TEX, uOutputColor, uTexcoord, uSampler);

			EffectCode.AddPass( VertexFunctionDefiner.EndDefine(), FragmentFunctionDefiner.EndDefine() );

			m_pEffect = SceneManager.GetRenderSystem().CreateEffect(EffectCode);

			if(m_pEffect)
			{
				UINT ByteAlignmentShift = Compiler.GetByteAlignmentShift();
				const CEffectCode::VARIABLE* pVariable;

				pVariable = EffectCode.GetVariable(uWorldViewProjectionMatrix);
				if(pVariable)
					m_uEffectParameters[EFFECT_PARAMETER_WORLD_VIEW_PROJECTION_MATRIX] = pVariable->uRegister >> ByteAlignmentShift;
				else
				{
					ALCHEMY_DEBUG_WARNING("CSkyPlane::Create: Create effect fail.");

					m_pMesh = ALCHEMY_NULL;

					m_pTexture = ALCHEMY_NULL;

					m_pEffect = ALCHEMY_NULL;

					return false;
				}

				pVariable = EffectCode.GetVariable(uSampler);
				if(pVariable)
					m_uEffectParameters[EFFECT_PARAMETER_TEXTURE] = pVariable->uRegister;
				else
				{
					ALCHEMY_DEBUG_WARNING("CSkyPlane::Create: Create effect fail.");

					m_pMesh = ALCHEMY_NULL;

					m_pTexture = ALCHEMY_NULL;

					m_pEffect = ALCHEMY_NULL;

					return false;
				}

				if(bIsUVOffset)
				{
					pVariable = EffectCode.GetVariable(uUVOffset);
					if(pVariable)
						m_uEffectParameters[EFFECT_PARAMETER_UV_OFFSET] = pVariable->uRegister >> ByteAlignmentShift;
					else
					{
						ALCHEMY_DEBUG_WARNING("CSkyPlane::Create: Create effect fail.");

						m_pMesh = ALCHEMY_NULL;

						m_pTexture = ALCHEMY_NULL;

						m_pEffect = ALCHEMY_NULL;

						return false;
					}
				}

				m_bIsUVOffset = bIsUVOffset;

				if(bIsAmbientLight)
				{
					pVariable = EffectCode.GetVariable(uAmbientLight);
					if(pVariable)
						m_uEffectParameters[EFFECT_PARAMETER_AMBIENT_LIGHT] = pVariable->uRegister >> ByteAlignmentShift;
					else
					{
						ALCHEMY_DEBUG_WARNING("CSkyPlane::Create: Create effect fail.");

						m_pMesh = ALCHEMY_NULL;

						m_pTexture = ALCHEMY_NULL;

						m_pEffect = ALCHEMY_NULL;

						return false;
					}
				}

				m_bIsAmbientLight = bIsAmbientLight;

				if(bIsEmissive)
				{
					pVariable = EffectCode.GetVariable(uEmissive);
					if(pVariable)
						m_uEffectParameters[EFFECT_PARAMETER_EMISSIVE] = pVariable->uRegister >> ByteAlignmentShift;
					else
					{
						ALCHEMY_DEBUG_WARNING("CSkyPlane::Create: Create effect fail.");

						m_pMesh = ALCHEMY_NULL;

						m_pTexture = ALCHEMY_NULL;

						m_pEffect = ALCHEMY_NULL;

						return false;
					}
				}

				m_bIsEmissive = bIsEmissive;

				m_pSceneManager = &SceneManager;

					//m_pMesh->AddRef();
				m_pTexture->AddRef();
				//m_pEffect->AddRef();

				return true;
			}
			else
			{
				ALCHEMY_DEBUG_WARNING("CSkyPlane::Create: Create effect fail.");

				m_pMesh = ALCHEMY_NULL;

				m_pTexture = ALCHEMY_NULL;

				m_pEffect = ALCHEMY_NULL;

				return false;
			}
		}
		else
		{
			ALCHEMY_DEBUG_WARNING("CSkyPlane::Create: Error texture handle.");

			m_pMesh = ALCHEMY_NULL;

			return false;
		}
	}

	ALCHEMY_DEBUG_WARNING("CSkyPlane::Create: create mesh fail.");

	return false;
}

void CSkyPlane::Update()
{
	m_UVOffset += m_UVOffsetSpeed;
}

bool CSkyPlane::ApplyForRender()
{
	if( !__IsValuable() )
		return false;

	CRenderQueue::RENDERENTRY& Entry = m_pSceneManager->LockRenderEntry(*this);

	Entry.uRenderTarget      = 0;
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

void CSkyPlane::Render(const CRenderQueue::RENDERENTRY& Entry, UINT uFlag)
{
	if( !__IsValuable() )
		return;

	if( ALCHEMY_TEST_BIT(uFlag, CRenderQueue::EFFECT) && !m_pEffect->Begin() )
		return;

	if( ALCHEMY_TEST_BIT(uFlag, CRenderQueue::EFFECT_PASS) && !m_pEffect->BeginPass(Entry.uRenderPass) )
		return;

	if( ALCHEMY_TEST_BIT(uFlag, CRenderQueue::PARENT) || ALCHEMY_TEST_BIT(uFlag, CRenderQueue::EFFECT_PASS) )
	{
		const CCamera& Camera = m_pSceneManager->GetCamera();
		const FLOAT* pCameraPosition = Camera.GetWorldMatrix().fPosition[3];
		CMatrix4x4 Matrix(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			pCameraPosition[0], pCameraPosition[1], pCameraPosition[2], 1.0f);

		Matrix *= Camera.GetViewProjectionMatrix();

		m_pEffect->SetMatrix(   CEffectInterface::VERTEX_SHADER  , m_uEffectParameters[EFFECT_PARAMETER_WORLD_VIEW_PROJECTION_MATRIX], Matrix                          );
		m_pEffect->SetTexture(  CEffectInterface::FRAGMENT_SHADER, m_uEffectParameters[EFFECT_PARAMETER_TEXTURE]                     , m_pTexture->GetResourceHandle() );

		if(m_bIsAmbientLight)
			m_pEffect->SetParameter(CEffectInterface::FRAGMENT_SHADER, m_uEffectParameters[EFFECT_PARAMETER_AMBIENT_LIGHT], static_cast<const FLOAT*>( m_pSceneManager->GetAmbientLight() ), 4);

		if(m_bIsEmissive)
			m_pEffect->SetParameter(CEffectInterface::FRAGMENT_SHADER, m_uEffectParameters[EFFECT_PARAMETER_EMISSIVE], static_cast<const FLOAT*>(m_Emissive), 4);

		if(m_bIsUVOffset)
			m_pEffect->SetParameter(CEffectInterface::VERTEX_SHADER, m_uEffectParameters[EFFECT_PARAMETER_UV_OFFSET], static_cast<const FLOAT*>(m_UVOffset), 2);

		m_pEffect->CommitChange();
	}

	CRenderSystem& RenderSystem = m_pSceneManager->GetRenderSystem();
	RenderSystem.RegisterRenderState();
	RenderSystem.SetRenderState(CRenderSystem::SRC_BLEND, CRenderSystem::SRC_ALPHA);
	RenderSystem.SetRenderState(CRenderSystem::DEST_BLEND, CRenderSystem::INV_SRC_ALPHA);
	RenderSystem.SetRenderState(CRenderSystem::CULL_MODE, CRenderSystem::NONE);
	RenderSystem.SetRenderState(CRenderSystem::DEPTH_FUNC, CRenderSystem::LESS_EQUAL);
	m_pMesh->DrawSubset( 0, ALCHEMY_TEST_BIT(uFlag, CRenderQueue::MODEL), ALCHEMY_TEST_BIT(uFlag, CRenderQueue::VERTEX_DECLARATION) );
	RenderSystem.RestoreRenderState();

	m_pEffect->EndPass();
	m_pEffect->End();
}

void CSkyPlane::Destroy()
{
	if(m_pMesh)
	{
		m_pMesh->Release();

		m_pMesh = ALCHEMY_NULL;
	}

	if(m_pTexture)
	{
		m_pTexture->Release();

		m_pTexture = ALCHEMY_NULL;
	}

	if(m_pEffect)
	{
		m_pEffect->Release();

		m_pEffect = ALCHEMY_NULL;
	}
}