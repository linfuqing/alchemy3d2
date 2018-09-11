#pragma once

#include "../Graphics/AlchemyLight.h"
#include "AlchemyFlashObjectInterface.h"

namespace alchemy
{
	class CFlashSceneManager;
	class CFlashLight :
		public CLight, public CFlashObjectInterface
	{
		friend class CFlashSceneManager;
	public:
		CFlashLight(void);
		~CFlashLight(void);

#ifdef AS3_H
		virtual AS3TYPE Init();
#endif

	protected:
		bool _Create();
	};
}