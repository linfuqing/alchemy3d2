#pragma once
/*
 *  AlchemyInstanceMeshProcessor.h
 *  IOS
 *
 *  Created by lin fuqing on 12-5-21.
 *  Copyright 2012 AstepGame. All rights reserved.
 *
 */

#include "AlchemyMeshProcessorInterface.h"

#include "../Core/AlchemySmartClass.h"
#include "../Math/AlchemyMatrix4x4.h"

namespace alchemy
{
	class CInstanceMeshProcessor : 
	public CSmartClass<IMeshProcessor>
	{
	public:
		CInstanceMeshProcessor(FLOAT fInstanceCount, const MATRIX4X4* pMatrices = ALCHEMY_NULL);
		~CInstanceMeshProcessor();
		
		IMeshProcessor* Clone();

		CMesh* GetMesh();
		CMesh* ConvertMesh(CMesh& Mesh, UINT uSubset);
		void Update(FLOAT fElapse);
		bool OnProcess(IRenderMethod& RenderMethod, UINT uRenderFlag);
		
	private:
		const MATRIX4X4* m_pDestinationMatrices;

		CMesh* m_pMesh;
		UINT m_uSubset;

		FLOAT m_fInstanceCount;
	};
}