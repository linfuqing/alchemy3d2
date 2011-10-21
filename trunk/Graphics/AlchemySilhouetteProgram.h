#pragma once

#include "AlchemyProgramInstance.h"
#include "AlchemyEffectCode.h"
#include "AlchemyCompilerInterface.h"

namespace alchemy
{
	class CAttributeViewSpaceEffect;
	class CSilhouetteEffect;

	class CSilhouetteProgram :
		public CProgramInstance
	{
	public:
		CSilhouetteProgram();
		virtual ~CSilhouetteProgram();

		bool Create(CGraphicsResource::HANDLE Mesh, FLOAT WidthModulus = 0.2f, CARGBColor& SilhouetteColor = CARGBColor(0.0f, 0.0f, 0.0f, 0.0f), bool isUpdate = false);

		//ÂÖÀª±ß×îºó»æÖÆ
		virtual bool OnEndRenderSceneObject(CSceneObject* pSceneObject, UINT uFlag);
		virtual void OnAddedToSceneObject();

		void Destroy();

	private:
		void __GetFaceNormal(const CVector4& v0, const CVector4& v1, const CVector4& v2, CVector3& Normal);
		void __Update();

		bool __IsReady() const;


		bool			m_bIsUpdate;
		CRenderMethod*	m_pRenderMethod;
		CMesh*			m_pSourceMesh;
		CMesh*			m_pSilhoutteMesh;

		CARGBColor		m_SilhouetteColor;
		FLOAT			m_fWidthModulus;
		UINT*			m_pAdjBuffer;

		CAttributeViewSpaceEffect*	m_pViewEffect[4];
		CSilhouetteEffect*			m_pSihoutetteEffect;

		UINT			m_uSourceNumFaces;
		UINT			m_uOffsets[2];
		LPINDEXBUFFER	m_pSourceIndexBuffer;
		LPVERTEXBUFFER	m_pSourcePosBuffer;
		LPVERTEXBUFFER	m_pSourceNormalBuffer;

		LPINDEXBUFFER	m_pDestIndexBuffer;
		LPVERTEXBUFFER	m_pDestPosBuffer;
		LPVERTEXBUFFER	m_pDestNormalBuffer[3];
	};


	ALCHEMY_INLINE bool CSilhouetteProgram::__IsReady() const
	{
		return m_pSourceIndexBuffer && m_pDestIndexBuffer && m_pSourcePosBuffer && m_pDestPosBuffer;
	}
}