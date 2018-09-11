#pragma once

#include "../Graphics/AlchemyGraphicsManager.h"
#include "../Graphics/AlchemySceneNodeFactory.h"

namespace alchemy
{
    class CIOSGraphicsManager :
	public CGraphicsManager
    {
    public:
        CIOSGraphicsManager();
        ~CIOSGraphicsManager();
		
        IWindow* CreateWindow(FLOAT x, FLOAT y, FLOAT fWidth, FLOAT fHeight);
        CRenderSystem* CreateRenderSystem(IWindow* pWindow);
		CSceneManager* CreateSceneManager(CRenderSystem& RenderSystem);
		
	private:
		CMeshManager m_MeshManager;
		CSceneNodeFactory m_SceneNodeFactory;
    };
}