#pragma once

#include "AlchemyFlashGraphicsBuffer.h"
#include "AlchemyFlashObjectInterface.h"

namespace alchemy
{
	class CFlashContext3D;
	class CFlashVertexBuffer :
		public CFlashGraphicsBuffer<CGraphicsBuffer<CVertexBuffer>::CLocker>, public CFlashObjectInterface
	{
		friend class CFlashContext3D;
	public:
		CFlashVertexBuffer(void);
		~CFlashVertexBuffer(void);

		void Reset();

		void* Lock(UINT uFlag);
		void* Lock(UINT uStart, UINT uEnd, UINT uFlag);

		bool Unlock();

		void Destroy();

#ifdef AS3_H
		AS3TYPE Init();
#endif

	protected:
		bool _Create();

	private:
		PVOID m_pData;

		UINT m_uStartDirty;
		UINT m_uEndDirty;
	};

	ALCHEMY_INLINE void CFlashVertexBuffer::Reset()
	{
		if(IsNoData( GetUsage() ) )
			ALCHEMY_DEBUG_DELETE_ARRAY(m_pData);

		m_uStartDirty = GetMemberCount();
		m_uEndDirty   = 0;

		CFlashGraphicsBuffer<CGraphicsBuffer<CVertexBuffer>::CLocker>::Reset();
	}
}