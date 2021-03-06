#include "AlchemyWin32Window.h"

using namespace alchemy;

namespace alchemy
{
	typedef struct  
	{
		UINT uPointCount;
		CWin32Window* pWindow;
	}WIN32WINDOW, * LPWIN32WINDOW;

	CMap<HWND, WIN32WINDOW> g_WindowMap;

	LRESULT CALLBACK WndProc(HWND hWnd, alchemy::UINT message, WPARAM wParam, LPARAM lParam)
	{
		int wmId, wmEvent;
		PAINTSTRUCT ps;
		HDC hdc;

		LPWIN32WINDOW pWin32Window;

		switch (message)
		{
		case WM_COMMAND:
			wmId    = LOWORD(wParam);
			wmEvent = HIWORD(wParam);

			switch (wmId)
			{
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);

			EndPaint(hWnd, &ps);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_MOUSEMOVE:
			pWin32Window = g_WindowMap.Get(hWnd);
			if(pWin32Window && pWin32Window->pWindow && pWin32Window->uPointCount)
			{
				alchemy::UINT uType;
				if( ALCHEMY_GET_EVENT_TYPE(uType, POINT_MOVE, IWindow) )
				{
					FLOAT2 Point[2];
					FLOAT2& Source = Point[0];
					Source.x = LOWORD(lParam);
					Source.y = HIWORD(lParam);

					if(pWin32Window->uPointCount > 1)
					{
						FLOAT2& Destination = Point[1];
						Destination.x = 0.0f;
						Destination.y = 0.0f;
					}

					IWindow::CPointEvent Event(Point, pWin32Window->uPointCount, uType);

					pWin32Window->pWindow->DispatchEvent(Event);
				}
			}
			break;
		case WM_RBUTTONDOWN:
			pWin32Window = g_WindowMap.Get(hWnd);
			if(pWin32Window && pWin32Window->pWindow)
			{
				++ pWin32Window->uPointCount;

				alchemy::UINT uType;
				if( ALCHEMY_GET_EVENT_TYPE(uType, POINT_ON, IWindow) )
				{
					FLOAT2 Point;
					Point.x = 0.0f;
					Point.y = 0.0f;
					IWindow::CPointEvent Event(&Point, 1, uType);

					pWin32Window->pWindow->DispatchEvent(Event);
				}
			}
			break;
		case WM_RBUTTONUP:
			pWin32Window = g_WindowMap.Get(hWnd);
			if(pWin32Window && pWin32Window->pWindow)
			{
				-- pWin32Window->uPointCount;

				alchemy::UINT uType;
				if( ALCHEMY_GET_EVENT_TYPE(uType, POINT_OUT, IWindow) )
				{
					FLOAT2 Point;
					Point.x = 0.0f;
					Point.y = 0.0f;
					IWindow::CPointEvent Event(&Point, 1, uType);

					pWin32Window->pWindow->DispatchEvent(Event);
				}
			}
			break;
		case WM_LBUTTONDOWN:
			pWin32Window = g_WindowMap.Get(hWnd);
			if(pWin32Window && pWin32Window->pWindow)
			{
				++ pWin32Window->uPointCount;

				alchemy::UINT uType;
				if( ALCHEMY_GET_EVENT_TYPE(uType, POINT_ON, IWindow) )
				{
					FLOAT2 Point;
					Point.x = LOWORD(lParam);
					Point.y = HIWORD(lParam);
					IWindow::CPointEvent Event(&Point, 1, uType);

					pWin32Window->pWindow->DispatchEvent(Event);
				}
			}
			break;
		case WM_LBUTTONUP:
			pWin32Window = g_WindowMap.Get(hWnd);
			if(pWin32Window && pWin32Window->pWindow)
			{
				-- pWin32Window->uPointCount;

				alchemy::UINT uType;
				if( ALCHEMY_GET_EVENT_TYPE(uType, POINT_OUT, IWindow) )
				{
					FLOAT2 Point;
					Point.x = LOWORD(lParam);
					Point.y = HIWORD(lParam);
					IWindow::CPointEvent Event(&Point, 1, uType);

					pWin32Window->pWindow->DispatchEvent(Event);
				}
			}
			break;
		case WM_KEYDOWN:
			switch(wParam)
			{
			case VK_ESCAPE:
				PostQuitMessage(0);
				break;

			case 0x41:
			case 0x42:
			case 0x43:
			case 0x44:
			case 0x45:
			case 0x46:
			case 0x47:
			case 0x48:
			case 0x49:
			case 0x4A:
			case 0x4B:
			case 0x4C:
			case 0x4E:
			case 0x4F:
			case 0x50:
			case 0x51:
			case 0x52:
			case 0x53:
			case 0x54:
			case 0x55:
			case 0x56:
			case 0x57:
			case 0x58:
			case 0x59:
			case 0x5A:
				pWin32Window = g_WindowMap.Get(hWnd);
				if(pWin32Window && pWin32Window->pWindow)
				{
					alchemy::UINT uType;
					if( ALCHEMY_GET_EVENT_TYPE(uType, KEY_DOWN, IWindow) )
					{
						IWindow::CKeyEvent Event(static_cast<IWindow::KEYCODE>(wParam - 0x41 + IWindow::KEY_A), uType);
						
						pWin32Window->pWindow->DispatchEvent(Event);
					}
				}
				break;

			case VK_NUMPAD0:
			case VK_NUMPAD1:
			case VK_NUMPAD2:
			case VK_NUMPAD3:
			case VK_NUMPAD4:
			case VK_NUMPAD5:
			case VK_NUMPAD6:
			case VK_NUMPAD7:
			case VK_NUMPAD8:
			case VK_NUMPAD9:
				pWin32Window = g_WindowMap.Get(hWnd);
				if(pWin32Window && pWin32Window->pWindow)
				{
					alchemy::UINT uType;
					if( ALCHEMY_GET_EVENT_TYPE(uType, KEY_DOWN, IWindow) )
					{
						IWindow::CKeyEvent Event(static_cast<IWindow::KEYCODE>(wParam - VK_NUMPAD0 + IWindow::KEY_0), uType);

						pWin32Window->pWindow->DispatchEvent(Event);
					}
				}
				break;

			default:
				break;
			}
			break;

		case WM_KEYUP:
			switch(wParam)
			{
			case 0x41:
			case 0x42:
			case 0x43:
			case 0x44:
			case 0x45:
			case 0x46:
			case 0x47:
			case 0x48:
			case 0x49:
			case 0x4A:
			case 0x4B:
			case 0x4C:
			case 0x4E:
			case 0x4F:
			case 0x50:
			case 0x51:
			case 0x52:
			case 0x53:
			case 0x54:
			case 0x55:
			case 0x56:
			case 0x57:
			case 0x58:
			case 0x59:
			case 0x5A:
				pWin32Window = g_WindowMap.Get(hWnd);
				if(pWin32Window && pWin32Window->pWindow)
				{
					alchemy::UINT uType;
					if( ALCHEMY_GET_EVENT_TYPE(uType, KEY_UP, IWindow) )
					{
						IWindow::CKeyEvent Event(static_cast<IWindow::KEYCODE>(wParam - 0x41 + IWindow::KEY_A), uType);

						pWin32Window->pWindow->DispatchEvent(Event);
					}
				}
				break;

			case VK_NUMPAD0:
			case VK_NUMPAD1:
			case VK_NUMPAD2:
			case VK_NUMPAD3:
			case VK_NUMPAD4:
			case VK_NUMPAD5:
			case VK_NUMPAD6:
			case VK_NUMPAD7:
			case VK_NUMPAD8:
			case VK_NUMPAD9:
				pWin32Window = g_WindowMap.Get(hWnd);
				if(pWin32Window && pWin32Window->pWindow)
				{
					alchemy::UINT uType;
					if( ALCHEMY_GET_EVENT_TYPE(uType, KEY_UP, IWindow) )
					{
						IWindow::CKeyEvent Event(static_cast<IWindow::KEYCODE>(wParam - VK_NUMPAD0 + IWindow::KEY_0), uType);

						pWin32Window->pWindow->DispatchEvent(Event);
					}
				}
				break;

			default:
				break;
			}
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		return 0;
	}
}

CWin32Window::CWin32Window(alchemy::FLOAT fViewportX ,
						   alchemy::FLOAT fViewportY, 
						   alchemy::FLOAT fViewportWidth, 
						   alchemy::FLOAT fViewportHeight) :
m_pcName( ALCHEMY_TEXT("Alchemy3D") ), 
m_fViewportX(fViewportX), 
m_fViewportY(fViewportY), 
m_fViewportWidth(fViewportWidth), 
m_fViewportHeight(fViewportHeight)
{
	HINSTANCE hInstance = GetModuleHandle(ALCHEMY_NULL);

	WNDCLASSEX Class;

	Class.cbSize = sizeof(WNDCLASSEX);

	Class.style			= CS_HREDRAW | CS_VREDRAW;
	Class.lpfnWndProc	= WndProc;
	Class.cbClsExtra	= 0;
	Class.cbWndExtra	= 0;
	Class.hInstance		= hInstance;
	Class.hIcon			= LoadIcon(ALCHEMY_NULL, IDI_APPLICATION);
	Class.hCursor		= LoadCursor(ALCHEMY_NULL, IDC_ARROW);
	Class.hbrBackground	= reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
	Class.lpszMenuName	= ALCHEMY_NULL;
	Class.lpszClassName	= m_pcName;
	Class.hIconSm		= LoadIcon(ALCHEMY_NULL, IDI_WINLOGO);

	RegisterClassEx(&Class);

	m_hWindow = CreateWindow(
		m_pcName,
		m_pcName, 
		WS_OVERLAPPEDWINDOW, 
		static_cast<UINT>(m_fViewportX), 
		static_cast<UINT>(m_fViewportY), 
		static_cast<UINT>(m_fViewportWidth), 
		static_cast<UINT>(m_fViewportHeight), 
		ALCHEMY_NULL, 
		ALCHEMY_NULL, 
		hInstance, 
		ALCHEMY_NULL);

	WIN32WINDOW Win32Window;
	Win32Window.uPointCount = 0;
	Win32Window.pWindow = this;
	g_WindowMap.Insert(m_hWindow, Win32Window);

	bool bIsFullScreen = !m_fViewportWidth && !m_fViewportHeight;
	if(bIsFullScreen)
	{
		SetWindowLong(m_hWindow, GWL_STYLE, GetWindowLong(m_hWindow,GWL_STYLE) & ~WS_CAPTION & ~WS_SIZEBOX);
		SetWindowPos(m_hWindow, ALCHEMY_NULL, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_NOZORDER);
		ShowCursor(FALSE);
	}
}

CWin32Window::~CWin32Window(void)
{
}

bool CWin32Window::AttachToScreen(bool bIsAsKeyWindow)
{
	return m_hWindow && ( !bIsAsKeyWindow || ShowWindow(m_hWindow, SW_SHOWNORMAL) ) && UpdateWindow(m_hWindow);
}

bool CWin32Window::AddRunLoop(IDisplayLink& DisplayLink)
{
	DisplayLink.AddRef();

	m_DisplayLinks.Add(&DisplayLink);

	return true;
}

void CWin32Window::RunLoop(alchemy::FLOAT fTime, alchemy::FLOAT fElapseTime)
{
	CVector<IDisplayLink*>::CIterator Iterator = m_DisplayLinks.GetIterator(0);

	IDisplayLink* pDisplayLink;
	while( CVector<IDisplayLink*>::IsValuable(Iterator) )
	{
		pDisplayLink = *Iterator;
		if(pDisplayLink)
			pDisplayLink->Run(fTime, fElapseTime);

		++ Iterator;
	}
}