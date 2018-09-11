#pragma once

#include "AlchemyMesh.h"
#include "AlchemySkinInfo.h"
#include "../Core/AlchemyVector.h"
#include "AlchemyEffectCode.h"

namespace alchemy
{
	class CBone;
	class CMatrix4x4;
	class CBuffer;
	class CHardwareSkinEffect;

	class CSkinMeshController
	{
	public:
		typedef enum
		{
			SKIN_NONE = 0,
			SKIN_SOFTWARE,
			SKIN_HARDWARE
		}SKINTYPE;

		CSkinMeshController();
		~CSkinMeshController();

		bool Create(CMesh& Mesh, SKINTYPE SkinType, CSkinInfo& SkinInfo, CBone& Root, const CEffectCode::ELEMENTDECLARATION* pSkinElementParameters, UINT uElementCount, UINT uMaxBoneInfluence = 0);
		bool Create(CMesh& Mesh, SKINTYPE SkinType, CSkinInfo& SkinInfo, CBone& Root, UINT uMaxBoneInfluence = 0);

		CMatrix4x4* GetBoneMatrix(UINT& uBoneCount);

		const CMesh::BONECOMBINATION* GetBoneCombination() const;

		void GetBoneCombinationRange(PUINT puOutputStart, PUINT puOutputEnd, const CMesh::ATTRIBUTERANGE& AttributeRange);

		CHardwareSkinEffect* GetHardwareSkinEffect();

		void Update();

		void Destroy();

		const CMesh* GetDesinationMesh() const;
		CMesh* GetDesinationMesh();

		const CMesh* GetSourceMesh() const;
		CMesh* GetSourceMesh();

		CBone* GetRootBone();

		SKINTYPE GetSkinType() const;

		const CSkinInfo* GetSkinInfo() const;

	protected:
		CBone*		m_pRoot;
		CSkinInfo*	m_pSkinInfo;
		CBone**		m_ppBones;

		CMesh*		m_pSourceMesh;
		CMesh*		m_pDestinationMesh;


		CSkinInfo::LPCHANNEL*	m_ppChannels;
		PUINT					m_puChannelsCount;

		PUINT					m_puSkinningStream;
		UINT					m_uSkinningStreamCount;

		CBuffer*		m_pBoneCombination;
		UINT			m_uBoneCombinationCount;
		CMatrix4x4* 	m_pBoneMatrices;

		CHardwareSkinEffect*	m_pHardwareSkinEffect;

		SKINTYPE		m_SkinType;
	};

	ALCHEMY_INLINE const CMesh* CSkinMeshController::GetDesinationMesh() const
	{
		return m_pDestinationMesh;
	}

	ALCHEMY_INLINE CMesh* CSkinMeshController::GetDesinationMesh()
	{
		return m_pDestinationMesh;
	}

	ALCHEMY_INLINE const CMesh* CSkinMeshController::GetSourceMesh() const
	{
		return m_pSourceMesh;
	}

	ALCHEMY_INLINE CMesh* CSkinMeshController::GetSourceMesh()
	{
		return m_pSourceMesh;
	}

	ALCHEMY_INLINE CSkinMeshController::SKINTYPE CSkinMeshController::GetSkinType() const
	{
		return m_SkinType;
	}

	ALCHEMY_INLINE const CSkinInfo* CSkinMeshController::GetSkinInfo() const
	{
		return m_pSkinInfo;
	}

	ALCHEMY_INLINE const CMesh::BONECOMBINATION* CSkinMeshController::GetBoneCombination() const
	{
		return m_pBoneCombination == ALCHEMY_NULL ? ALCHEMY_NULL : reinterpret_cast<const CMesh::BONECOMBINATION*>( m_pBoneCombination->GetBufferPointer() );
	}

	ALCHEMY_INLINE CHardwareSkinEffect* CSkinMeshController::GetHardwareSkinEffect()
	{
		return m_pHardwareSkinEffect;
	}

	ALCHEMY_INLINE CMatrix4x4* CSkinMeshController::GetBoneMatrix(UINT& uBoneCount)
	{
		uBoneCount = m_pSkinInfo == ALCHEMY_NULL ? 0 : m_pSkinInfo->GetBoneCount();
		return m_pBoneMatrices;
	}

	ALCHEMY_INLINE CBone* CSkinMeshController::GetRootBone()
	{
		return m_pRoot;
	}
}