/*
 *  AlchemyGLESShader.cpp
 *  IOS
 *
 *  Created by lin fuqing on 12-5-2.
 *  Copyright 2012 AstepGame. All rights reserved.
 *
 */

#include "AlchemyGLESShader.h"

using namespace alchemy;

CGLESShader::CGLESShader() :
m_uShaderHandle(0)
{
}

CGLESShader::~CGLESShader()
{
}

void CGLESShader::Destroy()
{
	if(m_uShaderHandle)
	{
		if(GetShaderType() == IEffect::PROGRAM)
		{
			GLint uShaderHandle;
			glGetProgramiv(m_uShaderHandle, GL_VERTEX_SHADER  , &uShaderHandle);
			glDeleteShader(uShaderHandle);
			glGetProgramiv(m_uShaderHandle, GL_FRAGMENT_SHADER, &uShaderHandle);
			glDeleteShader(uShaderHandle);
			
			glDeleteProgram(m_uShaderHandle);
		}
		else
			glDeleteShader(m_uShaderHandle);
	}
}
#include <stdio.h>
bool CGLESShader::_Create()
{
	if(m_pCode)
	{
		IEffect::EFFECTTYPE Type = GetShaderType();
		if(Type == IEffect::PROGRAM && m_pAdditionalCode)
		{
			m_uShaderHandle = glCreateProgram();
			
			GLint nShaderSourceLength, nSuccess;
			GLuint uShaderHandle;
			const GLchar* pcCode;
			
			uShaderHandle = glCreateShader(GL_VERTEX_SHADER);
			
			pcCode = static_cast<const GLchar*>( m_pCode->GetBufferPointer() );
			nShaderSourceLength = m_pCode->GetBufferSize() / sizeof(CHAR);
			glShaderSource(uShaderHandle, 1, &pcCode, &nShaderSourceLength);
			
			glCompileShader(uShaderHandle);
			
			glGetShaderiv(uShaderHandle, GL_COMPILE_STATUS, &nSuccess);
			if (nSuccess == GL_FALSE) 
			{
				GLint uInfoLogLength = 0;
				glGetShaderiv(uShaderHandle, GL_INFO_LOG_LENGTH, &uInfoLogLength);
				GLchar* pcMessages;
				ALCHEMY_DEBUG_NEW(pcMessages, GLchar[uInfoLogLength]);
				
				glGetShaderInfoLog(uShaderHandle, uInfoLogLength, ALCHEMY_NULL, pcMessages);
				
				ALCHEMY_DEBUG_WARNING(pcMessages);
				
				printf(pcMessages);
				
				ALCHEMY_DEBUG_DELETE_ARRAY(pcMessages);
				
				return false;
			}
			
			glAttachShader(m_uShaderHandle, uShaderHandle);
			
			uShaderHandle = glCreateShader(GL_FRAGMENT_SHADER);
			
			pcCode = static_cast<const GLchar*>( m_pAdditionalCode->GetBufferPointer() );
			nShaderSourceLength = m_pAdditionalCode->GetBufferSize() / sizeof(CHAR);
			glShaderSource(uShaderHandle, 1, &pcCode, &nShaderSourceLength);
			
			glCompileShader(uShaderHandle);
			
			glGetShaderiv(uShaderHandle, GL_COMPILE_STATUS, &nSuccess);
			if (nSuccess == GL_FALSE) 
			{
				GLint uInfoLogLength = 0;
				glGetShaderiv(uShaderHandle, GL_INFO_LOG_LENGTH, &uInfoLogLength);
				GLchar* pcMessages;
				ALCHEMY_DEBUG_NEW(pcMessages, GLchar[uInfoLogLength]);
				
				glGetShaderInfoLog(uShaderHandle, uInfoLogLength, ALCHEMY_NULL, pcMessages);
				
				ALCHEMY_DEBUG_WARNING(pcMessages);
				
				printf(pcMessages);
				
				ALCHEMY_DEBUG_DELETE_ARRAY(pcMessages);
				
				return false;
			}
			
			glAttachShader(m_uShaderHandle, uShaderHandle);
			
			glLinkProgram(m_uShaderHandle);
			glGetProgramiv(m_uShaderHandle, GL_LINK_STATUS, &nSuccess);
			if(nSuccess == GL_FALSE) 
			{
				GLint uInfoLogLength = 0;
				glGetProgramiv(m_uShaderHandle, GL_INFO_LOG_LENGTH, &uInfoLogLength);
				GLchar* pcMessages;
				ALCHEMY_DEBUG_NEW(pcMessages, GLchar[uInfoLogLength]);
				
				glGetProgramInfoLog(m_uShaderHandle, uInfoLogLength, ALCHEMY_NULL, pcMessages);
				
				ALCHEMY_DEBUG_WARNING(pcMessages);
				
				printf(pcMessages);
				
				ALCHEMY_DEBUG_DELETE_ARRAY(pcMessages);
				
				return false;
			}
		}
		else if(Type == IEffect::VERTEX_SHADER)
		{
			m_uShaderHandle = glCreateShader(GL_VERTEX_SHADER);
			
			const GLchar* pcCode = static_cast<const GLchar*>( m_pCode->GetBufferPointer() );
			GLint nShaderSourceLength = m_pCode->GetBufferSize() / sizeof(CHAR);
			glShaderSource(m_uShaderHandle, 1, &pcCode, &nShaderSourceLength);
			
			glCompileShader(m_uShaderHandle);
			
			GLint nCompileSuccess;
			glGetShaderiv(m_uShaderHandle, GL_COMPILE_STATUS, &nCompileSuccess);
			if(nCompileSuccess == GL_FALSE) 
			{
				GLint uInfoLogLength = 0;
				glGetShaderiv(m_uShaderHandle, GL_INFO_LOG_LENGTH, &uInfoLogLength);
				GLchar* pcMessages;
				ALCHEMY_DEBUG_NEW(pcMessages, GLchar[uInfoLogLength]);
				
				glGetShaderInfoLog(m_uShaderHandle, uInfoLogLength, ALCHEMY_NULL, pcMessages);
				
				ALCHEMY_DEBUG_WARNING(pcMessages);
				
				ALCHEMY_DEBUG_DELETE_ARRAY(pcMessages);
				
				return false;
			}
		}
		else if(Type == IEffect::FRAGMENT_SHADER)
		{
			m_uShaderHandle = glCreateShader(GL_FRAGMENT_SHADER);
			
			const GLchar* pcCode = static_cast<const GLchar*>( m_pCode->GetBufferPointer() );
			GLint nShaderSourceLength = m_pCode->GetBufferSize() / sizeof(CHAR);
			glShaderSource(m_uShaderHandle, 1, &pcCode, &nShaderSourceLength);
			
			glCompileShader(m_uShaderHandle);
			
			GLint nCompileSuccess;
			glGetShaderiv(m_uShaderHandle, GL_COMPILE_STATUS, &nCompileSuccess);
			if(nCompileSuccess == GL_FALSE) 
			{
				GLint uInfoLogLength = 0;
				glGetShaderiv(m_uShaderHandle, GL_INFO_LOG_LENGTH, &uInfoLogLength);
				GLchar* pcMessages;
				ALCHEMY_DEBUG_NEW(pcMessages, GLchar[uInfoLogLength]);
				
				glGetShaderInfoLog(m_uShaderHandle, uInfoLogLength, ALCHEMY_NULL, pcMessages);
				
				ALCHEMY_DEBUG_WARNING(pcMessages);
				
				ALCHEMY_DEBUG_DELETE_ARRAY(pcMessages);
				
				return false;
			}
		}
	
		return true;
	}
	
	ALCHEMY_DEBUG_WARNING("CGLESShader::_Create: Error code.");
	
	return false;
}