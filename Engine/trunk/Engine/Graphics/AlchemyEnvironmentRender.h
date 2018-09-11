#pragma once

#include "AlchemyRenderGroupInterface.h"

#include "AlchemyGraphicsResource.h"

#include "../Math/AlchemyMatrix4x4.h"

#include "../Core/AlchemySmartClass.h"

namespace alchemy
{
	class CCamera;
	class CSceneNode;
	class CSceneManager;

	class CEnvironmentRender : 
		public CSmartClass<IRenderGroup>
	{
	public:
		CEnvironmentRender();
		virtual ~CEnvironmentRender();

		bool Create(CGraphicsResource::HANDLE RenderTarget, const CMatrix4x4& WorldMatrix, CSceneNode** ppNodes = ALCHEMY_NULL, UINT uSceneNodeNum = 0 );

		void Reset();

		void RenderAll();

	private:
		UINT											m_uRenderList;

		CCamera*										m_pCamera;
		const CMatrix4x4*								m_pWorldMatrix;
		CGraphicsResource::HANDLE						m_RenderTarget;

		UINT											m_uSceneNodeNum;
		CSceneNode**									m_ppSceneNodes;

		CSceneManager*									m_pSceneManager;
	};
}