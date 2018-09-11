#pragma once

#include "AlchemyMeshProcessorInterface.h"
#include "AlchemyMesh.h"
#include "AlchemySkinMeshController.h"
#include "AlchemyRenderMethodInterface.h"
#include "AlchemyGraphicsResource.h"

#include "../Math/AlchemyMatrix4x4.h"

#include "../Core/AlchemySmartClass.h"

namespace alchemy
{
	class CSkinMeshProcessor :
		public CSmartClass<IMeshProcessor>
	{
	public:
		typedef struct _RENDERDATA
		{
			UINT					uBoneCount;
			CMesh::ATTRIBUTERANGE*	pAttribute;
			CMatrix4x4**			ppBoneMatrix;

			struct _RENDERDATA*		pNext;
		}RENDERDATA, * LPRENDERDATA;

		CSkinMeshProcessor(
		CSkinMeshController::SKINTYPE SkinType,
		CSkinInfo& SkinInfo, 
		CBone& Root, 
		const CEffectCode::ELEMENTDECLARATION* pSkinElementParameters = ALCHEMY_NULL, 
		UINT uElementCount = 0, 
		UINT uMaxBoneInfluence = 0);
		
		//CSkinMeshProcessor( CSkinMeshController& Controller, UINT uSubset, CAnimationController* pAnimationController );
		virtual ~CSkinMeshProcessor();

		IMeshProcessor* Clone();
		
		CMesh* GetMesh();

		//void SetAnimationController(CAnimationController& AnimController);

		//CAnimationController* GetAnimationController();

		CSkinMeshController& GetSkinMeshController();

		//bool SetSkinController( CSkinMeshController& Controller );

		//virtual UINT GetSubset();

		CMesh* ConvertMesh(CMesh& Mesh, UINT uSubset);
		void Update(FLOAT fElapse);
		bool OnProcess(IRenderMethod& RenderMethod, UINT uRenderFlag);

		void Destroy();

	private:
		void __UpdateSingleRenderData( CMesh::ATTRIBUTERANGE* pAttributeRange, RENDERDATA& RenderData );
		
		CSkinMeshController::SKINTYPE m_SkinType;
		CSkinInfo& m_SkinInfo;
		CBone& m_Root;
		const CEffectCode::ELEMENTDECLARATION* m_pSkinElementParameters;
		UINT m_uElementCount;
		UINT m_uMaxBoneInfluence;

		UINT				m_uSubset;
		CMesh*				m_pDestMesh;
		CSkinMeshController	m_SkinMeshController;
		RENDERDATA*			m_pRenderDataNode;

		CMatrix4x4*						m_pBoneMatrices;
		UINT							m_uBoneCount;
		//CAnimationController*			m_pAnimationController;
		const CMesh::BONECOMBINATION*	m_pBoneCombination;
	};

	ALCHEMY_INLINE CSkinMeshController& CSkinMeshProcessor::GetSkinMeshController()
	{
		return m_SkinMeshController;
	}

	/*ALCHEMY_INLINE CAnimationController* CSkinMeshProcessor::GetAnimationController()
	{
		return m_pAnimationController;
	}*/
}