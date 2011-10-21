#pragma once

#include "AlchemyVertex.h"
//#include "AlchemyIndexBuffer.h"
#include "../Core/AlchemyPool.h"

namespace alchemy
{
	class CSceneManager;
	class CMesh :
		public CHighLevelGraphicsResource
	{
		friend class CSceneManager;
	public:
		typedef CIndexBuffer::TRIANGLETYPE TRIANGLETYPE;
		typedef struct
		{
			UINT         uStartIndex;
			UINT         uIndexCount;
		}ATTRIBUTERANGE, * LPATTRIBUTERANGE;

		typedef struct
		{
			UINT uStartIndex;
			UINT uIndexCount;
			UINT uBoneCount;
			PUINT puBoneIndices;
		}BONECOMBINATION, * LPBONECOMBINATION;

		CMesh(void);
		~CMesh(void);

		CPool<ATTRIBUTERANGE>& GetAttributeTable();
		CVertex& GetVertex();
		CVertex& GetVertex()const;
		INDEXBUFFER&  GetIndexBuffer();

		CMesh* Clone(const CVertexDeclaration::VERTEXELEMENT* pCloneElements, UINT uCloneElementCount, const UINT* puVertexUsage = ALCHEMY_NULL, bool bIsCloneIndices = false, UINT IndexUsage = 0);
		CMesh* Clone(const                               UINT* puCloneStream, UINT uCloneStreamCount , const UINT* puVertexUsage = ALCHEMY_NULL, bool bIsCloneIndices = false, UINT IndexUsage = 0);

		bool GenerateAdjacency(FLOAT Epsilon, UINT* pAdjacency);

		CMesh* ConvertToIndexedMesh();

		CMesh* ConvertToBlendedMesh(
			const CSkinInfo& SkinInfo, 
			UINT uBlendWeightUsage, 
			UINT uBlendIndexUsage, 
			UINT uMatrixIndexOffset, 
			UINT uMaxBoneCount = 0, 
			PUINT puOutputMaxVertexInfluenceCount = ALCHEMY_NULL, 
			CBuffer** ppBoneCombination = ALCHEMY_NULL,
			PUINT puBoneCombinationCount = ALCHEMY_NULL);

		//bool Create(UINT uNumVertices, UINT uNumIndices, UINT uIndexUsage, CIndexBuffer::FORMAT IndexFormat, const CVertexDeclaration::VERTEXELEMENT* pVertexElement, UINT uElementCount);

		bool DrawSubset(const ATTRIBUTERANGE& AttributeRange, bool bIsActivate = true, bool bIsActivateVertexDeclaration = true);
		bool DrawSubset(UINT AttributeID                    , bool bIsActivate = true, bool bIsActivateVertexDeclaration = true);

		void Destroy();

	private:
		CPool<ATTRIBUTERANGE> m_AttributeTable;
		CVertex* m_pVertex;
		//CIndexBuffer*  m_pIndexBuffer;
		LPINDEXBUFFER m_pIndexBuffer;
	};

	ALCHEMY_INLINE CPool<CMesh::ATTRIBUTERANGE>& CMesh::GetAttributeTable()
	{
		return m_AttributeTable;
	}

	ALCHEMY_INLINE CVertex& CMesh::GetVertex()
	{
		return *m_pVertex;
	}

	ALCHEMY_INLINE CVertex& CMesh::GetVertex()const
	{
		return *m_pVertex;
	}

	ALCHEMY_INLINE INDEXBUFFER&  CMesh::GetIndexBuffer()
	{
		return *m_pIndexBuffer;
	}
}
