#pragma once

//#include "AlchemyPool.h"
#include "AlchemyAllocatorInterface.h"
#include "AlchemyVector.h"
#include "AlchemySmartClass.h"

namespace alchemy
{
	/*template< typename T >
	class CBasicAllocator
	{
	public:
		CBasicAllocator()
		{

		}

		virtual ~CBasicAllocator()
		{

		}

		T* Allocate()
		{
			T* pPointer;
			ALCHEMY_DEBUG_NEW(pPointer, T);
			return pPointer;
		}

		void Dellocate( T* pPointer )
		{
			ALCHEMY_DEBUG_DELETE(pPointer);
		}
	};*/

	template<typename T>
	class CPoolAllocator : 
	public CSmartClass<IAllocator>
	{
	public:
		CPoolAllocator()
		//:m_nCurrentUsed(0)
		{

		}

		virtual ~CPoolAllocator()
		{
			//UINT nLen = m_Pools.GetLength();
			//while (nLen--)
			//	ALCHEMY_DEBUG_DELETE( m_Pools[nLen] )
		}

		void* Allocate()
		{
			/*T* pPointer;
			if(m_nCurrentUsed == 0)
			{
				ALCHEMY_DEBUG_NEW(pPointer,T);
			}
			else
				pPointer = m_Pools[--m_nCurrentUsed];

			return pPointer;*/
			
			UINT uFreePoolLength = m_FreePool.GetLength();
			if(uFreePoolLength)
			{
				-- uFreePoolLength;
				
				T** ppPointer = m_FreePool.Get(uFreePoolLength);
				m_FreePool.Remove(uFreePoolLength, 1);
				
				return *ppPointer;
			}
			
			static T s_Buffer;
			
			return m_MallocPool.Get( m_MallocPool.Add(s_Buffer) );
		}

		void Dellocate(void* pPointer)
		{
			/*if( m_nCurrentUsed < m_Pools.GetLength() )
				m_Pools[m_nCurrentUsed++] = pPointer;
			else
			{
				++m_nCurrentUsed;
				m_Pools.Add(pPointer);
			}*/
			
			if(pPointer)
				m_FreePool.Add( static_cast<T*>(pPointer) );
		}

	private:
		//UINT			m_nCurrentUsed;
		CPool<T>  m_MallocPool;
		CVector<T*> m_FreePool;
	};
}