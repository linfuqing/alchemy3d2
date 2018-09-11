#pragma once

//#include "AlchemyDataType.h"
//#include "AlchemyDebug.h"
#include "AlchemyFunction.h"
//#include "AlchemyNull.h"
#include "AlchemyPoolAllocator.h"


namespace alchemy
{
	typedef enum
	{
		RED,
		BLACK
	}NodeColor;

	/*template<typename KEY, typename VALUE>*/
	template<typename TREEVALUE>
	struct TREENODE
	{
		/*KEY			key;
		VALUE		data;*/
		TREEVALUE	data;
		NodeColor	color;

		TREENODE* pLeftChild, *pRightChild;
		TREENODE* pParent;
	};

	template<typename TREEVALUE, typename LESSFUNCION = BASICLESS<TREEVALUE>, typename EQUALFUNCTION = BASICEQUAL<TREEVALUE>, typename ALLOCATOR = CPoolAllocator< TREENODE<TREEVALUE> > >
	class CRBTree
	{
	public:
		typedef TREENODE<TREEVALUE>	 RBTREENODE;

		CRBTree()
		:m_pRootNode(&sm_NULL_NODE)
		{
			sm_NULL_NODE.pLeftChild = sm_NULL_NODE.pRightChild = sm_NULL_NODE.pParent = &sm_NULL_NODE;
			sm_NULL_NODE.color = BLACK;
		}

		CRBTree( const CRBTree& rhs )
		{
			CopyFrom(rhs);
		}

		~CRBTree()
		{
			_deleteTree(m_pRootNode, true);
			m_pRootNode = &sm_NULL_NODE;
		}

		CRBTree& operator = (const CRBTree& rhs)
		{
			if(this == &rhs)
				return *this;

			CopyFrom(rhs);

			return *this;
		}

		bool InsertNode(const TREEVALUE& Value, TREEVALUE* pSource = ALCHEMY_NULL)
		{
			bool isLeft; 
			RBTREENODE* pNode = m_pRootNode, *pCurParentNode = &sm_NULL_NODE;

			while( pNode != &sm_NULL_NODE)
			{
				pCurParentNode = pNode;

				if( m_EqualFun(Value,pNode->data) )
				{
					pNode->data = Value;
					if(pSource)
						*pSource = pNode->data;

					return true;
				}

				if( m_CompareFun(Value, pNode->data) )
				{
					isLeft = true;
					pNode = pNode->pLeftChild;
				}
				else
				{
					isLeft = false;
					pNode = pNode->pRightChild;
				}
			}

			pNode = __CreateNode(Value);
			pNode->pParent = pCurParentNode;

			if(pCurParentNode == &sm_NULL_NODE)
			{
				m_pRootNode = pNode;
				m_pRootNode->color = BLACK;
			}
			else
			{
				if(isLeft)
					pCurParentNode->pLeftChild = pNode;
				else
					pCurParentNode->pRightChild = pNode;

				__insertAdjustTree( pNode );
			}

			return true;
		}

		void RemoveNode(const TREEVALUE& Key, TREEVALUE* pSource = ALCHEMY_NULL)
		{
			if(m_pRootNode == &sm_NULL_NODE)
				return;

			RBTREENODE* pNode = m_pRootNode;
			while (pNode != &sm_NULL_NODE)
			{
				if( m_EqualFun(Key,pNode->data) )
				{
					if(pSource)
						*pSource = pNode->data;

					break;
				}

				if( m_CompareFun(Key, pNode->data) )
					pNode = pNode->pLeftChild;
				else
					pNode = pNode->pRightChild;
			}

			if(pNode == &sm_NULL_NODE)
				return;

			RBTREENODE* pCurNode, *pDeleteNoe;
			if( pNode->pLeftChild == &sm_NULL_NODE || pNode->pRightChild == &sm_NULL_NODE )
				pCurNode = pNode;
			else
			{
				pCurNode = pNode->pRightChild;
				while (pCurNode->pLeftChild != &sm_NULL_NODE)
					pCurNode = pCurNode->pLeftChild;
			}

			if(pCurNode->pLeftChild != &sm_NULL_NODE)
				pDeleteNoe = pCurNode->pLeftChild;
			else
				pDeleteNoe = pCurNode->pRightChild;

			pDeleteNoe->pParent = pCurNode->pParent;
			if(pCurNode->pParent == &sm_NULL_NODE)
				m_pRootNode = pDeleteNoe;
			else if(pCurNode == pCurNode->pParent->pLeftChild)
				pCurNode->pParent->pLeftChild = pDeleteNoe;
			else
				pCurNode->pParent->pRightChild = pDeleteNoe;

			if(pCurNode != pNode)
				pNode->data = pCurNode->data;

			if( pCurNode->color == BLACK)
				__deleteAdjustTree( pDeleteNoe );

			m_Allocator.Dellocate( pCurNode );
		}

		void Reset()
		{
			_deleteTree(m_pRootNode);
			m_pRootNode = &sm_NULL_NODE;
		}

		RBTREENODE*  GetNode( const TREEVALUE& Key )
		{
			RBTREENODE* pNode = m_pRootNode;
			while (pNode != &sm_NULL_NODE)
			{
				if( m_EqualFun(Key,pNode->data) )
					return pNode;

				if( m_CompareFun(Key, pNode->data) )
					pNode = pNode->pLeftChild;
				else
					pNode = pNode->pRightChild;
			}

			return ALCHEMY_NULL;
		}

		const RBTREENODE*  GetNode( const TREEVALUE& Key ) const
		{
			RBTREENODE* pNode = m_pRootNode;
			while (pNode != &sm_NULL_NODE)
			{
				if( m_EqualFun(Key,pNode->data) )
					return pNode;

				if( m_CompareFun(Key, pNode->data) )
					pNode = pNode->pLeftChild;
				else
					pNode = pNode->pRightChild;
			}

			return ALCHEMY_NULL;
		}

		void CopyFrom( const CRBTree& rhs )
		{
			if(m_pRootNode)
				Reset();

			if(rhs.m_pRootNode != &sm_NULL_NODE)
			{
				m_pRootNode = static_cast<RBTREENODE*>( m_Allocator.Allocate() );
				m_pRootNode->pParent = &sm_NULL_NODE;
				_copyFrom(m_pRootNode, rhs.m_pRootNode);
			}
		}

	protected:
		RBTREENODE* __CreateNode( const TREEVALUE& Value )
		{
			RBTREENODE* pNode;

			pNode = static_cast<RBTREENODE*>( m_Allocator.Allocate() );
			
			pNode->data = Value;
			pNode->color = RED;
			pNode->pLeftChild = pNode->pRightChild = pNode->pParent = &sm_NULL_NODE;
			return pNode;
		}

		void __insertAdjustTree( RBTREENODE* pNode )
		{
			RBTREENODE* pUncle;
			while (pNode->pParent->color == RED)
			{
				if(pNode->pParent == pNode->pParent->pParent->pLeftChild)
				{
					pUncle = pNode->pParent->pParent->pRightChild;
					if(pUncle->color == RED)
					{
						pNode->pParent->color = pUncle->color = BLACK;
						pNode->pParent->pParent->color = RED;
						pNode = pNode->pParent->pParent;
					}
					else
					{
						if(pNode == pNode->pParent->pRightChild)
						{
							pNode = pNode->pParent;
							__LeftRotate( pNode );
						}

						pNode->pParent->color = BLACK;
						pNode->pParent->pParent->color = RED;
						__RightRotate( pNode->pParent->pParent );
					}
				}
				else
				{
					pUncle = pNode->pParent->pParent->pLeftChild;
					if(pUncle->color == RED)
					{
						pNode->pParent->color = pUncle->color = BLACK;
						pNode->pParent->pParent->color = RED;
						pNode = pNode->pParent->pParent;
					}
					else
					{
						if(pNode == pNode->pParent->pLeftChild)
						{
							pNode = pNode->pParent;
							__RightRotate( pNode );
						}

						pNode->pParent->color = BLACK;
						pNode->pParent->pParent->color = RED;
						__LeftRotate( pNode->pParent->pParent );
					}
				}
			}

			m_pRootNode->color = BLACK;
		}

		void __deleteAdjustTree( RBTREENODE* pNode )
		{
			RBTREENODE* pBrother;
			while (pNode != m_pRootNode && pNode->color == BLACK)
			{
				if( pNode == pNode->pParent->pLeftChild )
				{
					pBrother = pNode->pParent->pRightChild;

					if( pBrother->color == RED )
					{
						pBrother->color = BLACK;
						pNode->pParent->color = RED;
						__LeftRotate( pNode->pParent );
						pBrother = pNode->pParent->pRightChild;
					}

					if( pBrother->pLeftChild->color == BLACK && pBrother->pRightChild->color == BLACK )
					{
						pBrother->color = RED;
						pNode = pNode->pParent;
					}
					else
					{
						if( pBrother->pRightChild->color == BLACK )
						{
							pBrother->pLeftChild->color = BLACK;
							pBrother->color = RED;
							__RightRotate( pBrother );
							pBrother = pBrother->pParent->pRightChild;
						}

						pBrother->color = pNode->pParent->color;
						pNode->pParent->color = BLACK;
						pBrother->pRightChild->color = BLACK;
						__LeftRotate( pNode->pParent );
						pNode = m_pRootNode;
					}
				}
				else
				{
					pBrother = pNode->pParent->pLeftChild;
					if( pBrother->color == RED )
					{
						pBrother->color = BLACK;
						pNode->pParent->color = RED;
						__RightRotate( pNode->pParent );
						pBrother = pNode->pParent->pLeftChild;
					}
					if( pBrother->pRightChild->color == BLACK && pBrother->pLeftChild->color == BLACK )
					{
						pBrother->color = RED;
						pNode = pNode->pParent;
					}
					else
					{
						if( pBrother->pLeftChild->color == BLACK )
						{
							pBrother->pRightChild->color = BLACK;
							pBrother->color = RED;
							__LeftRotate( pBrother );
							pBrother = pBrother->pParent->pLeftChild;
						}

						pBrother->color = pNode->pParent->color;
						pNode->pParent->color = BLACK;
						pBrother->pLeftChild->color = BLACK;
						__RightRotate( pNode->pParent );
						pNode = m_pRootNode;
					}
				}
			}

			pNode->color = BLACK;
		}

		void __LeftRotate( RBTREENODE* pNode )
		{
			RBTREENODE* pRight = pNode->pRightChild;

			pNode->pRightChild = pRight->pLeftChild;
			if(pRight->pLeftChild != &sm_NULL_NODE)
				pRight->pLeftChild->pParent = pNode;
			
			pRight->pParent = pNode->pParent;

			if(pNode->pParent == &sm_NULL_NODE)
				m_pRootNode = pRight;
			else
			{
				if(pNode == pNode->pParent->pLeftChild)
					pNode->pParent->pLeftChild = pRight;
				else
					pNode->pParent->pRightChild = pRight;
			}

			pRight->pLeftChild = pNode;
			pNode->pParent = pRight;
		}

		void __RightRotate( RBTREENODE* pNode )
		{
			RBTREENODE* pRight = pNode->pLeftChild;

			pNode->pLeftChild = pRight->pRightChild;
			if(pRight->pRightChild != &sm_NULL_NODE)
				pRight->pRightChild->pParent = pNode;

			pRight->pParent = pNode->pParent;

			if(pNode->pParent == &sm_NULL_NODE)
				m_pRootNode = pRight;
			else
			{
				if(pNode == pNode->pParent->pRightChild)
					pNode->pParent->pRightChild = pRight;
				else
					pNode->pParent->pLeftChild = pRight;
			}

			pRight->pRightChild = pNode;
			pNode->pParent = pRight;
		}

		void _deleteTree(RBTREENODE* pNode, bool isDestroy = false)
		{
			if(pNode == ALCHEMY_NULL || pNode == &sm_NULL_NODE)
				return;

			_deleteTree(pNode->pLeftChild, isDestroy);
			_deleteTree(pNode->pRightChild, isDestroy);

			if(!isDestroy)
				m_Allocator.Dellocate(pNode);
		}

		void _copyFrom(RBTREENODE* pCopyNode, RBTREENODE* pFromNode)
		{
			pCopyNode->color = pFromNode->color;
			pCopyNode->data = pFromNode->data;

			if(pFromNode->pLeftChild != &sm_NULL_NODE)
			{
				pCopyNode->pLeftChild = static_cast<RBTREENODE*>( m_Allocator.Allocate() );
				pCopyNode->pLeftChild->pParent = pCopyNode;
				_copyFrom(pCopyNode->pLeftChild, pFromNode->pLeftChild);
			}
			else
				pCopyNode->pLeftChild = &sm_NULL_NODE;

			if(pFromNode->pRightChild != &sm_NULL_NODE)
			{
				pCopyNode->pRightChild = static_cast<RBTREENODE*>( m_Allocator.Allocate() );
				pCopyNode->pRightChild->pParent = pCopyNode;
				_copyFrom(pCopyNode->pRightChild, pFromNode->pRightChild);
			}
			else
				pCopyNode->pRightChild = &sm_NULL_NODE;
		}

		RBTREENODE*			m_pRootNode;
		EQUALFUNCTION		m_EqualFun;
		LESSFUNCION			m_CompareFun;
		ALLOCATOR			m_Allocator;

		static RBTREENODE	    sm_NULL_NODE;
	};

	template<typename TREEVALUE, typename LESSFUNCION, typename EQUALFUNCTION, typename ALLOCATOR >
	typename CRBTree<TREEVALUE,LESSFUNCION,EQUALFUNCTION,ALLOCATOR>::RBTREENODE  CRBTree<TREEVALUE,LESSFUNCION,EQUALFUNCTION,ALLOCATOR>::sm_NULL_NODE;
}