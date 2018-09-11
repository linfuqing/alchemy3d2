#pragma once

#include "AlchemyRenderGroupInterface.h"
#include "AlchemyGraphicsResource.h"

#include "../Math/AlchemyPlane.h"
#include "../Core/AlchemySmartClass.h"

namespace alchemy
{
	class CSceneManager;
	class CRenderQueue;
	class CSceneNode;
	class CCamera;

	class CReflectRender :
		public CSmartClass<IRenderGroup>
	{
	public:
		CReflectRender();
		virtual ~CReflectRender();

		bool Create(CGraphicsResource::HANDLE RenderTarget, const CPlane& Plane, CSceneNode** ppSceneNodes = ALCHEMY_NULL, UINT uSceneNodeNum = 0 );

		void Destroy();

		void Reset();

		void RenderAll();

	private:
		CGraphicsResource::HANDLE	m_RenderTarget;

		const CPlane*				m_pPlane;

		CSceneNode**				m_ppSceneNodes;
		UINT						m_uSceneNodeNum;

		UINT						m_uRenderList;
		CSceneManager*				m_pSceneManager;
		CRenderQueue*				m_pRenderQueue;
	};
}