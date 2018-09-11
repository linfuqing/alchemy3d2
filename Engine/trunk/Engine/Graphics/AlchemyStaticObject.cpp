#include "AlchemyStaticObject.h"
#include "AlchemySceneManager.h"

using namespace alchemy;

CStaticObject::CStaticObject()
:m_pMesh(ALCHEMY_NULL),
m_uSubset(0),
m_pMaterial(ALCHEMY_NULL),
m_pEffect(ALCHEMY_NULL),
m_pRenderMethod(ALCHEMY_NULL),
m_uRenderPass(0),
m_pBoundingVolume(ALCHEMY_NULL), 
m_pSceneManager(ALCHEMY_NULL)
{
	
}

CStaticObject::~CStaticObject()
{

}

bool CStaticObject::Create(
			CGraphicsResource::HANDLE Mesh, 
			UINT uSubset, 
			CGraphicsResource::HANDLE Effect, 
			IRenderMethod* pRenderMethod, 
			UINT uRenderPass, 
			CGraphicsResource::HANDLE Material,
			CSceneManager* pSceneManager)
{
	Destroy();

	m_pSceneManager = pSceneManager ? pSceneManager : CSceneManager::GetInstance();

	SetMesh(Mesh);
	SetMeshSubset(uSubset);
	SetEffect(Effect);
	SetRenderMethod(pRenderMethod);
	SetRenderPass(uRenderPass);
	SetMaterial(Material);

	return true;
}

bool CStaticObject::SetEffect( CGraphicsResource::HANDLE Effect )
{
	if(!m_pSceneManager)
		return false;

	CEffect* pEffect = static_cast<CEffect*>( m_pSceneManager->GetRenderSystem().GetResource(Effect, CGraphicsResource::EFFECT) );
	if(pEffect)
		pEffect->AddRef();

	if(m_pEffect)
		m_pEffect->Release();

	m_pEffect = pEffect;

	if(m_pMesh && m_pEffect)
	{
		UINT uEventType;

		if( GetEventType(uEventType, SCENE_DIRTY) )
		{
			CEvent Event(uEventType);

			DispatchEvent(Event);
		}
	}

	return true;
}

void CStaticObject::SetRenderMethod( IRenderMethod* pRenderMethod )
{
	if(pRenderMethod)
		pRenderMethod->AddRef();

	if(m_pRenderMethod)
		m_pRenderMethod->Release();

	m_pRenderMethod = pRenderMethod;

	if(m_pMesh && m_pEffect && m_pSceneManager)
	{
		UINT uEventType;

		if( GetEventType(uEventType, SCENE_DIRTY) )
		{
			CEvent Event(uEventType);

			DispatchEvent(Event);
		}
	}
}

void CStaticObject::SetRenderPass( UINT uRenderPass )
{
	if(m_pEffect)
	{
		if( uRenderPass < m_pEffect->GetTotalPass() )
		{
			m_uRenderPass = uRenderPass;

			if(m_pMesh && m_pEffect && m_pSceneManager)
			{
				UINT uEventType;

				if( GetEventType(uEventType, SCENE_DIRTY) )
				{
					CEvent Event(uEventType);

					DispatchEvent(Event);
				}
			}
		}
	}
}

bool CStaticObject::SetMaterial( CGraphicsResource::HANDLE Material )
{
	if(!m_pSceneManager)
		return false;

	CMaterial* pMaterial = static_cast<CMaterial*>( m_pSceneManager->GetResource(Material, CGraphicsResource::MATERIAL) );
	if(pMaterial)
		pMaterial->AddRef();

	if(m_pMaterial)
		m_pMaterial->Release();

	m_pMaterial = pMaterial;

	if(m_pMesh && m_pEffect && m_pSceneManager)
	{
		UINT uEventType;

		if( GetEventType(uEventType, SCENE_DIRTY) )
		{
			CEvent Event(uEventType);

			DispatchEvent(Event);
		}
	}

	return true;
}

bool CStaticObject::SetMesh(CGraphicsResource::HANDLE Mesh)
{
	if(!m_pSceneManager)
		return false;

	CMesh* pMesh = static_cast<CMesh*>( m_pSceneManager->GetResource(Mesh, CGraphicsResource::MESH) );
	if(pMesh)
	{
		pMesh = _ConvertMesh(pMesh);
		if(pMesh)
			pMesh->AddRef();
	}

	if(m_pMesh)
		m_pMesh->Release();

	m_pMesh = pMesh;

	if(m_pMesh && m_pEffect && m_pSceneManager)
	{
		UINT uEventType;

		if( GetEventType(uEventType, SCENE_DIRTY) )
		{
			CEvent Event(uEventType);

			DispatchEvent(Event);
		}
	}

	return true;
}

void CStaticObject::SetMeshSubset(UINT uSubset)
{
	m_uSubset = uSubset;

	if(m_pMesh && m_pBoundingVolume)
	{
		const CMesh::ATTRIBUTERANGE* pAttributeRange = m_pMesh->GetAttributeTable().Get(m_uSubset);
		if(pAttributeRange)
			m_pMesh->ComputeBoundingVolume(*m_pBoundingVolume, pAttributeRange);
	}

	if(m_pMesh && m_pEffect && m_pSceneManager)
	{
		UINT uEventType;

		if( GetEventType(uEventType, SCENE_DIRTY) )
		{
			CEvent Event(uEventType);

			DispatchEvent(Event);
		}
	}
}

void CStaticObject::SetBoundingVolume(IBoundingVolume* pBoundingVolume)
{
	if(pBoundingVolume)
		pBoundingVolume->AddRef();

	if(m_pBoundingVolume)
		m_pBoundingVolume->Release();

	m_pBoundingVolume = pBoundingVolume;

	if(m_pMesh && m_pBoundingVolume)
	{
		const CMesh::ATTRIBUTERANGE* pAttributeRange = m_pMesh->GetAttributeTable().Get(m_uSubset);
		if(pAttributeRange)
			m_pMesh->ComputeBoundingVolume(*m_pBoundingVolume, pAttributeRange);
	}

	if(m_pMesh && m_pEffect && m_pSceneManager)
	{
		UINT uEventType;

		if( GetEventType(uEventType, SCENE_DIRTY) )
		{
			CEvent Event(uEventType);

			DispatchEvent(Event);
		}
	}
}

void CStaticObject::UpdateTransform()
{
	CSceneNode::UpdateTransform();

	if(m_pBoundingVolume)
		m_pBoundingVolume->SetTransform( GetWorldMatrix() );
}

bool CStaticObject::Intersect(
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
	if(m_pMesh)
	{
		CRay TransformedRay;

		TransformNormal(TransformedRay.Direction, GetTransposeWorldMatrix(), Ray.Direction);
		Normalize(TransformedRay.Direction, TransformedRay.Direction);

		TransformCoord(TransformedRay.Origin, GetInverseWorldMatrix(), Ray.Origin);

		if( m_pMesh->Intersect(
			TransformedRay,
			pIntersectInfos, 
			puIntersectInfoCount, 
			bIsCheckHitOn, 
			bHitOnMask,
			bIsCheckFront, 
			bFrontMask, 
			pHitPoints, 
			pfFaceNormals) )
		{
			FLOAT3 Distance;
			CMesh::LPINTERSECTINFO pIntersectInfo;
			UINT uIntersectInfoCount = puIntersectInfoCount ? *puIntersectInfoCount : 0, i;
			const CMatrix4x4& WorldMatrix = GetWorldMatrix();
			for(i = 0;i < uIntersectInfoCount; ++ i)
			{
				pIntersectInfo = &pIntersectInfos[i];
				Distance.x = TransformedRay.Origin.x + pIntersectInfo->t * TransformedRay.Direction.x;
				Distance.y = TransformedRay.Origin.y + pIntersectInfo->t * TransformedRay.Direction.y;
				Distance.z = TransformedRay.Origin.z + pIntersectInfo->t * TransformedRay.Direction.z;

				TransformCoord(Distance, WorldMatrix, Distance);

				Distance.x -= Ray.Origin.x;
				Distance.y -= Ray.Origin.y;
				Distance.z -= Ray.Origin.z;

				pIntersectInfo->t = pIntersectInfo->t < 0.0f ? - GetLength(Distance) : GetLength(Distance);
			}

			if(pHitPoints)
			{
				for(i = 0; i < uIntersectInfoCount; ++ i)
					TransformCoord(pHitPoints[i], WorldMatrix, pHitPoints[i]);
			}

			if(pfFaceNormals)
			{
				for(i = 0; i < uIntersectInfoCount; ++ i)
					TransformNormal(pfFaceNormals[i], WorldMatrix, pfFaceNormals[i]);
			}

			return true;
		}
	}

	return false;
}

bool CStaticObject::ApplyForRender(UINT uRenderPass, UINT uRenderList)
{
	if(!m_pSceneManager || !m_pEffect || !m_pMesh)
		return false;

	CRenderQueue& RenderQueue = m_pSceneManager->GetRenderQueue();
	CRenderQueue::RENDERENTRY& RenderEntry = RenderQueue.LockRenderEntry(*this, &m_bIVisible, m_pBoundingVolume);

	RenderEntry.uRenderTarget	   = 0;
	RenderEntry.uEffect            = m_pEffect->GetResourceHandle();
	RenderEntry.uRenderPass        = uRenderPass == ~0 ? m_uRenderPass : uRenderPass;
	RenderEntry.uVertexDeclaration = m_pMesh->GetVertex().GetVertexDeclaration().GetResourceHandle();
	RenderEntry.uModel             = m_pMesh->GetResourceHandle();
	RenderEntry.uModelParameter    = m_uSubset;
	RenderEntry.uUserData          = 0;

	m_pSceneManager->GetRenderQueue().UnlockRenderEntry(uRenderList);

	return true;
}

void CStaticObject::Render(const CRenderQueue::RENDERENTRY& Entry, const CRenderQueue::RENDERKEY& Key, UINT uFlag)
{
	if(!m_pSceneManager || !m_pMesh)
		return;

	CRenderQueue& RenderQueue = m_pSceneManager->GetRenderQueue();
	CRenderSystem& RenderSystem = m_pSceneManager->GetRenderSystem();

	if(m_pMaterial && m_pRenderMethod)
	{
		UINT uTextureCount;
		const UINT* pTextureAlphaIndexes = m_pRenderMethod->GetTextureInfo(Key.uRenderPass, Key.uRenderTargetIndex, uTextureCount);
		if(pTextureAlphaIndexes)
		{
			const LPTEXTURE* ppTexture;
			for(UINT i = 0; i < uTextureCount; ++ i)
			{
				ppTexture = m_pMaterial->GetTextures().Get( pTextureAlphaIndexes[i] );

				if( ppTexture && *ppTexture && ALCHEMY_TEST_BIT( (*ppTexture)->GetUsage(), CTexture::RENDER_TARGET ) && (*ppTexture)->GetResourceHandle() == RenderQueue.GetRenderTarget() )
					return;
			}
		}
	}

	if(ALCHEMY_TEST_BIT(uFlag, CRenderQueue::PARENT) || ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_EFFECT) || ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_EFFECT_PASS) )
	{
		if( ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_EFFECT) && Entry.uEffect )
			RenderSystem.SetShader(Entry.uEffect);

		if( ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_EFFECT_PASS) && Entry.uRenderPass )
			RenderSystem.SetShader(Entry.uRenderPass);
	}

	if(m_pRenderMethod && !m_pRenderMethod->Activate(Key.uRenderPass, Key.uRenderTargetIndex) )
		return;

	RenderSystem.ResetTexture();

	RenderSystem.RegisterRenderState();

	if(m_pRenderMethod)
		m_pRenderMethod->CommitChange( &GetWorldMatrix(), m_pMaterial);

	if( ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_VERTEX_DECLARATION) && ( !Entry.uVertexDeclaration || !RenderSystem.SetVertexDeclaration(Entry.uVertexDeclaration) ) )
		return;

	m_pMesh->DrawSubset( Entry.uModelParameter, ALCHEMY_TEST_BIT(uFlag, CRenderQueue::MODEL), ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_VERTEX_DECLARATION) && !Entry.uVertexDeclaration);

	RenderSystem.RestoreRenderState();
}

void CStaticObject::Destroy()
{
	if(m_pMesh)
	{
		m_pMesh->Release();
		m_pMesh = ALCHEMY_NULL;
	}

	if(m_pMaterial)
	{
		m_pMaterial->Release();
		m_pMaterial = ALCHEMY_NULL;
	}

	if(m_pEffect)
	{
		m_pEffect->Release();
		m_pEffect = ALCHEMY_NULL;
	}

	if(m_pRenderMethod)
	{
		m_pRenderMethod->Release();
		m_pRenderMethod = ALCHEMY_NULL;
	}

	m_uSubset = m_uRenderPass = 0;

	m_pSceneManager = ALCHEMY_NULL;
}

CMesh* CStaticObject::_ConvertMesh(CMesh* pSourceMesh)
{
	if(pSourceMesh && m_pBoundingVolume)
	{
		const CMesh::ATTRIBUTERANGE* pAttributeRange = pSourceMesh->GetAttributeTable().Get(m_uSubset);
		if(pAttributeRange)
			pSourceMesh->ComputeBoundingVolume(*m_pBoundingVolume, pAttributeRange);
	}

	return pSourceMesh;
}