#include "AlchemySceneDataFiler.h"

#include "AlchemyLoader.h"

using namespace alchemy;

CSceneDataFiler::CSceneDataFiler(void)
{
}

CSceneDataFiler::~CSceneDataFiler(void)
{
}

void CSceneDataFiler::ClearIndexMap(UINT uFlag)
{
	for(UINT i = 0; i < TOTAL_DATA_FORMAT; ++ i)
	{
		if( ALCHEMY_TEST_BIT(uFlag, i) )
			m_IndexMap[i].Clear();
	}
}

bool CSceneDataFiler::WriteMeshData(const CMeshManager::MESHDATA& Data, IFileWriter& Writer, const CHAR* pcPath)
{
	UINT uFlie = Writer.Open(pcPath, IFileWriter::BINARY, true);
	if(uFlie)
	{
		Writer.Write( uFlie, &Data.uNumVertices, sizeof(UINT) );
		Writer.Write( uFlie, &Data.uNumIndices, sizeof(UINT) );
		Writer.Write( uFlie, &Data.uIndexUsage, sizeof(UINT) );
		Writer.Write( uFlie, &Data.IndexFormat, sizeof(CIndexBuffer::FORMAT) );
		Writer.Write( uFlie, &Data.VertexElementData.uElementCount, sizeof(UINT) );

		Writer.Write( uFlie, Data.VertexElementData.pVertexElement, Data.VertexElementData.uElementCount * sizeof(CVertexDeclaration::VERTEXELEMENT) );

		UINT uStreamCount = CVertexDeclaration::GetVertexElementStreamCount(Data.VertexElementData.pVertexElement, Data.VertexElementData.uElementCount);

		Writer.Write( uFlie, Data.VertexElementData.puUsage, uStreamCount * sizeof(UINT) );

		Writer.Write(uFlie, &Data.uAttributeRangeCount, sizeof(UINT) );

		Writer.Write( uFlie, Data.pAttributeRange, Data.uAttributeRangeCount * sizeof(CMesh::ATTRIBUTERANGE) );

		UINT uStream = 0, uMaxStream = CVertexDeclaration::GetVertexElementMaxinumStream(Data.VertexElementData.pVertexElement, Data.VertexElementData.uElementCount), uStride;
		for(UINT i = 0; i < uStreamCount; ++ i)
		{
			while( ( uStride = CVertexDeclaration::GetVertexElementStreamStrideOf(uStream, Data.VertexElementData.pVertexElement, Data.VertexElementData.uElementCount) ) && (uStream ++) < uMaxStream );

			if(uStride)
				Writer.Write(uFlie, Data.ppVertexData[i], uStride * Data.uNumVertices);
		}

		Writer.Write(uFlie, Data.pIndexData, (Data.IndexFormat == CIndexBuffer::BIT32 ? 4 : 2) * Data.uNumIndices);

		return Writer.Close(uFlie);
	}

	return false;
}

bool CSceneDataFiler::WriteTextureData(const CMeshManager::TEXTUREDATA& Data, IFileWriter& Writer, const CHAR* pcPath)
{
	UINT uFlie = Writer.Open(pcPath, IFileWriter::BINARY, true);
	if(uFlie)
	{
		Writer.Write( uFlie, &Data.uWidth, sizeof(UINT) );
		Writer.Write( uFlie, &Data.uHeight, sizeof(UINT) );
		Writer.Write( uFlie, &Data.Format, sizeof(CTexture::FORMAT) );
		Writer.Write( uFlie, &Data.uUsage, sizeof(UINT) );
		Writer.Write( uFlie, &Data.uMipLevel, sizeof(UINT) );

		UINT uNameLength = ALCHEMY_STRING_LENGTH(Data.pcFileName);
		Writer.Write( uFlie, &uNameLength, sizeof(UINT) );
		if(uNameLength)
			Writer.Write( uFlie, Data.pcFileName, sizeof(CHAR) * (uNameLength + 1) );
		else
			Writer.Write( uFlie, Data.pData, Data.uWidth * Data.uHeight * CTexture::GetFormatSize(Data.Format) );

		return Writer.Close(uFlie);
	}

	return false;
}

bool CSceneDataFiler::WriteMaterialData(const CMeshManager::MATERIALDATA& Data, IFileWriter& Writer, const CHAR* pcPath, UINT uIndex, const CHAR* pcTexturePath, PUINT puOutputIndex)
{
	UINT uFlie = Writer.Open(pcPath, IFileWriter::BINARY, true);
	if(uFlie)
	{
		Writer.Write( uFlie, &Data.Ambient, sizeof(ARGBCOLOR) );
		Writer.Write( uFlie, &Data.Diffuse, sizeof(RGBCOLOR) );
		Writer.Write( uFlie, &Data.Specular, sizeof(RGBCOLOR) );
		Writer.Write( uFlie, &Data.Emissive, sizeof(RGBCOLOR) );
		Writer.Write( uFlie, &Data.fPower, sizeof(FLOAT) );
		Writer.Write( uFlie, &Data.uTextureCount, sizeof(UINT) );

		const UINT uZERO = 0;
		CMap<CMeshManager::HANDLE,UINT>& IndexMap = m_IndexMap[MESH_DATA];
		STRING TexturePath;
		CMeshManager::HANDLE TextureDataHandle;
		const UINT* puIndex;
		const CMeshManager::TEXTUREDATA* pTextureData;
		for(UINT i = 0; i < Data.uTextureCount; ++ i)
		{
			TextureDataHandle = Data.pTextureData[i];
			puIndex = IndexMap.Get(TextureDataHandle);
			if(puIndex)
				Writer.Write( uFlie, puIndex, sizeof(UINT) );
			else
			{
				pTextureData = ALCHEMY_MESH_MANAGER.GetTextureData(TextureDataHandle);
				if(pTextureData)
				{
					TexturePath = pcTexturePath;

					TexturePath += uIndex;

					WriteTextureData(*pTextureData, Writer, TexturePath);

					++ uIndex;

					m_IndexMap[SCENE_NODE_DATA].Insert(TextureDataHandle, uIndex);

					Writer.Write( uFlie, &uIndex, sizeof(UINT) );
				}

				Writer.Write( uFlie, &uZERO, sizeof(UINT) );
			}
		}

		if(puOutputIndex)
			*puOutputIndex = uIndex;

		return Writer.Close(uFlie);
	}

	return false;
}

bool WriteSkinData(const CMeshManager::SKINDATA& Data, IFileWriter& Writer, const CHAR* pcPath)
{
	UINT uFlie = Writer.Open(pcPath, IFileWriter::BINARY, true);
	if(uFlie)
	{
		Writer.Write( uFlie, &Data.uBoneDataCount, sizeof(UINT) );

		const CSkinInfo::BONE* pBone;
		UINT uNameLength;
		for(UINT i = 0; i < Data.uBoneDataCount; ++ i)
		{
			pBone = &Data.pBoneData[i];
			Writer.Write( uFlie, &( uNameLength = ALCHEMY_STRING_LENGTH(pBone->pcName) ), sizeof(UINT) );
			Writer.Write( uFlie, pBone->pcName, sizeof(CHAR) * (uNameLength + 1) );
			Writer.Write( uFlie, &pBone->uInfluenceCount, sizeof(UINT) );
			Writer.Write(uFlie, pBone->pfWeights, sizeof(FLOAT) * pBone->uInfluenceCount);
			Writer.Write(uFlie, pBone->puIndices, sizeof(UINT) * pBone->uInfluenceCount);
			Writer.Write( uFlie, &pBone->OffsetMatrix, sizeof(MATRIX4X4) );
		}

		return Writer.Close(uFlie);
	}

	return false;
}

bool CSceneDataFiler::WriteSceneNodeData(const CMeshManager::SCENENODEDATA& Data, IFileWriter& Writer, const CHAR* pcPath, UINT uIndex, const CHAR* pcDataPath[TOTAL_DATA_FORMAT], PUINT puOutputIndex)
{
	UINT uFlie = Writer.Open(pcPath, IFileWriter::BINARY, true);
	if(uFlie)
	{
		const UINT uZERO = 0;

		const UINT* puIndex;

		CMap<CMeshManager::HANDLE, UINT>& SceneNodeIndexMap = m_IndexMap[SCENE_NODE_DATA];
		puIndex = SceneNodeIndexMap.Get(Data.Parent);
		if(puIndex)
			Writer.Write( uFlie, puIndex, sizeof(UINT) );
		else
		{
			const CMeshManager::SCENENODEDATA* pSceneNodeData = ALCHEMY_MESH_MANAGER.GetSceneNodeData(Data.Parent);
			if(pSceneNodeData)
			{
				STRING ParentPath = pcDataPath ? pcDataPath[SCENE_NODE_DATA] : ALCHEMY_NULL;

				ParentPath += uIndex ++;

				WriteSceneNodeData(*pSceneNodeData, Writer, ParentPath, uIndex, pcDataPath, &uIndex);

				SceneNodeIndexMap.Insert(Data.Parent, uIndex);

				Writer.Write( uFlie, &uIndex, sizeof(UINT) );
			}

			Writer.Write( uFlie, &uZERO, sizeof(UINT) );
		}

		UINT uNameLength = ALCHEMY_STRING_LENGTH(Data.pcName);
		Writer.Write( uFlie, &uNameLength, sizeof(UINT) );
		Writer.Write( uFlie, Data.pcName, sizeof(CHAR) * (uNameLength + 1) );
		Writer.Write( uFlie, &Data.Type, sizeof(CMeshManager::SCENETYPE) );
		Writer.Write( uFlie, &Data.Transform, sizeof(MATRIX4X4) );
		
		CMap<CMeshManager::HANDLE, UINT>& MeshIndexMap = m_IndexMap[MESH_DATA];
		puIndex = MeshIndexMap.Get(Data.MeshData);
		if(puIndex)
			Writer.Write( uFlie, puIndex, sizeof(UINT) );
		else
		{
			const CMeshManager::MESHDATA* pMeshData = ALCHEMY_MESH_MANAGER.GetMeshData(Data.MeshData);
			if(pMeshData)
			{
				STRING MeshPath = pcDataPath ? pcDataPath[MESH_DATA] : ALCHEMY_NULL;

				MeshPath += uIndex ++;

				WriteMeshData(*pMeshData, Writer, MeshPath);

				MeshIndexMap.Insert(Data.MeshData, uIndex);
			}

			Writer.Write( uFlie, &uIndex, sizeof(UINT) );
		}

		Writer.Write( uFlie, &Data.uSubset, sizeof(UINT) );

		CMap<CMeshManager::HANDLE, UINT>& MaterialIndexMap = m_IndexMap[MATERIAL_DATA];
		puIndex = MaterialIndexMap.Get(Data.MaterialData);
		if(puIndex)
			Writer.Write( uFlie, puIndex, sizeof(UINT) );
		else
		{
			const CMeshManager::MATERIALDATA* pMaterialData = ALCHEMY_MESH_MANAGER.GetMaterialData(Data.MaterialData);
			if(pMaterialData)
			{
				STRING MaterialPath = pcDataPath ? pcDataPath[MATERIAL_DATA] : ALCHEMY_NULL;

				MaterialPath += uIndex ++;

				WriteMaterialData(*pMaterialData, Writer, MaterialPath, uIndex, pcDataPath ? pcDataPath[TEXTURE_DATA] : ALCHEMY_NULL, &uIndex);

				MaterialIndexMap.Insert(Data.MaterialData, uIndex);
			}

			Writer.Write( uFlie, &uIndex, sizeof(UINT) );
		}

		CMap<CMeshManager::HANDLE, UINT>& SkinIndexMap = m_IndexMap[SKIN_DATA];
		puIndex = SkinIndexMap.Get(Data.SkinData);
		if(puIndex)
			Writer.Write( uFlie, puIndex, sizeof(UINT) );
		else
		{
			const CMeshManager::SKINDATA* pSkinData = ALCHEMY_MESH_MANAGER.GetSkinData(Data.SkinData);
			if(pSkinData)
			{
				STRING SkinPath = pcDataPath ? pcDataPath[SKIN_DATA] : ALCHEMY_NULL;

				SkinPath += uIndex ++;

				WriteSkinData(*pSkinData, Writer, SkinPath);

				SkinIndexMap.Insert(Data.SkinData, uIndex);
			}

			Writer.Write( uFlie, &uIndex, sizeof(UINT) );
		}

		puIndex = SceneNodeIndexMap.Get(Data.RootBone);
		if(puIndex)
			Writer.Write( uFlie, puIndex, sizeof(UINT) );
		else
		{
			const CMeshManager::SCENENODEDATA* pSceneNodeData = ALCHEMY_MESH_MANAGER.GetSceneNodeData(Data.RootBone);
			if(pSceneNodeData)
			{
				STRING BonePath = pcDataPath ? pcDataPath[SCENE_NODE_DATA] : ALCHEMY_NULL;

				BonePath += uIndex ++;

				WriteSceneNodeData(*pSceneNodeData, Writer, BonePath, uIndex, pcDataPath, &uIndex);

				SceneNodeIndexMap.Insert(Data.RootBone, uIndex);
			}

			Writer.Write( uFlie, &uIndex, sizeof(UINT) );
		}

		if(puOutputIndex)
			*puOutputIndex = uIndex;

		return true;
	}

	return false;
}

bool CSceneDataFiler::WriteSceneData(const CMeshManager::SCENEDATA& Data, IFileWriter& Writer, const CHAR* pcPath, UINT uIndex, const CHAR* pcDataPath[TOTAL_DATA_FORMAT], PUINT puOutputIndex)
{
	UINT uFlie = Writer.Open(pcPath, IFileWriter::BINARY, true);
	if(uFlie)
	{
		Writer.Write( uFlie, &Data.uSceneDataCount, sizeof(UINT) );

		CMap<CMeshManager::HANDLE, UINT>& SceneNodeIndexMap = m_IndexMap[SCENE_NODE_DATA];
		CMeshManager::HANDLE SceneNodeHandle;
		const UINT* puIndex;
		STRING SceneNodePath;
		const CMeshManager::SCENENODEDATA* pSceneNodeData;
		for(UINT i = 0; i < Data.uSceneDataCount; ++ i)
		{
			SceneNodeHandle = Data.pSceneNodeDatas[i];
			puIndex = SceneNodeIndexMap.Get(SceneNodeHandle);
			if(puIndex)
				Writer.Write( uFlie, puIndex, sizeof(UINT) );
			else
			{
				pSceneNodeData = ALCHEMY_MESH_MANAGER.GetSceneNodeData(SceneNodeHandle);
				if(pSceneNodeData)
				{
					SceneNodePath = pcDataPath ? pcDataPath[SCENE_NODE_DATA] : ALCHEMY_NULL;

					SceneNodePath += uIndex ++;
					WriteSceneNodeData(*pSceneNodeData, Writer, SceneNodePath, uIndex, pcDataPath, &uIndex);

					SceneNodeIndexMap.Insert(SceneNodeHandle, uIndex);
				}

				Writer.Write( uFlie, &uIndex, sizeof(UINT) );
			}
		}

		if(puOutputIndex)
			*puOutputIndex = uIndex;

		return Writer.Close(uFlie);
	}

	return false;
}

CBuffer& CSceneDataFiler::ReadMeshData(IFileReader& Reader)
{
	CMeshManager::MESHDATA Data;
	UINT uSize = sizeof(CMeshManager::MESHDATA);
	Reader.Read( &Data.uNumVertices, sizeof(UINT) );
	Reader.Read( &Data.uNumIndices, sizeof(UINT) );
	Reader.Read( &Data.uIndexUsage, sizeof(UINT) );
	Reader.Read( &Data.IndexFormat, sizeof(CIndexBuffer::FORMAT) );
	Reader.Read( &Data.VertexElementData.uElementCount, sizeof(UINT) );

	ALCHEMY_DEBUG_NEW(Data.VertexElementData.pVertexElement, CVertexDeclaration::VERTEXELEMENT[Data.VertexElementData.uElementCount]);

	UINT uVertexElementOffset = uSize, uVertexElementSize = Data.VertexElementData.uElementCount * sizeof(CVertexDeclaration::VERTEXELEMENT);
	uSize += uVertexElementSize;

	Reader.Read( &Data.VertexElementData.pVertexElement, uVertexElementSize);

	UINT uStreamCount = CVertexDeclaration::GetVertexElementStreamCount(Data.VertexElementData.pVertexElement, Data.VertexElementData.uElementCount);

	ALCHEMY_DEBUG_NEW(Data.VertexElementData.puUsage, UINT[uStreamCount]);

	UINT uStreamUsageOffset = uSize, uStreamUsageSize = uStreamCount * sizeof(UINT);
	uSize += uStreamUsageSize;

	Reader.Read(Data.VertexElementData.puUsage, uStreamUsageSize);

	Reader.Read( &Data.uAttributeRangeCount, sizeof(UINT) );

	ALCHEMY_DEBUG_NEW(Data.pAttributeRange, CMesh::ATTRIBUTERANGE[Data.uAttributeRangeCount]);

	UINT uAttributeRangeOffset = uSize, uAttributeRangeSize = Data.uAttributeRangeCount * sizeof(CMesh::ATTRIBUTERANGE);
	uSize += uAttributeRangeSize;

	Reader.Read(Data.pAttributeRange, uAttributeRangeSize);

	PUINT puVertexDataOffset, puVertexDataSize;
	ALCHEMY_DEBUG_NEW(puVertexDataOffset, UINT[uStreamCount]);
	ALCHEMY_DEBUG_NEW(puVertexDataSize, UINT[uStreamCount]);
	UINT i;
	for(i = 0; i < uStreamCount; ++ i)
	{
		puVertexDataOffset[i] = 0;
		puVertexDataSize[i] = 0;
	}

	UINT uStream = 0, uMaxStream = CVertexDeclaration::GetVertexElementMaxinumStream(Data.VertexElementData.pVertexElement, Data.VertexElementData.uElementCount), uStride;
	PVOID pVertexData;
	for(i = 0; i < uStreamCount; ++ i)
	{
		while( ( uStride = CVertexDeclaration::GetVertexElementStreamStrideOf(uStream, Data.VertexElementData.pVertexElement, Data.VertexElementData.uElementCount) ) && (uStream ++) < uMaxStream );

		if(uStride)
		{
			puVertexDataOffset[i] = uSize;
			puVertexDataSize[i] = uStride *= Data.uNumVertices;
			uSize += uStride;

			ALCHEMY_DEBUG_NEW(pVertexData, UINT8[uStride]);
			Reader.Read(pVertexData, uStride);

			Data.ppVertexData[i] = pVertexData;
		}
		else
			Data.ppVertexData[i] = ALCHEMY_NULL;
	}

	UINT uIndexDataOffset = uSize, uIndexDataSize = (Data.IndexFormat == CIndexBuffer::BIT32 ? 4 : 2) * Data.uNumIndices;
	uSize += uIndexDataSize;

	ALCHEMY_DEBUG_NEW(Data.pIndexData, UINT8[uIndexDataSize]);
	Reader.Read(Data.pIndexData, uIndexDataSize);

	CBuffer& Buffer = CreateBuffer(uSize);
	PUINT8 puBuffer = reinterpret_cast<PUINT8>( Buffer.GetBufferPointer() );
	CMeshManager::LPMESHDATA pMeshData = reinterpret_cast<CMeshManager::LPMESHDATA>(puBuffer);
	Buffer.CopyFrom( &Data, 0, sizeof(CMeshManager::MESHDATA) );

	pMeshData->VertexElementData.pVertexElement = reinterpret_cast<CVertexDeclaration::LPVERTEXELEMENT>(puBuffer + uVertexElementOffset);
	Buffer.CopyFrom(Data.VertexElementData.pVertexElement, uVertexElementOffset, uVertexElementSize);

	pMeshData->pAttributeRange = reinterpret_cast<CMesh::LPATTRIBUTERANGE>(puBuffer + uAttributeRangeOffset);
	Buffer.CopyFrom(Data.pAttributeRange, uAttributeRangeOffset, uAttributeRangeSize);
	
	UINT uOffset;
	for(i = 0; i < uStreamCount; ++ i)
	{
		uSize = puVertexDataSize[i];
		uOffset = puVertexDataOffset[i];
		if(uSize && uOffset)
		{
			pMeshData->ppVertexData[i] = reinterpret_cast<PVOID>(puBuffer + uOffset);
			Buffer.CopyFrom(Data.ppVertexData[i], uOffset, uSize);
		}
		else
			pMeshData->ppVertexData[i] = ALCHEMY_NULL;
	}

	ALCHEMY_DEBUG_DELETE_ARRAY(puVertexDataOffset);
	ALCHEMY_DEBUG_DELETE_ARRAY(puVertexDataSize);

	pMeshData->pIndexData = reinterpret_cast<PVOID>(puBuffer + uIndexDataOffset);

	Buffer.CopyFrom(Data.pIndexData, uIndexDataOffset, uIndexDataSize);

	CMeshManager::DestroyMeshData(Data);

	return Buffer;
}

CBuffer& CSceneDataFiler::ReadTextureData(IFileReader& Reader)
{
	CMeshManager::TEXTUREDATA Data;
	Reader.Read( &Data.uWidth, sizeof(UINT) );
	Reader.Read( &Data.uHeight, sizeof(UINT) );
	Reader.Read( &Data.Format, sizeof(CTexture::FORMAT) );
	Reader.Read( &Data.uUsage, sizeof(UINT) );
	Reader.Read( &Data.uMipLevel, sizeof(UINT) );

	UINT uNameLength = 0;
	Reader.Read( &uNameLength, sizeof(UINT) );

	UINT uSize = uNameLength ? ( sizeof(CHAR) * (uNameLength + 1) ) : ( Data.uWidth * Data.uHeight * CTexture::GetFormatSize(Data.Format) );
	CBuffer& Buffer = CreateBuffer(sizeof(CMeshManager::TEXTUREDATA) + uSize);
	PUINT8 puBuffer = reinterpret_cast<PUINT8>( Buffer.GetBufferPointer() );
	CMeshManager::LPTEXTUREDATA pTextureData = reinterpret_cast<CMeshManager::LPTEXTUREDATA>(puBuffer);
	Buffer.CopyFrom( &Data, 0, sizeof(CMeshManager::TEXTUREDATA) );
	if(uNameLength)
	{
		pTextureData->pData = ALCHEMY_NULL;
		pTextureData->pcFileName = reinterpret_cast<PCHAR>( puBuffer + sizeof(CMeshManager::TEXTUREDATA) );

		Reader.Read(pTextureData->pcFileName, uSize);
	}
	else
	{
		pTextureData->pData = reinterpret_cast<PVOID>( puBuffer + sizeof(CMeshManager::TEXTUREDATA) );
		pTextureData->pcFileName = ALCHEMY_NULL;

		Reader.Read(pTextureData->pData, uSize);
	}

	return Buffer;
}

CBuffer& CSceneDataFiler::ReadMaterialData(IFileReader& Reader)
{
	CMeshManager::MATERIALDATA Data;
	Reader.Read( &Data.Ambient, sizeof(ARGBCOLOR) );
	Reader.Read( &Data.Diffuse, sizeof(RGBCOLOR) );
	Reader.Read( &Data.Specular, sizeof(RGBCOLOR) );
	Reader.Read( &Data.Emissive, sizeof(RGBCOLOR) );
	Reader.Read( &Data.fPower, sizeof(FLOAT) );
	Reader.Read( &Data.uTextureCount, sizeof(UINT) );

	CBuffer& Buffer = CreateBuffer(sizeof(CMeshManager::MATERIALDATA) + sizeof(CMeshManager::HANDLE) * Data.uTextureCount);
	Buffer.CopyFrom( &Data, 0, sizeof(CMeshManager::MATERIALDATA) );
	CMeshManager::LPMATERIALDATA pMaterialData = reinterpret_cast<CMeshManager::LPMATERIALDATA>( Buffer.GetBufferPointer() );
	pMaterialData->pTextureData = reinterpret_cast<CMeshManager::HANDLE*>(pMaterialData + 1);
	UINT uIndex;
	for(UINT i = 0; i < Data.uTextureCount; ++ i)
	{
		Reader.Read( &uIndex, sizeof(UINT) );

		pMaterialData->pTextureData[i] = uIndex;
	}

	return Buffer;
}

CBuffer& CSceneDataFiler::ReadSkinData(IFileReader& Reader)
{
	CMeshManager::SKINDATA Data;
	Reader.Read( &Data.uBoneDataCount, sizeof(UINT) );

	CSkinInfo::BONE* pBone;
	UINT i, uNameLength, uCurrentSize, uSize = sizeof(CMeshManager::SKINDATA);
	ALCHEMY_DEBUG_NEW(Data.pBoneData, CSkinInfo::BONE[Data.uBoneDataCount]);
	for(i = 0; i < Data.uBoneDataCount; ++ i)
	{
		uSize += sizeof(CSkinInfo::BONE);

		pBone = &Data.pBoneData[i];
		Reader.Read( &uNameLength, sizeof(UINT) );

		++ uNameLength;

		ALCHEMY_DEBUG_NEW(pBone->pcName, CHAR[uNameLength]);
		uCurrentSize = uNameLength * sizeof(CHAR);
		Reader.Read(pBone->pcName, uCurrentSize);
		uSize += uCurrentSize;

		Reader.Read( &pBone->uInfluenceCount, sizeof(UINT) );

		ALCHEMY_DEBUG_NEW(pBone->pfWeights, FLOAT[pBone->uInfluenceCount]);
		uCurrentSize = pBone->uInfluenceCount * sizeof(FLOAT);
		Reader.Read(pBone->pfWeights, sizeof(FLOAT) * pBone->uInfluenceCount);
		uSize += uCurrentSize;

		ALCHEMY_DEBUG_NEW(pBone->puIndices, UINT[pBone->uInfluenceCount]);
		uCurrentSize = pBone->uInfluenceCount * sizeof(UINT);
		Reader.Read(pBone->puIndices, sizeof(UINT) * pBone->uInfluenceCount);
		uSize += uCurrentSize;

		Reader.Read( &pBone->OffsetMatrix, sizeof(MATRIX4X4) );
	}

	CBuffer& Buffer = CreateBuffer(uSize);
	Buffer.CopyFrom( &Data, 0, sizeof(CMeshManager::SKINDATA) );
	PUINT8 puBuffer = reinterpret_cast<PUINT8>( Buffer.GetBufferPointer() );
	CMeshManager::LPSKINDATA pSkinData = reinterpret_cast<CMeshManager::LPSKINDATA>(puBuffer);
	const CSkinInfo::BONE* pSourceBone;
	UINT uOffset = sizeof(CMeshManager::SKINDATA) + sizeof(CSkinInfo::BONE) * Data.uBoneDataCount;

	pSkinData->pBoneData = reinterpret_cast<CSkinInfo::LPBONE>( puBuffer + sizeof(CMeshManager::SKINDATA) );
	for(i = 0; i < Data.uBoneDataCount; ++ i)
	{
		pBone = pSkinData->pBoneData;
		pSourceBone = &Data.pBoneData[i];

		uCurrentSize = (ALCHEMY_STRING_LENGTH(pSourceBone->pcName) + 1) * sizeof(CHAR);
		pBone->pcName = reinterpret_cast<CHAR*>(puBuffer + uOffset);
		uOffset += uCurrentSize;
		ALCHEMY_MEMORY_COPY(pBone->pcName, pSourceBone->pcName, uCurrentSize);

		pBone->uInfluenceCount = pSourceBone->uInfluenceCount;

		uCurrentSize = pSourceBone->uInfluenceCount * sizeof(FLOAT);
		pBone->pfWeights = reinterpret_cast<PFLOAT>(puBuffer + uOffset);
		uOffset += uCurrentSize;
		ALCHEMY_MEMORY_COPY(pBone->pfWeights, pSourceBone->pfWeights, uCurrentSize);

		uCurrentSize = pSourceBone->uInfluenceCount * sizeof(UINT);
		pBone->puIndices = reinterpret_cast<PUINT>(puBuffer + uOffset);
		uOffset += uCurrentSize;
		ALCHEMY_MEMORY_COPY(pBone->puIndices, pSourceBone->puIndices, uCurrentSize);

		pBone->OffsetMatrix = pSourceBone->OffsetMatrix;
	}

	CMeshManager::DestroySkinData(Data);

	return Buffer;
}

CBuffer& CSceneDataFiler::ReadSceneNodeData(IFileReader& Reader)
{
	CMeshManager::SCENENODEDATA Data;
	UINT uIndex;

	Reader.Read( &uIndex, sizeof(UINT) );
	Data.Parent = uIndex;

	UINT uNameLength;
	Reader.Read( &uNameLength, sizeof(UINT) );

	ALCHEMY_DEBUG_NEW(Data.pcName, CHAR[uNameLength += 1]);

	UINT uSize = sizeof(CHAR) * uNameLength;
	Reader.Read(Data.pcName, uSize);

	Reader.Read( &Data.Type, sizeof(CMeshManager::SCENETYPE) );
	Reader.Read( &Data.Transform, sizeof(MATRIX4X4) );

	Reader.Read( &uIndex, sizeof(UINT) );
	Data.MeshData = uIndex;

	Reader.Read( &Data.uSubset, sizeof(UINT) );

	Reader.Read( &uIndex, sizeof(UINT) );
	Data.MaterialData = uIndex;

	Reader.Read( &uIndex, sizeof(UINT) );
	Data.SkinData = uIndex;

	Reader.Read( &uIndex, sizeof(UINT) );
	Data.RootBone = uIndex;

	CBuffer& Buffer = CreateBuffer(sizeof(CMeshManager::SCENENODEDATA) + uSize);
	Buffer.CopyFrom( &Data, 0, sizeof(CMeshManager::SCENENODEDATA) );
	CMeshManager::LPSCENENODEDATA pSceneNodeData = reinterpret_cast<CMeshManager::LPSCENENODEDATA>( Buffer.GetBufferPointer() );
	pSceneNodeData->pcName = reinterpret_cast<PCHAR>(pSceneNodeData + 1);
	ALCHEMY_MEMORY_COPY(pSceneNodeData->pcName, Data.pcName, uSize);

	CMeshManager::DestroySceneNodeData(Data);

	return Buffer;
}

CBuffer& CSceneDataFiler::ReadSceneData(IFileReader& Reader)
{
	UINT uSceneNodeDataCount;
	Reader.Read( &uSceneNodeDataCount, sizeof(UINT) );

	CBuffer& Buffer = CreateBuffer( sizeof(CMeshManager::SCENEDATA) + uSceneNodeDataCount * sizeof(CMeshManager::HANDLE) );
	CMeshManager::LPSCENEDATA pSceneData = reinterpret_cast<CMeshManager::LPSCENEDATA>( Buffer.GetBufferPointer() );
	pSceneData->uSceneDataCount = uSceneNodeDataCount;
	pSceneData->pSceneNodeDatas = reinterpret_cast<CMeshManager::HANDLE*>(pSceneData + 1);

	UINT uIndex;
	for(UINT i =0; i < uSceneNodeDataCount; ++ i)
	{
		Reader.Read( &uIndex, sizeof(UINT) );

		pSceneData->pSceneNodeDatas[i] = uIndex;
	}

	return Buffer;
}

namespace alchemy
{
	class CSceneNodeDataListener : 
		public  CSmartClass<IEventDispatcher::IListener>
	{
		friend class CSceneDataListener;
	public:
		CSceneNodeDataListener(CSceneDataFiler& Parent, IFileReader& FileReader, const STRING DataPath[CSceneDataFiler::TOTAL_DATA_FORMAT]) :
		  m_Parent(Parent), 
			  m_FileReader(FileReader)
		{
			for(UINT i = 0; i < CSceneDataFiler::TOTAL_DATA_FORMAT; ++ i)
				m_DataPath[i] = DataPath[i];
		}

		~CSceneNodeDataListener()
		{

		}

		void Handler(const IEventDispatcher::CEvent& Event)
		{
			const CLoader::CLoadAllEvent& LoadAllEvent = static_cast<const CLoader::CLoadAllEvent&>(Event);
			UINT uDataCount = LoadAllEvent.GetDataCount();
			const UINT* puDataIndices = LoadAllEvent.GetDataIndices();

			UINT uCurrentCount = 0;
			CVector<CMeshManager::HANDLE> Handles[CSceneDataFiler::TOTAL_DATA_FORMAT];
			CVector<CMeshManager::HANDLE>::CIterator Iterator;
			CBuffer* pBuffer;
			CBuffer** ppBuffer;

			Iterator = m_Handles[CSceneDataFiler::MESH_DATA].GetIterator(0);
			while( CVector<CMeshManager::HANDLE>::IsValuable(Iterator) )
			{
				if( m_FileReader.Open( ALCHEMY_LOADER.GetData(puDataIndices[uCurrentCount ++]) ) )
				{
					pBuffer = &m_Parent.ReadMeshData(m_FileReader);
					m_FileReader.Close();

					m_Parent.m_DataMap.Insert(*Iterator, pBuffer);
				}

				++ Iterator;
			}

			Iterator = m_Handles[CSceneDataFiler::TEXTURE_DATA].GetIterator(0);
			while( CVector<CMeshManager::HANDLE>::IsValuable(Iterator) )
			{
				if( m_FileReader.Open( ALCHEMY_LOADER.GetData(puDataIndices[uCurrentCount ++]) ) )
				{
					pBuffer = &m_Parent.ReadTextureData(m_FileReader);
					m_FileReader.Close();

					m_Parent.m_DataMap.Insert(*Iterator, pBuffer);
				}

				++ Iterator;
			}

			UINT i;
			const CMeshManager::MATERIALDATA* pMaterialData;
			CMeshManager::HANDLE Handle;

			Iterator = m_Handles[CSceneDataFiler::MATERIAL_DATA].GetIterator(0);
			while( CVector<CMeshManager::HANDLE>::IsValuable(Iterator) )
			{
				if( m_FileReader.Open( ALCHEMY_LOADER.GetData(puDataIndices[uCurrentCount ++]) ) )
				{
					pBuffer = &m_Parent.ReadMaterialData(m_FileReader);
					m_FileReader.Close();

					m_Parent.m_DataMap.Insert(*Iterator, pBuffer);

					pMaterialData = reinterpret_cast<const CMeshManager::MATERIALDATA*>( pBuffer->GetBufferPointer() );
					for(i = 0 ;i < pMaterialData->uTextureCount; ++ i)
					{
						Handle = pMaterialData->pTextureData[i];
						if(Handle)
						{
							ppBuffer = m_Parent.m_DataMap.Get(Handle);
							if(!ppBuffer)
								Handles[CSceneDataFiler::SCENE_NODE_DATA].Add(Handle);
						}
					}
				}

				++ Iterator;
			}

			Iterator = m_Handles[CSceneDataFiler::SKIN_DATA].GetIterator(0);
			while( CVector<CMeshManager::HANDLE>::IsValuable(Iterator) )
			{
				if( m_FileReader.Open( ALCHEMY_LOADER.GetData(puDataIndices[uCurrentCount ++]) ) )
				{
					pBuffer = &m_Parent.ReadSkinData(m_FileReader);
					m_FileReader.Close();

					m_Parent.m_DataMap.Insert(*Iterator, pBuffer);
				}

				++ Iterator;
			}

			const CMeshManager::SCENENODEDATA* pSceneNodeData;

			Iterator = m_Handles[CSceneDataFiler::SCENE_NODE_DATA].GetIterator(0);
			while( CVector<CMeshManager::HANDLE>::IsValuable(Iterator) )
			{
				if( m_FileReader.Open( ALCHEMY_LOADER.GetData(puDataIndices[uCurrentCount ++]) ) )
				{
					pBuffer = &m_Parent.ReadSceneNodeData(m_FileReader);
					m_FileReader.Close();

					m_Parent.m_DataMap.Insert(*Iterator, pBuffer);

					pSceneNodeData = reinterpret_cast<const CMeshManager::SCENENODEDATA*>( pBuffer->GetBufferPointer() );

					if(pSceneNodeData->Parent)
					{
						ppBuffer = m_Parent.m_DataMap.Get(pSceneNodeData->Parent);
						if(!ppBuffer)
							Handles[CSceneDataFiler::SCENE_NODE_DATA].Add(pSceneNodeData->Parent);
					}

					if(pSceneNodeData->RootBone)
					{
						ppBuffer = m_Parent.m_DataMap.Get(pSceneNodeData->RootBone);
						if(!ppBuffer)
							Handles[CSceneDataFiler::SCENE_NODE_DATA].Add(pSceneNodeData->RootBone);
					}

					if(pSceneNodeData->MeshData)
					{
						ppBuffer = m_Parent.m_DataMap.Get(pSceneNodeData->MeshData);
						if(!ppBuffer)
							m_Handles[CSceneDataFiler::MESH_DATA].Add(pSceneNodeData->MeshData);
					}

					if(pSceneNodeData->MaterialData)
					{
						ppBuffer = m_Parent.m_DataMap.Get(pSceneNodeData->MaterialData);
						if(!ppBuffer)
							m_Handles[CSceneDataFiler::MATERIAL_DATA].Add(pSceneNodeData->MaterialData);
					}

					if(pSceneNodeData->SkinData)
					{
						ppBuffer = m_Parent.m_DataMap.Get(pSceneNodeData->SkinData);
						if(!ppBuffer)
							m_Handles[CSceneDataFiler::SKIN_DATA].Add(pSceneNodeData->SkinData);
					}
				}
			}

			UINT uType;
			if( ALCHEMY_GET_EVENT_TYPE(uType, LOAD_ALL_COMPLETE, CLoader) )
			{
				UINT uPathCount = 0;
				for(i = 0; i < CSceneDataFiler::TOTAL_DATA_FORMAT; ++ i)
					uPathCount += Handles[i].GetLength();

				if(uPathCount)
				{
					STRING* pPath;
					ALCHEMY_DEBUG_NEW(pPath, STRING[uPathCount]);

					const CHAR** ppcPath;
					ALCHEMY_DEBUG_NEW(ppcPath, const CHAR*[uPathCount]);

					uPathCount = 0;

					STRING* pTemp;
					STRING* pCurrentPath;
					CVector<CMeshManager::HANDLE>::CIterator Iterator;
					for(i = 0; i < CSceneDataFiler::TOTAL_DATA_FORMAT; ++ i)
					{
						pCurrentPath = &m_DataPath[i];
						Iterator = Handles[i].GetIterator(0);
						while( CVector<CMeshManager::HANDLE>::IsValuable(Iterator) )
						{
							pTemp   = &pPath[uPathCount];
							*pTemp  = *pCurrentPath;
							*pTemp += *Iterator;

							ppcPath[uPathCount ++] = *pTemp;

							++ Iterator;
						}
					}

					for(i = 0; i < CSceneDataFiler::TOTAL_DATA_FORMAT; ++ i)
						m_Handles[i] = Handles[i];

					ALCHEMY_LOADER.LoadAll(ppcPath, uPathCount);

					ALCHEMY_DEBUG_DELETE_ARRAY(pPath);
					ALCHEMY_DEBUG_DELETE_ARRAY(ppcPath);
				}
				else
				{
					CSceneDataFiler& Parent = m_Parent;
					ALCHEMY_LOADER.RemoveEventListener(uType, *this);

					//Parent.DispatchEvent();
				}
			}
		}

	private:
		CSceneDataFiler& m_Parent;
		IFileReader& m_FileReader;
		STRING m_DataPath[CSceneDataFiler::TOTAL_DATA_FORMAT];
		CVector<CMeshManager::HANDLE> m_Handles[CSceneDataFiler::TOTAL_DATA_FORMAT];
	};

	class CSceneDataListener : 
		public CSmartClass<IEventDispatcher::IListener>
	{
	public:
		CSceneDataListener(CSceneDataFiler& Parent, IFileReader& FileReader, const CHAR* pcDataPath[CSceneDataFiler::TOTAL_DATA_FORMAT]) :
		  m_Parent(Parent), 
			  m_FileReader(FileReader)
		  {
			  if(pcDataPath)
			  {
				  for(UINT i = 0; i < CSceneDataFiler::TOTAL_DATA_FORMAT; ++ i)
					  m_DataPath[i] = pcDataPath[i];
			  }
		  }

		~CSceneDataListener()
		{

		}

		void Handler(const IEventDispatcher::CEvent& Event)
		{
			const CLoader::CLoadEvent& LoadEvent = static_cast<const CLoader::CLoadEvent&>(Event);
			if( m_FileReader.Open( ALCHEMY_LOADER.GetData( LoadEvent.GetIndex() ) ) )
			{
				CBuffer& Buffer = m_Parent.ReadSceneData(m_FileReader);
				m_FileReader.Close();

				const CMeshManager::SCENEDATA* pSceneData = reinterpret_cast<const CMeshManager::SCENEDATA*>( Buffer.GetBufferPointer() );
				STRING* pPath, * pTemp;
				ALCHEMY_DEBUG_NEW(pPath, STRING[pSceneData->uSceneDataCount]);

				const CHAR** ppcPath;
				ALCHEMY_DEBUG_NEW(ppcPath, const CHAR*[pSceneData->uSceneDataCount]);

				CVector<CMeshManager::HANDLE> SceneNodeDataHandles;
				UINT uSceneDataCount = 0;
				CMeshManager::HANDLE Handle;
				CBuffer** ppBuffer;
				for(UINT i = 0; i < pSceneData->uSceneDataCount; ++ i)
				{
					Handle = pSceneData->pSceneNodeDatas[i];
					if(Handle)
					{
						ppBuffer = m_Parent.m_DataMap.Get(Handle);
						if(!ppBuffer)
						{
							SceneNodeDataHandles.Add(Handle);

							pTemp   = &pPath[uSceneDataCount];
							*pTemp  = m_DataPath[CSceneDataFiler::SCENE_NODE_DATA];
							*pTemp += Handle;

							ppcPath[uSceneDataCount ++] = *pTemp;
						}
					}
				}

				UINT uType;
				if( ALCHEMY_GET_EVENT_TYPE(uType, LOAD_ALL_COMPLETE, CLoader) )
				{
					CSceneNodeDataListener* pSceneNodeDataListener;
					ALCHEMY_DEBUG_NEW( pSceneNodeDataListener, CSceneNodeDataListener(m_Parent, m_FileReader, m_DataPath) );

					pSceneNodeDataListener->m_Handles[CSceneDataFiler::SCENE_NODE_DATA] = SceneNodeDataHandles;

					ALCHEMY_LOADER.AddEventListener(uType, *pSceneNodeDataListener);

					pSceneNodeDataListener->Release();

					ALCHEMY_LOADER.LoadAll(ppcPath, uSceneDataCount);
				}

				ALCHEMY_DEBUG_DELETE_ARRAY(pPath);
				ALCHEMY_DEBUG_DELETE_ARRAY(ppcPath);

				ALCHEMY_LOADER.RemoveEventListener(Event.GetType(), *this);
			}
		}

	private:
		CSceneDataFiler& m_Parent;
		IFileReader& m_FileReader;
		STRING m_DataPath[CSceneDataFiler::TOTAL_DATA_FORMAT];
	};
}

void CSceneDataFiler::LoadSceneData(IFileReader& FileReader, const CHAR* pcSceneDataPath, const CHAR* pcDataPath[TOTAL_DATA_FORMAT])
{
	UINT uType;
	if( ALCHEMY_GET_EVENT_TYPE(uType, LOAD_COMPLETE, CLoader) )
	{
		CSceneDataListener* pSceneDataListener;
		ALCHEMY_DEBUG_NEW( pSceneDataListener, CSceneDataListener(*this, FileReader, pcDataPath) );

		ALCHEMY_LOADER.AddEventListener(uType, *pSceneDataListener);

		pSceneDataListener->Release();

		ALCHEMY_LOADER.Load(pcSceneDataPath);
	}
}