#include "AlchemyD3D9VertexDeclaration.h"
#include "AlchemyD3D9Device.h"

#include <d3d9.h>

#pragma comment(lib,"d3d9.lib")

using namespace alchemy;

CD3D9VertexDeclaration::CD3D9VertexDeclaration(void) :
m_pD3D9VertexDeclaration(ALCHEMY_NULL)
{
}

CD3D9VertexDeclaration::~CD3D9VertexDeclaration(void)
{
}

bool CD3D9VertexDeclaration::_Create()
{
	if(m_pD3D9VertexDeclaration)
	{
		m_pD3D9VertexDeclaration->Release();

		m_pD3D9VertexDeclaration = ALCHEMY_NULL;
	}

	const CVertexDeclaration::VERTEXELEMENT* pVertexElements = GetVertexElements(), * pVertexElement;
	alchemy::UINT uVertexElementCount = GetVertexElementCount();

	LPD3DVERTEXELEMENT9 pD3D9VertexElements, pD3D9VertexElement;
	ALCHEMY_DEBUG_NEW(pD3D9VertexElements, D3DVERTEXELEMENT9[uVertexElementCount + 1]);

	static const BYTE s_VERTEX_ELEMENT_TYPE[] = 
	{
		D3DDECLTYPE_FLOAT1, 
		D3DDECLTYPE_FLOAT2, 
		D3DDECLTYPE_FLOAT3, 
		D3DDECLTYPE_FLOAT4
	};

	UINT uTypeIndex;
	for(alchemy::UINT i = 0; i < uVertexElementCount; ++ i)
	{
		pVertexElement = &pVertexElements[i];
		pD3D9VertexElement = &pD3D9VertexElements[pVertexElement->uIndex];

		uTypeIndex = pVertexElement->Type / sizeof(FLOAT);
		if(!uTypeIndex || uTypeIndex > 4)
		{
			ALCHEMY_DEBUG_DELETE_ARRAY(pD3D9VertexElements);

			ALCHEMY_DEBUG_WARNING("CD3D9VertexDeclaration::_Create: Error vertex element type");

			return false;
		}

		pD3D9VertexElement->Stream     = pVertexElement->uStream;
		pD3D9VertexElement->Offset     = pVertexElement->uOffset;
		pD3D9VertexElement->Type       = s_VERTEX_ELEMENT_TYPE[uTypeIndex - 1];
		pD3D9VertexElement->Method     = D3DDECLMETHOD_DEFAULT;

		if(pVertexElement->uIndex)
		{
			pD3D9VertexElement->Usage      = D3DDECLUSAGE_TEXCOORD;
			pD3D9VertexElement->UsageIndex = pVertexElement->uIndex - 1;
		}
		else
		{
			pD3D9VertexElement->Usage      = D3DDECLUSAGE_POSITION;
			pD3D9VertexElement->UsageIndex = pVertexElement->uIndex;
		}
	}

	D3DVERTEXELEMENT9 D3DDclEnd = D3DDECL_END();
	pD3D9VertexElements[uVertexElementCount] = D3DDclEnd;

	IDirect3DDevice9* pDevice = static_cast<CD3D9Device&>( GetGraphicsResourcePool() ).GetDevice();
	if(pDevice)
	{
		HRESULT hr = pDevice->CreateVertexDeclaration(pD3D9VertexElements, &m_pD3D9VertexDeclaration);

		ALCHEMY_DEBUG_DELETE_ARRAY(pD3D9VertexElements);

		return hr == D3D_OK;
	}

	ALCHEMY_DEBUG_DELETE_ARRAY(pD3D9VertexElements);

	return false;
}