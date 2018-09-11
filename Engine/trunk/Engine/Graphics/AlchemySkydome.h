 #pragma once

#include "AlchemyStaticObject.h"

#include "../Math/AlchemyFloat2.h"

namespace alchemy
{
	class CSkyDome :
		public CStaticObject
	{
	public:
		CSkyDome();
		~CSkyDome();

		bool Create(FLOAT fRadius, FLOAT fPhiDelta, FLOAT fThetaDelta, CGraphicsResource::HANDLE Material,
			UINT uMainTextureIndex = 0, UINT* pOffsetTextureIndices = ALCHEMY_NULL, UINT uOffsetNum = 0);

		void SetUVSpeed(FLOAT fUSpeed, FLOAT fVSpeed);

		void Update();

		void Render(const CRenderQueue::RENDERENTRY& Entry, const CRenderQueue::RENDERKEY& Key, UINT uFlag);

		void Destroy();

	private:
		UINT			m_uMeshData;

		UINT*			m_pOffsetIndicesNum;
		UINT			m_uOffsetTextureCount;
		CMatrix4x4		m_SkyMatrix;

		CFloat2			m_fUVOffsets;
		CFloat2			m_fUVOffsetSpeed;

		static const IEffect::SAMPLERDECLARATION sm_OFFSET_SAMPLER_DECLARATION;
	};

	ALCHEMY_INLINE void CSkyDome::SetUVSpeed(FLOAT fUSpeed, FLOAT fVSpeed)
	{
		m_fUVOffsetSpeed.u = fUSpeed;
		m_fUVOffsetSpeed.v = fVSpeed;
	}
}