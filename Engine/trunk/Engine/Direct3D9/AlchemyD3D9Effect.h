#pragma once

#include "AlchemyD3D9Shader.h"

#include "../Graphics/AlchemyEffect.h"

namespace alchemy
{
	class CD3D9Device;
	class CD3D9Effect : 
		public CEffect
	{
		friend class CD3D9Device;
	public:
		CD3D9Effect(void);
		~CD3D9Effect(void);

		void Destroy();

		bool GetShader(UINT uPass, UINT uRenderTargetIndex, CShader** ppOutputShader, CShader** ppOutputAdditionalShader);

	protected:
		typedef struct  
		{
			CD3D9Shader* pVertexShader;
			CD3D9Shader* pPixelShader;
		}PASS, * LPPASS;

		bool _Create();

		PASS* m_pPasses;

		UINT m_uPassCount;
	};
}