#pragma once

#include "AlchemyMirror.h"
//#include "AlchemyFresnelEffect.h"
#include "AlchemyWaterBlendMethod.h"

namespace alchemy
{
	class CWater :
		public CMirror
	{
		class CRefractionMapRenderer :
			CSmartClass<CRenderQueue::ICallBackToRender>
		{
		public:
			CRefractionMapRenderer(CWater& Water);
			~CRefractionMapRenderer();

			bool ApplyForRender();

			void Render(const CRenderQueue::RENDERENTRY& Entry, const CRenderQueue::RENDERKEY& Key, UINT uFlag);
		private:
			CWater& m_Water;
		};

		class CRefractionMapMaskRenderer :
			CSmartClass<CRenderQueue::ICallBackToRender>
		{
		public:
			CRefractionMapMaskRenderer(CWater& Water);
			~CRefractionMapMaskRenderer();

			bool Create();

			bool ApplyForRender();

			void Render(const CRenderQueue::RENDERENTRY& Entry, const CRenderQueue::RENDERKEY& Key, UINT uFlag);

			void Destroy();
		private:
			CWater& m_Water;
			CEffect* m_pEffect;
			CVertexDeclaration* m_pVertexDeclaration;

			UINT m_uWorldViewProjectionMatrix;
			UINT m_uZero;
		};
	public:
		CWater(void);
		~CWater(void);

		bool Create(
			FLOAT fWidth, 
			FLOAT fHeight, 
			UINT uWidthSegment,
			UINT uHeightSegment, 
			UINT uReflectionMap,
			INT nRefractionMapShift = 0, 
			UINT uNormalMap = 0,
			CGraphicsResource::HANDLE Material = 0,
			const CEffectCode::CPassDefiner::EFFECTINFO* pEffectInfos = ALCHEMY_NULL, 
			UINT uEffectInfoCount = 0);

		void Update();

		bool ApplyForRender(UINT uRenderPass = ~0, UINT uRenderList = CRenderQueue::NORMAL);

		void Render(const CRenderQueue::RENDERENTRY& Entry, const CRenderQueue::RENDERKEY& Key, UINT uFlag);

		void Destroy();

	private:
		void __Destroy();

		LPTEXTURE m_pRefractionMap;

		//CViewportTextureInfo* m_pRefractionMapInfo;

		//CFresnelEffect m_FresnelEffect;
		CWaterBlendMethod m_WaterBlendMethod;

		FLOAT m_fRefractionScale;

		CFloat2 m_NormalMapTranslationScale;

		CRefractionMapRenderer m_RefractionMapRenderer;
		CRefractionMapMaskRenderer m_RefractionMapMaskRenderer;
	};
}