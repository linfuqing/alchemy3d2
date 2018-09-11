#pragma once

#include "AlchemyFlashAbstractSceneNode.h"
#include "../Graphics/AlchemyParticleController.h"

namespace alchemy
{
	class CFlashParticleSystem : 
		public PARTICLESYSTEM, public CFlashAbstractSceneNode
	{
	public:
		CFlashParticleSystem(void);
		~CFlashParticleSystem(void);

		bool AddChild(CSceneNode& Child);
		bool RemoveChild(CSceneNode& Child);

		ALCHEMY_IMPLEMENT_INTERFACE
#ifdef AS3_H
			AS3TYPE Init();
#endif
	};
}