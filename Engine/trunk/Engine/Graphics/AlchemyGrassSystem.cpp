/*
 *  AlchemyGrassSystem.cpp
 *  IOS
 *
 *  Created by lin fuqing on 12-6-28.
 *  Copyright 2012 AstepGame. All rights reserved.
 *
 */

#include "AlchemyGrassSystem.h"
#include "AlchemySceneManager.h"

#include "../Core/AlchemyMTRandom.h"

#include "../Math/AlchemyRay.h"
#include "../Math/AlchemyVector3.h"

using namespace alchemy;

CGrassSystem::CMeshTerrain::CMeshTerrain() :
m_pTerrainDataInfos(ALCHEMY_NULL),
m_uTerrainDataInfoCount(0), 
m_pMatrices(ALCHEMY_NULL),
m_uMatrixCount(0)
{
}

CGrassSystem::CMeshTerrain::~CMeshTerrain()
{
	ALCHEMY_DEBUG_DELETE_ARRAY(m_pTerrainDataInfos);
	ALCHEMY_DEBUG_DELETE_ARRAY(m_pMatrices);
}

bool CGrassSystem::CMeshTerrain::Create(const TERRAINDATA* pTerrainData, UINT uTerrainDataCount)
{
	CSceneManager* pSceneManager = CSceneManager::GetInstance();
	if(!pSceneManager)
		return false;
		
	ALCHEMY_DEBUG_DELETE_ARRAY(m_pTerrainDataInfos);
	ALCHEMY_DEBUG_NEW(m_pTerrainDataInfos, TERRAINDATAINFO[uTerrainDataCount]);
	
	m_uTerrainDataInfoCount = uTerrainDataCount;
	m_uMatrixCount = 0;
	const TERRAINDATA* pCurrentTerrainData;
	LPTERRAINDATAINFO pTerrainDataInfo;
	UINT i;
	for(i = 0; i < uTerrainDataCount; ++ i)
	{
		pCurrentTerrainData = &pTerrainData[i];
		pTerrainDataInfo = &m_pTerrainDataInfos[i];
		
		pTerrainDataInfo->pMesh               = static_cast<CMesh*>( pSceneManager->GetResource(pCurrentTerrainData->Mesh, CGraphicsResource::MESH) );
		pTerrainDataInfo->Up                  = pCurrentTerrainData->Up;
		pTerrainDataInfo->pTransform          = pCurrentTerrainData->pTransform;
		pTerrainDataInfo->pTransposeTransform = ALCHEMY_NULL;
		pTerrainDataInfo->pInverceTransform   = ALCHEMY_NULL;
		
		if(pTerrainData->pTransform)
			m_uMatrixCount += 2;
	}
	
	ALCHEMY_DEBUG_NEW(m_pMatrices, CMatrix4x4[m_uMatrixCount]);
	m_uMatrixCount = 0;
	for(i = 0; i < uTerrainDataCount; ++ i)
	{
		pTerrainDataInfo = &m_pTerrainDataInfos[i];
		if(pTerrainDataInfo->pTransform)
		{
			pTerrainDataInfo->pTransposeTransform = &m_pMatrices[m_uMatrixCount ++];
			pTerrainDataInfo->pInverceTransform   = &m_pMatrices[m_uMatrixCount ++];
			
			Copy( *pTerrainDataInfo->pInverceTransform, Copy(*pTerrainDataInfo->pTransposeTransform, *pTerrainDataInfo->pTransform) );
			pTerrainDataInfo->pTransposeTransform->Transpose();
			pTerrainDataInfo->pInverceTransform->Invert4x4();
		}
	}
	
	return true;
}

bool CGrassSystem::CMeshTerrain::GetHeight(FLOAT3& Output, const FLOAT3& Input)
{
	static CRay s_Ray;
	
	LPTERRAINDATAINFO pTerrainDataInfo;
	FLOAT fUpStep;
	UINT uIntersectInfoCount;
	for(UINT i = 0; i < m_uTerrainDataInfoCount; ++ i)
	{
		pTerrainDataInfo = &m_pTerrainDataInfos[i];
		if(pTerrainDataInfo->pMesh)
		{
			fUpStep = Normalize(s_Ray.Direction, pTerrainDataInfo->Up);
			fUpStep = fUpStep > 1.0f ? (fUpStep - 1.0f) : 0.0f;
			s_Ray.Direction.x = - s_Ray.Direction.x;
			s_Ray.Direction.y = - s_Ray.Direction.y;
			s_Ray.Direction.z = - s_Ray.Direction.z;
			
			if(pTerrainDataInfo->pTransform)
			{
				TransformNormal(s_Ray.Direction, *pTerrainDataInfo->pTransposeTransform, s_Ray.Direction);
				Normalize(s_Ray.Direction, s_Ray.Direction);
	
				Copy(s_Ray.Origin, Input);
				
				TransformCoord(s_Ray.Origin, *pTerrainDataInfo->pInverceTransform, s_Ray.Origin);
				
				static CMesh::INTERSECTINFO s_IntersectInfo;
				uIntersectInfoCount = 1;
				if( pTerrainDataInfo->pMesh->Intersect(s_Ray, &s_IntersectInfo, &uIntersectInfoCount) )
				{
					s_Ray.Origin.x += s_IntersectInfo.t * s_Ray.Direction.x;
					s_Ray.Origin.y += s_IntersectInfo.t * s_Ray.Direction.y;
					s_Ray.Origin.z += s_IntersectInfo.t * s_Ray.Direction.z;
					
					TransformCoord(s_Ray.Origin, *pTerrainDataInfo->pTransform, s_Ray.Origin);
					
					Output.x = s_Ray.Origin.x + fUpStep;
					Output.y = s_Ray.Origin.y + fUpStep;
					Output.z = s_Ray.Origin.z + fUpStep;
				}
			}
		}
	}
	
	return true;
}

CGrassSystem::CGrassSystem() :
m_pSourceMesh(ALCHEMY_NULL), 
m_uSubset(0), 
m_ppDestinationMesh(ALCHEMY_NULL), 
m_uDestinationMeshCount(0), 
m_pEffect(ALCHEMY_NULL), 
m_pRenderMethod(ALCHEMY_NULL), 
m_pMaterial(ALCHEMY_NULL), 
m_pSceneManager(ALCHEMY_NULL),
m_uRenderPass(0)
{
}

CGrassSystem::~CGrassSystem()
{
	ALCHEMY_DEBUG_DELETE_ARRAY(m_ppDestinationMesh);
}

bool CGrassSystem::SetEffect(CGraphicsResource::HANDLE Effect)
{
	if(!m_pSceneManager)
		return false;
	
	CEffect* pEffect = static_cast<CEffect*>( m_pSceneManager->GetRenderSystem().GetResource(Effect, CGraphicsResource::EFFECT) );
	
	if(pEffect)
		pEffect->AddRef();
	
	if(m_pEffect)
		m_pEffect->Release();
	
	m_pEffect = pEffect;
	
	return true;
}

bool CGrassSystem::SetRenderMethod(IRenderMethod& RenderMethod)
{
	RenderMethod.AddRef();
	
	if(m_pRenderMethod)
		m_pRenderMethod->Release();
	
	m_pRenderMethod = &RenderMethod;
	
	return true;
}

bool CGrassSystem::SetMaterial(CGraphicsResource::HANDLE Material)
{
	if(!m_pSceneManager)
		return false;
	
	CMaterial* pMaterial = static_cast<CMaterial*>( m_pSceneManager->GetResource(Material, CGraphicsResource::MATERIAL) );
	
	if(pMaterial)
		pMaterial->AddRef();
	
	if(m_pMaterial)
		m_pMaterial->Release();
	
	m_pMaterial = pMaterial;
	
	return true;
}

void CGrassSystem::SetRenderPass( UINT uRenderPass )
{
	m_uRenderPass = uRenderPass;
}

typedef struct
{
	UINT uInstanceIndex;
	UINT uMapIndex;
	CVector<UINT> Maps;
}GRASSINFO, * LPGRASSINFO;

bool CGrassSystem::Create(CGraphicsResource::HANDLE GrassMesh, 
						  UINT uSubset, 
						  const GRASSMAPINFO& GrassMapInfo,
						  ITerrain* pTerrain)
{
	Destroy();
	
	if(GrassMapInfo.pGrassMaps && GrassMapInfo.uGrassMapCount && GrassMapInfo.uWidth && GrassMapInfo.uHeight)
	{
		m_pSceneManager = CSceneManager::GetInstance();
		if(m_pSceneManager)
		{
			m_pSourceMesh = static_cast<CMesh*>( m_pSceneManager->GetResource(GrassMesh, CGraphicsResource::MESH) );
			if(m_pSourceMesh)
			{
				m_pSourceMesh->AddRef();
				
				m_uSubset = uSubset;
				
				LPVERTEXBUFFER pVertexBuffer = m_pSourceMesh->GetVertex().GetVertexBuffer(0);
				if(pVertexBuffer)
				{
					UINT uMapSize = GrassMapInfo.uWidth * GrassMapInfo.uHeight;
					PUINT puMapBuffer;
					ALCHEMY_DEBUG_NEW(puMapBuffer, UINT[uMapSize]);
					ALCHEMY_MEMORY_ZERO( puMapBuffer, uMapSize * sizeof(UINT) );
					PUINT puAttributes;
					ALCHEMY_DEBUG_NEW(puAttributes, UINT[GrassMapInfo.uGrassMapCount]);
					UINT uGrassInstanceCount = 0, uAttributeCount = 0, uIndex, uMapIndex, uMapHandle, uStride, uPitch, i, j, k;
					CVector<GRASSINFO> GrassInfos;
					GRASSINFO GrassInfo;
					GrassInfo.Maps.Add(0);
					
					const GRASSMAP* pGrassMap;
					const UINT8* puColorData;
					for(i = 0; i < GrassMapInfo.uGrassMapCount; ++ i)
					{
						pGrassMap = &GrassMapInfo.pGrassMaps[i];
						puColorData = static_cast<const UINT8*> (pGrassMap->pColorData);
						uStride = pGrassMap->uStride > sizeof(UINT) ? sizeof(UINT) : pGrassMap->uStride;
						uPitch = GrassMapInfo.uWidth * pGrassMap->uStride;
						uPitch = pGrassMap->uPitch ? (pGrassMap->uPitch - uPitch) : 0;
						
						uIndex = 0;
						for(j = 0; j < GrassMapInfo.uHeight; ++ j)
						{
							for(k = 0; k < GrassMapInfo.uWidth; ++ k)
							{
								if( !ALCHEMY_MEMORY_COMPARE(&puColorData[uIndex], &pGrassMap->uColorMask, uStride) )
								{
									uMapIndex = j * GrassMapInfo.uWidth + k;
									uMapHandle = puMapBuffer[uMapIndex];
									if(uMapHandle)
										GrassInfos[uMapHandle - 1].Maps.Add(i);
									else
									{
										GrassInfo.uInstanceIndex = uGrassInstanceCount;
										GrassInfo.uMapIndex      = uIndex;
										GrassInfo.Maps[0]        = i;
										
										puMapBuffer[uMapIndex] = GrassInfos.Add(GrassInfo) + 1;
										
										++ uGrassInstanceCount;
									}
								}
								
								uIndex += uStride;
							}
							
							uIndex += uPitch;
						}
						
						if( ALCHEMY_TEST_BIT(pGrassMap->uMapUsage, COLOR_ALPHA) || 
						    ALCHEMY_TEST_BIT(pGrassMap->uMapUsage, COLOR_RED)   ||
						    ALCHEMY_TEST_BIT(pGrassMap->uMapUsage, COLOR_GREEN) ||
						    ALCHEMY_TEST_BIT(pGrassMap->uMapUsage, COLOR_BLUE) )
							puAttributes[i] = ++ uAttributeCount;
						else
							puAttributes[i] = 0;
					}
					ALCHEMY_DEBUG_DELETE_ARRAY(puMapBuffer);
					CMatrix4x4* pMatrices;
					ALCHEMY_DEBUG_NEW(pMatrices, CMatrix4x4[uGrassInstanceCount]);
					for(i = 0; i < uGrassInstanceCount; ++ i)
						pMatrices[i].Identity();
						
					PFLOAT* ppfAttributeData = ALCHEMY_NULL;
					PFLOAT pfAttributeData;
					UINT uColorAttributeByteSize = uGrassInstanceCount * 4;
					if(uAttributeCount)
					{
						ALCHEMY_DEBUG_NEW(ppfAttributeData, PFLOAT[uAttributeCount]);
						for(i = 0; i < uAttributeCount; ++ i)
						{
							ALCHEMY_DEBUG_NEW(pfAttributeData, FLOAT[uColorAttributeByteSize]);
							ALCHEMY_MEMORY_ZERO(pfAttributeData, uColorAttributeByteSize);
							ppfAttributeData[i] = pfAttributeData;
						}
					}
					
					CVector<GRASSINFO>::CIterator GrassInfosIterator;
					CVector<UINT>::CConstantIterator MapIndexIterator;
					const GRASSINFO* pGrassInfo;
					UINT uColor, uChannelIndex, uChannelMask, uMask;
					FLOAT x, y, z, w;
					CQuaternion Rotation;
					CMatrix4x4 Matrix;
					CMatrix4x4* pMatrix;
					
					if(pTerrain)
					{
						FLOAT3 Position;
						GrassInfosIterator = GrassInfos.GetIterator(0);
						while ( CVector<GRASSINFO>::IsValuable(GrassInfosIterator) ) 
						{
							pGrassInfo = &(*GrassInfosIterator);
							pMatrix = &pMatrices[pGrassInfo->uInstanceIndex];
							MapIndexIterator = pGrassInfo->Maps.GetIterator(0);
							while ( CVector<UINT>::IsValuable(MapIndexIterator) ) 
							{
								uMapIndex = *MapIndexIterator;
								pGrassMap = &GrassMapInfo.pGrassMaps[uMapIndex];
								uStride = pGrassMap->uStride > sizeof(UINT) ? sizeof(UINT) : pGrassMap->uStride;
								
								uColor = 0;
								ALCHEMY_MEMORY_COPY(&uColor, static_cast<const UINT8*>(pGrassMap->pColorData) + pGrassInfo->uMapIndex, uStride);
								
								uChannelIndex = 0;
								
								if( ALCHEMY_TEST_BIT(pGrassMap->uMapUsage, COLOR_ALPHA) || 
									ALCHEMY_TEST_BIT(pGrassMap->uMapUsage, COLOR_RED)   ||
									ALCHEMY_TEST_BIT(pGrassMap->uMapUsage, COLOR_GREEN) ||
									ALCHEMY_TEST_BIT(pGrassMap->uMapUsage, COLOR_BLUE) )
								{
									uMask  = 0;
									x = y = z = 0.0f;
									w = 1.0f;
									
									if( ALCHEMY_TEST_BIT(pGrassMap->uMapUsage, COLOR_ALPHA) )
									{
										uChannelMask = pGrassMap->puChannelMask[uChannelIndex];
										w  = uChannelMask ? (static_cast<FLOAT>(uColor & uChannelMask) / uChannelMask) : ALCHEMY_RAND;
										w *= pGrassMap->pfChannelScale[uChannelIndex];
										
										uMask |= uChannelMask;
										
										++ uChannelIndex;
									}
									
									if( ALCHEMY_TEST_BIT(pGrassMap->uMapUsage, COLOR_RED) )
									{
										uChannelMask = pGrassMap->puChannelMask[uChannelIndex];
										x  = uChannelMask ? (static_cast<FLOAT>(uColor & uChannelMask) / uChannelMask) : ALCHEMY_RAND;
										x *= pGrassMap->pfChannelScale[uChannelIndex];
										
										uMask |= uChannelMask;
										
										++ uChannelIndex;
									}
									
									if( ALCHEMY_TEST_BIT(pGrassMap->uMapUsage, COLOR_GREEN) )
									{
										uChannelMask = pGrassMap->puChannelMask[uChannelIndex];
										y  = uChannelMask ? (static_cast<FLOAT>(uColor & uChannelMask) / uChannelMask) : ALCHEMY_RAND;
										y *= pGrassMap->pfChannelScale[uChannelIndex];
										
										uMask |= uChannelMask;
										
										++ uChannelIndex;
									}
									
									if( ALCHEMY_TEST_BIT(pGrassMap->uMapUsage, COLOR_BLUE) )
									{
										uChannelMask = pGrassMap->puChannelMask[uChannelIndex];
										z  = uChannelMask ? (static_cast<FLOAT>(uColor & uChannelMask) / uChannelMask) : ALCHEMY_RAND;
										z *= pGrassMap->pfChannelScale[uChannelIndex];
										
										uMask |= uChannelMask;
										
										++ uChannelIndex;
									}

									if(uMask)
									{
										uMapHandle = puAttributes[uMapIndex];
										if(uMapHandle)
										{
											pfAttributeData = ppfAttributeData[uMapHandle - 1];
											if(pfAttributeData)
											{
												pfAttributeData += pGrassInfo->uInstanceIndex * 4;
												*(pfAttributeData ++) = x;
												*(pfAttributeData ++) = y;
												*(pfAttributeData ++) = z;
												*(pfAttributeData ++) = w;
											}
										}
									}
								}
								
								if( ALCHEMY_TEST_BIT(pGrassMap->uMapUsage, SCALE_X) || 
									ALCHEMY_TEST_BIT(pGrassMap->uMapUsage, SCALE_Y) || 
									ALCHEMY_TEST_BIT(pGrassMap->uMapUsage, SCALE_Z) )
								{
									x = y = z = 0.0f;
									if( ALCHEMY_TEST_BIT(pGrassMap->uMapUsage, SCALE_X) )
									{
										uChannelMask = pGrassMap->puChannelMask[uChannelIndex];
										x  = uChannelMask ? (static_cast<FLOAT>(uColor & uChannelMask) / uChannelMask) : ALCHEMY_RAND;
										x *= pGrassMap->pfChannelScale[uChannelIndex];
											
										++ uChannelIndex;
									}
									
									if( ALCHEMY_TEST_BIT(pGrassMap->uMapUsage, SCALE_Y) )
									{
										uChannelMask = pGrassMap->puChannelMask[uChannelIndex];
										y  = uChannelMask ? (static_cast<FLOAT>(uColor & uChannelMask) / uChannelMask) : ALCHEMY_RAND;
										y *= pGrassMap->pfChannelScale[uChannelIndex];
										
										++ uChannelIndex;
									}
									
									if( ALCHEMY_TEST_BIT(pGrassMap->uMapUsage, SCALE_Z) )
									{
										uChannelMask = pGrassMap->puChannelMask[uChannelIndex];
										z = uChannelMask ? (static_cast<FLOAT>(uColor & uChannelMask) / uChannelMask) : ALCHEMY_RAND;
										z *= pGrassMap->pfChannelScale[uChannelIndex];
										
										++ uChannelIndex;
									}
									
									pMatrix->Scaling(x, y, z);
								}
								
								if( ALCHEMY_TEST_BIT(pGrassMap->uMapUsage, ROTATION_X) || 
									ALCHEMY_TEST_BIT(pGrassMap->uMapUsage, ROTATION_Y) || 
									ALCHEMY_TEST_BIT(pGrassMap->uMapUsage, ROTATION_Z) )
								{
									x = y = z = 0.0f;
									if( ALCHEMY_TEST_BIT(pGrassMap->uMapUsage, ROTATION_X) )
									{
										uChannelMask = pGrassMap->puChannelMask[uChannelIndex];
										x = uChannelMask ? (static_cast<FLOAT>(uColor & uChannelMask) / uChannelMask) : ALCHEMY_RAND;
										x *= pGrassMap->pfChannelScale[uChannelIndex];
										
										++ uChannelIndex;
									}
									
									if( ALCHEMY_TEST_BIT(pGrassMap->uMapUsage, ROTATION_Y) )
									{
										uChannelMask = pGrassMap->puChannelMask[uChannelIndex];
										y = uChannelMask ? (static_cast<FLOAT>(uColor & uChannelMask) / uChannelMask) : ALCHEMY_RAND;
										y *= pGrassMap->pfChannelScale[uChannelIndex];
										
										++ uChannelIndex;
									}
									
									if( ALCHEMY_TEST_BIT(pGrassMap->uMapUsage, ROTATION_Z) )
									{
										uChannelMask = pGrassMap->puChannelMask[uChannelIndex];
										z = uChannelMask ? (static_cast<FLOAT>(uColor & uChannelMask) / uChannelMask) : ALCHEMY_RAND;
										z *= pGrassMap->pfChannelScale[uChannelIndex];
										
										++ uChannelIndex;
									}
									
									Rotation.RotationYawPitchRoll(y, x, z);
									Matrix.RotationQuaternion(Rotation);
									
									Multiply4x3(*pMatrix, Matrix, *pMatrix);
								}
								
								uStride *= GrassMapInfo.uWidth;
								uPitch   = pGrassMap->uPitch ? pGrassMap->uPitch : uStride;

								x        = static_cast<FLOAT>(pGrassInfo->uMapIndex % uPitch) / uStride              - 0.5f;
								z        = static_cast<FLOAT>(pGrassInfo->uMapIndex / uPitch) / GrassMapInfo.uHeight - 0.5f;
								
								x       *=   GrassMapInfo.fScale;
								z       *= - GrassMapInfo.fScale;
								
								pMatrix->_41 = x;
								pMatrix->_42 = 0.0f;
								pMatrix->_43 = z;
								
								if( ALCHEMY_TEST_BIT(pGrassMap->uMapUsage, HEIGHT) )
								{
									uChannelMask = pGrassMap->puChannelMask[uChannelIndex];
									y  = uChannelMask ? ( (static_cast<FLOAT>(uColor & uChannelMask) / uChannelMask) ) : ALCHEMY_RAND;
									y *= pGrassMap->pfChannelScale[uChannelIndex];
									
									pMatrix->_42 = y;
								}
								
								++ MapIndexIterator;
							}
							
							Position.x = pMatrix->_41;
							Position.y = pMatrix->_42;
							Position.z = pMatrix->_43;
							
							pTerrain->GetHeight(Position, Position);
							
							pMatrix->_41 = Position.x;
							pMatrix->_42 = Position.y;
							pMatrix->_43 = Position.z;
							
							++ GrassInfosIterator;
						}

					}
					else
					{
						GrassInfosIterator = GrassInfos.GetIterator(0);
						while ( CVector<GRASSINFO>::IsValuable(GrassInfosIterator) ) 
						{
							pGrassInfo = &(*GrassInfosIterator);
							pMatrix = &pMatrices[pGrassInfo->uInstanceIndex];
							MapIndexIterator = pGrassInfo->Maps.GetIterator(0);
							while ( CVector<UINT>::IsValuable(MapIndexIterator) ) 
							{
								pGrassMap = &GrassMapInfo.pGrassMaps[*MapIndexIterator];
								uStride = pGrassMap->uStride > sizeof(UINT) ? sizeof(UINT) : pGrassMap->uStride;
								
								uColor = 0;
								ALCHEMY_MEMORY_COPY(&uColor, static_cast<const UINT8*>(pGrassMap->pColorData) + pGrassInfo->uMapIndex, uStride);
								
								uChannelIndex = 0;
								
								if( ALCHEMY_TEST_BIT(pGrassMap->uMapUsage, COLOR_ALPHA) || 
									ALCHEMY_TEST_BIT(pGrassMap->uMapUsage, COLOR_RED)   ||
									ALCHEMY_TEST_BIT(pGrassMap->uMapUsage, COLOR_GREEN) ||
									ALCHEMY_TEST_BIT(pGrassMap->uMapUsage, COLOR_BLUE) )
								{
									uMask  = 0;
									x = y = z = 0.0f;
									w = 1.0f;
									
									if( ALCHEMY_TEST_BIT(pGrassMap->uMapUsage, COLOR_ALPHA) )
									{
										uChannelMask = pGrassMap->puChannelMask[uChannelIndex];
										w  = uChannelMask ? (static_cast<FLOAT>(uColor & uChannelMask) / uChannelMask) : ALCHEMY_RAND;
										w *= pGrassMap->pfChannelScale[uChannelIndex];
										
										uMask |= uChannelMask;
										
										++ uChannelIndex;
									}
									
									if( ALCHEMY_TEST_BIT(pGrassMap->uMapUsage, COLOR_RED) )
									{
										uChannelMask = pGrassMap->puChannelMask[uChannelIndex];
										x  = uChannelMask ? (static_cast<FLOAT>(uColor & uChannelMask) / uChannelMask) : ALCHEMY_RAND;
										x *= pGrassMap->pfChannelScale[uChannelIndex];
										
										uMask |= uChannelMask;
										
										++ uChannelIndex;
									}
									
									if( ALCHEMY_TEST_BIT(pGrassMap->uMapUsage, COLOR_GREEN) )
									{
										uChannelMask = pGrassMap->puChannelMask[uChannelIndex];
										y  = uChannelMask ? (static_cast<FLOAT>(uColor & uChannelMask) / uChannelMask) : ALCHEMY_RAND;
										y *= pGrassMap->pfChannelScale[uChannelIndex];
										
										uMask |= uChannelMask;
										
										++ uChannelIndex;
									}
									
									if( ALCHEMY_TEST_BIT(pGrassMap->uMapUsage, COLOR_BLUE) )
									{
										uChannelMask = pGrassMap->puChannelMask[uChannelIndex];
										z  = uChannelMask ? (static_cast<FLOAT>(uColor & uChannelMask) / uChannelMask) : ALCHEMY_RAND;
										z *= pGrassMap->pfChannelScale[uChannelIndex];
										
										uMask |= uChannelMask;
										
										++ uChannelIndex;
									}

									if(uMask)
									{
										uMapHandle = puAttributes[uMapIndex];
										if(uMapHandle)
										{
											pfAttributeData = ppfAttributeData[uMapHandle - 1];
											if(pfAttributeData)
											{
												pfAttributeData += pGrassInfo->uInstanceIndex * 4;
												*(pfAttributeData ++) = x;
												*(pfAttributeData ++) = y;
												*(pfAttributeData ++) = z;
												*(pfAttributeData ++) = w;
											}
										}
									}
								}
								
								if( ALCHEMY_TEST_BIT(pGrassMap->uMapUsage, SCALE_X) || 
									ALCHEMY_TEST_BIT(pGrassMap->uMapUsage, SCALE_Y) || 
									ALCHEMY_TEST_BIT(pGrassMap->uMapUsage, SCALE_Z) )
								{
									x = y = z = 0.0f;
									if( ALCHEMY_TEST_BIT(pGrassMap->uMapUsage, SCALE_X) )
									{
										uChannelMask = pGrassMap->puChannelMask[uChannelIndex];
										x  = uChannelMask ? (static_cast<FLOAT>(uColor & uChannelMask) / uChannelMask) : ALCHEMY_RAND;
										x *= pGrassMap->pfChannelScale[uChannelIndex];
											
										++ uChannelIndex;
									}
									
									if( ALCHEMY_TEST_BIT(pGrassMap->uMapUsage, SCALE_Y) )
									{
										uChannelMask = pGrassMap->puChannelMask[uChannelIndex];
										y  = uChannelMask ? (static_cast<FLOAT>(uColor & uChannelMask) / uChannelMask) : ALCHEMY_RAND;
										y *= pGrassMap->pfChannelScale[uChannelIndex];
										
										++ uChannelIndex;
									}
									
									if( ALCHEMY_TEST_BIT(pGrassMap->uMapUsage, SCALE_Z) )
									{
										uChannelMask = pGrassMap->puChannelMask[uChannelIndex];
										z = uChannelMask ? (static_cast<FLOAT>(uColor & uChannelMask) / uChannelMask) : ALCHEMY_RAND;
										z *= pGrassMap->pfChannelScale[uChannelIndex];
										
										++ uChannelIndex;
									}
									
									pMatrix->Scaling(x, y, z);
								}
								
								if( ALCHEMY_TEST_BIT(pGrassMap->uMapUsage, ROTATION_X) || 
									ALCHEMY_TEST_BIT(pGrassMap->uMapUsage, ROTATION_Y) || 
									ALCHEMY_TEST_BIT(pGrassMap->uMapUsage, ROTATION_Z) )
								{
									x = y = z = 0.0f;
									if( ALCHEMY_TEST_BIT(pGrassMap->uMapUsage, ROTATION_X) )
									{
										uChannelMask = pGrassMap->puChannelMask[uChannelIndex];
										x = uChannelMask ? (static_cast<FLOAT>(uColor & uChannelMask) / uChannelMask) : ALCHEMY_RAND;
										x *= pGrassMap->pfChannelScale[uChannelIndex];
										
										++ uChannelIndex;
									}
									
									if( ALCHEMY_TEST_BIT(pGrassMap->uMapUsage, ROTATION_Y) )
									{
										uChannelMask = pGrassMap->puChannelMask[uChannelIndex];
										y = uChannelMask ? (static_cast<FLOAT>(uColor & uChannelMask) / uChannelMask) : ALCHEMY_RAND;
										y *= pGrassMap->pfChannelScale[uChannelIndex];
										
										++ uChannelIndex;
									}
									
									if( ALCHEMY_TEST_BIT(pGrassMap->uMapUsage, ROTATION_Z) )
									{
										uChannelMask = pGrassMap->puChannelMask[uChannelIndex];
										z = uChannelMask ? (static_cast<FLOAT>(uColor & uChannelMask) / uChannelMask) : ALCHEMY_RAND;
										z *= pGrassMap->pfChannelScale[uChannelIndex];
										
										++ uChannelIndex;
									}
									
									Rotation.RotationYawPitchRoll(y, x, z);
									Matrix.RotationQuaternion(Rotation);
									
									Multiply4x3(*pMatrix, Matrix, *pMatrix);
								}
								
								uStride *= GrassMapInfo.uWidth;
								uPitch   = pGrassMap->uPitch ? pGrassMap->uPitch : uStride;

								x        = static_cast<FLOAT>(pGrassInfo->uMapIndex % uPitch) / uStride              - 0.5f;
								z        = static_cast<FLOAT>(pGrassInfo->uMapIndex / uPitch) / GrassMapInfo.uHeight - 0.5f;
								
								x       *=   GrassMapInfo.fScale;
								z       *= - GrassMapInfo.fScale;
								
								pMatrix->_41 = x;
								pMatrix->_42 = 0.0f;
								pMatrix->_43 = z;
								
								if( ALCHEMY_TEST_BIT(pGrassMap->uMapUsage, HEIGHT) )
								{
									uChannelMask = pGrassMap->puChannelMask[uChannelIndex];
									y  = uChannelMask ? ( (static_cast<FLOAT>(uColor & uChannelMask) / uChannelMask) ) : ALCHEMY_RAND;
									y *= pGrassMap->pfChannelScale[uChannelIndex];
									
									pMatrix->_42 = y;
								}
								
								++ MapIndexIterator;
							}
							
							++ GrassInfosIterator;
						}
					}
					
					ALCHEMY_DEBUG_DELETE_ARRAY(puAttributes);
					
					const CMesh::ATTRIBUTERANGE* pAttributeRange = m_pSourceMesh->GetAttributeTable().Get(uSubset);
					CMesh* pSourceMesh = pAttributeRange->uStartIndex == 0 && pAttributeRange->uIndexCount == m_pSourceMesh->GetIndexBuffer().GetMemberCount() ? m_pSourceMesh : m_pSourceMesh->Clone(pAttributeRange, 1);
					if(!pSourceMesh)
						pSourceMesh = m_pSourceMesh;
					
					if(uGrassInstanceCount)
					{
						CVertex::LPINSTANCEDATA pInstanceData = ALCHEMY_NULL;
						if(ppfAttributeData && uAttributeCount)
						{
							ALCHEMY_DEBUG_NEW(pInstanceData, CVertex::INSTANCEDATA[uAttributeCount]);
							CVertex::LPINSTANCEDATA pCurrentInstanceData;
							for(i = 0; i < uAttributeCount; ++ i)
							{
								pCurrentInstanceData                 = &pInstanceData[i];
								pCurrentInstanceData->pData          = ppfAttributeData[i];
								pCurrentInstanceData->ElementType    = CVertexDeclaration::FLOAT4;
								pCurrentInstanceData->ElementUsage   = CVertexDeclaration::COLOR;
								pCurrentInstanceData->bIsElementData = true;
							}
						}
						
						CRenderSystem& RenderSystem = m_pSceneManager->GetRenderSystem();
						
						m_uDestinationMeshCount = 1;
						if( !RenderSystem.CheckFormat(CRenderSystem::INDEX_BUFFER, CIndexBuffer::BIT32) )
						{
							const CRenderSystem::RENDERSYSTEMINFO& RenderSystemInfo = RenderSystem.GetRenderSystemInfo();
							UINT uInstanceCount = RenderSystemInfo.uMaxVertexCount / pVertexBuffer->GetMemberCount();
							uInstanceCount = ALCHEMY_MININUM( uInstanceCount, RenderSystemInfo.uMaxVertexIndexCount / m_pSourceMesh->GetIndexBuffer().GetMemberCount() );
							
							if(uInstanceCount < uGrassInstanceCount)
							{
								m_uDestinationMeshCount = ALCHEMY_CEIL(static_cast<FLOAT>(uGrassInstanceCount) / uInstanceCount);
								ALCHEMY_DEBUG_NEW(m_ppDestinationMesh, CMesh*[m_uDestinationMeshCount]);
								
								UINT uFinalInstanceIndex = m_uDestinationMeshCount - 1, uOffset = uInstanceCount * uFinalInstanceIndex, uFinalInstanceCount = uGrassInstanceCount - uOffset;
								m_ppDestinationMesh[uFinalInstanceIndex] = pSourceMesh->ConvertToInstancedMesh(static_cast<FLOAT>(uFinalInstanceCount), pMatrices + uOffset, ALCHEMY_NULL, 0, pInstanceData, uAttributeCount);
								uOffset = 0;
								for(i = 0; i < uFinalInstanceIndex; ++ i)
								{
									m_ppDestinationMesh[i] = pSourceMesh->ConvertToInstancedMesh(static_cast<FLOAT>(uInstanceCount), pMatrices + uOffset, ALCHEMY_NULL, 0, pInstanceData, uAttributeCount);
									
									uOffset += uInstanceCount;
								}
							}
						}
						
						if(!m_ppDestinationMesh)
						{
							ALCHEMY_DEBUG_NEW(m_ppDestinationMesh, CMesh*[m_uDestinationMeshCount]);
							*m_ppDestinationMesh = pSourceMesh->ConvertToInstancedMesh(static_cast<FLOAT>(uGrassInstanceCount), pMatrices, ALCHEMY_NULL, 0, pInstanceData, uAttributeCount);
						}
						
						if(pSourceMesh != m_pSourceMesh)
							pSourceMesh->Release();
						
						ALCHEMY_DEBUG_DELETE_ARRAY(pInstanceData);
					}
					
					ALCHEMY_DEBUG_DELETE_ARRAY(pMatrices);
						
					for(i = 0; i < uAttributeCount; ++ i)
						ALCHEMY_DEBUG_DELETE_ARRAY(ppfAttributeData[i]);
							
					ALCHEMY_DEBUG_DELETE_ARRAY(ppfAttributeData);
					
					return true;
				}
				else
					ALCHEMY_DEBUG_WARNING("CGrassSystem::Create: Error grass mesh.");

			}
			else 
				ALCHEMY_DEBUG_WARNING("CGrassSystem::Create: Grass mesh has been get fail.");

		}
	}
	
	return false;
}

bool CGrassSystem::ApplyForRender(UINT uRenderPass, UINT uRenderList)
{
	if(m_pSceneManager && m_ppDestinationMesh && *m_ppDestinationMesh && m_pEffect)
	{
		CRenderQueue& RenderQueue = m_pSceneManager->GetRenderQueue();
		
		CRenderQueue::LPRENDERENTRY pEntry;
		
		CGraphicsResource::HANDLE VertexDeclaration = (*m_ppDestinationMesh)->GetVertex().GetVertexDeclaration().GetResourceHandle(), Mesh = m_pSourceMesh->GetResourceHandle();

		pEntry = &RenderQueue.LockRenderEntry(*this);

		pEntry->uEffect            = m_pEffect->GetResourceHandle();
		pEntry->uRenderPass        = uRenderPass == ~0 ? m_uRenderPass : uRenderPass;
		pEntry->uVertexDeclaration = VertexDeclaration;
		pEntry->uModel             = Mesh;
		pEntry->uRenderTarget	   = 0;
		pEntry->uModelParameter    = m_uSubset;
		pEntry->uUserData          = 0;

		m_pSceneManager->GetRenderQueue().UnlockRenderEntry(uRenderList);
		
		return true;
	}
	
	return false;
}

void CGrassSystem::Render(const CRenderQueue::RENDERENTRY& Entry, const CRenderQueue::RENDERKEY& Key, UINT uFlag)
{
	if(m_pSceneManager && m_ppDestinationMesh && m_uDestinationMeshCount && m_pEffect && m_pRenderMethod)
	{
		CRenderQueue& RenderQueue = m_pSceneManager->GetRenderQueue();

		if(m_pMaterial)
		{
			UINT uTextureCount;
			const UINT* pTextureAlphaIndexes = m_pRenderMethod->GetTextureInfo(Key.uRenderPass, Key.uRenderTargetIndex, uTextureCount);
			if(pTextureAlphaIndexes)
			{
				const LPTEXTURE* ppTexture;
				for(UINT i = 0; i < uTextureCount; ++ i)
				{
					ppTexture = m_pMaterial->GetTextures().Get( pTextureAlphaIndexes[i] );

					if( ppTexture && *ppTexture && ALCHEMY_TEST_BIT( (*ppTexture)->GetUsage(), CTexture::RENDER_TARGET ) && (*ppTexture)->GetResourceHandle() == RenderQueue.GetRenderTarget() )
						return;
				}
			}
		}

		if( ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_RENDER_TARGET) )
		{
			if(Entry.uRenderTarget)
				RenderQueue.SetRenderTarget(Entry.uRenderTarget, 0, false);
			else
				RenderQueue.SetRenderTarget();

			RenderQueue.ActivateMultipleRenderTarget( Key.uRenderTargetIndex, m_pEffect->GetRenderTargetCount(Key.uRenderPass) );
		}

		CRenderSystem& RenderSystem = m_pSceneManager->GetRenderSystem();

		if(ALCHEMY_TEST_BIT(uFlag, CRenderQueue::PARENT) || ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_EFFECT) || ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_EFFECT_PASS) )
		{
			if( ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_EFFECT) && Entry.uEffect )
				RenderSystem.SetShader(Entry.uEffect);

			if( ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_EFFECT_PASS) && Entry.uRenderPass )
				RenderSystem.SetShader(Entry.uRenderPass);
		}

		if( !m_pRenderMethod->Activate(Key.uRenderPass, Key.uRenderTargetIndex) )
			return;

		RenderSystem.ResetTexture();

		RenderSystem.RegisterRenderState();

		m_pRenderMethod->CommitChange(&GetWorldMatrix(), m_pMaterial);

		if( ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_VERTEX_DECLARATION) && ( !Entry.uVertexDeclaration || !RenderSystem.SetVertexDeclaration(Entry.uVertexDeclaration) ) )
			return;

		CMesh* pMesh;
		for(UINT i = 0; i < m_uDestinationMeshCount; ++ i)
		{
			pMesh = m_ppDestinationMesh[i];
			if(pMesh)
				pMesh->DrawSubset(0, ALCHEMY_TEST_BIT(uFlag, CRenderQueue::MODEL), ALCHEMY_TEST_BIT(uFlag, CRenderQueue::ENTRY_VERTEX_DECLARATION) && !Entry.uVertexDeclaration);
		}

		RenderSystem.RestoreRenderState();
	}
}

void CGrassSystem::Destroy()
{
	if(m_pSourceMesh)
		m_pSourceMesh->Release();
	
	if(m_ppDestinationMesh)
	{
		CMesh* pMesh;
		for(UINT i = 0; i < m_uDestinationMeshCount; ++ i)
		{
			pMesh = m_ppDestinationMesh[i];
			
			if(pMesh)
				pMesh->Release();
		}
		
		ALCHEMY_DEBUG_DELETE_ARRAY(m_ppDestinationMesh);
	}
	
	if(m_pEffect)
		m_pEffect->Release();
	
	if(m_pRenderMethod)
		m_pRenderMethod->Release();
	
	if(m_pMaterial)
		m_pMaterial->Release();
	
	m_pSceneManager = ALCHEMY_NULL;
}