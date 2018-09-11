#pragma once

#include "AlchemySceneObject.h"

namespace alchemy
{
	class CEntity
	{
	public:
		CEntity(void);
		~CEntity(void);

		CEffectCode::CPassDefiner* BeginEffect();
		bool EndEffect(CGraphicsResource::HANDLE Mesh, CGraphicsResource::HANDLE Material = 0);

		bool AttachToScene(ISceneNode& SceneNode);
		bool DeattachFromScene();

	private:
		CSceneObject* m_pSceneObject;
	};
}