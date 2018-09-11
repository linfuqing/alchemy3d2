#pragma once

#include "AlchemySceneManager.h"

namespace alchemy
{
	class CFullScreenDrawer :
		public CSmartClass<CSceneManager::IFullScreenDrawer>
	{
	public:
		CFullScreenDrawer(void);
		~CFullScreenDrawer(void);

		const CEffect* GetEffect()const;

		bool IsValuable()const;

		bool Create();

		UINT AddUserData(CGraphicsResource::HANDLE RenderTexture, IRenderMethod* pRenderMethod, CGraphicsResource::HANDLE Material, UINT uRenderTextureIndex, const IEffect::SAMPLERDECLARATION* pSamplerDeclaration);

		void Render(const CRenderQueue::RENDERENTRY& Entry, const CRenderQueue::RENDERKEY& Key, UINT uFlag);

		void Destroy();

	private:
		typedef struct  
		{
			CGraphicsResource::HANDLE RenderTexture;
			IRenderMethod* pRenderMethod;
			CGraphicsResource::HANDLE Material;
			UINT uRenderTextureIndex;
			const IEffect::SAMPLERDECLARATION* pSamplerDeclaration;
		}USERDATA, * LPUSERDATA;
		
		static const IEffect::SAMPLERDECLARATION sm_SAMPLER_DECLARATION;

		CSceneManager* m_pSceneManager;

		CVertexDeclaration* m_pVertexDeclaration;

		LPVERTEXBUFFER m_pVertexBuffer;
		LPINDEXBUFFER m_pIndexBuffer;

		CEffect* m_pEffect;

		CVector<USERDATA> m_UserData;
		CPool<IEffect::SAMPLERDECLARATION> m_SamplerDeclarationPool;
	};

	ALCHEMY_INLINE const CEffect* CFullScreenDrawer::GetEffect()const
	{
		return m_pEffect;
	}

	ALCHEMY_INLINE bool CFullScreenDrawer::IsValuable()const
	{
		return m_pSceneManager && m_pVertexDeclaration && m_pVertexBuffer && m_pIndexBuffer && m_pEffect;
	}
}