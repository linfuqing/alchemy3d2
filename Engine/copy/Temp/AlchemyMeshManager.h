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

	class CAnimationController;
	class CSkinInfo;
	class CEffect;
	class CMeshManager :
		public CSingleton<CMeshManager>
	{
	public:
		typedef UINT HANDLE;

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

			PCHAR            pcFileName;
		}TEXTUREDATA, * LPTEXTUREDATA;

		typedef struct
		{
			ARGBCOLOR Ambient;

			RGBCOLOR Diffuse;
			RGBCOLOR Specular;
			RGBCOLOR Emissive;

			FLOAT fPower;

			HANDLE* pTextureData;

			UINT uTextureCount;
		}MATERIALDATA, * LPMATERIALDATA;

		typedef struct  
		{
			CSkinInfo::BONE*		pBoneData;
			UINT					uBoneDataCount;
		}SKINDATA, * LPSKINDATA;

		typedef struct
		{
			HANDLE Parent;

			PCHAR	pcName;

			SCENETYPE Type;
			MATRIX4X4 Transform;

			HANDLE MeshData;
			UINT   uSubset;

			HANDLE	MaterialData;

			HANDLE	SkinData;

			HANDLE	RootBone;
		}SCENENODEDATA, * LPSCENENODEDATA;

		typedef struct
		{
			HANDLE* pSceneNodeDatas;

			UINT uSceneDataCount;
		}SCENEDATA, * LPSCENEDATA;

		CMeshManager(void);
		~CMeshManager(void);

		const MESHDATA* GetMeshData(HANDLE uHandle)const;

		const TEXTUREDATA* GetTextureData(HANDLE uHandle) const;

		const MATERIALDATA* GetMaterialData(HANDLE uHandle) const;

		const SKINDATA*	GetSkinData(HANDLE uHandle) const;

		const SCENENODEDATA* GetSceneNodeData(HANDLE uHandle)const;

		const SCENEDATA* GetSceneData(HANDLE uHandle)const;

		HANDLE AddMeshData(const MESHDATA& MeshData);

		HANDLE AddTextureData(const TEXTUREDATA& TextureData);

		HANDLE AddMaterialData(const MATERIALDATA& MaterialData);

		HANDLE AddSkinData(const SKINDATA& SkinData);

		HANDLE AddSceneNodeData(const SCENENODEDATA& SceneNodeData);

		HANDLE AddSceneData(const SCENEDATA& SceneData);

		HANDLE CreateBillboardMeshData( FLOAT fWidth, FLOAT fHeight, UINT uBillboardNum,
			const CVertexDeclaration::LPVERTEXELEMENT pVertexElements,
			const UINT* pVertexUsage,
			UINT  uElementCount,
			UINT  uIndexUsage = 0 );

		HANDLE CreateSkyDome(
			FLOAT fRadius,
			FLOAT fPhiDelta,
			FLOAT fThetaDelta,
			const CVertexDeclaration::LPVERTEXELEMENT pVertexElements,
			const UINT* pVertexUsage,
			UINT  uElementCount,
			UINT  uIndexUsage = 0);

		HANDLE CreatePlane(
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

		bool DestroyMeshData(HANDLE uHandle);
		
		bool DestroyTextureData(HANDLE uHandle);

		bool DestroyMaterialData(HANDLE uHandle);

		bool DestroySkinData(HANDLE uHandle);

		bool DestroySceneNodeData(HANDLE uHandle);

		bool DestroySceneData(HANDLE uHandle);

		void Destroy();
	private:
		void __DestroyMeshData(MESHDATA& MeshData)const;
		void __DestroyTextureData(TEXTUREDATA& TextureData)const;
		void __DestroyMaterialData(MATERIALDATA& MaterialData)const;
		void __DestroySkinData(SKINDATA& SkinData)const;
		void __DestroySceneNodeData(SCENENODEDATA& SceneNodeData)const;
		void __DestroySceneData(SCENEDATA& SceneData)const;

		PVOID* __CreateVertexData(UINT uNumVertices, const CVertexDeclaration::VERTEXELEMENT* pVertexElements, UINT uElementCount, PUINT puStreamCount = ALCHEMY_NULL);
		void __DestroyVertexData(PVOID* ppData, UINT uStreamCount);

		CPool<MESHDATA>		 m_MeshPool;
		CPool<TEXTUREDATA>	 m_TexturePool;
		CPool<MATERIALDATA>	 m_MaterialPool;
		CPool<SKINDATA>		 m_SkinPool;
		CPool<SCENENODEDATA> m_SceneNodePool;
		CPool<SCENEDATA>	 m_ScenePool;
	};

	ALCHEMY_INLINE const CMeshManager::MESHDATA* CMeshManager::GetMeshData(HANDLE uHandle)const
	{
		if(!uHandle)
			return ALCHEMY_NULL;

		return m_MeshPool.Get(uHandle - 1);
	}

	ALCHEMY_INLINE const CMeshManager::TEXTUREDATA* CMeshManager::GetTextureData(HANDLE uHandle) const
	{
		if(!uHandle)
			return ALCHEMY_NULL;

		return m_TexturePool.Get(uHandle - 1);
	}

	ALCHEMY_INLINE const CMeshManager::MATERIALDATA* CMeshManager::GetMaterialData(HANDLE uHandle) const
	{
		if(!uHandle)
			return ALCHEMY_NULL;

		return m_MaterialPool.Get(uHandle - 1);
	}

	ALCHEMY_INLINE const CMeshManager::SKINDATA*	CMeshManager::GetSkinData(HANDLE uHandle) const
	{
		if(!uHandle)
			return ALCHEMY_NULL;

		return m_SkinPool.Get(uHandle - 1);
	}

	ALCHEMY_INLINE const CMeshManager::SCENENODEDATA* CMeshManager::GetSceneNodeData(HANDLE uHandle)const
	{
		if(!uHandle)
			return ALCHEMY_NULL;

		return m_SceneNodePool.Get(uHandle - 1);
	}

	ALCHEMY_INLINE const CMeshManager::SCENEDATA* CMeshManager::GetSceneData(HANDLE uHandle)const
	{
		if(!uHandle)
			return ALCHEMY_NULL;

		return m_ScenePool.Get(uHandle - 1);
	}
}