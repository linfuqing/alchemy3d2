#include "AlchemyBone.h"
#include "AlchemySceneNodeFactoryInterface.h"

using namespace alchemy;

CBone::CBone(void) :
m_bIsBoneParent(false),
m_pNext(ALCHEMY_NULL),
m_bIsRegisterAnimationController(false)
{
	m_CoordinateSystem = CSceneNode::IMMEDIATE;
}

CBone::~CBone(void)
{
}

void CBone::UpdateTransform()
{
	CSceneNode* pParent = GetParent();
	if(m_bIsBoneParent && pParent)
		Multiply4x3(m_WorldMatrix, pParent->GetWorldMatrix(), m_LocalMatrix);
	else
		Copy4x3(m_WorldMatrix, m_LocalMatrix);
}

void CBone::Update()
{
	if(m_bIsRegisterAnimationController)
	{
		ALCHEMY_SET_BIT(m_uDirtyFlag, SCALE);
		ALCHEMY_SET_BIT(m_uDirtyFlag, ROTATION);
		ALCHEMY_SET_BIT(m_uDirtyFlag, TRANSLATION);

		ALCHEMY_CLEAR_BIT(m_uDirtyFlag, MATRIX);

		m_bIsTransformDirty = true;
	}

	CSceneNode::Update();
}

bool CBone::AddChild(CSceneNode& Child)
{
	if( CSceneNode::AddChild(Child) )
	{
		CBone* pBone = dynamic_cast<CBone*>(&Child);

		if(pBone)
		{
			pBone->m_bIsBoneParent = true;

			CBone& Temp = pBone->__GetTreeTail();

			Temp.m_pNext = m_pNext;

			m_pNext = pBone;
		}

		return true;
	}

	return false;
}

bool CBone::RemoveChild(CSceneNode& Child)
{
	if( CSceneNode::RemoveChild(Child) )
	{
		CBone* pBone = dynamic_cast<CBone*>(&Child);

		if(pBone)
		{
			pBone->m_bIsBoneParent = false;

			CBone* pTemp = this;
			while(pTemp->m_pNext)
			{
				if(pTemp->m_pNext == pBone)
					break;

				pTemp = pTemp->m_pNext;
			}

			CBone& Tail = pBone->__GetTreeTail();

			pTemp->m_pNext = Tail.m_pNext;

			Tail.m_pNext = ALCHEMY_NULL;
		}

		return true;
	}

	return false;
}

CSceneNode& CBone::Clone(ISceneNodeFactory& SceneNodeFactory)const
{
	CBone& Bone = SceneNodeFactory.CreateBone();

	Bone.SetName( GetName() );

	CTransform::Clone(Bone);

	return Bone;
}

CBone* CBone::GetChildByName(const CHAR* pName)
{
	CBone* pBone = m_pNext, * pEnd = __GetTreeTail().m_pNext;

	while(pBone != pEnd)
	{
		if(static_cast<const CBone*>(pBone)->GetName() == pName)
			return pBone;

		pBone = pBone->m_pNext;
	}

	return ALCHEMY_NULL;
}

void CBone::RegisterAnimationController(CAnimationController& AnimationController)
{
	CBone* pBone = this, * pEnd = __GetTreeTail().m_pNext;

	while(pBone != pEnd)
	{
		AnimationController.RegisterAnimationOutput(pBone->GetName(), pBone->m_LocalMatrix);

		pBone = pBone->m_pNext;
	}

	m_bIsRegisterAnimationController = true;
}

CBone& CBone::__GetTreeTail()
{
	CBone* pBone = this;

	while( pBone->m_pNext && pBone->m_pNext->GetParent() == static_cast<CSceneNode*>(this) )
		pBone = &pBone->m_pNext->__GetTreeTail();

	return *pBone;
}