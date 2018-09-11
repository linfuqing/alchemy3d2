/*
 *  AlchemyGLESContext.cpp
 *  IOS
 *
 *  Created by lin fuqing on 12-5-1.
 *  Copyright 2012 AstepGame. All rights reserved.
 *
 */

#include "AlchemyGLESContext.h"

#include "AlchemyGLESEffect.h"

using namespace alchemy;

const GLenum CGLESContext::sm_CULL_MODE[CRenderSystem::TOTAL_CULL_MODE] = 
{
	0, 
	GL_BACK,
	GL_FRONT
};

const GLenum CGLESContext::sm_CMP_FUNC[CRenderSystem::TOTAL_CMP_FUNC] = 
{
	GL_NEVER, 
	GL_LESS, 
	GL_EQUAL, 
	GL_LEQUAL, 
	GL_GREATER, 
	GL_NOTEQUAL, 
	GL_GEQUAL, 
	GL_ALWAYS
};

const GLenum CGLESContext::sm_BLEND_MODE[CRenderSystem::TOTAL_BLEND_MODE] = 
{
	GL_ZERO, 
	GL_ONE, 
	GL_SRC_COLOR, 
	GL_ONE_MINUS_SRC_COLOR, 
	GL_SRC_ALPHA, 
	GL_ONE_MINUS_SRC_ALPHA, 
	GL_DST_ALPHA, 
	GL_ONE_MINUS_DST_ALPHA, 
	GL_DST_COLOR, 
	GL_ONE_MINUS_DST_COLOR, 
	GL_SRC_ALPHA_SATURATE
};

const GLenum CGLESContext::sm_STENCIL_OP[CRenderSystem::TOTAL_STENCIL_OP] = 
{
	GL_KEEP, 
	0,
	GL_REPLACE, 
	GL_INCR_WRAP, 
	GL_DECR_WRAP, 
	GL_INVERT, 
	GL_INCR, 
	GL_DECR
};

CGLESContext::CGLESContext() :
m_uRenderStateFlag(0), 
m_uRenderStateStackCount(0), 
m_uBackRenderBuffer(0), 
m_uBackDepthBuffer(0), 
m_uFrameBuffer(0),
m_uProgram(0), 
m_uUserProgram(0),
m_pRenderTarget(ALCHEMY_NULL), 
m_pVertexDeclaration(ALCHEMY_NULL), 
m_pIndexBuffer(ALCHEMY_NULL),
m_ppVertexBuffers(ALCHEMY_NULL),
m_uVertexBufferCount(0), 
m_uVertexAttribeFlag(0),
m_pTextureDatas(ALCHEMY_NULL), 
m_uTextureDataCount(0), 
m_uTextureFlag(0), 
m_pfShaderConstants(ALCHEMY_NULL), 
m_uShaderConstantCount(0), 
m_pbDirtyShaderRegisters(ALCHEMY_NULL), 
m_uDirtyVertexShaderRegisterOffset(~0), 
m_uDirtyVertexShaderRegisterEnd(0), 
m_uDirtyFragmentShaderRegisterOffset(~0), 
m_uDirtyFragmentShaderRegisterEnd(0),
m_uWidth(0),
m_uHeight(0)
{	
	m_uRenderState[IRenderState::FILL_MODE] = 0;
	
	m_uRenderState[IRenderState::SHADE_MODE] = 0;
	
	m_uRenderState[IRenderState::DEPTH_WRITEE_ENABLE] = ALCHEMY_TRUE;
	m_uRenderState[IRenderState::DEPTH_FUNC] = IRenderState::LESS;
	
	m_uRenderState[IRenderState::ALPHA_FUNC] = IRenderState::LESS;
	
	m_uRenderState[IRenderState::SRC_BLEND] = IRenderState::ONE;
	m_uRenderState[IRenderState::DEST_BLEND] = IRenderState::ZERO;
	
	m_uRenderState[IRenderState::ALPHA_REF] = 0;
	m_uRenderState[IRenderState::ALPHA_FUNC] = IRenderState::LESS;
	
	m_uRenderState[IRenderState::CULL_MODE] = IRenderState::CULL_NONE;
	
	m_uRenderState[IRenderState::STENCIL_FUNC] = IRenderState::ALWAYS;
	m_uRenderState[IRenderState::STENCIL_FAIL] = IRenderState::KEEP;
	m_uRenderState[IRenderState::STENCIL_Z_FAIL] = IRenderState::KEEP;
	m_uRenderState[IRenderState::STENCIL_PASS] = IRenderState::KEEP;
	
	m_uRenderState[IRenderState::STENCIL_REF] = 0;
	m_uRenderState[IRenderState::STENCIL_READ_MASK] = 0xff;
	m_uRenderState[IRenderState::STENCIL_WRITE_MASK] = 0xff;
	
	m_uRenderState[IRenderState::CCW_STENCIL_FAIL] = IRenderState::KEEP;
	m_uRenderState[IRenderState::CCW_STENCIL_Z_FAIL] = IRenderState::KEEP;
	m_uRenderState[IRenderState::CCW_STENCIL_PASS] = IRenderState::KEEP;
	m_uRenderState[IRenderState::CCW_STENCIL_FUNC] = IRenderState::ALWAYS;
}

CGLESContext::~CGLESContext()
{
	//glDeleteRenderbuffers(1, &m_uBackRenderBuffer);
	//glDeleteRenderbuffers(1, &m_uBackDepthBuffer );
	//glDeleteFramebuffers( 1, &m_uFrameBuffer     );
	
	ALCHEMY_DEBUG_DELETE_ARRAY(m_ppVertexBuffers);
	ALCHEMY_DEBUG_DELETE_ARRAY(m_pTextureDatas);
	ALCHEMY_DEBUG_DELETE_ARRAY(m_pfShaderConstants);
    ALCHEMY_DEBUG_DELETE_ARRAY(m_pbDirtyShaderRegisters);
}

bool CGLESContext::CheckFormat(FORMATTYPE Type, UINT uFormat)
{
	switch(Type)
	{
	case CRenderSystem::TEXTURE:
		switch(uFormat)
		{
		case CTexture::R8G8B8A8:
			return true;
			break;
		case CTexture::RED32:
			return false;
			break;
		case CTexture::RED32FLOAT:
			return false;
			break;
		}
		break;

	case CRenderSystem::DEPTH_STENCIL:
		return false;
		break;

	case CRenderSystem::INDEX_BUFFER:
		switch(uFormat)
		{
		case CIndexBuffer::BIT16:
			return true;
			break;
		case CIndexBuffer::BIT32:
			return false;
			break;
		}
		break;
		
	default:
		break;
	}

	return false;
}

bool CGLESContext::SetViewport(UINT uWidth, UINT uHeight)
{
	m_uWidth = uWidth;
	m_uHeight = uHeight;
	
	if(m_uBackRenderBuffer)
		glDeleteRenderbuffers(1, &m_uBackRenderBuffer);
	
	if(m_uBackDepthBuffer)
		glDeleteRenderbuffers(1, &m_uBackDepthBuffer );
	
	if(m_uFrameBuffer)
		glDeleteFramebuffers( 1, &m_uFrameBuffer     );
	
	glGenRenderbuffers(1, &m_uBackDepthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_uBackDepthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, uWidth, uHeight);
	
	/*glGenTextures(1, &m_uBackDepthBuffer);
	glBindTexture(GL_TEXTURE_2D, m_uBackDepthBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, uWidth, uHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT_5_6_5, ALCHEMY_NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);*/
	
	glGenRenderbuffers(1, &m_uBackRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_uBackRenderBuffer);
	
    glGenFramebuffers(1, &m_uFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_uFrameBuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_uBackRenderBuffer);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT , GL_RENDERBUFFER, m_uBackDepthBuffer );
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_uBackDepthBuffer, 0);
	
	glViewport(0, 0, uWidth, uHeight);
	
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	
	UINT i;
	GLint nCount;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nCount);
	m_uVertexBufferCount = nCount;
	ALCHEMY_DEBUG_DELETE_ARRAY(m_ppVertexBuffers);
	ALCHEMY_DEBUG_NEW(m_ppVertexBuffers, CGLESVertexBuffer*[m_uVertexBufferCount]);
	for(i = 0; i < m_uVertexBufferCount; ++ i)
		m_ppVertexBuffers[i] = ALCHEMY_NULL;
	
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &nCount);
	m_uTextureDataCount = nCount;
	ALCHEMY_DEBUG_DELETE_ARRAY(m_pTextureDatas);
	ALCHEMY_DEBUG_NEW(m_pTextureDatas, TEXTUREDATA[m_uTextureDataCount]);
	LPTEXTUREDATA pTextureData;
	for(i = 0; i < m_uTextureDataCount; ++ i)
	{
		pTextureData = &m_pTextureDatas[i];
		pTextureData->pTexture  = ALCHEMY_NULL;
		pTextureData->MagFilter = IEffect::LINEAR;
		pTextureData->MinFilter = IEffect::LINEAR;
		pTextureData->MipFilter = IEffect::LINEAR;
		pTextureData->WrapS     = IEffect::WRAP;
		pTextureData->WrapT     = IEffect::WRAP;
	}
	
	glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &nCount);
	m_uShaderConstantCount = nCount;
	glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_VECTORS, &nCount);
	m_uShaderConstantCount += nCount; 
	m_uShaderConstantCount += m_uTextureDataCount;
	ALCHEMY_DEBUG_DELETE_ARRAY(m_pbDirtyShaderRegisters);
	ALCHEMY_DEBUG_NEW(m_pbDirtyShaderRegisters, bool[m_uShaderConstantCount]);
	for(i = 0; i < m_uShaderConstantCount; ++ i)
		m_pbDirtyShaderRegisters[i] = false;
	
	ALCHEMY_DEBUG_DELETE_ARRAY(m_pfShaderConstants);
	ALCHEMY_DEBUG_NEW(m_pfShaderConstants, FLOAT[m_uShaderConstantCount *= 4]);
	for(i = 0; i < m_uShaderConstantCount; ++ i)
		m_pfShaderConstants[i] = 0;
		
	m_RenderSystemInfo.uRenderTargetCount   = 1;
	m_RenderSystemInfo.uMaxVertexCount      = 0xffff;
	m_RenderSystemInfo.uMaxVertexIndexCount = ~0;
	
	return true;
}

bool CGLESContext::GetViewport(PUINT puWidth, PUINT puHeight)
{
    if(puWidth)
        *puWidth = m_uWidth;
    
    if(puHeight)
        *puHeight = m_uHeight;
    
    return true;
}

ICompiler& CGLESContext::GetCompiler()
{
    return m_Compiler;
}

bool CGLESContext::Clear(ARGBCOLOR Color, UINT uFlag)
{
    GLbitfield ClearFlag = 0;
    if( ALCHEMY_TEST_BIT(uFlag, CRenderSystem::TARGET) )
    {
        glClearColor( ALCHEMY_ARGBCOLOR_RED(Color), ALCHEMY_ARGBCOLOR_GREEN(Color), ALCHEMY_ARGBCOLOR_BLUE(Color), ALCHEMY_ARGBCOLOR_ALPHA(Color) );
        ClearFlag |= GL_COLOR_BUFFER_BIT;
    }
    
    if ( ALCHEMY_TEST_BIT(uFlag, CRenderSystem::ZBUFFER)) 
    {
        glClearDepthf(1.0f);
        ClearFlag |= GL_DEPTH_BUFFER_BIT;
    }
    
    if ( ALCHEMY_TEST_BIT(uFlag, CRenderSystem::STENCIL) ) 
    {
        glClearStencil(0);
        ClearFlag |= GL_STENCIL_BUFFER_BIT;
    }
	
	glClear(ClearFlag);
	
	return true;
}

bool CGLESContext::ResetTexture()
{
	m_uTextureFlag = 0;
	
    return true;
}

bool CGLESContext::RegisterRenderState()
{
	m_uRenderStateStackCount = 0;
	
	m_uRenderStateFlag = 0;
	
    return true;
}

bool CGLESContext::RestoreRenderState()
{
	CRenderSystem::RENDERSTATE State;
	UINT uValue;
	for(UINT i = 0; i < m_uRenderStateStackCount; ++ i)
	{
		State  = m_RenderStateStack[i];
		uValue = m_uRenderStateValueStack[i];
		
		if(m_uRenderState[State] != uValue)
		{
			m_uRenderState[State] = uValue;
			
			__SetRenderState(State, uValue);
		}
	}
		
	m_uRenderStateStackCount = 0;
	
	m_uRenderStateFlag = 0;
	
    return true;
}

bool CGLESContext::SetRenderState(const RENDERSTATEINFO* pRenderStateInfos, UINT uRenderStateInfoCount, bool bIsRegister)
{
	if(bIsRegister)
	{
		m_uRenderStateStackCount = 0;
	
		m_uRenderStateFlag = 0;
	}
	
	const RENDERSTATEINFO* pRenderStateInfo;
	for(UINT i = 0; i < uRenderStateInfoCount; ++ i)
	{
		pRenderStateInfo = &pRenderStateInfos[i];
		if(pRenderStateInfo->State > 0 && pRenderStateInfo->State < CRenderSystem::TOTAL_RENDER_STATE)
		{
			if( !ALCHEMY_TEST_BIT(m_uRenderStateFlag, pRenderStateInfo->State) )
			{
				m_RenderStateStack[m_uRenderStateStackCount] = pRenderStateInfo->State;
				
				m_uRenderStateValueStack[m_uRenderStateStackCount] = m_uRenderState[pRenderStateInfo->State];
				
				ALCHEMY_SET_BIT(m_uRenderStateFlag, pRenderStateInfo->State);
				
				++ m_uRenderStateStackCount;
			}
			
			m_uRenderState[pRenderStateInfo->State] = pRenderStateInfo->uValue;
			
			if( !__SetRenderState(pRenderStateInfo->State, pRenderStateInfo->uValue) )
				return false;
		}
	}
	
	return true;
}

bool CGLESContext::SetRenderTarget(CGraphicsResource::HANDLE Texture, UINT uSurface, UINT uIndex)
{
	if(uIndex > 0)
		return false;
	
	if(!Texture && m_pRenderTarget)
	{
		//glBindFramebuffer(GL_FRAMEBUFFER, m_uFrameBuffer);
		//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_uBackDepthBuffer);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_uBackRenderBuffer);
		
		m_pRenderTarget = ALCHEMY_NULL;
		
		return true;
	}
	
	CGLESTexture* pTexture = static_cast<CGLESTexture*>( GetResource(Texture, CGraphicsResource::TEXTURE) );
	if( pTexture && m_pRenderTarget != pTexture && ALCHEMY_TEST_BIT(pTexture->GetUsage(), CTexture::RENDER_TARGET) )
	{
		//pTexture->BindToRenderTarget(m_uBackDepthBuffer);
		
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, CGLESTexture::GetGLESTextureType( pTexture->GetTextureType() ), pTexture->GetResourceHandle(), 0);
		
		m_pRenderTarget = pTexture;
		
		return true;
	}
	
	return false;
}

bool CGLESContext::SetVertexDeclaration(CGraphicsResource::HANDLE VertexDeclaration)
{
	CGLESVertexDeclaration* pVertexDeclaration = static_cast<CGLESVertexDeclaration*>( GetResource(VertexDeclaration, CGraphicsResource::VERTEX_DECLARATION) );
	if(pVertexDeclaration)
	{
		pVertexDeclaration->AddRef();
		
		if(m_pVertexDeclaration)
			m_pVertexDeclaration->Release();
		
		m_pVertexDeclaration = pVertexDeclaration;
		
		return true;
	}
	
	return false;
}

bool CGLESContext::SetStreamSource(CGraphicsResource::HANDLE VertexBuffer, UINT uStream)
{
	if(uStream < m_uVertexBufferCount)
	{
		CGLESVertexBuffer* pVertexBuffer = static_cast<CGLESVertexBuffer*>( GetResource(VertexBuffer, CGraphicsResource::VERTEX_BUFFER) );
		if(pVertexBuffer)
		{
			pVertexBuffer->AddRef();
			
			CGLESVertexBuffer* pCurrentVertexBuffer = m_ppVertexBuffers[uStream];
			if (pCurrentVertexBuffer) 
				pCurrentVertexBuffer->Release();
			
			m_ppVertexBuffers[uStream] = pVertexBuffer;
		}
	}
	else 
		ALCHEMY_DEBUG_WARNING("CIOSOpenGLESContext::SetStreamSource: Stream out of range.");
	
    return false;
}

bool CGLESContext::SetIndices(CGraphicsResource::HANDLE IndexBuffer)
{
	CGLESIndexBuffer* pIndexBuffer = static_cast<CGLESIndexBuffer*>( GetResource(IndexBuffer, CGraphicsResource::INDEX_BUFFER) );
    if(pIndexBuffer)
	{
		pIndexBuffer->AddRef();
		
		if(m_pIndexBuffer)
			m_pIndexBuffer->Release();
		
		m_pIndexBuffer = pIndexBuffer;
		
		return true;
	}
	
	return false;
}

bool CGLESContext::SetShader(CGraphicsResource::HANDLE Shader)
{
	CGLESShader* pShader = static_cast<CGLESShader*>( GetResource(Shader, CGraphicsResource::SHADER) );
	if(pShader)
	{
		IEffect::EFFECTTYPE Type = pShader->GetShaderType();
		if(Type == IEffect::PROGRAM)
		{
			m_uUserProgram = pShader->GetShaderHandle();
			
			glUseProgram(m_uUserProgram);
		}
		else 
		{
			m_uUserProgram = 0;
			
			if(!m_uProgram)
				m_uProgram = glCreateProgram();
			
			glAttachShader( m_uProgram, pShader->GetShaderHandle() );
		}
		
		return true;
	}
	
	return false;
}

bool CGLESContext::SetShaderParameters(EFFECTTYPE Type, UINT uIndex, const FLOAT* pfValues, UINT uCount)
{
	/*if( !__UseProgram() )
		return false;*/

	/*if(uCount & 1)
		glUniform1fv(uIndex, uCount, pfValues);
	else if(uCount & 2)
		glUniform2fv(uIndex, uCount >> 1, pfValues);
	else if(uCount & 4)
		glUniform4fv(uIndex, uCount >> 2, pfValues);*/
	/*UINT uUniform4Count = 0;
	if(uCount & 3)
	{
		uUniform4Count = ( (uCount + 3) >> 2 ) - 1;
		
		FLOAT fValues[4];
		UINT i;
		for(i = uCount & ~3; i < uCount; ++ i)
			fValues[i & 3] = pfValues[i];
		
		//for(i = uCount & 3; i < 4; ++ i)
		//	fValues[i] = 0.0f;
		
		glUniform4fv(uIndex + uUniform4Count, 1, fValues);
	}
	else
		uUniform4Count = (uCount + 3) >> 2;
	
	if(uUniform4Count)
		glUniform4fv(uIndex, uUniform4Count, pfValues);*/
		
	if(!m_pfShaderConstants || !m_uShaderConstantCount)
		return false;
	
	if(Type == IEffect::VERTEX_SHADER)
		m_uDirtyVertexShaderRegisterOffset = ALCHEMY_MININUM(m_uDirtyVertexShaderRegisterOffset, uIndex >> 2);
	else if(Type == IEffect::FRAGMENT_SHADER)
		m_uDirtyFragmentShaderRegisterOffset = ALCHEMY_MININUM(m_uDirtyFragmentShaderRegisterOffset, uIndex >> 2);
	
	UINT uDirtyFlag = uIndex & 3;
	for(UINT i = 0; i < uCount; ++ i)
	{
		if( (uIndex & 3) == uDirtyFlag ) 
			m_pbDirtyShaderRegisters[uIndex >> 2] = true;

		m_pfShaderConstants[uIndex ++] = pfValues[i];
	}
	
	if(Type == IEffect::VERTEX_SHADER)
		m_uDirtyVertexShaderRegisterEnd = ALCHEMY_MAXINUM(m_uDirtyVertexShaderRegisterEnd, (uIndex + 3) >> 2);
	else if(Type == IEffect::FRAGMENT_SHADER)
		m_uDirtyFragmentShaderRegisterEnd = ALCHEMY_MAXINUM(m_uDirtyFragmentShaderRegisterEnd, (uIndex + 3) >> 2);
		
	return true;
}

bool CGLESContext::SetShaderMatrixArray(EFFECTTYPE Type, UINT uIndex, const FLOAT* pfValues, UINT uRow, UINT uColumn, UINT uRowOffset, UINT uOffset, UINT uCount)
{
	/*if( !__UseProgram() )
		return false;*/
	
	if(!m_pfShaderConstants || !m_uShaderConstantCount)
		return false;
		
	if(uRowOffset < uRow)
		return false;
		
	UINT uStride = uRowOffset * uColumn;
	
	if(uOffset < uStride)
		return false;

	uOffset -= uStride;
	
	if(Type == IEffect::VERTEX_SHADER)
		m_uDirtyVertexShaderRegisterOffset = ALCHEMY_MININUM(m_uDirtyVertexShaderRegisterOffset, uIndex >> 2);
	else if(Type == IEffect::FRAGMENT_SHADER)
		m_uDirtyFragmentShaderRegisterOffset = ALCHEMY_MININUM(m_uDirtyFragmentShaderRegisterOffset, uIndex >> 2);
	
	UINT i, j, k, uDirtyFlag = uIndex & 3;
	for(i = 0; i < uCount; ++ i)
	{
		for(j = 0; j < uColumn; ++ j)
		{
			for(k = 0; k < uRow; ++ k)
			{
				if( (uIndex & 3) == uDirtyFlag ) 
						m_pbDirtyShaderRegisters[uIndex >> 2] = true;
						
				m_pfShaderConstants[uIndex ++] = pfValues[k];
			}
			
			pfValues += uRowOffset;
		}
		
		pfValues += uOffset;
	}
	
	if(Type == IEffect::VERTEX_SHADER)
		m_uDirtyVertexShaderRegisterEnd = ALCHEMY_MAXINUM(m_uDirtyVertexShaderRegisterEnd, (uIndex + 3) >> 2);
	else if(Type == IEffect::FRAGMENT_SHADER)
		m_uDirtyFragmentShaderRegisterEnd = ALCHEMY_MAXINUM(m_uDirtyFragmentShaderRegisterEnd, (uIndex + 3) >> 2);

	/*UINT uMatrixOffset = uRow * uColumn;
	if(uMatrixOffset == uOffset && uRow == uColumn)
	{
		if(uRow == 4)
			//glUniformMatrix4fv(uIndex, uCount * 16, GL_FALSE, pfValues);
			glUniform4fv(uIndex, uCount * 4, pfValues);
			uCount *= 16;
		else if(uRow == 3)
			glUniform3fv(uIndex, uCount * 3, pfValues);
			//glUniformMatrix3fv(uIndex, uCount, GL_FALSE, pfValues);
			uCount *= 9;
		else if(uRow == 2)
			glUniform2fv(uIndex, uCount * 2, pfValues);
			//glUniformMatrix2fv(uIndex, uCount, GL_FALSE, pfValues);
		else 
		{
			ALCHEMY_DEBUG_WARNING("CGLESContext::SetShaderMatrixArray: Error matrix");
			
			return false;
		}
	}
	else
	{
		UINT i;

		switch (uRowOffset) 
		{
			case 4:
				for(i = 0; i < uCount; ++ i)
				{
					glUniform4fv(uIndex, uColumn, pfValues);
					
					uIndex   += uColumn;
					pfValues += uOffset;
				}
				break;
			case 3:
				for(i = 0; i < uCount; ++ i)
				{
					glUniform3fv(uIndex, uColumn, pfValues);
					
					uIndex   += uColumn;
					pfValues += uOffset;
				}
				break;
			case 2:
				for(i = 0; i < uCount; ++ i)
				{
					glUniform2fv(uIndex, uColumn, pfValues);
					
					uIndex   += uColumn;
					pfValues += uOffset;
				}
				break;
			default:
				ALCHEMY_DEBUG_WARNING("CGLESContext::SetShaderMatrixArray: Error matrix");
				
				return false;
				
				break;
		}
	}*/

	return true;
}

bool CGLESContext::SetShaderMatrixPointerArray(EFFECTTYPE Type, UINT uIndex, const PFLOAT* ppfValues, UINT uRow, UINT uColumn, UINT uRowOffset, UINT uCount)
{
	if(!m_pfShaderConstants || !m_uShaderConstantCount)
		return false;
		
	if(uRowOffset < uRow)
		return false;
	
	if(Type == IEffect::VERTEX_SHADER)
		m_uDirtyVertexShaderRegisterOffset = ALCHEMY_MININUM(m_uDirtyVertexShaderRegisterOffset, uIndex >> 2);
	else if(Type == IEffect::FRAGMENT_SHADER)
		m_uDirtyFragmentShaderRegisterOffset = ALCHEMY_MININUM(m_uDirtyFragmentShaderRegisterOffset, uIndex >> 2);
	
	UINT i, j, k, uDirtyFlag = uIndex & 3;
	const FLOAT* pfValues;
	for(i = 0; i < uCount; ++ i)
	{
		pfValues = ppfValues[i];
		if(pfValues)
		{
			for(j = 0; j < uColumn; ++ j)
			{
				for(k = 0; k < uRow; ++ k)
				{
					if( (uIndex & 3) == uDirtyFlag ) 
						m_pbDirtyShaderRegisters[uIndex >> 2] = true;
						
					m_pfShaderConstants[uIndex ++] = pfValues[k];
				}
				
				pfValues += uRowOffset;
			}
		}
	}
	
	if(Type == IEffect::VERTEX_SHADER)
		m_uDirtyVertexShaderRegisterEnd = ALCHEMY_MAXINUM(m_uDirtyVertexShaderRegisterEnd, (uIndex + 3) >> 2);
	else if(Type == IEffect::FRAGMENT_SHADER)
		m_uDirtyFragmentShaderRegisterEnd = ALCHEMY_MAXINUM(m_uDirtyFragmentShaderRegisterEnd, (uIndex + 3) >> 2);
	
	/*if( !__UseProgram() )
		return false;
	
	UINT i;
	
	switch (uRowOffset) 
	{
		case 4:
			for(i = 0; i < uCount; ++ i)
			{
				glUniform4fv(uIndex, uColumn, ppfValue[i]);
				
				uIndex += uColumn;
			}
			break;
		case 3:
			for(i = 0; i < uCount; ++ i)
			{
				glUniform3fv(uIndex, uColumn, ppfValue[i]);
				
				uIndex += uColumn;
			}
			break;
		case 2:
			for(i = 0; i < uCount; ++ i)
			{
				glUniform2fv(uIndex, uColumn, ppfValue[i]);
				
				uIndex += uColumn;
			}
			break;
		default:
			ALCHEMY_DEBUG_WARNING("CGLESContext::SetShaderMatrixArray: Error matrix");
			
			return false;
			
			break;
	}*/
	
	return true;
}

bool CGLESContext::SetShaderConstants(IEffect::EFFECTTYPE Type, CGraphicsResource::HANDLE ConstantBuffer, UINT uIndex)
{
	return false;
}

bool CGLESContext::SetTexture(IEffect::EFFECTTYPE Type, CGraphicsResource::HANDLE Texture, UINT uIndex, const IEffect::SAMPLERDECLARATION* pSamplerDeclaration)
{
	if(uIndex < m_uTextureDataCount)
	{
		CGLESTexture* pTexture = static_cast<CGLESTexture*>( GetResource(Texture, CGraphicsResource::TEXTURE) );
		TEXTUREDATA& TextureData = m_pTextureDatas[uIndex];
		if(TextureData.pTexture  != pTexture                       || 
		   !pSamplerDeclaration                                    ||
		   TextureData.MagFilter != pSamplerDeclaration->MagFilter ||
		   TextureData.MinFilter != pSamplerDeclaration->MinFilter ||
		   TextureData.MipFilter != pSamplerDeclaration->MipFilter ||
		   TextureData.WrapS     != pSamplerDeclaration->AddressU  ||
		   TextureData.WrapT     != pSamplerDeclaration->AddressV)
		{
			TextureData.pTexture  = pTexture;
			if(pSamplerDeclaration)
			{
				TextureData.MagFilter = pSamplerDeclaration->MagFilter;
				TextureData.MinFilter = pSamplerDeclaration->MinFilter;
				TextureData.MipFilter = pSamplerDeclaration->MipFilter;
				
				TextureData.WrapS     = pSamplerDeclaration->AddressU;
				TextureData.WrapT     = pSamplerDeclaration->AddressV;
			}
			else 
			{
				TextureData.MagFilter = IEffect::LINEAR;
				TextureData.MinFilter = IEffect::LINEAR;
				TextureData.MipFilter = IEffect::LINEAR;
				
				TextureData.WrapS     = IEffect::WRAP;
				TextureData.WrapT     = IEffect::WRAP;
			}

			
			ALCHEMY_SET_BIT(m_uTextureFlag, uIndex);
		}
		
		return true;
	}
	
	return false;
}

bool CGLESContext::DrawIndexedTriangles(TRIANGLETYPE Type, UINT uStartIndex, UINT uTriangleCount)
{
    if(m_pIndexBuffer)
    {
		if( !__UseProgram() )
			return false;
		
		UINT i;
		
		if(m_uDirtyVertexShaderRegisterEnd > m_uDirtyVertexShaderRegisterOffset)
		{
			UINT uOffset = m_uDirtyVertexShaderRegisterOffset, uCount = 0;
			for(i = m_uDirtyVertexShaderRegisterOffset; i < m_uDirtyVertexShaderRegisterEnd; ++ i)
			{
				if(m_pbDirtyShaderRegisters[i])
				{
					m_pbDirtyShaderRegisters[i] = false;
					
					++ uCount;
				}
				else
				{
					glUniform4fv(uOffset, uCount, m_pfShaderConstants + (uOffset << 2) );
					uOffset += uCount;
					uCount   = 0;
				}
			}
			
			if(uCount)
				glUniform4fv(uOffset, uCount, m_pfShaderConstants + (uOffset << 2) );
			
			m_uDirtyVertexShaderRegisterOffset = ~0;
			m_uDirtyVertexShaderRegisterEnd    = 0;
		}
		
		if(m_uDirtyFragmentShaderRegisterEnd > m_uDirtyFragmentShaderRegisterOffset)
		{
			UINT uOffset = m_uDirtyFragmentShaderRegisterOffset, uCount = 0;
			for(i = m_uDirtyFragmentShaderRegisterOffset; i < m_uDirtyFragmentShaderRegisterEnd; ++ i)
			{
				if(m_pbDirtyShaderRegisters[i])
				{
					m_pbDirtyShaderRegisters[i] = false;
					
					++ uCount;
				}
				else
				{
					glUniform4fv(uOffset, uCount, m_pfShaderConstants + (uOffset << 2) );
					uOffset += uCount;
					uCount   = 0;
				}
			}
			
			if(uCount)
				glUniform4fv(uOffset, uCount, m_pfShaderConstants + (uOffset << 2) );
			
			m_uDirtyFragmentShaderRegisterOffset = ~0;
			m_uDirtyFragmentShaderRegisterEnd    = 0;
		}

		UINT uBegin, uEnd;
		if(m_uTextureFlag)
		{
			static const GLenum s_FILTER[] = 
			{
				GL_NEAREST,
				GL_LINEAR,
				GL_NEAREST_MIPMAP_NEAREST, 
				GL_NEAREST_MIPMAP_LINEAR, 
				GL_LINEAR_MIPMAP_NEAREST, 
				GL_LINEAR_MIPMAP_LINEAR
			};
			
			static const GLenum s_WRAP[] = 
			{
				GL_REPEAT, 
				GL_MIRRORED_REPEAT, 
				GL_CLAMP_TO_EDGE
			};
			
			GLenum TextureType;
			const TEXTUREDATA* pTextureData;
			
			uBegin = ALCHEMY_MININUM_BIT(m_uTextureFlag);
			uEnd = ALCHEMY_MAXINUM_BIT(m_uTextureFlag);
			for(i = uBegin - 1; i < uEnd; ++ i)
			{
				if( ALCHEMY_SET_BIT(m_uTextureFlag, i) )
				{
					pTextureData = &m_pTextureDatas[i];
					if(pTextureData->pTexture)
					{
						glActiveTexture(i + GL_TEXTURE0);
						
						pTextureData->pTexture->Activate();
						
						TextureType = CGLESTexture::GetGLESTextureType( pTextureData->pTexture->GetTextureType() );					
						//glBindTexture( TextureType, pTextureData->pTexture->GetTextureHandle() );
						if(pTextureData->MagFilter)
							glTexParameteri(TextureType, GL_TEXTURE_MAG_FILTER, s_FILTER[pTextureData->MagFilter - 1]);
						
						if(pTextureData->MinFilter)
						{
							if(pTextureData->MipFilter)
								glTexParameteri(TextureType, GL_TEXTURE_MIN_FILTER, s_FILTER[pTextureData->MipFilter << 1 | (pTextureData->MinFilter - 1)]);
							else
								glTexParameteri(TextureType, GL_TEXTURE_MIN_FILTER, s_FILTER[pTextureData->MinFilter - 1]);
						}
						
						glTexParameteri(TextureType, GL_TEXTURE_WRAP_S, s_WRAP[pTextureData->WrapS]);
						glTexParameteri(TextureType, GL_TEXTURE_WRAP_T, s_WRAP[pTextureData->WrapT]);
					}
				}
			}
			
			m_uTextureFlag = 0;
		}
		
		if(m_pVertexDeclaration)
		{
			UINT8 uVertexAttribeFlag = 0;
			const UINT* puElementEndOfStream = m_pVertexDeclaration->GetElementEndOfStream();
			const CVertexDeclaration::VERTEXELEMENT* pVertexElement = m_pVertexDeclaration->GetVertexElements(); 
			UINT uVertexElementCount = m_pVertexDeclaration->GetVertexElementCount();
			CGLESVertexBuffer* pVertexBuffer = m_ppVertexBuffers[pVertexElement->uStream];
			if(pVertexBuffer)
				pVertexBuffer->Activate();
				//glBindBuffer( GL_ARRAY_BUFFER, pVertexBuffer->GetBufferHandle() );

			for(i = 0; i < uVertexElementCount; ++ i)
			{
				ALCHEMY_SET_BIT(uVertexAttribeFlag, pVertexElement->uIndex);
				
				glEnableVertexAttribArray(pVertexElement->uIndex);
				glVertexAttribPointer( pVertexElement->uIndex, 
									   pVertexElement->Type / sizeof(FLOAT), 
									   GL_FLOAT, GL_FALSE, 
									   pVertexBuffer->GetStride(), 
									   reinterpret_cast<const GLvoid*>(pVertexElement->uOffset) );
				
				++ pVertexElement;
				if(i == *puElementEndOfStream && (i + 1) != uVertexElementCount)
				{
					pVertexBuffer = m_ppVertexBuffers[pVertexElement->uStream];
					if(pVertexBuffer)
						pVertexBuffer->Activate();
						//glBindBuffer( GL_ARRAY_BUFFER, pVertexBuffer->GetBufferHandle() );
					
					++ puElementEndOfStream;
				}
			}
			
			ALCHEMY_CLEAR_FLAG(m_uVertexAttribeFlag, uVertexAttribeFlag);
			if(m_uVertexAttribeFlag)
			{
				uBegin = ALCHEMY_MININUM_BIT(m_uVertexAttribeFlag);
				uEnd = ALCHEMY_MAXINUM_BIT(m_uVertexAttribeFlag);
				for(i = uBegin - 1; i < uEnd; ++ i)
				{
					if( ALCHEMY_TEST_BIT(m_uVertexAttribeFlag, i) )
						glDisableVertexAttribArray(i);
				}
			}
			
			m_uVertexAttribeFlag = uVertexAttribeFlag;
		}
		else 
		{
			ALCHEMY_DEBUG_WARNING("CGLESContext::DrawIndexedTriangles: Vertex declaration has not been set.");
			
			return false;
		}
		
		static const GLenum s_TRINGALE_TYPE[] = 
		{
			GL_TRIANGLES, 
			GL_TRIANGLE_STRIP,
			GL_TRIANGLE_FAN
		};
		
        /*GLenum TriangleType;
        switch (Type) {
            case CIndexBuffer::TRIANGLE_STRIP:
                TriangleType = GL_TRIANGLE_STRIP;
                break;
            case CIndexBuffer::TRIANGLE_FAN:
                TriangleType = GL_TRIANGLE_FAN;
                break;
            default:
                TriangleType = GL_TRIANGLES;
                break;
        }*/
		
		//m_pIndexBuffer->Activate();
		//glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_pIndexBuffer->GetBufferHandle() );
		
        glDrawElements(s_TRINGALE_TYPE[Type], 
					   CIndexBuffer::GetIndexCount(uTriangleCount, Type), 
					   m_pIndexBuffer->GetFormat() == CIndexBuffer::BIT32 ? 
					   GL_UNSIGNED_INT : GL_UNSIGNED_SHORT,
					   m_pIndexBuffer->Activate(uStartIndex) );

        return true;
    }
	else 
		ALCHEMY_DEBUG_WARNING("CGLESContext::DrawIndexedTriangles: Index buffer has not been set.");
    
    return false;
}

CGraphicsResource* CGLESContext::_Create(CGraphicsResource::TYPE Type)
{
	CGraphicsResource* pResource = ALCHEMY_NULL;
	switch (Type)
	{
		case CGraphicsResource::VERTEX_DECLARATION:
			ALCHEMY_DEBUG_NEW(pResource, CGLESVertexDeclaration);
			break;
			
		case CGraphicsResource::VERTEX_BUFFER:
			ALCHEMY_DEBUG_NEW(pResource, CGLESVertexBuffer);
			break;
			
		case CGraphicsResource::INDEX_BUFFER:
			ALCHEMY_DEBUG_NEW(pResource, CGLESIndexBuffer);
			break;
			
		case CGraphicsResource::SHADER:
			ALCHEMY_DEBUG_NEW(pResource, CGLESShader);
			break;
			
		case CGraphicsResource::EFFECT:
			ALCHEMY_DEBUG_NEW(pResource, CGLESEffect);
			break;
			
		case CGraphicsResource::TEXTURE:
			ALCHEMY_DEBUG_NEW(pResource, CGLESTexture);
			break;
			
		default:
			break;
	}
	
	return pResource;
}

bool CGLESContext::__UseProgram()
{
	if(!m_uUserProgram)
	{
		if(!m_uProgram)
		{
			ALCHEMY_DEBUG_WARNING("CGLESContext::__UseProgram: Shader has not been set.");
			
			return false;
		}
		
		glLinkProgram(m_uProgram);
		GLint nLinkSuccess;
		glGetProgramiv(m_uProgram, GL_LINK_STATUS, &nLinkSuccess);
		if(nLinkSuccess == GL_FALSE) 
		{
			GLint uInfoLogLength = 0;
			glGetProgramiv(m_uProgram, GL_INFO_LOG_LENGTH, &uInfoLogLength);
			GLchar* pcMessages;
			ALCHEMY_DEBUG_NEW(pcMessages, GLchar[uInfoLogLength]);
			
			glGetProgramInfoLog(m_uProgram, uInfoLogLength, ALCHEMY_NULL, pcMessages);
			
			ALCHEMY_DEBUG_WARNING(pcMessages);
			
			ALCHEMY_DEBUG_DELETE_ARRAY(pcMessages);
			
			return false;
		}
		
		m_uUserProgram = m_uProgram;
		
		glUseProgram(m_uUserProgram);
		
		return true;
	}
	
	return true;
}

bool CGLESContext::__SetRenderState(CRenderSystem::RENDERSTATE State, UINT uValue)
{
	switch (State) 
	{
		case FILL_MODE:
			break;
			
		case SHADE_MODE:
			break;
			
		case DEPTH_WRITEE_ENABLE:
			glDepthMask(uValue ? GL_TRUE : GL_FALSE);
			break;
			
		case DEPTH_FUNC:
			glDepthFunc(sm_CMP_FUNC[uValue]);
			break;
			
		case SRC_BLEND:
			break;
			
		case DEST_BLEND:
			break;
			
		case ALPHA_REF:
			break;
			
		case ALPHA_FUNC:
			break;
			
		case CULL_MODE:
			if(uValue)
			{
				glFrontFace(GL_CW);
				glEnable(GL_CULL_FACE);
				glCullFace(sm_CULL_MODE[uValue]);
			}
			else
				glDisable(GL_CULL_FACE);
			break;
			
		case STENCIL_FAIL:
			break;
			
		case STENCIL_Z_FAIL:
			break;
			
		case STENCIL_PASS:
			break;
			
		case STENCIL_FUNC:
			break;
			
		case STENCIL_REF:
			break;
			
		case STENCIL_READ_MASK:
			break;
			
		case STENCIL_WRITE_MASK:
			break;
			
		case CCW_STENCIL_FAIL:
			break;
			
		case CCW_STENCIL_Z_FAIL:
			break;
			
		case CCW_STENCIL_PASS:
			break;
			
		case CCW_STENCIL_FUNC:
			break;
			
			
		case COLOR_WRITE_ENABLE:
			glColorMask( ALCHEMY_TEST_BIT(uValue, CRenderSystem::RED), ALCHEMY_TEST_BIT(uValue, CRenderSystem::GREEN), ALCHEMY_TEST_BIT(uValue, CRenderSystem::BLUE), ALCHEMY_TEST_BIT(uValue, CRenderSystem::ALPHA) );
			break;
			
		default:
			break;
	}
	
    return true;
}