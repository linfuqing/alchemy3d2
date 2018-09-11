#include "GameMain.h"

//#include "Tower.h"

#include "../Core/AlchemyMTRandom.h"
#include "../Core/AlchemyConfiguration.h"

#include "../Graphics/AlchemySceneManager.h"

#ifdef ALCHEMY_WINDOW_OS
#include "../Win32/AlchemyWin32Manager.h"
#endif

using namespace alchemy;

namespace alchemy
{
	class CGameLoop : 
		public CSmartClass<IWindow::IDisplayLink>
	{
	public:
		CGameLoop(CGameMain& Parent) : m_Parent(Parent) {}
		~CGameLoop() {}

		void Run(FLOAT fTime, FLOAT fElapseTime)
		{
			if(m_Parent.m_pSceneManager)
			{
				m_Parent.m_pSceneManager->Update(fElapseTime);
				m_Parent.m_pSceneManager->Render();

				m_Parent.m_pSceneManager->GetRenderSystem().Present();
			}
		}

	private:
		CGameMain& m_Parent;
	};

	class CGameSceneCreator : 
		public CSmartClass<IEventDispatcher::IListener>
	{
	public:
		CGameSceneCreator(CGameMain& Parent) : m_Parent(Parent) {}
		~CGameSceneCreator() {}

		void Handler(const IEventDispatcher::CEvent& Event)
		{
			UINT uType;

			if( ALCHEMY_GET_EVENT_TYPE(uType, INPUT_EVENT, CInput) )
				m_Parent.m_Input.AddEventListener(uType, m_Parent.m_InputHandler);

			if( ALCHEMY_GET_EVENT_TYPE(uType, TRANSVERSE, CPointOutput) )
				m_Parent.m_PointOutput.AddEventListener(uType, m_Parent.m_TransversePointOutputHandler);

			if( ALCHEMY_GET_EVENT_TYPE(uType, VERTICAL, CPointOutput) )
				m_Parent.m_PointOutput.AddEventListener(uType, m_Parent.m_VerticalPointOutputHandler);

			if( ALCHEMY_GET_EVENT_TYPE(uType, SCALE, CPointOutput) )
				m_Parent.m_PointOutput.AddEventListener(uType, m_Parent.m_ScalePointOutputHandler);

			if( ALCHEMY_GET_EVENT_TYPE(uType, CLICK, CPointOutput) )
				m_Parent.m_PointOutput.AddEventListener(uType, m_Parent.m_ClickPointOutputHandler);

			m_Parent.m_PointOutput.SetFlag(~0);
			m_Parent.m_PointOutput.Run();

			m_Parent.m_Tower.RemoveEventListener(Event.GetType(), *this);
		}

	private:
		CGameMain& m_Parent;
	};

	class CEngineCreator : 
		public CSmartClass<IEventDispatcher::IListener>
	{
	public:
		CEngineCreator(CGameMain& Parent) : m_Parent(Parent) {}
		~CEngineCreator() {}

		void Handler(const IEventDispatcher::CEvent& Event)
		{
			CRenderSystem& RenderSystem = static_cast<const CPlatformManager::CRenderSystemEvent&>(Event).GetRenderSystem();

			ALCHEMY_DEBUG_NEW( m_Parent.m_pSceneManager, CSceneManager(RenderSystem) );

			if(m_Parent.m_pSceneManager)
			{
				m_Parent.m_pSceneManager->SetBackgroundColor(0xff000000);
				m_Parent.m_pSceneManager->GetSceneCamera().SetProjection(45.0f, 1.0f, 3000.0f, 1.0f);

				m_Parent.m_pSceneManager->GetSceneCamera().SetTranslation(0.0, 0.0, - 50.0f);

				UINT uType;
				if( ALCHEMY_GET_EVENT_TYPE(uType, TOWER_LOADED_COMPELTE, CTower) )
				{
					CGameSceneCreator* pGameSceneCreator;
					ALCHEMY_DEBUG_NEW( pGameSceneCreator, CGameSceneCreator(m_Parent) );
					m_Parent.m_Tower.AddEventListener(uType, *pGameSceneCreator);
					pGameSceneCreator->Release();

					CTower::FLOORPATH FloorPaths[10];
					CTower::LPFLOORPATH pFloorPath;
					PCHAR pcIndexPath[300];
					UINT i, j, uIndex = 0;
					for(i = 0; i < 5; ++ i)
					{
						pFloorPath = &FloorPaths[i];
						pFloorPath->pcTypePath = ALCHEMY_TEXT("D:\\CloudSpace\\ActivateCode\\Resource\\Test\\f_DAE_a.xml");

						pFloorPath->ppIndexPath = pcIndexPath + uIndex;
						for(j = 0; j < 30; ++ j)
							pcIndexPath[uIndex ++] = ALCHEMY_TEXT("D:\\CloudSpace\\ActivateCode\\Resource\\Test\\001.jpg");

						pFloorPath->uIndexCount = 30;
					}

					for(i = 5; i < 10; ++ i)
					{
						pFloorPath = &FloorPaths[i];
						pFloorPath->pcTypePath = ALCHEMY_TEXT("D:\\CloudSpace\\ActivateCode\\Resource\\Test\\f_DAE_b.xml");

						pFloorPath->ppIndexPath = pcIndexPath + uIndex;
						for(j = 0; j < 30; ++ j)
							pcIndexPath[uIndex ++] = ALCHEMY_TEXT("D:\\CloudSpace\\ActivateCode\\Resource\\Test\\001.jpg");

						pFloorPath->uIndexCount = 30;
					}

					m_Parent.m_Tower.Load(0.0f, 3.0f, ALCHEMY_NULL, ALCHEMY_NULL, FloorPaths, 10);
				}
			}

			ALCHEMY_PLATFORM_MANAGER.RemoveEventListener(Event.GetType(), *this);
		}

	private:
		CGameMain& m_Parent;
	};

	class CGameCreator : 
		public alchemy::CSmartClass<CPlatformManager::IPlatformCreator>
	{
	public:
		CGameCreator(CGameMain& Parent) : m_Parent(Parent) {}
		~CGameCreator() {}

		bool Handler()
		{
			UINT uWindow = ALCHEMY_PLATFORM_MANAGER.CreateWindow(0.0, 0.0, 800.0f, 640.0f);
			IWindow* pWindow = ALCHEMY_PLATFORM_MANAGER.GetWindow(uWindow);

			if(pWindow)
			{
				pWindow->AttachToScreen(true);

				CGameLoop* pGameLoop;
				ALCHEMY_DEBUG_NEW( pGameLoop, CGameLoop(m_Parent) );
				pWindow->AddRunLoop(*pGameLoop);
				pGameLoop->Release();
			}

			UINT uType;
			if( ALCHEMY_GET_EVENT_TYPE(uType, RENDER_SYSTEM_CREATED, CPlatformManager) )
			{
				CEngineCreator* pEngineCreator;
				ALCHEMY_DEBUG_NEW( pEngineCreator, CEngineCreator(m_Parent) );
				ALCHEMY_PLATFORM_MANAGER.AddEventListener(uType, *pEngineCreator);
				pEngineCreator->Release();
			}

			ALCHEMY_PLATFORM_MANAGER.CreateRenderSystem(uWindow, 0, 0, 0, 0);

			return true;
		}

	private:
		CGameMain& m_Parent;
	};
}

CGameMain::CGameInputHandler::CGameInputHandler(CGameMain& Parent) : 
m_Parent(Parent)
{
}

CGameMain::CGameInputHandler::~CGameInputHandler()
{
}

void CGameMain::CGameInputHandler::Handler(const IEventDispatcher::CEvent& Event)
{
	const CInput::CInputEvent& InputEvent = static_cast<const CInput::CInputEvent&>(Event);
	switch( InputEvent.GetInputType() )
	{
	case CInput::CITY_CHANGE_TOWER:
		break;
	case CInput::TOWER_CHANGE_FLOOR:
		break;

	case CInput::CITY_BUILD_TOWER:
		break;
	case CInput::TOWER_BUILD_FLOOR:
		m_Parent.m_Tower.LoadFloor( static_cast<UINT>(ALCHEMY_RAND * 10) );
		break;
	}
}

CGameMain::CGameTransversePointOutputHandler::CGameTransversePointOutputHandler(CGameMain& Parent) : 
m_Parent(Parent)
{
}

CGameMain::CGameTransversePointOutputHandler::~CGameTransversePointOutputHandler()
{
}

void CGameMain::CGameTransversePointOutputHandler::Handler(const IEventDispatcher::CEvent& Event)
{
	if(m_Parent.m_pSceneManager)
	{
		CCamera& Camera = m_Parent.m_pSceneManager->GetSceneCamera();
		const FLOAT3& Translation = Camera.GetTranslation();
		FLOAT fValue = static_cast<const CPointOutput::CPointEvent&>(Event).GetValue() * g_fDEG2RAD;
		FLOAT fSin = sinf(fValue);
		FLOAT fCos = cosf(fValue);
		CVector3 Position(Translation.x * fCos - Translation.z * fSin, 0.0f, Translation.x * fSin + Translation.z * fCos);
		Camera.SetTranslation(Position.x, Translation.y, Position.z);
		Position.Negate();
		Position.Normalize();
		Camera.SetDirection(Position);
	}
}

CGameMain::CGameVerticalPointOutputHandler::CGameVerticalPointOutputHandler(CGameMain& Parent) : 
m_Parent(Parent)
{
}

CGameMain::CGameVerticalPointOutputHandler::~CGameVerticalPointOutputHandler()
{
}

void CGameMain::CGameVerticalPointOutputHandler::Handler(const IEventDispatcher::CEvent& Event)
{
	if(m_Parent.m_pSceneManager)
	{
		CCamera& Camera = m_Parent.m_pSceneManager->GetSceneCamera();
		const FLOAT3& Translation = Camera.GetTranslation();
		FLOAT fValue = static_cast<const CPointOutput::CPointEvent&>(Event).GetValue();
		Camera.SetTranslation(Translation.x, Translation.y + fValue, Translation.z);
	}
}

CGameMain::CGameScalePointOutputHandler::CGameScalePointOutputHandler(CGameMain& Parent) : 
m_Parent(Parent)
{
}

CGameMain::CGameScalePointOutputHandler::~CGameScalePointOutputHandler()
{
}

void CGameMain::CGameScalePointOutputHandler::Handler(const IEventDispatcher::CEvent& Event)
{
	if(m_Parent.m_pSceneManager)
	{
		CCamera& Camera = m_Parent.m_pSceneManager->GetSceneCamera();
		const FLOAT3& Translation = Camera.GetTranslation();
		FLOAT fValue = static_cast<const CPointOutput::CPointEvent&>(Event).GetValue(), fLength = Translation.x * Translation.x + Translation.z * Translation.z;
		if(fLength)
		{
			fValue /= sqrtf(fLength);
			if(fValue < 1.0f)
				Camera.SetTranslation(Translation.x - Translation.x * fValue, Translation.y, Translation.z - Translation.z * fValue);
		}
	}
}

CGameMain::CGameClickPointOutputHandler::CGameClickPointOutputHandler(CGameMain& Parent) : 
m_Parent(Parent)
{
}

CGameMain::CGameClickPointOutputHandler::~CGameClickPointOutputHandler()
{
}

void CGameMain::CGameClickPointOutputHandler::Handler(const IEventDispatcher::CEvent& Event)
{
	m_Parent.m_Input.DispatchEvent(CInput::TOWER_BUILD_FLOOR, 0.0f);
}

CGameMain::CGameMain(void) : 
m_InputHandler(*this), 
m_TransversePointOutputHandler(*this),
m_VerticalPointOutputHandler(*this),
m_ScalePointOutputHandler(*this),
m_ClickPointOutputHandler(*this),
m_pSceneManager(ALCHEMY_NULL)
{
}

CGameMain::~CGameMain(void)
{
}

INT CGameMain::Run(const CHAR* pcPath)
{
	BeginPlatform();

	CGameCreator* pGameCreator;
	ALCHEMY_DEBUG_NEW( pGameCreator, CGameCreator(*this) );
	INT nResult = ALCHEMY_PLATFORM_MANAGER.MainLoop(pGameCreator);
	ALCHEMY_DEBUG_DELETE(pGameCreator);

	ALCHEMY_DEBUG_DELETE(m_pSceneManager);

	EndPlatform();

	return nResult;
}