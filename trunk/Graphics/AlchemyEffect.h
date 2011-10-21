#pragma once

//#include "../Math/AlchemyMatrix4x4.h"
//#include "AlchemyTexture.h"
#include "AlchemyEffectInterface.h"
#include "AlchemyEffectCode.h"

namespace alchemy
{
	class CRenderSystem;
	class CEffect :
		public CEffectInterface, public CGraphicsResource
	{
		friend class CRenderSystem;
	public:

		CEffect(void);
		~CEffect(void);

		UINT GetTotalPass();

		virtual bool CommitChange() = 0;

		virtual bool Begin() = 0;
		virtual bool BeginPass(UINT uIndex) = 0;
		virtual bool EndPass() = 0;
		virtual bool End() = 0;

		void Destroy();

	protected:
		UINT m_uPassCount;

		CEffectCode* m_pEffectCode;

		STRING* m_pCode;
		CBuffer** m_ppCompilationErrors;
	};

	ALCHEMY_INLINE UINT CEffect::GetTotalPass()
	{
		return m_uPassCount;
	}
}