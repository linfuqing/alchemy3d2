#include "AlchemyMeshManager.h"
#include "AlchemyEffect.h"
#include "AlchemySkinInfo.h"
#include "AlchemyAnimationController.h"
#include "AlchemySceneManager.h"

#include "../Math/AlchemyFloat2.h"
#include "../Math/AlchemyVector3.h"
#include "../Math/AlchemyVector4.h"

using namespace alchemy;

CMeshManager::CMeshManager(void) :
CSingleton<CMeshManager>(*this)
{
}

CMeshManager::~CMeshManager(void)
{
	Destroy();
}

void CMeshManager::DestroyMeshData(MESHDATA& MeshData)
{
	UINT uVertexBufferSize = CVertexDeclaration::GetVertexElementStreamCount(MeshData.VertexElementData.pVertexElement, MeshData.VertexElementData.uElementCount);

	if(MeshData.ppVertexData)
	{
		for(UINT i = 0; i < uVertexBufferSize; ++ i)
			ALCHEMY_DEBUG_DELETE_ARRAY(MeshData.ppVertexData[i]);

		ALCHEMY_DEBUG_DELETE_ARRAY(MeshData.ppVertexData);
	}

	ALCHEMY_DEBUG_DELETE_ARRAY(MeshData.pIndexData);

	ALCHEMY_DEBUG_DELETE_ARRAY(MeshData.VertexElementData.pVertexElement);
	ALCHEMY_DEBUG_DELETE_ARRAY(MeshData.VertexElementData.puUsage);

	ALCHEMY_DEBUG_DELETE_ARRAY(MeshData.pAttributeRange);

	MeshData.uNumVertices                    = 0;
	MeshData.uNumIndices                     = 0;
	MeshData.VertexElementData.uElementCount = 0;
	MeshData.uAttributeRangeCount            = 0;
}

void CMeshManager::DestroyTextureData(CMeshManager::TEXTUREDATA& TextureData)
{
	ALCHEMY_DEBUG_DELETE_ARRAY(TextureData.pData);
	ALCHEMY_DEBUG_DELETE_ARRAY(TextureData.pcFileName);
}

void CMeshManager::DestroyMaterialData(MATERIALDATA& MaterialData)
{
	ALCHEMY_DEBUG_DELETE_ARRAY(MaterialData.pTextureData);

	MaterialData.uTextureCount = 0;
}

void CMeshManager::DestroySkinData(CMeshManager::SKINDATA& SkinData) 
{
	UINT i;
	for (i=0; i<SkinData.uBoneDataCount; ++i)
	{
		ALCHEMY_DEBUG_DELETE_ARRAY( SkinData.pBoneData[i].pcName );
		ALCHEMY_DEBUG_DELETE_ARRAY( SkinData.pBoneData[i].pfWeights );
		ALCHEMY_DEBUG_DELETE_ARRAY( SkinData.pBoneData[i].puIndices );
	}

	ALCHEMY_DEBUG_DELETE_ARRAY(SkinData.pBoneData);
}

void CMeshManager::DestroySceneNodeData(SCENENODEDATA& SceneNodeData)
{
	ALCHEMY_DEBUG_DELETE_ARRAY(SceneNodeData.pcName);
}

void CMeshManager::DestroySceneData(SCENEDATA& SceneData)
{
	ALCHEMY_DEBUG_DELETE_ARRAY(SceneData.pSceneNodeDatas);

	SceneData.uSceneDataCount = 0;
}

CMeshManager::HANDLE CMeshManager::AddMeshData(const CMeshManager::MESHDATA& MeshData)
{
	MESHDATA CurMeshData;
	CurMeshData.IndexFormat = MeshData.IndexFormat;
	CurMeshData.VertexElementData = MeshData.VertexElementData;
	CurMeshData.uIndexUsage = MeshData.uIndexUsage;
	CurMeshData.uNumIndices = MeshData.uNumIndices;
	CurMeshData.uNumVertices = MeshData.uNumVertices;

	CurMeshData.VertexElementData.uElementCount = MeshData.VertexElementData.uElementCount;
	if(CurMeshData.VertexElementData.uElementCount)
	{
		ALCHEMY_DEBUG_NEW(CurMeshData.VertexElementData.puUsage, UINT[CurMeshData.VertexElementData.uElementCount]);
		ALCHEMY_DEBUG_NEW(CurMeshData.VertexElementData.pVertexElement, CVertexDeclaration::VERTEXELEMENT[CurMeshData.VertexElementData.uElementCount]);

		for (UINT i=0; i<CurMeshData.VertexElementData.uElementCount; ++i)
		{
			CurMeshData.VertexElementData.puUsage[i] = MeshData.VertexElementData.puUsage[i];
			CurMeshData.VertexElementData.pVertexElement[i] = MeshData.VertexElementData.pVertexElement[i];
		}
	}

	CurMeshData.uAttributeRangeCount = MeshData.uAttributeRangeCount;
	if(CurMeshData.uAttributeRangeCount)
	{
		ALCHEMY_DEBUG_NEW(CurMeshData.pAttributeRange, CMesh::ATTRIBUTERANGE[CurMeshData.uAttributeRangeCount]);
		for (UINT i=0; i<CurMeshData.uAttributeRangeCount; ++i)
		{
			CurMeshData.pAttributeRange[i].uStartIndex = MeshData.pAttributeRange[i].uStartIndex;
			CurMeshData.pAttributeRange[i].uIndexCount = MeshData.pAttributeRange[i].uIndexCount;
		}
	}

	if(CurMeshData.uNumIndices)
	{
		UINT uIndexSize = sizeof(UINT16);
		if(CurMeshData.IndexFormat == INDEXBUFFER::BIT16)
		{
			PUINT16 pIndicesData;
			ALCHEMY_DEBUG_NEW(pIndicesData, UINT16[CurMeshData.uNumIndices]);
			CurMeshData.pIndexData = reinterpret_cast<PVOID>(pIndicesData);
		}
		else
		{
			uIndexSize = sizeof(UINT);
			PUINT pIndicesData;
			ALCHEMY_DEBUG_NEW(pIndicesData, UINT[CurMeshData.uNumIndices]);
			CurMeshData.pIndexData = reinterpret_cast<PVOID>(pIndicesData);
		}

		ALCHEMY_MEMORY_COPY(CurMeshData.pIndexData, MeshData.pIndexData, CurMeshData.uNumIndices * uIndexSize);
	}

	if(CurMeshData.uNumVertices)
	{
		UINT uStreamNum = CVertexDeclaration::GetVertexElementStreamCount(CurMeshData.VertexElementData.pVertexElement, CurMeshData.VertexElementData.uElementCount);
		UINT uStreamSize;
		ALCHEMY_DEBUG_NEW( CurMeshData.ppVertexData, PVOID[uStreamNum] );
		for (UINT i=0; i<uStreamNum; ++i)
		{
			uStreamSize = CVertexDeclaration::GetVertexElementStreamStrideOf(i,CurMeshData.VertexElementData.pVertexElement, CurMeshData.VertexElementData.uElementCount);
			ALCHEMY_DEBUG_NEW( CurMeshData.ppVertexData[i], UINT8[ uStreamSize*CurMeshData.uNumVertices ] );
			ALCHEMY_MEMORY_COPY(CurMeshData.ppVertexData[i], MeshData.ppVertexData[i], CurMeshData.uNumVertices * uStreamSize);
		}
	}

	return m_MeshPool.Add( CurMeshData ) + 1;
}

CMeshManager::HANDLE CMeshManager::AddTextureData(const CMeshManager::TEXTUREDATA& TextureData)
{
	TEXTUREDATA CurTextureData;
	CurTextureData.Format = TextureData.Format;
	CurTextureData.uHeight = TextureData.uHeight;
	CurTextureData.uWidth = TextureData.uWidth;
	CurTextureData.uUsage = TextureData.uUsage;
	CurTextureData.uMipLevel = TextureData.uMipLevel;
 
	if(TextureData.pData)
	{
		UINT uByteSize = CurTextureData.uHeight * CurTextureData.uWidth * CTexture::GetFormatSize(CurTextureData.Format);
		ALCHEMY_DEBUG_NEW( CurTextureData.pData, UINT8[uByteSize] );
		ALCHEMY_MEMORY_COPY(CurTextureData.pData, TextureData.pData, uByteSize);
	}
	else
		CurTextureData.pData = ALCHEMY_NULL;

	if(TextureData.pcFileName)
	{
		UINT uLength = ALCHEMY_STRING_LENGTH(TextureData.pcFileName);
		ALCHEMY_DEBUG_NEW(CurTextureData.pcFileName, CHAR[uLength + 1]);
		for(UINT i = 0; i < uLength; ++ i)
			CurTextureData.pcFileName[i] = TextureData.pcFileName[i];

		CurTextureData.pcFileName[uLength] = 0;
	}
	else
		CurTextureData.pcFileName = ALCHEMY_NULL;

	return m_TexturePool.Add(CurTextureData) + 1;
}

CMeshManager::HANDLE CMeshManager::AddMaterialData(const CMeshManager::MATERIALDATA& MaterialData)
{
	MATERIALDATA CurMaterialData;

	CurMaterialData.Ambient = MaterialData.Ambient;
	CurMaterialData.Diffuse = MaterialData.Diffuse;
	CurMaterialData.Emissive = MaterialData.Emissive;
	CurMaterialData.fPower = MaterialData.fPower;
	CurMaterialData.Specular = MaterialData.Specular;

	CurMaterialData.uTextureCount = MaterialData.uTextureCount;

	if(CurMaterialData.uTextureCount)
	{
		ALCHEMY_DEBUG_NEW(CurMaterialData.pTextureData,HANDLE[CurMaterialData.uTextureCount]);
		for (UINT i=0; i<CurMaterialData.uTextureCount; ++i)
			CurMaterialData.pTextureData[i] = MaterialData.pTextureData[i];
	}
	else
		CurMaterialData.pTextureData = ALCHEMY_NULL;

	return m_MaterialPool.Add(CurMaterialData) + 1;
}

CMeshManager::HANDLE CMeshManager::AddSkinData(const CMeshManager::SKINDATA& SkinData)
{
	SKINDATA DestSkinData = {0};
	CSkinInfo::BONE* pBoneData, *pSourceBoneData;
	UINT uNameLen;

	DestSkinData.uBoneDataCount = SkinData.uBoneDataCount;

	if(SkinData.pBoneData && SkinData.uBoneDataCount)
	{
		ALCHEMY_DEBUG_NEW(DestSkinData.pBoneData, CSkinInfo::BONE[DestSkinData.uBoneDataCount]);
		for (UINT j=0; j<DestSkinData.uBoneDataCount; ++j)
		{
			pBoneData = &DestSkinData.pBoneData[j];
			pSourceBoneData = &SkinData.pBoneData[j];

			pBoneData->OffsetMatrix	   = pSourceBoneData->OffsetMatrix;
			pBoneData->uInfluenceCount = pSourceBoneData->uInfluenceCount;

			uNameLen = pSourceBoneData->pcName == ALCHEMY_NULL ? 0 : StringLength(pSourceBoneData->pcName);

			if(uNameLen > 0)
			{
				ALCHEMY_DEBUG_NEW(pBoneData->pcName, CHAR[uNameLen+1]);
				pBoneData->pcName[uNameLen] = 0;

				for (UINT c=0; c<uNameLen; ++c)
					pBoneData->pcName[c] = pSourceBoneData->pcName[c];
			}

			ALCHEMY_DEBUG_NEW(pBoneData->pfWeights, FLOAT[pBoneData->uInfluenceCount]);
			ALCHEMY_MEMORY_COPY(pBoneData->pfWeights, pSourceBoneData->pfWeights, sizeof(FLOAT)*pBoneData->uInfluenceCount);

			ALCHEMY_DEBUG_NEW(pBoneData->puIndices, UINT[pBoneData->uInfluenceCount]);
			ALCHEMY_MEMORY_COPY(pBoneData->puIndices, pSourceBoneData->puIndices, sizeof(UINT)*pBoneData->uInfluenceCount);
		}
	}

	return m_SkinPool.Add(DestSkinData) + 1;
}

CMeshManager::HANDLE CMeshManager::AddSceneNodeData(const CMeshManager::SCENENODEDATA& SceneNodeData)
{
	SCENENODEDATA CurrentSceneNodeData;

	if(SceneNodeData.pcName)
	{
		CurrentSceneNodeData.Parent = SceneNodeData.Parent;

		UINT uLength = ALCHEMY_STRING_LENGTH(SceneNodeData.pcName);
		ALCHEMY_DEBUG_NEW(CurrentSceneNodeData.pcName, CHAR[uLength + 1]);
		for (UINT i = 0; i < uLength; ++ i)
			CurrentSceneNodeData.pcName[i] = SceneNodeData.pcName[i];

		CurrentSceneNodeData.pcName[uLength] = 0;
	}
	else
		CurrentSceneNodeData.pcName = ALCHEMY_NULL;

	CurrentSceneNodeData.Type = SceneNodeData.Type;
	Copy(CurrentSceneNodeData.Transform, SceneNodeData.Transform);

	CurrentSceneNodeData.MeshData = SceneNodeData.MeshData;
	CurrentSceneNodeData.uSubset = SceneNodeData.uSubset;
	CurrentSceneNodeData.MaterialData = SceneNodeData.MaterialData;
	CurrentSceneNodeData.SkinData = SceneNodeData.SkinData;

	CurrentSceneNodeData.RootBone = SceneNodeData.RootBone;

	return m_SceneNodePool.Add(CurrentSceneNodeData) + 1;
}

CMeshManager::HANDLE CMeshManager::AddSceneData(const CMeshManager::SCENEDATA& SceneData)
{
	SCENEDATA CurrentSceneData;

	CurrentSceneData.uSceneDataCount = SceneData.uSceneDataCount;
	if(CurrentSceneData.uSceneDataCount)
	{
		ALCHEMY_DEBUG_NEW(CurrentSceneData.pSceneNodeDatas, HANDLE[CurrentSceneData.uSceneDataCount]);

		for(UINT i = 0; i < CurrentSceneData.uSceneDataCount; ++ i)
			CurrentSceneData.pSceneNodeDatas[i] = SceneData.pSceneNodeDatas[i];
	}

	return m_ScenePool.Add(CurrentSceneData) + 1;
}

CMeshManager::HANDLE CMeshManager::CreateBillboardMeshData( 
								FLOAT fWidth, 
								FLOAT fHeight, 
								UINT uBillboardNum,
							    const CVertexDeclaration::LPVERTEXELEMENT pVertexElements,
							    const UINT* pVertexUsage,
							    UINT  uElementCount,
							    UINT  uIndexUsage )
{
	if(!pVertexElements || !uElementCount || !pVertexUsage)
		return 0;

	MESHDATA MeshData = {0};
	MeshData.uNumVertices = uBillboardNum << 2;
	MeshData.uNumIndices  = uBillboardNum * 6;
	MeshData.uIndexUsage = uIndexUsage;

	MeshData.IndexFormat = MeshData.uNumVertices >= 0xffff ? CIndexBuffer::BIT32 : CIndexBuffer::BIT16;

	UINT i, j,uIndex = 0;
	PUINT8 pIndicesData;
	ALCHEMY_DEBUG_NEW(pIndicesData, UINT8[ MeshData.IndexFormat * MeshData.uNumIndices * sizeof(UINT16) ]);
	MeshData.pIndexData = reinterpret_cast<PVOID>(pIndicesData);

	if(MeshData.IndexFormat == CIndexBuffer::BIT16)
	{
		PUINT16 pIndex16Data = reinterpret_cast<PUINT16>(pIndicesData);
		for (i=0; i<uBillboardNum; ++i)
		{
			pIndex16Data[i*6] = uIndex;
			pIndex16Data[i*6+1] = uIndex+1;
			pIndex16Data[i*6+2] = uIndex+2;

			pIndex16Data[i*6+3] = uIndex;
			pIndex16Data[i*6+4] = uIndex+2;
			pIndex16Data[i*6+5] = uIndex+3;

			uIndex += 4;
		}
	}
	else
	{
		PUINT32 pIndex32Data = reinterpret_cast<PUINT32>(pIndicesData);
		for (i=0; i<uBillboardNum; ++i)
		{
			pIndex32Data[i*6] = uIndex;
			pIndex32Data[i*6+1] = uIndex+1;
			pIndex32Data[i*6+2] = uIndex+2;

			pIndex32Data[i*6+3] = uIndex;
			pIndex32Data[i*6+4] = uIndex+2;
			pIndex32Data[i*6+5] = uIndex+3;

			uIndex += 4;
		}
	}

	ALCHEMY_DEBUG_NEW(MeshData.pAttributeRange, CMesh::ATTRIBUTERANGE);
	MeshData.pAttributeRange->uStartIndex = 0;
	MeshData.pAttributeRange->uIndexCount = MeshData.uNumIndices;
	MeshData.uAttributeRangeCount = 1;
	
	UINT uStream = CVertexDeclaration::GetVertexElementStreamCount(pVertexElements, uElementCount);
	if(!uStream)
		return 0;

	PUINT8* ppVertexData;
	ALCHEMY_DEBUG_NEW(ppVertexData, PUINT8[uStream]);
	for (i=0; i<uStream; ++i)
	{
		ALCHEMY_DEBUG_NEW(ppVertexData[i], UINT8[ CVertexDeclaration::GetVertexElementStreamStrideOf( i, pVertexElements, uElementCount ) * MeshData.uNumVertices ]);
	}

	MeshData.VertexElementData.uElementCount = uElementCount;
	ALCHEMY_DEBUG_NEW( MeshData.VertexElementData.pVertexElement, CVertexDeclaration::VERTEXELEMENT[uElementCount] );
	ALCHEMY_DEBUG_NEW( MeshData.VertexElementData.puUsage, UINT[uElementCount] );

	for (i=0; i<uElementCount; ++i)
	{
		MeshData.VertexElementData.pVertexElement[i] = pVertexElements[i];
		MeshData.VertexElementData.puUsage[i]		 = pVertexUsage[i];
	}

	UINT uStride;
	const CVertexDeclaration::VERTEXELEMENT* pElement;
	FLOAT4* pPos;
	FLOAT2* pTexcoord;

	FLOAT fHalfWidth = fWidth * 0.5f, fHalfHeight = fHeight * 0.5f;

	PUINT8 pData;

	for (i=0; i<uElementCount; ++i)
	{
		pElement = &pVertexElements[i];
		pData = ppVertexData[ pElement->uStream ];
		uStride = CVertexDeclaration::GetVertexElementStreamStrideOf( pElement->uStream, pVertexElements, uElementCount );
		switch (pElement->Type)
		{
		case CVertexDeclaration::POSITION:
			{
				for (j=0; j<uBillboardNum; ++j)
				{
					pPos = reinterpret_cast<FLOAT4*>( pData + j*4*uStride + pElement->uOffset );
					pPos->x =  -fHalfWidth;
					pPos->y =  fHalfHeight;
					pPos->z =  0.0f;
					pPos->w =  1.0f;

					pPos = reinterpret_cast<FLOAT4*>( pData + (j*4+1)*uStride + pElement->uOffset );
					pPos->x =  fHalfWidth;
					pPos->y =  fHalfHeight;
					pPos->z =  0.0f;
					pPos->w =  1.0f;

					pPos = reinterpret_cast<FLOAT4*>( pData + (j*4+2)*uStride + pElement->uOffset );
					pPos->x =  fHalfWidth;
					pPos->y =  -fHalfHeight;
					pPos->z =  0.0f;
					pPos->w =  1.0f;

					pPos = reinterpret_cast<FLOAT4*>( pData + (j*4+3)*uStride + pElement->uOffset );
					pPos->x =  -fHalfWidth;
					pPos->y =  -fHalfHeight;
					pPos->z =  0.0f;
					pPos->w =  1.0f;
				}
			}
			break;
		case CVertexDeclaration::TEXCOORD:
			{
				for (j=0; j<uBillboardNum; ++j)
				{
					pTexcoord = reinterpret_cast<FLOAT2*>( pData + j*4*uStride + pElement->uOffset );
					pTexcoord->x =  0.0f;
					pTexcoord->y =  0.0f;

					pTexcoord = reinterpret_cast<FLOAT2*>( pData + (j*4+1)*uStride + pElement->uOffset );
					pTexcoord->x =  1.0f;
					pTexcoord->y =  0.0f;

					pTexcoord = reinterpret_cast<FLOAT2*>( pData + (j*4+2)*uStride + pElement->uOffset );
					pTexcoord->x =  1.0f;
					pTexcoord->y =  1.0f;

					pTexcoord = reinterpret_cast<FLOAT2*>( pData + (j*4+3)*uStride + pElement->uOffset );
					pTexcoord->x =  0.0f;
					pTexcoord->y =  1.0f;
				}
			}
			break;
		}
	}

	return m_MeshPool.Add(MeshData) + 1;
}

CMeshManager::HANDLE CMeshManager::CreateSkyDome(
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
	CFloat3* pUVWData = ALCHEMY_NULL;
	CFloat2* pUVData  = ALCHEMY_NULL;
	bool isCubeTexture = false; 

	UINT uVertexElement = CVertexDeclaration::FindVertexElement(CVertexDeclaration::POSITION, CVertexDeclaration::FLOAT4, 0, pVertexElements, uElementCount);
	pfPositionData = reinterpret_cast<CFloat4*>( MeshData.ppVertexData[ pVertexElements[ uVertexElement ].uStream ] );

	uVertexElement = CVertexDeclaration::FindVertexElement( CVertexDeclaration::TEXCOORD, CVertexDeclaration::FLOAT2, 0, pVertexElements, uElementCount );
	if(uVertexElement == uElementCount)
	{
		isCubeTexture = true;
		uVertexElement = CVertexDeclaration::FindVertexElement(CVertexDeclaration::TEXCOORD, CVertexDeclaration::FLOAT3, 0, pVertexElements, uElementCount);

		if(uVertexElement < uElementCount)
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

			if(pUVWData || pUVData )
			{
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
			}

			++uVertexIndex;
		}

		++uCurrentPhi;
	}

	pfPositionData[0].x = 0.0f;
	pfPositionData[0].y = fRadius;
	pfPositionData[0].z = 0.0f;
	pfPositionData[0].w = 1.0f;

	if(pUVWData || pUVData )
	{
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

CMeshManager::HANDLE CMeshManager::CreatePlane(
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

		Temp = CFloat2(0.0f, 0.0f);

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
			Temp.y -= 1.0f;//fHeightPerSegment;
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

				*(puIndexData16 ++) = uOffset;
				*(puIndexData16 ++) = uApha16;
				*(puIndexData16 ++) = uBeta16;

				*(puIndexData16 ++) = uBeta16;
				*(puIndexData16 ++) = uApha16;
				*(puIndexData16 ++) = uApha16 + 1;

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

bool CMeshManager::DestroyMeshData(UINT uHandle)
{
	if(!uHandle)
		return false;

	UINT uIndex = uHandle - 1;
	MESHDATA* pMeshData = m_MeshPool.Get(uIndex);

	if(pMeshData)
	{
		DestroyMeshData(*pMeshData);

		return m_MeshPool.Remove(uIndex, 1);
	}

	return false;
}

bool CMeshManager::DestroyTextureData(HANDLE uHandle)
{
	if(!uHandle)
		return false;

	UINT uIndex = uHandle - 1;
	TEXTUREDATA* pTextureData = m_TexturePool.Get(uIndex);

	if(pTextureData)
	{
		DestroyTextureData(*pTextureData);

		return m_TexturePool.Remove(uIndex, 1);
	}

	return false;
}

bool CMeshManager::DestroyMaterialData(HANDLE uHandle)
{
	if(!uHandle)
		return false;

	UINT uIndex = uHandle - 1;
	MATERIALDATA* pMaterialData = m_MaterialPool.Get(uIndex);

	if(pMaterialData)
	{
		DestroyMaterialData(*pMaterialData);

		return m_MaterialPool.Remove(uIndex, 1);
	}

	return false;
}

bool CMeshManager::DestroySkinData(HANDLE uHandle)
{
	if(!uHandle)
		return false;

	DestroySkinData( *m_SkinPool.Get(uHandle - 1) );

	return true;
}

bool CMeshManager::DestroySceneNodeData(HANDLE uHandle)
{
	if(!uHandle)
		return false;

	UINT uIndex = uHandle - 1;
	SCENENODEDATA* pSceneNodeData = m_SceneNodePool.Get(uIndex);

	if(pSceneNodeData)
	{
		DestroySceneNodeData(*pSceneNodeData);

		return m_SceneNodePool.Remove(uIndex, 1);
	}

	return false;
}

bool CMeshManager::DestroySceneData(UINT uHandle)
{
	if(!uHandle)
		return false;

	UINT uIndex = uHandle - 1;
	SCENEDATA* pSceneData = m_ScenePool.Get(uIndex);

	if(pSceneData)
	{
		DestroySceneData(*pSceneData);

		return m_ScenePool.Remove(uIndex, 1);
	}

	return false;
}

void CMeshManager::Destroy()
{
	CPool<MESHDATA>::CIterator MeshIterator = m_MeshPool.GetIterator(0);

	while( CPool<MESHDATA>::IsValuable(MeshIterator) )
	{
		DestroyMeshData(*MeshIterator);

		++ MeshIterator;
	}

	CPool<TEXTUREDATA>::CIterator TextureIterator = m_TexturePool.GetIterator(0);

	while ( CPool<TEXTUREDATA>::IsValuable(TextureIterator) )
	{
		DestroyTextureData(*TextureIterator);

		++ TextureIterator;
	}

	CPool<MATERIALDATA>::CIterator MaterialIterator = m_MaterialPool.GetIterator(0);

	while ( CPool<MATERIALDATA>::IsValuable(MaterialIterator) )
	{
		DestroyMaterialData(*MaterialIterator);

		++ MaterialIterator;
	}

	CPool<SKINDATA>::CIterator SkinDataIterator = m_SkinPool.GetIterator(0);
	while ( CPool<SKINDATA>::IsValuable( SkinDataIterator ) )
	{
		DestroySkinData(*SkinDataIterator);

		++ SkinDataIterator;
	}

	CPool<SCENENODEDATA>::CIterator SceneNodeIterator = m_SceneNodePool.GetIterator(0);

	while( CPool<SCENENODEDATA>::IsValuable(SceneNodeIterator) )
	{
		DestroySceneNodeData(*SceneNodeIterator);

		++ SceneNodeIterator;
	}

	CPool<SCENEDATA>::CIterator SceneIterator = m_ScenePool.GetIterator(0);

	while( CPool<SCENEDATA>::IsValuable(SceneIterator) )
	{
		DestroySceneData(*SceneIterator);

		++ SceneIterator;
	}
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