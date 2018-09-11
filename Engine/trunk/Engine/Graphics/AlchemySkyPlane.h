#pragma once

#include "AlchemyStaticObject.h"

#include "../Math/AlchemyFloat2.h"

namespace alchemy
{
	class CSkyPlane :
		public CStaticObject
	{
	public:
		CSkyPlane(void);
		~CSkyPlane(void);

		bool Create(
			FLOAT fLength,
			FLOAT fWidth,
			FLOAT fHeight,
			CGraphicsResource::HANDLE Texture, 
			bool bIsAmbientLight,
			bool bIsEmissive,
			bool bIsUVOffset);

		void Update();

		void Render(const CRenderQueue::RENDERENTRY& Entry, const CRenderQueue::RENDERKEY& Key, UINT uFlag);

	protected:
		CFloat2 m_UVOffsetSpeed;
		CFloat2 m_UVOffset;
		CARGBColor m_Emissive;

		CMatrix4x4	m_SkyMatrix;
		static const IEffect::SAMPLERDECLARATION sm_SAMPLER_DECLARATION;
	};
}
