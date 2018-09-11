#include "AlchemySkinInfo.h"
#include "../Core/AlchemyDebug.h"
#include "../Core/AlchemyString.h"
#include "../Math/AlchemyVector3.h"

using namespace alchemy;

CSkinInfo::CSkinInfo(void) :
m_pBones(ALCHEMY_NULL),
m_uBoneCount(0),
m_uMaxBoneInfluence(0)
{
}

CSkinInfo::~CSkinInfo(void)
{
	for(UINT i = 0; i < m_uBoneCount; ++ i)
	{
		ALCHEMY_DEBUG_DELETE_ARRAY(m_pBones[i].pcName   );
		ALCHEMY_DEBUG_DELETE_ARRAY(m_pBones[i].pfWeights);
		ALCHEMY_DEBUG_DELETE_ARRAY(m_pBones[i].puIndices);
	}

	ALCHEMY_DEBUG_DELETE_ARRAY(m_pBones);
}

void CSkinInfo::Destroy()
{
	for(UINT i = 0; i < m_uBoneCount; ++ i)
	{
		ALCHEMY_DEBUG_DELETE_ARRAY(m_pBones[i].pcName   );
		ALCHEMY_DEBUG_DELETE_ARRAY(m_pBones[i].pfWeights);
		ALCHEMY_DEBUG_DELETE_ARRAY(m_pBones[i].puIndices);
	}

	ALCHEMY_DEBUG_DELETE_ARRAY(m_pBones);

	m_uBoneCount        = 0;
	m_uMaxBoneInfluence = 0;
}

bool CSkinInfo::Create(const CSkinInfo::LPBONE pBones, UINT uBoneCount)
{
	Destroy();

	m_uBoneCount = uBoneCount;

	ALCHEMY_DEBUG_NEW(m_pBones, BONE[uBoneCount]);

	UINT i, j, uInfluenceCount, uNameLength;
	LPBONE pSoruceBone, pDestinationBone;
	for(i = 0; i < uBoneCount; ++ i)
	{
		pSoruceBone      = &pBones[i];

		pDestinationBone = &m_pBones[i];

		if(!pSoruceBone->pcName || !pSoruceBone->pfWeights || !pSoruceBone->puIndices)
		{
			pDestinationBone->pcName = ALCHEMY_NULL;
			pDestinationBone->pfWeights = ALCHEMY_NULL;
			pDestinationBone->puIndices = ALCHEMY_NULL;
			pDestinationBone->uInfluenceCount = 0;
			continue;
		}

		ALCHEMY_DEBUG_NEW(pDestinationBone->pcName, CHAR[uNameLength = StringLength(pSoruceBone->pcName) + 1]);

		ALCHEMY_MEMORY_COPY( pDestinationBone->pcName, pSoruceBone->pcName, uNameLength * sizeof(CHAR) );

		uInfluenceCount  = pDestinationBone->uInfluenceCount = pSoruceBone->uInfluenceCount;

		Copy(pDestinationBone->OffsetMatrix, pSoruceBone->OffsetMatrix);
		//pDestinationBone->OffsetMatrix = pSoruceBone->OffsetMatrix;

		ALCHEMY_DEBUG_NEW(pDestinationBone->pfWeights, FLOAT[uInfluenceCount]);
		ALCHEMY_DEBUG_NEW(pDestinationBone->puIndices, UINT[uInfluenceCount]);

		for(j = 0; j < uInfluenceCount; ++ j)
		{
			pDestinationBone->pfWeights[j] = pSoruceBone->pfWeights[j];
			pDestinationBone->puIndices[j] = pSoruceBone->puIndices[j];
		}
	}

	return true;
}

bool CSkinInfo::Skinning(
						 UINT uVertexCount,
						 const void* pSourceVertices,
						 UINT uSourceStride,
						 void* pDestinationVertices,
						 UINT uDestinationStride,
						 const MATRIX4X4 *pBoneMatrices,
						 const MATRIX4X4 *pInverseTransposeBoneMatrices,
						 const CHANNEL *pChannels,
						 UINT uChannelCount)
{
	UINT i, j;
	FLOAT3* pTemp;
	const CHANNEL* pChannel;
	PUINT8 puDestinationTemp = static_cast<PUINT8>(pDestinationVertices);

	for(i = 0; i < uVertexCount; ++ i)
	{
		for(j = 0; j < uChannelCount; ++j)
		{
			pChannel = &pChannels[j];

			pTemp = reinterpret_cast<FLOAT3*>(puDestinationTemp + pChannel->uDestinationOffset);
			pTemp->x = 0.0f;
			pTemp->y = 0.0f;
			pTemp->z = 0.0f;
		}

		puDestinationTemp += uDestinationStride;
	}


	UINT uInfluenceVertices, uIndex, uSourceOffsetByte, uDestinationOffsetByte;
	FLOAT fWeight;
	LPBONE pBone;
	const FLOAT3* pSourceTemp;
	FLOAT3* pDestinationTemp;
	CVector3 Temp;
	const UINT8* puSourceTemp = static_cast<const UINT8*>(pSourceVertices);
	
	puDestinationTemp = static_cast<PUINT8>(pDestinationVertices);

	for(i = 0; i < m_uBoneCount; ++ i)
	{
		pBone = &m_pBones[i];
		uInfluenceVertices = pBone->uInfluenceCount;

		if(uInfluenceVertices == 0)
			continue;

		while(uInfluenceVertices -- > 0)
		{
			uIndex = pBone->puIndices[uInfluenceVertices];

			fWeight = pBone->pfWeights[uInfluenceVertices];

			if(fWeight == 0.0f)
				continue;

			uSourceOffsetByte      = uIndex * uSourceStride;

			uDestinationOffsetByte = uIndex * uDestinationStride;

			for(j = 0; j < uChannelCount; ++ j)
			{
				pChannel = &pChannels[j];

				pSourceTemp      = reinterpret_cast<const FLOAT3*>(puSourceTemp + uSourceOffsetByte      + pChannel->uSourceOffset     );
				pDestinationTemp = reinterpret_cast<FLOAT3*>(puDestinationTemp  + uDestinationOffsetByte + pChannel->uDestinationOffset);

				Temp.x = pSourceTemp->x;
				Temp.y = pSourceTemp->y;
				Temp.z = pSourceTemp->z;
				//Temp = *pSourceTemp;

				if(pChannel->bIsNormal)
					Temp.TransformNormal(pInverseTransposeBoneMatrices[i]);
				else
					Temp.TransformCoord(pBoneMatrices[i]);

				Temp *= fWeight;

				/*AS3_Trace( AS3_Number(pTemp->x) );
				AS3_Trace( AS3_Number(pTemp->y) );
				AS3_Trace( AS3_Number(pTemp->z) );*/

				pDestinationTemp->x += Temp.x;
				pDestinationTemp->y += Temp.y;
				pDestinationTemp->z += Temp.z;
			}
		}
	}

	//puDestinationTemp = static_cast<PUINT8>(pDestinationVertices);

	//for(i = 0; i < uVertexCount; ++ i)
	//{
	//	for(j = 0; j < uChannelCount; ++j)
	//	{
	//		pChannel = &pChannels[j];

	//		pTemp = reinterpret_cast<FLOAT3*>(puDestinationTemp + pChannel->uDestinationOffset);

	//		/*AS3_Trace( AS3_Number(pTemp->x) );
	//		AS3_Trace( AS3_Number(pTemp->y) );
	//		AS3_Trace( AS3_Number(pTemp->z) );*/
	//	}

	//	puDestinationTemp += uDestinationStride;
	//}

	return true;
}