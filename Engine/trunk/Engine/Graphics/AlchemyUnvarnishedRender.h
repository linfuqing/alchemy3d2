#pragma once

#include "AlchemyRenderGroupInterface.h"
#include "AlchemyARGBColor.h"
#include "AlchemyGraphicsResource.h"

#include "../Core/AlchemyPool.h"
#include "../Core/AlchemySmartClass.h"

namespace alchemy
{
	class CSceneNode;
	class CCamera;
	class CRenderQueue;
	class CSceneManager;

	class CUnvarnishedRender : 
		public CSmartClass<IRenderGroup>
	{
		typedef struct  
		{
			CSceneNode* pSceneNode;
			UINT		RenderPass;
		}RENDEROBJECT,*LPRENDEROBJECT;

	public:
		CUnvarnishedRender();
		virtual ~CUnvarnishedRender();

		bool Create(ARGBCOLOR ClearColor, UINT uClearFlag, CGraphicsResource::HANDLE hRenderTarget, const CCamera* pCamera = ALCHEMY_NULL);

		UINT AddRenderObject(CSceneNode& SceneNode, UINT uRenderPass);

		void RemoveRenderObject( UINT uRenderObjectHandle );

		void Reset();

		void RenderAll();

		void Destroy();

	private:
		UINT						m_uClearFlag;
		ARGBCOLOR					m_ClearColor;
		CPool<RENDEROBJECT>			m_RenderObjects;
		CGraphicsResource::HANDLE	m_RenderTarget;
		const CCamera*				m_pCamera;

		UINT					m_uRenderList;
		CSceneManager*			m_pSceneManager;
		CRenderQueue*			m_pRenderQueue;
	};
}