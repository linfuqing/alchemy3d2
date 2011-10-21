#pragma once

#include "../Core/AlchemyDataType.h"
#include "../Core/AlchemyList.h"

namespace alchemy
{
	class CRenderSystem;

	class CRenderQueue
	{
	public:
		
		typedef enum
		{
			RENDER_TARGET,
			//效果文件
			EFFECT,
			//效果Pass
			EFFECT_PASS,
			MODEL,
			MODEL_PARAMETER,
			VERTEX_DECLARATION,
			//MATERIAL,
			PARENT,

			TOTAL_ACTIVATE_FLAGS
		}ACTIVATEFLAG;

		///
		// 渲染块结构体.
		///
		typedef struct RENDERENTRY
		{
			//内存集中化
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
					// uSortValueA		
					UINT16 uEffect;	
					UINT16 uRenderTarget;

					// uSortValueB
					UINT16 uVertexDeclaration;
					UINT16 uRenderPass;	

					//UINT8 uModelType;
					//UINT8 uDepth;

					// uSortValueC
					UINT16 uModelParameter;	
					UINT16 uModel;	
				};
			};
#pragma pack()

			UINT uUserData;
		}RENDERENTRY, * LPRENDERENTRY;

		typedef struct
		{
			bool operator()(const LPRENDERENTRY& Source, const LPRENDERENTRY& Target)const;
		}LPRENDERENTRYLESS;

		class CCallBackToRenderInterface
		{
		public:
			CCallBackToRenderInterface() {}
			virtual ~CCallBackToRenderInterface() {}

			virtual void Render(const CRenderQueue::RENDERENTRY& Entry, UINT uFlag) = 0;
		};

		CRenderQueue(UINT uMaxRenderEntries, CRenderSystem& RenderSystem);
		~CRenderQueue(void);

		static UINT BuildRenderFlag(const RENDERENTRY& Current, const RENDERENTRY& Previous, bool bIsParentDirty);

		/*template<typename T>
		static typename CList<T>::LPLISTNODE ForEachRenderEntryList( typename CList<T>::LPLISTNODE pListHead, const RENDERENTRY& Get(T&, CCallBackToRenderInterface**), void CallBack(T&, UINT) )
		{
			ALCHEMY_LIST_SORT(T, LPRENDERENTRYLESS)(pListHead);

			CList<T>::LPLISTNODE pCurrentNode = pListHead;

			CallBack(pCurrentNode->Data, ~0);

			CList<T>::LPLISTNODE pPreviousNode = pCurrentNode;
			pCurrentNode = pCurrentNode->pNext;

			CCallBackToRenderInterface* pCurrentRenderer, * pPreviousRenderer;

			while(pCurrentNode)
			{
				pCurrentRenderer = pPreviousRenderer = ALCHEMY_NULL;

				CallBack( pCurrentNode->Data, BuildRenderFlag( Get(pCurrentNode->Data, &pCurrentRenderer), Get(pPreviousNode->Data, &pPreviousRenderer), pCurrentRenderer != pPreviousRenderer) );

				pPreviousNode = pCurrentNode;
				pCurrentNode  = pCurrentNode->pNext;
			}

			return pListHead;
		}*/

		void Destroy();

		//锁定渲染队列从渲染块池获取一个渲染块.
		RENDERENTRY& LockRenderEntry();

		//把锁定的渲染块加入渲染列表并解锁,
		void UnlockRenderEntry(RENDERENTRY& Entry, CCallBackToRenderInterface& Renderer);

		//清空渲染列表.
		void Reset();

		//渲染渲染列表中的渲染块.
		void Render(const RENDERENTRY* pPreviousEntry = ALCHEMY_NULL, bool bIsParentDirty = false);

	private:
		UINT m_uActiveEntries;
		UINT m_uMaxRenderEntries;

		LPRENDERENTRY  m_pEntryPool;
		LPRENDERENTRY* m_ppEntryList;

		CCallBackToRenderInterface** m_ppRenderers;

		CRenderSystem& m_RenderSystem;
	};

	ALCHEMY_INLINE bool CRenderQueue::LPRENDERENTRYLESS::operator()(
		const CRenderQueue::LPRENDERENTRY& Source, 
		const CRenderQueue::LPRENDERENTRY& Target)const
	{
		return Source->uSortValueA == Target->uSortValueA ? (Source->uSortValueB == Target->uSortValueB ? 
			(Source->uSortValueC == Target->uSortValueC ? false : Source->uSortValueC < Target->uSortValueC) :
			Source->uSortValueB < Target->uSortValueB) : Source->uSortValueA < Target->uSortValueA;
	};

	ALCHEMY_INLINE void CRenderQueue::Reset()
	{
		m_uActiveEntries = 0;
	}
}