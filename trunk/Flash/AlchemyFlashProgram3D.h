#pragma once

#include "AlchemyFlashObjectInterface.h"

#include "../Graphics/AlchemyShader.h"

namespace alchemy
{
	class CFlashContext3D;
	class CFlashProgram3D :
		public CShader, public CFlashObjectInterface
	{
		friend class CFlashContext3D;
	public:
		CFlashProgram3D(void);
		~CFlashProgram3D(void);

#ifdef AS3_H
		AS3TYPE Init();
#endif
	protected:
		bool _Create();
	};
}