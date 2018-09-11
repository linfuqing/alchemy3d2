#pragma once

#include <OpenGLES/ES2/gl.h>

#include "../Graphics/AlchemyGraphicsBuffer.h"


namespace alchemy
{
	class CGLESContext;
    class CGLESIndexBuffer :
    public INDEXBUFFER
    {
		friend class CGLESContext;
    public:
        CGLESIndexBuffer();
        ~CGLESIndexBuffer();
		
		GLuint GetBufferHandle()const;
		
		const void* Activate(UINT uOffset)const;
        
        void* Lock(UINT uStart, UINT uEnd, UINT uFlag);
		void* Lock(                        UINT uFlag);
		bool  Unlock(                                );
		
		void Destroy();
        
	protected:
		bool _Create();
		
    private:
        GLuint m_uBufferHandle;
        PUINT8 m_puData;
        UINT m_uLockStart;
        UINT m_uLockEnd;
        UINT m_uLockFlag;
    };
	
	ALCHEMY_INLINE GLuint CGLESIndexBuffer::GetBufferHandle()const
	{
		return m_uBufferHandle;
	}
}