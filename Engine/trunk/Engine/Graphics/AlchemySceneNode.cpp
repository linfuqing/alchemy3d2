#include "AlchemySceneNode.h"
#include "AlchemySceneNodeFactoryInterface.h"

#include "../Core/AlchemyDebug.h"
#include "../Math/AlchemyVector3.h"
#include "../SceneManagement/AlchemyAxisAlignedBoundingBox.h"

using namespace alchemy;

CSceneNode::CSceneNode(void) :
CTransform(m_LocalMatrix),
m_CoordinateSystem(RELATICE),
m_pPrevious(ALCHEMY_NULL),
m_pNext(ALCHEMY_NULL),
m_pParent(ALCHEMY_NULL),
m_pChild(ALCHEMY_NULL),
m_uChildrenCount(0),
m_bIVisible(true), 
m_bIsInverseWorldMatrixDirty(false), 
m_bIsTransposeWorldMatrixDirty(false)
{
	m_LocalMatrix.Identity();
	m_WorldMatrix.Identity();

	m_InverseWorldMatrix.Identity();
	m_TransposeWorldMatrix.Identity();
}

CSceneNode::~CSceneNode(void)
{
}

const CMatrix4x4& CSceneNode::GetInverseWorldMatrix()
{
	if(m_bIsInverseWorldMatrixDirty)
	{
		m_InverseWorldMatrix = m_WorldMatrix;

		m_InverseWorldMatrix.Invert3x4();

		m_bIsInverseWorldMatrixDirty = false;
	}

	return m_InverseWorldMatrix;
}

const CMatrix4x4& CSceneNode::GetTransposeWorldMatrix()
{
	if(m_bIsTransposeWorldMatrixDirty)
	{
		m_TransposeWorldMatrix = m_WorldMatrix;

		m_TransposeWorldMatrix.Transpose();

		m_bIsTransposeWorldMatrixDirty = false;
	}

	return m_TransposeWorldMatrix;
}

bool CSceneNode::AddChild(CSceneNode& Child)
{
	if(&Child == this)
	{
		ALCHEMY_DEBUG_WARNING("CSceneNode::AddChild: It is not the child.");

		return false;
	}

	if( Child.m_pParent && !Child.m_pParent->RemoveChild(Child) )
		return false;

	if(m_pChild)
		m_pChild->m_pPrevious = &Child;

	Child.m_pParent = this;

	Child.m_pNext = m_pChild;

	Child.m_pPrevious = ALCHEMY_NULL;

	m_pChild = &Child;

	Child.m_bIsTransformDirty = true;

	++ m_uChildrenCount;

	UINT uEventType;

	if( GetEventType(uEventType, ADD_CHILD) )
	{
		CEvent Event(uEventType);

		DispatchEvent(Event);
	}

	if( GetEventType(uEventType, ADDED) )
	{
		CEvent Event(uEventType);

		Child.DispatchEvent(Event);
	}

	return true;
}

bool CSceneNode::RemoveChild(CSceneNode& Child)
{
	CSceneNode* pParent = Child.m_pParent;
	while(pParent && pParent != this)
		pParent = pParent->m_pParent;

	if(!pParent)
	{
		ALCHEMY_DEBUG_WARNING("CSceneNode::RemoveChild: It is not the child.");

		return false;
	}

	UINT uEventType;

	if( GetEventType(uEventType, REMOVE_CHILD) )
	{
		CEvent Event(uEventType);

		DispatchEvent(Event);
	}

	if( GetEventType(uEventType, REMOVED) )
	{
		CEvent Event(uEventType);

		Child.DispatchEvent(Event);
	}

	if(Child.m_pParent->m_pChild == &Child)
		Child.m_pParent->m_pChild = Child.m_pNext;
	else if(Child.m_pPrevious)
		Child.m_pPrevious->m_pNext = Child.m_pNext;

	if(Child.m_pNext)
		Child.m_pNext->m_pPrevious = Child.m_pPrevious;

	Child.m_pParent   = ALCHEMY_NULL;
	Child.m_pPrevious = ALCHEMY_NULL;
	Child.m_pNext     = ALCHEMY_NULL;

	Child.m_bIsTransformDirty = true;

	-- m_uChildrenCount;

	return true;
}

void CSceneNode::SetVisible(bool visible)
{
	m_bIVisible = visible;

	CSceneNode* pNode = m_pChild;

	while(pNode)
	{
		pNode->SetVisible(visible);

		pNode = pNode->m_pNext;
	}
}

void CSceneNode::UpdateTransform()
{
	Copy4x3(m_WorldMatrix, m_LocalMatrix);

	CSceneNode* pParent = ALCHEMY_NULL;
	
	if(m_pParent && m_pParent->m_CoordinateSystem == IMMEDIATE)
	{
		Multiply4x3(m_WorldMatrix, m_pParent->m_WorldMatrix, m_WorldMatrix);

		pParent = m_pParent->m_pParent;
	}
	else
		pParent = m_pParent;

	while(pParent)
	{
		if(pParent->m_CoordinateSystem == RELATICE || pParent->m_CoordinateSystem == ABSOLUTE)
		{
			Multiply4x3(m_WorldMatrix, pParent->m_WorldMatrix, m_WorldMatrix);

			if(pParent->m_CoordinateSystem == RELATICE)
				break;
		}

		pParent = pParent->m_pParent;
	}

	m_bIsInverseWorldMatrixDirty = true;
	m_bIsTransposeWorldMatrixDirty = true;
}

void CSceneNode::Update()
{
	if(!m_bIVisible)
		return;

	CTransform::Update();

	CSceneNode* pNode = m_pChild;

	if(m_bIsTransformDirty)
	{
		while(pNode)
		{
			pNode->m_bIsTransformDirty = true;

			pNode = pNode->m_pNext;
		}

		pNode = m_pChild;
	}

	while(pNode)
	{
		pNode->Update();

		pNode = pNode->m_pNext;
	}

	m_bIsTransformDirty = false;
}

void CSceneNode::AddedToRenderQueue()
{
	CSceneNode* pNode = m_pChild;

	while(pNode)
	{
		pNode->AddedToRenderQueue();

		pNode = pNode->m_pNext;
	}

	ApplyForRender();
}

bool CSceneNode::DispatchEvent(IEventDispatcher::CEvent& Event)
{
	bool bResult = CEventDispatcher<IEventDispatcher>::DispatchEvent(Event);

	if( m_pParent && Event.IsBubbles() )
		m_pParent->DispatchEvent(Event);

	return bResult;
}

CSceneNode& CSceneNode::CloneTree(ISceneNodeFactory& SceneNodeFactory)const
{
	CSceneNode& SceneNode = Clone(SceneNodeFactory);

	SceneNode.m_Name = m_Name;

	if(m_pChild)
	{
		CSceneNode* pChild = m_pChild;

		while(pChild->m_pNext)
			pChild = pChild->m_pNext;

		while(pChild)
		{
			SceneNode.AddChild( pChild->CloneTree(SceneNodeFactory) );

			pChild = pChild->m_pPrevious;
		}
	}

	return SceneNode;
}

bool CSceneNode::IntersectTree(
							   const RAY& Ray, 
							   CMesh::LPINTERSECTINFO pIntersectInfos, 
							   PUINT puIntersectInfoCount, 
							   bool bIsCheckHitOn,
							   bool bHitOnMask,
							   bool bIsCheckFront, 
							   bool bFrontMask, 
							   LPFLOAT3 pHitPoints, 
							   LPFLOAT3 pfFaceNormals)
{
	UINT uMaxIntersectInfoCount = puIntersectInfoCount ? *puIntersectInfoCount : 0, uIntersectInfoCount = uMaxIntersectInfoCount, uCurrentIntersectInfoCount = 0;
	uMaxIntersectInfoCount = uMaxIntersectInfoCount ? uMaxIntersectInfoCount : (puIntersectInfoCount ? ~0 : 1);
	puIntersectInfoCount = puIntersectInfoCount ? puIntersectInfoCount : &uIntersectInfoCount;

	if( Intersect(
		Ray, 
		pIntersectInfos, 
		puIntersectInfoCount, 
		bIsCheckHitOn, 
		bHitOnMask, 
		bIsCheckFront, 
		bFrontMask, 
		pHitPoints, 
		pfFaceNormals) )
	{
		uCurrentIntersectInfoCount += *puIntersectInfoCount;

		if(uMaxIntersectInfoCount <= uCurrentIntersectInfoCount)
		{
			*puIntersectInfoCount = uCurrentIntersectInfoCount;

			return true;
		}
	}

	*puIntersectInfoCount = uMaxIntersectInfoCount - uCurrentIntersectInfoCount;

	CSceneNode* pChild = m_pChild;
	while(pChild)
	{
		if( pChild->IntersectTree(
			Ray, 
			pIntersectInfos ? pIntersectInfos + uCurrentIntersectInfoCount : ALCHEMY_NULL, 
			puIntersectInfoCount, 
			bIsCheckHitOn, 
			bHitOnMask, 
			bIsCheckFront, 
			bFrontMask, 
			pHitPoints ? pHitPoints + uCurrentIntersectInfoCount : ALCHEMY_NULL, 
			pfFaceNormals ? pfFaceNormals + uCurrentIntersectInfoCount : ALCHEMY_NULL) )
		{
			uCurrentIntersectInfoCount += *puIntersectInfoCount;

			if(uMaxIntersectInfoCount <= uCurrentIntersectInfoCount)
			{
				*puIntersectInfoCount = uCurrentIntersectInfoCount;

				return true;
			}
		}

		*puIntersectInfoCount = uMaxIntersectInfoCount - uCurrentIntersectInfoCount;

		pChild = pChild->m_pNext;
	}

	*puIntersectInfoCount = uCurrentIntersectInfoCount;

	return uCurrentIntersectInfoCount ? true : false;
}

CSceneNode&  CSceneNode::Clone(ISceneNodeFactory& SceneNodeFactory)const
{
	CSceneNode& SceneNode = SceneNodeFactory.CreateSceneNode();

	SceneNode.m_Name = m_Name;

	CTransform::Clone( static_cast<CTransform&>(SceneNode) );

	return SceneNode;
}

bool CSceneNode::Intersect(const RAY& Ray, 
						   CMesh::LPINTERSECTINFO pIntersectInfos, 
						   PUINT puIntersectInfoCount, 
						   bool bIsCheckHitOn,
						   bool bHitOnMask,
						   bool bIsCheckFront, 
						   bool bFrontMask, 
						   LPFLOAT3 pHitPoints, 
						   LPFLOAT3 pfFaceNormals)
{
	if(puIntersectInfoCount)
		*puIntersectInfoCount = 0;

	return false;
}

CSceneNode* CSceneNode::GetChild(const CHAR* pcName)
{
	CSceneNode* pChild = m_pChild, * pCurrent;
	while(pChild)
	{
		if(pChild->m_Name == pcName)
			return pChild;

		pCurrent = pChild->GetChild(pcName);
		if(pCurrent)
			return pCurrent;

		pChild = pChild->m_pNext;
	}

	return ALCHEMY_NULL;
}

bool CSceneNode::ApplyForRender(UINT uRenderPass, UINT uRenderList)
{
	return true;
}

void CSceneNode::Render(const CRenderQueue::RENDERENTRY& Entry, const CRenderQueue::RENDERKEY& Key, UINT uFlag)
{
}