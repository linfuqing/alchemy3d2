#pragma once

#include "AlchemyFlashObject.h"
#include "../Graphics/AlchemySceneNode.h"

namespace alchemy
{
	class CFlashAbstractSceneNode :
		public CFlashObject, virtual public ISceneNode
	{
	public:
		CFlashAbstractSceneNode(void);
		~CFlashAbstractSceneNode(void);

		bool AddChild(CSceneNode& Child);
		bool RemoveChild(CSceneNode& Child);

#ifdef AS3_H
		void Fill(AS3TYPE Object, AS3TYPE MehtodArray);

	private:
		AS3TYPE m_AddSceneNodeMethod;
		AS3TYPE m_RemoveSceneNodeMethod;
#endif
	};
}