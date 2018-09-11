#pragma once

#include "AlchemyRenderGroupInterface.h"
#include "AlchemyARGBColor.h"
#include "AlchemyGraphicsResource.h"

#include "../Core/AlchemySmartClass.h"
#include "../Core/AlchemyPool.h"

namespace alchemy
{
	class CSceneNode;
	class CSceneManager;
	class CCamera;
	class CRenderQueue;

	class CCommonRender :
		public CSmartClass<IRenderGroup>
	{
		typedef struct  
		{
			CSceneNode*						pSceneNode;
			UINT							RenderPass;
		}RENDEROBJECT,*LPRENDEROBJECT;

		typedef struct  
		{
			ARGBCOLOR				  ClearColor;
			UINT					  ClearFlag;
			CGraphicsResource::HANDLE RenderTarget;
			UINT					  RenderList;
			CPool<RENDEROBJECT>		  RenderObjects;
		}RENDERTARGETGROUP,*LPRENDERTARGETGROUP;

	public:
		CCommonRender();
		virtual ~CCommonRender();

		bool Create( CCamera* pCamera = ALCHEMY_NULL );

		UINT ApplyRenderTargetGroup( ARGBCOLOR ClearColor, UINT uClearFlag, CGraphicsResource::HANDLE RenderTarget );
		void RemoveRenderTargetGroup( UINT uGroupHandle );

		UINT AddRenderObject( UINT uGroupHandle, CSceneNode& SceneNode, UINT uRenderPass );
		void RemoveRenderObject( UINT uGroupHandle, UINT uRenderObjectHandle );

		void Destroy();

		void Reset();

		void RenderAll();

	private:
		CCamera*						m_pCamera;
		CPool<RENDERTARGETGROUP>		m_GroupObjects;

		CRenderQueue*					m_pRenderQueue;
		CSceneManager*					m_pSceneManager;
	};
}