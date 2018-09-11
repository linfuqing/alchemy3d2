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

		class CConstantIterator
		{
			friend class CList;
		public:
			CConstantIterator() : m_pParent(ALCHEMY_NULL), m_pNode(ALCHEMY_NULL) {}
			CConstantIterator(const CConstantIterator& Iterator) : m_pParent(Iterator.m_pParent), m_pNode(Iterator.m_pNode) {}
			~CConstantIterator() {}

			ALCHEMY_INLINE bool operator==(const CConstantIterator& Iterator)
			{
				return m_pParent == Iterator.m_pParent && m_pNode == Iterator.m_pNode;
			}

			ALCHEMY_INLINE bool operator!=(const CConstantIterator& Iterator)
			{
				return m_pParent != Iterator.m_pParent || m_pNode != Iterator.m_pNode;
			}

			ALCHEMY_INLINE CConstantIterator& operator++()
			{
				ALCHEMY_DEBUG_ASSERT(m_pNode, "CList::CConstantIterator::operator++: No value iterator");

				m_pNode = m_pNode->pNext;

				return *this;
			}

			ALCHEMY_INLINE CConstantIterator operator++(int)
			{
				CIterator Result = *this;

				++(*this);

				return Result;
			}

			ALCHEMY_INLINE const T& operator*()const
			{
				ALCHEMY_DEBUG_ASSERT(m_pNode, "CList::CConstantIterator::operator*: No value iterator");

				return m_pNode->Data;
			}

			ALCHEMY_INLINE const T* operator->()const
			{
				return &**this;
			}

			ALCHEMY_INLINE void Reset()
			{
				m_pParent = ALCHEMY_NULL;
				m_pNode   = ALCHEMY_NULL;
			}
		protected:
			LPLISTNODE m_pNode;
			const CList* m_pParent;
		};

		class CIterator : 
			public CConstantIterator
		{
		public:
			CIterator() {}
			CIterator(const CIterator& Iterator) : CConstantIterator(Iterator) {}
			~CIterator() {}

			CIterator& operator=(const CIterator& Iterator)
			{
				CConstantIterator::m_pParent = Iterator.m_pParent;
				CConstantIterator::m_pNode   = Iterator.m_pNode;

				return *this;
			}

			ALCHEMY_INLINE T& operator*()
			{
				ALCHEMY_DEBUG_ASSERT(CConstantIterator::m_pNode, "CList::CConstantIterator::operator*: No value iterator");

				return CConstantIterator::m_pNode->Data;
			}

			ALCHEMY_INLINE T* operator->()
			{
				return &**this;
			}
		};

		CList(void) : m_pHead(ALCHEMY_NULL), m_pTail(ALCHEMY_NULL) {}
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
				pLastTail = __Merge(pHead, uCurrentBlockSize, LessFunction);
				if(pLastTail != ALCHEMY_NULL)
				{
					do 
					{
						pLastTail = __Merge(pLastTail->pNext, uCurrentBlockSize, LessFunction);
					} 
					while(pLastTail != ALCHEMY_NULL);

					uCurrentBlockSize *= 2;
				}
				else
					break;
			}

			return pHead;
		}

		ALCHEMY_INLINE static bool IsValuable(const CConstantIterator& Iterator)
		{
			return Iterator.m_pParent && Iterator.m_pNode;
		}

		ALCHEMY_INLINE bool IsEmpty()
		{
			return m_pHead == ALCHEMY_NULL; 
		}

		ALCHEMY_INLINE void Reset()
		{
			m_pHead = ALCHEMY_NULL;
			m_pTail = ALCHEMY_NULL;

			m_ListNodePool.Reset();
		}

		ALCHEMY_INLINE CConstantIterator GetBeginIterator()const
		{
			CConstantIterator Iterator;
			Iterator.m_pParent = this;
			Iterator.m_pNode = m_pHead;

			return Iterator;
		}

		ALCHEMY_INLINE CIterator GetBeginIterator()
		{
			CIterator Iterator;
			Iterator.m_pParent = this;
			Iterator.m_pNode = m_pHead;

			return Iterator;
		}

		CConstantIterator GetEndIterator()const
		{
			CConstantIterator Iterator;
			Iterator.m_pParent = this;

			if(m_pTail && m_pTail == ALCHEMY_NULL)
				Iterator.m_pNode = m_pTail;
			else if(m_pHead)
			{
				LPLISTNODE pNode = m_pHead;
				while(pNode->pNext)
					pNode = pNode->pNext;

				m_pTail = pNode;

				Iterator.m_pNode = pNode;
			}

			return Iterator;
		}

		CIterator GetEndIterator()
		{
			CIterator Iterator;
			Iterator.m_pParent = this;

			if(m_pTail && m_pTail == ALCHEMY_NULL)
				Iterator.m_pNode = m_pTail;
			else if(m_pHead)
			{
				LPLISTNODE pNode = m_pHead;
				while(pNode->pNext)
					pNode = pNode->pNext;

				m_pTail = pNode;

				Iterator.m_pNode = pNode;
			}

			return Iterator;
		}

		CList<T>& operator+=(const CList<T>& List)
		{
			typename CList<T>::CConstantIterator Iterator = List.GetBeginIterator();

			while( CList<T>::IsValuable(Iterator) )
			{
				Push(*Iterator);

				++ Iterator;
			}

			return *this;
		}

		CList<T>& operator=(const CList<T>& List)
		{
			m_ListNodePool.Reset();

			m_pHead = ALCHEMY_NULL;
			m_pTail = ALCHEMY_NULL;

			*this += List;

			return *this;
		}

		CList<T> operator+(const CList<T>& List)
		{
			CList<T> Result = *this;

			Result += List;

			return Result;
		}

		template<typename LESSFUNCTION>
		void Sort()
		{
			m_pHead = MergeSort<LESSFUNCTION>(m_pHead);
			m_pTail = ALCHEMY_NULL;
		}

		void Unshift(const T& Data, CConstantIterator* pIterator = ALCHEMY_NULL)
		{
			LISTNODE Node;
			UINT uIndex = m_ListNodePool.Add(Node);

			LISTNODE& CurrentNode = m_ListNodePool[uIndex];
			CurrentNode.Data  = Data;
			CurrentNode.pNext = m_pHead;
			m_pHead           = &CurrentNode;

			if(!CurrentNode.pNext)
				m_pTail = &CurrentNode;

			if(pIterator)
			{
				pIterator->m_pParent = this;
				pIterator->m_pNode   = &CurrentNode;
			}
		}

		bool Shift(T* pOutputData = ALCHEMY_NULL)
		{
			if(m_pHead)
			{
				if(pOutputData)
					*pOutputData = m_pHead->Data;

				m_pHead = m_pHead->pNext;

				if(!m_pHead)
					m_pTail = ALCHEMY_NULL;

				return true;
			}

			return false;
		}

		void Push(const T& Data, CConstantIterator* pIterator = ALCHEMY_NULL)
		{
			LISTNODE Node;
			UINT uIndex = m_ListNodePool.Add(Node);

			LISTNODE& CurrentNode = m_ListNodePool[uIndex];
			CurrentNode.Data = Data;
			CurrentNode.pNext = ALCHEMY_NULL;

			if(m_pTail && m_pTail->pNext == ALCHEMY_NULL)
				m_pTail->pNext = &CurrentNode;
			else if(m_pHead)
			{
				LPLISTNODE pNode = m_pHead;
				while(pNode->pNext)
					pNode = pNode->pNext;

				pNode->pNext = &CurrentNode;
			}
			else
				m_pHead = &CurrentNode;

			m_pTail = &CurrentNode;

			if(pIterator)
			{
				pIterator->m_pParent = this;
				pIterator->m_pNode   = &CurrentNode;
			}
		}

		bool Insert(const T& Data, CConstantIterator& From)
		{
			if(From.m_pParent == this)
			{
				if(From.m_pNode)
				{
					LISTNODE Node;
					UINT uIndex = m_ListNodePool.Add(Node);

					LISTNODE& CurrentNode = m_ListNodePool[uIndex];
					CurrentNode.Data = Data;
					CurrentNode.pNext = From.m_pNode->pNext;

					From.m_pNode->pNext = &CurrentNode;

					if(CurrentNode.pNext == ALCHEMY_NULL)
						m_pTail = &CurrentNode;
				}
				else
				{
					m_pHead = m_pHead->pNext;
					if(m_pHead == ALCHEMY_NULL)
						m_pTail = ALCHEMY_NULL;
				}

				return true;
			}

			return false;
		}

		bool Remove(CConstantIterator& From)
		{
			if(From.m_pParent == this && From.m_pNode)
			{
				LPLISTNODE pNext = From.m_pNode->pNext;
				if(pNext)
				{
					if(pNext->pNext == ALCHEMY_NULL)
					{
						m_pTail        = From.m_pNode;
						m_pTail->pNext = ALCHEMY_NULL;
					}
					else
					{
						From.m_pNode->pNext = pNext->pNext;
						pNext->pNext        = ALCHEMY_NULL;
					}

					return true;
				}
			}

			return false;
		}

		template<typename EQUALFUNCTION>
		bool Remove(const T& Data)
		{
			if(!m_pHead)
				return false;

			EQUALFUNCTION EqualFunction;

			if( EqualFunction(m_pHead->Data, Data) )
			{
				m_pHead = m_pHead->pNext;

				if(!m_pHead)
					m_pTail = ALCHEMY_NULL;

				return true;
			}

			CConstantIterator From = GetBeginIterator();
			while( IsValuable(From) )
			{
				if( EqualFunction(*From, Data) )
					return Remove(From);

				++ From;
			}

			return false;
		}

		bool Remove(const T& Data)
		{
			if(!m_pHead)
				return false;

			if(&m_pHead->Data == &Data)
			{
				m_pHead = m_pHead->pNext;

				if(!m_pHead)
					m_pTail = ALCHEMY_NULL;

				return true;
			}

			CConstantIterator From = GetBeginIterator();
			while( IsValuable(From) )
			{
				if(&(*From) == &Data)
					return Remove(From);

				++ From;
			}

			return false;
		}

		void Clear()
		{
			m_pHead = ALCHEMY_NULL;
			m_pTail = ALCHEMY_NULL;

			m_ListNodePool.Clear();
		}

	private:
		static LPLISTNODE __GetTailNode(LPLISTNODE pHead, UINT uLength)
		{
			while(uLength -- && pHead->pNext != ALCHEMY_NULL)
				pHead = pHead->pNext;

			return pHead;
		}

		template<typename LESSFUNCTION>
		static LPLISTNODE __Merge(LPLISTNODE& pHead, UINT uCurrentBlockSize, LESSFUNCTION LessFunction)
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
					pCurrentTailNode = __GetTailNode(pCurrentSortedNode, uCurrentBlockSize - uRightCount);

					break;
				}
				else if(uRightCount == uCurrentBlockSize || pRightBlock == ALCHEMY_NULL)
				{
					pTempNode = pCurrentSortedNode->pNext;

					pCurrentSortedNode->pNext = pLeftBlock;
					pCurrentTailNode          = __GetTailNode(pCurrentSortedNode, uCurrentBlockSize - uLeftCount);
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
		LPLISTNODE m_pTail;

		CPool<LISTNODE> m_ListNodePool;
	};
}
