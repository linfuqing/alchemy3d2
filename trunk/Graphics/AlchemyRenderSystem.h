#pragma once

#include "AlchemyGraphicsResourcePool.h"
#include "AlchemyMeshManager.h"
#include "AlchemyRenderMethod.h"
#include "AlchemyShader.h"

#include "../Core/AlchemyMemory.h"

namespace alchemy
{
	class CRenderSystem :
		public CGraphicsResourcePool
	{
	public:
		typedef CIndexBuffer::TRIANGLETYPE TRIANGLETYPE;

		typedef enum
		{
			STENCIL = 0,
			TARGET,
			ZBUFFER
		}CLEARTYPE;

		typedef enum  
		{
			FILL_MODE = 0,

			SHADE_MODE,

			DEPTH_WRITEE_ENABLE,
			DEPTH_FUNC,

			SRC_BLEND,
			DEST_BLEND,
			ALPHA_REF,
			ALPHA_FUNC,

			CULL_MODE,

			STENCIL_FAIL,
			STENCIL_Z_FAIL,
			STENCIL_PASS,
			STENCIL_FUNC,
			STENCIL_REF,
			STENCIL_READ_MASK,
			STENCIL_WRITE_MASK,
			CCW_STENCIL_FAIL,
			CCW_STENCIL_Z_FAIL,
			CCW_STENCIL_PASS,
			CCW_STENCIL_FUNC,

			TOTAL_RENDER_STATE
		}RENDERSTATE;

		typedef enum
		{
			POINT = 0,
			WIREFRAME,
			SOLID,
		}FILLMODE;

		typedef enum
		{
			FLAT = 0,
			GOURAUD,
			PHONG         
		}SHADEMODE;

		typedef enum
		{
			NONE = 0,
			CCW,
			CW
		}CULLMODE;

		typedef enum
		{
			NEVER = 0,
			LESS,
			EQUAL,
			LESS_EQUAL,
			GREATER,
			NOT_EQUAL,
			GREATER_EQUAL,
			ALWAYS,
		}CMPFUNC;

		typedef enum 
		{
			ZERO = 0,
			ONE,
			SRC_COLOR,
			INV_SRC_COLOR,
			SRC_ALPHA,
			INV_SRC_ALPHA,
			DEST_ALPHA,
			INV_DEST_ALPHA,
			DEST_COLOR,
			INV_DEST_COLOR,
			SRC_ALPHA_SAT,
			BOTH_SRCALPHA,
			BOTH_INV_SRC_ALPHA
		}BLENDMODE;

		typedef enum
		{
			KEEP = 0,
			THROW,
			REPLACE,
			INCR_SAT,
			DECR_SAT,
			INV_ERT,
			INCR,
			DECR,
		}STENCILOP;

		CRenderSystem(void);
		virtual ~CRenderSystem(void);

		CVertexDeclaration* CreateVertexDeclaration(const CVertexDeclaration::VERTEXELEMENT* pVertexElements = ALCHEMY_NULL, UINT uElementCount = 0);

		LPVERTEXBUFFER CreateVertexBuffer(UINT uCount, UINT uStride, UINT uUsage, const void* pData = ALCHEMY_NULL);
		LPINDEXBUFFER CreateIndexBuffer(UINT uCount, UINT uUsage, CIndexBuffer::FORMAT Format, const void* pData = ALCHEMY_NULL);

		CShader* CreateShader(CEffectInterface::EFFECTTYPE Type, CCompilerInterface& Compiler, const CHAR* pCode = ALCHEMY_NULL, const CHAR* pAdditionalCode = ALCHEMY_NULL, CBuffer** ppCompilationErrors = ALCHEMY_NULL);
		CEffect* CreateEffect(UINT uPassCount = 0, const CHAR* pCode = ALCHEMY_NULL, CBuffer** ppCompilationErrors = ALCHEMY_NULL);
		CEffect* CreateEffect(CEffectCode& EffectCode, CBuffer** ppCompilationErrors = ALCHEMY_NULL);

		LPTEXTURE CreateTexture(UINT uWidth, UINT uHeight, UINT uUsage, CTexture::FORMAT Format, UINT uMipLevel = 0, const void* pData = ALCHEMY_NULL);
		LPTEXTURE CreateTexture(CTexture::CTextureInfoInterface& Info, UINT uUsage, CTexture::FORMAT Format, UINT uMipLevel = 0, const void* pData = ALCHEMY_NULL);
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

		virtual CCompilerInterface& GetComplier() = 0;

		virtual bool GetViewport(PUINT puWidth, PUINT puHeight) = 0;

		virtual CGraphicsResource::HANDLE GetRenderTarget() = 0;

		virtual bool Clear(ARGBCOLOR Color, UINT uFlag) = 0;

		virtual bool RegisterRenderState() = 0;
		virtual bool RestoreRenderState() = 0;

		virtual bool SetRenderState(RENDERSTATE State, UINT uValue) = 0;

		virtual bool SetRenderTarget(CGraphicsResource::HANDLE Texture, UINT uSurface, UINT uIndex) = 0;

		virtual bool SetVertexDeclaration(CGraphicsResource::HANDLE VertexDeclaration) = 0;
		virtual bool SetStreamSource(CGraphicsResource::HANDLE VertexBuffer, UINT uStream) = 0;
		virtual bool SetIndices(CGraphicsResource::HANDLE IndexBuffer) = 0;
		virtual bool SetShader(CGraphicsResource::HANDLE Shader) = 0;

		//virtual bool DrawTriangles(TRIANGLETYPE Type, UINT uStartVertex, UINT uTriangleCount) = 0;
		virtual bool DrawIndexedTriangles(TRIANGLETYPE Type, UINT uStartIndex, UINT uTriangleCount) = 0;

		virtual bool DrawToTexture(CGraphicsResource::HANDLE Source, CEffectCode::SAMPLERFILTER Filter) = 0;
	};
}