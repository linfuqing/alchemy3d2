#include "Tower.h"

#include "Floor.h"

#include "../Core/AlchemyMTRandom.h"

#include "../Graphics/AlchemySceneManager.h"
#include "../Graphics/AlchemySceneObject.h"
#include "../Graphics/AlchemyPositionEffect.h"
#include "../Graphics/AlchemyTextureEffect.h"

#include "../Loader/AlchemyLoader.h"
#include "../Loader/AlchemyDAEParser.h"

using namespace alchemy;

namespace alchemy
{
	class CTowerLoadAllListener :
		public CSmartClass<IEventDispatcher::IListener>
	{
	public:
		CTowerLoadAllListener(CTower& Parent) : m_Parent(Parent) {}
		~CTowerLoadAllListener() {}

		void Handler(const IEventDispatcher::CEvent& Event)
		{
			if(m_Parent.m_pSceneManager)
			{
				const CLoader::CLoadAllEvent& LoadAllEvent = static_cast<const CLoader::CLoadAllEvent&>(Event);
				UINT uDataCount = LoadAllEvent.GetDataCount();
				if(uDataCount)
				{
					const CLoader::DATA* pData = ALCHEMY_LOADER.GetData(LoadAllEvent.GetDataIndices()[uDataCount - 1]);
					if(pData)
					{
						CDAEParser Parser;
						Parser.Parse(pData->pData);

						CMeshManager::HANDLE SceneData = ALCHEMY_MESH_MANAGER.AddSceneData( Parser.GetSceneData() );
						CVector<CSceneObject*> SceneObjects;
						m_Parent.m_pFoundation = m_Parent.m_pSceneManager->CreateScene(SceneData, CSkinMeshController::SKIN_NONE, &SceneObjects);
						ALCHEMY_MESH_MANAGER.DestroySceneData(SceneData);

						if(m_Parent.m_pFoundation)
						{
							m_Parent.m_SceneNode.AddChild(*m_Parent.m_pFoundation);

							CSceneObject* pSceneObject;
							CVector<CSceneObject*>::CIterator Iterator = SceneObjects.GetIterator(0);
							while( CVector<CSceneObject*>::IsValuable(Iterator) )
							{
								pSceneObject = *Iterator;
								if(pSceneObject)
								{
									pSceneObject->SetEffect(m_Parent.m_Effect);
									pSceneObject->SetRenderMethod(m_Parent.m_pRenderMethod);
								}

								++ Iterator;
							}
						}
					}

					if(uDataCount > 1)
					{
						const CLoader::DATA* pData = ALCHEMY_LOADER.GetData(LoadAllEvent.GetDataIndices()[0]);
						if(pData)
						{
							CDAEParser Parser;
							Parser.Parse(pData->pData);

							CMeshManager::HANDLE SceneData = ALCHEMY_MESH_MANAGER.AddSceneData( Parser.GetSceneData() );
							CVector<CSceneObject*> SceneObjects;
							m_Parent.m_pFirstFloor = m_Parent.m_pSceneManager->CreateScene(SceneData, CSkinMeshController::SKIN_NONE, &SceneObjects);
							ALCHEMY_MESH_MANAGER.DestroySceneData(SceneData);

							if(m_Parent.m_pFirstFloor)
							{
								m_Parent.m_SceneNode.AddChild(*m_Parent.m_pFirstFloor);

								CSceneObject* pSceneObject;
								CVector<CSceneObject*>::CIterator Iterator = SceneObjects.GetIterator(0);
								while( CVector<CSceneObject*>::IsValuable(Iterator) )
								{
									pSceneObject = *Iterator;
									if(pSceneObject)
									{
										pSceneObject->SetEffect(m_Parent.m_Effect);
										pSceneObject->SetRenderMethod(m_Parent.m_pRenderMethod);
									}

									++ Iterator;
								}
							}
						}
					}

					UINT uType;
					if( ALCHEMY_GET_EVENT_TYPE(uType, TOWER_LOADED_COMPELTE, CTower) )
					{
						IEventDispatcher::CEvent Event(uType);
						m_Parent.DispatchEvent(Event);
					}
				}
			}

			IEventDispatcher* pTarget = Event.GetTarget();
			if(pTarget)
				pTarget->RemoveEventListener(Event.GetType(), *this);
		}

	private:
		CTower& m_Parent;
	};

	class CFloorLoadAllListener : 
		public CSmartClass<IEventDispatcher::IListener>
	{
	public:
		CFloorLoadAllListener(CTower& Parent, UINT uType, UINT uIndex, CSceneNode*& pOutputSceneNode, CSceneNode* pSourceSceneNode, CGraphicsResource::HANDLE* pTexture) : 
		  m_Parent(Parent), m_uType(uType), m_uIndex(uIndex), m_pOutputSceneNode(pOutputSceneNode), m_pSourceSceneNode(pSourceSceneNode), m_pTexture(pTexture) {}
		~CFloorLoadAllListener() {}

		void Handler(const IEventDispatcher::CEvent& Event)
		{
			if(m_Parent.m_pSceneManager)
			{
				const CLoader::CLoadAllEvent& LoadAllEvent = static_cast<const CLoader::CLoadAllEvent&>(Event);
				UINT uDataCount = LoadAllEvent.GetDataCount();
				CVector<CSceneObject*> SceneObjects;
				if(uDataCount > 1)
				{
					const CLoader::DATA* pData = ALCHEMY_LOADER.GetData(LoadAllEvent.GetDataIndices()[0]);
					if(pData)
					{
						CDAEParser Parser;
						Parser.Parse(pData->pData);

						CMeshManager::HANDLE SceneData = ALCHEMY_MESH_MANAGER.AddSceneData( Parser.GetSceneData() );
						m_pOutputSceneNode = m_Parent.m_pSceneManager->CreateScene(SceneData, CSkinMeshController::SKIN_NONE, &SceneObjects);
						ALCHEMY_MESH_MANAGER.DestroySceneData(SceneData);

						if(m_pOutputSceneNode)
						{
							CSceneObject* pSceneObject;
							CVector<CSceneObject*>::CIterator Iterator = SceneObjects.GetIterator(0);
							while( CVector<CSceneObject*>::IsValuable(Iterator) )
							{
								pSceneObject = *Iterator;
								if(pSceneObject)
								{
									pSceneObject->SetEffect(m_Parent.m_Effect);
									pSceneObject->SetRenderMethod(m_Parent.m_pRenderMethod);
								}

								++ Iterator;
							}
						}
					}
				}
				else if(m_pSourceSceneNode)
				{
					CTower::CFloorNodeFactory FloorNodeFactory;
					m_pOutputSceneNode = &m_pSourceSceneNode->Clone(FloorNodeFactory);

					FloorNodeFactory.PopSceneObjects(SceneObjects);
				}

				if(m_pOutputSceneNode)
				{
					m_pOutputSceneNode->SetTranslation(0.0f, m_Parent.m_fHeight, 0.0f);

					m_Parent.m_fHeight += m_Parent.m_fFloorHeight;

					m_Parent.m_SceneNode.AddChild(*m_pOutputSceneNode);
				}

				if( uDataCount && m_pTexture && !(*m_pTexture) )
				{
					const CLoader::DATA* pData = ALCHEMY_LOADER.GetData(LoadAllEvent.GetDataIndices()[uDataCount - 1]);
					if(pData)
					{
						LPTEXTURE pTexture = m_Parent.m_pSceneManager->GetRenderSystem().CreateTexture(pData->uAlpha, pData->uBeta, ALCHEMY_FLAG(TEXTURE::WRITE_ONLY), CTexture::DEFAULT, 0, pData->pData);
						if(pTexture)
							*m_pTexture = pTexture->GetResourceHandle();
					}
				}

				if(m_pTexture && *m_pTexture)
				{
					CSceneObject* pSceneObject;
					CVector<CSceneObject*>::CIterator Iterator = SceneObjects.GetIterator(0);
					while( CVector<CSceneObject*>::IsValuable(Iterator) )
					{
						pSceneObject = *Iterator;
						if(pSceneObject)
						{
							CMaterial* pMaterial = pSceneObject->GetMaterial();
							pMaterial = pMaterial ? pMaterial->Clone() : m_Parent.m_pSceneManager->CreateMaterial();
							if(pMaterial)
							{
								if( pMaterial->GetTextures().GetLength() )
									pMaterial->SetTexture(*m_pTexture, 0);
								else
									pMaterial->AddTexture(*m_pTexture);

								pSceneObject->SetMaterial( pMaterial->GetResourceHandle() );
							}
						}

						++ Iterator;
					}
				}

				UINT uType;
				if( ALCHEMY_GET_EVENT_TYPE(uType, FLOOR_LOADED_COMPELTE, CTower) )
				{
					CFloor* pFloor;
					ALCHEMY_DEBUG_NEW( pFloor, CFloor(m_Parent, m_uType, m_uIndex) );
					UINT uFloor = m_Parent.m_Floors.Add(pFloor);
					CTower::CFloorEvent FloorEvent(uFloor, uType);
					m_Parent.DispatchEvent(FloorEvent);
				}
			}

			ALCHEMY_LOADER.RemoveEventListener(Event.GetType(), *this);
		}

	private:
		CTower& m_Parent;
		UINT m_uType;
		UINT m_uIndex;
		CSceneNode*& m_pOutputSceneNode;
		CSceneNode* m_pSourceSceneNode;
		CGraphicsResource::HANDLE* m_pTexture;
	};
}

CTower::CFloorEvent::CFloorEvent(UINT uFloorIndex, UINT uType, bool bIsBubbles) : 
m_uFloorIndex(uFloorIndex), 
CEvent(uType, bIsBubbles)
{

}

CTower::CFloorEvent::~CFloorEvent()
{

}

CTower::CFloorNodeFactory::CFloorNodeFactory()
{

}

CTower::CFloorNodeFactory::~CFloorNodeFactory()
{
}

void CTower::CFloorNodeFactory::PopSceneObjects(CVector<CSceneObject*>& SceneObjects)
{
	static_cast<CPool<CSceneObject*>&>(SceneObjects) = m_SceneObjects;
	m_SceneObjects.Reset();
}

CSceneObject& CTower::CFloorNodeFactory::CreateSceneObject()
{
	CSceneObject& SceneObject = CSceneNodeFactory::CreateSceneObject();
	m_SceneObjects.Add(&SceneObject);

	return SceneObject;
}

CTower::CTower(void) : 
m_fHeight(0), 
m_fFloorHeight(0), 
m_pFirstFloor(ALCHEMY_NULL), 
m_pFoundation(ALCHEMY_NULL), 
m_pSceneManager(ALCHEMY_NULL), 
m_Effect(0), 
m_pRenderMethod(ALCHEMY_NULL), 
m_pFloor(ALCHEMY_NULL), 
m_uFloorCount(0)
{
}

CTower::~CTower(void)
{
	if(m_pFloor && m_uFloorCount)
	{
		UINT i, j;
		LPFLOOR pFloor;
		for(i = 0; i < m_uFloorCount; ++ i)
		{
			pFloor = &m_pFloor[i];

			ALCHEMY_DEBUG_DELETE_ARRAY(pFloor->Path.pcTypePath);

			for(j = 0; j < pFloor->Path.uIndexCount; ++ j)
				ALCHEMY_DEBUG_DELETE_ARRAY(pFloor->Path.ppIndexPath[j]);

			ALCHEMY_DEBUG_DELETE_ARRAY(pFloor->Path.ppIndexPath);
			ALCHEMY_DEBUG_DELETE_ARRAY(pFloor->ppSceneNodes);

			ALCHEMY_DEBUG_DELETE_ARRAY(pFloor->pTextures);
		}

		ALCHEMY_DEBUG_DELETE_ARRAY(m_pFloor);
	}

	CFloor* pFloor;
	CVector<CFloor*>::CIterator Iterator = m_Floors.GetIterator(0);
	while( CVector<CFloor*>::IsValuable(Iterator) )
	{
		pFloor = *Iterator;
		ALCHEMY_DEBUG_DELETE(pFloor);

		++ Iterator;
	}
}

bool CTower::Load(FLOAT fFirstHeight, FLOAT fFloorHeight, const CHAR* pcFirstFloorPath, const CHAR* pcFoundationPath, const FLOORPATH* pFloorPaths, UINT uFloorCount)
{
	Destroy();

	m_pSceneManager = CSceneManager::GetInstance();

	if(m_pSceneManager)
	{
		CRenderSystem& RenderSystem = m_pSceneManager->GetRenderSystem();
		CEffectCode EffectCode( m_pSceneManager->GetRenderSystem().GetCompiler() );
		CEffectCode::CPassDefiner PassDefiner(EffectCode);
		CPositionEffect PositionEffect;
		CTextureEffect TextureEffect;
		PassDefiner.AddInfo(PositionEffect, ALCHEMY_NULL);
		PassDefiner.AddInfo(TextureEffect, ALCHEMY_NULL);
		PassDefiner.EndDefine();

		CEffect* pEffect = RenderSystem.CreateEffect(EffectCode);

		if(pEffect)
		{
			m_Effect = pEffect->GetResourceHandle();

			IRenderMethod* pRenderMethod = m_pSceneManager->CreateRenderMethod(EffectCode);
			if(pRenderMethod)
			{
				m_pRenderMethod = pRenderMethod;
				
				m_pSceneManager->AddChild(m_SceneNode);

				ALCHEMY_DEBUG_NEW(m_pFloor, FLOOR[uFloorCount]);
				LPFLOOR pFloor;
				const FLOORPATH* pFloorPath;
				PCHAR pSourceFloorIndexPath, pDestinationFloorIndexPath;;
				UINT uLength, i, j;
				for(i = 0; i < uFloorCount; ++ i)
				{
					pFloor = &m_pFloor[i];
					pFloorPath = &pFloorPaths[i];

					uLength = ALCHEMY_STRING_LENGTH(pFloorPath->pcTypePath) + 1;
					ALCHEMY_DEBUG_NEW(pFloor->Path.pcTypePath, CHAR[uLength]);
					ALCHEMY_MEMORY_COPY(pFloor->Path.pcTypePath, pFloorPath->pcTypePath, sizeof(CHAR) * uLength);
					ALCHEMY_DEBUG_NEW(pFloor->Path.ppIndexPath, PCHAR[pFloorPath->uIndexCount]);

					for(j = 0; j < pFloorPath->uIndexCount; ++ j)
					{
						pDestinationFloorIndexPath = pFloorPath->ppIndexPath[j];
						uLength = ALCHEMY_STRING_LENGTH(pDestinationFloorIndexPath) + 1;
						ALCHEMY_DEBUG_NEW(pSourceFloorIndexPath, CHAR[uLength]);
						ALCHEMY_MEMORY_COPY(pSourceFloorIndexPath, pDestinationFloorIndexPath, sizeof(CHAR) * uLength);

						pFloor->Path.ppIndexPath[j] = pSourceFloorIndexPath;
					}

					pFloor->Path.uIndexCount = pFloorPath->uIndexCount;
					pFloor->ppSceneNodes = ALCHEMY_NULL;
					pFloor->pTextures = ALCHEMY_NULL;
				}

				m_uFloorCount = uFloorCount;

				m_fHeight = fFirstHeight;
				m_fFloorHeight = fFloorHeight;

				if(pcFirstFloorPath || pcFoundationPath)
				{
					const CHAR* pcPaths[2];
					UINT uPathCount = 0;

					if(pcFirstFloorPath)
						pcPaths[uPathCount ++] = pcFirstFloorPath;

					if(pcFoundationPath)
						pcPaths[uPathCount ++] = pcFoundationPath;

					UINT uType;
					if( ALCHEMY_GET_EVENT_TYPE(uType, TOWER_LOADED_COMPELTE, CTower) )
					{
						CTowerLoadAllListener* pTowerLoadListener;
						ALCHEMY_DEBUG_NEW( pTowerLoadListener, CTowerLoadAllListener(*this) );
						
						ALCHEMY_LOADER.AddEventListener(uType, *pTowerLoadListener);

						pTowerLoadListener->Release();

						ALCHEMY_LOADER.LoadAll(pcPaths, uPathCount);
					}
				}
				else
				{
					UINT uType;
					if( ALCHEMY_GET_EVENT_TYPE(uType, TOWER_LOADED_COMPELTE, CTower) )
					{
						IEventDispatcher::CEvent Event(uType);
						DispatchEvent(Event);
					}
				}

				return true;
			}
		}
	}

	return false;
}

bool CTower::LoadFloor(UINT uType)
{
	if(uType < m_uFloorCount)
	{
		const CHAR* pcPaths[2];

		UINT i, uPathCount = 0;
		LPFLOOR pFloor = &m_pFloor[uType];
		if( !pFloor->ppSceneNodes || !(*pFloor->ppSceneNodes) )
		{
			if(!pFloor->ppSceneNodes)
			{
				ALCHEMY_DEBUG_NEW(pFloor->ppSceneNodes, CSceneNode*[pFloor->Path.uIndexCount]);
				for(i = 0; i < pFloor->Path.uIndexCount; ++ i)
					pFloor->ppSceneNodes[i] = ALCHEMY_NULL;
			}

			pcPaths[uPathCount ++] = pFloor->Path.pcTypePath;
		}

		if(!pFloor->pTextures)
		{
			ALCHEMY_DEBUG_NEW(pFloor->pTextures, CGraphicsResource::HANDLE[pFloor->Path.uIndexCount]);
			for(UINT i = 0; i < pFloor->Path.uIndexCount; ++ i)
				pFloor->pTextures[i] = 0;
		}

		UINT uIndex = static_cast<UINT>(ALCHEMY_RAND * pFloor->Path.uIndexCount);
		for(i = uIndex; i < pFloor->Path.uIndexCount; ++ i)
		{
			if(!pFloor->pTextures[i])
				break;
		}

		if(i == pFloor->Path.uIndexCount)
		{
			for(i = 0; i < uIndex; ++ i)
			{
				if(!pFloor->pTextures[i])
					break;
			}

			if(i == uIndex)
				i = pFloor->Path.uIndexCount;
		}

		if(i != pFloor->Path.uIndexCount)
		{
			pcPaths[uPathCount ++] = pFloor->Path.ppIndexPath[i];

			UINT uType;
			if( ALCHEMY_GET_EVENT_TYPE(uType, LOAD_ALL_COMPLETE, CLoader) )
			{
				CFloorLoadAllListener* pFloorLoadAllListener;
				ALCHEMY_DEBUG_NEW( pFloorLoadAllListener, CFloorLoadAllListener(*this, uType, i, pFloor->ppSceneNodes[i], pFloor->ppSceneNodes[0], &pFloor->pTextures[i]) )
				ALCHEMY_LOADER.AddEventListener(uType, *pFloorLoadAllListener);

				pFloorLoadAllListener->Release();

				ALCHEMY_LOADER.LoadAll(pcPaths, uPathCount);

				return true;
			}
		}
	}

	return false;
}

void CTower::Destroy()
{
	if(m_pFloor && m_uFloorCount)
	{
		UINT i, j;
		LPFLOOR pFloor;
		LPTEXTURE pTexture;
		for(i = 0; i < m_uFloorCount; ++ i)
		{
			pFloor = &m_pFloor[i];

			ALCHEMY_DEBUG_DELETE_ARRAY(pFloor->Path.pcTypePath);

			for(j = 0; j < pFloor->Path.uIndexCount; ++ j)
				ALCHEMY_DEBUG_DELETE_ARRAY(pFloor->Path.ppIndexPath[j]);

			ALCHEMY_DEBUG_DELETE_ARRAY(pFloor->Path.ppIndexPath);
			ALCHEMY_DEBUG_DELETE_ARRAY(pFloor->ppSceneNodes);

			if(pFloor->pTextures)
			{
				if(m_pSceneManager)
				{
					for(j = 0; j < pFloor->Path.uIndexCount; ++ j)
					{
						pTexture = static_cast<LPTEXTURE>( m_pSceneManager->GetResource(pFloor->pTextures[j], CGraphicsResource::TEXTURE) );
						if(pTexture)
							pTexture->Release();
					}
				}

				ALCHEMY_DEBUG_DELETE_ARRAY(pFloor->pTextures);
			}
		}

		ALCHEMY_DEBUG_DELETE_ARRAY(m_pFloor);

		m_uFloorCount = 0;
	}

	CFloor* pFloor;
	CVector<CFloor*>::CIterator Iterator = m_Floors.GetIterator(0);
	while( CVector<CFloor*>::IsValuable(Iterator) )
	{
		pFloor = *Iterator;
		ALCHEMY_DEBUG_DELETE(pFloor);

		++ Iterator;
	}
}