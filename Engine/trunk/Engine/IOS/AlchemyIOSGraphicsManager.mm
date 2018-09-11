#include "AlchemyIOSGraphicsManager.h"
#include "AlchemyIOSWindow.h"
#include "AlchemyIOSOpenGLESContext.h"

using namespace alchemy;

CIOSGraphicsManager::CIOSGraphicsManager()
{
}

CIOSGraphicsManager::~CIOSGraphicsManager()
{
}

IWindow* CIOSGraphicsManager::CreateWindow(FLOAT x, FLOAT y, FLOAT fWidth, FLOAT fHeight)
{
	CGRect Rect;
	Rect.origin.x    = x;
	Rect.origin.y    = y;
	Rect.size.width  = fWidth;
	Rect.size.height = fHeight;
	
	CIOSWindow* pWindow;
	ALCHEMY_DEBUG_NEW( pWindow, CIOSWindow(Rect) );
	
	//m_WindowPool.Add(pWindow);

	return pWindow;
}

CRenderSystem* CIOSGraphicsManager::CreateRenderSystem(IWindow* pWindow)
{
	CIOSWindow* pIOSWindow = dynamic_cast<CIOSWindow*>(pWindow);
	if(pIOSWindow)
	{
		CIOSOpenGLESContext* pContext;
		CAEAGLLayer* pLayer = (CAEAGLLayer*)[&pIOSWindow->GetView() layer];
		ALCHEMY_DEBUG_NEW( pContext, CIOSOpenGLESContext(*pLayer) );
		
		return pContext;
	}
	
	return ALCHEMY_NULL;
}

CSceneManager* CIOSGraphicsManager::CreateSceneManager(CRenderSystem& RenderSystem)
{
	CSceneManager* pSceneManager;
	ALCHEMY_DEBUG_NEW( pSceneManager, CSceneManager(RenderSystem, &m_SceneNodeFactory) );
	
	return pSceneManager;
}