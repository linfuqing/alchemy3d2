#pragma once

#include "AlchemyClipperInterface.h"
#include "AlchemySceneTreeObjectInterface.h"

namespace alchemy
{
	ALCHEMY_INTERFACE(ISceneTreeManager)
	{
		class CSceneTreeObjectIterator
		{
		public:
			CSceneTreeObjectIterator(ISceneTreeObject& SceneTreeObject) : m_SceneTreeObject(SceneTreeObject), m_pNext(ALCHEMY_NULL) {};
			virtual ~CSceneTreeObjectIterator() {};

			ISceneTreeObject& GetSceneTreeObject()const;
			CSceneTreeObjectIterator* GetNext()const;

		protected:
			void _PushTo(CSceneTreeObjectIterator& Tail);

			ISceneTreeObject& m_SceneTreeObject;
			CSceneTreeObjectIterator* m_pNext;
		};

		virtual CSceneTreeObjectIterator* SearchObject(const IBoundingVolume& BoundingVolume, const IClipper* pClipper, ISceneTreeManager::CSceneTreeObjectIterator** ppSearchListTail) = 0;

		virtual bool AddObject(ISceneTreeObject& SceneTreeNode) = 0;
	};

	ALCHEMY_INLINE ISceneTreeObject& ISceneTreeManager::CSceneTreeObjectIterator::GetSceneTreeObject()const
	{
		return m_SceneTreeObject;
	}

	ALCHEMY_INLINE ISceneTreeManager::CSceneTreeObjectIterator* ISceneTreeManager::CSceneTreeObjectIterator::GetNext()const
	{
		return m_pNext;
	}

	ALCHEMY_INLINE void ISceneTreeManager::CSceneTreeObjectIterator::_PushTo(ISceneTreeManager::CSceneTreeObjectIterator& Tail)
	{
		Tail.m_pNext = this;
	}
}