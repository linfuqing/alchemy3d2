#pragma once

#include "AlchemyFlashGraphicsBuffer.h"
#include "AlchemyFlashObjectInterface.h"
#include "../Graphics/AlchemyEffectCode.h"

namespace alchemy
{
	class CFlashContext3D;
	class CFlashTexture :
		public CFlashGraphicsBuffer<CTexture>, public CFlashObjectInterface
	{
		friend class CFlashContext3D;

		typedef enum
		{
			TOTAL_MIP_MAP_LEVEL = ALCHEMY_TOTAL_BITS(UINT)
		}CONSTANT;
	public:
		CFlashTexture(void);
		~CFlashTexture(void);

		void Reset();

		PVOID Lock(UINT uSurface, UINT uFlag, PUINT puRowPitch, PUINT puSlicePitch);

		bool  Unlock();

		void Destroy();

#ifdef AS3_H
		AS3TYPE Init();
#endif

	protected:
		bool _Create();

	private:
		PVOID m_pData;

		UINT m_uMipLevelDirtyFlag;

		UINT m_uCubeMapFaces[TOTAL_MIP_MAP_LEVEL];

		CGraphicsResource::HANDLE m_RenderTarget;

		CEffectCode::SAMPLERFILTER m_Filter;

		bool m_bIsDirty;
	};
}