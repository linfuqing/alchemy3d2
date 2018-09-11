#pragma once

#include "AlchemyWindowInterface.h"

#include "../Graphics/AlchemyRenderSystem.h"

namespace alchemy
{
#define ALCHEMY_PLATFORM_MANAGER CPlatformManager::GetInstance()

	void BeginPlatform();
	void EndPlatform();

	class CPlatformManager : 
		public CSingleton<CPlatformManager>, public EVENTDISPATCHER
	{
	public:
		ALCHEMY_BEGIN_EVENT_ENUM
			RENDER_SYSTEM_CREATED
		ALCHEMY_END_EVENT_ENUM

		ALCHEMY_INTERFACE(IPlatformCreator)
		{
			virtual bool Handler() = 0;
		};

		class CRenderSystemEvent :
			public IEventDispatcher::CEvent
		{
		public:
			CRenderSystemEvent(CRenderSystem& RenderSystem, UINT uType, bool bIsBubbles = true);
			~CRenderSystemEvent();

			CRenderSystem& GetRenderSystem()const;
		private:
			CRenderSystem& m_RenderSystem;
		};

		CPlatformManager(void);
		~CPlatformManager(void);

		IWindow* GetWindow(UINT uHandle)const;
		UINT CreateWindow(FLOAT x, FLOAT y, FLOAT fWidth, FLOAT fHeight);

		virtual void CreateRenderSystem(UINT uWindowHandle, UINT uBackBufferWidth, UINT uBackBufferHeight, INT nAntiAliasing, UINT uType) = 0;

		virtual INT MainLoop(IPlatformCreator* pCreator) = 0;

	protected:
		virtual IWindow* _CreateWindow(FLOAT x, FLOAT y, FLOAT fWidth, FLOAT fHeight) = 0;

		CPool<IWindow*>::CIterator _GetWindowIterator(UINT uIndex);

	private:
		CPool<IWindow*> m_Windows;
	};

	ALCHEMY_INLINE CRenderSystem& CPlatformManager::CRenderSystemEvent::GetRenderSystem()const
	{
		return m_RenderSystem;
	}

	ALCHEMY_INLINE IWindow* CPlatformManager::GetWindow(UINT uHandle)const
	{
		IWindow* const* ppWindow = uHandle ? m_Windows.Get(uHandle - 1) : ALCHEMY_NULL;

		if(ppWindow)
			return *ppWindow;

		return ALCHEMY_NULL;
	}
}