#pragma once

#include "AlchemyRenderSystem.h"

namespace alchemy
{
	class CViewportTextureInfo :
		public CTexture::CTextureInfoInterface
	{
	public:
		CViewportTextureInfo(CRenderSystem& RenderSystem);
		~CViewportTextureInfo();

		void SetShift(INT uShift);
		INT GetShift()const;

		bool GetSize(PUINT puWidth, PUINT puHeight, PUINT puDepth);

	private:
		CRenderSystem& m_RenderSystem;
		INT m_nShift;
	};

	ALCHEMY_INLINE void CViewportTextureInfo::SetShift(INT nShift)
	{
		m_nShift = nShift;
	}

	ALCHEMY_INLINE INT CViewportTextureInfo::GetShift()const
	{
		return m_nShift;
	}
}