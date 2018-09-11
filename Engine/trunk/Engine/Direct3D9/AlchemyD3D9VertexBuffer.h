#pragma once

#include "../Graphics/AlchemyGraphicsBuffer.h"

struct IDirect3DVertexBuffer9;

namespace alchemy
{
	class CD3D9Device;
	class CD3D9VertexBuffer : 
		public VERTEXBUFFER
	{
		friend class CD3D9Device;
	public:
		CD3D9VertexBuffer(void);
		~CD3D9VertexBuffer(void);

		IDirect3DVertexBuffer9* GetInstance()const;

		void* Lock(UINT uFlag);
		void* Lock(UINT uStart, UINT uEnd, UINT uFlag);

		bool Unlock();

		void Destroy();

	protected:
		bool _Create();

	private:
		IDirect3DVertexBuffer9* m_pD3D9VertexBuffer;
		PVOID m_pData;
		void* m_pSourceData;
	};

	ALCHEMY_INLINE IDirect3DVertexBuffer9* CD3D9VertexBuffer::GetInstance()const
	{
		return m_pD3D9VertexBuffer;
	}
}