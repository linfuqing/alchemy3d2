// TestWin32.cpp : ����Ӧ�ó������ڵ㡣
//
#include "stdafx.h"
#include "TestWin32.h"

#define MAX_LOADSTRING 100

// ȫ�ֱ���:
HINSTANCE hInst;								// ��ǰʵ��
TCHAR szTitle[MAX_LOADSTRING];					// �������ı�
TCHAR szWindowClass[MAX_LOADSTRING];			// ����������

// �˴���ģ���а����ĺ�����ǰ������:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

alchemy::CRenderSystem* g_pRenderSystem = NULL;
alchemy::CSceneManager* g_pSceneManager = NULL;

alchemy::CKeyListener g_KeyListener;
alchemy::CAnimationController* g_pAnimationController;

class CRenderLoop :
	public alchemy::CSmartClass<alchemy::IWindow::IDisplayLink>
{
public:
	CRenderLoop() {}
	~CRenderLoop() {}

	void Run(FLOAT fTime, FLOAT fElapseTime)
	{
		if(g_pRenderSystem && g_pSceneManager)
		{
			//g_pRenderSystem->Clear( 0xffff0000, ALCHEMY_FLAG(alchemy::CRenderSystem::TARGET) | ALCHEMY_FLAG(alchemy::CRenderSystem::ZBUFFER) | ALCHEMY_FLAG(alchemy::CRenderSystem::STENCIL) );

			alchemy::CCamera& Camera = g_pSceneManager->GetSceneCamera();

			//static FLOAT z = 0;
			/*g_pSceneManager->GetSceneCamera().Right(0.01f);

			alchemy::CVector3 Position = g_pSceneManager->GetSceneCamera().GetTranslation();
			Position.Normalize();
			Position.Negate();
			g_pSceneManager->GetSceneCamera().SetDirection(Position);*/

			const alchemy::FLOAT3& Position = Camera.GetTranslation();

			if( g_KeyListener.CheckKeyDown(alchemy::IWindow::KEY_W) )
				Camera.SetTranslation(Position.x, Position.y, Position.z + 1.0f);

			if( g_KeyListener.CheckKeyDown(alchemy::IWindow::KEY_S) )
				Camera.SetTranslation(Position.x, Position.y, Position.z - 1.0f);

			if( g_KeyListener.CheckKeyDown(alchemy::IWindow::KEY_A) )
				Camera.SetTranslation(Position.x - 1.0f, Position.y, Position.z);

			if( g_KeyListener.CheckKeyDown(alchemy::IWindow::KEY_D) )
				Camera.SetTranslation(Position.x + 1.0f, Position.y, Position.z);

			if(g_pAnimationController)
				g_pAnimationController->Update(0.001f);

			g_pSceneManager->Update(fElapseTime);
			g_pSceneManager->Render();

			g_pRenderSystem->Present();
		}
	}
};

CRenderLoop g_RenderLoop;

alchemy::CRectangleInstance g_RectangleInstance;
alchemy::CDAEParser* g_pPaser;

class CRenderSystemCreatedListener : 
	public alchemy::CSmartClass<alchemy::IEventDispatcher::IListener>
{
public:
	CRenderSystemCreatedListener()
	{

	}

	~CRenderSystemCreatedListener()
	{

	}

	void Handler(const alchemy::IEventDispatcher::CEvent& Event)
	{
		g_pRenderSystem = &static_cast<const alchemy::CPlatformManager::CRenderSystemEvent&>(Event).GetRenderSystem();

		return;

		ALCHEMY_DEBUG_NEW( g_pSceneManager, alchemy::CSceneManager(*g_pRenderSystem) );
		if(g_pSceneManager)
		{
			if(g_pPaser)
			{
				alchemy::CVector<alchemy::CSceneObject*> SceneObjects;
				alchemy::CVector<alchemy::CBone*> Bones;
				alchemy::CSceneNode* pSceneNode = g_pSceneManager->CreateScene(
					alchemy::ALCHEMY_MESH_MANAGER.AddSceneData( g_pPaser->GetSceneData() ), alchemy::CSkinMeshController::SKIN_SOFTWARE, &SceneObjects, &Bones, NULL, NULL, NULL);
				if(pSceneNode)
				{
					//pSceneNode->SetScale(10.0f, 10.0f, 10.0f);

					g_pSceneManager->AddChild(*pSceneNode);

					alchemy::CEffectCode Code( g_pRenderSystem->GetCompiler() );

					alchemy::CEffectCode::CPassDefiner PassDefiner(Code);

					alchemy::CHardwareSkinEffect HardwareSkinEffect;
					alchemy::CPositionEffect PositionEffect;
					alchemy::CTextureEffect TextureEffect;
					//alchemy::CFloat4 Color(0.0, 1.0, 0.0, 1.0);
					//alchemy::CPixelColorEffect PixelColorEffect(&Color);
					alchemy::CEmptyEffect EmptyEffect;
					alchemy::CAlphaTestBlendMethod AlphaTestBlendMethod;

					/*alchemy::CEffectCode::ELEMENTDECLARATION ElementDeclaration[] = 
					{
						{
							alchemy::CVertexDeclaration::FLOAT4, 
								alchemy::CVertexDeclaration::POSITION, 
								0
						}, 

						{
							alchemy::CVertexDeclaration::FLOAT3, 
								alchemy::CVertexDeclaration::NORMAL, 
								0
						}
					};*/

					//HardwareSkinEffect.Create(16, 4, ElementDeclaration, 1);
					//PassDefiner.AddInfo(HardwareSkinEffect, NULL);
					PassDefiner.AddInfo(PositionEffect, NULL);
					PassDefiner.AddInfo(TextureEffect, NULL);
					//PassDefiner.AddInfo(PixelColorEffect, NULL);
					//PassDefiner.AddInfo(EmptyEffect, &AlphaTestBlendMethod);
					PassDefiner.EndDefine();

					alchemy::CEffect* pEffect = g_pRenderSystem->CreateEffect(Code);
					alchemy::CRenderMethod* pRenderMethod = g_pSceneManager->CreateRenderMethod(Code);

					alchemy::CSceneObject* pSceneObject;
					alchemy::CVector<alchemy::CSceneObject*>::CIterator SceneObjectIterator = SceneObjects.GetIterator(0);
					while( alchemy::CVector<alchemy::CSceneObject*>::IsValuable(SceneObjectIterator) )
					{
						pSceneObject = *SceneObjectIterator;
						if(pSceneObject)
						{
							pSceneObject->SetEffect( pEffect->GetResourceHandle() );
							pSceneObject->SetRenderMethod(pRenderMethod);
						}

						++ SceneObjectIterator;
					}

					g_pAnimationController = g_pPaser->GetAnimationController();
					if(g_pAnimationController)
					{
						alchemy::CBone* pBone;
						alchemy::CVector<alchemy::CBone*>::CIterator BoneIterator = Bones.GetIterator(0);
						while( alchemy::CVector<alchemy::CBone*>::IsValuable(BoneIterator) )
						{
							pBone = *BoneIterator;
							if(pBone)
							{
								pBone->RegisterAnimationController(*g_pAnimationController);

								break;
							}

							++ BoneIterator;
						}
					}
				}

				//delete g_pPaser;
			}

			alchemy::CEffectCode Code( g_pRenderSystem->GetCompiler() );

			alchemy::CEffectCode::CPassDefiner PassDefiner(Code);

			alchemy::CInstancePosition2Scale2Effect InstancePosition2Scale2Effect(10);
			alchemy::CTextureEffect TextureEffect;
			PassDefiner.AddInfo(InstancePosition2Scale2Effect, NULL);
			PassDefiner.AddInfo(TextureEffect, NULL);
			//PassDefiner.AddInfo(PixelColorEffect, NULL);
			//PassDefiner.AddInfo(EmptyEffect, &AlphaTestBlendMethod);
			PassDefiner.EndDefine();

			alchemy::CEffect* pEffect = g_pRenderSystem->CreateEffect(Code);
			alchemy::CRenderMethod* pRenderMethod = g_pSceneManager->CreateRenderMethod(Code);

			g_RectangleInstance.Create(100.0f, 100.0f, alchemy::CRectangleInstance::NORMAL, pEffect->GetResourceHandle(), pRenderMethod, 1);

			alchemy::CFloat2 Scale(1.0f, 1.0f);
			alchemy::CRectangleInstance::CSprite* pSprite, * pChild = NULL;
			for(UINT i = 0; i < 10; ++ i)
			{
				pSprite = &g_RectangleInstance.CreateInstance();
				pSprite->SetPosition(i * 50.0f, 0.0f);
				pSprite->SetTransform(Scale.fValue);

				/*if(pChild)
					pSprite->AddChild(*pChild);*/

				pChild = pSprite;
			}

			pSprite = g_RectangleInstance.Test(60.0f, 1.0f);

			g_pSceneManager->AddChild(g_RectangleInstance);

			g_pSceneManager->SetBackgroundColor(0xffff0000);
			g_pSceneManager->GetSceneCamera().SetProjection(45, 1.0, 3000.0, 1);

			g_pSceneManager->GetSceneCamera().SetTranslation(0.0, 0.0, - 20);
		}
	}
};

CRenderSystemCreatedListener g_RenderSystemCreatedListener;

class CLoaderListener : 
	public alchemy::CSmartClass<alchemy::IEventDispatcher::IListener>
{
public:
	CLoaderListener()
	{

	}

	~CLoaderListener()
	{

	}

	void Handler(const alchemy::IEventDispatcher::CEvent& Event)
	{
#undef CreateWindow

		UINT uWindow = alchemy::ALCHEMY_PLATFORM_MANAGER.CreateWindow(0.0, 0.0, 800.0, 640.0);
		alchemy::IWindow* pWindow = alchemy::ALCHEMY_PLATFORM_MANAGER.GetWindow(uWindow);

		if(pWindow)
		{
			pWindow->AttachToScreen(true);

			pWindow->AddRunLoop(g_RenderLoop);

			g_KeyListener.AddEventListenerTo(*pWindow);
		}

		const alchemy::CLoader::CLoadAllEvent& LoadAllEvent = static_cast<const alchemy::CLoader::CLoadAllEvent&>(Event);
		alchemy::IEventDispatcher* pEventDispatcher = LoadAllEvent.GetTarget();
		if(pEventDispatcher)
		{
			const UINT* puDataIndices = LoadAllEvent.GetDataIndices();
			UINT uDataCount = LoadAllEvent.GetDataCount() - 1;
			const alchemy::CLoader::DATA* pData;
			alchemy::CMeshManager::TEXTUREDATA TextureData;
			for(UINT i = 0; i < uDataCount; ++ i)
			{
				pData = alchemy::ALCHEMY_LOADER.GetData(puDataIndices[i]);
				if(pData)
				{
					TextureData.pData = pData->pData;
					TextureData.uWidth = pData->uAlpha;
					TextureData.uHeight = pData->uBeta;
					TextureData.Format = alchemy::CTexture::DEFAULT;
					TextureData.uUsage = 0;
					TextureData.uMipLevel = 0;
					TextureData.pcFileName = NULL;

					alchemy::ALCHEMY_MESH_MANAGER.AddTextureData(TextureData);
				}
			}

			pData = alchemy::ALCHEMY_LOADER.GetData(puDataIndices[uDataCount]);
			if(pData)
			{
				alchemy::CDAEParser::TEXTUREDATARES TextureDataRes[] = 
				{
					/*{ALCHEMY_TEXT("Map__36-image")       , 1}, 
					{ALCHEMY_TEXT("Map__36_ncl1_1-image"), 2}, 
					{ALCHEMY_TEXT("Map__38-image")       , 3}, 
					{ALCHEMY_TEXT("Map__48-image")       , 4}, 
					{ALCHEMY_TEXT("Map__39-image")       , 5}, 
					{ALCHEMY_TEXT("Map__37-image")       , 6}, */
					/*{ALCHEMY_TEXT("Map__23-image")       , 1}, 
					{ALCHEMY_TEXT("Map__19-image")       , 2}, 
					{ALCHEMY_TEXT("Map__21-image")       , 3}, 
					{ALCHEMY_TEXT("Map__20-image")       , 4}, 
					{ALCHEMY_TEXT("Map__22-image")       , 5}, 
					{ALCHEMY_TEXT("Map__27-image")       , 6}, */
					{ALCHEMY_TEXT("Map__284-image")       , 1}
				};

				alchemy::CDAEParser::STREAMUSAGE StreamUsage[] = 
				{
					{alchemy::CVertexDeclaration::FLOAT4, alchemy::CVertexDeclaration::POSITION, 0, 0},
					{alchemy::CVertexDeclaration::FLOAT3, alchemy::CVertexDeclaration::NORMAL, 0, 0},
				};

				ALCHEMY_DEBUG_NEW( g_pPaser, alchemy::CDAEParser(TextureDataRes, sizeof(TextureDataRes) / sizeof(alchemy::CDAEParser::TEXTUREDATARES), StreamUsage, sizeof(StreamUsage) / sizeof(alchemy::CDAEParser::STREAMUSAGE), 0) );
				g_pPaser->Parse(pData->pData);
			}
		}

		UINT uType;
		if( ALCHEMY_GET_EVENT_TYPE(uType, RENDER_SYSTEM_CREATED, alchemy::CPlatformManager) )
			alchemy::ALCHEMY_PLATFORM_MANAGER.AddEventListener(uType, g_RenderSystemCreatedListener);

		alchemy::ALCHEMY_PLATFORM_MANAGER.CreateRenderSystem(uWindow, 0, 0, 0, 0);
	}
};

CLoaderListener g_LoaderListener;

class CCreator :
	public alchemy::CSmartClass<alchemy::CPlatformManager::IPlatformCreator>
{
public:
	CCreator() {}
	~CCreator() {}

	bool Handler()
	{
		UINT uType;
		if( ALCHEMY_GET_EVENT_TYPE(uType, LOAD_ALL_COMPLETE, alchemy::CLoader) )
			alchemy::ALCHEMY_LOADER.AddEventListener(uType, g_LoaderListener);

		static const alchemy::CHAR* pcPATH[] = 
		{
			/*ALCHEMY_TEXT("D:\\flash projects\\TR\\SourceCode\\TombRunner\\res\\data\\all_m_01.png"), 
			ALCHEMY_TEXT("D:\\flash projects\\TR\\SourceCode\\TombRunner\\res\\data\\all_m_02.png"), 
			ALCHEMY_TEXT("D:\\flash projects\\TR\\SourceCode\\TombRunner\\res\\data\\all_m_03.png"), 
			ALCHEMY_TEXT("D:\\flash projects\\TR\\SourceCode\\TombRunner\\res\\data\\all_m_04.png"), 
			ALCHEMY_TEXT("D:\\flash projects\\TR\\SourceCode\\TombRunner\\res\\data\\all_m_05.png"), 
			ALCHEMY_TEXT("D:\\flash projects\\TR\\SourceCode\\TombRunner\\res\\data\\all_m_06.png"), 
			ALCHEMY_TEXT("D:\\flash projects\\TR\\SourceCode\\TombRunner\\res\\data\\all.xml")*/

			/*ALCHEMY_TEXT("D:\\flash projects\\TR\\SourceCode\\TombRunner\\res\\data\\Walk_man_feet_diff.jpg"),
			ALCHEMY_TEXT("D:\\flash projects\\TR\\SourceCode\\TombRunner\\res\\data\\Walk_man_head_spec.jpg"),
			ALCHEMY_TEXT("D:\\flash projects\\TR\\SourceCode\\TombRunner\\res\\data\\Walk_man_leg_diff.jpg"),
			ALCHEMY_TEXT("D:\\flash projects\\TR\\SourceCode\\TombRunner\\res\\data\\Walk_man_body_spec.jpg"),
			ALCHEMY_TEXT("D:\\flash projects\\TR\\SourceCode\\TombRunner\\res\\data\\Walk_man_hand_diff.jpg"),
			ALCHEMY_TEXT("D:\\flash projects\\TR\\SourceCode\\TombRunner\\res\\data\\Walk_man_head_diff.jpg"),
			ALCHEMY_TEXT("D:\\flash projects\\TR\\SourceCode\\TombRunner\\res\\data\\all_ban_01.xml"),*/

			ALCHEMY_TEXT("D:\\CloudSpace\\ActivateCode\\Resource\\Test\\001.jpg"),
			ALCHEMY_TEXT("D:\\CloudSpace\\ActivateCode\\Resource\\Test\\f_DAE_a.xml")
		};

		alchemy::ALCHEMY_LOADER.LoadAll( pcPATH, sizeof(pcPATH) / sizeof(const alchemy::CHAR*) );

		return true;
	}
};

CCreator g_Creator;

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	/*hInstance = GetModuleHandle(NULL);
	hPrevInstance = NULL;
	lpCmdLine = GetCommandLine();
	nCmdShow = SW_SHOWNORMAL;*/

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: �ڴ˷��ô��롣
	/*MSG msg;
	HACCEL hAccelTable;

	// ��ʼ��ȫ���ַ���
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_TESTWIN32, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// ִ��Ӧ�ó����ʼ��:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TESTWIN32));

	// ����Ϣѭ��:
	while( PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) )
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;*/

	AllocConsole();

	long lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);  
	int hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);  
	FILE* fp = _fdopen( hConHandle, "w" ); 
	*stdout = *fp;  
	setvbuf( stdout, NULL, _IONBF, 0 );  // �� STDIN�ض���console  
	lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);  
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);  
	fp = _fdopen( hConHandle, "r" );  
	*stdin = *fp;  setvbuf( stdin, NULL, _IONBF, 0 );  //�� STDERR�ض���console  
	lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);  
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);  
	fp = _fdopen( hConHandle, "w" );  
	*stderr = *fp;  
	setvbuf( stderr, NULL, _IONBF, 0 );

	alchemy::CreatePlatformManager();

	return alchemy::ALCHEMY_PLATFORM_MANAGER.MainLoop(&g_Creator);
}



//
//  ����: MyRegisterClass()
//
//  Ŀ��: ע�ᴰ���ࡣ
//
//  ע��:
//
//    ����ϣ��
//    �˴�������ӵ� Windows 95 �еġ�RegisterClassEx��
//    ����֮ǰ�� Win32 ϵͳ����ʱ������Ҫ�˺��������÷������ô˺���ʮ����Ҫ��
//    ����Ӧ�ó���Ϳ��Ի�ù�����
//    ����ʽ��ȷ�ġ�Сͼ�ꡣ
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TESTWIN32));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_TESTWIN32);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   ����: InitInstance(HINSTANCE, int)
//
//   Ŀ��: ����ʵ�����������������
//
//   ע��:
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
/*BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}*/

//
//  ����: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��: ���������ڵ���Ϣ��
//
//  WM_COMMAND	- ����Ӧ�ó���˵�
//  WM_PAINT	- ����������
//  WM_DESTROY	- �����˳���Ϣ������
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// �����˵�ѡ��:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: �ڴ���������ͼ����...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// �����ڡ������Ϣ�������
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
