#pragma once

#include "AlchemyDataType.h"
#include "AlchemyDebug.h"
#include "AlchemyFunction.h"

namespace alchemy
{
	template<typename KEY, typename VALUE, typename LESSFUNCION = BASICLESS<KEY>, typename EQUALFUNCTION = BASICEQUAL<KEY> >
	class CMap
	{
		typedef struct _TREENODE
		{
			typedef enum 
			{
				RED,
				BLACK
			}ATTRIBUTE;

			KEY					key;
			VALUE				data;

			struct _TREENODE*	pParent;
			struct _TREENODE*	pLeftChild;
			struct _TREENODE*	pRightChild;
			ATTRIBUTE				color;
		}TREENODE,*PTREENODE;

		typedef struct _LISTNODE
		{
			struct _LISTNODE*	pNext;
			TREENODE*			pData;
		}LISTNODE,*PLISTNODE;

	public:
		class CConstantIterator
		{
			friend class CMap;
		public:
			CConstantIterator():m_pCurrentNode(ALCHEMY_NULL) {}
			~CConstantIterator(){}

			const KEY& GetKey()
			{
				ALCHEMY_DEBUG_ASSERT(m_pCurrentNode, "CMap::CConstantIterator::GetKey: No value iterator.");
				return m_pCurrentNode->key;
			}

			const VALUE& GetValue()
			{
				ALCHEMY_DEBUG_ASSERT(m_pCurrentNode, "CMap::CConstantIterator::GetValue: No value iterator.");
				return m_pCurrentNode->data;
			}

			CConstantIterator& operator++()
			{
				ALCHEMY_DEBUG_ASSERT(m_pCurrentNode, "CMap::CConstantIterator::GetValue: No value iterator.");

				m_pCurrentNode = CMap::GetNeighborNode(m_pCurrentNode);

				return *this; 
			}

			CConstantIterator operator++(int)
			{
				ALCHEMY_DEBUG_ASSERT(m_pCurrentNode, "CMap::CConstantIterator::operator++: No value iterator.");

				CConstantIterator Result = *this;

				++(*this);

				return Result;
			}

		protected:
			PTREENODE		m_pCurrentNode;
		};

		class CIterator : public CConstantIterator
		{
		public:
			CIterator(){}
			~CIterator(){}

			VALUE& GetValue()
			{
				ALCHEMY_DEBUG_ASSERT(CConstantIterator::m_pCurrentNode, "CMap::CConstantIterator::GetValue: No value iterator.");
				return CConstantIterator::m_pCurrentNode->data;
			}
		};

		static bool IsValuable(const CIterator& Iteraotr)
		{
			return Iteraotr.m_pCurrentNode != ALCHEMY_NULL;
		}

		CMap():m_pRootNode(ALCHEMY_NULL)//,m_pListNode(ALCHEMY_NULL)
		{
			sm_NullNode.color = TREENODE::BLACK;
			sm_NullNode.pLeftChild = sm_NullNode.pRightChild = ALCHEMY_NULL;
		}
		~CMap() 
		{
			Clear();
		}

		CConstantIterator& GetBeginIterator()const
		{
			if(!m_pRootNode)
			{
				m_beginIter.m_pCurrentNode = m_pRootNode;
				return m_beginIter;
			}

			_TREENODE* pNode = m_pRootNode;
			while(pNode->pLeftChild)
				pNode = pNode->pLeftChild;

			m_beginIter.m_pCurrentNode = pNode;
			return m_beginIter;
		}

		CIterator& GetBeginIterator()
		{
			if(!m_pRootNode)
			{
				m_beginIter.m_pCurrentNode = m_pRootNode;
				return m_beginIter;
			}

			_TREENODE* pNode = m_pRootNode;
			while(pNode->pLeftChild && pNode->pLeftChild != &sm_NullNode)
				pNode = pNode->pLeftChild;

			m_beginIter.m_pCurrentNode = pNode;

			return m_beginIter;
		}

		void Clear()
		{
			_deleteTree(m_pRootNode);
			m_pRootNode = ALCHEMY_NULL;

			/*PLISTNODE pNode = m_pListNode, pNextNode;
			while(pNode != ALCHEMY_NULL)
			{
				pNextNode = pNode->pNext;
				delete pNode;
				pNode = pNextNode;
			}

			m_pListNode = ALCHEMY_NULL;*/
		}

		static PTREENODE GetNeighborNode(_TREENODE* pCurNode)
		{
			if( !_isValueNode(pCurNode) )
				return ALCHEMY_NULL;

			_TREENODE* pNode = ALCHEMY_NULL;

			_TREENODE* pParent = pCurNode->pParent;
			pNode = pCurNode->pRightChild;

			if( !_isValueNode(pNode) )
			{
				if( _isValueNode(pParent) )
				{
					if(pCurNode == pParent->pLeftChild)
						return pParent;
					else
					{
						do 
						{
							pNode = pParent;
							pParent = pParent->pParent;

						} while (_isValueNode(pParent) && pParent->pRightChild == pNode );

						return _isValueNode(pParent) ? pParent : ALCHEMY_NULL;
					}
				}
				else
				{
					while( _isValueNode(pNode->pLeftChild) )
						pNode = pNode->pLeftChild;

					return pNode;
				}
			}

			while( _isValueNode(pNode->pLeftChild) )
				pNode = pNode->pLeftChild;

			return pNode;
		}

		bool Insert(const KEY& Key, const VALUE& Value, VALUE* pSource = ALCHEMY_NULL)
		{
			if(m_pRootNode == ALCHEMY_NULL)
			{
				m_pRootNode = new TREENODE;
				m_pRootNode->key = Key;
				m_pRootNode->data = Value;
				m_pRootNode->pParent = ALCHEMY_NULL;
				m_pRootNode->pLeftChild = m_pRootNode->pRightChild = &sm_NullNode;
				m_pRootNode->color = TREENODE::BLACK;

				/*m_pListNode = new LISTNODE;
				m_pListNode->pNext = ALCHEMY_NULL;
				m_pListNode->pData = m_pRootNode;*/

				return true;
			}

			PTREENODE pParentNode = ALCHEMY_NULL;
			PTREENODE pFindNode = _findNode(Key,m_pRootNode,&pParentNode);
			if(pFindNode != ALCHEMY_NULL && pFindNode != &sm_NullNode)
			{
				if(pSource != ALCHEMY_NULL)
					*pSource = pFindNode->data;

				pFindNode->data = Value;

				return true;
			}

			PTREENODE pNode = _addChild(Key,Value,pParentNode);
			_adjustTree(pNode);

			/*LISTNODE* pNewNode = new LISTNODE;
			pNewNode->pData = pNode;
			pNewNode->pNext = m_pListNode;

			m_pListNode = pNewNode;*/

			return true;
		}

		VALUE* Get(const KEY& Key)
		{
			PTREENODE pNode = _findNode(Key,m_pRootNode);
			if(pNode != ALCHEMY_NULL)
				return &pNode->data;
			else
				return ALCHEMY_NULL;
		}

		VALUE& operator[](const KEY& Key)
		{
			VALUE* pData = Get(Key);
			ALCHEMY_DEBUG_ASSERT(pData, "CMap::operator[]: No value iterator.");

			return *pData;
		}

		const VALUE& operator[](const KEY& Key)const
		{
			VALUE* pData = Get(Key);
			ALCHEMY_DEBUG_ASSERT(pData, "CMap::operator[]: No value iterator.");

			return *pData;
		}

	protected:
		static bool _isValueNode(PTREENODE pNode)
		{
			if(pNode == ALCHEMY_NULL || pNode == &sm_NullNode)
				return false;

			return true;
		}

		void _adjustTree(PTREENODE pNode)
		{
			if(pNode != ALCHEMY_NULL && pNode != m_pRootNode)
			{
				if(pNode->pParent->color == TREENODE::BLACK)
					return;

				bool isFlip = false;
				PTREENODE pUncle;
				if(pNode->pParent->pParent->pLeftChild == pNode->pParent)
				{
					pUncle = pNode->pParent->pParent->pRightChild;
					isFlip = true;
				}
				else
					pUncle = pNode->pParent->pParent->pLeftChild;

				if(pUncle != ALCHEMY_NULL)
				{
					if(pUncle->color == TREENODE::RED)
					{
						pNode->pParent->color = TREENODE::BLACK;
						pUncle->color = TREENODE::BLACK;
						if(pUncle->pParent != m_pRootNode)
							pUncle->pParent->color = TREENODE::RED;
						if(pUncle->pParent->pParent != ALCHEMY_NULL && pUncle->pParent->pParent->color == TREENODE::RED)
							_adjustTree(pUncle->pParent);
					}
					else
					{
						if(isFlip == true)
						{
							if(pNode->pParent->pLeftChild == pNode)
							{
								_rotateNode(pNode->pParent,1);
								pNode->pParent->color = TREENODE::BLACK;
								pNode->pParent->pRightChild->color = TREENODE::RED;
							}
							else
							{
								_rotateNode(pNode,0);
								_rotateNode(pNode,1);
								pNode->color = TREENODE::BLACK;
								pNode->pRightChild->color = TREENODE::RED;
							}
						}
						else
						{
							if(pNode->pParent->pLeftChild == pNode)
							{
								_rotateNode(pNode,1);
								_rotateNode(pNode,0);
								pNode->color = TREENODE::BLACK;
								pNode->pLeftChild->color = TREENODE::RED;
							}
							else
							{
								_rotateNode(pNode->pParent,0);
								pNode->pParent->color = TREENODE::BLACK;
								pNode->pParent->pLeftChild->color = TREENODE::RED;
							}
						}
					}
				}
			}

			m_pRootNode->color = TREENODE::BLACK;
		}

		void _deleteTree(PTREENODE pNode)
		{
			if(pNode == ALCHEMY_NULL || pNode == &sm_NullNode)
				return;

			_deleteTree(pNode->pLeftChild);
			_deleteTree(pNode->pRightChild);

			delete pNode;
		}

		PTREENODE _findNode(const KEY& key, PTREENODE pNode, PTREENODE* ppParentNode = ALCHEMY_NULL)
		{
			while(pNode != ALCHEMY_NULL && pNode != &sm_NullNode)
			{
				if(ppParentNode != ALCHEMY_NULL)
					*ppParentNode = pNode;

				if(m_EqualFun(pNode->key,key))
					return pNode;

				if( m_CompareFun(pNode->key,key) )
					pNode = pNode->pRightChild;
				else
					pNode = pNode->pLeftChild;
			}

			return ALCHEMY_NULL;
		}

		PTREENODE _addChild(const KEY& key, const VALUE& val, PTREENODE pParent)
		{
			PTREENODE pNode = new TREENODE;
			pNode->data = val;
			pNode->key = key;
			pNode->pLeftChild = pNode->pRightChild = &sm_NullNode;
			pNode->pParent = pParent;
			pNode->color = TREENODE::RED;

			if( m_CompareFun(pParent->key,key) )
				pParent->pRightChild = pNode;
			else
				pParent->pLeftChild = pNode;

			return pNode;
		}

		void _rotateNode(PTREENODE pNode, int dir)
		{
			if(pNode != ALCHEMY_NULL)
			{
				PTREENODE pParentNode = pNode->pParent;
				if(pParentNode == ALCHEMY_NULL)
					return;

				PTREENODE pGPNode = pParentNode->pParent;

				if(pGPNode == ALCHEMY_NULL)
					m_pRootNode = pNode;
				else
				{
					if(pParentNode == pGPNode->pLeftChild)
						pGPNode->pLeftChild = pNode;
					else if(pParentNode == pGPNode->pRightChild)
						pGPNode->pRightChild = pNode;
				}

				pNode->pParent = pGPNode;
				pParentNode->pParent = pNode;

				PTREENODE pTmpNode;
				if(dir == 0)	//left
				{
					pTmpNode = pNode->pLeftChild;
					pNode->pLeftChild = pParentNode;
					pTmpNode->pParent = pParentNode;
					pParentNode->pRightChild = pTmpNode;
				}
				else if(dir == 1)
				{
					pTmpNode = pNode->pRightChild;
					pNode->pRightChild = pParentNode;
					pTmpNode->pParent = pParentNode;
					pParentNode->pLeftChild = pTmpNode;
				}
			}
		}

	private:
		static TREENODE			sm_NullNode;
		PTREENODE			m_pRootNode;

		//PLISTNODE			m_pListNode;

		CIterator			m_beginIter;
		EQUALFUNCTION		m_EqualFun;
		LESSFUNCION			m_CompareFun;
	};

	template<typename KEY, typename VALUE, typename LESSFUNCION, typename EQUALFUNCTION >
	typename CMap<KEY,VALUE,LESSFUNCION,EQUALFUNCTION>::TREENODE CMap<KEY,VALUE,LESSFUNCION,EQUALFUNCTION>::sm_NullNode;
}
