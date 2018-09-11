#pragma once

#include "AlchemyPool.h"
#include "AlchemyInterface.h"

namespace alchemy
{
	template<typename T>
	class CSmartClass : 
		public T
	{
		class CPointerPool :
			public CPool<CSmartClass*>
		{
		public:
			CPointerPool()
			{
			}

			~CPointerPool()
			{
				typename CPool<CSmartClass*>::CIterator Iterator = CPool<CSmartClass*>::GetIterator(0);

				while( CPool<CSmartClass*>::IsValuable(Iterator) )
				{
					delete *Iterator;

					++ Iterator;
				}
			}
		};
	public:
		CSmartClass(bool bIsWeakKey = false) :
		m_uIndex(0),
			m_bIsWeakKey(bIsWeakKey),
			m_bIsHeapOperation( const_cast<bool&>(sm_bIsHeapOperation) )
		{
			if(!bIsWeakKey)
				m_uIndex = sm_PointerPool.Add(this);

			sm_bIsHeapOperation = false;
		}

		virtual ~CSmartClass(void)
		{
			if(!m_bIsWeakKey)
			{
				CSmartClass** ppPointer = sm_PointerPool.Get(m_uIndex);

				if(ppPointer && *ppPointer == this)
					sm_PointerPool.Remove(m_uIndex, 1);
			}

			if(m_bIsHeapOperation)
			{
				while(sm_bIsHeapOperation);

				sm_bIsHeapOperation = true;
			}
		}

		ALCHEMY_INLINE bool IsWeakKey()const
		{
			return m_bIsWeakKey;
		}

		static void* operator new(size_t uSize)
		{
			while(sm_bIsHeapOperation);

			sm_bIsHeapOperation = true;

			return ALCHEMY_NEW(uSize);
		}

		static void operator delete(void* pUserData)
		{
			if(sm_bIsHeapOperation)
			{
				ALCHEMY_DELETE(pUserData);

				sm_bIsHeapOperation = false;
			}
		}

		ALCHEMY_IMPLEMENT_INTERFACE

	private:
		static volatile bool sm_bIsHeapOperation;

		static CPointerPool sm_PointerPool;

		UINT m_uIndex;

		bool m_bIsWeakKey;

		bool m_bIsHeapOperation;
	};

	template<typename T>
	typename CSmartClass<T>::CPointerPool CSmartClass<T>::sm_PointerPool;

	template<typename T>
	volatile bool CSmartClass<T>::sm_bIsHeapOperation = false;

	typedef CSmartClass<IInterface> SMARTCLASS;
}