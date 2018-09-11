#pragma once

#include "../Core/AlchemyVector.h"

namespace alchemy
{
	class CScriptMemery
	{
		typedef struct  
		{
			PVOID pData;
			UINT uSize;
			bool bIsHeap;
			const void* pSource;
		}MEMERYINFO, * LPMEMERYINFO;
	public:
		CScriptMemery(void);
		~CScriptMemery(void);

		UINT GetStackIndexCount()const;

		void* Get(UINT uHandle, UINT uOffset = 0);

		UINT Alloc(UINT uSize, bool bIsHeap, const void* pData = ALCHEMY_NULL);

		bool Free(UINT uHandle);
		bool FreeStack(UINT uFrom);

		void Destroy();
	private:
		CPool<MEMERYINFO> m_MemeryInfos;
		CVector<UINT> m_StackIndices;
		UINT m_uStackIndexCount;
		PVOID m_pStackMemery;
		UINT m_uStackMemeryTotalSize;
		UINT m_uStackMemeryCurrentSize;
	};

	ALCHEMY_INLINE UINT CScriptMemery::GetStackIndexCount()const
	{
		return m_uStackIndexCount;
	}
}