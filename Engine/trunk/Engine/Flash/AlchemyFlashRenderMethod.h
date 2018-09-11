#pragma once

#include "../Graphics/AlchemyRenderMethod.h"
#include "AlchemyFlashObjectInterface.h"

namespace alchemy
{
	class CFlashSceneManager;
	class CFlashRenderMethod :
		public CRenderMethod, public CFlashObjectInterface
	{
		friend class CFlashSceneManager;
	public:
		CFlashRenderMethod(void);
		~CFlashRenderMethod(void);

#ifdef AS3_H
		virtual AS3TYPE Init();
#endif

	protected:
		bool _Create();
	};
}