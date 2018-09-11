#pragma once
#include "AlchemyRenderSystem.h"
#include "AlchemyCamera.h"
#include "AlchemyRenderQueue.h"
#include "AlchemyMaterial.h"
#include "AlchemySceneNodeFactoryInterface.h"
#include "AlchemyLightManager.h"
#include "AlchemyDeferredRenderingPipeline.h"

#include "AlchemyMeshManager.h"

#include "AlchemySkinMeshController.h"

#include "../Core/AlchemyMap.h"

namespace alchemy
{
	struct IRenderGroup;
	class CFullScreenDrawer;
	class CSceneManager :
		public CGraphicsResourcePool
	{
		class CSceneEventListener : 
			public CSmartClass<IEventDispatcher::IListener>
		{
		public:
			CSceneEventListener(CSceneManager& Parent);
			~CSceneEventListener(void);

			void Handler(const IEventDispatcher::CEvent& Event);

		private:
			CSceneManager& m_SceneManager;
		};

		class CViewportEventListener : 
			public CSmartClass<IEventDispatcher::IListener>
		{
		public:
			CViewportEventListener(CSceneManager& Parent);
			~CViewportEventListener(void);

			void Handler(const IEventDispatcher::CEvent& Event);

		private:
			CSceneManager& m_SceneManager;
		};
	public:
		ALCHEMY_BEGIN_EVENT_ENUM
			RESET_SCENE = 0
		ALCHEMY_END_EVENT_ENUM

		struct IFullScreenDrawer :
			public CRenderQueue::ICallBackToRender
		{
			virtual UINT AddUserData(CGraphicsResource::HANDLE RenderTexture, IRenderMethod* pRenderMethod, CGraphicsResource::HANDLE Material, UINT uRenderTextureIndex, const IEffect::SAMPLERDECLARATION* pSamplerDeclaration) = 0;
		};

		/*struct ISceneDrawer :
			public CRenderQueue::ICallBackToRender
		{
			virtual UINT AddRenderData(const CMatrix4x4& WorldMatrix, IRenderMethod& RenderMethod, CGraphicsResource::HANDLE Material = 0, IMeshProcessor* pMeshProcessor = ALCHEMY_NULL) = 0;
			virtual UINT AddRenderData(const CMatrix4x4& WorldMatrix, IRenderMethod& RenderMethod, ARGBCOLOR ClearColor, UINT uClearFlag, CGraphicsResource::HANDLE Material = 0, IMeshProcessor* pMeshProcessor = ALCHEMY_NULL, UINT uCameraIndex = 0) = 0;
			virtual void RemoveRenderData(UINT uHandle) = 0;
		};*/

		typedef struct 
		{
			CMeshManager::HANDLE		DataHandle;
			CGraphicsResource::HANDLE	ResourceHandle;
		}RESOURCEEINFO,*LPRESOURCEINFO;

		CSceneManager(CRenderSystem& RenderSystem, ISceneNodeFactory* pSceneNodeFactory = ALCHEMY_NULL);
		~CSceneManager(void);

		static CSceneManager* GetInstance(); 

		CRenderSystem& GetRenderSystem();

		ISceneNodeFactory& GetSceneNodeFactory();

		CRenderQueue& GetRenderQueue();

		void SetAmbientLight(ARGBCOLOR Color);
		const CARGBColor& GetAmbientLight()const;

		void SetBackgroundColor(ARGBCOLOR ARGBColor);
		ARGBCOLOR GetBackgroundColor()const;

		CCamera& GetSceneCamera();

		CDeferredRenderingPipeline& GetDeferredRenderingPipeline();

		IFullScreenDrawer* GetFullScreenDrawer();

		//ISceneDrawer* GetSceneDrawer();

		FLOAT GetElapsedTime()const;
		FLOAT GetTime()const;

		//UINT GetCameraCount()const;

		//UINT GetLightCount()const;

		const CCamera* GetCamera(UINT uIndex)const;

		//const CLight* GetLight(UINT uIndex);

		bool SetCamera(const CCamera* pCamera, UINT uIndex);

		//CLight* SetLight(CGraphicsResource::HANDLE Light, CLightManager::SHADEMODE LightShadeMode, UINT uIndex);

		CVertex* CreateVertex(UINT uNumVertices, CGraphicsResource::HANDLE VertexDeclaration, const UINT* puUsage = ALCHEMY_NULL);
		CVertex* CreateVertex(UINT uNumVertices, const CVertexDeclaration::VERTEXELEMENT* pVertexElement = ALCHEMY_NULL, UINT uElementCount = 0, const UINT* puUsage = ALCHEMY_NULL);
		CMesh* CreateMesh(UINT uNumVertices, UINT uNumIndices, UINT uIndexUsage, CIndexBuffer::FORMAT IndexFormat, CGraphicsResource::HANDLE VertexDeclaration, PUINT puVertexUsage);
		CMesh* CreateMesh(UINT uNumVertices, UINT uNumIndices, UINT uIndexUsage, CIndexBuffer::FORMAT IndexFormat, const CVertexDeclaration::VERTEXELEMENT* pVertexElements, UINT uElementCount, PUINT puVertexUsage);
		//CMesh* CreateMesh(const CMeshManager::MESHDATA& MeshData);
		CMesh* CreateMesh(CMeshManager::HANDLE uMeshDataHandle);

		CMaterial* CreateMaterial( CMeshManager::HANDLE uMaterialDataHandle, CMap<CMeshManager::HANDLE, CGraphicsResource::HANDLE>* pTextureCache = ALCHEMY_NULL, CVector<RESOURCEEINFO>* pTextureInfo = ALCHEMY_NULL );
		CMaterial* CreateMaterial(const CMeshManager::MATERIALDATA* pMaterialData = ALCHEMY_NULL, const CMeshManager::TEXTUREDATA* pTextureData = ALCHEMY_NULL, UINT uCount = 0);

		CLight* CreateLight(); 

		CRenderMethod* CreateRenderMethod(const CRenderMethod::PARAMETERLIST* pParameterList, UINT uPassCount, const UINT* puRenderTargetIndexCount, CRenderMethod::IShaderParametersHandler* pShaderParametersHandler = ALCHEMY_NULL, CRenderMethod::IRenderStateHandler* pRenderStateHandler = ALCHEMY_NULL);
		CRenderMethod* CreateRenderMethod(CEffectCode& EffectCode, CEffectCode::CGlobalInputHandler* pShaderParametersHandler = ALCHEMY_NULL, CRenderMethod::IRenderStateHandler* pRenderStateHandler = ALCHEMY_NULL);

		//CSceneObject* CreateSceneObject(CGraphicsResource::HANDLE Mesh, UINT16 uSubset, CGraphicsResource::HANDLE Material, CGraphicsResource::HANDLE RenderMethod = 0, CSceneObject* pInput = ALCHEMY_NULL);
		//CSceneNode  * CreateScene(const CMeshManager::SCENEDATA& SceneData, CGraphicsResource::HANDLE uMesh, CGraphicsResource::HANDLE RenderMethod);

		CSceneNode* CreateScene(CMeshManager::HANDLE hSceneDataHandle, CSkinMeshController::SKINTYPE SkinType = CSkinMeshController::SKIN_SOFTWARE, CVector<CSceneObject*>* pSceneObjects = ALCHEMY_NULL, 
			CVector<RESOURCEEINFO>* pMaterialInfo = ALCHEMY_NULL, CVector<RESOURCEEINFO>* pTextureInfo = ALCHEMY_NULL, CVector<RESOURCEEINFO>* pMeshInfo = ALCHEMY_NULL, CVector<CBone*>* pRootBone = ALCHEMY_NULL);

		//void AddLightEffectInfoToPass(CEffectCode::CPassDefiner& PassDefiner, IEffectInfoBlendMethod* pEffectInfoBlendMethod, IEffectInfo* pSpecularEffect = ALCHEMY_NULL, IEffectInfo* pNormalMapEffect = ALCHEMY_NULL, UINT uOutputKey = 0, UINT uInputKey = 0);

		//CLight* AddLight(CGraphicsResource::HANDLE Light, CLightManager::SHADEMODE LightShadeMode);

		//UINT AddCamera(const CCamera& Camera);

		bool AddChild(CSceneNode& Child);
		bool RemoveChild(CSceneNode& Child);

		void AddRenderGroup( IRenderGroup& Render );
		void RemoveRenderGroup( IRenderGroup& Render );

		void Activate();

		void ResetScene();

		void RenderScene(UINT uClearFlag);
		
		void Destroy();

		virtual void Update(FLOAT fElapseTime);

		virtual void Render();
		
		ALCHEMY_IMPLEMENT_INTERFACE

	protected:
		//CSceneNode& _CreateSceneNode(const CMeshManager::SCENEDATA& SceneData, CMesh& Mesh, CRenderMethod* pRenderMethod);
		CGraphicsResource* _Create(CGraphicsResource::TYPE Type);

		CARGBColor m_AmbientLight;

		ARGBCOLOR m_BackgroundColor;

		bool m_bEnbleDeferredRendering;
	private:
		void __ResetScene();

		static CSceneManager* sm_pInstance;

		CRenderSystem& m_RenderSystem;

		ISceneNodeFactory& m_SceneNodeFactory;

		CCamera& m_SceneCamera;
		//CCamera* m_pCamera;

		CRenderQueue m_RenderQueue;
		CSceneNode& m_Scene;

		CLightManager m_LightManager;

		CPool<const CCamera*> m_Cameras;

		CVector<IRenderGroup*>	m_RenderGroups;

		CDeferredRenderingPipeline m_DeferredRenderingPipeline;

		CFullScreenDrawer* m_pFullScreenDrawer;
		//ISceneDrawer*		m_pSceneDrawer;

		FLOAT m_fElapsedTime;
		FLOAT m_fTime;

		bool m_bIsSceneDirty;

		CSceneEventListener m_SceneEventListener;
		CViewportEventListener m_ViewportEventListener;
	};

	ALCHEMY_INLINE CSceneManager* CSceneManager::GetInstance()
	{
		return sm_pInstance;
	}

	ALCHEMY_INLINE void CSceneManager::SetAmbientLight(ARGBCOLOR Color)
	{
		m_AmbientLight = Color;
	}

	ALCHEMY_INLINE CRenderSystem& CSceneManager::GetRenderSystem()
	{
		return m_RenderSystem;
	}

	ALCHEMY_INLINE ISceneNodeFactory& CSceneManager::GetSceneNodeFactory()
	{
		return m_SceneNodeFactory;
	}

	ALCHEMY_INLINE CRenderQueue& CSceneManager::GetRenderQueue()
	{
		return m_RenderQueue;
	}
	
	ALCHEMY_INLINE void CSceneManager::SetBackgroundColor(ARGBCOLOR ARGBColor)
	{
		m_BackgroundColor = ARGBColor;
	}

	ALCHEMY_INLINE ARGBCOLOR CSceneManager::GetBackgroundColor()const
	{
		return m_BackgroundColor;
	}

	ALCHEMY_INLINE const CARGBColor& CSceneManager::GetAmbientLight()const
	{
		return m_AmbientLight;
	}

	ALCHEMY_INLINE CCamera& CSceneManager::GetSceneCamera()
	{
		return m_SceneCamera;
	}

	ALCHEMY_INLINE CDeferredRenderingPipeline& CSceneManager::GetDeferredRenderingPipeline()
	{
		return m_DeferredRenderingPipeline;
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

	/*ALCHEMY_INLINE UINT CSceneManager::GetCameraCount()const
	{
		return m_Cameras.GetLength();
	}

	ALCHEMY_INLINE UINT CSceneManager::GetLightCount()const
	{
		return m_LightManager.GetLightCount();
	}*/

	ALCHEMY_INLINE const CCamera* CSceneManager::GetCamera(UINT uIndex)const
	{
		//return ( m_pSceneRenderTarget ? m_SceneRenderTarget == m_pSceneRenderTarget->GetResourceHandle() : m_SceneRenderTarget == 0 ) ? m_SceneCamera : *m_pCamera;
		if( uIndex < m_Cameras.GetLength() )
			return ( !uIndex && m_RenderQueue.IsNormalScene() ) ? &m_SceneCamera : m_Cameras[uIndex];

		return ALCHEMY_NULL;
	}

	/*ALCHEMY_INLINE const CLight* CSceneManager::GetLight(UINT uIndex)
	{
		return static_cast<const CLight*>( m_LightManager.GetLight(uIndex) );
	}*/
}
