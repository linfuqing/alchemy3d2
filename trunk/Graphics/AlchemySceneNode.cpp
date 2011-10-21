#include "AlchemySceneNode.h"
#include "../Core/AlchemyDebug.h"
#include "AlchemySceneManager.h"
#include "../Math/AlchemyVector3.h"

using namespace alchemy;

CSceneNode::CSceneNode(void) :
CTransform(m_LocalMatrix),
m_CoordinateSystem(RELATICE),
m_pPrevious(ALCHEMY_NULL),
m_pNext(ALCHEMY_NULL),
m_pParent(ALCHEMY_NULL),
m_pChild(ALCHEMY_NULL),
m_uChildrenCount(0),
m_bIsDynamic(false)
{
	m_LocalMatrix.Identity();
	m_WorldMatrix.Identity();
}

CSceneNode::~CSceneNode(void)
{
}

void CSceneNode::Destroy()
{

}

bool CSceneNode::IsDynamic()const
{
	return m_bIsDynamic;
}

bool CSceneNode::AddChild(CSceneNode& Child)
{
	if(&Child == this)
	{
		ALCHEMY_DEBUG_WARNING("It is not the child.");

		return false;
	}

	if( Child.m_pParent && !Child.m_pParent->RemoveChild(Child) )
		return false;

	if(m_pChild)
		m_pChild->m_pPrevious = &Child;

	Child.m_pParent = this;

	Child.m_pNext = m_pChild;

	m_pChild = &Child;

	Child.m_bIsTransformDirty = true;

	++ m_uChildrenCount;

	return true;
}

bool CSceneNode::RemoveChild(CSceneNode& Child)
{
	if(Child.m_pParent != this)
	{
		ALCHEMY_DEBUG_WARNING("It is not the child.");

		return false;
	}

	if(m_pChild == &Child)
		m_pChild = Child.m_pNext;
	else if(Child.m_pPrevious)
		Child.m_pPrevious->m_pNext = Child.m_pNext;
	else
		return false;

	Child.m_pParent   = ALCHEMY_NULL;
	Child.m_pPrevious = ALCHEMY_NULL;
	Child.m_pNext     = ALCHEMY_NULL;

	Child.m_bIsTransformDirty = true;

	-- m_uChildrenCount;

	return true;
}

void CSceneNode::UpdateTransform()
{
	CTransform::UpdateTransform();

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
}

void CSceneNode::Update()
{
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

bool CSceneNode::ApplyForRender()
{
	return true;
}

void CSceneNode::Render(const CRenderQueue::RENDERENTRY& Entry, UINT uFlag)
{
}