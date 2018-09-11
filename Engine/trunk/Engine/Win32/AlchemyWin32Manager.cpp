#include "AlchemyWin32Manager.h"

#include "AlchemyWin32Window.h"

#include "../Direct3D9/AlchemyD3D9Device.h"

using namespace alchemy;

void alchemy::BeginPlatform()
{
	CWin32Manager* pWin32Manager;
	ALCHEMY_DEBUG_NEW(pWin32Manager, CWin32Manager);
}

void alchemy::EndPlatform()
{
	CWin32Manager* pWin32Manager = static_cast<CWin32Manager*>(&ALCHEMY_PLATFORM_MANAGER);
	ALCHEMY_DEBUG_DELETE(pWin32Manager);
}

CWin32Manager::CWin32Manager(void)
{
}

CWin32Manager::~CWin32Manager(void)
{
	CRenderSystem* pRenderSystem;
	CVector<CRenderSystem*>::CIterator Iterator = m_RenderSystemList.GetIterator(0);
	while ( CVector<CRenderSystem*>::IsValuable(Iterator) )
	{
		pRenderSystem = *Iterator;

		ALCHEMY_DEBUG_DELETE(pRenderSystem);

		++ Iterator;
	}
}


void CWin32Manager::CreateRenderSystem(alchemy::UINT uWindowHandle, alchemy::UINT uBackBufferWidth, alchemy::UINT uBackBufferHeight, alchemy::INT nAntiAliasing, alchemy::UINT uType)
{
	IWindow* pWindow = GetWindow(uWindowHandle);
	if(pWindow)
	{
		CWin32Window* pWin32Window = static_cast<CWin32Window*>(pWindow);

		CD3D9Device* pDevice;
		ALCHEMY_DEBUG_NEW( pDevice, CD3D9Device(pWin32Window->GetWindowHandle(), pWin32Window->GetViewportWidth() || pWin32Window->GetViewportHeight(), uBackBufferWidth, uBackBufferHeight, nAntiAliasing) );

		m_RenderSystemList.Add(pDevice);

		UINT uType;
		if( ALCHEMY_GET_EVENT_TYPE(uType, RENDER_SYSTEM_CREATED, CPlatformManager) )
		{
			CPlatformManager::CRenderSystemEvent Event(*pDevice, uType);

			DispatchEvent(Event);
		}
	}
}

alchemy::INT CWin32Manager::MainLoop(CPlatformManager::IPlatformCreator* pCreator)
{
	if(pCreator)
	{
		if( !pCreator->Handler() )
			return 0;
	}

	HACCEL hAccelTable = LoadAccelerators(GetModuleHandle(ALCHEMY_NULL), ALCHEMY_NULL);

	CPool<IWindow*>::CIterator Iterator;
	IWindow* pWindow;
	CWin32Window* pWin32Window;

	alchemy::FLOAT fTime, fCurrentTime;

	MSG msg;
	while(true)
	{
		if ( PeekMessage(&msg, ALCHEMY_NULL, 0, 0, PM_REMOVE) && !TranslateAccelerator(msg.hwnd, hAccelTable, &msg) )
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if(msg.message == WM_QUIT)
				break;
		}
		else
		{
			fTime = static_cast<FLOAT>( timeGetTime() );

			Iterator = _GetWindowIterator(0);
			while ( CVector<IWindow*>::IsValuable(Iterator) )
			{
				pWindow = *Iterator;
				if( pWindow && ( pWin32Window = static_cast<CWin32Window*>(pWindow) )->IsLoop() )
				{
					fCurrentTime = static_cast<FLOAT>( timeGetTime() );

					pWin32Window->RunLoop(fTime, fCurrentTime - fTime);
				}

				++ Iterator;
			}
		}
	}

	return static_cast<INT>(msg.wParam);
}

IWindow* CWin32Manager::_CreateWindow(alchemy::FLOAT x, alchemy::FLOAT y, alchemy::FLOAT fWidth, alchemy::FLOAT fHeight)
{
	CWin32Window* pWindow;
	ALCHEMY_DEBUG_NEW( pWindow, CWin32Window(x, y, fWidth, fHeight) );

	return pWindow;
}