#pragma once

#include "AlchemyRenderQueue.h"

namespace alchemy
{
	class CDeferredRenderingPipeline
	{
	public:
		class CPassDefiner :
			public CEffectCode::CPassDefiner, public SMARTCLASS
		{
		public:
			CPassDefiner(CEffectCode& Parent) : CEffectCode::CPassDefiner(Parent) {}
			~CPassDefiner(void) {}
		};

		CDeferredRenderingPipeline(CRenderQueue& RenderQueue);
		~CDeferredRenderingPipeline(void);

		UINT GetRenderTextureCount()const;
		LPTEXTURE GetRenderTexture(UINT uIndex);
		bool AddRenderTexture(CTexture::FORMAT TextureFormat, IEffectInfo& EffectInfo, INT nShift = 0);

		CPassDefiner& CreatePassDefiner(CEffectCode& Code);

		void Activate();

		void Destroy();

	private:
		typedef struct  
		{
			LPTEXTURE pTexture;
			IEffectInfo* pEffectInfo;
		}RENDERTEXTURE, * LPRENDERTEXTURE;

		class CDeferredRenderingPassDefiner :
			public CPassDefiner
		{
		public:
			CDeferredRenderingPassDefiner(CEffectCode& Parent, CDeferredRenderingPipeline& Pipeline);
			~CDeferredRenderingPassDefiner(void);

			UINT EndDefine();

		private:
			CDeferredRenderingPipeline& m_Pipeline;
		};

		//CMap<INT, CViewportTextureInfo*> m_ViewportTextureInfoMap;

		CVector<RENDERTEXTURE> m_RenderTextures;

		CRenderQueue& m_RenderQueue;

		bool m_bIsDirty;
	};

	ALCHEMY_INLINE UINT CDeferredRenderingPipeline::GetRenderTextureCount()const
	{
		return m_RenderTextures.GetLength();
	}

	ALCHEMY_INLINE LPTEXTURE CDeferredRenderingPipeline::GetRenderTexture(UINT uIndex)
	{
		LPRENDERTEXTURE pRenderTexture = m_RenderTextures.Get(uIndex);
		if(pRenderTexture)
			return pRenderTexture->pTexture;

		return ALCHEMY_NULL;
	}
}