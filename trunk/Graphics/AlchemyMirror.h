#pragma once

#include "AlchemySceneNode.h"
#include "AlchemyMesh.h"
#include "AlchemyRenderMethod.h"
#include "AlchemyViewportTextureInfo.h"
#include "AlchemyFresnelBlendMethod.h"

#include "../Math/AlchemyFloat2.h"

namespace alchemy
{
	class CMirror :
		public CSceneNode
	{
	public:
		CMirror(void);
		~CMirror(void);

		CSceneManager* GetSceneManager()const;

		bool Create(
			FLOAT fWidth, 
			FLOAT fHeight, 
			UINT uWidthSegment,
			UINT uHeightSegment,
			INT nReflectionMapShift = 0, 
			UINT uNormalMap = 0,
			CGraphicsResource::HANDLE Material = 0,
			const CEffectCode::CPassDefiner::EFFECTINFO* pEffectInfos = ALCHEMY_NULL, 
			UINT uEffectInfoCount = 0,
			bool bIsYUp = false);

		void UpdateTransform();

		bool ApplyForRender();
		void Render(const CRenderQueue::RENDERENTRY& Entry, UINT uFlag);

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

		LPTEXTURE m_pReflectionMap;

		CMaterial* m_pMaterial;

		CRenderMethod* m_pRenderMethod;

		CSceneManager* m_pSceneManager;

		bool m_bIsYUp;

		CViewportTextureInfo* m_pReflectionMapInfo;

		FLOAT m_fReflectionScale;
	};

	ALCHEMY_INLINE CSceneManager* CMirror::GetSceneManager()const
	{
		return m_pSceneManager;
	}

	ALCHEMY_INLINE bool CMirror::__IsValuable()const
	{
		return m_Plane && m_pPlaneMesh && m_pMaterial && m_pReflectionMap && m_pRenderMethod && m_pSceneManager;
	}
}