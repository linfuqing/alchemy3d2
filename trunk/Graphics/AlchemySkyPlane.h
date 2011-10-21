#pragma once

#include "AlchemySceneManager.h"
#include "../Math/AlchemyFloat2.h"

namespace alchemy
{
	class CSkyPlane :
		public CSceneNode
	{
		typedef enum
		{
			EFFECT_PARAMETER_WORLD_VIEW_PROJECTION_MATRIX = 0,

			EFFECT_PARAMETER_TEXTURE,

			EFFECT_PARAMETER_UV_OFFSET,

			EFFECT_PARAMETER_AMBIENT_LIGHT,

			EFFECT_PARAMETER_EMISSIVE,

			TOTAL_EFFECT_PARAMETERS
		}EFFECTPARMAETER;
	public:
		CSkyPlane(void);
		~CSkyPlane(void);

		bool Create(
			FLOAT fLength,
			FLOAT fWidth,
			FLOAT fHeight,
			CGraphicsResource::HANDLE Texture, 
			CCompilerInterface& Compiler, 
			bool bIsAmbientLight,
			bool bIsEmissive,
			bool bIsUVOffset);

		void Update();

		bool ApplyForRender();
		void Render(const CRenderQueue::RENDERENTRY& Entry, UINT uFlag);

		void Destroy();

	protected:
		CFloat2 m_UVOffsetSpeed;

		CARGBColor m_Emissive;

	private:

		bool __IsValuable()const;

		CSceneManager* m_pSceneManager;
		CMesh* m_pMesh;
		CTexture* m_pTexture;
		CEffect* m_pEffect;

		UINT m_uEffectParameters[TOTAL_EFFECT_PARAMETERS];

		bool m_bIsAmbientLight;
		bool m_bIsEmissive;
		bool m_bIsUVOffset;

		CFloat2 m_UVOffset;
	};

	ALCHEMY_INLINE bool CSkyPlane::__IsValuable()const
	{
		return m_pSceneManager && m_pMesh && m_pTexture && m_pEffect;
	}
}
