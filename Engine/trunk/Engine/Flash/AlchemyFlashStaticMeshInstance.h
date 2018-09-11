#pragma once

#include "../Graphics/AlchemyStaticMeshInstance.h"
#include "AlchemyFlashAbstractSceneNode.h"

namespace alchemy
{
	class CFlashStaticMeshInstance :
		public CStaticMeshInstance, public CFlashAbstractSceneNode
	{
	public:
		CFlashStaticMeshInstance(void);
		~CFlashStaticMeshInstance(void);

		bool AddChild(CSceneNode& Child);
		bool RemoveChild(CSceneNode& Child);

		ALCHEMY_IMPLEMENT_INTERFACE

#ifdef AS3_H
			AS3TYPE Init();
#endif
	};
}