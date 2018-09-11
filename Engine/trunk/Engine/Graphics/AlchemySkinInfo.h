#pragma once

#include "../Math/AlchemyMatrix4x4.h"

#include "../Core/AlchemyDebug.h"
#include "../Core/AlchemyNull.h"

#include "../Core/AlchemyInterface.h"
#include "../Core/AlchemySmartClass.h"

namespace alchemy
{
	class CSkinInfo :
		public CSmartClass<IInterface>
	{
	public:
		CSkinInfo(void);
		~CSkinInfo(void);

		typedef struct  
		{
			PCHAR     pcName;
			PFLOAT    pfWeights;
			PUINT     puIndices;
			MATRIX4X4 OffsetMatrix;
			UINT      uInfluenceCount;
		}BONE, * LPBONE;

		typedef struct  
		{
			UINT uSourceOffset;
			UINT uDestinationOffset;
			bool bIsNormal;
		}CHANNEL, * LPCHANNEL;

		UINT GetBoneCount()const;
		UINT GetMaxBoneInfluence()const;

		const CHAR* GetName(UINT uIndex)const;
		const FLOAT* GetWeights(UINT uIndex)const;
		const UINT* GetIndices(UINT uIndex)const;
		const MATRIX4X4* GetBoneOffsetMatrix(UINT uIndex)const;
		const UINT* GetBoneInfluenceCount(UINT uIndex)const;

		void Destroy();

		bool Create(const LPBONE pBones, UINT uBoneCount);

		bool Skinning(
			UINT uVertexCount,
			const void* pSourceVertices,
			UINT uSourceStride,
			void* pDestinationVertices,
			UINT uDestinationStride,
			const MATRIX4X4 *pBoneMatrices,
			const MATRIX4X4 *pInverseTransposeBoneMatrices,
			const CHANNEL *pChannels,
			UINT uChannelCount);

	private:
		LPBONE m_pBones;
		UINT   m_uBoneCount;
		UINT   m_uMaxBoneInfluence;
	};

	ALCHEMY_INLINE UINT CSkinInfo::GetBoneCount()const
	{
		return m_uBoneCount;
	}

	ALCHEMY_INLINE UINT CSkinInfo::GetMaxBoneInfluence()const
	{
		return m_uMaxBoneInfluence;
	}

	ALCHEMY_INLINE const CHAR* CSkinInfo::GetName(UINT uIndex)const
	{
		if(uIndex < m_uBoneCount)
			return m_pBones[uIndex].pcName;

		ALCHEMY_DEBUG_WARNING("Index out of range.");

		return ALCHEMY_NULL;
	}

	ALCHEMY_INLINE const FLOAT* CSkinInfo::GetWeights(UINT uIndex)const
	{
		if(uIndex < m_uBoneCount)
			return m_pBones[uIndex].pfWeights;

		ALCHEMY_DEBUG_WARNING("Index out of range.");

		return ALCHEMY_NULL;
	}

	ALCHEMY_INLINE const UINT* CSkinInfo::GetIndices(UINT uIndex)const
	{
		if(uIndex < m_uBoneCount)
			return m_pBones[uIndex].puIndices;

		ALCHEMY_DEBUG_WARNING("Index out of range.");

		return ALCHEMY_NULL;
	}

	ALCHEMY_INLINE const MATRIX4X4* CSkinInfo::GetBoneOffsetMatrix(UINT uIndex)const
	{
		if(uIndex < m_uBoneCount)
			return &m_pBones[uIndex].OffsetMatrix;

		ALCHEMY_DEBUG_WARNING("Index out of range.");

		return ALCHEMY_NULL;
	}

	ALCHEMY_INLINE const UINT* CSkinInfo::GetBoneInfluenceCount(UINT uIndex)const
	{
		if(uIndex < m_uBoneCount)
			return &m_pBones[uIndex].uInfluenceCount;

		ALCHEMY_DEBUG_WARNING("Index out of range.");

		return ALCHEMY_NULL;
	}
}