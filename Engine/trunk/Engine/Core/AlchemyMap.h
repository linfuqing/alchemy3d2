#pragma once

#include "AlchemyDataType.h"
#include "AlchemyDebug.h"
#include "AlchemyFunction.h"
#include "AlchemyNull.h"
#include "AlchemyRBTree.h"
#include "AlchemyPair.h"

namespace alchemy
{
	template<typename KEY, typename VALUE, typename LESSFUNCTION>
	struct PAIR_LESS
	{
		bool operator()(const PAIR<KEY, VALUE>& Left, const PAIR<KEY, VALUE>& Right)const
		{
			return lessFun(Left.first,Right.first);
		}

		LESSFUNCTION	lessFun;
	};

	template<typename KEY, typename VALUE, typename EQUALFUNCTION>
	struct PAIR_EQUAL
	{
		bool operator()(const PAIR<KEY, VALUE>& Left, const PAIR<KEY, VALUE>& Right)const
		{
			return equalFun(Left.first,Right.first);
		}

		EQUALFUNCTION	equalFun;
	};


	template<typename KEY, typename VALUE, typename LESSFUNCION = BASICLESS<KEY>,  typename EQUALFUNCTION = BASICEQUAL<KEY>, typename ALLOCATOR = CPoolAllocator< TREENODE< PAIR<KEY,VALUE> > > >
	class CMap :
		public CRBTree< PAIR<KEY,VALUE>,PAIR_LESS<KEY, VALUE, LESSFUNCION> ,PAIR_EQUAL<KEY, VALUE, EQUALFUNCTION>,ALLOCATOR>
	{
		typedef CRBTree< PAIR<KEY,VALUE>,PAIR_LESS<KEY, VALUE, LESSFUNCION> ,PAIR_EQUAL<KEY, VALUE, EQUALFUNCTION>,ALLOCATOR> BASE_TYPE;
	public:
		typedef PAIR<KEY,VALUE>		RB_TREE_NODE_VALUE;

		class CFastIteratorInterface
		{
		public:
			virtual void OnIterator( PAIR<KEY,VALUE>& pairVal ) = 0;
		};

		class CConstantIterator
		{
			friend class CMap;
		public:
			CConstantIterator():m_pCurrentNode(&BASE_TYPE::sm_NULL_NODE) {}
			~CConstantIterator(){}

			const KEY& GetKey()
			{
				ALCHEMY_DEBUG_ASSERT(m_pCurrentNode, "CMap::CConstantIterator::GetKey: No value iterator.");
				return m_pCurrentNode->data.first;
			}

			const VALUE& GetValue()
			{
				ALCHEMY_DEBUG_ASSERT(m_pCurrentNode, "CMap::CConstantIterator::GetValue: No value iterator.");
				return m_pCurrentNode->data.second;
			}

			CConstantIterator& operator++()
			{
				ALCHEMY_DEBUG_ASSERT(m_pCurrentNode, "CMap::CConstantIterator::GetValue: No value iterator.");

				if(m_pCurrentNode == &BASE_TYPE::sm_NULL_NODE)
					return *this;

				typename BASE_TYPE::RBTREENODE* pNode = &BASE_TYPE::sm_NULL_NODE;

				typename BASE_TYPE::RBTREENODE* pParent = m_pCurrentNode->pParent;
				pNode = m_pCurrentNode->pRightChild;

				if( pNode == &BASE_TYPE::sm_NULL_NODE )
				{
					if( pParent != &BASE_TYPE::sm_NULL_NODE )
					{
						if(m_pCurrentNode == pParent->pLeftChild)
						{
							m_pCurrentNode = pParent;
							return *this;
						}
						else
						{
							do 
							{
								pNode = pParent;
								pParent = pParent->pParent;

							} while (pParent != &BASE_TYPE::sm_NULL_NODE && pParent->pRightChild == pNode );

							m_pCurrentNode = pParent;
							return *this;
						}
					}
					else
					{
						while( pNode->pLeftChild != &BASE_TYPE::sm_NULL_NODE )
							pNode = pNode->pLeftChild;

						m_pCurrentNode = pNode;
						return *this; 
					}
				}

				while( pNode->pLeftChild != &BASE_TYPE::sm_NULL_NODE )
					pNode = pNode->pLeftChild;

				m_pCurrentNode = pNode;
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
			typename BASE_TYPE::RBTREENODE*		m_pCurrentNode;
		};

		class CIterator : public CConstantIterator
		{
		public:
			CIterator(){}
			~CIterator(){}

			VALUE& GetValue()
			{
				ALCHEMY_DEBUG_ASSERT(CConstantIterator::m_pCurrentNode, "CMap::CConstantIterator::GetValue: No value iterator.");
				return CConstantIterator::m_pCurrentNode->data.second;
			}
		};

		CMap()
		{
			
		}

		CMap( const CMap& rhs )
		:BASE_TYPE(rhs)
		{
			
		}

		~CMap() 
		{
			
		}

		CMap& operator = (const CMap& rhs)
		{
			if(this == &rhs)
				return *this;

			BASE_TYPE::CopyFrom(rhs);

			return *this;
		}

		void Clear()
		{
			BASE_TYPE::Reset();
		}

		VALUE* Get(const KEY& Key)
		{
			RB_TREE_NODE_VALUE tmpNode;
			tmpNode.first = Key;

			typename BASE_TYPE::RBTREENODE* pNode = BASE_TYPE::GetNode(tmpNode);
			//ALCHEMY_DEBUG_ASSERT(pNode, "CMap::Get: No value iterator.");

			return pNode ? &pNode->data.second : ALCHEMY_NULL;
		}

		const VALUE* Get(const KEY& Key)const
		{
			RB_TREE_NODE_VALUE tmpNode;
			tmpNode.first = Key;

			const typename BASE_TYPE::RBTREENODE * pNode = BASE_TYPE::GetNode(tmpNode);
			//ALCHEMY_DEBUG_ASSERT(pNode, "CMap::Get: No value iterator.");

			return pNode ? &pNode->data.second : ALCHEMY_NULL;
		}

		bool Insert(const KEY& Key, const VALUE& Value, VALUE* pSource = ALCHEMY_NULL)
		{
			RB_TREE_NODE_VALUE	tmpNode;
			tmpNode.first = Key;
			tmpNode.second = Value;
			bool bRet = BASE_TYPE::InsertNode(tmpNode,&tmpNode);

			if(pSource)
				*pSource = tmpNode.second;

			return bRet;
		}

		void Remove( const KEY& Key, VALUE* pSource = ALCHEMY_NULL)
		{
			RB_TREE_NODE_VALUE	tmpNode, sourceNode;
			tmpNode.first = Key;
			BASE_TYPE::RemoveNode(tmpNode, &sourceNode);
			if(pSource)
				*pSource = sourceNode.second;
		}

		VALUE& operator[](const KEY& Key)
		{
			RB_TREE_NODE_VALUE tmpNode;
			tmpNode.first = Key;

			typename BASE_TYPE::RBTREENODE* pNode = BASE_TYPE::GetNode(tmpNode);
			ALCHEMY_DEBUG_ASSERT(pNode, "CMap::operator[]: No value iterator.");

			return pNode->data.second;
		}

		const VALUE& operator[](const KEY& Key)const
		{
			RB_TREE_NODE_VALUE tmpNode;
			tmpNode.first = Key;

			const typename BASE_TYPE::RBTREENODE* pNode = GetNode(tmpNode);
			ALCHEMY_DEBUG_ASSERT(pNode, "CMap::operator[]: No value iterator.");

			return pNode ? pNode->data.second : tmpNode.second;
		}

		static bool IsValuable( CConstantIterator& Iter )
		{
			return Iter.m_pCurrentNode != &BASE_TYPE::sm_NULL_NODE;
		}

		CConstantIterator GetBeginIterator()const
		{
			CConstantIterator Iter;
			typename BASE_TYPE::RBTREENODE* pNode = BASE_TYPE::m_pRootNode;

			if(pNode == &BASE_TYPE::sm_NULL_NODE)
				Iter.m_pCurrentNode = pNode;
			else
			{
				while( pNode->pLeftChild != &BASE_TYPE::sm_NULL_NODE)
					pNode = pNode->pLeftChild;

				Iter.m_pCurrentNode = pNode;
			}

			return Iter;
		}

		CIterator GetBeginIterator()
		{
			CIterator Iter;
			typename BASE_TYPE::RBTREENODE* pNode = BASE_TYPE::m_pRootNode;

			if(pNode == &BASE_TYPE::sm_NULL_NODE)
				Iter.m_pCurrentNode = pNode;
			else
			{
				while( pNode->pLeftChild != &BASE_TYPE::sm_NULL_NODE)
					pNode = pNode->pLeftChild;

				Iter.m_pCurrentNode = pNode;
			}

			return Iter;
		}

		void FastIterator( CFastIteratorInterface* pIterator )
		{
			__IterTree(BASE_TYPE::m_pRootNode, pIterator);
		}

	private:
		void __IterTree( typename BASE_TYPE::RBTREENODE* pNode, CFastIteratorInterface* pIterator )
		{
			if(!pIterator)
				return;

			if(pNode)
			{
				RB_TREE_NODE_VALUE pair;

				if(pNode->pLeftChild)
					__IterTree( pNode->pLeftChild, pIterator );

				pair.first = pNode->key;
				pair.second = pNode->data;
				pIterator->OnIterator( pair );

				if(pNode->pRightChild)
					__IterTree( pNode->pRightChild, pIterator );
			}
		}
	};
}
