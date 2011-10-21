#pragma once

#include "AlchemyMirror.h"

namespace alchemy
{
	class CWater :
		public CMirror
	{
	public:
		CWater(void);
		~CWater(void);

		bool Create(
			FLOAT fWidth, 
			FLOAT fHeight, 
			UINT uWidthSegment = 1,
			UINT uHeightSegment = 1,
			INT nReflectionMapShift = 0, 
			INT nRefractionMapShift = 0, 
			UINT uNormalMap = 0,
			CGraphicsResource::HANDLE Material = 0,
			const CEffectCode::CPassDefiner::EFFECTINFO* pEffectInfos = ALCHEMY_NULL, 
			UINT uEffectInfoCount = 0);

		void Update();

		void Render(const CRenderQueue::RENDERENTRY& Entry, UINT uFlag);

		void Destroy();

	private:
		void __Destroy();

		LPTEXTURE m_pRefractionMap;

		CViewportTextureInfo* m_pRefractionMapInfo;

		CFresnelBlendMethod m_FresnelBlendMethod;

		FLOAT m_fRefractionScale;

		CFloat2 m_NormalMapTranslationScale;
	};
}