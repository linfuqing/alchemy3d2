#include "AlchemyD3D9Shader.h"

#include "AlchemyD3D9Device.h"

#include <d3d9.h>
#include <d3dx9.h>

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")

using namespace alchemy;

CD3D9Shader::CD3D9Shader(void)
{
	m_Shader.pVertexShader = ALCHEMY_NULL;
	m_Shader.pPixelShader = ALCHEMY_NULL;
}

CD3D9Shader::~CD3D9Shader(void)
{
}

void CD3D9Shader::Destroy()
{
	IEffect::EFFECTTYPE Type = GetShaderType();
	if(Type == IEffect::VERTEX_SHADER)
	{
		if(m_Shader.pVertexShader)
			m_Shader.pVertexShader->Release();
	}
	else if(Type == IEffect::FRAGMENT_SHADER)
	{
		if(m_Shader.pPixelShader)
			m_Shader.pPixelShader->Release();
	}

	m_Shader.pVertexShader = ALCHEMY_NULL;
	m_Shader.pPixelShader = ALCHEMY_NULL;
}

bool CD3D9Shader::_Create()
{
	LPD3DXBUFFER pBuffer = ( m_pCode && m_pCode->GetBufferPointer() ) ? *reinterpret_cast<LPD3DXBUFFER*>( m_pCode->GetBufferPointer() ) : ALCHEMY_NULL;
	if(pBuffer)
	{
		LPD3DXBUFFER pAdditionalBuffer = ( m_pAdditionalCode && m_pAdditionalCode->GetBufferPointer() ) ? *reinterpret_cast<LPD3DXBUFFER*>( m_pAdditionalCode->GetBufferPointer() ) : ALCHEMY_NULL;

		IEffect::EFFECTTYPE Type = GetShaderType();
		if(Type == IEffect::VERTEX_SHADER)
		{
			IDirect3DDevice9* pDevice = static_cast<CD3D9Device&>( GetGraphicsResourcePool() ).GetDevice();
			if(pDevice && pDevice->CreateVertexShader( reinterpret_cast<const DWORD*>( pBuffer->GetBufferPointer() ), &m_Shader.pVertexShader) == D3D_OK)
			{
				pBuffer->Release();

				m_pCode->Release();

				m_pCode = ALCHEMY_NULL;

				return true;
			}
		}
		else if(Type == IEffect::FRAGMENT_SHADER)
		{
			IDirect3DDevice9* pDevice = static_cast<CD3D9Device&>( GetGraphicsResourcePool() ).GetDevice();
			if(pDevice && pDevice->CreatePixelShader( reinterpret_cast<const DWORD*>( pBuffer->GetBufferPointer() ), &m_Shader.pPixelShader) == D3D_OK)
			{
				pBuffer->Release();

				m_pCode->Release();

				m_pCode = ALCHEMY_NULL;

				return true;
			}
		}
		else if(pAdditionalBuffer)
		{
			IDirect3DDevice9* pDevice = static_cast<CD3D9Device&>( GetGraphicsResourcePool() ).GetDevice();

			if(pDevice && pDevice->CreateVertexShader( reinterpret_cast<const DWORD*>( pBuffer->GetBufferPointer() ), &m_Shader.pVertexShader) == D3D_OK)
			{
				pBuffer->Release();

				m_pCode->Release();

				m_pCode = ALCHEMY_NULL;

				return true;
			}

			if(pDevice && pDevice->CreatePixelShader( reinterpret_cast<const DWORD*>( pAdditionalBuffer->GetBufferPointer() ), &m_Shader.pPixelShader) == D3D_OK)
			{
				pAdditionalBuffer->Release();

				m_pAdditionalCode->Release();

				m_pAdditionalCode = ALCHEMY_NULL;

				return true;
			}
		}
	}

	if(m_pCode)
	{
		m_pCode->Release();

		m_pCode = ALCHEMY_NULL;
	}

	if(m_pAdditionalCode)
	{
		m_pAdditionalCode->Release();

		m_pAdditionalCode = ALCHEMY_NULL;
	}

	return false;
}