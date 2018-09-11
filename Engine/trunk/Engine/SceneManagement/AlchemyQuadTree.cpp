#include "AlchemyQuadTree.h"

using namespace alchemy;

CQuadTree::CQuadTreeObject::CQuadTreeObject(ISceneTreeObject& SceneTreeObject) :
ISceneTreeManager::CSceneTreeObjectIterator(SceneTreeObject),
m_pParentQuadTreeNode(ALCHEMY_NULL),
m_pTreeForward(ALCHEMY_NULL),
m_pTreeRear(ALCHEMY_NULL),
m_uMaskY(0)
{

}

CQuadTree::CQuadTreeObject::~CQuadTreeObject(void)
{

}

CQuadTree::CQuadTreeNode::CQuadTreeNode() :
m_pMembers(ALCHEMY_NULL),
m_pParent(ALCHEMY_NULL),
m_uLocalMaskY(0),
m_uWorldMaskY(0)
{
	m_pChildren[0] = m_pChildren[1] = m_pChildren[2] = m_pChildren[3] = ALCHEMY_NULL;
}

CQuadTree::CQuadTreeNode::~CQuadTreeNode()
{

}

bool CQuadTree::CQuadTreeNode::AddMember(CQuadTree::CQuadTreeObject& Object, const CQuadTree::QUADTREEBOX& Box)
{
	UINT32 uMaskY = CQuadTree::__GetMaskY(Box.uMinY, Box.uMaxY);

	if(Object.m_pParentQuadTreeNode != this)
	{
		if(Object.m_pParentQuadTreeNode)
			Object.m_pParentQuadTreeNode->RemoveMember(Object);

		if(m_pMembers)
		{
			Object.m_pTreeRear    = ALCHEMY_NULL;
			Object.m_pTreeForward = m_pMembers;

			m_pMembers->m_pTreeRear = &Object;
		}

		m_pMembers = &Object;

		ALCHEMY_SET_FLAG(m_uWorldMaskY, uMaskY);
		ALCHEMY_SET_FLAG(m_uLocalMaskY, uMaskY);

		if(m_pParent)
			m_pParent->__DescendantMemberAdded(uMaskY);
	}
	else
		__RebuildLocalMaskY();

	Object.m_pParentQuadTreeNode = this;
	Object.m_uMaskY              = uMaskY;

	return true;
}

bool CQuadTree::CQuadTreeNode::RemoveMember(CQuadTree::CQuadTreeObject& Object)
{
	if(Object.m_pParentQuadTreeNode != this)
	{
		ALCHEMY_DEBUG_WARNING("Error removing quad tree member");

		return false;
	}

	CQuadTreeObject* pTemp;

	if( (pTemp = Object.m_pTreeForward) != ALCHEMY_NULL )
		pTemp->m_pTreeRear = Object.m_pTreeRear;

	if( (pTemp = Object.m_pTreeRear) != ALCHEMY_NULL )
		pTemp->m_pTreeForward = Object.m_pTreeForward;

	if(m_pMembers == &Object)
		m_pMembers = Object.m_pTreeForward;

	Object.m_pTreeForward = ALCHEMY_NULL;
	Object.m_pTreeRear    = ALCHEMY_NULL;

	if(m_pParent)
		m_pParent->__DescendantMemberRemoved();

	return true;
}

void CQuadTree::CQuadTreeNode::AddMemberToSearchList(
	CQuadTree::CQuadTreeObject** ppListHead,
	CQuadTree::CQuadTreeObject** ppListTail,
	UINT32 uMaskY,
	const IClipper* pClipper)
{
	//static UINT32 s_uTESTFLAG = CRectangle3D::PLANE_FRONT | CRectangle3D::PLANE_INTERSECT;

	CQuadTreeObject* pObject;
	if( ALCHEMY_TEST_ANY(m_uLocalMaskY, uMaskY) )
	{
		for(pObject = m_pMembers; pObject; pObject = pObject->m_pTreeForward)
		{
			if( ALCHEMY_TEST_ANY(pObject->m_uMaskY, uMaskY) 
				&& ( !pClipper || pClipper->Test( pObject->m_SceneTreeObject.GetBoundingVolume() ) ) )
			{
				if(*ppListTail)
				{
					(*ppListTail)->m_pNext = pObject;
					*ppListTail            = pObject;
				}
				else
				{
					pObject->m_pNext = ALCHEMY_NULL;

					*ppListTail      = pObject;
					*ppListHead      = pObject;
				}
			}
		}
	}
}

void CQuadTree::CQuadTreeNode::AddMemberToSearchList(
	CQuadTree::CQuadTreeObject** ppListHead,
	CQuadTree::CQuadTreeObject** ppListTail,
	UINT32 uMaskY,
	const BOUNDINGBOX& BoundingBox,
	const IClipper* pClipper)
{
	CQuadTreeObject* pObject;
	const IBoundingVolume* pBoundingVolume;
	if( ALCHEMY_TEST_ANY(m_uLocalMaskY, uMaskY) )
	{
		for(pObject = m_pMembers; pObject; pObject = pObject->m_pTreeForward)
		{
			if( ALCHEMY_TEST_ANY(pObject->m_uMaskY, uMaskY) )
			{
				pBoundingVolume = &pObject->m_SceneTreeObject.GetBoundingVolume();
				if( pBoundingVolume->TestHit(BoundingBox) && ( !pClipper || pClipper->Test(*pBoundingVolume) ) )
				{
					if(*ppListTail)
					{
						(*ppListTail)->m_pNext = pObject;
						*ppListTail            = pObject;
					}
					else
					{
						pObject->m_pNext = ALCHEMY_NULL;

						*ppListTail = pObject;
						*ppListHead = pObject;
					}
			}
			}
		}
	}
}

void CQuadTree::CQuadTreeNode::__RebuildLocalMaskY()
{
	m_uLocalMaskY = 0;

	CQuadTreeObject* pObject = m_pMembers;

	while(pObject)
	{
		ALCHEMY_SET_FLAG(m_uLocalMaskY, pObject->m_uMaskY);

		pObject = pObject->m_pTreeForward;
	}

	__RebuildWorldMaskY();
}

void CQuadTree::CQuadTreeNode::__RebuildWorldMaskY()
{
	m_uWorldMaskY = m_uLocalMaskY;

	if (m_pChildren[0])
		ALCHEMY_SET_FLAG( m_uWorldMaskY, m_pChildren[0]->m_uWorldMaskY);

	if (m_pChildren[1])
		ALCHEMY_SET_FLAG( m_uWorldMaskY, m_pChildren[1]->m_uWorldMaskY);

	if (m_pChildren[2])
		ALCHEMY_SET_FLAG( m_uWorldMaskY, m_pChildren[2]->m_uWorldMaskY);

	if (m_pChildren[3])
		ALCHEMY_SET_FLAG( m_uWorldMaskY, m_pChildren[3]->m_uWorldMaskY);
}

void CQuadTree::CQuadTreeNode::__DescendantMemberAdded(UINT32 uMaskY)
{
	ALCHEMY_SET_FLAG(m_uWorldMaskY, uMaskY);

	if(m_pParent)
		m_pParent->__DescendantMemberAdded(uMaskY);
}

void CQuadTree::CQuadTreeNode::__DescendantMemberRemoved()
{
	__RebuildWorldMaskY();

	if(m_pParent)
		m_pParent->__DescendantMemberRemoved();
}

CQuadTree::CQuadTree(void) :
m_uDepth(0)
{
	for(UINT i = 0; i < MAXINUM_TREE_DEPTH; ++ i)
		m_pLevelNodes[i] = ALCHEMY_NULL;
}

CQuadTree::~CQuadTree(void)
{
	for (UINT i = 0; i < MAXINUM_TREE_DEPTH; i ++)
		ALCHEMY_DEBUG_DELETE_ARRAY(m_pLevelNodes[i]);
}

bool CQuadTree::Create(const BOUNDINGBOX& WorldBoundary, UINT uDepth)
{
	if(uDepth >= MININUM_TREE_DEPTH && uDepth <= MAXINUM_TREE_DEPTH)
	{
		ALCHEMY_DEBUG_WARNING("CQuadTree::Create: Invalid tree depth.");

		return false;
	}

	Destroy();

	m_uDepth = uDepth;

	Copy(m_Offset, WorldBoundary.Min);

	Negate(m_Offset, m_Offset);

	Size(m_Scale, WorldBoundary);

	m_Scale.x = 256.0f / m_Scale.x;
	m_Scale.y = 32.0f / m_Scale.y;
	m_Scale.z = 256.0f / m_Scale.z;

	UINT uNodeCount, i;
	for(i = 0; i < uDepth; i ++)
	{
		uNodeCount  = ALCHEMY_FLAG(i);
		uNodeCount *= uNodeCount;

		ALCHEMY_DEBUG_NEW(m_pLevelNodes[i], CQuadTreeNode[uNodeCount]);
	}

	UINT x, z, uLevelDimension, uLevelIndex;
	for(i = 0; i < uDepth; ++ i)
	{
		uLevelDimension = ALCHEMY_FLAG(i);
		uLevelIndex     = 0;

		for(z = 0; z < uLevelDimension; ++ z)
		{
			for(x = 0; x < uLevelDimension; ++ x)
			{
				m_pLevelNodes[i][uLevelIndex].Setup(
					__GetNodeFromLevelXZ(i - 1, (x >> 1)    , (z >> 1)    ),
					__GetNodeFromLevelXZ(i + 1, (x << 1)    , (z << 1)    ),
					__GetNodeFromLevelXZ(i + 1, (x << 1) + 1, (z << 1)    ),
					__GetNodeFromLevelXZ(i + 1, (x << 1)    , (z << 1) + 1),
					__GetNodeFromLevelXZ(i + 1, (x << 1) + 1, (z << 1) + 1) );

				++ uLevelIndex;
			}
		}
	}

	return true;
}

ISceneTreeManager::CSceneTreeObjectIterator* CQuadTree:: SearchObject(const IBoundingVolume& BoundingVolume, const IClipper* pClipper, ISceneTreeManager::CSceneTreeObjectIterator** ppSearchListTail)
{
	CQuadTreeObject* pListHead = ALCHEMY_NULL;
	CQuadTreeObject* pListTail = ALCHEMY_NULL;

	CQuadTreeNode* pNode;

	static BOUNDINGBOX BoundingBox;
	BoundingVolume.GetBoundingBox(BoundingBox);

	static QUADTREEBOX ByteBox;
	__Convert(ByteBox, BoundingBox, m_Offset, m_Scale);

	UINT32 uMaskY = __GetMaskY(ByteBox.uMinY, ByteBox.uMaxY);

	UINT uShift, uMinX, uMaxX, uMinZ, uMaxZ, x, z, uLevel = 0;

	bool bIsSearch = true;

	while(uLevel < m_uDepth && bIsSearch)
	{
		uShift = MAXINUM_TREE_DEPTH - uLevel - MININUM_TREE_DEPTH;

		uMinX = ByteBox.uMinX >> uShift,
		uMaxX = ByteBox.uMaxX >> uShift,
		uMinZ = ByteBox.uMinZ >> uShift,
		uMaxZ = ByteBox.uMaxZ >> uShift;

		bIsSearch = false;

		for(z = uMinZ; z <= uMaxZ; z ++)
		{
			for(x = uMinX; x <= uMaxX; x ++)
			{
				pNode = __GetNodeFromLevelXZ(uLevel, x, z);

				if (pNode->GetWorldMaskY() & uMaskY)
				{
					bIsSearch = true;

					if (z == uMinZ || z == uMaxZ || x == uMinX || x == uMaxX)
					{
						pNode->AddMemberToSearchList(
							&pListHead, 
							&pListTail, 
							uMaskY, 
							BoundingBox, 
							pClipper);
					}
					else
					{
						pNode->AddMemberToSearchList(
							&pListHead, 
							&pListTail, 
							uMaskY, 
							pClipper);
					}
				}
			}
		}

		uLevel ++;
	}

	if(ppSearchListTail)
	{
		//_AttachToSearchListTail(pListHead, *ppSearchListTail);

		if(pListHead && *ppSearchListTail)
			pListHead->_PushTo(**ppSearchListTail);

		*ppSearchListTail = pListTail;
	}

	return pListHead;
}

bool CQuadTree::AddObject(ISceneTreeObject& SceneTreeObject)
{
	if(!m_uDepth)
	{
		ALCHEMY_DEBUG_WARNING("CQuadTree::AddObject: The quad tree has not been created.");

		return false;
	}

	QUADTREEBOX ByteBox;
	BOUNDINGBOX BoundingBox;
	SceneTreeObject.GetBoundingVolume().GetBoundingBox(BoundingBox);
	__Convert(ByteBox, BoundingBox, m_Offset, m_Scale);

	CQuadTreeNode* pNode = __FindTreeNode(ByteBox);

	if(!pNode)
	{
		ALCHEMY_DEBUG_WARNING("CQuadTree::AddObject: Failed to locate quad tree node.");

		return false;
	}

	return pNode->AddMember(__CreateQuadTreeObject(SceneTreeObject), ByteBox);
}

void CQuadTree::Destroy()
{
	for (UINT i = 0; i < MAXINUM_TREE_DEPTH; i ++)
	{
		ALCHEMY_DEBUG_DELETE_ARRAY(m_pLevelNodes[i]);

		m_pLevelNodes[i] = ALCHEMY_NULL;
	}

	m_uDepth = 0;
}

CQuadTree::QUADTREEBOX& CQuadTree::__Convert(CQuadTree::QUADTREEBOX& Output, const BOUNDINGBOX BoundingBox , const FLOAT3& Offset, const FLOAT3& Scale)
{
	CBoundingBox ConvertBoundingBox;

	Copy(ConvertBoundingBox, BoundingBox);

	ConvertBoundingBox += Offset;
	ConvertBoundingBox *= Scale;

	ConvertBoundingBox.fRight -= 0.01f;
	ConvertBoundingBox.fTop   -= 0.01f;
	ConvertBoundingBox.fBack  -= 0.01f;

	ConvertBoundingBox.fRight = ALCHEMY_MAXINUM(ConvertBoundingBox.fRight, ConvertBoundingBox.fLeft  );
	ConvertBoundingBox.fTop   = ALCHEMY_MAXINUM(ConvertBoundingBox.fTop  , ConvertBoundingBox.fBottom);
	ConvertBoundingBox.fBack  = ALCHEMY_MAXINUM(ConvertBoundingBox.fBack , ConvertBoundingBox.fFront );

	Output.uMaxX = ALCHEMY_FLOOR( ConvertBoundingBox.fRight );
	Output.uMinX = ALCHEMY_FLOOR( ConvertBoundingBox.fLeft  );
	Output.uMaxY = ALCHEMY_FLOOR( ConvertBoundingBox.fTop   );
	Output.uMinY = ALCHEMY_FLOOR( ConvertBoundingBox.fBottom);
	Output.uMaxZ = ALCHEMY_FLOOR( ConvertBoundingBox.fBack  );
	Output.uMinZ = ALCHEMY_FLOOR( ConvertBoundingBox.fFront );

	Output.uMinX = ALCHEMY_CLAMP( Output.uMinX, static_cast<UINT>(0), static_cast<UINT>(254) );
	Output.uMaxX = ALCHEMY_CLAMP( Output.uMaxX, Output.uMinX + 1    , static_cast<UINT>(255) );
	Output.uMinY = ALCHEMY_CLAMP( Output.uMinY, static_cast<UINT>(0), static_cast<UINT>(30 ) );
	Output.uMaxY = ALCHEMY_CLAMP( Output.uMaxY, Output.uMinY + 1    , static_cast<UINT>(31 ) );
	Output.uMinZ = ALCHEMY_CLAMP( Output.uMinZ, static_cast<UINT>(0), static_cast<UINT>(254) );
	Output.uMaxZ = ALCHEMY_CLAMP( Output.uMaxZ, Output.uMinZ + 1    , static_cast<UINT>(255) );

	return Output;
}

CQuadTree::CQuadTreeObject& CQuadTree::__CreateQuadTreeObject(ISceneTreeObject& SceneTreeObject)
{
	CQuadTreeObject* pQuadTreeObject;
	ALCHEMY_DEBUG_NEW( pQuadTreeObject, CQuadTreeObject(SceneTreeObject) );
	m_Objects.Add(pQuadTreeObject);

	return *pQuadTreeObject;
}