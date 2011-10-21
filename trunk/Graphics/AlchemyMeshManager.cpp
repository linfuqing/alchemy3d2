#include "AlchemyMeshManager.h"
#include "../Math/AlchemyFloat2.h"
#include "../Math/AlchemyVector3.h"

using namespace alchemy;

CMeshManager::CMeshManager(void) :
CSingleton<CMeshManager>(*this)
{
}

CMeshManager::~CMeshManager(void)
{
	Destroy();
}

const CMeshManager::SCENEDATA* CMeshManager::GetSceneData(UINT uHandle)const
{
	if(!uHandle)
		return ALCHEMY_NULL;

	return m_ScenePool.Get(uHandle - 1);
}

const CMeshManager::MESHDATA* CMeshManager::GetMeshData(UINT uHandle)const
{
	if(!uHandle)
		return ALCHEMY_NULL;

	return m_MeshPool.Get(uHandle - 1);
}

UINT CMeshManager::CreateSkyDome(
					  FLOAT fRadius,
					  FLOAT fPhiDelta,
					  FLOAT fThetaDelta,
					  const CVertexDeclaration::LPVERTEXELEMENT pVertexElements,
					  const UINT* pVertexUsage,
					  UINT  uElementCount,
					  UINT  uIndexUsage)
{
	if( !CVertexDeclaration::IsValuableMeshDeclaration(pVertexElements, uElementCount) )
	{
		ALCHEMY_DEBUG_WARNING("CMeshManager::CreateSphereDome: Invalid mesh declaration.");

		return 0;
	}

	MESHDATA MeshData;

	FLOAT fPhiDeltaRadian = fPhiDelta * g_fDEG2RAD, fPhiEndRadian = 90.0f * g_fDEG2RAD;
	FLOAT fThetaBeginRadian = 0.0f,fThetaDeltaRadian = fThetaDelta * g_fDEG2RAD,fThetaEndRadian = 360.0f * g_fDEG2RAD;

	UINT i;
	ALCHEMY_DEBUG_NEW(MeshData.VertexElementData.pVertexElement, CVertexDeclaration::VERTEXELEMENT[uElementCount]);
	for(i = 0; i < uElementCount; ++ i)
		MeshData.VertexElementData.pVertexElement[i] = pVertexElements[i];

	MeshData.VertexElementData.uElementCount = uElementCount;

	MeshData.uNumVertices = (UINT)( (90.0f / fPhiDelta)  * ( 360.0f / fThetaDelta + 1)  + 1);

	UINT uStreamCount;
	MeshData.ppVertexData = __CreateVertexData(MeshData.uNumVertices, pVertexElements, uElementCount, &uStreamCount);

	ALCHEMY_DEBUG_NEW(MeshData.VertexElementData.puUsage, UINT[uStreamCount]);

	for(i = 0; i < uStreamCount; ++ i)
		MeshData.VertexElementData.puUsage[i] = pVertexUsage[i];

	CFloat4* pfPositionData;
	CFloat3* pUVWData;
	CFloat2* pUVData;
	bool isCubeTexture = false; 

	UINT uVertexElement = CVertexDeclaration::FindVertexElement(CVertexDeclaration::POSITION, CVertexDeclaration::FLOAT4, 0, pVertexElements, uElementCount);
	pfPositionData = reinterpret_cast<CFloat4*>( MeshData.ppVertexData[ pVertexElements[ uVertexElement ].uStream ] );

	uVertexElement = CVertexDeclaration::FindVertexElement( CVertexDeclaration::TEXCOORD, CVertexDeclaration::FLOAT2, 0, pVertexElements, uElementCount );
	if(uVertexElement == uElementCount)
	{
		isCubeTexture = true;
		uVertexElement = CVertexDeclaration::FindVertexElement(CVertexDeclaration::TEXCOORD, CVertexDeclaration::FLOAT3, 0, pVertexElements, uElementCount);
		pUVWData = reinterpret_cast<CFloat3*>( MeshData.ppVertexData[ pVertexElements[ uVertexElement ].uStream ] );
	}
	else
		pUVData = reinterpret_cast<CFloat2*>( MeshData.ppVertexData[ pVertexElements[ uVertexElement ].uStream ] );

	UINT uVertexIndex = 1;
	CVector3 tmpNormal;

	UINT nPerThetaVertexCount = (UINT) (360.0f/fThetaDelta+1);
	UINT nPerPhiVertexCount = (UINT)(90.0f/fPhiDelta);

	FLOAT fUVRadius = 0.5f / (90.0f / fPhiDelta);
	UINT  uCurrentPhi = 1;
	FLOAT fCurrentUVRadius;

	FLOAT f90Radian = 90.0f*g_fDEG2RAD;

	for (FLOAT fPhiElapse = fPhiDeltaRadian; fPhiEndRadian >= fPhiElapse; fPhiElapse += fPhiDeltaRadian )
	{
		fCurrentUVRadius = uCurrentPhi * fUVRadius;
		for (FLOAT fThetaElapse = fThetaBeginRadian; fThetaEndRadian >= fThetaElapse /*>= g_fFLT_EPSILON*/; fThetaElapse += fThetaDeltaRadian )
		{
			pfPositionData[uVertexIndex].x = fRadius * sinf(fPhiElapse) * sinf(fThetaElapse);
			pfPositionData[uVertexIndex].y = fRadius * cosf(fPhiElapse);
			pfPositionData[uVertexIndex].z = -fRadius * sinf(fPhiElapse) * cosf(fThetaElapse);
			pfPositionData[uVertexIndex].w = 1.0f;

			if(isCubeTexture)
			{
				tmpNormal.x = pfPositionData[uVertexIndex].x;
				tmpNormal.y = pfPositionData[uVertexIndex].y;
				tmpNormal.z = pfPositionData[uVertexIndex].z;
				tmpNormal.Normalize();

				pUVWData[uVertexIndex].u = tmpNormal.x;
				pUVWData[uVertexIndex].v = tmpNormal.y;
				pUVWData[uVertexIndex].w = tmpNormal.z;
			}
			else
			{
				pUVData[uVertexIndex].u = fCurrentUVRadius*cosf(fThetaElapse-f90Radian) + 0.5f;
				pUVData[uVertexIndex].v = -fCurrentUVRadius*sinf(fThetaElapse-f90Radian) + 0.5f;
			}

			++uVertexIndex;
		}

		++uCurrentPhi;
	}

	pfPositionData[0].x = 0.0f;
	pfPositionData[0].y = fRadius;
	pfPositionData[0].z = 0.0f;
	pfPositionData[0].w = 1.0f;

	if(isCubeTexture)
	{
		pUVWData[0].u = 0.0f;
		pUVWData[0].v = 1.0f;
		pUVWData[0].w = 0.0f;
	}
	else
	{
		pUVData[0].u = 0.5f;
		pUVData[0].v = 0.5f;
	}

	MeshData.uIndexUsage = uIndexUsage;

	MeshData.uNumIndices = (nPerPhiVertexCount-1)*nPerThetaVertexCount*2*3 + nPerThetaVertexCount*3;
	uVertexIndex = 0;

	UINT uPerPhiQuad = nPerPhiVertexCount, uPerThetaQuad = nPerThetaVertexCount;

	if(MeshData.uNumIndices < 0xffff)
	{
		MeshData.IndexFormat = INDEXBUFFER::BIT16;
		ALCHEMY_DEBUG_NEW( MeshData.pIndexData, UINT8[MeshData.uNumIndices * sizeof(UINT16)] );

		PUINT16 pIndexData = reinterpret_cast<PUINT16>( MeshData.pIndexData );

		for (UINT j=0; j<uPerThetaQuad; ++j)
		{
			if(j == uPerThetaQuad-1)
			{
				pIndexData[uVertexIndex++] = 0;
				pIndexData[uVertexIndex++] = j+1;
				pIndexData[uVertexIndex++] = 1;
			}
			else
			{
				pIndexData[uVertexIndex++] = 0;
				pIndexData[uVertexIndex++] = j+1;
				pIndexData[uVertexIndex++] = j+2;
			}
		}

		for (UINT i=1; i<uPerPhiQuad; ++i)
		{
			for (UINT j=0; j<uPerThetaQuad; ++j)
			{
				if(j == uPerThetaQuad-1)
				{
					pIndexData[uVertexIndex++] = i * nPerThetaVertexCount - nPerThetaVertexCount + 1;
					pIndexData[uVertexIndex++] = i * nPerThetaVertexCount - nPerThetaVertexCount + 1 + j;
					pIndexData[uVertexIndex++] = (i + 1) * nPerThetaVertexCount - nPerThetaVertexCount + 1;

					pIndexData[uVertexIndex++] = (i + 1) * nPerThetaVertexCount - nPerThetaVertexCount + 1;
					pIndexData[uVertexIndex++] = i * nPerThetaVertexCount - nPerThetaVertexCount + 1 + j;
					pIndexData[uVertexIndex++] = (i + 1) * nPerThetaVertexCount - nPerThetaVertexCount + 1+ j;
				}
				else
				{
					pIndexData[uVertexIndex++] = i * nPerThetaVertexCount - nPerThetaVertexCount + 1 + j + 1;
					pIndexData[uVertexIndex++] = i * nPerThetaVertexCount - nPerThetaVertexCount + 1 + j;
					pIndexData[uVertexIndex++] = (i + 1) * nPerThetaVertexCount - nPerThetaVertexCount + 1 + j + 1;

					pIndexData[uVertexIndex++] = (i + 1) * nPerThetaVertexCount - nPerThetaVertexCount + 1 + j + 1;
					pIndexData[uVertexIndex++] = i * nPerThetaVertexCount - nPerThetaVertexCount + 1 + j;
					pIndexData[uVertexIndex++] = (i + 1) * nPerThetaVertexCount - nPerThetaVertexCount + 1 + j;
				}
			}
		}
	}
	else
	{
		MeshData.IndexFormat = INDEXBUFFER::BIT32;
		ALCHEMY_DEBUG_NEW( MeshData.pIndexData, UINT8[MeshData.uNumIndices * sizeof(UINT32)] );

		PUINT32 pIndexData = reinterpret_cast<PUINT32>( MeshData.pIndexData );

		for (UINT j=0; j<uPerThetaQuad; ++j)
		{
			if(j == uPerThetaQuad-1)
			{
				pIndexData[uVertexIndex++] = 0;
				pIndexData[uVertexIndex++] = j+1;
				pIndexData[uVertexIndex++] = 1;
			}
			else
			{
				pIndexData[uVertexIndex++] = 0;
				pIndexData[uVertexIndex++] = j+1;
				pIndexData[uVertexIndex++] = j+2;
			}
		}

		for (UINT i=1; i<uPerPhiQuad; ++i)
		{
			for (UINT j=0; j<uPerThetaQuad; ++j)
			{
				if(j == uPerThetaQuad-1)
				{
					pIndexData[uVertexIndex++] = i * nPerThetaVertexCount - nPerThetaVertexCount + 1;
					pIndexData[uVertexIndex++] = i * nPerThetaVertexCount - nPerThetaVertexCount + 1 + j;
					pIndexData[uVertexIndex++] = (i + 1) * nPerThetaVertexCount - nPerThetaVertexCount + 1;

					pIndexData[uVertexIndex++] = (i + 1) * nPerThetaVertexCount - nPerThetaVertexCount + 1;
					pIndexData[uVertexIndex++] = i * nPerThetaVertexCount - nPerThetaVertexCount + 1 + j;
					pIndexData[uVertexIndex++] = (i + 1) * nPerThetaVertexCount - nPerThetaVertexCount + 1+ j;
				}
				else
				{
					pIndexData[uVertexIndex++] = i * nPerThetaVertexCount - nPerThetaVertexCount + 1 + j + 1;
					pIndexData[uVertexIndex++] = i * nPerThetaVertexCount - nPerThetaVertexCount + 1 + j;
					pIndexData[uVertexIndex++] = (i + 1) * nPerThetaVertexCount - nPerThetaVertexCount + 1 + j + 1;

					pIndexData[uVertexIndex++] = (i + 1) * nPerThetaVertexCount - nPerThetaVertexCount + 1 + j + 1;
					pIndexData[uVertexIndex++] = i * nPerThetaVertexCount - nPerThetaVertexCount + 1 + j;
					pIndexData[uVertexIndex++] = (i + 1) * nPerThetaVertexCount - nPerThetaVertexCount + 1 + j;
				}
			}
		}
	}

	ALCHEMY_DEBUG_NEW( MeshData.pAttributeRange, CMesh::ATTRIBUTERANGE );
	MeshData.pAttributeRange->uStartIndex = 0;
	MeshData.pAttributeRange->uIndexCount = MeshData.uNumIndices;
	MeshData.uAttributeRangeCount = 1;

	return m_MeshPool.Add( MeshData ) + 1;
}

UINT CMeshManager::CreatePlane(
				 FLOAT fWidth, 
				 FLOAT fHeight, 
				 UINT uWidthSegment, 
				 UINT uHeightSegment, 
				 UINT uWidthStep,
				 UINT uHeightStep,
				 bool bIsYUp,
				 const CVertexDeclaration::VERTEXELEMENT* pVertexElements,
				 UINT uElementCount,
				 const FLOAT3* pPositionOffset,
				 const UINT* puVertexUsage,
				 UINT uIndexUsage)
{
	if(!pVertexElements || !uElementCount)
	{
		static const CVertexDeclaration::VERTEXELEMENT s_POSITION_ELEMENT = {0, 0, CVertexDeclaration::FLOAT4, CVertexDeclaration::POSITION};

		pVertexElements = &s_POSITION_ELEMENT;

		uElementCount = 1;
	}

	if( !CVertexDeclaration::IsValuableMeshDeclaration(pVertexElements, uElementCount) )
	{
		ALCHEMY_DEBUG_WARNING("CMeshManager::CreatePlane: Invalid mesh declaration.");

		return 0;
	}

	MESHDATA MeshData;

	UINT i;
	ALCHEMY_DEBUG_NEW(MeshData.VertexElementData.pVertexElement, CVertexDeclaration::VERTEXELEMENT[uElementCount]);
	for(i = 0; i < uElementCount; ++ i)
		MeshData.VertexElementData.pVertexElement[i] = pVertexElements[i];

	MeshData.VertexElementData.uElementCount = uElementCount;

	UINT uNumVerticesX = uWidthSegment  + 1,
		 uNumVerticesY = uHeightSegment + 1;

	MeshData.uNumVertices = uNumVerticesX * uNumVerticesY;

	UINT uStreamCount;
	MeshData.ppVertexData = __CreateVertexData(MeshData.uNumVertices, pVertexElements, uElementCount, &uStreamCount);

	ALCHEMY_DEBUG_NEW(MeshData.VertexElementData.puUsage, UINT[uStreamCount]);
	if(puVertexUsage)
	{
		for(i = 0; i < uStreamCount; ++ i)
			MeshData.VertexElementData.puUsage[i] = puVertexUsage[i];
	}
	else
		for(i = 0; i < uStreamCount; ++ i)
			MeshData.VertexElementData.puUsage[i] = 0;

	bool bIsFillZ = false, bIsFillW = false;
	UINT uVertexElement;
	uVertexElement = CVertexDeclaration::FindVertexElement(CVertexDeclaration::POSITION, CVertexDeclaration::FLOAT2, 0, pVertexElements, uElementCount);

	if(uVertexElement == uElementCount)
	{
		uVertexElement = CVertexDeclaration::FindVertexElement(CVertexDeclaration::POSITION, CVertexDeclaration::FLOAT3, 0, pVertexElements, uElementCount);

		bIsFillZ = true;
	}

	if(uVertexElement == uElementCount)
	{
		uVertexElement = CVertexDeclaration::FindVertexElement(CVertexDeclaration::POSITION, CVertexDeclaration::FLOAT4, 0, pVertexElements, uElementCount);

		bIsFillW = true;
	}

	UINT j, uOffset, uStreamSize;
	CFloat2 Temp;
	PFLOAT pfOffset;
	PUINT8 puData;
	FLOAT fWidthPerSegment  = fWidth  / uWidthSegment,
		  fHeightPerSegment = fHeight / uHeightSegment;

	const CVertexDeclaration::VERTEXELEMENT* pVertexElement;

	UINT uYOffset, uZOffset;
	if(bIsYUp)
	{
		uYOffset = 2;
		uZOffset = 1;
	}
	else
	{
		uYOffset = 1;
		uZOffset = 2;
	}

	if(uVertexElement < uElementCount)
	{
		FLOAT fBaseX = - fWidth * 0.5f;

		pVertexElement = &pVertexElements[uVertexElement];

		uStreamSize = CVertexDeclaration::GetVertexElementStreamStrideOf(pVertexElement->uStream, pVertexElements, uElementCount);

		Temp = CFloat2(fBaseX, - fHeight * 0.5f);

		if(pPositionOffset)
		{
			Temp.x += pPositionOffset->x;
			Temp.y += bIsYUp ? pPositionOffset->z : pPositionOffset->y;
		}

		uOffset = pVertexElement->uOffset;

		puData = reinterpret_cast<PUINT8>(MeshData.ppVertexData[pVertexElement->uStream]);

		for(j = 0; j < uNumVerticesY; j ++)
		{
			for(i = 0; i < uNumVerticesX; i ++)
			{
				pfOffset = reinterpret_cast<PFLOAT>(puData + uOffset);

				* pfOffset              = Temp.x;
				*(pfOffset += uYOffset) = Temp.y;

				Temp.x += fWidthPerSegment;

				uOffset += uStreamSize;
			}

			Temp.x  = fBaseX;
			Temp.y += fHeightPerSegment;
		}

		if(bIsFillZ)
		{
			FLOAT z = 0.0f;
			if(pPositionOffset)
				z = bIsYUp ? pPositionOffset->y : pPositionOffset->z;

			uOffset = pVertexElement->uOffset + (sizeof(FLOAT) * uZOffset);
			for(j = 0; j < uNumVerticesY; j ++)
			{
				for(i = 0; i < uNumVerticesX; i ++)
				{
					pfOffset = reinterpret_cast<PFLOAT>(puData + uOffset);

					*pfOffset = z;

					uOffset += uStreamSize;
				}
			}
		}

		if(bIsFillW)
		{
			uOffset = pVertexElement->uOffset + sizeof(FLOAT) * 3;
			for(j = 0; j < uNumVerticesY; j ++)
			{
				for(i = 0; i < uNumVerticesX; i ++)
				{
					pfOffset = reinterpret_cast<PFLOAT>(puData + uOffset);

					*pfOffset = 1.0f;

					uOffset += uStreamSize;
				}
			}
		}
	}

	uVertexElement = CVertexDeclaration::FindVertexElement(CVertexDeclaration::TEXCOORD, CVertexDeclaration::FLOAT2, 0, pVertexElements, uElementCount);

	if(uVertexElement < uElementCount)
	{
		pVertexElement = &pVertexElements[uVertexElement];

		uStreamSize = CVertexDeclaration::GetVertexElementStreamStrideOf(pVertexElement->uStream, pVertexElements, uElementCount);

		fWidthPerSegment  /= fWidth;
		fHeightPerSegment /= fHeight;

		Temp = CFloat2();

		uOffset = pVertexElement->uOffset;

		puData = reinterpret_cast<PUINT8>(MeshData.ppVertexData[pVertexElement->uStream]);

		for(j = 0; j < uNumVerticesY; j ++)
		{
			for(i = 0; i < uNumVerticesX; i ++)
			{
				pfOffset = reinterpret_cast<PFLOAT>(puData + uOffset);

				*   pfOffset  = Temp.x;
				*(++pfOffset) = Temp.y;

				Temp.x += 1.0f;//fWidthPerSegment;

				uOffset += uStreamSize;
			}

			Temp.x  = 0.0f;
			Temp.y += 1.0f;//fHeightPerSegment;
		}
	}

	uVertexElement = CVertexDeclaration::FindVertexElement(CVertexDeclaration::NORMAL, CVertexDeclaration::FLOAT3, 0, pVertexElements, uElementCount);

	if(uVertexElement < uElementCount)
	{
		pVertexElement = &pVertexElements[uVertexElement];

		uStreamSize = CVertexDeclaration::GetVertexElementStreamStrideOf(pVertexElement->uStream, pVertexElements, uElementCount);

		uOffset = pVertexElement->uOffset;

		puData = reinterpret_cast<PUINT8>(MeshData.ppVertexData[pVertexElement->uStream]);

		for(j = 0; j < uNumVerticesY; j ++)
		{
			for(i = 0; i < uNumVerticesX; i ++)
			{
				pfOffset = reinterpret_cast<PFLOAT>(puData + uOffset);

				* pfOffset             = 0.0f;
				*(pfOffset + uYOffset) = 0.0f;
				*(pfOffset + uZOffset) = 1.0f;

				uOffset += uStreamSize;
			}
		}
	}

	uVertexElement = CVertexDeclaration::FindVertexElement(CVertexDeclaration::TANGENT, CVertexDeclaration::FLOAT3, 0, pVertexElements, uElementCount);

	if(uVertexElement < uElementCount)
	{
		pVertexElement = &pVertexElements[uVertexElement];

		uStreamSize = CVertexDeclaration::GetVertexElementStreamStrideOf(pVertexElement->uStream, pVertexElements, uElementCount);

		uOffset = pVertexElement->uOffset;

		puData = reinterpret_cast<PUINT8>(MeshData.ppVertexData[pVertexElement->uStream]);

		for(j = 0; j < uNumVerticesY; j ++)
		{
			for(i = 0; i < uNumVerticesX; i ++)
			{
				pfOffset = reinterpret_cast<PFLOAT>(puData + uOffset);

				* pfOffset             = 1.0f;
				*(pfOffset + uYOffset) = 0.0f;
				*(pfOffset + uZOffset) = 0.0f;

				uOffset += uStreamSize;
			}
		}
	}

	uVertexElement = CVertexDeclaration::FindVertexElement(CVertexDeclaration::BINORMAL, CVertexDeclaration::FLOAT3, 0, pVertexElements, uElementCount);

	if(uVertexElement < uElementCount)
	{
		pVertexElement = &pVertexElements[uVertexElement];

		uStreamSize = CVertexDeclaration::GetVertexElementStreamStrideOf(pVertexElement->uStream, pVertexElements, uElementCount);

		uOffset = pVertexElement->uOffset;

		puData = reinterpret_cast<PUINT8>(MeshData.ppVertexData[pVertexElement->uStream]);

		for(j = 0; j < uNumVerticesY; j ++)
		{
			for(i = 0; i < uNumVerticesX; i ++)
			{
				pfOffset = reinterpret_cast<PFLOAT>(puData + uOffset);

				* pfOffset             = 0.0f;
				*(pfOffset + uYOffset) = 1.0f;
				*(pfOffset + uZOffset) = 0.0f;

				uOffset += uStreamSize;
			}
		}
	}

	uOffset = 0;

	MeshData.uNumIndices = uWidthSegment * uHeightSegment * 6;

	if(MeshData.uNumIndices < 0xffff)
	{
		MeshData.IndexFormat = CIndexBuffer::BIT16;

		ALCHEMY_DEBUG_NEW(MeshData.pIndexData, UINT8[MeshData.uNumIndices * sizeof(UINT16)]);

		PUINT16 puIndexData16 = reinterpret_cast<PUINT16>(MeshData.pIndexData);
		UINT16 uApha16, uBeta16;

		for(j = 0; j < uHeightSegment; j ++)
		{
			for(i = 0; i < uWidthSegment; i ++)
			{
				uApha16 = uOffset + uNumVerticesX;
				uBeta16 = uOffset + 1            ;

				*puIndexData16 ++ = uOffset;
				*puIndexData16 ++ = uApha16;
				*puIndexData16 ++ = uBeta16;

				*puIndexData16 ++ = uBeta16;
				*puIndexData16 ++ = uApha16;
				*puIndexData16 ++ = uApha16 + 1;

				uOffset ++;
			}

			uOffset ++;
		}
	}
	else
	{
		MeshData.IndexFormat = CIndexBuffer::BIT32;

		ALCHEMY_DEBUG_NEW(MeshData.pIndexData, UINT8[MeshData.uNumIndices * sizeof(UINT32)]);

		PUINT32 puIndexData32 = reinterpret_cast<PUINT32>(MeshData.pIndexData);
		UINT32 uApha32, uBeta32;

		for(j = 0; j < uHeightSegment; j ++)
		{
			for(i = 0; i < uWidthSegment; i ++)
			{
				uApha32 = uOffset + uNumVerticesX;
				uBeta32 = uOffset + 1            ;

				*(puIndexData32 ++) = uOffset;
				*(puIndexData32 ++) = uApha32;
				*(puIndexData32 ++) = uBeta32;

				*(puIndexData32 ++) = uBeta32;
				*(puIndexData32 ++) = uApha32;
				*(puIndexData32 ++) = uApha32 + 1;

				uOffset ++;
			}

			uOffset ++;
		}
	}

	MeshData.uIndexUsage = uIndexUsage;

	UINT uWidthUnit = uWidthSegment / uWidthStep, uHeightUnit = uHeightSegment / uHeightStep;
	MeshData.uAttributeRangeCount = uWidthUnit * uHeightUnit;
	CMesh::ATTRIBUTERANGE* pAttributeRange;

	ALCHEMY_DEBUG_NEW(pAttributeRange, CMesh::ATTRIBUTERANGE[MeshData.uAttributeRangeCount]);
	MeshData.pAttributeRange = pAttributeRange;

	UINT uIndexCount = uWidthStep * uHeightStep * 6;
	uOffset = 0;
	for(j = 0; j < uHeightUnit; ++ j)
	{
		for(i = 0; i < uWidthUnit; ++ i)
		{
			pAttributeRange->uStartIndex = uOffset;
			pAttributeRange->uIndexCount = uIndexCount;

			uOffset += uIndexCount;

			++ pAttributeRange;
		}
	}

	return m_MeshPool.Add(MeshData) + 1;
}

bool CMeshManager::DestroySceneData(UINT uHandle)
{
	if(!uHandle)
		return false;

	UINT uIndex = uHandle - 1;
	SCENEDATA* pSceneData = m_ScenePool.Get(uIndex);

	if(pSceneData)
	{
		__DestroySceneData(*pSceneData);

		return m_ScenePool.Remove(uIndex, 1);
	}

	return false;
}

bool CMeshManager::DestroyMeshData(UINT uHandle)
{
	if(!uHandle)
		return false;

	UINT uIndex = uHandle - 1;
	MESHDATA* pMeshData = m_MeshPool.Get(uIndex);

	if(pMeshData)
	{
		__DestroyMeshData(*pMeshData);

		return m_MeshPool.Remove(uIndex, 1);
	}

	return false;
}

void CMeshManager::Destroy()
{
	CPool<SCENEDATA>::CIterator SceneIterator = m_ScenePool.GetBeginIterator();

	while( CPool<SCENEDATA>::IsValuable(SceneIterator) )
	{
		__DestroySceneData(*SceneIterator);

		++ SceneIterator;
	}

	CPool<MESHDATA>::CIterator MeshIterator = m_MeshPool.GetBeginIterator();

	while( CPool<MESHDATA>::IsValuable(MeshIterator) )
	{
		__DestroyMeshData(*MeshIterator);

		++ MeshIterator;
	}
}

void CMeshManager::__DestroySceneData(SCENEDATA& SceneData)const
{
	if(SceneData.uSceneDataCount > 1)
	{
		ALCHEMY_DEBUG_DELETE_ARRAY(SceneData.pSceneNodeData);
	}
	else
		ALCHEMY_DEBUG_DELETE(SceneData.pSceneNodeData);

	SceneData.uSceneDataCount = 0;
}

void CMeshManager::__DestroyMeshData(MESHDATA& MeshData)const
{
	UINT uVertexBufferSize = CVertexDeclaration::GetVertexElementStreamCount(MeshData.VertexElementData.pVertexElement, MeshData.VertexElementData.uElementCount);

	for(UINT i = 0; i < uVertexBufferSize; ++ i)
		ALCHEMY_DEBUG_DELETE_ARRAY(MeshData.ppVertexData[i]);

	ALCHEMY_DEBUG_DELETE_ARRAY(MeshData.ppVertexData);
	ALCHEMY_DEBUG_DELETE_ARRAY(MeshData.pIndexData);

	ALCHEMY_DEBUG_DELETE_ARRAY(MeshData.VertexElementData.pVertexElement);
	ALCHEMY_DEBUG_DELETE_ARRAY(MeshData.VertexElementData.puUsage);

	ALCHEMY_DEBUG_DELETE_ARRAY(MeshData.pAttributeRange);

	MeshData.uNumVertices                    = 0;
	MeshData.uNumIndices                     = 0;
	MeshData.VertexElementData.uElementCount = 0;
	MeshData.uAttributeRangeCount            = 0;
}

PVOID* CMeshManager::__CreateVertexData(UINT uNumVertices, const CVertexDeclaration::VERTEXELEMENT* pVertexElements, UINT uElementCount, PUINT puStreamCount)
{
	PVOID* ppData;
	UINT uTotalStream = CVertexDeclaration::GetVertexElementStreamCount(pVertexElements, uElementCount);

	if(puStreamCount)
		*puStreamCount = uTotalStream;

	ALCHEMY_DEBUG_NEW(ppData, PVOID[uTotalStream]);

	for(UINT i = 0; i < uTotalStream; i ++)
		ALCHEMY_DEBUG_NEW(ppData[i], UINT8[CVertexDeclaration::GetVertexElementStreamStrideOf(i, pVertexElements, uElementCount) * uNumVertices]);

	return ppData;
}

void CMeshManager::__DestroyVertexData(PVOID* ppData, UINT uStreamCount)
{
	for(UINT i = 0; i < uStreamCount; i ++)
		ALCHEMY_DEBUG_DELETE_ARRAY(ppData[i]);

	ALCHEMY_DEBUG_DELETE_ARRAY(ppData);
}