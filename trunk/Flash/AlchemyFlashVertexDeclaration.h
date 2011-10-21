#pragma once

#include "../Graphics/AlchemyVertexDeclaration.h"
#include "AlchemyFlashObject.h"

namespace alchemy
{
	class CFlashContext3D;
	class CFlashVertexDeclaration :
		public CVertexDeclaration
	{
		friend class CFlashContext3D;
	public:
		CFlashVertexDeclaration(void);
		~CFlashVertexDeclaration(void);

	protected:
		bool _Create();
	};
}
