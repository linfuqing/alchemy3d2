#pragma once

#include "AlchemyRenderGroupInterface.h"

#include "../Core/AlchemySmartClass.h"

namespace alchemy
{
	class CMRTRender : 
		CSmartClass<IRenderGroup>
	{
	public:
		CMRTRender();
		virtual ~CMRTRender();

		void RenderAll();
		void SetDirty(bool bDirty);
	};
}