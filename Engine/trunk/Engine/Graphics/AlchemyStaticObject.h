#pragma once

#include "AlchemySceneNode.h"

#include "AlchemyMaterial.h"

namespace alchemy
{
	class CSceneManager;
	class CStaticObject :
		public CSceneNode
	{
	public:
		CStaticObject();
		virtual ~CStaticObject();

		bool Create(
			CGraphicsResource::HANDLE Mesh, 
			UINT uSubset, 
			CGraphicsResource::HANDLE Effect, 
			IRenderMethod* pRenderMethod, 
			UINT uRenderPass, 
			CGraphicsResource::HANDLE Material = 0,
			CSceneManager* pSceneManager = ALCHEMY_NULL);

		bool SetEffect( CGraphicsResource::HANDLE Effect );
		CEffect* GetEffect() const;

		void SetRenderMethod( IRenderMethod* pRenderMethod );
		IRenderMethod* GetRenderMethod() const;

		void SetRenderPass( UINT uRenderPass );
		UINT GetRenderPass() const;

		bool SetMesh(CGraphicsResource::HANDLE Mesh);
		CMesh* GetMesh() const;

		void SetMeshSubset(UINT uSubset);
		UINT GetMeshSubset() const;

		bool SetMaterial( CGraphicsResource::HANDLE Material );
		CMaterial* GetMaterial() const;

		void SetBoundingVolume(IBoundingVolume* pBoundingVolume);
		IBoundingVolume* GetBoundingVolume()const;

		CSceneManager* GetSceneManager()const;

		void UpdateTransform();

		bool Intersect(
			const RAY& Ray, 
			CMesh::LPINTERSECTINFO pIntersectInfos, 
			PUINT puIntersectInfoCount, 
			bool bIsCheckHitOn,
			bool bHitOnMask,
			bool bIsCheckFront, 
			bool bFrontMask, 
			LPFLOAT3 pHitPoints, 
			LPFLOAT3 pfFaceNormals);

		bool ApplyForRender(UINT uRenderPass = ~0, UINT uRenderList = CRenderQueue::NORMAL);

		void Render(const CRenderQueue::RENDERENTRY& Entry, const CRenderQueue::RENDERKEY& Key, UINT uFlag);

		virtual void Destroy();

	protected:
		virtual CMesh* _ConvertMesh(CMesh* pSourceMesh);
		
	private:
		CMesh*			m_pMesh;
		UINT			m_uSubset;

		CMaterial*		m_pMaterial;
		CEffect*		m_pEffect;
		IRenderMethod*	m_pRenderMethod;
		UINT			m_uRenderPass;

		IBoundingVolume* m_pBoundingVolume;

		CSceneManager*	m_pSceneManager;
	};

	ALCHEMY_INLINE CEffect* CStaticObject::GetEffect() const
	{
		return m_pEffect;
	}

	ALCHEMY_INLINE IRenderMethod* CStaticObject::GetRenderMethod() const
	{
		return m_pRenderMethod;
	}

	ALCHEMY_INLINE UINT CStaticObject::GetRenderPass() const
	{
		return m_uSubset;
	}

	ALCHEMY_INLINE CMesh* CStaticObject::GetMesh() const
	{
		return m_pMesh;
	}

	ALCHEMY_INLINE UINT CStaticObject::GetMeshSubset() const
	{
		return m_uSubset;
	}

	ALCHEMY_INLINE CMaterial* CStaticObject::GetMaterial() const
	{
		return m_pMaterial;
	}

	ALCHEMY_INLINE IBoundingVolume* CStaticObject::GetBoundingVolume()const
	{
		return m_pBoundingVolume;
	}

	ALCHEMY_INLINE CSceneManager* CStaticObject::GetSceneManager()const
	{
		return m_pSceneManager;
	}
}