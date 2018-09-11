#pragma once

#include "AlchemySceneNode.h"
#include "AlchemyAnimationController.h"
#include "../Core/AlchemyString.h"

namespace alchemy
{
	class CBone :
		public CSceneNode
	{
	public:
		CBone(void);
		~CBone(void);

		CMatrix4x4& GetLocalMatrix();

		void UpdateTransform();

		void Update();

		bool AddChild(CSceneNode& Child);
		bool RemoveChild(CSceneNode& Child);

		CSceneNode& Clone(ISceneNodeFactory& SceneNodeFactory)const;

		CBone* GetChildByName(const CHAR* pName);

		void RegisterAnimationController(CAnimationController& AnimationController);
	private:
		CBone& __GetTreeTail();

		bool m_bIsBoneParent;
		CBone* m_pNext;

		bool m_bIsRegisterAnimationController;
	};

	ALCHEMY_INLINE CMatrix4x4& CBone::GetLocalMatrix()
	{
		return m_LocalMatrix;
	}
}