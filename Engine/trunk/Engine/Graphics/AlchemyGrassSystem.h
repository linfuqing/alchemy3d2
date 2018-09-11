/*
 *  AlchemyGrassSystem.h
 *  IOS
 *
 *  Created by lin fuqing on 12-6-28.
 *  Copyright 2012 AstepGame. All rights reserved.
 *
 */

#include "AlchemySceneNode.h"

namespace alchemy
{
	class CGrassSystem : 
	public CSceneNode
	{
	public:
		typedef enum
		{
			COLOR_ALPHA = 0,
			COLOR_RED,
			COLOR_GREEN,
			COLOR_BLUE,
			
			SCALE_X, 
			SCALE_Y,
			SCALE_Z,
			
			ROTATION_X, 
			ROTATION_Y, 
			ROTATION_Z,
			
			HEIGHT
		}GRASSMAPUSAGE;
		
		typedef struct
		{
			void* pColorData;
			UINT uColorMask;
			UINT uStride;
			UINT uMapUsage;
			PUINT puChannelMask;
			PFLOAT pfChannelScale;
			UINT uPitch;
		}GRASSMAP, * LPGRASSMAP;
		
		typedef struct
		{
			UINT uWidth;
			UINT uHeight;
			FLOAT fScale;
			LPGRASSMAP pGrassMaps;
			UINT uGrassMapCount;
		}GRASSMAPINFO, * LPGRASSMAPINFO;
		
		/*typedef struct
		{
			UINT uTexcoordIndex;
			UINT uChannelMask[4];
			UINT uStride;
			UINT uWidth;
			UINT uHeight;
			UINT uPitch;
			void* pMapData;
		}TERRAINMAP, * LPTERRAINMAP;*/
		
		/*typedef struct
		{
			CGraphicsResource::HANDLE Mesh;
			FLOAT3 Up;
			const MATRIX4X4* pTransform;
		}TERRAIN, * LPTERRAIN;*/
		
		ALCHEMY_INTERFACE(ITerrain)
		{
			virtual bool GetHeight(FLOAT3& Output, const FLOAT3& Input) = 0;
		};
		
		class CMeshTerrain :
			public CSmartClass<ITerrain>
		{
		public:
			typedef struct
			{
				CGraphicsResource::HANDLE Mesh;
				FLOAT3 Up;
				const MATRIX4X4* pTransform;
			}TERRAINDATA, * LPTERRAINDATA;
		
			CMeshTerrain();
			~CMeshTerrain();
			
			bool Create(const TERRAINDATA* pTerrainData, UINT uTerrainDataCount);
			
			bool GetHeight(FLOAT3& Output, const FLOAT3& Input);
			
		private:
			typedef struct
			{
				CMesh* pMesh;
				FLOAT3 Up;
				const MATRIX4X4* pTransform;
				CMatrix4x4* pTransposeTransform;
				CMatrix4x4* pInverceTransform;
			}TERRAINDATAINFO, * LPTERRAINDATAINFO;
			
			LPTERRAINDATAINFO m_pTerrainDataInfos;
			UINT m_uTerrainDataInfoCount;
			
			CMatrix4x4* m_pMatrices;
			UINT m_uMatrixCount;
		};
		
		CGrassSystem();
		~CGrassSystem();

		bool Create(CGraphicsResource::HANDLE GrassMesh, 
					UINT uSubset, 
					const GRASSMAPINFO& GrassMapInfo,
					ITerrain* pTerrain = ALCHEMY_NULL);
		
		bool SetMaterial(CGraphicsResource::HANDLE Material);
		void SetRenderPass( UINT uRenderPass );
		bool SetEffect(CGraphicsResource::HANDLE Effect);
		bool SetRenderMethod(IRenderMethod& RenderMethod);
		
		bool ApplyForRender(UINT uRenderPass = ~0, UINT uRenderList = CRenderQueue::NORMAL);
		
		void Render(const CRenderQueue::RENDERENTRY& Entry, const CRenderQueue::RENDERKEY& Key, UINT uFlag);
		
		void Destroy();
		
	private:
		CMesh* m_pSourceMesh;
		UINT m_uSubset;
		UINT m_uRenderPass;
		CMesh** m_ppDestinationMesh;
		UINT m_uDestinationMeshCount;
		CEffect* m_pEffect;
		IRenderMethod* m_pRenderMethod;
		CMaterial* m_pMaterial;
		CSceneManager* m_pSceneManager;
	};
}