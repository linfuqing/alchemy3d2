#pragma once

#include "AlchemyGLESShader.h"

#include "../Graphics/AlchemyEffect.h"

namespace alchemy
{
	class CGLESContext;
	class CGLESEffect:
	public CEffect
	{
		friend class CGLESContext;
	public:
		CGLESEffect();
		~CGLESEffect();
		
		bool GetShader(UINT uPass, UINT uRenderTargetIndex, CShader** ppOutputShader, CShader** ppOutputAdditionalShader);
		
		void Destroy();
		
	protected:
		bool _Create();
		
	private:
		CGLESShader** m_ppPass;
	};
}