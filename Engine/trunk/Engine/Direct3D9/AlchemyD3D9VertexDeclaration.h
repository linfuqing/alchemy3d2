#pragma once

#include "../Graphics/AlchemyVertexDeclaration.h"

struct IDirect3DVertexDeclaration9;

namespace alchemy
{
	class CD3D9Device;
	class CD3D9VertexDeclaration : 
		public CVertexDeclaration
	{
		friend class CD3D9Device;
	public:
		CD3D9VertexDeclaration(void);
		~CD3D9VertexDeclaration(void);

		IDirect3DVertexDeclaration9* GetInstance()const;

	protected:
		bool _Create();

	private:
		IDirect3DVertexDeclaration9* m_pD3D9VertexDeclaration;
	};

	ALCHEMY_INLINE IDirect3DVertexDeclaration9* CD3D9VertexDeclaration::GetInstance()const
	{
		return m_pD3D9VertexDeclaration;
	}
}