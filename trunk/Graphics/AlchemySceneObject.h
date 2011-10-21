#pragma once

#include "AlchemyMesh.h"
#include "AlchemyRenderMethod.h"
#include "AlchemyBone.h"
#include "AlchemySkinInfo.h"
#include "AlchemyAnimationController.h"
#include "AlchemyHardwareSkinEffect.h"
#include "AlchemyShadowMapDisplayEffect.h"
#include "AlchemySkinMeshController.h"

namespace alchemy
{
	class CProgramInstance;
	class CSceneManager;
	class CSceneObject :
		public CSceneNode
	{
		friend class CSceneManager;

		/*typedef struct DESTINATIONMESH
		{
			CMesh* pMesh;
			UINT uSubset;
			struct DESTINATIONMESH* pNext;
		}DESTINATIONMESH, * LPDESTINATIONMESH;*/

		typedef struct DESTINATIONVERTEXDECLARATION
		{
			CVertexDeclaration* pVertexDeclaration;
			UINT uRenderData;
			struct DESTINATIONVERTEXDECLARATION* pNext;
		}DESTINATIONVERTEXDECLARATION, * LPDESTINATIONVERTEXDECLARATION;

		typedef struct	_RENDERDATA
		{
			UINT					uBoneCount;
			CMesh::ATTRIBUTERANGE*	pAttribute;
			CMatrix4x4**			ppBoneMatrix;

			struct _RENDERDATA*		pNext;
		}RENDERDATA, * LPRENDERDATA;

		typedef struct  
		{
			CEffectCode::LPELEMENTPARAMETER pElementParameters;
			UINT uElementParameterCount;
			UINT uRenderData;
		}SHAREVERTEXINFO, * LPSHAREVERTEXINFO;

		typedef struct
		{
			INT					nPassIndex;
			CProgramInstance*	pProgramInstance;
		}PROGRAM_INFO;
	public:
		typedef enum
		{
			SKIN_NONE = 0,
			SKIN_SOFTWARE,
			SKIN_HARDWARE
		}SKINTYPE;

		typedef enum
		{
			SHADOW_NONE = 0,
			SHADOW_MAP,
			SHADOW_VOLUME
		}SHADOWTYPE;

		CSceneObject(void);
		~CSceneObject(void);

		void Destroy();

		bool SetRenderMethod(CRenderMethodInterface& RenderMethod, bool bIsUseSavedInfo = true);

		void AddProgramInstance(CProgramInstance& ProgramInstance, INT nPassIndex = -1);
		//CEffectInstanceController& GetEffectController();

		CMesh* SetSkinInfo(SKINTYPE SkinType, CSkinInfo& SkinInfo, CBone& Root, const CEffectCode::ELEMENTPARAMETER* pSkinElementParameters, UINT uElementCount, UINT uMaxBoneInfluence = 0, CEffectCode::CPassDefiner* pPassDefiner = ALCHEMY_NULL);
		CMesh* SetSkinInfo(SKINTYPE SkinType, CSkinInfo& SkinInfo, CBone& Root, UINT uMaxBoneInfluence = 0, CEffectCode::CPassDefiner* pPassDefiner = ALCHEMY_NULL);

		bool SetAnimationController(CAnimationController& AnimationController);

		bool SetShadowEnble(SHADOWTYPE Type, UINT uLightIndex, CCompilerInterface& Compiler);

		CShadowMapDisplayEffect* SetShadowDisplay(UINT uLightIndex);

		/*bool AddShareVertexDestination(CGraphicsResource::HANDLE* pOutputVertexBuffer, const UINT* puStream, UINT uStreamCount, const UINT* puUsage = ALCHEMY_NULL, const CMesh::ATTRIBUTERANGE* pAttributeRange = ALCHEMY_NULL);*/
		bool AddShareVertexDestination(const CEffectCode::ELEMENTPARAMETER* pVertexElementParameters, UINT uVertexElementParameterCount, const CMesh::ATTRIBUTERANGE* pAttributeRange = ALCHEMY_NULL, bool bIsSavedInfo = true);

		void ClearShareVertexInfo();

		void Update();

		bool ApplyForRenderShadowMap();
		bool ApplyForRender();

		void Render(const CRenderQueue::RENDERENTRY& Entry, UINT uFlag);

		void SetSkinMeshController(CSkinMeshController* pSkinMeshController ,bool isUpdate = false);

		CGraphicsResource::HANDLE GetMeshHanlde() const;

		const CMesh* GetMesh() const;

		const CMaterial* GetMaterial() const;
		CMaterial* GetMaterial();

		const CRenderMethodInterface* GetRenderMethod() const;
		CRenderMethodInterface* GetRenderMethod();

		UINT16 GetSubset() const;
		CSceneManager* GetSceneManager();

	private:
		bool __IsValuable();
		bool __IsSkinned();

		CVertexDeclaration* __CreateVertexDeclaration(const CEffectCode::ELEMENTPARAMETER* pVertexElementParameters, UINT uVertexElementParameterCount);

		bool __AddShareVertexDestination(const CEffectCode::ELEMENTPARAMETER* pVertexElementParameter, UINT uVertexElementParameterCount, UINT uRenderData);

		void __DestroyDestinationVertexDeclaration();

		//void __GetBoneCombinationRange(PUINT puOutputStart, PUINT puOutputEnd, const CMesh::ATTRIBUTERANGE& AttributeRange);

		UINT __AddRenderData(const CMesh::ATTRIBUTERANGE& AttributeRange);

		void __UpdateRenderData();

		void __UpdateSingleRenderData( CMesh::ATTRIBUTERANGE* pAttributeRange, RENDERDATA& RenderData );

		void __BuildProgramInstanceMap();

		CMesh* m_pSourceMesh;
		CMesh* m_pDestinationMesh;

		UINT16 m_uSubset;
		CMaterial* m_pMaterial;
		CRenderMethodInterface* m_pRenderMethod;
		CRenderMethodInterface* m_pDestinationRenderMethod;

		LPDESTINATIONVERTEXDECLARATION* m_ppDestinationVertexDeclaration;

		bool							m_bIsUpdate;
		CSkinMeshController*			m_pSkinMeshController;

		CPool<CMesh::ATTRIBUTERANGE>	m_AttributeTable;
		CPool<RENDERDATA*>				m_RenderDataPool;

		CVector<SHAREVERTEXINFO> m_ShareVertexInfos;

		CVector<CProgramInstance*>	m_ProgramInstances;
				
		CAnimationController* m_pAnimationController;

		CBone* m_pRoot;
		CBone** m_ppBones;
		CSkinInfo* m_pSkinInfo;

		CSkinInfo::LPCHANNEL* m_ppChannels;
		PUINT m_puChannelsCount;

		PUINT m_puSkinningStream;
		UINT m_uSkinningStreamCount;

		CMatrix4x4** m_ppBoneCombinationMatrices;
		const CMesh::BONECOMBINATION* m_pBoneCombination;
		UINT m_uBoneCombinationCount;
		CHardwareSkinEffect* m_pHardwareSkinEffect;

		SKINTYPE m_SkinType;

		CMatrix4x4* m_pBoneMatrices;
		UINT		m_uBoneCount;

		CRenderMethod* m_pShadowMapRenderMethod;
		CVertexDeclaration* m_pShadowMapVertexDeclaration;

		CMap<UINT, CShadowMapDisplayEffect*> m_ShadowMapDisplayEffectMap;

		CProgramInstance**			m_ppPassProgramMap;
		CVector<CProgramInstance*>*	m_pNoDrawInstance;
		CVector<PROGRAM_INFO>*		m_pAssistantInstance;

		UINT m_uSceneIndex;
		CSceneManager* m_pSceneManager;
	};

	/*ALCHEMY_INLINE CEffectInstanceController& CSceneObject::GetEffectController()
	{
		return *m_pEffectController;
	}*/

	ALCHEMY_INLINE const CMesh* CSceneObject::GetMesh() const
	{
		return m_pSourceMesh;
	}

	ALCHEMY_INLINE const CMaterial* CSceneObject::GetMaterial() const
	{
		return m_pMaterial;
	}

	ALCHEMY_INLINE CMaterial* CSceneObject::GetMaterial()
	{
		return m_pMaterial;
	}

	ALCHEMY_INLINE const CRenderMethodInterface* CSceneObject::GetRenderMethod() const
	{
		return m_pRenderMethod;
	}

	ALCHEMY_INLINE CRenderMethodInterface* CSceneObject::GetRenderMethod()
	{
		return m_pRenderMethod;
	}

	ALCHEMY_INLINE UINT16 CSceneObject::GetSubset() const
	{
		return m_uSubset;
	}

	ALCHEMY_INLINE CSceneManager* CSceneObject::GetSceneManager()
	{
		return m_pSceneManager;
	}

	ALCHEMY_INLINE bool CSceneObject::__IsValuable()
	{
		return /*m_pDestinationMesh && m_pDestinationMesh->pMesh*/m_pDestinationMesh && m_pMaterial && m_pRenderMethod;
	}

	ALCHEMY_INLINE bool CSceneObject::__IsSkinned()
	{
		return m_pSkinMeshController != ALCHEMY_NULL;
	}

	/*ALCHEMY_INLINE bool CSceneObject::__IsSkinned()
	{
		return m_pRoot && m_ppBones && m_pSkinInfo && m_pBoneMatrices && m_SkinType == SKIN_HARDWARE ? true : (m_SkinType == SKIN_SOFTWARE ? (m_ppChannels && m_puChannelsCount && m_puSkinningStream && m_uSkinningStreamCount) : false);
	}*/
}