#include "AlchemySceneManager.h"

#include "../Core/AlchemyMemory.h"
#include "../Core/AlchemyUtils.h"
//#include "AlchemySceneObject.h"

using namespace alchemy;

CSceneManager* CSceneManager::sm_pInstance = ALCHEMY_NULL;

CVector<CSceneManager::RENDERLISTNODE> CSceneManager::sm_RenderEntryPool;
UINT CSceneManager::sm_uRenderEntryPoolSize = 0;

CSceneManager::CSceneManager(CRenderSystem& RenderSystem, CSceneNodeFactoryInterface& SceneNodeFactory, UINT uMaxRenderEntries) :
CGraphicsResourcePool(CGraphicsResource::BEGIN_SCENE_RESOURCE, CGraphicsResource::END_SCENE_RESOURCE),
m_RenderSystem(RenderSystem),
m_SceneNodeFactory(SceneNodeFactory),
m_Scene( SceneNodeFactory.CreateSceneNode() ),
m_SceneCamera( SceneNodeFactory.CreateCamera() ),
m_pCamera(&m_SceneCamera),
m_fElapsedTime(0.0f),
m_fTime(0.0f),
m_SceneRenderTarget(0),
m_BackgroundColor(0),
m_SceneRenderTargetInfo(RenderSystem),
m_pSceneRenderTarget(ALCHEMY_NULL),
m_uSceneRenderTargetSurface(0),
m_FullScreenDrawer(*this)
{
	sm_pInstance = this;

	ALCHEMY_DEBUG_NEW( m_pRenderQueue, CRenderQueue(uMaxRenderEntries, RenderSystem) );

	UINT i;
	for(i = 0; i < EFFECT_TYPE_COUNT; ++ i)
		m_pRenderEntryList[i] = ALCHEMY_NULL;

	for(i = 0; i < EFFECT_TYPE_COUNT; ++ i)
		m_pDynamicRenderEntryList[i] = ALCHEMY_NULL;

	for( i= 0; i < EFFECT_TYPE_COUNT; ++ i)
		m_pFinalRenderEntry[i] = ALCHEMY_NULL;

	 m_pCurrentRenderEntry = ALCHEMY_NULL;
}

CSceneManager::~CSceneManager(void)
{
	CPool<CSceneObject*>::CIterator Iterator = m_SceneObjectPool.GetBeginIterator();
	while( CPool<CSceneObject*>::IsValuable(Iterator) )
	{
		(*Iterator)->Destroy();

		++ Iterator;
	}

	CGraphicsResourcePool::Destroy();

	ALCHEMY_DEBUG_DELETE(m_pRenderQueue);
}

CFullScreenDrawer& CSceneManager::GetFullScreenDrawer()
{
	if( !m_FullScreenDrawer.IsValuable() )
		m_FullScreenDrawer.Create();

	return m_FullScreenDrawer;
}

bool CSceneManager::SetSceneRenderTarget(CGraphicsResource::HANDLE RenderTarget, UINT uSceneRenderTargetSurface)
{
	CGraphicsResource::HANDLE SceneRenderTarget = m_SceneRenderTarget;

	m_SceneRenderTarget         = RenderTarget;
	m_uSceneRenderTargetSurface = uSceneRenderTargetSurface;

	SetSceneRenderTarget();

	return false;
}

bool CSceneManager::SetSceneRenderTarget()
{
	return m_RenderSystem.SetRenderTarget(m_SceneRenderTarget, m_uSceneRenderTargetSurface, 0);
}

CRenderQueue::RENDERENTRY& CSceneManager::LockRenderEntry(CCallBackToRenderInterface& Renderer)
{
	if( sm_uRenderEntryPoolSize == sm_RenderEntryPool.GetLength() )
	{
		static RENDERLISTNODE s_RenderEntry;
		sm_RenderEntryPool.Add(s_RenderEntry);

		++ sm_uRenderEntryPoolSize;
	}

	m_pCurrentRenderEntry = &sm_RenderEntryPool[sm_uRenderEntryPoolSize - 1];
	m_pCurrentRenderEntry->Data.pRenderer = &Renderer;
	m_pCurrentRenderEntry->pNext          = ALCHEMY_NULL;

	return m_pCurrentRenderEntry->Data.RenderEntry;
}

bool CSceneManager::UnlockRenderEntry(RENDERENTRYTYPE RenderEntryType)
{
	if(m_pCurrentRenderEntry && RenderEntryType >= 0 && RenderEntryType < EFFECT_TYPE_COUNT)
	{
		bool bIsDynamic = RenderEntryType < EFFECT_FINAL && m_pCurrentRenderEntry->Data.pRenderer && m_pCurrentRenderEntry->Data.pRenderer->IsDynamic();
		LPRENDERLISTNODE& pRenderEntryHead = bIsDynamic ? m_pDynamicRenderEntryList[RenderEntryType] : m_pRenderEntryList[RenderEntryType];

		if(pRenderEntryHead == ALCHEMY_NULL)
		{
			pRenderEntryHead = m_pCurrentRenderEntry;

			if(RenderEntryType == EFFECT_FINAL)
				m_pFinalRenderEntry[RenderEntryType] = &m_pCurrentRenderEntry->Data;
		}
		else
		{
			m_pCurrentRenderEntry->pNext = pRenderEntryHead;
			pRenderEntryHead             = m_pCurrentRenderEntry;
		}

		if(RenderEntryType == EFFECT_FINAL)
		{
			LPRENDERENTRY pNormalFinalRenderEntry = m_pFinalRenderEntry[EFFECT_NORMAL];

			if(pNormalFinalRenderEntry)
				m_pCurrentRenderEntry->Data.uFlag = CRenderQueue::BuildRenderFlag(m_pCurrentRenderEntry->Data.RenderEntry, pNormalFinalRenderEntry->RenderEntry, m_pCurrentRenderEntry->Data.pRenderer != pNormalFinalRenderEntry->pRenderer);
		}
		else if(!bIsDynamic)
		{
			m_pFinalRenderEntry[RenderEntryType] = ALCHEMY_NULL;

			if(RenderEntryType == EFFECT_2D && !m_SceneRenderTarget)
			{
				m_pSceneRenderTarget = m_RenderSystem.CreateTexture(m_SceneRenderTargetInfo, ALCHEMY_FLAG(TEXTURE::WRITE_ONLY) | ALCHEMY_FLAG(TEXTURE::RENDER_TARGET), TEXTURE::A8R8G8B8, 1);
				if(m_pSceneRenderTarget)
					m_SceneRenderTarget = m_pSceneRenderTarget->GetResourceHandle();
			}
		}

		m_pCurrentRenderEntry = ALCHEMY_NULL;

		return true;
	}

	return false;
}

CLight* CSceneManager::GetLight(UINT uIndex)
{
	CLight** ppLight = m_Lights.Get(uIndex);

	return ppLight ? *ppLight : ALCHEMY_NULL;
}

CLight* CSceneManager::SetLight(CGraphicsResource::HANDLE Light, COldLightEffect::SHADEMODE LightShadeMode, UINT uIndex)
{
	CLight* pLight = static_cast<CLight*>( GetResource(Light, CGraphicsResource::LIGHT) );

	if(pLight)
	{
		if( m_LightEffect.SetLight(pLight->GetLightType(), LightShadeMode, uIndex) && m_Lights.Set(pLight, uIndex) )
			return pLight;

		ALCHEMY_DEBUG_WARNING("CSceneManager::SetLight: Set light fail.");

		return ALCHEMY_NULL;
	}

	ALCHEMY_DEBUG_WARNING("CSceneManager::AddLight: Error handle.");

	return ALCHEMY_NULL;
}

CVertex* CSceneManager::CreateVertex(UINT uNumVertices, CGraphicsResource::HANDLE VertexDeclaration, const UINT* puUsage)
{
	CVertexDeclaration* pVertexDeclaration = static_cast<CVertexDeclaration*>( m_RenderSystem.GetResource(VertexDeclaration, CGraphicsResource::VERTEX_DECLARATION) );
	if(!pVertexDeclaration)
	{
		ALCHEMY_DEBUG_WARNING("CSceneManager::CreateVertex: Error vertex declaration handle.");

		return ALCHEMY_NULL;
	}

	const CVertexDeclaration::VERTEXELEMENT* pVertexElements = pVertexDeclaration->GetVertexElements();
	UINT uVertexElementCount = pVertexDeclaration->GetVertexElementCount();
	UINT uVertexBufferCount  = pVertexDeclaration->GetStreamCount();
	if( (pVertexElements[uVertexElementCount - 1].uStream + 1) != uVertexBufferCount)
	{
		ALCHEMY_DEBUG_WARNING("CSceneManager::CreateVertex: Error vertex element.");

		return ALCHEMY_NULL;
	}

	CVertex* pOutput;
	if( pOutput = static_cast<CVertex*>( CreateResource(CGraphicsResource::VERTEX) ) )
	{
		pOutput->m_pVertexDeclaration = pVertexDeclaration;

		//pOutput->m_pVertexDeclaration->AddRef();
		CVertex::_AddRef(*pOutput->m_pVertexDeclaration);

		const UINT* puElementEndOfStream = pOutput->m_pVertexDeclaration->GetElementEndOfStream();
		const CVertexDeclaration::VERTEXELEMENT* pCurrentVertexElement;

		LPVERTEXBUFFER pVertexBuffer;

		ALCHEMY_DEBUG_NEW(pOutput->m_ppVertexBuffer, LPVERTEXBUFFER[uVertexBufferCount]);

		for(UINT i = 0; i < uVertexBufferCount; i ++)
		{
			pCurrentVertexElement = &pVertexElements[puElementEndOfStream[i]];
			pVertexBuffer = m_RenderSystem.CreateVertexBuffer(uNumVertices, pCurrentVertexElement->uOffset + pCurrentVertexElement->Type, puUsage ? puUsage[i] : 0);

			if(pVertexBuffer)
			{
				pOutput->m_ppVertexBuffer[pCurrentVertexElement->uStream] = pVertexBuffer;

				CVertex::_AddRef(*pVertexBuffer);
			}
			else
			{
				ALCHEMY_DEBUG_WARNING("CSceneManager::CreateVertex: Vertex has been created fail: vertex buffer can not be created.");

				DestroyResource(*pOutput);

				return ALCHEMY_NULL;
			}
		}

		if( pOutput->_Create() )
			return pOutput;

		DestroyResource(*pOutput);
	}

	return ALCHEMY_NULL;
}

CVertex* CSceneManager::CreateVertex(UINT uNumVertices, const CVertexDeclaration::VERTEXELEMENT* pVertexElements, UINT uElementCount, const UINT* puUsage)
{
	if( CVertexDeclaration::GetVertexElementStreamCount(pVertexElements, uElementCount) != ( CVertexDeclaration::GetVertexElementMaxinumStream(pVertexElements, uElementCount) + 1 ) )
	{
		ALCHEMY_DEBUG_WARNING("CSceneManager::CreateVertex: Error vertex element.");

		return ALCHEMY_NULL;
	}

	CVertexDeclaration* pVertexDeclaration = m_RenderSystem.CreateVertexDeclaration(pVertexElements, uElementCount);

	CVertex* pOutput = CreateVertex(uNumVertices, pVertexDeclaration->GetResourceHandle(), puUsage);

	if(pOutput)
		return pOutput;

	pVertexDeclaration->Release();

	return ALCHEMY_NULL;
}

CMesh* CSceneManager::CreateMesh(UINT uNumVertices, UINT uNumIndices, UINT uIndexUsage, CIndexBuffer::FORMAT IndexFormat, CGraphicsResource::HANDLE VertexDeclaration, PUINT puVertexUsage)
{
	CMesh* pOutput;
	if( pOutput = static_cast<CMesh*>( CreateResource(CGraphicsResource::MESH) ) )
	{
		pOutput->m_pVertex = CreateVertex(uNumVertices, VertexDeclaration, puVertexUsage);

		if(!pOutput->m_pVertex)
		{
			ALCHEMY_DEBUG_WARNING("CSceneManager::CreateMesh: Mesh has been created fail: vertex can not be created.");

			DestroyResource(*pOutput);

			return ALCHEMY_NULL;
		}

		//pOutput->m_pVertex->AddRef();
		CMesh::_AddRef(*pOutput->m_pVertex);

		pOutput->m_pIndexBuffer = m_RenderSystem.CreateIndexBuffer(uNumIndices, uIndexUsage, IndexFormat);

		if(pOutput->m_pIndexBuffer)
		{
			CMesh::_AddRef(*pOutput->m_pIndexBuffer);
			//pOutput->m_pIndexBuffer->AddRef();

			if( pOutput->_Create() )
				return pOutput;
		}

		DestroyResource(*pOutput);
	}

	return ALCHEMY_NULL;
}

CMesh* CSceneManager::CreateMesh(UINT uNumVertices, UINT uNumIndices, UINT uIndexUsage, CIndexBuffer::FORMAT IndexFormat, const CVertexDeclaration::VERTEXELEMENT* pVertexElement, UINT uElementCount, PUINT puVertexUsage)
{
	CMesh* pOutput;
	if( pOutput = static_cast<CMesh*>( CreateResource(CGraphicsResource::MESH) ) )
	{
		pOutput->m_pVertex = CreateVertex(uNumVertices, pVertexElement, uElementCount, puVertexUsage);

		if(!pOutput->m_pVertex)
		{
			ALCHEMY_DEBUG_WARNING("CSceneManager::CreateMesh: Mesh has been created fail: vertex can not be created.");

			DestroyResource(*pOutput);

			return ALCHEMY_NULL;
		}

		//pOutput->m_pVertex->AddRef();
		CMesh::_AddRef(*pOutput->m_pVertex);

		pOutput->m_pIndexBuffer = m_RenderSystem.CreateIndexBuffer(uNumIndices, uIndexUsage, IndexFormat);

		if(pOutput->m_pIndexBuffer)
		{
			CMesh::_AddRef(*pOutput->m_pIndexBuffer);
			//pOutput->m_pIndexBuffer->AddRef();

			if( pOutput->_Create() )
				return pOutput;
		}

		DestroyResource(*pOutput);
	}

	return ALCHEMY_NULL;
}


CMesh* CSceneManager::CreateMesh(const CMeshManager::MESHDATA MeshData)
{
	CMesh* pMesh = CreateMesh(
		MeshData.uNumVertices,
		MeshData.uNumIndices, 
		MeshData.uIndexUsage, 
		MeshData.IndexFormat, 
		MeshData.VertexElementData.pVertexElement, 
		MeshData.VertexElementData.uElementCount,
		MeshData.VertexElementData.puUsage);

	if(pMesh)
	{
		UINT i;
		if(MeshData.pAttributeRange)
		{
			for(i = 0; i < MeshData.uAttributeRangeCount; i ++)
				pMesh->m_AttributeTable.Add(MeshData.pAttributeRange[i]);
		}

		PVOID pData;
		if(MeshData.ppVertexData && pMesh->m_pVertex->m_ppVertexBuffer)
		{
			LPVERTEXBUFFER pVertexBuffer;
			UINT uVertexBufferCount = pMesh->m_pVertex->m_pVertexDeclaration->GetStreamCount();
			for(i = 0; i < uVertexBufferCount; i ++)
			{
				pVertexBuffer = pMesh->m_pVertex->m_ppVertexBuffer[i];

				if(pVertexBuffer && MeshData.ppVertexData[i])
				{
					pData = pVertexBuffer->Lock(0);

					if(pData)
					{
						ALCHEMY_MEMORY_COPY( pData, MeshData.ppVertexData[i], pVertexBuffer->GetByteSize() );

						pVertexBuffer->Unlock();
					}
				}
			}
		}

		if(MeshData.pIndexData)
		{
			LPINDEXBUFFER pIndexBuffer = pMesh->m_pIndexBuffer;

			pData = pIndexBuffer->Lock(0);

			if(pData)
			{
				ALCHEMY_MEMORY_COPY( pData, MeshData.pIndexData, pIndexBuffer->GetByteSize() );

				pIndexBuffer->Unlock();
			}
		}
	}

	return pMesh;
}

CMaterial* CSceneManager::CreateMaterial(const CMeshManager::MATERIALDATA* pMaterialData)
{
	CMaterial* pOutput;
	if( pOutput = static_cast<CMaterial*>( CreateResource(CGraphicsResource::MATERIAL) ) )
	{
		if(pMaterialData)
		{
			pOutput->m_Ambient  = pMaterialData->Ambient;
			pOutput->m_Diffuse  = pMaterialData->Diffuse;
			pOutput->m_Specular = pMaterialData->Specular;
			pOutput->m_Emissive = pMaterialData->Emissive;

			pOutput->m_fPower   = pMaterialData->fPower;
			
			if(pMaterialData->pTextureData)
			{
				CMeshManager::LPTEXTUREDATA pTextureData;
				LPTEXTURE pTexture;
				for(UINT i = 0; i < pMaterialData->uTextureCount; i ++)
				{
					pTextureData = &pMaterialData->pTextureData[i];

					pTexture = m_RenderSystem.CreateTexture(pTextureData->uWidth, pTextureData->uHeight, pTextureData->uUsage, pTextureData->Format, pTextureData->uMipLevel, pTextureData->pData);

					if(pTexture)
					{
						//pTexture->AddRef();
						CMaterial::_AddRef(*pOutput);

						pOutput->m_Textures.Add(pTexture);
					}
					else
					{
						ALCHEMY_DEBUG_WARNING("CSceneManager::CreateMaterial: Material has been created fail: texture can not be created.");

						DestroyResource(*pOutput);

						return ALCHEMY_NULL;
					}
				}
			}
		}

		if( pOutput->_Create() )
			return pOutput;

		DestroyResource(*pOutput);
	}

	return ALCHEMY_NULL;
}

CLight* CSceneManager::CreateLight()
{
	CLight* pOutput;
	if( pOutput = static_cast<CLight*>( CreateResource(CGraphicsResource::LIGHT) ) )
	{
		if( pOutput->_Create() )
			return pOutput;

		DestroyResource(*pOutput);
	}

	return ALCHEMY_NULL;
}

CRenderMethod* CSceneManager::CreateRenderMethod(const CRenderMethod::PARAMETERLIST* pParameterList, UINT uPassCount, const CHAR* pCode, CBuffer** ppCompilationErrors, CEffectCode::CEffectParameterHandlerInterface* pEffectParameterHandler)
{
	CRenderMethod* pOutput;
	if( pOutput = static_cast<CRenderMethod*>( CreateResource(CGraphicsResource::RENDER_METHOD) ) )
	{
		pOutput->m_pEffect = m_RenderSystem.CreateEffect(uPassCount, pCode, ppCompilationErrors);

		if(pOutput->m_pEffect)
		{
			CRenderMethod::_AddRef(*pOutput->m_pEffect);

			pOutput->m_pParameterListBuffer = &CRenderMethod::CreateParameterListBuffer(pParameterList, uPassCount);

			pOutput->m_pParameterList = reinterpret_cast<CRenderMethod::LPPARAMETERLIST>( pOutput->m_pParameterListBuffer->GetBufferPointer() );

			if(pEffectParameterHandler)
			{
				pOutput->m_pEffectParameterHandler = pEffectParameterHandler;

				pEffectParameterHandler->AddRef();
			}

			if( pOutput->_Create() )
				return pOutput;

			DestroyResource(*pOutput);

			return ALCHEMY_NULL;
		}

		ALCHEMY_DEBUG_WARNING("CSceneManager::CreateRenderMethod: Render method has been created fail: render method can not be created.");

		DestroyResource(*pOutput);
	}

	return ALCHEMY_NULL;
}

CRenderMethod* CSceneManager::CreateRenderMethod(const CRenderMethod::PARAMETERLIST* pParameterList, CGraphicsResource::HANDLE Effect, CEffectCode::CEffectParameterHandlerInterface* pEffectParameterHandler)
{
	CRenderMethod* pOutput;
	if( pOutput = static_cast<CRenderMethod*>( CreateResource(CGraphicsResource::RENDER_METHOD) ) )
	{
		pOutput->m_pEffect = static_cast<CEffect*>( m_RenderSystem.GetResource(Effect, CGraphicsResource::EFFECT) );

		if(pOutput->m_pEffect)
		{
			CRenderMethod::_AddRef(*pOutput->m_pEffect);

			pOutput->m_pEffect->AddRef();

			pOutput->m_pParameterListBuffer = &CRenderMethod::CreateParameterListBuffer( pParameterList, pOutput->m_pEffect->GetTotalPass() );

			pOutput->m_pParameterList = reinterpret_cast<CRenderMethod::LPPARAMETERLIST>( pOutput->m_pParameterListBuffer->GetBufferPointer() );

			if(pEffectParameterHandler)
			{
				pOutput->m_pEffectParameterHandler = pEffectParameterHandler;

				pEffectParameterHandler->AddRef();
			}

			if( pOutput->_Create() )
				return pOutput;

			DestroyResource(*pOutput);

			return ALCHEMY_NULL;
		}

		ALCHEMY_DEBUG_WARNING("CSceneManager::CreateRenderMethod: Render method has been created fail: render method can not be created.");

		DestroyResource(*pOutput);
	}

	return ALCHEMY_NULL;
}

CRenderMethod* CSceneManager::CreateRenderMethod(CEffectCode& EffectCode, CGraphicsResource::HANDLE Effect, CBuffer** ppCompilationErrors)
{
	CRenderMethod* pOutput;
	if( ( pOutput = static_cast<CRenderMethod*>( CreateResource(CGraphicsResource::RENDER_METHOD) ) ) != ALCHEMY_NULL )
	{
		pOutput->m_pEffect = ALCHEMY_NULL;

		if(Effect)
			pOutput->m_pEffect = static_cast<CEffect*>( m_RenderSystem.GetResource(Effect, CGraphicsResource::EFFECT) );

		if(!pOutput->m_pEffect)
			pOutput->m_pEffect = m_RenderSystem.CreateEffect(EffectCode, ppCompilationErrors);
		else
			pOutput->m_pEffect->AddRef();

		if(pOutput->m_pEffect)
		{
			CRenderMethod::_AddRef(*pOutput->m_pEffect);

			CEffectCode::LPGLOBALINPUTHANDLER pGlobalInputHandler = ALCHEMY_NULL;
			pOutput->m_pParameterListBuffer = &EffectCode.CreateParameterList(&pGlobalInputHandler);

			pOutput->m_pParameterList = reinterpret_cast<CRenderMethod::LPPARAMETERLIST>( pOutput->m_pParameterListBuffer->GetBufferPointer() );

			if(pGlobalInputHandler)
			{
				pOutput->m_pEffectParameterHandler = pGlobalInputHandler;

				pGlobalInputHandler->AddRef();
			}

			if( pOutput->_Create() )
				return pOutput;

			DestroyResource(*pOutput);

			return ALCHEMY_NULL;
		}

		ALCHEMY_DEBUG_WARNING("CSceneManager::CreateRenderMethod: Render method has been created fail: render method can not be created.");

		DestroyResource(*pOutput);
	}

	return ALCHEMY_NULL;
}

CSceneObject* CSceneManager::CreateSceneObject(CGraphicsResource::HANDLE Mesh, UINT16 uSubset, CGraphicsResource::HANDLE Material, CGraphicsResource::HANDLE RenderMethod, CSceneObject* pInput)
{
	CSceneObject& Object = pInput ? *pInput : m_SceneNodeFactory.CreateSceneObject();

	//Object.Destroy();

	Object.m_pSceneManager = this;

	Object.m_uSubset       = uSubset;
	Object.m_pSourceMesh   = static_cast<CMesh*>( GetResource(Mesh, CGraphicsResource::MESH) );
	Object.m_pMaterial     = static_cast<CMaterial*>( GetResource(Material, CGraphicsResource::MATERIAL) );

	if(Object.m_pSourceMesh && Object.m_pMaterial)
	{
		const CMesh::ATTRIBUTERANGE* pAttributeRange = Object.m_pSourceMesh->GetAttributeTable().Get(uSubset);

		if(!pAttributeRange)
		{
			ALCHEMY_DEBUG_WARNING("CSceneManager::CreateSceneObject: Scene object has been created fail: Error subset.");

			Object.Destroy();

			return ALCHEMY_NULL;
		}

		Object.__AddRenderData(*pAttributeRange);

		Object.m_pDestinationMesh = Object.m_pSourceMesh;

		Object.m_pSourceMesh->AddRef();
		Object.m_pDestinationMesh->AddRef();
		Object.m_pMaterial->AddRef();

		CRenderMethod* pRenderMethod = RenderMethod ? static_cast<CRenderMethod*>( GetResource(RenderMethod, CGraphicsResource::RENDER_METHOD) ) : ALCHEMY_NULL;

		if( pRenderMethod && Object.SetRenderMethod(*pRenderMethod) )
			pRenderMethod->AddRef();

		Object.m_uSceneIndex = m_SceneObjectPool.Add(&Object);

		return &Object;
	}

	ALCHEMY_DEBUG_WARNING("CSceneManager::CreateSceneObject: Scene object has been created fail: mesh or material is null.");

	Object.Destroy();

	return ALCHEMY_NULL;
}

CSceneNode  * CSceneManager::CreateScene(const CMeshManager::SCENEDATA& SceneData, CGraphicsResource::HANDLE Mesh, CGraphicsResource::HANDLE RenderMethod)
{
	CSceneNode** ppSceneNode;
	UINT i;

	CMaterial* pMaterial;
	CMesh* pMesh = static_cast<CMesh*>( GetResource(Mesh, CGraphicsResource::MESH) );

	if(!pMesh)
	{
		ALCHEMY_DEBUG_WARNING("CSceneManager::CreateScene: Error mesh handle.");

		return ALCHEMY_NULL;
	}

	ALCHEMY_DEBUG_NEW(ppSceneNode, CSceneNode*[SceneData.uSceneDataCount]);

	for(i = 0; i < SceneData.uSceneDataCount; ++ i)
	{
		switch(SceneData.pSceneNodeData->Type)
		{
		case CMeshManager::OBJECT:
			pMaterial = CreateMaterial(&SceneData.pSceneNodeData[i].MaterialData);
			if(pMaterial)
			{
				pMaterial->AddRef();
				pMesh->AddRef();

				ppSceneNode[i] = CreateSceneObject(Mesh, SceneData.pSceneNodeData->uSubset, pMaterial->GetResourceHandle(), RenderMethod);
			}
			else
			{
				for(UINT j = 0; j < i; ++ j)
					m_SceneNodeFactory.DestroySceneNode(*ppSceneNode[j]);

				ALCHEMY_DEBUG_DELETE_ARRAY(ppSceneNode);

				return ALCHEMY_NULL;
			}
			break;

		case CMeshManager::BONE:
			ppSceneNode[i] = &m_SceneNodeFactory.CreateBone();
			break;

		default:
			ppSceneNode[i] = &m_SceneNodeFactory.CreateSceneNode();
		}
	}

	CSceneNode& Scene = m_SceneNodeFactory.CreateSceneNode();
	UINT uParent;
	for(i = 0; i < SceneData.uSceneDataCount; ++ i)
	{
		uParent = SceneData.pSceneNodeData[i].uParent;
		if(uParent)
			ppSceneNode[uParent + 1]->AddChild(*ppSceneNode[i]);
		else
			Scene.AddChild(*ppSceneNode[i]);
	}

	ALCHEMY_DEBUG_DELETE_ARRAY(ppSceneNode);

	return &Scene;
}

void CSceneManager::BuildSceneEffectPass(CEffectCode::CPassDefiner& PassDefiner, UINT uLightFlag)
{
	m_LightEffect.EnableEffect(uLightFlag);
	m_LightEffect.BuildLightEffectPass(PassDefiner);
}

CLight* CSceneManager::AddLight(CGraphicsResource::HANDLE Light, COldLightEffect::SHADEMODE LightShadeMode)
{
	CLight* pLight = static_cast<CLight*>( GetResource(Light, CGraphicsResource::LIGHT) );

	if(pLight)
	{
		m_LightEffect.AddLight(pLight->GetLightType(), LightShadeMode);

		m_Lights.Add(pLight);

		return pLight;
	}

	ALCHEMY_DEBUG_WARNING("CSceneManager::AddLight: Error handle.");

	return ALCHEMY_NULL;
}

bool CSceneManager::AddChild(CSceneNode& Child)
{
	Child.AddedToRenderQueue();

	return m_Scene.AddChild(Child);
}

bool CSceneManager::RemoveChild(CSceneNode& Child)
{
	return m_Scene.RemoveChild(Child);
}

void CSceneManager::Activate()
{
	sm_pInstance = this;
}

void CSceneManager::Destroy()
{
	if(m_pSceneRenderTarget)
	{
		m_pSceneRenderTarget->Release();

		m_pSceneRenderTarget = ALCHEMY_NULL;
	}

	CPool<CSceneObject*>::CIterator Iterator = m_SceneObjectPool.GetBeginIterator();
	while( CPool<CSceneObject*>::IsValuable(Iterator) )
	{
		(*Iterator)->Destroy();

		++ Iterator;
	}

	m_SceneObjectPool.Clear();

	CGraphicsResourcePool::Destroy();

	ALCHEMY_DEBUG_DELETE(m_pRenderQueue);
}

void CSceneManager::ResetScene()
{
	for (UINT i = 0; i < EFFECT_TYPE_COUNT; ++ i)
		m_pRenderEntryList[i] = ALCHEMY_NULL;

	m_Scene.AddedToRenderQueue();
}

void CSceneManager::RenderNormalScene(UINT uClearFlag)
{
	m_RenderSystem.Clear(m_BackgroundColor, uClearFlag);

	__RenderScene(EFFECT_NORMAL);
}

void CSceneManager::Update(FLOAT fElapseTime)
{
	m_pCamera->Update();
	m_Scene.Update();

	m_fElapsedTime = fElapseTime;
	m_fTime += fElapseTime;
}


void CSceneManager::Render()
{
	static bool s_bIsRendering = false;

	if(s_bIsRendering)
	{
		ALCHEMY_DEBUG_WARNING("CSceneManager::Render: Scene manager is rendering.");

		return;
	}

	s_bIsRendering = true;

	m_RenderSystem.SetRenderState(CRenderSystem::CULL_MODE, CRenderSystem::CCW);

	__RenderScene(EFFECT_BEFOREHAND);

	if(m_SceneRenderTarget)
		m_RenderSystem.SetRenderTarget(m_SceneRenderTarget, 0, 0);

	m_RenderSystem.Clear( m_BackgroundColor, ALCHEMY_FLAG(CRenderSystem::TARGET) | ALCHEMY_FLAG(CRenderSystem::ZBUFFER) | ALCHEMY_FLAG(CRenderSystem::STENCIL) );

	__RenderScene(EFFECT_NORMAL);
	__RenderScene(EFFECT_FINAL);

	if(m_SceneRenderTarget)
	{
		m_RenderSystem.SetRenderTarget(0, 0, 0);

		m_RenderSystem.Clear( m_BackgroundColor, ALCHEMY_FLAG(CRenderSystem::TARGET) | ALCHEMY_FLAG(CRenderSystem::ZBUFFER) | ALCHEMY_FLAG(CRenderSystem::STENCIL) );
	}

	__RenderScene(EFFECT_2D);

	s_bIsRendering = false;
}

CGraphicsResource* CSceneManager::_Create(CGraphicsResource::TYPE Type)
{
	CGraphicsResource* pResource = ALCHEMY_NULL;

	switch(Type)
	{
	case CGraphicsResource::VERTEX:
		ALCHEMY_DEBUG_NEW(pResource, CVertex);
		break;
	case CGraphicsResource::MESH:
		ALCHEMY_DEBUG_NEW(pResource, CMesh);
		break;
	case CGraphicsResource::MATERIAL:
		ALCHEMY_DEBUG_NEW(pResource, CMaterial);
		break;
	case CGraphicsResource::RENDER_METHOD:
		ALCHEMY_DEBUG_NEW(pResource, CRenderMethod);
		break;
	case CGraphicsResource::LIGHT:
		ALCHEMY_DEBUG_NEW(pResource, CLight);
		break;
	}

	return pResource;
}

void CSceneManager::__RenderScene(RENDERENTRYTYPE Type)
{
	if(Type >= EFFECT_TYPE_COUNT || Type < 0)
		return;

	LPRENDERLISTNODE pCurrentRenderNode = m_pRenderEntryList[Type];
	if(!pCurrentRenderNode)
		return;

	if(!m_pFinalRenderEntry[Type] && Type != EFFECT_FINAL)
	{
		pCurrentRenderNode = m_pRenderEntryList[Type] = ALCHEMY_LIST_SORT(RENDERENTRY, RENDERENTRYLESS)(pCurrentRenderNode);

		if(pCurrentRenderNode->Data.pRenderer)
			pCurrentRenderNode->Data.pRenderer->Render(pCurrentRenderNode->Data.RenderEntry, pCurrentRenderNode->Data.uFlag = ~0);

		LPRENDERLISTNODE pPreviousRenderNode = pCurrentRenderNode;
		pCurrentRenderNode = pCurrentRenderNode->pNext;

		while(pCurrentRenderNode)
		{
			if(pCurrentRenderNode->Data.pRenderer)
				pCurrentRenderNode->Data.pRenderer->Render(
				pCurrentRenderNode->Data.RenderEntry, 
				pCurrentRenderNode->Data.uFlag = CRenderQueue::BuildRenderFlag(
				pCurrentRenderNode->Data.RenderEntry, 
				pPreviousRenderNode->Data.RenderEntry, 
				pCurrentRenderNode->Data.pRenderer != pPreviousRenderNode->Data.pRenderer) );

			pPreviousRenderNode = pCurrentRenderNode;
			pCurrentRenderNode  = pCurrentRenderNode->pNext;
		}

		m_pFinalRenderEntry[Type] = &pPreviousRenderNode->Data;

		if(Type == EFFECT_NORMAL)
		{
			pCurrentRenderNode = m_pRenderEntryList[EFFECT_FINAL];

			while(pCurrentRenderNode)
			{
				pCurrentRenderNode->Data.uFlag = CRenderQueue::BuildRenderFlag(
				pCurrentRenderNode->Data.RenderEntry, 
				pPreviousRenderNode->Data.RenderEntry, 
				pCurrentRenderNode->Data.pRenderer != pPreviousRenderNode->Data.pRenderer);

				pCurrentRenderNode  = pCurrentRenderNode->pNext;
			}
		}
	}
	else
	{
		while (pCurrentRenderNode)
		{
			if(pCurrentRenderNode->Data.pRenderer)
				pCurrentRenderNode->Data.pRenderer->Render(pCurrentRenderNode->Data.RenderEntry, pCurrentRenderNode->Data.uFlag);

			pCurrentRenderNode = pCurrentRenderNode->pNext;
		}
	}

	if(Type < EFFECT_FINAL)
	{
		pCurrentRenderNode = m_pDynamicRenderEntryList[Type];
		while(pCurrentRenderNode)
		{
			if(pCurrentRenderNode->Data.pRenderer)
				m_pRenderQueue->UnlockRenderEntry(pCurrentRenderNode->Data.RenderEntry, *pCurrentRenderNode->Data.pRenderer);

			pCurrentRenderNode = pCurrentRenderNode->pNext;
		}

		m_pRenderQueue->Render();

		m_pDynamicRenderEntryList[Type] = ALCHEMY_NULL;
	}
}