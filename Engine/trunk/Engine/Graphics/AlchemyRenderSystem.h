#pragma once

#include "AlchemyGraphicsResourcePool.h"
#include "AlchemyMeshManager.h"
#include "AlchemyRenderMethod.h"
#include "AlchemyEffectCode.h"
#include "AlchemyRenderStateInterface.h"

#include "../Core/AlchemyMemory.h"

namespace alchemy
{
	class CRenderSystem :
		public CGraphicsResourcePool, public IEffect, public IRenderState
	{
	public:
		typedef CIndexBuffer::TRIANGLETYPE TRIANGLETYPE;

		ALCHEMY_BEGIN_EVENT_ENUM
			RESET_VIEWPORT
		ALCHEMY_END_EVENT_ENUM

		typedef enum
		{
			STENCIL = 0,
			TARGET,
			ZBUFFER
		}CLEARTYPE;

		typedef enum
		{
			TEXTURE = 0,
			DEPTH_STENCIL,
			INDEX_BUFFER,
		}FORMATTYPE;

		typedef struct
		{
			UINT uRenderTargetCount;
			UINT uMaxVertexCount;
			UINT uMaxVertexIndexCount;
		}RENDERSYSTEMINFO;

		class CRenderTargetViewListener :
			public CSmartClass<IEventDispatcher::IListener>
		{
		public:
			CRenderTargetViewListener(CGraphicsBuffer<CTexture>& Parent, INT nShift);
			~CRenderTargetViewListener();

			INT GetShift()const;
			void Handler(const IEventDispatcher::CEvent& Event);

		private:
			CGraphicsBuffer<CTexture>& m_Parent;
			INT m_nShift;
		};

		CRenderSystem(void);
		~CRenderSystem(void);

		const RENDERSYSTEMINFO& GetRenderSystemInfo()const;

		CVertexDeclaration* CreateVertexDeclaration(const CVertexDeclaration::VERTEXELEMENT* pVertexElements = ALCHEMY_NULL, UINT uElementCount = 0);

		LPVERTEXBUFFER CreateVertexBuffer(UINT uCount, UINT uStride, UINT uUsage, const void* pData = ALCHEMY_NULL);
		LPINDEXBUFFER CreateIndexBuffer(UINT uCount, UINT uUsage, CIndexBuffer::FORMAT Format, const void* pData = ALCHEMY_NULL);
		LPCONSTANTBUFFER CreateConstantBuffer(UINT uByteSize, UINT uUsage, const void* pData = ALCHEMY_NULL);

		CShader* CreateShader(IEffect::EFFECTTYPE Type, ICompiler& Compiler, const CHAR* pcCode = ALCHEMY_NULL, const CHAR* pcAdditionalCode = ALCHEMY_NULL);
		CEffect* CreateEffect(UINT uPassCount, const UINT* puRenderTargetIndexCount, const CHAR** ppCodes, const CHAR** ppAdditionalCodes, const CEffect::PASSINFO* pPassInfos);
		CEffect* CreateEffect(CEffectCode& EffectCode);

		LPTEXTURE CreateTexture(UINT uWidth, UINT uHeight, UINT uUsage, CTexture::FORMAT Format, UINT uMipLevel = 0, const void* pData = ALCHEMY_NULL);
		LPTEXTURE CreateRenderTargetView(CTexture::FORMAT Format, INT nShift);
		//LPTEXTURE CreateTexture(CTexture::ITextureInfo& Info, UINT uUsage, CTexture::FORMAT Format, UINT uMipLevel = 0, const void* pData = ALCHEMY_NULL);
		LPTEXTURE CreateCubeTexture(
			UINT uSize, 
			UINT uUsage,
			CTexture::FORMAT Format,
			UINT uMipLevel = 0, 
			const void* pPositiveXData = ALCHEMY_NULL,
			const void* pNegativeXData = ALCHEMY_NULL,
			const void* pPositiveYData = ALCHEMY_NULL,
			const void* pNegativeYData = ALCHEMY_NULL,
			const void* pPositiveZData = ALCHEMY_NULL,
			const void* pNegativeZData = ALCHEMY_NULL);

		virtual bool CheckFormat(FORMATTYPE Type, UINT uFormat) = 0;

		virtual ICompiler& GetCompiler() = 0;

		virtual bool GetViewport(PUINT puWidth, PUINT puHeight) = 0;

		virtual bool Clear(ARGBCOLOR Color, UINT uFlag) = 0;
		virtual bool ResetTexture() = 0;

		/*virtual bool RegisterRenderState() = 0;
		virtual bool RestoreRenderState() = 0;

		virtual bool SetRenderState(const RENDERSTATEINFO* pRenderStateInfos, UINT uRenderStateInfoCount, bool bIsRegister) = 0;*/

		virtual bool SetRenderTarget(CGraphicsResource::HANDLE Texture, UINT uSurface, UINT uIndex) = 0;

		virtual bool SetVertexDeclaration(CGraphicsResource::HANDLE VertexDeclaration) = 0;
		virtual bool SetStreamSource(CGraphicsResource::HANDLE VertexBuffer, UINT uStream) = 0;
		virtual bool SetIndices(CGraphicsResource::HANDLE IndexBuffer) = 0;
		virtual bool SetShader(CGraphicsResource::HANDLE Shader) = 0;

		/*virtual SetShaderParameters(EFFECTTYPE Type, UINT uIndex, const FLOAT* pfValues, UINT uCount) = 0;

		virtual bool SetShaderMatrixArray(EFFECTTYPE Type, UINT uIndex, const FLOAT* pfValues, UINT uRow, UINT uColumn, UINT uRowOffset, UINT uOffset, UINT uCount) = 0;
		virtual bool SetShaderMatrixPointerArray(EFFECTTYPE Type, UINT uIndex, const PFLOAT* ppfValue, UINT uRow, UINT uColumn, UINT uRowOffset, UINT uCount) = 0;

		virtual bool SetShaderConstants(IEffect::EFFECTTYPE Type, CGraphicsResource::HANDLE ConstantBuffer, UINT uIndex) = 0;

		virtual bool SetTexture(EFFECTTYPE Type, CGraphicsResource::HANDLE Texture, UINT uIndex, const SAMPLERDECLARATION* pSamplerDeclaration) = 0;*/

		//virtual bool DrawTriangles(TRIANGLETYPE Type, UINT uStartVertex, UINT uTriangleCount) = 0;
		virtual bool DrawIndexedTriangles(TRIANGLETYPE Type, UINT uStartIndex, UINT uTriangleCount) = 0;

		virtual bool Present() = 0;
		
		ALCHEMY_IMPLEMENT_INTERFACE

	protected:
		RENDERSYSTEMINFO m_RenderSystemInfo;

	private:
		static void __CreateRenderTargetView(CGraphicsBuffer<CTexture>& Texture, INT nShift);
	};

	ALCHEMY_INLINE INT CRenderSystem::CRenderTargetViewListener::GetShift()const
	{
		return m_nShift;
	}

	ALCHEMY_INLINE const CRenderSystem::RENDERSYSTEMINFO& CRenderSystem::GetRenderSystemInfo()const
	{
		return m_RenderSystemInfo;
	}
}