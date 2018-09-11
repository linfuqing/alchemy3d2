#pragma once

#include "AlchemyFlashAbstractSceneNode.h"

#include "../Graphics/AlchemyMirror.h"

namespace alchemy
{
	class CFlashMirror :
		public CMirror, public CFlashAbstractSceneNode
	{
	public:
		CFlashMirror(void);
		~CFlashMirror(void);

		bool AddChild(CSceneNode& Child);
		bool RemoveChild(CSceneNode& Child);

		ALCHEMY_IMPLEMENT_INTERFACE
#ifdef AS3_H
		AS3TYPE Init();
#endif
	};
}