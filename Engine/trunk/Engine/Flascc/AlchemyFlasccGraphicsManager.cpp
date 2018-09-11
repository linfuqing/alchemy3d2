#include "AlchemyFlasccStdafx.h"
#include "AlchemyFlasccGraphicsManager.h"
#include "AlchemyFlasccWindow.h"

using namespace alchemy;

extern "C" void CallbackToResetViewport(UINT uViewportWidth, UINT uViewportHeight)
{
	UINT uType;
	if( ALCHEMY_GET_EVENT_TYPE(uType, RESET_VIEWPORT, CFlasccGraphicsManager) )
	{
		CFlasccGraphicsManager::CViewportEvent ViewportEvent(uViewportWidth, uViewportHeight, uType);
		ALCHEMY_GRAPHICS_MANAGER.DispatchEvent(ViewportEvent);
	}
}

extern "C" void CallbackToCreateRenderSystem(UINT uRenderSystemIndex, UINT uViewportWidth, UINT uViewportHeight, INT nAntiAlias, bool bIsAutoResetViewport)
{
	CFlasccContext3D* pContext3D;
	ALCHEMY_DEBUG_NEW( pContext3D, CFlasccContext3D(uRenderSystemIndex) );

	pContext3D->SetViewport(uViewportWidth, uViewportHeight, nAntiAlias);

	if(bIsAutoResetViewport)
		pContext3D->AddEventListenerForViewportReset();

	UINT uType;
	if( ALCHEMY_GET_EVENT_TYPE(uType, RENDER_SYSTEM_CREATED, CGraphicsManager) )
	{
		CGraphicsManager::CRenderSystemEvent RenderSystemEvent(*pContext3D, uType);
		ALCHEMY_GRAPHICS_MANAGER.DispatchEvent(RenderSystemEvent);
	}
}

extern "C" void CallbackToRunLoop(alchemy::FLOAT fElapseTime)
{
	static_cast<CFlasccGraphicsManager&>(ALCHEMY_GRAPHICS_MANAGER).RunLoop(fElapseTime);
}

extern "C" void CallbackToCreateBitmapData(const void* pData, UINT uWidth, UINT uHeight, const CHAR* pcPath)
{
	UINT uType;
	if( ALCHEMY_GET_EVENT_TYPE(uType, LOAD_COMPLETE, CGraphicsManager) )
	{
		CGraphicsManager::DATA Data;
		Data.uAlpha = uWidth;
		Data.uBeta = uHeight;
		Data.pData = reinterpret_cast<PVOID>( const_cast<void*>(pData) );
		CGraphicsManager::CLoadEvent LoadEvent(Data, pcPath, uType);
		ALCHEMY_GRAPHICS_MANAGER.DispatchEvent(LoadEvent);
	}
}

extern "C" void CallbackToCreateData(const void* pData, UINT uLength, const CHAR* pcPath)
{
	UINT uType;
	if( ALCHEMY_GET_EVENT_TYPE(uType, LOAD_COMPLETE, CGraphicsManager) )
	{
		CGraphicsManager::DATA Data;
		Data.uAlpha = uLength;
		Data.uBeta = 0;
		Data.pData = reinterpret_cast<PVOID>( const_cast<void*>(pData) );
		CGraphicsManager::CLoadEvent LoadEvent(Data, pcPath, uType);
		ALCHEMY_GRAPHICS_MANAGER.DispatchEvent(LoadEvent);
	}
}

class CRenderSystemCreateHandler :
	public CSmartClass<IEventDispatcher::IListener>
{
public:
	CRenderSystemCreateHandler() : m_pRenderSystem(ALCHEMY_NULL) {}
	~CRenderSystemCreateHandler() {}

	ALCHEMY_INLINE CRenderSystem* GetRenderSystem() const
	{
		return m_pRenderSystem;
	}

	void Handler(const IEventDispatcher::CEvent& Event)
	{
		m_pRenderSystem = &static_cast<const CGraphicsManager::CRenderSystemEvent&>(Event).GetRenderSystem();
	}

private:
	CRenderSystem* m_pRenderSystem;
};

CFlasccGraphicsManager::CViewportEvent::CViewportEvent(UINT uViewportWidth, UINT uViewportHeight, UINT uType, bool bIsBubbles) :
m_uViewportWidth(uViewportWidth), 
m_uViewportHeight(uViewportHeight), 
CEvent(uType, bIsBubbles)
{

}

CFlasccGraphicsManager::CViewportEvent::~CViewportEvent()
{

}

/*CFlasccGraphicsManager::CBitmapDataEvent::CBitmapDataEvent(const void* pData, UINT uWidth, UINT uHeight, UINT uType, bool bIsBubbles) :
m_uWidth(uWidth), 
m_uHeight(uHeight), 
m_pData(pData), 
CEvent(uType, bIsBubbles)
{

}

CFlasccGraphicsManager::CBitmapDataEvent::~CBitmapDataEvent()
{

}

CFlasccGraphicsManager::CDataEvent::CDataEvent(const void* pData, UINT uType, bool bIsBubbles) :
m_pData(pData),
CEvent(uType, bIsBubbles)
{

}

CFlasccGraphicsManager::CDataEvent::~CDataEvent()
{

}*/

CFlasccGraphicsManager::CFlasccGraphicsManager(void) :
m_fTime(0.0f)
{
}

CFlasccGraphicsManager::~CFlasccGraphicsManager(void)
{
}

void CFlasccGraphicsManager::Load(const CHAR* pcPath)
{
#ifdef AS3_H
	STRING Extension;
	if( GetPathInfo(pcPath, ALCHEMY_NULL, ALCHEMY_NULL, &Extension) )
	{
		if( Extension == ALCHEMY_TEXT("jpg") || Extension == ALCHEMY_TEXT("png") )
		{
			inline_as3(
				"import alchemy.core.AlchemyGraphicsManager;\n"
				"AlchemyGraphicsManager.instance.loadBitmapData(%0, %1);\n"
				: : "r"(pcPath), "r"( ALCHEMY_STRING_LENGTH(pcPath) )
				);
		}
		else
		{
			inline_as3(
				"import alchemy.core.AlchemyGraphicsManager;\n"
				"AlchemyGraphicsManager.instance.loadData(%0, %1);\n"
				: : "r"(pcPath), "r"( ALCHEMY_STRING_LENGTH(pcPath) )
				);
		}
	}
#endif
}

void CFlasccGraphicsManager::CreateRenderSystem(UINT uWindowHandle, UINT uBackBufferWidth, UINT uBackBufferHeight, INT nAntiAliasing, UINT uType)
{
	CFlasccWindow* pCurrentWindow = static_cast<CFlasccWindow*>( GetWindow(uWindowHandle) );
	if(pCurrentWindow)
	{
		FLOAT fWidth = uBackBufferWidth ? uBackBufferWidth : pCurrentWindow->GetViewportWidth(), fHeight = uBackBufferHeight ? uBackBufferHeight : pCurrentWindow->GetViewportHeight();
#ifdef AS3_H
		inline_as3(
			"import alchemy.core.AlchemyGraphicsManager;\n"
			"AlchemyGraphicsManager.instance.createRenderSystem(%0, %1, %2, %3, %4);\n"
			: : "r"( pCurrentWindow->GetViewportX() ), "r"( pCurrentWindow->GetViewportY() ), "r"(fWidth), "r"(fHeight), "r"(nAntiAliasing) );
#endif
	}
}

CSceneManager* CFlasccGraphicsManager::CreateSceneManager(CRenderSystem& RenderSystem)
{
	CSceneManager* pSceneManager;
	ALCHEMY_DEBUG_NEW( pSceneManager, CSceneManager(RenderSystem) );

	return pSceneManager;
}

INT CFlasccGraphicsManager::MainLoop()
{
	return 0;
}

/*void CFlasccGraphicsManager::LoadBitmapData(const CHAR* pcFilePath)
{
#ifdef AS3_H
	inline_as3(
		"import alchemy.core.AlchemyGraphicsManager;\n"
		"AlchemyGraphicsManager.instance.loadBitmapData(%0, %1);\n"
		: : "r"(pcFilePath), "r"( ALCHEMY_STRING_LENGTH(pcFilePath) )
	);
#endif
}

void CFlasccGraphicsManager::LoadData(const CHAR* pcFilePath)
{
#ifdef AS3_H
	inline_as3(
		"import alchemy.core.AlchemyGraphicsManager;\n"
		"AlchemyGraphicsManager.instance.loadData(%0, %1);\n"
		: : "r"(pcFilePath), "r"( ALCHEMY_STRING_LENGTH(pcFilePath) )
		);
#endif
}*/

bool CFlasccGraphicsManager::AddRunLoop(IWindow::IDisplayLink& DisplayLink)
{
#ifdef AS3_H
	if( !m_DisplayLinks.GetLength() )
	{
		inline_as3(
			"import alchemy.core.AlchemyGraphicsManager;\n"
			"AlchemyGraphicsManager.instance.runLoop();\n"
			: : 
		);
	}
#endif

	m_DisplayLinks.Add(&DisplayLink);

	return true;
}

void CFlasccGraphicsManager::RunLoop(FLOAT fElapseTime)
{
	CVector<IWindow::IDisplayLink*>::CIterator Iterator = m_DisplayLinks.GetIterator(0);
	while( CVector<IWindow::IDisplayLink*>::IsValuable(Iterator) )
	{
		(*Iterator)->Run(m_fTime, fElapseTime);

		++ Iterator;
	}

	m_fTime += fElapseTime;
}

IWindow* CFlasccGraphicsManager::_CreateWindow(FLOAT x, FLOAT y, FLOAT fWidth, FLOAT fHeight)
{
	CFlasccWindow* pWindow;
	ALCHEMY_DEBUG_NEW( pWindow, CFlasccWindow(x, y, fWidth, fHeight) );

	return pWindow;
}