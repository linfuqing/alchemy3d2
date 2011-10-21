#pragma once
#include "../core/alchemysingleton.h"
#include "AlchemyMesh.h"
#include "AlchemyTexture.h"
#include "../Math/AlchemyMatrix4x4.h"
#include "AlchemyARGBColor.h"
#include "AlchemyRGBColor.h"

namespace alchemy
{
#define ALCHEMY_MESH_MANAGER CMeshManager::GetInstance()

	class CMeshManager :
		public CSingleton<CMeshManager>
	{
	public:
		typedef enum
		{
			NODE,
			OBJECT,
			BONE
		}SCENETYPE;

		typedef struct
		{
			CVertexDeclaration::VERTEXELEMENT* pVertexElement;
			UINT uElementCount;

			PUINT puUsage;
		}VERTEXELEMENTDATA;

		typedef struct
		{
			UINT uNumVertices;
			UINT uNumIndices;
			UINT uIndexUsage;
			CIndexBuffer::FORMAT IndexFormat;
			VERTEXELEMENTDATA VertexElementData;

			CMesh::ATTRIBUTERANGE* pAttributeRange;
			UINT uAttributeRangeCount;

			PVOID* ppVertexData;
			PVOID  pIndexData;
		}MESHDATA, * LPMESHDATA;

		typedef struct
		{
			PVOID            pData;

			UINT             uWidth;
			UINT             uHeight;

			CTexture::FORMAT Format;
			UINT             uUsage;

			UINT             uMipLevel;
		}TEXTUREDATA, * LPTEXTUREDATA;

		typedef struct
		{
			ARGBCOLOR Ambient;

			RGBCOLOR Diffuse;
			RGBCOLOR Specular;
			RGBCOLOR Emissive;

			FLOAT fPower;

			TEXTUREDATA* pTextureData;

			UINT uTextureCount;
		}MATERIALDATA, * LPMATERIALDATA;

		typedef struct
		{
			UINT uParent;

			SCENETYPE Type;
			MATRIX4X4 Transform;

			UINT uSubset;

			MATERIALDATA MaterialData;
		}SCENENODEDATA, * LPSCENENODEDATA;

		typedef struct
		{
			SCENENODEDATA* pSceneNodeData;

			UINT uSceneDataCount;
		}SCENEDATA, * LPSCENEDATA;

		typedef struct  
		{
			UINT uMeshData;
			UINT uSceneData;
		}ENTITYDATA, * LPENTITYDATA;

		CMeshManager(void);
		~CMeshManager(void);

		const SCENEDATA* GetSceneData(UINT uHandle)const;
		const MESHDATA* GetMeshData(UINT uHandle)const;

		UINT CreateSkyDome(
			FLOAT fRadius,
			FLOAT fPhiDelta,
			FLOAT fThetaDelta,
			const CVertexDeclaration::LPVERTEXELEMENT pVertexElements,
			const UINT* pVertexUsage,
			UINT  uElementCount,
			UINT  uIndexUsage = 0);

		UINT CreatePlane(
			FLOAT fWidth, 
			FLOAT fHeight, 
			UINT uWidthSegment = 1, 
			UINT uHeightSegment = 1, 
			UINT uWidthStep = 1,
			UINT uHeightStep = 1,
			bool bIsYUp = false,
			const CVertexDeclaration::VERTEXELEMENT* pVertexElements = ALCHEMY_NULL,
			UINT uElementCount = 0,
			const FLOAT3* pPositionOffset = ALCHEMY_NULL,
			const UINT* puVertexUsage = ALCHEMY_NULL,
			UINT uIndexUsage = 0);

		bool DestroySceneData(UINT uHandle);
		bool DestroyMeshData(UINT uHandle);

		void Destroy();
	private:
		void __DestroySceneData(SCENEDATA& SceneData)const;
		void __DestroyMeshData(MESHDATA& MeshData)const;

		PVOID* __CreateVertexData(UINT uNumVertices, const CVertexDeclaration::VERTEXELEMENT* pVertexElements, UINT uElementCount, PUINT puStreamCount = ALCHEMY_NULL);
		void __DestroyVertexData(PVOID* ppData, UINT uStreamCount);

		CPool<MESHDATA> m_MeshPool;
		CPool<SCENEDATA> m_ScenePool;

		CPool<ENTITYDATA> m_EntityPool;
	};
}