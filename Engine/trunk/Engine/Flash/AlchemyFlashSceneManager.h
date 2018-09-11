#pragma once

#include "../Graphics/AlchemySceneManager.h"
#include "AlchemyFlashContext3D.h"
#include "AlchemyFlashSceneNodeFactory.h"

namespace alchemy
{
	class CFlashSceneManager :
		public CSceneManager, public CFlashGraphicsResourcePool
	{
	public:
		CFlashSceneManager(CFlashContext3D& Context3D);
		~CFlashSceneManager(void);

		void Render();

#ifdef AS3_H
		AS3TYPE Init();
		void Fill(AS3TYPE Object, AS3TYPE MethodArray);
#endif

	protected:
		CGraphicsResource* _Create(CGraphicsResource::TYPE Type);

#ifdef AS3_H
		bool _RemoveResource(CGraphicsResource::HANDLE Handle, CGraphicsResource::TYPE Type);
#endif
	};
}
