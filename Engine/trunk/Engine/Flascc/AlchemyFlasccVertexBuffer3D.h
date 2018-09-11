#pragma once

#include "../Graphics/AlchemyGraphicsBuffer.h"

namespace alchemy
{
	class CFlasccContext3D;
	class CFlasccVertexBuffer3D : 
		public VERTEXBUFFER
	{
		friend class CFlasccContext3D;
	public:
		CFlasccVertexBuffer3D(void);
		~CFlasccVertexBuffer3D(void);

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