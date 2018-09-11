#pragma once

#include "AlchemyFlashSceneNode.h"

namespace alchemy
{
	class CFlashCustomSceneNode :
		public CFlashSceneNode
	{
	public:
		CFlashCustomSceneNode(void);
		virtual ~CFlashCustomSceneNode(void);

		//bool ApplyForRender();
		//void Render(const CRenderQueue::RENDERENTRY& Entry, UINT uFlag);

#ifdef AS3_H
		void Fill(AS3TYPE Object, AS3TYPE MethodArray);
#endif

	protected:
		AS3TYPE		m_ApplyRenderMethod;
		AS3TYPE		m_RenderMethod;
	};
}