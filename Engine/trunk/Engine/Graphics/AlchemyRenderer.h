#pragma once

#include "AlchemySceneManager.h"

namespace alchemy
{
	class CRenderer
	{
	public:
		class CRenderListener :
			public CSmartClass<IEventDispatcher::IListener>
		{
		public:
			CRenderListener(CRenderer& Parent);
			~CRenderListener(void);

			void Handler(const IEventDispatcher::CEvent& Event);

		private:
			CRenderer& m_Parent;
		};

		CRenderer(void);
		~CRenderer(void);

		bool GetVisible() const;
		void SetVisible(bool bValue);

		virtual bool ApplyForRender() = 0;

	protected:
		CSceneManager* m_pSceneManager;

	private:
		CRenderListener m_Listener;
		bool m_bIsVisible;
	};

	ALCHEMY_INLINE bool CRenderer::GetVisible() const
	{
		return m_bIsVisible;
	}
}