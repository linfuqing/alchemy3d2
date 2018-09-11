#pragma once

#include "AlchemyHLSLCompiler.h"

#include "../Graphics/AlchemyRenderSystem.h"

#include <Windows.h>

struct IDirect3D9;
struct IDirect3DDevice9;
struct IDirect3DSurface9;
namespace alchemy
{
	class CD3D9Device : 
		public CRenderSystem
	{
	public:
		CD3D9Device(HWND hWindow, bool bIsWindowed, UINT uBackBufferWidth, UINT uBackBufferHeight, INT nAntiAliasing);
		~CD3D9Device(void);

		IDirect3DDevice9* GetDevice()const;

		bool CheckFormat(FORMATTYPE Type, UINT uFormat);

		ICompiler& GetCompiler();

		bool GetViewport(PUINT puWidth, PUINT puHeight);

		LPTEXTURE CreateTextureFromFile(const CHAR* pcPath);

		bool Clear(ARGBCOLOR Color, UINT uFlag);
		bool ResetTexture();

		bool RegisterRenderState();
		bool RestoreRenderState();

		bool SetRenderState(const RENDERSTATEINFO* pRenderStateInfos, UINT uRenderStateInfoCount, bool bIsRegister);

		bool SetRenderTarget(CGraphicsResource::HANDLE Texture, UINT uSurface, UINT uIndex);

		bool SetVertexDeclaration(CGraphicsResource::HANDLE VertexDeclaration);
		bool SetStreamSource(CGraphicsResource::HANDLE VertexBuffer, UINT uStream);
		bool SetIndices(CGraphicsResource::HANDLE IndexBuffer);
		bool SetShader(CGraphicsResource::HANDLE Shader);

		bool SetShaderParameters(EFFECTTYPE Type, UINT uIndex, const FLOAT* pfValues, UINT uCount);

		bool SetShaderMatrixArray(EFFECTTYPE Type, UINT uIndex, const FLOAT* pfValues, UINT uRow, UINT uColumn, UINT uRowOffset, UINT uOffset, UINT uCount);
		bool SetShaderMatrixPointerArray(EFFECTTYPE Type, UINT uIndex, const PFLOAT* ppfValue, UINT uRow, UINT uColumn, UINT uRowOffset, UINT uCount);

		bool SetShaderConstants(IEffect::EFFECTTYPE Type, CGraphicsResource::HANDLE ConstantBuffer, UINT uIndex);

		bool SetTexture(EFFECTTYPE Type, CGraphicsResource::HANDLE Texture, UINT uIndex, const SAMPLERDECLARATION* pSamplerDeclaration);

		bool DrawIndexedTriangles(TRIANGLETYPE Type, UINT uStartIndex, UINT uTriangleCount);

		bool Present();

		void Destroy();

	protected:
		CGraphicsResource* _Create(CGraphicsResource::TYPE Type);

	private:
		bool __SetRenderState(RENDERSTATE State, UINT uValue);

		UINT m_ViewportWidth;
		UINT m_ViewportHeight;

		IDirect3D9* m_pDirect;
		IDirect3DDevice9* m_pDevice;
		IDirect3DSurface9* m_pBackgroundSurface;

		CHLSLCompiler* m_pCompiler;

		UINT m_uRenderState[TOTAL_RENDER_STATE];
		RENDERSTATE m_RenderStateStack[TOTAL_RENDER_STATE];
		UINT m_RenderStateValueStack[TOTAL_RENDER_STATE];
		UINT m_uRenderStateStackCount;
		UINT m_uRenderStateFlag;

		UINT m_uNumVertex;

		bool m_bIsBeginScene;
	};

	ALCHEMY_INLINE IDirect3DDevice9* CD3D9Device::GetDevice()const
	{
		return m_pDevice;
	}
}