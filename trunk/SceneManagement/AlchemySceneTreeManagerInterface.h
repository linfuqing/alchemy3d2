#pragma once

#include "AlchemyClipperInterface.h"
#include "AlchemySceneTreeObjectInterface.h"

namespace alchemy
{
	class CSceneTreeManagerInterface
	{
	public:
		class CSceneTreeObjectIterator
		{
		public:
			CSceneTreeObjectIterator(CSceneTreeObjectInterface& SceneTreeObject) : m_SceneTreeObject(SceneTreeObject), m_pNext(ALCHEMY_NULL) {};
			virtual ~CSceneTreeObjectIterator() {};

			CSceneTreeObjectInterface& GetSceneTreeObject()const;
			CSceneTreeObjectIterator* GetNext()const;

		protected:
			void _PushTo(CSceneTreeObjectIterator& Tail);

			CSceneTreeObjectInterface& m_SceneTreeObject;
			CSceneTreeObjectIterator* m_pNext;
		};

		CSceneTreeManagerInterface(void) {}
		virtual ~CSceneTreeManagerInterface(void) {}

		virtual CSceneTreeObjectIterator* SearchObject(const CBoundingVolumeInterface& BoundingVolume, const CClipperInterface* pClipper, CSceneTreeManagerInterface::CSceneTreeObjectIterator** ppSearchListTail) = 0;

		virtual bool AddObject(CSceneTreeObjectInterface& SceneTreeNode) = 0;
	};

	ALCHEMY_INLINE CSceneTreeObjectInterface& CSceneTreeManagerInterface::CSceneTreeObjectIterator::GetSceneTreeObject()const
	{
		return m_SceneTreeObject;
	}

	ALCHEMY_INLINE CSceneTreeManagerInterface::CSceneTreeObjectIterator* CSceneTreeManagerInterface::CSceneTreeObjectIterator::GetNext()const
	{
		return m_pNext;
	}

	ALCHEMY_INLINE void CSceneTreeManagerInterface::CSceneTreeObjectIterator::_PushTo(CSceneTreeManagerInterface::CSceneTreeObjectIterator& Tail)
	{
		Tail.m_pNext = this;
	}
}