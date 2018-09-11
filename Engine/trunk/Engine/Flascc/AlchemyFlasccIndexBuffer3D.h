#pragma once

#include "../Graphics/AlchemyGraphicsBuffer.h"

namespace alchemy
{
	class CFlasccContext3D;
	class CFlasccIndexBuffer3D : 
		public INDEXBUFFER
	{
		friend class CFlasccContext3D;
	public:
		CFlasccIndexBuffer3D(void);
		~CFlasccIndexBuffer3D(void);

		void* Lock(UINT uFlag);
		void* Lock(UINT uStart, UINT uEnd, UINT uFlag);

		bool Unlock();

		void Destroy();

	protected:
		bool _Create();

	private:
		PVOID m_pData;
		UINT m_uDataSize;

		UINT m_uLockStart;
		UINT m_uLockCount;
		UINT m_uLockFlag;
	};
}