#pragma once

#include "../Graphics/AlchemyGraphicsBuffer.h"

struct IDirect3DIndexBuffer9;

namespace alchemy
{
	class CD3D9Device;
	class CD3D9IndexBuffer : 
		public INDEXBUFFER
	{
		friend class CD3D9Device;
	public:
		CD3D9IndexBuffer(void);
		~CD3D9IndexBuffer(void);

		IDirect3DIndexBuffer9* GetInstance()const;

		void* Lock(UINT uFlag);
		void* Lock(UINT uStart, UINT uEnd, UINT uFlag);

		bool Unlock();

		void Destroy();

	protected:
		bool _Create();

	private:
		IDirect3DIndexBuffer9* m_pD3D9IndexBuffer;
		PVOID m_pData;
		void* m_pSourceData;
	};

	ALCHEMY_INLINE IDirect3DIndexBuffer9* CD3D9IndexBuffer::GetInstance()const
	{
		return m_pD3D9IndexBuffer;
	}
}