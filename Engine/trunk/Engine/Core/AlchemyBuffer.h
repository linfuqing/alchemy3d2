#pragma once

#include "AlchemySmartClass.h"

namespace alchemy
{
	class CBuffer;
	CBuffer& CreateBuffer(UINT uSize = 0, bool bIsWeakKey = false);

	class CBuffer :
		public IInterface
	{
		friend CBuffer& CreateBuffer(UINT uSize, bool bIsWeakKey);
	public:
		CBuffer();
		virtual ~CBuffer();

		void* GetBufferPointer();
		const void* GetBufferPointer()const;
		UINT GetBufferSize()const;

		bool CopyFrom(const void* pBuffer, UINT uFrom = 0, UINT uSize = 0);

	private:
		PVOID m_pBuffer;
		UINT m_uSize;
	};

	typedef CSmartClass<CBuffer> BUFFER, * LPBUFFER;

	ALCHEMY_INLINE void* CBuffer::GetBufferPointer()
	{
		return m_pBuffer;
	}

	ALCHEMY_INLINE const void* CBuffer::GetBufferPointer()const
	{
		return m_pBuffer;
	}

	ALCHEMY_INLINE UINT CBuffer::GetBufferSize()const
	{
		return m_uSize;
	}
}