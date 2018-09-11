#pragma once

#include "AlchemyFlasccProgram3D.h"

#include "../Graphics/AlchemyEffect.h"

namespace alchemy
{
	class CFlasccEffect : 
		public CEffect
	{
		friend class CFlasccContext3D;
	public:
		CFlasccEffect(void);
		~CFlasccEffect(void);

		void Destroy();

		bool GetShader(UINT uPass, UINT uRenderTargetIndex, CShader** ppOutputShader, CShader** ppOutputAdditionalShader);

	protected:
		bool _Create();

	private:
		CFlasccProgram3D** m_ppPass;
	};
}