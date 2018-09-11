#pragma once

#include "AlchemySceneNodeInterface.h"
#include "AlchemyTransform.h"
#include "AlchemyRenderQueue.h"

#include "../Core/AlchemyEventDispatcher.h"

namespace alchemy
{
	struct IUpdater;
	struct ISceneNodeFactory;
	class CSceneNode :
		public CTransform, public CEventDispatcher<IEventDispatcher>, virtual public ISceneNode, virtual public CRenderQueue::ICallBackToRender
	{ 
	public:
		typedef enum
		{
			RELATICE = 0,
			ABSOLUTE,
			INTERVAL,
			IMMEDIATE
		}COORDINATESYSTEM;

		/*typedef enum
		{
			ADD_CHILD = 0,
			REMOVE_CHILD,
			ADDED,
			REMOVED,

			TOTAL_EVENT
		}EVENT;*/
		ALCHEMY_BEGIN_EVENT_ENUM
			ADD_CHILD = 0,
			REMOVE_CHILD,
			SCENE_DIRTY,
			ADDED,
			REMOVED
		ALCHEMY_END_EVENT_ENUM

		CSceneNode(void);
		~CSceneNode(void);
		
		void SetName(const STRING& Name);
		void SetName(const CHAR* pName);
		const STRING& GetName()const;
		const CHAR* GetName();

		//static bool GetEventType(UINT& uOutput, EVENT Event);

		const CMatrix4x4& GetLocalMatrix()const;
		const CMatrix4x4& GetWorldMatrix()const;

		const CMatrix4x4& GetInverseWorldMatrix();
		const CMatrix4x4& GetTransposeWorldMatrix();

		CSceneNode* GetParent()const;

		COORDINATESYSTEM GetCoordinateSystem()const;

		UINT GetChildrenCount()const;

		bool AddChild(CSceneNode& Child);
		bool RemoveChild(CSceneNode& Child);

		void SetVisible(bool visible);
		bool GetVisible() const;

		CSceneNode* GetChild(const CHAR* pcName);

		void Render(const CRenderQueue::RENDERENTRY& Entry, const CRenderQueue::RENDERKEY& Key, UINT uFlag);

		void UpdateTransform();

		void Update();

		void UpdateChildren();

		//virtual void Destroy();

		void AddedToRenderQueue();

		bool DispatchEvent(IEventDispatcher::CEvent& Event);

		CSceneNode& CloneTree(ISceneNodeFactory& SceneNodeFactory)const;

		bool IntersectTree(
			const RAY& Ray, 
			CMesh::LPINTERSECTINFO pIntersectInfos,
			PUINT puIntersectInfoCount, 
			bool bIsCheckHitOn = false,
			bool bHitOnMask = false,
			bool bIsCheckFront = false, 
			bool bFrontMask = false, 
			LPFLOAT3 pHitPoints = ALCHEMY_NULL, 
			LPFLOAT3 pfFaceNormals = ALCHEMY_NULL);

		virtual CSceneNode& Clone(ISceneNodeFactory& SceneNodeFactory)const;

		virtual bool Intersect(
			const RAY& Ray, 
			CMesh::LPINTERSECTINFO pIntersectInfos, 
			PUINT puIntersectInfoCount, 
			bool bIsCheckHitOn,
			bool bHitOnMask,
			bool bIsCheckFront, 
			bool bFrontMask, 
			LPFLOAT3 pHitPoints, 
			LPFLOAT3 pfFaceNormals);

		virtual bool ApplyForRender(UINT uRenderPass = ~0, UINT uRenderList = CRenderQueue::NORMAL);

		ALCHEMY_IMPLEMENT_INTERFACE
	protected:
		//bool m_bIsTransformDirty;

		CMatrix4x4 m_LocalMatrix;
		CMatrix4x4 m_WorldMatrix;

		COORDINATESYSTEM m_CoordinateSystem;

		bool m_bIVisible;

	private:
		STRING m_Name;
		
		CSceneNode* m_pPrevious;
		CSceneNode* m_pNext;
		CSceneNode* m_pChild;
		CSceneNode* m_pParent;

		UINT m_uChildrenCount;

		CMatrix4x4 m_InverseWorldMatrix;
		CMatrix4x4 m_TransposeWorldMatrix;

		bool m_bIsInverseWorldMatrixDirty;
		bool m_bIsTransposeWorldMatrixDirty;
	};

	ALCHEMY_INLINE void CSceneNode::SetName(const STRING& Name)
	{
		m_Name = Name;
	}
	
	ALCHEMY_INLINE void CSceneNode::SetName(const CHAR* pName)
	{
		m_Name = pName;
	}

	ALCHEMY_INLINE const STRING& CSceneNode::GetName()const
	{
		return m_Name;
	}
	
	ALCHEMY_INLINE const CHAR* CSceneNode::GetName()
	{
		return static_cast<const CHAR*>(m_Name);
	}

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

	ALCHEMY_INLINE bool CSceneNode::GetVisible() const
	{
		return m_bIVisible;
	}
}