#include "AlchemySceneManager.h"
#include "AlchemyFullScreenDrawer.h"
#include "AlchemySceneNodeFactory.h"

#include "../Core/AlchemyMemory.h"
#include "../Core/AlchemyUtils.h"

#include "AlchemySceneObject.h"
#include "AlchemyBone.h"
#include "AlchemySceneNodeUpdaer.h"

#include "AlchemySkinMeshProcessor.h"

#include "AlchemyRenderGroupInterface.h"

using namespace alchemy;

CSceneManager* CSceneManager::sm_pInstance = ALCHEMY_NULL;

CSceneManager::CSceneEventListener::CSceneEventListener(CSceneManager& Parent) : 
m_SceneManager(Parent)
{
}

CSceneManager::CSceneEventListener::~CSceneEventListener(void)
{

}

void CSceneManager::CSceneEventListener::Handler(const IEventDispatcher::CEvent& Event)
{
	m_SceneManager.m_bIsSceneDirty = true;
}

CSceneManager::CViewportEventListener::CViewportEventListener(CSceneManager& Parent) : 
m_SceneManager(Parent)
{
}

CSceneManager::CViewportEventListener::~CViewportEventListener(void)
{
 
}

void CSceneManager::CViewportEventListener::Handler(const IEventDispatcher::CEvent& Event)
{
	UINT uWidth, uHeight;
	if( m_SceneManager.m_RenderSystem.GetViewport(&uWidth, &uHeight) )
	{
		UINT uEventType;
		if( ALCHEMY_GET_EVENT_TYPE(uEventType, PROJECTION_CHANGE, CCamera) && uEventType == Event.GetType() )
			m_SceneManager.m_SceneCamera.SetProjection(uWidth * 1.0f / uHeight);
		else
			m_SceneManager.m_SceneCamera.SetProjection(m_SceneManager.m_SceneCamera.GetFOV(), m_SceneManager.m_SceneCamera.GetNearPlane(), m_SceneManager.m_SceneCamera.GetFarPlane(), uWidth * 1.0f / uHeight);
	}
}

CSceneNodeFactory& CreateSceneNodeFactory()
{
	CSceneNodeFactory* pSceneNodeFactory;
	ALCHEMY_DEBUG_NEW(pSceneNodeFactory, CSceneNodeFactory);
	
	return *pSceneNodeFactory;
}

CSceneManager::CSceneManager(CRenderSystem& RenderSystem, ISceneNodeFactory* pSceneNodeFactory) :
CGraphicsResourcePool(CGraphicsResource::BEGIN_SCENE_RESOURCE, CGraphicsResource::END_SCENE_RESOURCE),
m_RenderSystem(RenderSystem),
m_SceneNodeFactory( pSceneNodeFactory ? *pSceneNodeFactory : CreateSceneNodeFactory() ),
m_Scene( m_SceneNodeFactory.CreateSceneNode() ),
m_SceneCamera( m_SceneNodeFactory.CreateCamera() ),
m_RenderQueue( RenderSystem, ALCHEMY_NULL, &m_SceneCamera.GetFrustum() ),
m_DeferredRenderingPipeline(m_RenderQueue),
//m_pCamera(&m_SceneCamera),
m_pFullScreenDrawer(ALCHEMY_NULL),
m_fElapsedTime(0.0f),
m_fTime(0.0f),
m_BackgroundColor(0),
m_bIsSceneDirty(false),
m_SceneEventListener(*this), 
m_ViewportEventListener(*this)
{
	sm_pInstance = this;

	UINT uEventType;

	if( ALCHEMY_GET_EVENT_TYPE(uEventType, ADD_CHILD, CSceneNode) )
		m_Scene.AddEventListener(uEventType, m_SceneEventListener);

	if( ALCHEMY_GET_EVENT_TYPE(uEventType, REMOVE_CHILD, CSceneNode) )
		m_Scene.AddEventListener(uEventType, m_SceneEventListener);

	if( ALCHEMY_GET_EVENT_TYPE(uEventType, SCENE_DIRTY, CSceneNode) )
		m_Scene.AddEventListener(uEventType, m_SceneEventListener);

	if( ALCHEMY_GET_EVENT_TYPE(uEventType, PROJECTION_CHANGE, CCamera) )
		m_SceneCamera.AddEventListener(uEventType, m_ViewportEventListener);

	if( ALCHEMY_GET_EVENT_TYPE(uEventType, RESET_VIEWPORT, CRenderSystem) )
		m_RenderSystem.AddEventListener(uEventType, m_ViewportEventListener);

	m_Cameras.Add(&m_SceneCamera);
}

CSceneManager::~CSceneManager(void)
{
	
}

CSceneManager::IFullScreenDrawer* CSceneManager::GetFullScreenDrawer()
{
	if(!m_pFullScreenDrawer)
	{
		CFullScreenDrawer* pFullScreenDrawer;
		ALCHEMY_DEBUG_NEW(pFullScreenDrawer, CFullScreenDrawer);
		CSceneManager* pSceneManager = sm_pInstance;
		sm_pInstance = this;
		if( !pFullScreenDrawer->Create() )
		{
			ALCHEMY_DEBUG_DELETE(pFullScreenDrawer);
			sm_pInstance = pSceneManager;

			return ALCHEMY_NULL;
		}
		sm_pInstance = pSceneManager;
		m_pFullScreenDrawer = pFullScreenDrawer;
	}

	return m_pFullScreenDrawer;
}

//CSceneManager::ISceneDrawer* CSceneManager::GetSceneDrawer()
//{
//	if(!m_pSceneDrawer)
//	{
//		CSceneDrawer* pSceneDrawer;
//		ALCHEMY_DEBUG_NEW(pSceneDrawer, CSceneDrawer);
//		pSceneDrawer->Create(*sm_pInstance);
//		m_pSceneDrawer = pSceneDrawer;
//	}
//
//	return m_pSceneDrawer;
//}

bool CSceneManager::SetCamera(const CCamera* pCamera, UINT uIndex)
{
	return m_Cameras.Set(pCamera, uIndex);
}

/*CLight* CSceneManager::SetLight(CGraphicsResource::HANDLE Light, CLightManager::SHADEMODE LightShadeMode, UINT uIndex)
{
	CLight* pLight = static_cast<CLight*>( GetResource(Light, CGraphicsResource::LIGHT) );

	if(pLight)
	{
		if( m_LightManager.SetLight(*pLight, LightShadeMode, uIndex) )
			return pLight;

		ALCHEMY_DEBUG_WARNING("CSceneManager::SetLight: Set light fail.");

		return ALCHEMY_NULL;
	}

	ALCHEMY_DEBUG_WARNING("CSceneManager::AddLight: Error handle.");

	return ALCHEMY_NULL;
}*/

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

	CVertex* pOutput = static_cast<CVertex*>( CreateResource(CGraphicsResource::VERTEX) );
	if(pOutput)
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

	if(pVertexDeclaration)
	{
		CVertex* pOutput = CreateVertex(uNumVertices, pVertexDeclaration->GetResourceHandle(), puUsage);

		if(pOutput)
			return pOutput;

		pVertexDeclaration->Release();
	}

	return ALCHEMY_NULL;
}

CMesh* CSceneManager::CreateMesh(UINT uNumVertices, UINT uNumIndices, UINT uIndexUsage, CIndexBuffer::FORMAT IndexFormat, CGraphicsResource::HANDLE VertexDeclaration, PUINT puVertexUsage)
{
	CMesh* pOutput = static_cast<CMesh*>( CreateResource(CGraphicsResource::MESH) ) ;
	if(pOutput)
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
	CMesh* pOutput = static_cast<CMesh*>( CreateResource(CGraphicsResource::MESH) ) ;
	if(pOutput)
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


CMesh* CSceneManager::CreateMesh(UINT uMeshDataHandle)
{
	const CMeshManager::MESHDATA* pMeshData = ALCHEMY_MESH_MANAGER.GetMeshData(uMeshDataHandle);
	if(!pMeshData)
		return ALCHEMY_NULL;

	CMesh* pMesh = CreateMesh(
		pMeshData->uNumVertices,
		pMeshData->uNumIndices, 
		pMeshData->uIndexUsage, 
		pMeshData->IndexFormat, 
		pMeshData->VertexElementData.pVertexElement, 
		pMeshData->VertexElementData.uElementCount,
		pMeshData->VertexElementData.puUsage);

	if(pMesh)
	{
		UINT i;
		if(pMeshData->pAttributeRange)
		{
			for(i = 0; i < pMeshData->uAttributeRangeCount; i ++)
				pMesh->m_AttributeTable.Add(pMeshData->pAttributeRange[i]);
		}

		void* pData;
		if(pMeshData->ppVertexData && pMesh->m_pVertex->m_ppVertexBuffer)
		{
			LPVERTEXBUFFER pVertexBuffer;
			UINT uVertexBufferCount = pMesh->m_pVertex->m_pVertexDeclaration->GetStreamCount();
			for(i = 0; i < uVertexBufferCount; i ++)
			{
				pVertexBuffer = pMesh->m_pVertex->m_ppVertexBuffer[i];

				if(pVertexBuffer && pMeshData->ppVertexData[i])
				{
					pData = pVertexBuffer->Lock(0);

					if(pData)
					{
						ALCHEMY_MEMORY_COPY( pData, pMeshData->ppVertexData[i], pVertexBuffer->GetByteSize() );

						pVertexBuffer->Unlock();
					}
				}
			}
		}

		if(pMeshData->pIndexData)
		{
			LPINDEXBUFFER pIndexBuffer = pMesh->m_pIndexBuffer;

			pData = pIndexBuffer->Lock(0);

			if(pData)
			{
				ALCHEMY_MEMORY_COPY( pData, pMeshData->pIndexData, pIndexBuffer->GetByteSize() );

				pIndexBuffer->Unlock();
			}
		}
	}

	return pMesh;
}

CMaterial* CSceneManager::CreateMaterial( CMeshManager::HANDLE uMaterialDataHandle, CMap<CMeshManager::HANDLE, CGraphicsResource::HANDLE>* pTextureCache, CVector<RESOURCEEINFO>* pTextureInfo)
{
	CMaterial* pOutput = static_cast<CMaterial*>( CreateResource(CGraphicsResource::MATERIAL) ) ;
	if(pOutput)
	{
		RESOURCEEINFO Info = {0};

		const CMeshManager::MATERIALDATA* pMaterialData = ALCHEMY_MESH_MANAGER.GetMaterialData(uMaterialDataHandle);
		if(pMaterialData)
		{
			pOutput->m_Ambient  = pMaterialData->Ambient;
			pOutput->m_Diffuse  = pMaterialData->Diffuse;
			pOutput->m_Specular = pMaterialData->Specular;
			pOutput->m_Emissive = pMaterialData->Emissive;

			pOutput->m_fPower   = pMaterialData->fPower;

			if(pMaterialData->pTextureData)
			{
				const CMeshManager::TEXTUREDATA* pTextureData;
				LPTEXTURE pTexture;
				CGraphicsResource::HANDLE* pCacheHandle;
				for(UINT i = 0; i < pMaterialData->uTextureCount; i ++)
				{
					pTextureData = ALCHEMY_MESH_MANAGER.GetTextureData( pMaterialData->pTextureData[i] );
					if(!pTextureData)
					{
						ALCHEMY_DEBUG_WARNING("CSceneManager::CreateMaterial: Material has been created fail: texture can not be created.");

						DestroyResource(*pOutput);

						return ALCHEMY_NULL;
					}

					if(pTextureCache && ( pCacheHandle = pTextureCache->Get( pMaterialData->pTextureData[i] ) ) != ALCHEMY_NULL )
						pTexture = static_cast<LPTEXTURE>( m_RenderSystem.GetResource(*pCacheHandle, CGraphicsResource::TEXTURE) );
					else
					{
						pTexture = m_RenderSystem.CreateTexture(pTextureData->uWidth, pTextureData->uHeight, pTextureData->uUsage, pTextureData->Format, pTextureData->uMipLevel, pTextureData->pData);
						if(pTexture)
						{
							if(pTextureInfo)
							{
								Info.DataHandle = pMaterialData->pTextureData[i];
								Info.ResourceHandle = pTexture->GetResourceHandle();

								pTextureInfo->Add(Info);
							}
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
		}

		if( pOutput->_Create() )
			return pOutput;

		DestroyResource(*pOutput);
	}

	return ALCHEMY_NULL;
}

CMaterial* CSceneManager::CreateMaterial(const CMeshManager::MATERIALDATA* pMaterialData, const CMeshManager::TEXTUREDATA* pTextureData, UINT uTextureDataCount )
{
	CMaterial* pOutput = static_cast<CMaterial*>( CreateResource(CGraphicsResource::MATERIAL) ) ;
	if(pOutput)
	{
		if(pMaterialData)
		{
			pOutput->m_Ambient  = pMaterialData->Ambient;
			pOutput->m_Diffuse  = pMaterialData->Diffuse;
			pOutput->m_Specular = pMaterialData->Specular;
			pOutput->m_Emissive = pMaterialData->Emissive;

			pOutput->m_fPower   = pMaterialData->fPower;
			
			if(pTextureData)
			{
				const CMeshManager::TEXTUREDATA* pCurTextureData;
				LPTEXTURE pTexture;
				for(UINT i = 0; i < uTextureDataCount; i ++)
				{
					pCurTextureData = &pTextureData[i];

					pTexture = m_RenderSystem.CreateTexture(pCurTextureData->uWidth, pCurTextureData->uHeight, pCurTextureData->uUsage, pCurTextureData->Format, pCurTextureData->uMipLevel, pCurTextureData->pData);

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
	CLight* pOutput = static_cast<CLight*>( CreateResource(CGraphicsResource::LIGHT) );
	if(pOutput) 
	{
		if( pOutput->_Create() )
			return pOutput;

		DestroyResource(*pOutput);
	}

	return ALCHEMY_NULL;
}

CRenderMethod* CSceneManager::CreateRenderMethod(const CRenderMethod::PARAMETERLIST* pParameterList, UINT uPassCount, const UINT* puRenderTargetIndexCount, CRenderMethod::IShaderParametersHandler* pShaderParametesrHandler, CRenderMethod::IRenderStateHandler* pRenderStateHandler)
{
	if(!puRenderTargetIndexCount)
		return ALCHEMY_NULL;

	CRenderMethod* pOutput = static_cast<CRenderMethod*>( CreateResource(CGraphicsResource::RENDER_METHOD) );
	if(pOutput)
	{
		ALCHEMY_DEBUG_NEW(pOutput->m_puRenderTargetIndexCount, UINT[uPassCount]);
		ALCHEMY_DEBUG_NEW(pOutput->m_puRenderTargetIndexOffset, UINT[uPassCount]);

		UINT uParameterListCount = 0, i;
		for(i = 0; i < uPassCount; ++ i)
		{
			pOutput->m_puRenderTargetIndexOffset[i] = uParameterListCount;

			uParameterListCount += (pOutput->m_puRenderTargetIndexCount[i] = puRenderTargetIndexCount[i]);
		}

		pOutput->m_pParameterListBuffer = &CRenderMethod::CreateParameterListBuffer( pParameterList, uPassCount);

		pOutput->m_pParameterList = reinterpret_cast<CRenderMethod::LPPARAMETERLIST>( pOutput->m_pParameterListBuffer->GetBufferPointer() );

		pOutput->m_uPassCount = uPassCount;

		ALCHEMY_DEBUG_NEW(pOutput->m_puTextureCount, UINT[uParameterListCount]);
		ALCHEMY_DEBUG_NEW(pOutput->m_ppTextureAlphaIndexes, UINT*[uParameterListCount]);
		UINT j, uTextureCount;
		CRenderMethod::LPPARAMETERLIST pParameterList;
		for(i = 0; i < uParameterListCount; ++ i)
		{
			uTextureCount = 0;
			pParameterList = &pOutput->m_pParameterList[i];
			ALCHEMY_DEBUG_NEW( pOutput->m_ppTextureAlphaIndexes[i], UINT[ pParameterList->uTextureCount] );
			for(j = 0; j < pParameterList->uTextureCount; ++ j)
			{
				if(pParameterList->pTextures[j].Type == IEffect::PROGRAM)
					uTextureCount += 2;
				else
					++ uTextureCount;

				pOutput->m_ppTextureAlphaIndexes[i][j] = pParameterList->pTextures[j].uAlphaIndex;
			}

			pOutput->m_puTextureCount[i] = uTextureCount;
		}

		if(pShaderParametesrHandler)
		{
			pOutput->m_pShaderParametersHandler = pShaderParametesrHandler;

			pShaderParametesrHandler->AddRef();
		}
		
		if(pRenderStateHandler)
		{
			pOutput->m_pRenderStateHandler = pRenderStateHandler;
			
			pRenderStateHandler->AddRef();
		}

		if( pOutput->_Create() )
			return pOutput;

		DestroyResource(*pOutput);

		return ALCHEMY_NULL;
	}

	return ALCHEMY_NULL;
}

CRenderMethod* CSceneManager::CreateRenderMethod(CEffectCode& EffectCode, CEffectCode::CGlobalInputHandler* pShaderParametersHandler, CRenderMethod::IRenderStateHandler* pRenderStateHandler)
{
	CRenderMethod* pOutput;
	if( ( pOutput = static_cast<CRenderMethod*>( CreateResource(CGraphicsResource::RENDER_METHOD) ) ) != ALCHEMY_NULL )
	{
		pOutput->m_uPassCount = EffectCode.GetPassCount();

		ALCHEMY_DEBUG_NEW(pOutput->m_puRenderTargetIndexCount, UINT[pOutput->m_uPassCount]);

		pOutput->m_pParameterListBuffer = &EffectCode.CreateParameterList(&pShaderParametersHandler, pOutput->m_puRenderTargetIndexCount);

		pOutput->m_pParameterList = reinterpret_cast<CRenderMethod::LPPARAMETERLIST>( pOutput->m_pParameterListBuffer->GetBufferPointer() );

		ALCHEMY_DEBUG_NEW(pOutput->m_puRenderTargetIndexOffset, UINT[pOutput->m_uPassCount]);

		UINT uOffset = 0, i;
		for(i = 0; i < pOutput->m_uPassCount; ++ i)
		{
			pOutput->m_puRenderTargetIndexOffset[i] = uOffset;

			uOffset += pOutput->m_puRenderTargetIndexCount[i];
		}

		ALCHEMY_DEBUG_NEW(pOutput->m_puTextureCount, UINT[uOffset]);
		ALCHEMY_DEBUG_NEW(pOutput->m_ppTextureAlphaIndexes, UINT*[uOffset]);
		UINT j, uTextureCount;
		CRenderMethod::LPPARAMETERLIST pParameterList;
		for(i = 0; i < uOffset; ++ i)
		{
			uTextureCount = 0;
			pParameterList = &pOutput->m_pParameterList[i];
			ALCHEMY_DEBUG_NEW( pOutput->m_ppTextureAlphaIndexes[i], UINT[ pParameterList->uTextureCount] );
			for(j = 0; j < pParameterList->uTextureCount; ++ j)
			{
				if(pParameterList->pTextures[j].Type == IEffect::PROGRAM)
					uTextureCount += 2;
				else
					++ uTextureCount;

				pOutput->m_ppTextureAlphaIndexes[i][j] = pParameterList->pTextures[j].uAlphaIndex;
			}

			pOutput->m_puTextureCount[i] = uTextureCount;
		}

		if(pShaderParametersHandler)
		{
			pOutput->m_pShaderParametersHandler = pShaderParametersHandler;

			pShaderParametersHandler->AddRef();
		}
		
		if(pRenderStateHandler)
		{
			pOutput->m_pRenderStateHandler = pRenderStateHandler;
			
			pRenderStateHandler->AddRef();
		}

		if( pOutput->_Create() )
			return pOutput;

		DestroyResource(*pOutput);

		return ALCHEMY_NULL;
	}

	return ALCHEMY_NULL;
}

//#include "AlchemyPositionEffect.h"
//#include "AlchemyTextureEffect.h"

//#include <as3.h>
CSceneNode* CSceneManager::CreateScene(CMeshManager::HANDLE hSceneDataHandle, CSkinMeshController::SKINTYPE SkinType, CVector<CSceneObject*>* pSceneObjects, CVector<RESOURCEEINFO>* pMaterialInfo, CVector<RESOURCEEINFO>* pTextureInfo, CVector<RESOURCEEINFO>* pMeshInfo, CVector<CBone*>* pRootBone)
{
	const CMeshManager::SCENEDATA* pSceneData = ALCHEMY_MESH_MANAGER.GetSceneData(hSceneDataHandle);
	if(!pSceneData)
		return ALCHEMY_NULL;

	CMap<CMeshManager::HANDLE, CGraphicsResource::HANDLE> MaterialCache, TextureCache, MeshCache;
	CMap<CMeshManager::HANDLE, CSceneNode*> SceneCache;
	CVector<UINT> SkinSceneObject;

	RESOURCEEINFO ResInfo = {0};

	CSceneNodeUpdater* pRootScene = &m_SceneNodeFactory.CreateSceneNodeUpdater();
	pRootScene->Create(this);

	CMeshManager::HANDLE SceneNodeHandle;
	const CMeshManager::SCENENODEDATA* pNodeData;
	const CMeshManager::SKINDATA* pSkinData;
	CSceneNode** ppSceneNode;
	CSceneNode* pSceneNode;
	CSceneObject* pSceneObject;
	CBone* pBone;
	CGraphicsResource::HANDLE* pResHandle, MeshHandle, MaterialHandle;
	CMaterial* pMaterial;
	CMesh* pMesh;
	UINT i;

	for (i=0; i<pSceneData->uSceneDataCount; ++i)
	{
		SceneNodeHandle = pSceneData->pSceneNodeDatas[i];
		ppSceneNode = SceneCache.Get(SceneNodeHandle);
		if( !ppSceneNode || !(*ppSceneNode) )
		{
			pNodeData = ALCHEMY_MESH_MANAGER.GetSceneNodeData(SceneNodeHandle);
			if(pNodeData)
			{
				switch(pNodeData->Type)
				{
				case CMeshManager::NODE:
					pSceneNode = &m_SceneNodeFactory.CreateSceneNode();
					pSceneNode->SetMatrix( pNodeData->Transform );
					pSceneNode->SetName(pNodeData->pcName);

					SceneCache.Insert(SceneNodeHandle, pSceneNode);
					break;
				case CMeshManager::BONE:
					pBone = &m_SceneNodeFactory.CreateBone();
					pBone->SetMatrix( pNodeData->Transform );
					pBone->SetName(pNodeData->pcName);

					SceneCache.Insert(SceneNodeHandle, pBone);
					break;
				case CMeshManager::OBJECT:
					{
						pSceneObject = &m_SceneNodeFactory.CreateSceneObject();
						pSceneObject->SetMatrix( pNodeData->Transform );
						pSceneObject->SetName( pNodeData->pcName );

						if(pSceneObjects)
							pSceneObjects->Add(pSceneObject);

						pResHandle = MaterialCache.Get(pNodeData->MaterialData);
						if(pResHandle)
							MaterialHandle = *pResHandle;
						else
						{
							pMaterial = CreateMaterial(pNodeData->MaterialData, &TextureCache, pTextureInfo);
							if(!pMaterial)
							{
								ALCHEMY_DEBUG_WARNING("CSceneManager::CreateScene() - Error - Material Data");
								return ALCHEMY_NULL;
							}
							MaterialHandle = pMaterial->GetResourceHandle();
							MaterialCache.Insert(pNodeData->MaterialData, MaterialHandle);

							if(pMaterialInfo)
							{
								ResInfo.DataHandle = pNodeData->MaterialData;
								ResInfo.ResourceHandle = MaterialHandle;
								pMaterialInfo->Add(ResInfo);
							}
						}

						pResHandle = MeshCache.Get( pNodeData->MeshData );
						if(pResHandle)
						{
							MeshHandle = *pResHandle;
							pSceneObject->Create(MeshHandle, pNodeData->uSubset, 0, ALCHEMY_NULL, 0, MaterialHandle);
						}
						else
						{
							pMesh = CreateMesh(pNodeData->MeshData);
							if(!pMesh)
							{
								ALCHEMY_DEBUG_WARNING("CSceneManager::CreateScene() - Error - Mesh Data");
								return ALCHEMY_NULL;
							}

							MeshHandle = pMesh->GetResourceHandle();
							MeshCache.Insert(pNodeData->MeshData, MeshHandle);
							pSceneObject->Create(MeshHandle, pNodeData->uSubset, 0, ALCHEMY_NULL, 0, MaterialHandle);

							if(pNodeData->SkinData)
								SkinSceneObject.Add(i);

							if(pMeshInfo)
							{
								ResInfo.DataHandle = pNodeData->MeshData;
								ResInfo.ResourceHandle = MeshHandle;
								pMeshInfo->Add(ResInfo);
							}
						}

						SceneCache.Insert(SceneNodeHandle, pSceneObject);
					}

					break;
				}
			}
		}
	}

	for(i = 0; i < pSceneData->uSceneDataCount; ++ i)
	{
		SceneNodeHandle = pSceneData->pSceneNodeDatas[i];
		ppSceneNode = SceneCache.Get(SceneNodeHandle);
		if(ppSceneNode && (pSceneNode = *ppSceneNode) != ALCHEMY_NULL)
		{
			pNodeData = ALCHEMY_MESH_MANAGER.GetSceneNodeData(SceneNodeHandle);
			if(pNodeData)
			{
				ppSceneNode = SceneCache.Get(pNodeData->Parent);
				if(ppSceneNode && *ppSceneNode != ALCHEMY_NULL)
					(*ppSceneNode)->AddChild(*pSceneNode);
				else
					pRootScene->AddChild(*pSceneNode);
			}
		}
	}

	CMap<CMeshManager::HANDLE, CSkinInfo*>	SkinInfoMap;
	CSkinInfo** ppSkinInfo;
	CSkinInfo *pSkinInfo;
	IMeshProcessor* pMeshProcessor;
	CVector<UINT>::CIterator Iterator = SkinSceneObject.GetIterator(0);
	while( CVector<UINT>::IsValuable(Iterator) )
	{
		SceneNodeHandle = pSceneData->pSceneNodeDatas[*Iterator];
		ppSceneNode = SceneCache.Get(SceneNodeHandle);
		if(ppSceneNode && ( pSceneObject = dynamic_cast<CSceneObject*>(*ppSceneNode) ) != ALCHEMY_NULL)
		{
			pNodeData = ALCHEMY_MESH_MANAGER.GetSceneNodeData(SceneNodeHandle);

			ppSkinInfo = SkinInfoMap.Get(pNodeData->SkinData);
			if(ppSkinInfo)
				pSkinInfo = *ppSkinInfo;
			else
			{
				pSkinData = ALCHEMY_MESH_MANAGER.GetSkinData(pNodeData->SkinData);
				if(pSkinData)
				{
					ALCHEMY_DEBUG_NEW(pSkinInfo, CSkinInfo);
					pSkinInfo->Create( pSkinData->pBoneData, pSkinData->uBoneDataCount );
					SkinInfoMap.Insert( pNodeData->SkinData,  pSkinInfo);
				}
				else
					pSkinInfo = ALCHEMY_NULL;
			}

			ppSceneNode = SceneCache.Get(pNodeData->RootBone);
			if(ppSceneNode && ( pBone = dynamic_cast<CBone*>(*ppSceneNode) ) != ALCHEMY_NULL)
			{
				if(pRootBone)
					pRootBone->Add(pBone);

				ALCHEMY_DEBUG_NEW( pMeshProcessor, CSkinMeshProcessor( SkinType, *pSkinInfo, *pBone, ALCHEMY_NULL, 0, 30) );

				pRootScene->AddUpdater(*pMeshProcessor);

				pSceneObject->SetMeshProcessor(pMeshProcessor);
			}
		}

		++ Iterator;
	}

	return pRootScene;
}

/*CSceneObject* CSceneManager::CreateSceneObject(CGraphicsResource::HANDLE Mesh, UINT16 uSubset, CGraphicsResource::HANDLE Material, CGraphicsResource::HANDLE RenderMethod, CSceneObject* pInput)
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

		//Object.__AddRenderData(*pAttributeRange);

		Object.m_pDestinationMesh = Object.m_pSourceMesh;

		Object.m_pSourceMesh->AddRef();
		Object.m_pDestinationMesh->AddRef();
		Object.m_pMaterial->AddRef();

		CRenderMethod* pRenderMethod = RenderMethod ? static_cast<CRenderMethod*>( GetResource(RenderMethod, CGraphicsResource::RENDER_METHOD) ) : ALCHEMY_NULL;

		if( pRenderMethod && Object.SetRenderMethod(*pRenderMethod) )
			pRenderMethod->AddRef();

		return &Object;
	}

	ALCHEMY_DEBUG_WARNING("CSceneManager::CreateSceneObject: Scene object has been created fail: mesh or material is null.");

	Object.Destroy();

	return ALCHEMY_NULL;
}*/

/*CSceneNode  * CSceneManager::CreateScene(const CMeshManager::SCENEDATA& SceneData, CGraphicsResource::HANDLE Mesh, CGraphicsResource::HANDLE RenderMethod)
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
}*/

/*void CSceneManager::AddLightEffectInfoToPass(CEffectCode::CPassDefiner& PassDefiner, IEffectInfoBlendMethod* pEffectInfoBlendMethod, IEffectInfo* pSpecularEffect, IEffectInfo* pNormalMapEffect, UINT uOutputKey, UINT uInputKey )
{
	m_LightManager.AddEffectInfoToPass(PassDefiner, pEffectInfoBlendMethod, pSpecularEffect, pNormalMapEffect, uOutputKey, uInputKey);
}

CLight* CSceneManager::AddLight(CGraphicsResource::HANDLE Light, CLightManager::SHADEMODE LightShadeMode)
{
	CLight* pLight = static_cast<CLight*>( GetResource(Light, CGraphicsResource::LIGHT) );

	if(pLight)
	{
		m_LightManager.AddLight(*pLight, LightShadeMode);

		return pLight;
	}

	ALCHEMY_DEBUG_WARNING("CSceneManager::AddLight: Error handle.");

	return ALCHEMY_NULL;
}*/

/*UINT CSceneManager::AddCamera(const CCamera& Camera)
{
	return m_Cameras.Add(&Camera);
}*/

bool CSceneManager::AddChild(CSceneNode& Child)
{
	if( m_Scene.AddChild(Child) )
	{
		//Child.AddedToRenderQueue();

		//m_bIsSceneDirty = true;

		return true;
	}

	return false;
}

bool CSceneManager::RemoveChild(CSceneNode& Child)
{
	if( m_Scene.RemoveChild(Child) )
	{
		//m_bIsSceneDirty = true;

		return true;
	}

	return false;
}

void CSceneManager::AddRenderGroup( IRenderGroup& Render )
{
	UINT uOutput;
	if( !m_RenderGroups.GetIndexOf(uOutput, &Render) )
		m_RenderGroups.Add(&Render);
}

void CSceneManager::RemoveRenderGroup( IRenderGroup& Render )
{
	UINT uOutput;
	if( m_RenderGroups.GetIndexOf(uOutput, &Render) )
		m_RenderGroups.Remove(uOutput, 1);
}

void CSceneManager::Activate()
{
	sm_pInstance = this;
}

void CSceneManager::ResetScene()
{
	m_bIsSceneDirty = true;
}

void CSceneManager::RenderScene(UINT uClearFlag)
{
	m_RenderQueue.RenderScene(m_BackgroundColor, uClearFlag);
}

void CSceneManager::Destroy()
{
	m_SceneNodeFactory.Release();
	
	CGraphicsResourcePool::Destroy();
}

void CSceneManager::Update(FLOAT fElapseTime)
{
	m_SceneCamera.Update();

	m_Scene.Update();

	m_fElapsedTime = fElapseTime;
	m_fTime += fElapseTime;
}


void CSceneManager::Render()
{
	if(m_bIsSceneDirty)
	{
		__ResetScene();

		m_bIsSceneDirty = false;
	}

	UINT i, uLen = m_RenderGroups.GetLength();
	for (i=0; i<uLen; ++i)
		m_RenderGroups[i]->RenderAll();

	m_RenderQueue.Render(m_BackgroundColor);
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
    default:
        break;
	}

	return pResource;
}

void CSceneManager::__ResetScene()
{
	m_RenderQueue.ResetScene();

	m_DeferredRenderingPipeline.Activate();

	UINT i, uLen = m_RenderGroups.GetLength();
	for (i=0; i<uLen; ++i)
		m_RenderGroups[i]->Reset();

	m_Scene.AddedToRenderQueue();

	UINT uEventType;
	if( ALCHEMY_GET_EVENT_TYPE(uEventType, RESET_SCENE, CSceneManager) )
	{
		CEvent Event(uEventType);
		DispatchEvent(Event);
	}
}