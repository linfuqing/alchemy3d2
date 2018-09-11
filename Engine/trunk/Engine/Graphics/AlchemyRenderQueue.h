#pragma once

//#include "AlchemyViewportTextureInfo.h"
#include "AlchemyARGBColor.h"
#include "AlchemyRenderSystem.h"

#include "../Core/AlchemyPool.h"
#include "../Core/AlchemyList.h"
#include "../Core/AlchemyMap.h"

#include "../SceneManagement/AlchemySceneTreeManagerInterface.h"

namespace alchemy
{
	class CRenderQueue
	{
	public:
		typedef enum
		{
			AUTO_RENDER_TARGET = 0,
			AUTO_SHADER,
			AUTO_VERTEX_DECLARATION,
		}RENDERFLAGTYPE;

		typedef enum
		{
			BEFOREHAND = 0,
			NORMAL,
			FINAL,
			FULL_SCREEN
		}RENDERENTRYTYPE;
		
		typedef enum
		{
			KEY_RENDER_TARGET = 0,
			KEY_EFFECT,
			KEY_EFFECT_PASS,
			KEY_VERTEX_DECLARATION,

			ENTRY_RENDER_TARGET,
			ENTRY_EFFECT,
			ENTRY_EFFECT_PASS,
			ENTRY_VERTEX_DECLARATION,

			MODEL,
			MODEL_PARAMETER,
			PARENT,

			TOTAL_ACTIVATE_FLAGS
		}ACTIVATEFLAG;

		typedef struct
		{
#pragma pack(1)
			union
			{
				struct
				{
					UINT32 uSortValueA;
					UINT32 uSortValueB;
					UINT32 uSortValueC;
				};

				struct
				{
					UINT16 uEffect;
					UINT16 uRenderTarget;

					UINT16 uVertexDeclaration;
					UINT16 uRenderPass;

					UINT16 uModelParameter;
					UINT16 uModel;
				};
			};
#pragma pack()

			UINT uUserData;
		}RENDERENTRY, * LPRENDERENTRY;

		typedef struct
		{
			CGraphicsResource::HANDLE VertexDeclaration;
			CGraphicsResource::HANDLE Effect;
			UINT uRenderPass;
			UINT uRenderTargetIndex;
			UINT uFlag;
		}RENDERKEY;

		ALCHEMY_INTERFACE(ICallBackToRender)
		{
			virtual void Render(const RENDERENTRY& Entry, const RENDERKEY& Key, UINT uFlag) = 0;
		};

		CRenderQueue(CRenderSystem& RenderSystem, const IBoundingVolume* pBoundingVolume = ALCHEMY_NULL, const IClipper* pClipper = ALCHEMY_NULL);
		~CRenderQueue(void);

		static UINT BuildRenderFlag(
			const CRenderQueue::RENDERENTRY& CurrentEntry, 
			const CRenderQueue::RENDERKEY& CurrentKey, 
			const CRenderQueue::RENDERENTRY& PreviousEntry, 
			const CRenderQueue::RENDERKEY& PreviousKey, 
			bool bIsParentDirty);

		bool IsNormalScene()const;

		CRenderSystem& GetRenderSystem()const;

		UINT GetCurrentRenderList()const;

		LPTEXTURE GetSceneRenderTarget()const;
		CGraphicsResource::HANDLE GetRenderTarget()const;
		UINT GetRenderTargetSurface()const;

		LPTEXTURE CreateSceneRenderTarget(INT nShift = 0);

		UINT AddRenderList(bool bIsSort, const IBoundingVolume* pBoundingVolume = ALCHEMY_NULL, const IClipper* pClipper = ALCHEMY_NULL);

		UINT AddRenderTargetToQueue(CGraphicsResource::HANDLE RenderTarget,UINT uRenderList, ARGBCOLOR ClearColor = 0);
		bool SetRenderTargetToQueue(CGraphicsResource::HANDLE RenderTarget, UINT uRenderTargetIndex, UINT uRenderList, ARGBCOLOR ClearColor = 0);
		bool ClearRenderTargetQueue(UINT uRenderList);

		bool SetRenderTarget(CGraphicsResource::HANDLE RenderTarget, UINT uRenderTargetSurface = 0, bool bIsSaved = true);
		bool SetRenderTarget();

		RENDERENTRY& LockRenderEntry(ICallBackToRender& Renderer, bool* pIsVisible = ALCHEMY_NULL, IBoundingVolume* pBoundingVolume = ALCHEMY_NULL, UINT uSceneTreeManagerIndex = 0);
		bool UnlockRenderEntry( UINT uRenderList, UINT uFlag = ALCHEMY_FLAG(AUTO_RENDER_TARGET) | ALCHEMY_FLAG(AUTO_SHADER) | ALCHEMY_FLAG(AUTO_VERTEX_DECLARATION) );

		bool ActivateMultipleRenderTarget(UINT uFromIndex = 0, UINT uCount = 0);

		void ResetScene();
		void RenderScene(ARGBCOLOR BackgroudColor, UINT uClearFlag);
		void RenderScene(UINT uRenderList);

		void Render(ARGBCOLOR BackgroudColor);

		void Destroy();

	private:
		typedef struct
		{
			CRenderQueue::RENDERENTRY RenderEntry;
			bool* pIsVisible;
			ICallBackToRender* pRenderer;
			IBoundingVolume* pBoundingVolume;
			UINT uSceneTreeManagerIndex;
			RENDERKEY RenderKey;
			UINT uFlag;
		}RENDERENTRYDATA, * LPRENDERENTRYDATA;

		typedef struct  
		{
			ISceneTreeManager* pSceneTreeManager;

			const IBoundingVolume* pBoundingVolume;
			const IClipper* pClipper;

			CMap<const IBoundingVolume*, UINT> RenderEntryMap;
		}SCENETREEMANAGER, * LPSCENETREEMANAGER;

		typedef struct
		{
			CGraphicsResource::HANDLE RenderTarget;
			ARGBCOLOR ClearColor;
		}RENDERTARGETINFO, * LPRENDERTARGETINFO;

		typedef CList<RENDERENTRYDATA>::LISTNODE RENDERLISTNODE, * LPRENDERLISTNODE;

		typedef struct  
		{
			LPRENDERLISTNODE pHead;
			LPRENDERLISTNODE pTail;
			bool bIsSort;

			const IBoundingVolume* pBoundingVolume;
			const IClipper* pClipper;

			CVector<RENDERTARGETINFO> RenderTargetInfos;
		}RENDERLIST, * LPRENDERLIST;

		typedef struct
		{
			bool operator()(const RENDERTARGETINFO& Source, const RENDERTARGETINFO& Target)const;
		}RENDERTARGETINFOEQUAL;

		typedef struct
		{
			bool operator()(const RENDERKEY& Source, const RENDERKEY& Target)const;
		}RENDERKEYLESS;

		typedef struct
		{
			bool operator()(const RENDERKEY& Source, const RENDERKEY& Target)const;
		}RENDERKEYEQUAL;

		typedef struct
		{
			bool operator()(const RENDERENTRYDATA& Source, const RENDERENTRYDATA& Target)const;
		}RENDERENTRYDATALESS;

		class CRenderEntry
		{
			typedef struct
			{
				UINT uRenderEntryNodeIndex;
				UINT uRenderList;
			}RENDERENTRYNODEINFO;
		public:
			CRenderEntry();
			virtual ~CRenderEntry();

			void PushRenderEntryNodeTo(CVector<RENDERLIST>& RenderLists)const;

			void AddRenderEntryNode(UINT uRenderEntryNodeIndex, UINT uRenderList);
		private:
			CVector<RENDERENTRYNODEINFO> m_RenderEntryNodeInfos;
		};

		class CSceneTreeObject : 
			public CRenderEntry, public CSmartClass<ISceneTreeObject>
		{
		public:
			CSceneTreeObject(const IBoundingVolume& BoundingVolume);
			~CSceneTreeObject();

			const IBoundingVolume& GetBoundingVolume()const;

		private:
			const IBoundingVolume& m_BoundingVolume;
		};

		static CPool<RENDERLISTNODE> sm_RenderEntryPool;

		LPRENDERLISTNODE m_pCurrentRenderEntry;

		UINT m_uCurrentRenderEntryIndex;

		CVector<RENDERLIST> m_RenderLists;

		UINT m_uCurrentRenderList;

		bool m_bIsSort;

		CGraphicsResource::HANDLE m_RenderTarget;
		CGraphicsResource::HANDLE m_CurrentRenderTarget;

		UINT m_uRenderTargetSurface;
		UINT m_uCurrentRenderTargetSurface;

		LPTEXTURE m_pRenderTarget;

		CMap<RENDERKEY, CGraphicsResource::HANDLE, RENDERKEYLESS, RENDERKEYEQUAL> m_VertexDeclarationMap;

		CVector<CRenderEntry*> m_RenderEntris;
		CVector<SCENETREEMANAGER> m_SceneTreeManagers;

		CRenderSystem& m_RenderSystem;
	};

	ALCHEMY_INLINE bool CRenderQueue::IsNormalScene()const
	{
		return m_RenderTarget == (m_pRenderTarget ? m_pRenderTarget->GetResourceHandle() : 0) && m_RenderTarget == m_CurrentRenderTarget;
	}

	ALCHEMY_INLINE CRenderSystem& CRenderQueue::GetRenderSystem()const
	{
		return m_RenderSystem;
	}

	/*ALCHEMY_INLINE CViewportTextureInfo& CRenderQueue::GetRenderTargetInfo()
	{
		return m_RenderTargetInfo;
	}*/

	ALCHEMY_INLINE UINT CRenderQueue::GetCurrentRenderList()const
	{
		return m_uCurrentRenderList;
	}

	ALCHEMY_INLINE LPTEXTURE CRenderQueue::GetSceneRenderTarget()const
	{
		return m_pRenderTarget;
	}

	ALCHEMY_INLINE CGraphicsResource::HANDLE CRenderQueue::GetRenderTarget()const
	{
		return m_RenderTarget;
	}

	ALCHEMY_INLINE UINT CRenderQueue::GetRenderTargetSurface()const
	{
		return m_uRenderTargetSurface;
	}

	ALCHEMY_INLINE bool CRenderQueue::RENDERTARGETINFOEQUAL::operator()(const CRenderQueue::RENDERTARGETINFO& Source, const CRenderQueue::RENDERTARGETINFO& Target)const
	{
		return Source.RenderTarget == Target.RenderTarget;
	}

	ALCHEMY_INLINE bool CRenderQueue::RENDERKEYLESS::operator()(const CRenderQueue::RENDERKEY& Source, const CRenderQueue::RENDERKEY& Target)const
	{
		if(Source.VertexDeclaration < Target.VertexDeclaration)
			return true;
		else if(Source.VertexDeclaration == Target.VertexDeclaration && Source.Effect < Target.Effect)
			return true;
		else if(Source.VertexDeclaration == Target.VertexDeclaration && Source.Effect == Target.Effect && Source.uRenderPass < Target.uRenderPass)
			return true;
		else if(Source.VertexDeclaration == Target.VertexDeclaration && Source.Effect == Target.Effect && Source.uRenderPass == Target.uRenderPass && Source.uRenderTargetIndex < Target.uRenderTargetIndex)
			return true;

		return false;
	}

	ALCHEMY_INLINE bool CRenderQueue::RENDERKEYEQUAL::operator()(const CRenderQueue::RENDERKEY& Source, const CRenderQueue::RENDERKEY& Target)const
	{
		return Source.VertexDeclaration == Target.VertexDeclaration && Source.Effect == Target.Effect && Source.uRenderPass == Target.uRenderPass && Source.uRenderTargetIndex == Target.uRenderTargetIndex;
	}

	ALCHEMY_INLINE bool CRenderQueue::RENDERENTRYDATALESS::operator()(
		const CRenderQueue::RENDERENTRYDATA& Source, 
		const CRenderQueue::RENDERENTRYDATA& Target)const
	{
		return Source.RenderEntry.uSortValueA == Target.RenderEntry.uSortValueA ? (Source.RenderEntry.uSortValueB == Target.RenderEntry.uSortValueB ? 
			(Source.RenderEntry.uSortValueC == Target.RenderEntry.uSortValueC ? false : Source.RenderEntry.uSortValueC < Target.RenderEntry.uSortValueC) :
			Source.RenderEntry.uSortValueB < Target.RenderEntry.uSortValueB) : Source.RenderEntry.uSortValueA < Target.RenderEntry.uSortValueA;
	};
}