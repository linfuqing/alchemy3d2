#pragma once

#include "AlchemySceneNode.h"

namespace alchemy
{
	class CSceneManager;
	class CSceneNodeUpdater : 
		public CSceneNode
	{
	public:
		CSceneNodeUpdater();
		~CSceneNodeUpdater();

		bool Create(CSceneManager* pSceneManager = ALCHEMY_NULL);

		void AddUpdater(IUpdater& Updater);

		void RemoveUpdaer(IUpdater& Updater);

		void Reset();

		void Update();

	private:
		CVector<IUpdater*>	m_Updaers;
		CSceneManager*		m_pSceneManager;
	};
}