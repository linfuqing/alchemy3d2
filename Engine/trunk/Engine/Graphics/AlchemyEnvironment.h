#pragma once

#include "AlchemySceneManager.h"

namespace alchemy
{
	class CEnvironment :
		public CSceneNode
	{
	public:
		CEnvironment();
		virtual ~CEnvironment();

		bool Create(CGraphicsResource::HANDLE Mesh, CGraphicsResource::HANDLE Material, UINT uEnvironmentSize, CSceneManager& SceneManager, CCompilerInterface& Compiler, CEffectCode::CPassDefiner::EFFECTINFO * pPassInfo = ALCHEMY_NULL, UINT uPassCount = 0);

		FLOAT GetReflection() const;
		void  SetReflection(FLOAT fRelction);

		virtual bool ApplyForRender();
		virtual void Render(const CRenderQueue::RENDERENTRY& Entry, UINT uFlag);

		virtual void Destroy();

	protected:
		bool __IsValuable() const;

	protected:
		CMesh*				m_pMesh;
		CMaterial*			m_pMaterial;
		LPTEXTURE			m_pEnvironmentTexture;
		CRenderMethod*		m_pRenderMethod;
		CVertexDeclaration*	m_pVertexDeclartion;

		CSceneManager*		m_pSceneManager;

		bool				m_bIsRenderring;

		CRenderQueue::RENDERENTRY m_RenderEntry;
	};

	ALCHEMY_INLINE bool CEnvironment::__IsValuable() const
	{
		return m_pMesh && m_pMaterial && m_pEnvironmentTexture && m_pRenderMethod && m_pSceneManager;
	}
}