#pragma once

#include "AlchemyFunction.h"
#include "AlchemyPool.h"

namespace alchemy
{

#define ALCHEMY_LIST_SORT(TYPE, LESSFUNCTION) CList<TYPE>::MergeSort<LESSFUNCTION>

	template<typename T>
	class CList
	{
	public:
		typedef struct LISTNODE
		{
			T Data;

			struct LISTNODE* pNext;
		}LISTNODE, * LPLISTNODE;

		class CIterator
		{
			friend class CList;
		public:
			CIterator() : m_pNode(ALCHEMY_NULL) {}
			~CIterator() {}

			CIterator& operator++()
			{
				m_pNode = m_pNode->pNext;

				return *this;
			}

			CIterator operator++(int)
			{
				CIterator Result = *this;

				++(*this);

				return Result;
			}

			const T& operator*()const
			{
				ALCHEMY_DEBUG_ASSERT(m_pNode, "CList::CIterator::operator*: No value iterator");

				return m_pNode->Data;
			}

			const T* operator->()const
			{
				return &**this;
			}
		private:
			LPLISTNODE m_pNode;
		};

		CList(void) : m_pHead(ALCHEMY_NULL) {}
		~CList(void) {}

		template<typename LESSFUNCTION>
		static LPLISTNODE MergeSort(LPLISTNODE pHead)
		{
			if(!pHead)
				return ALCHEMY_NULL;

			LESSFUNCTION LessFunction;
			UINT uCurrentBlockSize = 1;
			LPLISTNODE pLastTail;
			while(true)
			{
				pLastTail = Merge(pHead, uCurrentBlockSize, LessFunction);
				if(pLastTail != ALCHEMY_NULL)
				{
					do 
					{
						pLastTail = Merge(pLastTail->pNext, uCurrentBlockSize, LessFunction);
					} 
					while(pLastTail != ALCHEMY_NULL);

					uCurrentBlockSize *= 2;
				}
				else
					break;
			}

			return pHead;
		}

		static bool IsValuable(const CIterator& Iterator)
		{
			return Iterator.m_pNode != ALCHEMY_NULL;
		}

		CIterator GetBeginIterator()
		{
			CIterator Iterator;
			Iterator.m_pNode = m_pHead;

			return Iterator;
		}

		template<typename LESSFUNCTION>
		void Sort()
		{
			m_pHead = MergeSort<LESSFUNCTION>(m_pHead);
		}

		void Unshift(const T& Data)
		{
			LISTNODE Node;
			UINT uIndex = m_ListNodePool.Add(Node);

			LISTNODE& CurrentNode = m_ListNodePool[uIndex];
			CurrentNode.Data  = Data;
			CurrentNode.pNext = m_pHead;
			m_pHead           = &CurrentNode;
		}

		bool Shift(T* pOutputData = ALCHEMY_NULL)
		{
			if(m_pHead)
			{
				if(pOutputData)
					*pOutputData = m_pHead->Data;

				m_pHead = m_pHead->pNext;

				return true;
			}

			return false;
		}

		virtual void Push(const T& Data)
		{
			LISTNODE Node;
			UINT uIndex = m_ListNodePool.Add(Node);

			LISTNODE& CurrentNode = m_ListNodePool[uIndex];
			CurrentNode.Data = Data;
			CurrentNode.pNext = ALCHEMY_NULL;

			if(m_pHead)
			{
				LPLISTNODE pNode = m_pHead;
				while(pNode->pNext)
					pNode = pNode->pNext;

				pNode->pNext = &CurrentNode;
			}
			else
				m_pHead = &CurrentNode;
		}

	private:
		static LPLISTNODE GetTailNode(LPLISTNODE pHead, UINT uLength)
		{
			while(uLength -- && pHead->pNext != ALCHEMY_NULL)
				pHead = pHead->pNext;

			return pHead;
		}

		template<typename LESSFUNCTION>
		static LPLISTNODE Merge(LPLISTNODE& pHead, UINT uCurrentBlockSize, LESSFUNCTION LessFunction)
		{
			bool bIsCanMerge = true;
			LPLISTNODE pLeftBlock = pHead, pRightBlock = pHead;

			for(UINT i = 0; i < uCurrentBlockSize; ++ i)
			{
				pRightBlock = pRightBlock->pNext;
				if(pRightBlock == ALCHEMY_NULL)
				{
					bIsCanMerge = false;

					break;
				}
			}

			if(!bIsCanMerge)
				return ALCHEMY_NULL;

			LPLISTNODE pCurrentSortedNode, pCurrentTailNode = ALCHEMY_NULL;
			UINT uLeftCount = 0, uRightCount = 0;

			if( LessFunction(pLeftBlock->Data, pRightBlock->Data) )
			{
				++ uLeftCount;

				pCurrentSortedNode = pLeftBlock;
				pLeftBlock         = pLeftBlock ->pNext;
			}
			else
			{
				++ uRightCount;

				pCurrentSortedNode = pHead = pRightBlock;
				pRightBlock        = pRightBlock ->pNext;
			}

			LPLISTNODE pTempNode;
			while(true)
			{
				if(uLeftCount == uCurrentBlockSize || pLeftBlock == ALCHEMY_NULL)
				{
					pCurrentSortedNode->pNext = pRightBlock;
					pCurrentTailNode = GetTailNode(pCurrentSortedNode, uCurrentBlockSize - uRightCount);

					break;
				}
				else if(uRightCount == uCurrentBlockSize || pRightBlock == ALCHEMY_NULL)
				{
					pTempNode = pCurrentSortedNode->pNext;

					pCurrentSortedNode->pNext = pLeftBlock;
					pCurrentTailNode          = GetTailNode(pCurrentSortedNode, uCurrentBlockSize - uLeftCount);
					pCurrentTailNode->pNext   = pTempNode;
					break;
				}
				else
				{
					if( LessFunction(pLeftBlock->Data, pRightBlock->Data) )
					{
						++ uLeftCount;

						pCurrentSortedNode->pNext = pLeftBlock;
						pLeftBlock                = pLeftBlock->pNext;
					}
					else
					{
						++ uRightCount;

						pCurrentSortedNode->pNext = pRightBlock;
						pRightBlock               = pRightBlock->pNext;
					}

					pCurrentSortedNode = pCurrentSortedNode->pNext;
				}
			}

			return pCurrentTailNode->pNext != ALCHEMY_NULL ? pCurrentTailNode : ALCHEMY_NULL;
		}

		LPLISTNODE m_pHead;

		CPool<LISTNODE> m_ListNodePool;
	};
}
