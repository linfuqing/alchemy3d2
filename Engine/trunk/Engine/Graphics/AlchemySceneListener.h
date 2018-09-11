#pragma once

#include "AlchemyRenderGroupInterface.h"

#include "../Core/AlchemySingleton.h"
#include "../Core/AlchemyEventDispatcherInterface.h"
#include "../Core/AlchemySmartClass.h"
#include "../Core/AlchemyMap.h"
#include "../Core/AlchemyVector.h"

namespace alchemy
{
	class CSceneManager;
	class CSceneListener : 
		public CSmartClass<IEventDispatcher::IListener>, public CSingleton<CSceneListener>
	{
	public:
		CSceneListener();
		virtual ~CSceneListener();

		void RegisterRenderGroup( CSceneManager& SceneManager, IRenderGroup& RenderGroup );
		void UnResiterRenderGroup( CSceneManager& SceneManager, IRenderGroup& RenderGroup );

		void Handler(const IEventDispatcher::CEvent& Event);

	private:
		CMap< CSceneManager*, CVector<IRenderGroup*> >	m_RenderGroupsMap;
	};
}