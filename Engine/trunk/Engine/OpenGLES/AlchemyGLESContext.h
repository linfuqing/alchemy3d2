#pragma once
/*
 *  AlchemyGLESContext.h
 *  IOS
 *
 *  Created by lin fuqing on 12-5-1.
 *  Copyright 2012 AstepGame. All rights reserved.
 *
 */

#include "AlchemyGLSLCompiler.h"
#include "AlchemyGLESVertexDeclaration.h"
#include "AlchemyGLESVertexBuffer.h"
#include "AlchemyGLESIndexBuffer.h"
#include "AlchemyGLESTexture.h"

#include "../Graphics/AlchemyRenderSystem.h"

namespace  alchemy {
    class CGLESContext :
    public CRenderSystem
    {
    public:
        CGLESContext();
        ~CGLESContext();
		
		GLuint GetBackRenderBuffer()const;
        
        bool CheckFormat(FORMATTYPE Type, UINT uFormat);
        
		bool SetViewport(UINT uWidth, UINT uHeight);
		bool GetViewport(PUINT puWidth, PUINT puHeight);
		
		ICompiler& GetCompiler();
        
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
		
		bool SetTexture(IEffect::EFFECTTYPE Type, CGraphicsResource::HANDLE Texture, UINT uIndex, const IEffect::SAMPLERDECLARATION* pSamplerDeclaration);
		
		bool DrawIndexedTriangles(TRIANGLETYPE Type, UINT uStartIndex, UINT uTriangleCount);
		
	protected:
		CGraphicsResource* _Create(CGraphicsResource::TYPE Type);
        
    private:
		typedef struct
		{
			CGLESTexture*               pTexture;
			IEffect::SAMPLERFILTER      MagFilter;
			IEffect::SAMPLERFILTER      MinFilter;
			IEffect::SAMPLERFILTER      MipFilter;
			IEffect::SAMPLERADDRESSMODE WrapS;
			IEffect::SAMPLERADDRESSMODE WrapT;
		}TEXTUREDATA, * LPTEXTUREDATA;
		
		bool __UseProgram();
		
		bool __SetRenderState(CRenderSystem::RENDERSTATE State, UINT uValue);
		
		static const GLenum sm_CULL_MODE[CRenderSystem::TOTAL_CULL_MODE];
		static const GLenum sm_CMP_FUNC[CRenderSystem::TOTAL_CMP_FUNC];
		static const GLenum sm_BLEND_MODE[CRenderSystem::TOTAL_BLEND_MODE];
		static const GLenum sm_STENCIL_OP[CRenderSystem::TOTAL_STENCIL_OP];
		
		CRenderSystem::RENDERSTATE m_RenderStateStack[CRenderSystem::TOTAL_RENDER_STATE];
		UINT m_uRenderStateValueStack[CRenderSystem::TOTAL_RENDER_STATE];
		UINT m_uRenderState[CRenderSystem::TOTAL_RENDER_STATE];
		UINT m_uRenderStateFlag;
		UINT m_uRenderStateStackCount;

		GLuint m_uBackRenderBuffer;
		GLuint m_uBackDepthBuffer;
		GLuint m_uFrameBuffer;
		
		GLuint m_uProgram;
		GLuint m_uUserProgram;
		
		CGLESTexture* m_pRenderTarget;
		
        CGLSLCompiler m_Compiler;
		
		CGLESVertexDeclaration* m_pVertexDeclaration;
		
		CGLESIndexBuffer* m_pIndexBuffer;
		
		CGLESVertexBuffer** m_ppVertexBuffers;
		UINT m_uVertexBufferCount;
		UINT8 m_uVertexAttribeFlag;
		
		LPTEXTUREDATA m_pTextureDatas;
		UINT m_uTextureDataCount;
		UINT8 m_uTextureFlag;
		
		PFLOAT m_pfShaderConstants;
		UINT m_uShaderConstantCount;
		bool* m_pbDirtyShaderRegisters;
		UINT m_uDirtyVertexShaderRegisterOffset;
		UINT m_uDirtyVertexShaderRegisterEnd;
		UINT m_uDirtyFragmentShaderRegisterOffset;
		UINT m_uDirtyFragmentShaderRegisterEnd;
		
		UINT m_uWidth;
		UINT m_uHeight;
    };
	
	ALCHEMY_INLINE GLuint CGLESContext::GetBackRenderBuffer()const
	{
		return m_uBackRenderBuffer;
	}
}