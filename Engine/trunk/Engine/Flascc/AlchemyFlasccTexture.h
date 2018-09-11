#pragma once

#include "../Graphics/AlchemyGraphicsBuffer.h"

namespace alchemy
{
	class CFlasccTexture :
		public TEXTURE
	{
	public:
		CFlasccTexture(void);
		~CFlasccTexture(void);

		void* Lock(UINT uSurface, UINT uFlag, PUINT puRowPitch, PUINT puSlicePitch);

		bool  Unlock();

		void Destroy();

	protected:
		bool _Create();

	private:
		PVOID m_pData;
		UINT m_uLockSurface;
		UINT m_uLockFlag;
		UINT m_uLockOffset;
	};
}