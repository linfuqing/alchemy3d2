#pragma once

#include "../Graphics/AlchemyVertexDeclaration.h"

namespace alchemy
{
	class CFlasccVertexDeclaration :
		public CVertexDeclaration
	{
	public:
		CFlasccVertexDeclaration(void);
		~CFlasccVertexDeclaration(void);

	protected:
		bool _Create();
	};
}