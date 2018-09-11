#pragma once

#include "AlchemyRenderer.h"
#include "AlchemyRenderQueue.h"

#include "AlchemyGraphicsResource.h"

namespace alchemy
{
	struct IRenderMethod;
	struct IMeshProcessor;
	class CMeshInstance : 
		public CRenderer, public CSmartClass<CRenderQueue::ICallBackToRender>
	{
		typedef struct  
		{
			const MATRIX4X4*			pMatrix;
			IMeshProcessor*				pMeshProcessor;
			UINT16						uSubset;
			CGraphicsResource::HANDLE	Material;
		}SUBMESH,*LPSUBMESH;
	public:
		CMeshInstance(void);
		~CMeshInstance(void);

		bool Create(CGraphicsResource::HANDLE Effect, IRenderMethod& RenderMethod);

		UINT AddSubMesh(const MATRIX4X4* pMatrix, IMeshProcessor& MeshProcessor, UINT16 uSubset, CGraphicsResource::HANDLE Material = 0);
		void RemoveSubMesh(UINT uHandle);

		void Destroy();

		virtual bool ApplyForRender();
		virtual void Render(const CRenderQueue::RENDERENTRY& Entry, const CRenderQueue::RENDERKEY& Key, UINT uFlag);

	private:
		bool				m_bIsDirty;
		CVector<SUBMESH>	m_SubMeshes;

		CEffect*			m_pEffect;
		IRenderMethod*		m_pRenderMethod;
		CSceneManager*		m_pSceneManager;
	};
}