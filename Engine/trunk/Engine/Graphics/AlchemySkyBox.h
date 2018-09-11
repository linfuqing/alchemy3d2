#pragma once

#include "AlchemyStaticObject.h"

#include "../Math/AlchemyMatrix4x4.h"

namespace alchemy
{
	class CSkyBox : 
		public CStaticObject
	{
	public:
		CSkyBox();
		virtual ~CSkyBox();

		bool Create(FLOAT fWidth, FLOAT fHeight, FLOAT fDepth, CGraphicsResource::HANDLE Material, UINT uCubeTextureIndex = 0);

		void Render(const CRenderQueue::RENDERENTRY& Entry, const CRenderQueue::RENDERKEY& Key, UINT uFlag);

	private:

		bool __IsValuable() const;

		bool __BuildEffectCode(CEffectCode& Code);
		bool __ComputeVertexes(CMesh& Mesh);
		bool __ComputeIndices(CMesh& Mesh);

	private:
		FLOAT					m_fWidth;
		FLOAT					m_fHeight;
		FLOAT					m_fDepth;

		UINT					m_uCubeSamplerIndex;
		CMatrix4x4				m_SkyMatrix;
		static const IEffect::SAMPLERDECLARATION sm_SAMPLER_DECLARATION;
	};
}