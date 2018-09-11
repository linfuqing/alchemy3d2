#pragma once

#include "../Graphics/AlchemyMaterial.h"
#include "AlchemyFlashObjectInterface.h"

namespace alchemy
{
	class CFlashSceneManager;
	class CFlashMaterial :
		 public CMaterial, public CFlashObjectInterface
	{
		friend class CFlashSceneManager;
	public:
		CFlashMaterial(void);
		~CFlashMaterial(void);

#ifdef AS3_H
		virtual AS3TYPE Init();
#endif
	protected:
		bool _Create();

	};
}