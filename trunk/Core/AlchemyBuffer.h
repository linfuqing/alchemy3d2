#pragma once

#include "AlchemyReferenceCounterInterface.h"
#include "AlchemySmartPointer.h"

namespace alchemy
{
	class CBuffer;
	CBuffer& CreateBuffer(UINT uSize);

	class CBuffer :
		public CReferenceCounterInterface
	{
		friend CBuffer& CreateBuffer(UINT uSize);
	public:
		CBuffer();
		~CBuffer();

		PVOID GetBufferPointer();
		const VOID* GetBufferPointer()const;
		UINT GetBufferSize()const;

	private:
		PVOID m_pBuffer;
		UINT m_uSize;
	};

	typedef CSmartPointer<CBuffer> BUFFER, * LPBUFFER;

	ALCHEMY_INLINE PVOID CBuffer::GetBufferPointer()
	{
		return m_pBuffer;
	}

	ALCHEMY_INLINE const VOID* CBuffer::GetBufferPointer()const
	{
		return m_pBuffer;
	}

	ALCHEMY_INLINE UINT CBuffer::GetBufferSize()const
	{
		return m_uSize;
	}
}