#pragma once

#include "AlchemySceneTreeManagerInterface.h"

#include "../Math/AlchemyVector3.h"
#include "../Core/AlchemyVector.h"
#include "../Core/AlchemyBitFlag.h"
#include "../Core/AlchemySmartClass.h"

namespace alchemy
{
	class CQuadTree :
		public CSmartClass<ISceneTreeManager>
	{
		typedef enum
		{
			MININUM_TREE_DEPTH = 1,
			MAXINUM_TREE_DEPTH = 8 + MININUM_TREE_DEPTH
		}CONSTANTS;

		typedef struct
		{
			UINT uMaxX;
			UINT uMaxY;
			UINT uMaxZ;

			UINT uMinX;
			UINT uMinY;
			UINT uMinZ;
		}QUADTREEBOX;

		class CQuadTreeNode;
		class CQuadTreeObject :
			public ISceneTreeManager::CSceneTreeObjectIterator
		{
			friend class CQuadTree;
			friend class CQuadTreeNode;
		public:
			CQuadTreeObject(ISceneTreeObject& SceneTreeObject);
			~CQuadTreeObject(void);

		private:
			CQuadTreeObject* m_pTreeForward;
			CQuadTreeObject* m_pTreeRear;

			CQuadTree*     m_pParentQuadTree;
			CQuadTreeNode* m_pParentQuadTreeNode;

			UINT32 m_uMaskY;
		};

		class CQuadTreeNode
		{
		public:
			CQuadTreeNode();
			~CQuadTreeNode();

			bool AddMember(CQuadTreeObject& Object, const QUADTREEBOX& Box);
			bool RemoveMember(CQuadTreeObject& Object);

			void AddMemberToSearchList(
				CQuadTreeObject** ppListHead,
				CQuadTreeObject** ppListTail,
				UINT32 uMaskY,
				const IClipper* pClipper);

			void AddMemberToSearchList(
				CQuadTreeObject** ppListHead,
				CQuadTreeObject** ppListTail,
				UINT32 uMaskY,
				const BOUNDINGBOX& BoundingBox,
				const IClipper* pClipper); 

			void Setup(CQuadTreeNode* pParent, CQuadTreeNode* pChild1, CQuadTreeNode* pChild2, CQuadTreeNode* pChild3, CQuadTreeNode* pChild4);

			UINT32 GetLocalMaskY()const;
			UINT32 GetWorldMaskY()const;
		private:
			void __DescendantMemberAdded(UINT32 uMaskY);
			void __DescendantMemberRemoved();

			void __RebuildLocalMaskY();
			void __RebuildWorldMaskY();

			UINT32 m_uLocalMaskY;
			UINT32 m_uWorldMaskY;

			CQuadTreeObject* m_pMembers;

			CQuadTreeNode* m_pParent;
			CQuadTreeNode* m_pChildren[4];
		};
	public:
		CQuadTree(void);
		~CQuadTree(void);

		bool Create(const BOUNDINGBOX& WorldBoundary, UINT uDepth);

		CSceneTreeObjectIterator* SearchObject(const IBoundingVolume& BoundingVolume, const IClipper* pClipper, ISceneTreeManager::CSceneTreeObjectIterator** ppSearchListTail);

		bool AddObject(ISceneTreeObject& SceneTreeObject);

		void Destroy();

	private:
		static UINT32 __GetMaskY(UINT8 uMinY, UINT8 uMaxY);
		static QUADTREEBOX& __Convert(QUADTREEBOX& Output, const BOUNDINGBOX BoundingBox , const FLOAT3& Offset, const FLOAT3& Scale);

		void __FindTreeNodeInfo(const QUADTREEBOX& WorldByteBox, UINT& uLevel, UINT& uLevelX, UINT& uLevelZ);
		CQuadTreeNode* __FindTreeNode(const QUADTREEBOX& WorldByteBox);
		CQuadTreeNode* __GetNodeFromLevelXZ(INT nLevel, UINT x, UINT z);

		CQuadTreeObject& __CreateQuadTreeObject(ISceneTreeObject& SceneTreeObject);

		CQuadTreeNode* m_pLevelNodes[MAXINUM_TREE_DEPTH];

		CVector<CQuadTreeObject*> m_Objects;

		CVector3 m_Scale;
		CVector3 m_Offset;

		UINT m_uDepth;
	};
    
    ALCHEMY_INLINE void CQuadTree::CQuadTreeNode::Setup(CQuadTreeNode* pParent, CQuadTreeNode* pChild1, CQuadTreeNode* pChild2, CQuadTreeNode* pChild3, CQuadTreeNode* pChild4)
    {
        m_pParent = pParent;
        m_pChildren[0] = pChild1;
        m_pChildren[1] = pChild2;
        m_pChildren[2] = pChild3;
        m_pChildren[3] = pChild4;
    }
    
    ALCHEMY_INLINE UINT32 CQuadTree::CQuadTreeNode::GetLocalMaskY()const
    {
        return m_uLocalMaskY;
    }
    
    ALCHEMY_INLINE UINT32 CQuadTree::CQuadTreeNode::GetWorldMaskY()const
    {
        return m_uWorldMaskY;
    }

	ALCHEMY_INLINE UINT32 CQuadTree::__GetMaskY(UINT8 uMinY, UINT8 uMaxY)
	{
		UINT32 uHigh     = ALCHEMY_FLAG(uMaxY),
			uLow         = ALCHEMY_FLAG(uMinY),
			uSetMask     = uHigh - 1,
			uClearMask   = uLow  - 1,
			uResult      = uSetMask;

		if(uMinY)
			uResult &= ~uClearMask;

		uResult |= uHigh;
		uResult |= uLow;

		return uResult;
	}

	ALCHEMY_INLINE CQuadTree::CQuadTreeNode* CQuadTree::__GetNodeFromLevelXZ(INT nLevel, UINT x, UINT z)
	{
		if(nLevel >= 0 && nLevel < (INT)m_uDepth)
			return &m_pLevelNodes[nLevel][(z << nLevel) + x];

		return ALCHEMY_NULL;
	}

	ALCHEMY_INLINE void CQuadTree::__FindTreeNodeInfo(const QUADTREEBOX& WorldByteBox, UINT& uLevel, UINT& uLevelX, UINT& uLevelZ)
	{
		UINT8 uPatternX   = WorldByteBox.uMinX ^ WorldByteBox.uMaxX,
			uPatternZ   = WorldByteBox.uMinZ ^ WorldByteBox.uMaxZ,
			uBitPattern = ALCHEMY_MAXINUM(uPatternX, uPatternZ);

		//////////////////////////////////////////////////////
		INT nHighBit      = MaxinumBit(uBitPattern);

		UINT uShift;

		uLevel = MAXINUM_TREE_DEPTH - nHighBit - MININUM_TREE_DEPTH;

		uLevel = ALCHEMY_MININUM(uLevel, m_uDepth - 1);

		uShift = MAXINUM_TREE_DEPTH - uLevel - MININUM_TREE_DEPTH;

		uLevelX = WorldByteBox.uMaxX >> uShift;
		uLevelZ = WorldByteBox.uMaxZ >> uShift;
	}

	ALCHEMY_INLINE CQuadTree::CQuadTreeNode* CQuadTree::__FindTreeNode(const QUADTREEBOX& WorldByteBox)
	{
		UINT uLevel, uLevelX, uLevelZ;

		__FindTreeNodeInfo(WorldByteBox, uLevel, uLevelX, uLevelZ);

		return __GetNodeFromLevelXZ(uLevel, uLevelX, uLevelZ);
	}
}