#pragma once

#include "../Graphics/AlchemyGraphicsBuffer.h"

struct IDirect3DBaseTexture9;

namespace alchemy
{
	class CD3D9Device;
	class CD3D9Texture : 
		public TEXTURE
	{
		friend class CD3D9Device;
	public:
		CD3D9Texture(void);
		~CD3D9Texture(void);

		IDirect3DBaseTexture9* GetInstance()const;

		void* Lock(UINT uSurface, UINT uFlag, PUINT puRowPitch, PUINT puSlicePitch);

		bool  Unlock();

		void Destroy();

	protected:
		bool _Create();

	private:
		UINT __ComputeDataOffset(UINT uSurface);

		IDirect3DBaseTexture9* m_pD3D9Texture;

		PVOID m_pData;

		void* m_pLockData;
		UINT m_uLockSurface;
		UINT m_uLockFlag;

		UINT m_uCubeMapSize;
	};

	ALCHEMY_INLINE IDirect3DBaseTexture9* CD3D9Texture::GetInstance()const
	{
		return m_pD3D9Texture;
	}
}