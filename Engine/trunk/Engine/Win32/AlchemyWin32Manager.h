#pragma once

#include "AlchemyWin32Loader.h"

#include "../Platform/AlchemyPlatformManager.h"

namespace alchemy
{
	class CWin32Manager : 
		public CPlatformManager
	{
	public:
		CWin32Manager(void);
		~CWin32Manager(void);

		void CreateRenderSystem(UINT uWindowHandle, UINT uBackBufferWidth, UINT uBackBufferHeight, INT nAntiAliasing, UINT uType);

		INT MainLoop(CPlatformManager::IPlatformCreator* pCreator);

	protected:
		IWindow* _CreateWindow(FLOAT x, FLOAT y, FLOAT fWidth, FLOAT fHeight);

		CVector<CRenderSystem*> m_RenderSystemList;

		CMeshManager m_MeshManager;
		CWin32Loader m_Loader;
	};
}