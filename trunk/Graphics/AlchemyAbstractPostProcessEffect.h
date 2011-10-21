#pragma once

#include "AlchemyCallBackToRenderInterface.h"
#include "AlchemyFullScreenDrawer.h"

namespace alchemy
{
	class CSceneManager;
	class CAbstractPostProcessEffect :
		public CCallBackToRenderInterface
	{
	public:
		CAbstractPostProcessEffect(void);
		~CAbstractPostProcessEffect(void);

		virtual bool ApplyForRender() = 0;
		virtual void Render(const CRenderQueue::RENDERENTRY& Entry, UINT uFlag) = 0;

	private:
		bool IsDynamic()const;
	};
}