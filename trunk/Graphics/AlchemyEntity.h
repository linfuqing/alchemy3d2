#pragma once

#include "AlchemySceneObject.h"

namespace alchemy
{
	class CSceneManager;
	class CEntity
	{
		friend class CSceneManager;
	public:
		CEntity(void);
		~CEntity(void);

	private:
		CMesh* m_pMesh;
		CSceneObject* m_pScene;
	};
}