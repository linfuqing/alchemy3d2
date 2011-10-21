#pragma once

#include "AlchemyAbstractPostProcessEffect.h"
#include "AlchemyFullScreenDrawer.h"

namespace alchemy
{
	class CSimplePostProcessEffect :
		public CAbstractPostProcessEffect
	{
	public:
		CSimplePostProcessEffect(void);
		~CSimplePostProcessEffect(void);

		void SetFullScreenDrawer(CFullScreenDrawer* pFullScreenDrawer);
		CFullScreenDrawer* GetFullScreenDrawer()const;

		virtual bool ApplyForRender();
		virtual void Render(const CRenderQueue::RENDERENTRY& Entry, UINT uFlag);

	protected:
		virtual CGraphicsResource::HANDLE GetEffectHandle() = 0;
		virtual void OnEffectUpdate( UINT uFlag, CGraphicsResource::HANDLE uSceneTarget) = 0;

	private:
		CFullScreenDrawer* m_pFullScreenDrawer;
	};

	ALCHEMY_INLINE void CSimplePostProcessEffect::SetFullScreenDrawer(CFullScreenDrawer* pFullScreenDrawer)
	{
		m_pFullScreenDrawer = pFullScreenDrawer;
	}

	ALCHEMY_INLINE CFullScreenDrawer* CSimplePostProcessEffect::GetFullScreenDrawer()const
	{
		return m_pFullScreenDrawer;
	}
}