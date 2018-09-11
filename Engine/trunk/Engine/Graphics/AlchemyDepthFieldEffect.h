#pragma once

#include "AlchemySceneManager.h"

namespace alchemy
{
	class CDepthFieldEffect
		: public CSmartClass<CRenderQueue::ICallBackToRender>
	{
		static const FLOAT sm_NUM_SAMPLERS;
		typedef enum
		{
			EFFECT_CONSTANT_ONE,
			EFFECT_NUM_SAMPLER,
			EFFECT_ALPHA_TEXTURE,
			EFFECT_SCENE_TEXTURE,

			EFFECT_PARAMETER_COUNT
		}EFFECT_PARAMETER;
	public:
		CDepthFieldEffect();
		virtual ~CDepthFieldEffect();

		const CGraphicsResource::HANDLE GetAlphaTexture() const;
		FLOAT GetDistance() const;

		bool Create(FLOAT fDistance = 2.5f, FLOAT fBlur = 4.0f);

		void Destroy();

		bool ApplyForRender();
		void Render(const CRenderQueue::RENDERENTRY& Entry, const CRenderQueue::RENDERKEY& Key, UINT uFlag);

	private:
		CMesh*				        m_pMesh;
		CEffect*			        m_pEffect;
		FLOAT				        m_fDistance;
		FLOAT				        m_fBlur;
		LPTEXTURE			        m_pAlaphaTexture;
		CSceneManager*		        m_pSceneManager;
		UINT				        m_uEffectParameters[EFFECT_PARAMETER_COUNT];
		IEffect::SAMPLERDECLARATION m_SampleDeclaration[2];
	};

	ALCHEMY_INLINE const CGraphicsResource::HANDLE CDepthFieldEffect::GetAlphaTexture() const
	{
		return m_pAlaphaTexture == ALCHEMY_NULL ? 0 : m_pAlaphaTexture->GetResourceHandle();
	}

	ALCHEMY_INLINE FLOAT CDepthFieldEffect::GetDistance() const
	{
		return m_fDistance;
	}
}