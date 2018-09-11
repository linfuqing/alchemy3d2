#pragma once

#include "AlchemyInterface.h"
#include "AlchemyNull.h"

namespace alchemy
{
	template<typename T>
	class CSmartPointer
	{
	public:
		CSmartPointer(void) : m_pPointer(ALCHEMY_NULL) {}
		CSmartPointer(T* pPointer) : m_pPointer( static_cast<IInterface*>(pPointer) ) {}
		CSmartPointer(const CNull& Null) : m_pPointer(ALCHEMY_NULL) {}
		~CSmartPointer(void)
		{
			if(m_pPointer)
				m_pPointer->Release();
		}

		ALCHEMY_INLINE T& operator*()
		{
			return static_cast<T&>(*m_pPointer);
		}

		ALCHEMY_INLINE T* operator->()
		{
			return &**this;
		}

		ALCHEMY_INLINE const T& operator*()const
		{
			return static_cast<T&>(*m_pPointer);
		}

		ALCHEMY_INLINE const T* operator->()const
		{
			return &**this;
		}

		ALCHEMY_INLINE bool operator==(T* pPointer)const
		{
			return m_pPointer == static_cast<IInterface*>(pPointer);
		}

		ALCHEMY_INLINE bool operator==(const CSmartPointer& Pointer)const
		{
			return m_pPointer == Pointer.m_pPointer;
		}

		ALCHEMY_INLINE bool operator==(const CNull& Null)const
		{
			return !m_pPointer;
		}

		CSmartPointer& operator=(T* pPointer)
		{
			if(m_pPointer)
				m_pPointer->Release();

			m_pPointer = static_cast<IInterface*>(pPointer);

			return *this;
		}

		CSmartPointer& operator=(const CSmartPointer& Pointer)
		{
			if(m_pPointer)
				m_pPointer->Release();

			m_pPointer = Pointer.m_pPointer;

			if(m_pPointer)
				m_pPointer->AddRef();

			return *this;
		}

		CSmartPointer& operator=(const CNull& Null)
		{
			if(m_pPointer)
			{
				m_pPointer->Release();

				m_pPointer = ALCHEMY_NULL;
			}

			return *this;
		}

	private:
		IInterface* m_pPointer;
	};
}