#include "AlchemyRectangleInstance.h"

#include "AlchemySceneManager.h"

#include "../Math/AlchemyVector2.h"

using namespace alchemy;

CRectangleInstance::CSprite::CSprite(CRectangleInstance& RectangleInstance, UINT uTransformIndex) : 
m_RectangleInstance(RectangleInstance), 
m_uTransformIndex(uTransformIndex), 
m_uDepth(0), 
m_uChildrenCount(0), 
m_pParent(ALCHEMY_NULL), 
m_pChild(ALCHEMY_NULL), 
m_pNext(ALCHEMY_NULL), 
m_pPrevious(ALCHEMY_NULL)
{

}

CRectangleInstance::CSprite::~CSprite()
{

}

bool CRectangleInstance::CSprite::SetPosition(FLOAT x, FLOAT y)
{
	CSceneManager* pSceneManager = m_RectangleInstance.GetSceneManager();
	if(pSceneManager)
	{
		UINT uTransformIndex = __GetTransformIndex();
		if(uTransformIndex < m_RectangleInstance.m_uSpriteCount)
		{
			UINT uWidth, uHeight;
			pSceneManager->GetRenderSystem().GetViewport(&uWidth, &uHeight);

			m_Position.x = m_RectangleInstance.m_fWidth * 0.5f;
			m_Position.y = - m_RectangleInstance.m_fHeight * 0.5f;

			if(m_RectangleInstance.m_Format == CRectangleInstance::NORMAL)
			{
				m_Position.x *= m_Transform.x;
				m_Position.y *= m_Transform.y;
			}
			else
				Transform(m_Position, m_Transform, m_Position);

			m_Position.x += x / uWidth * 2.0f - 1.0f;
			m_Position.y += 1.0f - y / uHeight * 2.0f;

			m_RectangleInstance.SetTransform(uTransformIndex, m_RectangleInstance.m_uPositionXInfoIndex, &m_Position.x);
			m_RectangleInstance.SetTransform(uTransformIndex, m_RectangleInstance.m_uPositionYInfoIndex, &m_Position.y);

			m_Position.x = x;
			m_Position.y = y;

			return true;
		}
	}

	return false;
}

bool CRectangleInstance::CSprite::SetTransform(const FLOAT* pfValues)
{
	UINT uTransformIndex = __GetTransformIndex();
	if(uTransformIndex < m_RectangleInstance.m_uSpriteCount)
	{
		if(m_RectangleInstance.m_Format == CRectangleInstance::NORMAL)
		{
			m_Transform.x = pfValues[0];
			m_Transform.y = pfValues[1];

			m_RectangleInstance.SetTransform(uTransformIndex, m_RectangleInstance.m_uScaleXInfoIndex, &m_Transform.x);
			m_RectangleInstance.SetTransform(uTransformIndex, m_RectangleInstance.m_uScaleYInfoIndex, &m_Transform.y);
		}
		else
		{
			m_Transform.x = pfValues[0];
			m_Transform.y = pfValues[1];
			m_Transform.x = pfValues[2];
			m_Transform.y = pfValues[3];

			m_RectangleInstance.SetTransform(uTransformIndex, m_RectangleInstance.m_uTransformInfoIndex, m_Transform.fValue);
		}

		return SetPosition(m_Position.x, m_Position.y);
	}

	return false;
}

bool CRectangleInstance::CSprite::AddChild(CSprite& Sprite, UINT uCount)
{
	if( uCount && (&m_RectangleInstance == &Sprite.m_RectangleInstance) )
	{
		UINT uTransformIndex = Sprite.m_uTransformIndex, uDepth = Sprite.m_uDepth, uCurrentCount = 0;
		CSprite* pSprite = &Sprite, * pParent = Sprite.m_pParent, * pFinal;
		while(pSprite)
		{
			if(pSprite->m_pParent == pParent && uCurrentCount < uCount)
			{
				++ uCurrentCount;

				pSprite->m_pParent = this;

				pSprite->m_uDepth = m_uDepth + 1;

				pFinal = pSprite;
			}
			else
				pSprite->m_uTransformIndex = uTransformIndex ++;

			pSprite = pSprite->m_pNext;
		}

		bool bIsDirty = false;
		if(pFinal->m_pNext)
		{
			if ( !m_RectangleInstance.m_pDirtySprite || ( pFinal->m_pNext->__GetTransformIndex() < m_RectangleInstance.m_pDirtySprite->__GetTransformIndex() ) )
				m_RectangleInstance.m_pDirtySprite = pFinal->m_pNext;

			pFinal->m_pNext->m_pPrevious = ALCHEMY_NULL;
		}
		else
			bIsDirty = true;

		CVector<CRectangleInstance::DEPTHTRANSFORMINDEX>::CIterator Iterator = m_RectangleInstance.m_DepthTransformIndices.GetIterator(uDepth);
		if( CVector<CRectangleInstance::DEPTHTRANSFORMINDEX>::IsValuable(Iterator) )
		{
			CRectangleInstance::LPDEPTHTRANSFORMINDEX pDepthTransformIndex = &(*Iterator);
			if(pDepthTransformIndex->pHead == &Sprite)
				pDepthTransformIndex->pHead = pFinal->m_pNext;

			-- pDepthTransformIndex->uTransformIndex;
			++ Iterator;

			while( CVector<CRectangleInstance::DEPTHTRANSFORMINDEX>::IsValuable(Iterator) )
			{
				if(bIsDirty)
				{
					pDepthTransformIndex = &(*Iterator);

					-- pDepthTransformIndex->uTransformIndex;

					if(pDepthTransformIndex->pHead)
					{
						if ( !m_RectangleInstance.m_pDirtySprite || ( pDepthTransformIndex->pHead->__GetTransformIndex() < m_RectangleInstance.m_pDirtySprite->__GetTransformIndex() ) )
							m_RectangleInstance.m_pDirtySprite = pDepthTransformIndex->pHead;

						bIsDirty = false;
					}
				}
				else
					-- (Iterator->uTransformIndex);

				++ Iterator;
			}
		}

		if(pParent && pParent->m_pChild == &Sprite)
			pParent->m_pChild = ( pFinal->m_pNext && (pFinal->m_pNext->m_pParent == pParent) ) ? pFinal->m_pNext : ALCHEMY_NULL;

		if(Sprite.m_pPrevious)
			Sprite.m_pPrevious->m_pNext = pFinal->m_pNext;

		pSprite = this;
		while(pSprite && !pSprite->m_pChild)
			pSprite = pSprite->m_pPrevious;

		if(pSprite)
		{
			CSprite* pChild = pSprite->m_pChild;
			while(pChild->m_pNext && pChild->m_pNext->m_pParent == pSprite)
				pChild = pChild->m_pNext;

			Sprite.m_pPrevious = pChild;
			pFinal->m_pNext = pChild->m_pNext;
			pChild->m_pNext = &Sprite;

			uTransformIndex = pChild->m_uTransformIndex + 1;

			pSprite = &Sprite;
			while(pSprite)
			{
				pSprite->m_uTransformIndex = uTransformIndex ++;

				pSprite = pSprite->m_pNext;
			}

			Iterator = m_RectangleInstance.m_DepthTransformIndices.GetIterator(Sprite.m_uDepth);
			while( CVector<CRectangleInstance::DEPTHTRANSFORMINDEX>::IsValuable(Iterator) )
			{
				Iterator->uTransformIndex += uTransformIndex;
				++ Iterator;
			}
		}
		else
		{
			CRectangleInstance::LPDEPTHTRANSFORMINDEX pDepthTransformIndex = m_RectangleInstance.m_DepthTransformIndices.Get(Sprite.m_uDepth);
			pFinal->m_pNext = pDepthTransformIndex ? pDepthTransformIndex->pHead : ALCHEMY_NULL;
			uTransformIndex = 0;
			pSprite = &Sprite;
			while(pSprite)
			{
				pSprite->m_uTransformIndex = uTransformIndex ++;

				pSprite = pSprite->m_pNext;
			}

			if(pDepthTransformIndex)
			{
				pDepthTransformIndex->pHead                   = &Sprite;
				pDepthTransformIndex->uTransformIndex += m_uTransformIndex;
			}
			else
			{
				CRectangleInstance::DEPTHTRANSFORMINDEX DepthTransformIndex;
				DepthTransformIndex.pHead = &Sprite;
				DepthTransformIndex.uTransformIndex = m_RectangleInstance.m_DepthTransformIndices[m_uDepth].uTransformIndex + uTransformIndex; //////
				m_RectangleInstance.m_DepthTransformIndices.Add(DepthTransformIndex);
			}

			Sprite.m_pPrevious = ALCHEMY_NULL;
		}

		if ( !m_RectangleInstance.m_pDirtySprite || ( Sprite.__GetTransformIndex() < m_RectangleInstance.m_pDirtySprite->__GetTransformIndex() ) )
			m_RectangleInstance.m_pDirtySprite = &Sprite;

		m_uChildrenCount += uCurrentCount;

		if(Sprite.m_pChild)
			Sprite.AddChild(*Sprite.m_pChild, m_uChildrenCount);

		return true;
	}

	return false;
}

bool CRectangleInstance::CSprite::RemoveChild(CRectangleInstance::CSprite& Sprite)
{
	if(&Sprite.m_RectangleInstance == &m_RectangleInstance && Sprite.m_pParent == this)
	{
		UINT uTransformIndex = Sprite.m_uTransformIndex;
		CSprite* pSprite = Sprite.m_pNext;
		while(pSprite)
		{
			pSprite->m_uTransformIndex = uTransformIndex ++;

			pSprite = pSprite->m_pNext;
		}

		bool bIsDirty = false;
		if(Sprite.m_pNext)
		{
			if ( !m_RectangleInstance.m_pDirtySprite || ( Sprite.m_pNext->__GetTransformIndex() < m_RectangleInstance.m_pDirtySprite->__GetTransformIndex() ) )
				m_RectangleInstance.m_pDirtySprite = Sprite.m_pNext;

			Sprite.m_pNext->m_pPrevious = ALCHEMY_NULL;
		}
		else
			bIsDirty = true;

		CVector<CRectangleInstance::DEPTHTRANSFORMINDEX>::CIterator Iterator = m_RectangleInstance.m_DepthTransformIndices.GetIterator(Sprite.m_uDepth);
		if( CVector<CRectangleInstance::DEPTHTRANSFORMINDEX>::IsValuable(Iterator) )
		{
			CRectangleInstance::LPDEPTHTRANSFORMINDEX pDepthTransformIndex = &(*Iterator);
			if(pDepthTransformIndex->pHead == &Sprite)
				pDepthTransformIndex->pHead = Sprite.m_pNext;

			-- pDepthTransformIndex->uTransformIndex;
			++ Iterator;

			while( CVector<CRectangleInstance::DEPTHTRANSFORMINDEX>::IsValuable(Iterator) )
			{
				if(bIsDirty)
				{
					pDepthTransformIndex = &(*Iterator);

					-- pDepthTransformIndex->uTransformIndex;

					if(pDepthTransformIndex->pHead)
					{
						if ( !m_RectangleInstance.m_pDirtySprite || ( pDepthTransformIndex->pHead->__GetTransformIndex() < m_RectangleInstance.m_pDirtySprite->__GetTransformIndex() ) )
							m_RectangleInstance.m_pDirtySprite = pDepthTransformIndex->pHead;

						bIsDirty = false;
					}
				}
				else
					-- (Iterator->uTransformIndex);

				++ Iterator;
			}
		}

		if(Sprite.m_pPrevious)
			Sprite.m_pPrevious->m_pNext = Sprite.m_pNext;

		Sprite.m_pParent = this;

		Sprite.m_uDepth = 0;

		Sprite.m_uTransformIndex = m_RectangleInstance.m_DepthTransformIndices[0].uTransformIndex ++;

		if ( !m_RectangleInstance.m_pDirtySprite || ( Sprite.__GetTransformIndex() < m_RectangleInstance.m_pDirtySprite->__GetTransformIndex() ) )
			m_RectangleInstance.m_pDirtySprite = &Sprite;

		-- m_uChildrenCount;

		return true;
	}

	return false;
}

bool CRectangleInstance::CSprite::__SetTransform()
{
	return SetTransform(m_Transform.fValue);
}

CRectangleInstance::CRectangleInstance(void) : 
m_fWidth(0.0f), 
m_fHeight(0.0f), 
m_ppSprites(ALCHEMY_NULL), 
m_uSpriteCount(0), 
m_pDirtySprite(ALCHEMY_NULL), 
m_Format(NORMAL), 
m_uPositionXInfoIndex(0), 
m_uPositionYInfoIndex(0), 
m_uScaleXInfoIndex(0), 
m_uScaleYInfoIndex(0), 
m_uTransformInfoIndex(0)
{
}

CRectangleInstance::~CRectangleInstance(void)
{
	for(UINT i = 0; i < m_uSpriteCount; ++ i)
		ALCHEMY_DEBUG_DELETE(m_ppSprites[i]);

	ALCHEMY_DEBUG_DELETE_ARRAY(m_ppSprites);
}

bool CRectangleInstance::Create(
			FLOAT fWidth, 
			FLOAT fHeight,
			CGraphicsResource::HANDLE Effect, 
			IRenderMethod* pRenderMethod, 
			CGraphicsResource::HANDLE Texture, 
			UINT uRenderPass, 
			CSceneManager* pSceneManager)
{
	pSceneManager = pSceneManager ? pSceneManager : CSceneManager::GetInstance();
	if(pSceneManager)
	{
		const CVertexDeclaration::VERTEXELEMENT VertexElement[] = 
		{
			{0, 0                         , CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION, 0}, 
			{0, CVertexDeclaration::FLOAT4, CVertexDeclaration::FLOAT2, CVertexDeclaration::TEXCOORD, 1}, 
		};

		UINT uWidth, uHeight;
		pSceneManager->GetRenderSystem().GetViewport(&uWidth, &uHeight);

		CMeshManager::HANDLE Plane = ALCHEMY_MESH_MANAGER.CreatePlane( fWidth /= (uWidth * 0.5f), fHeight /= (uHeight * 0.5f), 1, 1, 1, 1, false, VertexElement, sizeof(VertexElement) / sizeof(CVertexDeclaration::VERTEXELEMENT) );
		CMesh* pMesh = pSceneManager->CreateMesh(Plane);
		ALCHEMY_MESH_MANAGER.DestroyMeshData(Plane);
		if(pMesh)
		{
			CMaterial* pMaterial = pSceneManager->CreateMaterial();
			if( pMaterial && pMaterial->AddTexture(Texture) )
			{
				if( CStaticObject::Create(pMesh->GetResourceHandle(), 0, Effect, pRenderMethod, uRenderPass, pMaterial->GetResourceHandle(), pSceneManager) )
				{
					m_fWidth = fWidth;
					m_fHeight = fHeight;

					pMesh->Release();

					pMaterial->Release();

					return true;
				}
			}
			else
			{
				pMesh->Release();

				if(pMaterial)
					pMaterial->Release();

				ALCHEMY_DEBUG_WARNING("CRectangleInstance::Create: Material has been created fail.");
			}
		}
		else
			ALCHEMY_DEBUG_WARNING("CRectangleInstance::Create: Mesh has been created fail.");
	}

	return false;
}

CRectangleInstance::CSprite** CRectangleInstance::SetInstances(UINT uInstanceCount, FORMAT Format)
{
	if(!uInstanceCount)
		return ALCHEMY_NULL;

	const CGeometryInstance::TRANSFORMINFO* pTransformInfo;
	UINT uTransformInfoCount;
	if(Format == NORMAL)
	{
		static const CGeometryInstance::TRANSFORMINFO s_TRANSFORM_INFO[] = 
		{
			{POSITION_X, 0}, 
			{POSITION_Y, 0}, 
			{SCALE_X, 0}, 
			{SCALE_Y, 0}
		};

		pTransformInfo = s_TRANSFORM_INFO;
		uTransformInfoCount = 4;

		m_Format = NORMAL;

		m_uPositionXInfoIndex = 0;
		m_uPositionYInfoIndex = 1;
		m_uScaleXInfoIndex = 2;
		m_uScaleYInfoIndex = 3;
	}
	else
	{
		static const CGeometryInstance::TRANSFORMINFO s_TRANSFORM_INFO[] = 
		{
			{QUATERNION, 0}, 
			{POSITION_X, 0}, 
			{POSITION_Y, 0}
		};

		pTransformInfo = s_TRANSFORM_INFO;
		uTransformInfoCount = 3;

		m_Format = TRANSFORM;

		m_uPositionXInfoIndex = 1;
		m_uPositionYInfoIndex = 2;
		m_uTransformInfoIndex = 0;
	}

	if( CGeometryInstance::SetInstances( static_cast<FLOAT>(uInstanceCount), uInstanceCount, pTransformInfo, uTransformInfoCount) )
	{
		UINT i;
		for(i = 0; i < m_uSpriteCount; ++ i)
			ALCHEMY_DEBUG_DELETE(m_ppSprites[i]);

		ALCHEMY_DEBUG_DELETE_ARRAY(m_ppSprites);

		ALCHEMY_DEBUG_NEW(m_ppSprites, CSprite*[uInstanceCount]);
		CSprite* pSprite, * pPrevious = ALCHEMY_NULL;
		for(i = 0; i < uInstanceCount; ++ i)
		{
			ALCHEMY_DEBUG_NEW( pSprite, CSprite(*this, i) );

			if(pPrevious)
			{
				pPrevious->m_pNext = pSprite;

				pSprite->m_pPrevious = pPrevious;
			}

			m_ppSprites[i] = pPrevious = pSprite;
		}

		m_uSpriteCount = uInstanceCount;

		m_DepthTransformIndices.Clear();

		DEPTHTRANSFORMINDEX DepthTransformIndex;
		DepthTransformIndex.pHead = m_ppSprites[0];
		DepthTransformIndex.uTransformIndex = uInstanceCount;
		m_DepthTransformIndices.Add(DepthTransformIndex);

		m_pDirtySprite = ALCHEMY_NULL;

		return m_ppSprites;
	}

	return ALCHEMY_NULL;
}

CRectangleInstance::CSprite* CRectangleInstance::Test(FLOAT x, FLOAT y)const
{
	if(m_Format == CRectangleInstance::NORMAL)
	{
		CVector<CSprite*>::CIterator Iterator = m_Sprites.GetIterator(0);
		while()
		{

		}
	}
}

void CRectangleInstance::Update()
{
	if(m_pDirtySprite)
	{
		CSprite* pSprite;

		CVector<CRectangleInstance::DEPTHTRANSFORMINDEX>::CIterator Iterator = m_DepthTransformIndices.GetIterator(m_pDirtySprite->m_uDepth + 1);
		while( CVector<CRectangleInstance::DEPTHTRANSFORMINDEX>::IsValuable(Iterator) )
		{
			pSprite = Iterator->pHead;
			while(pSprite)
			{
				pSprite->__SetTransform();

				pSprite = pSprite->m_pNext;
			}

			++ Iterator;
		}

		pSprite = m_pDirtySprite;
		while(pSprite)
		{
			pSprite->__SetTransform();

			pSprite = pSprite->m_pNext;
		}

		m_pDirtySprite = ALCHEMY_NULL;
	}

	CGeometryInstance::Update();
}