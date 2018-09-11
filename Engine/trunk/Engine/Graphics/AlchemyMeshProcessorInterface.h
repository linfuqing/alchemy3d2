#pragma once

#include "../Core/AlchemyUpdaterInterface.h"

namespace alchemy
{
	class CMesh;
	struct IRenderMethod;

	struct IMeshProcessor : public IUpdater
	{
		virtual IMeshProcessor* Clone() = 0;

		virtual CMesh* GetMesh() = 0;
		virtual CMesh* ConvertMesh(CMesh& Mesh, UINT uSubset) = 0;
		//virtual UINT   GetSubset() = 0;
		virtual void Update( FLOAT fElapse ) = 0;
		virtual bool OnProcess(IRenderMethod& RenderMethod, UINT uRenderFlag) = 0;
	};
}