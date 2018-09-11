#pragma once

#include "AlchemyPool.h"

namespace alchemy
{
	template<typename TYPE/*, UINT SHIFT = ALCHEMY_CONSTANT_SHIFT( ALCHEMY_TOTAL_BITS(UINT) )*/>
	class CVector :
		public CPool<TYPE>
	{
		typedef CPool<TYPE> POOL;
	public:

		CVector(void)
		{
		}

		~CVector(void)
		{
		}

		UINT Add(const TYPE& Value)
		{
			UINT uArrayIndex = POOL::m_uMemberCount >> POOL::ARRAY_MEMBER_COUNT_SHIFT;
			typename POOL::LPARRAY pArray = (POOL::m_ppPoolVector && POOL::m_uArrayCount > uArrayIndex) ? POOL::m_ppPoolVector[uArrayIndex] : POOL::_PushArray();
			if(pArray)
			{
				if(pArray->uLength >= POOL::ARRAY_MEMBER_COUNT)
				{
					pArray = POOL::_PushArray();

					if(pArray)
					{
						pArray->uFlag    = 1;
						pArray->uLength  = 1;
						pArray->Value[0] = Value;

						++ POOL::m_uMemberCount;
					}
					else
						ALCHEMY_DEBUG_WARNING("CVector::Add: Add value fail.");
				}
				else
				{
					UINT uIndex = pArray->uLength ++;
					pArray->Value[uIndex] = Value;
					ALCHEMY_SET_BIT(pArray->uFlag, uIndex);
					++ POOL::m_uMemberCount;
				}
			}
			else
				ALCHEMY_DEBUG_WARNING("CVector::Add: Add value fail.");

			return POOL::m_uMemberCount - 1;//POOL::m_uArrayCount ? ( (POOL::m_uArrayCount - 1) << POOL::ARRAY_MEMBER_COUNT_SHIFT ) : 1;
		}

		bool Remove(UINT uFrom, UINT uCount)
		{
			UINT uArrayIndex = uFrom >> POOL::ARRAY_MEMBER_COUNT_SHIFT;

			typename POOL::LPARRAY pArray;
			if(uArrayIndex < POOL::m_uArrayCount && (pArray = POOL::m_ppPoolVector[uArrayIndex]) != ALCHEMY_NULL)
			{
				uFrom = uFrom - (uArrayIndex << POOL::ARRAY_MEMBER_COUNT_SHIFT);
				UINT i, uBasicMemeberIndex;
				TYPE* pTemp = ALCHEMY_NULL;
				while(pArray)
				{
					uBasicMemeberIndex = (uArrayIndex << POOL::ARRAY_MEMBER_COUNT_SHIFT) + uCount;
					for(i = uFrom; i < POOL::ARRAY_MEMBER_COUNT; ++ i)
					{
						pTemp = POOL::Get(uBasicMemeberIndex + i);

						if(pTemp)
							pArray->Value[i] = *pTemp;
						else
						{
							POOL::m_uMemberCount -= pArray->uLength;

							pArray->uFlag = (1 << i) - 1;

							pArray->uLength = i;

							POOL::m_uMemberCount += pArray->uLength;

							break;
						}
					}

					if(!pTemp)
					{
						pArray = pArray->pNext;
						while(pArray)
						{
							POOL::m_uMemberCount -= pArray->uLength;

							pArray->uLength = 0;
							pArray->uFlag   = 0;

							pArray = pArray->pNext;
						}

						break;
					}

					uFrom = 0;

					++ uArrayIndex;

					pArray = pArray->pNext;
				}

				return true;
			}

			return false;
		}

		template<typename LESSFUNCTION>
		void Sort()
		{
			LESSFUNCTION LessFunction;
			TYPE Temp;
			UINT i, j, uNextStage, uStage = 1;

			while ( (uNextStage = uStage * 3 + 1) < POOL::m_uMemberCount ) 
				uStage = uNextStage;

			while(uStage > 0) 
			{
				for(i = uStage - 1; i < POOL::m_uMemberCount; i ++) 
				{
					Temp = *POOL::Get(i);

					for(j = i; (j >= uStage) && LessFunction( Temp, *POOL::Get(uNextStage = j - uStage) ); j -= uStage) 
						POOL::Set(*POOL::Get(uNextStage), j);

					POOL::Set(Temp, j);
				}

				uStage = uStage / 3;
			}
		}
	};
}