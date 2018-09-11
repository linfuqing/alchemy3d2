#pragma once

#include "AlchemyDataType.h"

namespace alchemy
{
	class CResource
	{
	public:

		CResource(void);
		virtual ~CResource(void);

		UINT AddRef();
		UINT Release();

	protected:
		static void* operator new(size_t uSize);
		static void operator delete(void* pUserData);

		//friend void* operator new(UINT uSize);

		virtual void _Destroy();

		UINT m_uReferenceCount;
	private:
		static volatile void* sm_pPointer;

		void* m_pPointer;
	};

	ALCHEMY_INLINE UINT CResource::AddRef()
	{
		return ++ m_uReferenceCount;
	}

	ALCHEMY_INLINE UINT CResource::Release()
	{
		if(m_uReferenceCount)
			-- m_uReferenceCount;

		if(!m_uReferenceCount)
		{
			_Destroy();

			return 0;
		}

		return m_uReferenceCount;
	}

	ALCHEMY_INLINE void CResource::_Destroy() 
	{
		ALCHEMY_DELETE(m_pPointer);
	}

	ALCHEMY_INLINE void CResource::operator delete(void* pUserData)
	{
		ALCHEMY_DELETE(pUserData);
	}
}