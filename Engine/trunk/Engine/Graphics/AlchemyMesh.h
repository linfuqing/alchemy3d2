#pragma once

#include "AlchemyVertex.h"
//#include "AlchemyIndexBuffer.h"
#include "AlchemyEffectCode.h"

#include "../Math/AlchemyRay.h"
#include "../SceneManagement/AlchemyBoundingVolumeInterface.h"

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
		
		typedef struct
		{
			MATRIX4X4 Matrix;
			UINT uStream;
			UINT uStreamOffset;
			UINT uAttributeRangeIndex;
			bool bIsNormal;
		}VERTEXELEMENTTRANSFORM, * LPVERTEXELEMENTTRANSFORM;
		
		typedef struct
		{
			UINT uFaceIndex;
			FLOAT u;
			FLOAT v;
			FLOAT t;
		}INTERSECTINFO, * LPINTERSECTINFO;

		CMesh(void);
		~CMesh(void);

		CPool<ATTRIBUTERANGE>& GetAttributeTable();
		CVertex& GetVertex();
		CVertex& GetVertex()const;
		INDEXBUFFER&  GetIndexBuffer();

		CMesh* Clone(const CVertexDeclaration::VERTEXELEMENT* pCloneElements, UINT uCloneElementCount, const UINT* puVertexUsage = ALCHEMY_NULL, bool bIsCloneIndices = false, UINT IndexUsage = 0);
		CMesh* Clone(const                               UINT* puCloneStream, UINT uCloneStreamCount , const UINT* puVertexUsage = ALCHEMY_NULL, bool bIsCloneIndices = false, UINT IndexUsage = 0);
		CMesh* Clone(const ATTRIBUTERANGE* pAttributeRanges, UINT uAttributeRangeCount, const VERTEXELEMENTTRANSFORM* pTransform = ALCHEMY_NULL, UINT uTransformCount = 0);
		CMesh* Clone(FLOAT fInstanceCount, const MATRIX4X4* pTransform = ALCHEMY_NULL, const CEffectCode::ELEMENTDECLARATION* pTransformElements = ALCHEMY_NULL, UINT uTransformElementCount = 0);

		bool ComputeTangent( UINT uTexcoordIndex = 0 );

		bool ComputeNormal();

		bool ComputeBoundingVolume(IBoundingVolume& BoundingVolume, const ATTRIBUTERANGE* pAttributeRange = ALCHEMY_NULL);

		bool GenerateAdjacency(FLOAT Epsilon, UINT* pAdjacency);

		CMesh* ConvertToIndexedMesh();
		
		CMesh* ConvertToInstancedMesh(FLOAT fInstanceCount, 
									  const MATRIX4X4* pTransform = ALCHEMY_NULL, 
									  const CEffectCode::ELEMENTDECLARATION* pTransformElements = ALCHEMY_NULL,
									  UINT uTransformElementCount = 0,
									  const CVertex::INSTANCEDATA* pInstanceData = ALCHEMY_NULL, 
									  UINT uInstanceDataCount = 0);
		
		CMesh* ConvertToInstancedMesh(const ATTRIBUTERANGE* pAttributeRanges, 
									  UINT uAttributeRangeCount,
									  const VERTEXELEMENTTRANSFORM* pTransform = ALCHEMY_NULL, 
									  UINT uTransformCount = 0, 
									  const CVertex::INSTANCEDATA* pInstanceData = ALCHEMY_NULL, 
									  UINT uInstanceDataCount = 0);

		CMesh* ConvertToBlendedMesh(
			const CSkinInfo& SkinInfo, 
			UINT uMatrixIndexOffset, 
			UINT uMaxBoneCount = 0, 
			PUINT puOutputMaxVertexInfluenceCount = ALCHEMY_NULL, 
			CBuffer** ppBoneCombination = ALCHEMY_NULL,
			PUINT puBoneCombinationCount = ALCHEMY_NULL);
			
		bool Intersect(
			const RAY& Ray, 
			LPINTERSECTINFO pIntersectInfos, 
			PUINT puIntersectInfoCount, 
			bool bIsCheckHitOn = false,
			bool bHitOnMask = false,
			bool bIsCheckFront = false, 
			bool bFrontMask = false, 
			LPFLOAT3 pHitPoints = ALCHEMY_NULL, 
			LPFLOAT3 pfFaceNormals = ALCHEMY_NULL);

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
