#pragma once

#include "AlchemySceneManager.h"
#include "../Math/AlchemyFloat2.h"

namespace alchemy
{
	class CSkyDome :
		public CSceneNode
	{
	public:
		typedef enum
		{
			EFFECT_PARAMETER_WORLD_VIEW_PROJECTION,

			EFFECT_PARAMETER_TEXTURE,

			EFFECT_PARAMETER_UV_OFFSET,

			EFFECT_PARAMETER_COUNT
		}EFFECT_PARAMETER_TYPE;

		CSkyDome();
		virtual ~CSkyDome();

		bool Create(FLOAT fRadius, FLOAT fPhiDelta, FLOAT fThetaDelta, CGraphicsResource::HANDLE textureHandle, 
					CGraphicsResource::HANDLE* pOffsetTextures = ALCHEMY_NULL, UINT uOffsetTexturesNum = 0);

		void SetUVSpeed(FLOAT fUSpeed, FLOAT fVSpeed);

		virtual void Update();
		virtual bool ApplyForRender();
		virtual void Render(const CRenderQueue::RENDERENTRY& Entry, UINT uFlag);

		virtual void Destroy();

	private:
		bool __IsValuable() const;

	private:
		UINT			m_uMeshData;
		CMesh*			m_pMesh;
		LPTEXTURE		m_pTexture;
		LPTEXTURE*		m_ppOffsetTextures;
		UINT			m_uOffsetTextureCount;

		UINT			m_uEffectParameters[EFFECT_PARAMETER_COUNT];
		UINT*			m_pOffsetTextureParameters;

		CFloat2			m_fUVOffsets;
		CFloat2			m_fUVOffsetSpeed;

		CEffect*		m_pEffect;

		CSceneManager*	m_pSceneManager;
	};

	ALCHEMY_INLINE bool CSkyDome::__IsValuable() const
	{
		return m_pSceneManager && m_pMesh && m_pTexture && m_pEffect;
	}

	ALCHEMY_INLINE void CSkyDome::SetUVSpeed(FLOAT fUSpeed, FLOAT fVSpeed)
	{
		m_fUVOffsetSpeed.u = fUSpeed;
		m_fUVOffsetSpeed.v = fVSpeed;
	}

}