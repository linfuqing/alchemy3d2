#pragma once

#include "../Graphics/AlchemyShader.h"

namespace alchemy
{
	class CFlasccContext3D;
	class CFlasccProgram3D :
		public CShader
	{
		friend class CFlasccContext3D;
	public:
		CFlasccProgram3D(void);
		~CFlasccProgram3D(void);

	protected:
		bool _Create();
	};
}