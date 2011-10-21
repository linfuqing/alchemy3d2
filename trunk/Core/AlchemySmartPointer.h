#pragma once

#include "AlchemyReferenceCounter.h"
#include "AlchemyPool.h"

namespace alchemy
{
	template<typename T>
	class CSmartPointer :
		public T, public CReferenceCounter
	{
		class CPointerPool :
			public CPool<CSmartPointer*>
		{
		public:
			CPointerPool()
			{
			}

			~CPointerPool()
			{
				typename CPool<CSmartPointer*>::CIterator Iterator = CPool<CSmartPointer*>::GetBeginIterator();

				while( CPool<CSmartPointer*>::IsValuable(Iterator) )
				{
					delete *Iterator;

					++ Iterator;
				}
			}
		};
	public:
		class CPointer
		{
		public:
			CPointer(CSmartPointer* pPointer) : m_pSmartPointer(pPointer) {}
			~CPointer()
			{
				if(m_pSmartPointer)
					m_pSmartPointer->Release();
			}

			ALCHEMY_INLINE CSmartPointer& operator*()
			{
				return *m_pSmartPointer;
			}

			CPointer& operator=(CSmartPointer* pPointer)
			{
				if(m_pSmartPointer)
					m_pSmartPointer->Release();

				if(pPointer)
					pPointer->AddRef();

				m_pSmartPointer = pPointer;

				return *this;
			}

			CPointer& operator=(CPointer& Pointer)
			{
				return *this = Pointer.m_pSmartPointer;
			}

		private:
			CSmartPointer* m_pSmartPointer;
		};

		static CSmartPointer& Create()
		{
			CSmartPointer* pPointer;
			sm_bIsPointerCanBeCreated = true;
			ALCHEMY_DEBUG_NEW(pPointer, CSmartPointer);
			sm_bIsPointerCanBeCreated = false;

			return *pPointer;
		}

		CSmartPointer(void) :
		  m_uIndex(0)
		{
			ALCHEMY_DEBUG_ASSERT(sm_bIsPointerCanBeCreated, "CSmartPointer::CSmartPointer: Smart pointer must be created by function \"static T& CSmartPointer::Create()\".");

			m_uIndex = sm_PointerPool.Add(this);
		}

		~CSmartPointer(void)
		{
			__Destroy(false);
		}

		INT AddRef()
		{
			return CReferenceCounter::AddRef();
		}

		INT Release()
		{
			INT nReferenceCount = CReferenceCounter::Release();
			if(nReferenceCount <= 0)
				__Destroy(true);

			return nReferenceCount;
		}
	private:

		void __Destroy(bool bIsDelete)
		{
			CSmartPointer** ppPointer = sm_PointerPool.Get(m_uIndex);

			if(ppPointer && *ppPointer == this)
			{
				sm_PointerPool.Remove(m_uIndex, 1);

				if(bIsDelete)
					delete this;
			}
		}

		static bool sm_bIsPointerCanBeCreated;

		static CPointerPool sm_PointerPool;

		UINT m_uIndex;
	};

	template<typename T>
	typename CSmartPointer<T>::CPointerPool CSmartPointer<T>::sm_PointerPool;

	template<typename T>
	bool CSmartPointer<T>::sm_bIsPointerCanBeCreated = false;
}