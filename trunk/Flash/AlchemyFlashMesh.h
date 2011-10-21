#pragma once

#include "../Graphics/AlchemyMesh.h"
#include "AlchemyFlashObjectInterface.h"

namespace alchemy
{
	class CFlashSceneManager;
	class CFlashMesh :
		public CMesh, public CFlashObjectInterface
	{
		friend class CFlashSceneManager;
	public:
		CFlashMesh(void);
		~CFlashMesh(void);

#ifdef AS3_H
		virtual AS3TYPE Init();
#endif

	protected:
		bool _Create();
	};
}