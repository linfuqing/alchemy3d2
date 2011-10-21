#pragma once

#include "AlchemySceneNodeInterface.h"
#include "../Math/AlchemyMatrix4x4.h"
#include "AlchemyTransform.h"
#include "AlchemyCallBackToRenderInterface.h"

namespace alchemy
{
	class CSceneNode :
		public CTransform, public CSceneNodeInterface, public CCallBackToRenderInterface
	{
	public:
		typedef enum
		{
			RELATICE = 0,
			ABSOLUTE,
			INTERVAL,
			IMMEDIATE
		}COORDINATESYSTEM;

		CSceneNode(void);
		~CSceneNode(void);

		const CMatrix4x4& GetLocalMatrix()const;
		const CMatrix4x4& GetWorldMatrix()const;

		CSceneNode* GetParent()const;

		COORDINATESYSTEM GetCoordinateSystem()const;

		UINT GetChildrenCount()const;

		bool IsDynamic()const;

		bool AddChild(CSceneNode& Child);
		bool RemoveChild(CSceneNode& Child);

		void Render(const CRenderQueue::RENDERENTRY& Entry, UINT uFlag);

		void UpdateTransform();

		void Update();

		//virtual void Destroy();

		virtual void AddedToRenderQueue();

		virtual bool ApplyForRender();
	protected:
		//bool m_bIsTransformDirty;

		CMatrix4x4 m_LocalMatrix;
		CMatrix4x4 m_WorldMatrix;

		COORDINATESYSTEM m_CoordinateSystem;
	private:
		CSceneNode* m_pPrevious;
		CSceneNode* m_pNext;
		CSceneNode* m_pChild;
		CSceneNode* m_pParent;

		UINT m_uChildrenCount;

		bool m_bIsDynamic;
	};

	ALCHEMY_INLINE const CMatrix4x4& CSceneNode::GetLocalMatrix()const
	{
		return m_LocalMatrix;
	}

	ALCHEMY_INLINE const CMatrix4x4& CSceneNode::GetWorldMatrix()const
	{
		return m_WorldMatrix;
	}

	ALCHEMY_INLINE CSceneNode* CSceneNode::GetParent()const
	{
		return m_pParent;
	}

	ALCHEMY_INLINE CSceneNode::COORDINATESYSTEM CSceneNode::GetCoordinateSystem()const
	{
		return m_CoordinateSystem;
	}

	ALCHEMY_INLINE UINT CSceneNode::GetChildrenCount()const
	{
		return m_uChildrenCount;
	}
}