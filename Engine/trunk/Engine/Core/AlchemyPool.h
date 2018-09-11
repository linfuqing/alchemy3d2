#pragma once

#include "AlchemyNull.h"
#include "AlchemyDebug.h"
#include "AlchemyBitFlag.h"
#include "AlchemyConstant.h"

namespace alchemy
{
	template<typename TYPE/*, UINT SHIFT = ALCHEMY_CONSTANT_SHIFT( ALCHEMY_TOTAL_BITS(UINT) )*/>
	class CPool
	{
	public:
		typedef enum
		{
			//ARRAY_MEMBER_COUNT_SHIFT = SHIFT,
			ARRAY_MEMBER_COUNT       = ALCHEMY_TOTAL_BITS(UINT),
			ARRAY_MEMBER_COUNT_SHIFT = ALCHEMY_CONSTANT_SHIFT(ARRAY_MEMBER_COUNT)
		}CONTAINS;
	protected:
		typedef struct ARRAY
		{
			TYPE Value[ARRAY_MEMBER_COUNT];
			UINT uFlag;
			UINT uLength;
			struct ARRAY * pNext;
		}ARRAY, * LPARRAY;
	public:
		class CConstantIterator
		{
			friend class CPool;
		public:

			CConstantIterator(): m_pArray(ALCHEMY_NULL), m_uIndex(0), m_uBaseIndex(0) {}
			~CConstantIterator() {}

			ALCHEMY_INLINE UINT GetIndex()const
			{
				return (m_uBaseIndex << ARRAY_MEMBER_COUNT_SHIFT) + m_uIndex;
			}

			CConstantIterator& operator++()
			{
				ALCHEMY_DEBUG_ASSERT(m_pArray, "CPool::CConstantIterator::operator++: No value iterator.");

				while(true)
				{
					++ m_uIndex;

					if(m_uIndex == ARRAY_MEMBER_COUNT)
					{
						m_uIndex  = 0;
						m_pArray = m_pArray->pNext;

						m_uBaseIndex ++;
					}

					if(!m_pArray || ALCHEMY_TEST_BIT(m_pArray->uFlag, m_uIndex) )
						return *this;
				}
			}

			CConstantIterator operator++(int)
			{
				ALCHEMY_DEBUG_ASSERT(m_pArray, "CPool::CConstantIterator::operator++: No value iterator.");

				CIterator Result = *this;

				++(*this);

				return Result;
			}

			bool operator==(const CConstantIterator& Iterator)const
			{
				return m_pArray == Iterator.m_pArray && m_uIndex == Iterator.m_uIndex;
			}

			const TYPE& operator*()const
			{
				ALCHEMY_DEBUG_ASSERT(m_pArray && ALCHEMY_TEST_BIT(m_pArray->uFlag, m_uIndex), "CPool::CConstantIterator::operator*: No value iterator.");

				return m_pArray->Value[m_uIndex];
			}

			const TYPE* operator->()const
			{
				return &**this;
			}
		protected:
			LPARRAY m_pArray;
			UINT m_uIndex;
			UINT m_uBaseIndex;
		};

		class CIterator :
			public CConstantIterator
		{
		public:
			CIterator() {}
			~CIterator() {}

			TYPE& operator*()
			{
				ALCHEMY_DEBUG_ASSERT(CConstantIterator::m_pArray && ALCHEMY_TEST_BIT(CConstantIterator::m_pArray->uFlag, CConstantIterator::m_uIndex), "CPool::CConstantIterator::operator*: No value iterator.");

				return CConstantIterator::m_pArray->Value[CConstantIterator::m_uIndex];
			}

			TYPE* operator->()
			{
				return &**this;
			}
		};

		CPool() : 
		m_pPoolList(ALCHEMY_NULL), 
			m_ppPoolVector(ALCHEMY_NULL), 
			m_uArrayCount(0), 
			m_uMemberCount(0) 
		{

		}
		~CPool(void)
		{
			Clear();
		}

		ALCHEMY_INLINE static bool IsValuable(const CConstantIterator& Iterator)
		{
			return Iterator.m_pArray != ALCHEMY_NULL;
		}

		ALCHEMY_INLINE bool IsEnd(const CConstantIterator& Iterator) const
		{
			return Iterator.m_pArray != ALCHEMY_NULL && Iterator.m_pArray->pNext == ALCHEMY_NULL && Iterator.GetIndex() == (m_uMemberCount - 1);
		}

		ALCHEMY_INLINE UINT GetLength()const
		{
			return m_uMemberCount;
		}

		ALCHEMY_INLINE bool Set(const TYPE& Value, UINT uIndex)
		{
			UINT uArrayIndex = uIndex >> ARRAY_MEMBER_COUNT_SHIFT;

			LPARRAY pArray;
			if(uArrayIndex < m_uArrayCount && (pArray = m_ppPoolVector[uArrayIndex]) != ALCHEMY_NULL)
			{
				uIndex -= (uArrayIndex << ARRAY_MEMBER_COUNT_SHIFT);

				ALCHEMY_SET_BIT(pArray->uFlag, uIndex);

				pArray->Value[uIndex] = Value;

				return true;
			}

			return false;
		}

		ALCHEMY_INLINE TYPE* Get(UINT uIndex)
		{
			UINT uArrayIndex = uIndex >> ARRAY_MEMBER_COUNT_SHIFT;

			LPARRAY pArray;
			if(uArrayIndex < m_uArrayCount && (pArray = m_ppPoolVector[uArrayIndex]) != ALCHEMY_NULL)
			{
				uIndex = uIndex - (uArrayIndex << ARRAY_MEMBER_COUNT_SHIFT);
				return ALCHEMY_TEST_BIT(pArray->uFlag, uIndex) ? &pArray->Value[uIndex] : ALCHEMY_NULL;
			}

			return ALCHEMY_NULL;
		}

		ALCHEMY_INLINE const TYPE* Get(UINT uIndex)const
		{
			UINT uArrayIndex = uIndex >> ARRAY_MEMBER_COUNT_SHIFT;

			LPARRAY pArray;
			if(uArrayIndex < m_uArrayCount && (pArray = m_ppPoolVector[uArrayIndex]) != ALCHEMY_NULL)
			{
				uIndex = uIndex - (uArrayIndex << ARRAY_MEMBER_COUNT_SHIFT);
				return ALCHEMY_TEST_BIT(pArray->uFlag, uIndex) ? &pArray->Value[uIndex] : ALCHEMY_NULL;
			}

			return ALCHEMY_NULL;
		}

		ALCHEMY_INLINE const TYPE& operator[](UINT uIndex)const
		{
			const TYPE* pValue = Get(uIndex);

			ALCHEMY_DEBUG_ASSERT(pValue, "uIndex out of range.");

			return *pValue;
		}

		ALCHEMY_INLINE TYPE& operator[](UINT uIndex)
		{
			TYPE* pValue = Get(uIndex);

			ALCHEMY_DEBUG_ASSERT(pValue, "uIndex out of range.");

			return *pValue;
		}

		CPool<TYPE>& operator+=(const CPool<TYPE>& Pool)
		{
			typename CPool<TYPE>::CConstantIterator Iterator;

			Iterator = Pool.GetIterator(0);

			while( CPool<TYPE>::IsValuable(Iterator) )
			{
				Add(*Iterator);

				++ Iterator;
			}

			return *this;
		}

		CPool<TYPE>& operator=(const CPool<TYPE>& Pool)
		{
			Reset();

			*this += Pool;

			return *this;
		}

		CPool<TYPE> operator+(const CPool<TYPE>& Pool)
		{
			CPool<TYPE> Result = *this;

			Result += Pool;

			return Result;
		}

		CConstantIterator GetIterator(UINT uFrom)const
		{
			CConstantIterator Iterator;

			__GetIterator(Iterator, uFrom);

			return Iterator;
		}

		CIterator GetIterator(UINT uFrom)
		{
			CIterator Iterator;

			__GetIterator(Iterator, uFrom);

			return Iterator;
		}

		void Reset()
		{
			LPARRAY pArray = m_pPoolList;

			while(pArray)
			{
				pArray->uFlag   = 0;
				pArray->uLength = 0;
				pArray = pArray->pNext;
			}

			m_uMemberCount = 0;
		}

		void Finish()
		{
			//if(!m_pPoolList)
			//	return;

			/*LPARRAY pArray;
			while(m_pPoolList && !m_pPoolList->uLength)
			{
				pArray  = m_pPoolList;
				m_pPoolList = m_pPoolList->pNext;

				ALCHEMY_DEBUG_DELETE(pArray);

				-- m_uArrayCount;
			}*/

			LPARRAY pArray;
			if(m_pPoolList)
			{
				UINT i = m_uArrayCount - 1;
				for(; i > 0; -- i)
				{
					pArray = m_ppPoolVector[i];
					if( pArray && (!pArray->uFlag || !pArray->uLength) )
					{
						ALCHEMY_DEBUG_DELETE(pArray);

						-- m_uArrayCount;
					}
					else
						break;
				}

				pArray = m_ppPoolVector[i];
				if(pArray)
					pArray->pNext = ALCHEMY_NULL;

				/*LPARRAY pPrevious = m_pPoolList;

				pArray = pPrevious->pNext;
				while(pArray)
				{
					if(!pArray->uLength)
					{
						pPrevious->pNext = pArray->pNext;

						ALCHEMY_DEBUG_DELETE(pArray);

						-- m_uArrayCount;

						pArray = pPrevious->pNext;
					}
					else
					{
						pPrevious = pArray;
						pArray    = pArray->pNext;
					}
				}*/
			}

			ALCHEMY_DEBUG_DELETE_ARRAY(m_ppPoolVector);
			ALCHEMY_DEBUG_NEW(m_ppPoolVector, LPARRAY[m_uArrayCount]);

			pArray = m_pPoolList;
			UINT uIndex = 0;
			while(pArray)
			{
				m_ppPoolVector[uIndex ++] = pArray;
				pArray = pArray->pNext;
			}
		}

		void Clear()
		{
			LPARRAY pArray = m_pPoolList;

			while(pArray)
			{
				m_pPoolList = pArray;
				pArray  = pArray->pNext;
				ALCHEMY_DEBUG_DELETE(m_pPoolList);
			}

			ALCHEMY_DEBUG_DELETE_ARRAY(m_ppPoolVector);

			m_uArrayCount  = 0;
			m_uMemberCount = 0;
		}

		template<typename EQUALFUNCTION>
		bool GetIndexOf(UINT& uOutput, const TYPE* pObject, UINT uCount, UINT uFrom = 0)
		{
			if(!uCount)
				return false;

			EQUALFUNCTION EqualFunction;

			CConstantIterator Iterator = GetIterator(uFrom), CurrentIterator;
			UINT uCurrentCount;

			const TYPE* pCurrentObject;

			while( IsValuable(Iterator) )
			{
				if( EqualFunction(*pObject, *Iterator) )
				{
					uCurrentCount = uCount;
					CurrentIterator = Iterator;
					pCurrentObject  = pObject;
					while( IsValuable(++ CurrentIterator) && -- uCurrentCount)
					{
						if( !EqualFunction(*(++ pCurrentObject), *CurrentIterator) )
							break;
					}

					if(!uCurrentCount)
						break;

					uOutput = Iterator.GetIndex();

					return true;
				}

				++ Iterator;
			}

			return false;
		}

		template<typename EQUALFUNCTION>
		bool GetIndexOf(UINT& uOutput, const TYPE& Object, UINT uFrom = 0)
		{
			EQUALFUNCTION EqualFunction;

			CConstantIterator Iterator = GetIterator(uFrom);

			while( IsValuable(Iterator) )
			{
				if( EqualFunction(Object, *Iterator) )
				{
					uOutput = Iterator.GetIndex();

					return true;
				}

				++ Iterator;
			}

			return false;
		}

		bool GetIndexOf(UINT& uOutput, const TYPE& Object, UINT uFrom = 0)
		{
			CConstantIterator Iterator = GetIterator(uFrom);

			while( IsValuable(Iterator) )
			{
				if( &Object == &(*Iterator) )
				{
					uOutput = Iterator.GetIndex();

					return true;
				}

				++ Iterator;
			}

			return false;
		}

		void AddArray(UINT uCount, bool bIsSet)
		{
			LPARRAY pArray;
			if(bIsSet)
			{
				for(UINT i = 0; i < uCount; ++ i)
				{
					pArray = _PushArray();

					if(pArray)
					{
						pArray->uLength = ARRAY_MEMBER_COUNT;
						pArray->uFlag   = ~0;

						m_uMemberCount += ARRAY_MEMBER_COUNT;
					}
					else
						ALCHEMY_DEBUG_WARNING("CPool::Add: Add value fail.");
				}
			}
			else
			{
				for(UINT i = 0; i < uCount; ++ i)
				{
					pArray = _PushArray();

					if(pArray)
					{
						pArray->uLength   = 0;
						pArray->uFlag     = 0;
					}
					else
						ALCHEMY_DEBUG_WARNING("CPool::Add: Add value fail.");
				}
			}
		}

		virtual UINT Add(const TYPE& Value)
		{
			/*if(!m_pPoolList)
			{
				ALCHEMY_DEBUG_NEW(m_pPoolList, ARRAY);

				m_pPoolList->uLength  = 1;
				m_pPoolList->uFlag    = 1;
				m_pPoolList->pNext    = ALCHEMY_NULL;
				m_pPoolList->Value[0] = Value;

				m_uMemberCount ++;

				return 0;
			}*/

			LPARRAY pArray = m_pPoolList, pPreviousArray;
			UINT uArrayCount = 0;
			UINT uFlag;
			UINT uIndex;
			while(pArray)
			{
				if( pArray->uLength < ARRAY_MEMBER_COUNT )
				{
					uFlag = pArray->uFlag;
					uIndex = 0;
					while(uFlag & 1)
					{
						uIndex ++;
						uFlag >>= 1;
					}

					ALCHEMY_SET_BIT(pArray->uFlag, uIndex);

					pArray->Value[uIndex] = Value;

					++ pArray->uLength;

					++ m_uMemberCount;

					return (uArrayCount << ARRAY_MEMBER_COUNT_SHIFT) + uIndex;
				}

				++ uArrayCount;

				pPreviousArray = pArray;
				pArray         = pArray->pNext;
			}

			/*if(m_pPoolList)
			{
				ALCHEMY_DEBUG_NEW(pPreviousArray->pNext, ARRAY);

				pArray = pPreviousArray->pNext;
			}
			else
			{
				ALCHEMY_DEBUG_NEW(m_pPoolList, ARRAY);

				pArray = m_pPoolList;
			}

			++ m_uArrayCount;*/
			pArray = _PushArray();

			if(pArray)
			{
				pArray->uLength   = 1;
				pArray->uFlag     = 1;
				//pArray->pNext     = ALCHEMY_NULL;
				pArray->Value[0]  = Value;

				++ m_uMemberCount;
			}
			else
				ALCHEMY_DEBUG_WARNING("CPool::Add: Add value fail.");

			/*LPARRAY* ppArrayVector, * ppOldArrayVector = m_ppPoolVector;

			ALCHEMY_DEBUG_NEW(ppArrayVector, LPARRAY[m_uArrayCount]);

			UINT i;
			for(i = 0; i < uArrayCount; ++ i)
				ppArrayVector[i] = ppOldArrayVector[i];

			ppArrayVector[i] = pArray;

			m_ppPoolVector = ppArrayVector;

			ALCHEMY_DEBUG_DELETE_ARRAY(ppOldArrayVector);*/

			return uArrayCount << ARRAY_MEMBER_COUNT_SHIFT;
		}

		virtual bool Remove(UINT uFrom, UINT uCount)
		{
			/*LPARRAY pArray = m_pPoolList, pTemp;

			if(uFrom < ARRAY_MEMBER_COUNT)
			{
				while(uCount --)
				{
					if( ALCHEMY_TEST_BIT(pArray->uFlag, uFrom) )
					{
						pArray->uLength --;

						ALCHEMY_CLEAR_BIT(pArray->uFlag, uFrom);

						m_uMemberCount --;
					}

					uFrom ++;

					if(uFrom >= ARRAY_MEMBER_COUNT)
						pArray = pArray->pNext;
				}

				return true;
			}

			UINT uIndex = __GetArrayByMemberIndex(&pArray, uFrom - ARRAY_MEMBER_COUNT);

			if( (pTemp = pArray) && (pArray = pArray->pNext) )
			{
				while(uCount --)
				{
					if( ALCHEMY_TEST_BIT(pArray->uFlag, uIndex) )
					{
						pArray->uLength --;

						ALCHEMY_CLEAR_BIT(pArray->uFlag, uIndex);

						m_uMemberCount --;

						if(!pArray->uLength)
						{
							if( uCount > ( uIndex = (ARRAY_MEMBER_COUNT - uIndex) ) )
							{
								uCount -= ARRAY_MEMBER_COUNT - uIndex;
								uIndex = 0;
								pTemp->pNext = pArray->pNext;

								ALCHEMY_DEBUG_DELETE(pArray);

								pArray       = pTemp->pNext;
							}
							else
								break;
						}
					}

					uIndex ++;

					if(uIndex >= ARRAY_MEMBER_COUNT)
					{
						uIndex = 0;
						pTemp  = pArray;
						pArray = pArray->pNext;
					}
				}

				return true;
			}

			return false;*/
			UINT uArrayIndex = uFrom >> ARRAY_MEMBER_COUNT_SHIFT;

			LPARRAY pArray;
			if(uArrayIndex < m_uArrayCount && (pArray = m_ppPoolVector[uArrayIndex]) != ALCHEMY_NULL)
			{
				uFrom = uFrom - (uArrayIndex << ARRAY_MEMBER_COUNT_SHIFT);
				UINT i;
				while(pArray && uCount)
				{
					for(i = uFrom; i < ARRAY_MEMBER_COUNT && uCount; ++ i)
					{
						if( ALCHEMY_TEST_BIT(pArray->uFlag, i) )
						{
							-- pArray->uLength;

							-- m_uMemberCount;

							ALCHEMY_CLEAR_BIT(pArray->uFlag, i);
						}

						-- uCount;
					}

					uFrom = 0;

					pArray = pArray->pNext;
				}

				return true;
			}

			return false;
		}

	protected:

		LPARRAY _PushArray()
		{
			LPARRAY pArray, pEndArray;
			ALCHEMY_DEBUG_NEW(pArray, ARRAY);

			if(m_pPoolList)
			{
				pEndArray = m_ppPoolVector[m_uArrayCount - 1];

				if(pEndArray)
					pEndArray->pNext = pArray;
				else
				{
					ALCHEMY_DEBUG_DELETE(pArray);

					ALCHEMY_DEBUG_WARNING("CPool::_PushArray: Push array fail.");

					return ALCHEMY_NULL;
				}
			}
			else
				m_pPoolList = pArray;

			pArray->uLength   = 0;
			pArray->uFlag     = 0;
			pArray->pNext     = ALCHEMY_NULL;

			UINT uOldArrayCount = m_uArrayCount ++;

			LPARRAY* ppArrayVector, * ppOldArrayVector = m_ppPoolVector;

			ALCHEMY_DEBUG_NEW(ppArrayVector, LPARRAY[m_uArrayCount]);

			for(UINT i = 0; i < uOldArrayCount; ++ i)
				ppArrayVector[i] = ppOldArrayVector[i];

			ppArrayVector[uOldArrayCount] = pArray;

			m_ppPoolVector = ppArrayVector;

			ALCHEMY_DEBUG_DELETE_ARRAY(ppOldArrayVector);

			return pArray;
		}

		LPARRAY m_pPoolList;
		LPARRAY* m_ppPoolVector;

		UINT m_uArrayCount;

		UINT m_uMemberCount;

	private:

		/*UINT __GetArrayByMemberIndex(LPARRAY* ppOutput, UINT uIndex)
		{
			LPARRAY pArray = m_pPoolList;

			if(pArray && !uIndex)
			{
				*ppOutput = pArray;

				return 0;
			}

			while(pArray && uIndex)
			{
				if(uIndex < ARRAY_MEMBER_COUNT)
				{
					*ppOutput = pArray;

					return uIndex;
				}

				uIndex -= ARRAY_MEMBER_COUNT;
				pArray  = pArray->pNext;
			}

			*ppOutput = ALCHEMY_NULL;

			return 0;
		}*/

		bool __GetIterator(CConstantIterator& Iterator, UINT uFrom)const
		{
			Iterator.m_uBaseIndex = uFrom >> ARRAY_MEMBER_COUNT_SHIFT;

			if(Iterator.m_uBaseIndex < m_uArrayCount && (Iterator.m_pArray = m_ppPoolVector[Iterator.m_uBaseIndex]) != ALCHEMY_NULL)
			{
				Iterator.m_uIndex = uFrom - (Iterator.m_uBaseIndex << ARRAY_MEMBER_COUNT_SHIFT);

				if(Iterator.m_pArray != ALCHEMY_NULL)
				{
					if( !ALCHEMY_TEST_BIT(Iterator.m_pArray->uFlag, Iterator.m_uIndex) )
						++ Iterator;

					return Iterator.m_pArray != ALCHEMY_NULL;
				}
			}

			return false;
		}
	};
}