#pragma once

#include "AlchemyCompilerInterface.h"
#include "AlchemyEffect.h"
#include "AlchemyMesh.h"

namespace alchemy
{
	class CFullScreenDrawer
	{
	public:
		CFullScreenDrawer(CSceneManager& SceneManager);
		~CFullScreenDrawer(void);

		CSceneManager& GetSceneManager();

		const CMesh* GetMesh()const;
		const CEffect* GetEffect()const;

		bool IsValuable()const;

		bool Create();

		void Draw(CGraphicsResource::HANDLE Texture, UINT uFlag = ~0);

		void Destroy();

	private:
		CSceneManager& m_SceneManager;

		CMesh* m_pFullScreenPlane;

		CEffect* m_pEffect;
	};

	ALCHEMY_INLINE CSceneManager& CFullScreenDrawer::GetSceneManager()
	{
		return m_SceneManager;
	}

	ALCHEMY_INLINE const CMesh* CFullScreenDrawer::GetMesh()const
	{
		return m_pFullScreenPlane;
	}

	ALCHEMY_INLINE const CEffect* CFullScreenDrawer::GetEffect()const
	{
		return m_pEffect;
	}

	ALCHEMY_INLINE bool CFullScreenDrawer::IsValuable()const
	{
		return m_pFullScreenPlane && m_pEffect;
	}
}