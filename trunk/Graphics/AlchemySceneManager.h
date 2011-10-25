#pragma once
#include "AlchemyRenderSystem.h"
#include "AlchemyCamera.h"
#include "AlchemyRenderQueue.h"
#include "AlchemySceneNodeFactoryInterface.h"
#include "AlchemyOldLightEffect.h"
#include "AlchemyViewportTextureInfo.h"
#include "AlchemyFullScreenDrawer.h"

#include "../Core/AlchemyList.h"

namespace alchemy
{
	class CSceneManager :
		public CGraphicsResourcePool, public CSceneNodeInterface
	{
		typedef struct
		{
			CRenderQueue::RENDERENTRY RenderEntry;
			CCallBackToRenderInterface* pRenderer;
			UINT uFlag;
		}RENDERENTRY, * LPRENDERENTRY;

		typedef CList<RENDERENTRY>::LISTNODE RENDERLISTNODE, * LPRENDERLISTNODE;

		typedef struct
		{
			bool operator()(const RENDERENTRY& Source, const RENDERENTRY& Target)const;
		}RENDERENTRYLESS;
	public:
		typedef enum
		{
			EFFECT_BEFOREHAND,
			EFFECT_NORMAL,
			EFFECT_FINAL,
			EFFECT_2D,

			EFFECT_TYPE_COUNT
		}RENDERENTRYTYPE;

		CSceneManager(CRenderSystem& RenderSystem, CSceneNodeFactoryInterface& SceneNodeFactory, UINT uMaxRenderEntries);
		~CSceneManager(void);

		static CSceneManager* GetInstance(); 

		void SetSceneNodeFactory(CSceneNodeFactoryInterface& SceneNodeFactory);
		CSceneNodeFactoryInterface& GetSceneNodeFactory();

		void SetRenderSystem(CRenderSystem& RenderSystem);
		CRenderSystem& GetRenderSystem();

		void SetAmbientLight(ARGBCOLOR Color);
		const CARGBColor& GetAmbientLight()const;

		void SetCamera(CCamera& Camera);
		const CCamera& GetCamera()const;

		bool SetSceneRenderTarget(CGraphicsResource::HANDLE RenderTarget, UINT uSceneRenderTargetSurface = 0);
		bool SetSceneRenderTarget();
		CGraphicsResource::HANDLE GetSceneRenderTarget()const;

		CRenderQueue::RENDERENTRY& LockRenderEntry(CCallBackToRenderInterface& Renderer);
		bool UnlockRenderEntry(RENDERENTRYTYPE RenderEntryType);

		CCamera& GetSceneCamera();

		FLOAT GetElapsedTime()const;
		FLOAT GetTime()const;

		UINT GetLightCount()const;

		CFullScreenDrawer& GetFullScreenDrawer();

		CLight* GetLight(UINT uIndex);
		CLight* SetLight(CGraphicsResource::HANDLE Light, COldLightEffect::SHADEMODE LightShadeMode, UINT uIndex);

		CVertex* CreateVertex(UINT uNumVertices, CGraphicsResource::HANDLE VertexDeclaration, const UINT* puUsage = ALCHEMY_NULL);
		CVertex* CreateVertex(UINT uNumVertices, const CVertexDeclaration::VERTEXELEMENT* pVertexElement = ALCHEMY_NULL, UINT uElementCount = 0, const UINT* puUsage = ALCHEMY_NULL);
		CMesh* CreateMesh(UINT uNumVertices, UINT uNumIndices, UINT uIndexUsage, CIndexBuffer::FORMAT IndexFormat, CGraphicsResource::HANDLE VertexDeclaration, PUINT puVertexUsage);
		CMesh* CreateMesh(UINT uNumVertices, UINT uNumIndices, UINT uIndexUsage, CIndexBuffer::FORMAT IndexFormat, const CVertexDeclaration::VERTEXELEMENT* pVertexElements, UINT uElementCount, PUINT puVertexUsage);
		CMesh* CreateMesh(const CMeshManager::MESHDATA MeshData);

		CMaterial* CreateMaterial(const CMeshManager::MATERIALDATA* pMaterialData = ALCHEMY_NULL);

		CLight* CreateLight();

		CRenderMethod* CreateRenderMethod(const CRenderMethod::PARAMETERLIST* pParameterList, UINT uPassCount = 0, const CHAR* pCode = ALCHEMY_NULL, CBuffer** ppCompilationErrors = ALCHEMY_NULL, CEffectCode::CEffectParameterHandlerInterface* pEffectParameterHandler = ALCHEMY_NULL);
		CRenderMethod* CreateRenderMethod(const CRenderMethod::PARAMETERLIST* pParameterList, CGraphicsResource::HANDLE Effect, CEffectCode::CEffectParameterHandlerInterface* pEffectParameterHandler = ALCHEMY_NULL);
		CRenderMethod* CreateRenderMethod(CEffectCode& EffectCode, CGraphicsResource::HANDLE Effect = 0, CBuffer** ppCompilationErrors = ALCHEMY_NULL);

		CSceneObject* CreateSceneObject(CGraphicsResource::HANDLE Mesh, UINT16 uSubset, CGraphicsResource::HANDLE Material, CGraphicsResource::HANDLE RenderMethod = 0, CSceneObject* pInput = ALCHEMY_NULL);
		CSceneNode  * CreateScene(const CMeshManager::SCENEDATA& SceneData, CGraphicsResource::HANDLE uMesh, CGraphicsResource::HANDLE RenderMethod);

		void BuildSceneEffectPass(CEffectCode::CPassDefiner& PassDefiner, UINT uLightFlag = 0xffffffff);

		CLight* AddLight(CGraphicsResource::HANDLE Light, COldLightEffect::SHADEMODE LightShadeMode);

		bool AddChild(CSceneNode& Child);
		bool RemoveChild(CSceneNode& Child);

		void Activate();

		void Destroy();

		void ResetScene();

		void RenderNormalScene(UINT uClearFlag);

		virtual void Update(FLOAT fElapseTime);

		virtual void Render();

	protected:
		//CSceneNode& _CreateSceneNode(const CMeshManager::SCENEDATA& SceneData, CMesh& Mesh, CRenderMethod* pRenderMethod);

		void __RenderScene(RENDERENTRYTYPE type);

		CGraphicsResource* _Create(CGraphicsResource::TYPE Type);

		CARGBColor m_AmbientLight;

		ARGBCOLOR m_BackgroundColor;
	private:
		static CVector<RENDERLISTNODE> sm_RenderEntryPool;
		static UINT sm_uRenderEntryPoolSize;

		static CSceneManager* sm_pInstance;

		CRenderSystem& m_RenderSystem;

		CSceneNodeFactoryInterface& m_SceneNodeFactory;

		CCamera& m_SceneCamera;
		CCamera* m_pCamera;

		CRenderQueue* m_pRenderQueue;
		CSceneNode& m_Scene;

		CVector<CLight*> m_Lights;
		COldLightEffect m_LightEffect;

		LPRENDERLISTNODE m_pRenderEntryList[EFFECT_TYPE_COUNT];
		LPRENDERLISTNODE m_pCurrentRenderEntry;

		LPRENDERLISTNODE m_pDynamicRenderEntryList[EFFECT_FINAL];

		LPRENDERENTRY m_pFinalRenderEntry[EFFECT_TYPE_COUNT];

		FLOAT m_fElapsedTime;
		FLOAT m_fTime;

		CGraphicsResource::HANDLE m_SceneRenderTarget;

		CViewportTextureInfo m_SceneRenderTargetInfo;

		LPTEXTURE m_pSceneRenderTarget;
		UINT m_uSceneRenderTargetSurface;

		bool m_bIsSceneDirty;

		CFullScreenDrawer m_FullScreenDrawer;
	};

	ALCHEMY_INLINE bool CSceneManager::RENDERENTRYLESS::operator()(
		const CSceneManager::RENDERENTRY& Source, 
		const CSceneManager::RENDERENTRY& Target)const
	{
		return Source.RenderEntry.uSortValueA == Target.RenderEntry.uSortValueA ? (Source.RenderEntry.uSortValueB == Target.RenderEntry.uSortValueB ? 
			(Source.RenderEntry.uSortValueC == Target.RenderEntry.uSortValueC ? false : Source.RenderEntry.uSortValueC < Target.RenderEntry.uSortValueC) :
			Source.RenderEntry.uSortValueB < Target.RenderEntry.uSortValueB) : Source.RenderEntry.uSortValueA < Target.RenderEntry.uSortValueA;
	};

	ALCHEMY_INLINE CSceneManager* CSceneManager::GetInstance()
	{
		return sm_pInstance;
	}

	ALCHEMY_INLINE void CSceneManager::SetRenderSystem(CRenderSystem& RenderSystem)
	{
		m_RenderSystem = RenderSystem;
	}

	ALCHEMY_INLINE void CSceneManager::SetSceneNodeFactory(CSceneNodeFactoryInterface& SceneNodeFactory)
	{
		m_SceneNodeFactory = SceneNodeFactory;
	}

	ALCHEMY_INLINE void CSceneManager::SetAmbientLight(ARGBCOLOR Color)
	{
		m_AmbientLight = Color;
	}

	ALCHEMY_INLINE void CSceneManager::SetCamera(CCamera& Camera)
	{
		m_pCamera = &Camera;
	}

	ALCHEMY_INLINE CSceneNodeFactoryInterface& CSceneManager::GetSceneNodeFactory()
	{
		return m_SceneNodeFactory;
	}

	ALCHEMY_INLINE CRenderSystem& CSceneManager::GetRenderSystem()
	{
		return m_RenderSystem;
	}

	ALCHEMY_INLINE const CARGBColor& CSceneManager::GetAmbientLight()const
	{
		return m_AmbientLight;
	}

	ALCHEMY_INLINE const CCamera& CSceneManager::GetCamera()const
	{
		return (m_pSceneRenderTarget ? m_SceneRenderTarget == m_pSceneRenderTarget->GetResourceHandle() : m_SceneRenderTarget == 0) ? m_SceneCamera : *m_pCamera;
	}

	ALCHEMY_INLINE CCamera& CSceneManager::GetSceneCamera()
	{
		return m_SceneCamera;
	}

	ALCHEMY_INLINE CGraphicsResource::HANDLE CSceneManager::GetSceneRenderTarget()const
	{
		return m_SceneRenderTarget;
	}

	/*ALCHEMY_INLINE CRenderQueue& CSceneManager::GetRenderQueue()
	{
		return *m_pRenderQueue;
	}*/

	ALCHEMY_INLINE FLOAT CSceneManager::GetElapsedTime()const
	{
		return m_fElapsedTime;
	}

	ALCHEMY_INLINE FLOAT CSceneManager::GetTime()const
	{
		return m_fTime;
	}

	ALCHEMY_INLINE UINT CSceneManager::GetLightCount()const
	{
		return m_Lights.GetLength();
	}
}
