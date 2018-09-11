#pragma once

#include "AlchemyFlasccContext3D.h"

#include "../Graphics/AlchemyGraphicsManager.h"

extern "C" void CallbackToResetViewport(alchemy::UINT uViewportWidth, alchemy::UINT uViewportHeight);
extern "C" void CallbackToCreateRenderSystem(alchemy::UINT uRenderSystemIndex, alchemy::UINT uViewportWidth, alchemy::UINT uViewportHeight, alchemy::INT nAntiAlias, bool bIsAutoResetViewport);
extern "C" void CallbackToRunLoop(alchemy::FLOAT fElapseTime);
extern "C" void CallbackToCreateBitmapData(const void* pData, alchemy::UINT uWidth, alchemy::UINT uHeight, const alchemy::CHAR* pcPath);
extern "C" void CallbackToCreateData(const void* pData, alchemy::UINT uLength, const alchemy::CHAR* pcPath);

namespace alchemy
{
	class CFlasccGraphicsManager : 
		public CGraphicsManager
	{
	public:
		ALCHEMY_BEGIN_EVENT_ENUM
			RESET_VIEWPORT, 
			BITMAP_DATA_CREATED, 
			DATA_CREATED
		ALCHEMY_END_EVENT_ENUM

		class CViewportEvent :
			public IEventDispatcher::CEvent
		{
		public:
			CViewportEvent(UINT uViewportWidth, UINT uViewportHeight, UINT uType, bool bIsBubbles = true);
			~CViewportEvent();

			UINT GetViewportWidth()const;
			UINT GetViewportHeight()const;
		private:
			UINT m_uViewportWidth;
			UINT m_uViewportHeight;
		};

		/*class CBitmapDataEvent :
			public IEventDispatcher::CEvent
		{
		public:
			CBitmapDataEvent(const void* pData, UINT uWidth, UINT uHeight, UINT uType, bool bIsBubbles = true);
			~CBitmapDataEvent();

			UINT GetWidth()const;
			UINT GetHeight()const;
			const void* GetData()const;
		private:
			UINT m_uWidth;
			UINT m_uHeight;
			const void* m_pData;
		};

		class CDataEvent :
			public IEventDispatcher::CEvent
		{
		public:
			CDataEvent(const void* m_pData, UINT uType, bool bIsBubbles = true);
			~CDataEvent();

			const void* GetData()const;
		private:
			const void* m_pData;
		};*/

		CFlasccGraphicsManager(void);
		~CFlasccGraphicsManager(void);

		void Load(const CHAR* pcPath);

		void CreateRenderSystem(UINT uWindowHandle, UINT uBackBufferWidth, UINT uBackBufferHeight, INT nAntiAliasing, UINT uType);

		CSceneManager* CreateSceneManager(CRenderSystem& RenderSystem);

		INT MainLoop();

		//void LoadBitmapData(const CHAR* pcFilePath);
		//void LoadData(const CHAR* pcFilePath);

		bool AddRunLoop(IWindow::IDisplayLink& DisplayLink);

		void RunLoop(FLOAT fElapseTime);

	protected:
		IWindow* _CreateWindow(FLOAT x, FLOAT y, FLOAT fWidth, FLOAT fHeight);

	private:
		CVector<IWindow::IDisplayLink*> m_DisplayLinks;
		FLOAT m_fTime;

		CMeshManager m_MeshManager;
	};

	ALCHEMY_INLINE UINT CFlasccGraphicsManager::CViewportEvent::GetViewportWidth()const
	{
		return m_uViewportWidth;
	}

	ALCHEMY_INLINE UINT CFlasccGraphicsManager::CViewportEvent::GetViewportHeight()const
	{
		return m_uViewportHeight;
	}

	/*ALCHEMY_INLINE UINT CFlasccGraphicsManager::CBitmapDataEvent::GetWidth()const
	{
		return m_uWidth;
	}

	ALCHEMY_INLINE UINT CFlasccGraphicsManager::CBitmapDataEvent::GetHeight()const
	{
		return m_uHeight;
	}

	ALCHEMY_INLINE const void* CFlasccGraphicsManager::CBitmapDataEvent::GetData()const
	{
		return m_pData;
	}

	ALCHEMY_INLINE const void* CFlasccGraphicsManager::CDataEvent::GetData()const
	{
		return m_pData;
	}*/
}