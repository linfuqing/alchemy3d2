#pragma once

#include "AlchemySceneNode.h"
#include "AlchemyMesh.h"
#include "AlchemyRenderMethod.h"
//#include "AlchemyViewportTextureInfo.h"

#include "../Math/AlchemyFloat2.h"
#include "../Math/AlchemyPlane.h"

namespace alchemy
{
	class CMirror :
		public CSceneNode
	{

	public:
		CMirror(void);
		~CMirror(void);

		CSceneManager* GetSceneManager();

		CMesh* GetPlaneMesh();

		bool Create(
			FLOAT fWidth, 
			FLOAT fHeight, 
			UINT uWidthSegment,
			UINT uHeightSegment,
			UINT uReflectionMap,
			UINT uNormalMap = 0,
			CGraphicsResource::HANDLE Material = 0,
			const CEffectCode::CPassDefiner::EFFECTINFO* pEffectInfos = ALCHEMY_NULL, 
			UINT uEffectInfoCount = 0,
			bool bIsYUp = false);

		const CPlane& GetPlane() const;

		void UpdateTransform();

		bool ApplyForRender(UINT uRenderPass = ~0, UINT uRenderList = CRenderQueue::NORMAL);
		void Render(const CRenderQueue::RENDERENTRY& Entry, const CRenderQueue::RENDERKEY& Key, UINT uFlag);

		void Destroy();

	protected:
		CFloat2 m_Translation0;
		CFloat2 m_Translation1;
		CFloat2 m_Translation2;
		CFloat2 m_Translation3;

	private:
		void __Destroy();

		bool __IsValuable()const;

		CPlane m_Plane;

		CMesh* m_pPlaneMesh;

		//LPTEXTURE m_pReflectionMap;

		CMaterial* m_pMaterial;

		CEffect* m_pEffect;

		CRenderMethod* m_pRenderMethod;

		CSceneManager* m_pSceneManager;

		bool m_bIsYUp;

		FLOAT m_fReflectionScale;
	};

	ALCHEMY_INLINE CSceneManager* CMirror::GetSceneManager()
	{
		return m_pSceneManager;
	}

	ALCHEMY_INLINE CMesh* CMirror::GetPlaneMesh()
	{
		return m_pPlaneMesh;
	}

	ALCHEMY_INLINE bool CMirror::__IsValuable()const
	{
		return m_pPlaneMesh && m_pMaterial && m_pEffect && m_pRenderMethod && m_pSceneManager;
	}

	ALCHEMY_INLINE const CPlane& CMirror::GetPlane() const
	{
		return m_Plane;
	}
}