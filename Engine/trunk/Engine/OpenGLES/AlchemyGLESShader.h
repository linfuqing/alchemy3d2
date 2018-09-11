#pragma once
/*
 *  AlchemyGLESShader.h
 *  IOS
 *
 *  Created by lin fuqing on 12-5-2.
 *  Copyright 2012 AstepGame. All rights reserved.
 *
 */

#include "../Graphics/AlchemyShader.h"

#include <OpenGLES/ES2/gl.h>

namespace alchemy
{
	class CGLESContext;
	class CGLESShader :
	public CShader
	{
		friend class CGLESContext;
	public:
		CGLESShader();
		~CGLESShader();
		
		GLuint GetShaderHandle()const;
		
		void Destroy();
		
	protected:
		bool _Create();
		
	private:
		GLuint m_uShaderHandle;
	};
	
	ALCHEMY_INLINE GLuint CGLESShader::GetShaderHandle()const
	{
		return m_uShaderHandle;
	}
}