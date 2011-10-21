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

		void SetName(const CHAR* pName);
		const STRING& GetName();

		CMatrix4x4& GetLocalMatrix();

		void UpdateTransform();

		void Update();

		bool AddChild(CSceneNode& Child);
		bool RemoveChild(CSceneNode& Child);

		CBone* GetChildByName(const CHAR* pName);

		void RegisterAnimationController(CAnimationController& AnimationController);
	private:
		CBone& __GetTreeTail();

		bool m_bIsBoneParent;
		CBone* m_pNext;

		STRING m_Name;

		bool m_bIsRegisterAnimationController;
	};

	ALCHEMY_INLINE void CBone::SetName(const CHAR* pName)
	{
		m_Name = pName;
	}

	ALCHEMY_INLINE const STRING& CBone::GetName()
	{
		return m_Name;
	}

	ALCHEMY_INLINE CMatrix4x4& CBone::GetLocalMatrix()
	{
		return m_LocalMatrix;
	}
}