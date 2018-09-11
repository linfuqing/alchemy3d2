#pragma once

#include "AlchemyRenderer.h"
#include "AlchemySceneManager.h"
#include "../Core/AlchemySmartClass.h"
#include "../Math/AlchemyVector3.h"
#include "../Math/AlchemyVector4.h"

namespace alchemy
{
	class CSilhouetteProgram :
		public CRenderer, CRenderQueue::ICallBackToRender
	{
	public:
		CSilhouetteProgram();
		virtual ~CSilhouetteProgram();

		bool Create( const CMatrix4x4* pWorldMatrix, CGraphicsResource::HANDLE Mesh, CARGBColor& SilhouetteColor, FLOAT WidthModulus = 0.2f, bool isUpdate = false );

		bool ApplyForRender();

		virtual void Render(const CRenderQueue::RENDERENTRY& Entry, const CRenderQueue::RENDERKEY& Key, UINT uFlag);

		void Destroy();

	private:
		void __GetFaceNormal(const CVector4& v0, const CVector4& v1, const CVector4& v2, CVector3& Normal);
		void __Update();

		bool __IsReady() const;

		const CMatrix4x4* m_pWorldMatrix;

		bool			m_bIsUpdate;
		CMesh*			m_pSilhoutteMesh;

		CARGBColor		m_SilhouetteColor;
		FLOAT			m_fWidthModulus;
		UINT*			m_pAdjBuffer;

		IEffectInfo*		m_pViewEffect[4];
		IEffectInfo*		m_pSihoutetteEffect;

		UINT			m_uSourceNumFaces;
		UINT			m_uOffsets[2];
		LPINDEXBUFFER	m_pSourceIndexBuffer;
		LPVERTEXBUFFER	m_pSourcePosBuffer;
		LPVERTEXBUFFER	m_pSourceNormalBuffer;

		LPINDEXBUFFER	m_pDestIndexBuffer;
		LPVERTEXBUFFER	m_pDestPosBuffer;
		LPVERTEXBUFFER	m_pDestNormalBuffer[3];

		CEffect*		m_pEffect;
		IRenderMethod*	m_pRenderMethod;
	};

	typedef CSmartClass<CSilhouetteProgram>	SILHOUETTEPROGRAM, *LPSILHOUETTEPROGRAM;

	ALCHEMY_INLINE bool CSilhouetteProgram::__IsReady() const
	{
		return m_pSourceIndexBuffer && m_pDestIndexBuffer && m_pSourcePosBuffer && m_pDestPosBuffer;
	}
}