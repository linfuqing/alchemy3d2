#include "AlchemySceneObject.h"
#include "AlchemySceneManager.h"
#include "AlchemyPositionEffect.h"
#include "AlchemyDepthEffect.h"
#include "AlchemyProgramInstance.h"
#include "AlchemySkinMeshController.h"

#include "../Core/AlchemyUtils.h"
#include "../Math/AlchemyVector3.h"

using namespace alchemy;

CSceneObject::CSceneObject() :
m_pSourceMesh(ALCHEMY_NULL),
m_pDestinationMesh(ALCHEMY_NULL),
m_pMaterial(ALCHEMY_NULL),
m_pRenderMethod(ALCHEMY_NULL),
m_pDestinationRenderMethod(ALCHEMY_NULL),
m_ppDestinationVertexDeclaration(ALCHEMY_NULL),
m_pSceneManager(ALCHEMY_NULL),
m_pRoot(ALCHEMY_NULL),
m_ppBones(ALCHEMY_NULL),
m_pSkinInfo(ALCHEMY_NULL),
m_ppChannels(ALCHEMY_NULL),
m_puChannelsCount(ALCHEMY_NULL),
m_puSkinningStream(ALCHEMY_NULL),
m_ppBoneCombinationMatrices(ALCHEMY_NULL),
m_pBoneCombination(ALCHEMY_NULL),
m_pHardwareSkinEffect(ALCHEMY_NULL),
m_pBoneMatrices(ALCHEMY_NULL),
m_pAnimationController(ALCHEMY_NULL),
m_SkinType(SKIN_NONE),
m_pShadowMapRenderMethod(ALCHEMY_NULL),
m_pShadowMapVertexDeclaration(ALCHEMY_NULL),
m_pSkinMeshController(ALCHEMY_NULL),
m_bIsUpdate(false),
m_ppPassProgramMap(ALCHEMY_NULL),
m_pAssistantInstance(ALCHEMY_NULL),
m_pNoDrawInstance(ALCHEMY_NULL)
{
	
}

CSceneObject::~CSceneObject(void)
{
	ClearShareVertexInfo();

	__DestroyDestinationVertexDeclaration();

	//ALCHEMY_DEBUG_DELETE_ARRAY(m_pBoneMatrices);

	ALCHEMY_DEBUG_DELETE_ARRAY(m_puSkinningStream);

	ALCHEMY_DEBUG_DELETE_ARRAY(m_ppChannels);

	ALCHEMY_DEBUG_DELETE_ARRAY(m_puChannelsCount);

	ALCHEMY_DEBUG_DELETE(m_pHardwareSkinEffect);

	ALCHEMY_DEBUG_DELETE_ARRAY(m_ppBoneCombinationMatrices);

	if(m_pBoneCombination)
	{
		//m_pBoneCombination->Release();

		m_pBoneCombination = ALCHEMY_NULL;
	}

	CMap<UINT, CShadowMapDisplayEffect*>::CIterator Iterator = m_ShadowMapDisplayEffectMap.GetBeginIterator();
	CShadowMapDisplayEffect* pShadowMapDisplayEffect;
	while( CMap<UINT, CShadowMapDisplayEffect*>::IsValuable(Iterator) )
	{
		pShadowMapDisplayEffect = Iterator.GetValue();
		if(pShadowMapDisplayEffect)
			ALCHEMY_DEBUG_DELETE(pShadowMapDisplayEffect);

		++ Iterator;
	}

	CPool<RENDERDATA*>::CIterator Iter = m_RenderDataPool.GetBeginIterator();
	RENDERDATA* pRenderData = ALCHEMY_NULL, *pNextRenderData = ALCHEMY_NULL;
	while( CPool<RENDERDATA*>::IsValuable( Iter ) )
	{
		pRenderData = *Iter;

		while ( pRenderData != ALCHEMY_NULL )
		{
			if(pRenderData->uBoneCount > 0)
			{
				ALCHEMY_DEBUG_DELETE( pRenderData->pAttribute );
				ALCHEMY_DEBUG_DELETE_ARRAY( pRenderData->ppBoneMatrix );
			}

			pNextRenderData = pRenderData->pNext;

			ALCHEMY_DEBUG_DELETE(pRenderData);

			pRenderData = pNextRenderData;
		}

		++Iter;
	}

	UINT uLen = m_ProgramInstances.GetLength();
	for (UINT i=0; i<uLen; ++i)
		m_ProgramInstances[i]->Release();

	if(m_pSkinMeshController != ALCHEMY_NULL && m_bIsUpdate)
		ALCHEMY_DEBUG_DELETE(m_pSkinMeshController);

	ALCHEMY_DEBUG_DELETE_ARRAY(m_ppPassProgramMap);
	ALCHEMY_DEBUG_DELETE_ARRAY(m_pAssistantInstance);
	ALCHEMY_DEBUG_DELETE(m_pNoDrawInstance);
}

void CSceneObject::Destroy()
{
	if(m_pMaterial)
	{
		m_pMaterial->Release();

		m_pMaterial = ALCHEMY_NULL;
	}

	if(m_pSourceMesh)
	{
		m_pSourceMesh->Release();

		m_pSourceMesh = ALCHEMY_NULL;
	}

	//__DestroyDestinationMesh();
	ClearShareVertexInfo();

	__DestroyDestinationVertexDeclaration();

	//ALCHEMY_DEBUG_DELETE_ARRAY(m_pBoneMatrices);

	ALCHEMY_DEBUG_DELETE_ARRAY(m_puSkinningStream);

	ALCHEMY_DEBUG_DELETE_ARRAY(m_ppChannels);

	ALCHEMY_DEBUG_DELETE_ARRAY(m_puChannelsCount);

	ALCHEMY_DEBUG_DELETE(m_pHardwareSkinEffect);

	if(m_pShadowMapRenderMethod)
	{
		m_pShadowMapRenderMethod->Release();

		m_pShadowMapRenderMethod = ALCHEMY_NULL;
	}

	if(m_pShadowMapVertexDeclaration)
	{
		m_pShadowMapVertexDeclaration->Release();

		m_pShadowMapVertexDeclaration = ALCHEMY_NULL;
	}

	ALCHEMY_DEBUG_DELETE_ARRAY(m_ppBoneCombinationMatrices);

	if(m_pBoneCombination)
	{
		//m_pBoneCombination->Release();

		m_pBoneCombination = ALCHEMY_NULL;
	}

	CMap<UINT, CShadowMapDisplayEffect*>::CIterator Iterator = m_ShadowMapDisplayEffectMap.GetBeginIterator();
	CShadowMapDisplayEffect* pShadowMapDisplayEffect;
	while( CMap<UINT, CShadowMapDisplayEffect*>::IsValuable(Iterator) )
	{
		pShadowMapDisplayEffect = Iterator.GetValue();
		if(pShadowMapDisplayEffect)
			ALCHEMY_DEBUG_DELETE(pShadowMapDisplayEffect);

		++ Iterator;
	}

	m_ShadowMapDisplayEffectMap.Clear();

	if(m_pSceneManager)
	{
		CSceneObject** ppSceneObject = m_pSceneManager->m_SceneObjectPool.Get(m_uSceneIndex);
		if(ppSceneObject && static_cast<CSceneObject*>(this) ==  *ppSceneObject)
			m_pSceneManager->m_SceneObjectPool.Remove(m_uSceneIndex, 1);

		m_pSceneManager = ALCHEMY_NULL;
	}

	CPool<RENDERDATA*>::CIterator Iter = m_RenderDataPool.GetBeginIterator();
	RENDERDATA* pRenderData = ALCHEMY_NULL, *pNextRenderData = ALCHEMY_NULL;
	while( CPool<RENDERDATA*>::IsValuable( Iter ) )
	{
		pRenderData = *Iter;

		while ( pRenderData != ALCHEMY_NULL )
		{
			if(pRenderData->uBoneCount > 0)
			{
				ALCHEMY_DEBUG_DELETE( pRenderData->pAttribute );
				ALCHEMY_DEBUG_DELETE_ARRAY( pRenderData->ppBoneMatrix );
			}

			pNextRenderData = pRenderData->pNext;

			ALCHEMY_DEBUG_DELETE(pRenderData);

			pRenderData = pNextRenderData;
		}

		++Iter;
	}

	m_RenderDataPool.Clear();

	m_AttributeTable.Clear();

	UINT uLen = m_ProgramInstances.GetLength();
	for (UINT i=0; i<uLen; ++i)
		m_ProgramInstances[i]->Release();

	m_ProgramInstances.Clear();

	if(m_pSkinMeshController != ALCHEMY_NULL && m_bIsUpdate)
		ALCHEMY_DEBUG_DELETE(m_pSkinMeshController);

	ALCHEMY_DEBUG_DELETE_ARRAY(m_ppPassProgramMap);
	ALCHEMY_DEBUG_DELETE_ARRAY(m_pAssistantInstance);
	ALCHEMY_DEBUG_DELETE(m_pNoDrawInstance);

	/*if(m_pEffectController)
	{
		ALCHEMY_DEBUG_DELETE(m_pEffectController);
		m_pEffectController = ALCHEMY_NULL;
	}*/
}

bool CSceneObject::SetRenderMethod(CRenderMethodInterface& RenderMethod, bool bIsUseSavedInfo)
{
	if(!m_pDestinationMesh || !m_pSceneManager)
		return false;

	__DestroyDestinationVertexDeclaration();

	m_pRenderMethod = &RenderMethod;

	UINT uPassCount = RenderMethod.GetEffect().GetTotalPass();

	ALCHEMY_DEBUG_NEW(m_ppDestinationVertexDeclaration, LPDESTINATIONVERTEXDECLARATION[uPassCount]);

	UINT i;
	for(i = 0; i < uPassCount; ++ i)
		m_ppDestinationVertexDeclaration[i] = ALCHEMY_NULL;

	UINT uVertexElementParameterCount;
	const CEffectCode::ELEMENTPARAMETER* pVertexElementParameters;
	CSceneObject::LPDESTINATIONVERTEXDECLARATION pDestinationVertexDeclaration;
	for(i = 0; i < uPassCount; ++ i)
	{
		pVertexElementParameters = RenderMethod.GetVertexElementParameters(i, &uVertexElementParameterCount);

		ALCHEMY_DEBUG_NEW(pDestinationVertexDeclaration, DESTINATIONVERTEXDECLARATION);
		pDestinationVertexDeclaration->uRenderData = 0;
		pDestinationVertexDeclaration->pNext   = ALCHEMY_NULL;
		m_ppDestinationVertexDeclaration[i] = pDestinationVertexDeclaration;
		if( !( pDestinationVertexDeclaration->pVertexDeclaration = __CreateVertexDeclaration(pVertexElementParameters, uVertexElementParameterCount) ) )
		{
			ALCHEMY_DEBUG_WARNING("CSceneObject::SetRenderMethod: Vertex declaration has been created fail: Vertex declaration can not be created.");

			__DestroyDestinationVertexDeclaration();

			m_pRenderMethod = ALCHEMY_NULL;

			return false;
		}
	}

	if(bIsUseSavedInfo)
	{
		CVector<SHAREVERTEXINFO>::CIterator Iterator = m_ShareVertexInfos.GetBeginIterator();

		LPSHAREVERTEXINFO pShareVertexInfo;
		while( CVector<SHAREVERTEXINFO>::IsValuable(Iterator) )
		{
			pShareVertexInfo = &(*Iterator);

			__AddShareVertexDestination(pShareVertexInfo->pElementParameters, pShareVertexInfo->uElementParameterCount, pShareVertexInfo->uRenderData);

			++ Iterator;
		}
	}

	__BuildProgramInstanceMap();

	//if(m_pRenderMethod)
	//	m_pRenderMethod->Release();

	return true;
}

void CSceneObject::AddProgramInstance(CProgramInstance& ProgramInstance, INT nPassIndex)
{
	if(nPassIndex == -1)
	{
		if(m_pNoDrawInstance == ALCHEMY_NULL)
			ALCHEMY_DEBUG_NEW(m_pNoDrawInstance, CVector<CProgramInstance*>);

		m_pNoDrawInstance->Add( &ProgramInstance );
	}
	else
	{
		PROGRAM_INFO ProgramInfo;
		ProgramInfo.nPassIndex = nPassIndex;
		ProgramInfo.pProgramInstance = &ProgramInstance;

		if(m_pAssistantInstance == ALCHEMY_NULL)
			ALCHEMY_DEBUG_NEW( m_pAssistantInstance, CVector<PROGRAM_INFO> );

		m_pAssistantInstance->Add( ProgramInfo );
	}

	m_ProgramInstances.Add( &ProgramInstance );
}

CMesh* CSceneObject::SetSkinInfo(SKINTYPE SkinType, CSkinInfo& SkinInfo, CBone& Root, const CEffectCode::ELEMENTPARAMETER* pSkinElementParameters, UINT uElementCount, UINT uMaxBoneInfluence, CEffectCode::CPassDefiner* pPassDefiner)
{
	if(!m_pSourceMesh)
		return ALCHEMY_NULL;

	m_SkinType = SkinType;

	if(!pSkinElementParameters || !uElementCount)
	{
		ALCHEMY_DEBUG_WARNING("No skin element.");

		return ALCHEMY_NULL;
	}

	m_pSkinInfo = &SkinInfo;
	m_pRoot     = &Root;

	UINT uBoneCount = SkinInfo.GetBoneCount();

	ALCHEMY_DEBUG_DELETE_ARRAY(m_ppBones);
	ALCHEMY_DEBUG_NEW(m_ppBones, CBone*[uBoneCount]);


	UINT i;
	STRING Name;
	for(i = 0; i < uBoneCount; ++ i)
	{
		Name = SkinInfo.GetName(i);
		if(Root.GetName() == Name)
			m_ppBones[i] = &Root;
		else
			m_ppBones[i] = Root.GetChildByName(Name);
	}
	ALCHEMY_DEBUG_DELETE_ARRAY(m_pBoneMatrices);
	ALCHEMY_DEBUG_NEW(m_pBoneMatrices, CMatrix4x4[uBoneCount]);

	CMesh* pMesh = ALCHEMY_NULL;

	if(SkinType == SKIN_HARDWARE)
	{
		UINT uMaxVertexInfluence = 0;
		UINT uMatrixIndexOffset  = 1;
		if(pPassDefiner)
		{
			CEffectCode& Code = pPassDefiner->GetParent();

			uMatrixIndexOffset = Code.GetRegisterCount(CEffectCode::MATRIX4X3_VARIABLE) >> Code.GetCompiler().GetByteAlignmentShift();
		}

		CBuffer* pBoneCombinationBuffer = ALCHEMY_NULL;
		UINT uBoneCombinationCount = 0;
		pMesh = m_pSourceMesh->ConvertToBlendedMesh(SkinInfo, ALCHEMY_FLAG(VERTEXBUFFER::WRITE_ONLY), ALCHEMY_FLAG(VERTEXBUFFER::WRITE_ONLY), uMatrixIndexOffset, uMaxBoneInfluence, &uMaxVertexInfluence, &pBoneCombinationBuffer, &uBoneCombinationCount);

		if(pMesh)
		{
			if(pBoneCombinationBuffer)
			{
				//m_pBoneCombination      = pBoneCombinationBuffer;
				m_uBoneCombinationCount = uBoneCombinationCount;

				__UpdateRenderData();

				ALCHEMY_DEBUG_DELETE_ARRAY(m_ppBoneCombinationMatrices);
				ALCHEMY_DEBUG_NEW(m_ppBoneCombinationMatrices, CMatrix4x4*[uMaxBoneInfluence]);
			}

			if(pPassDefiner)
			{
				if(!m_pHardwareSkinEffect)
					ALCHEMY_DEBUG_NEW(m_pHardwareSkinEffect, CHardwareSkinEffect);

				m_pHardwareSkinEffect->Create(uMaxBoneInfluence ? ALCHEMY_MININUM(uBoneCount, uMaxBoneInfluence) : uBoneCount, uMaxVertexInfluence, pSkinElementParameters, uElementCount);

				pPassDefiner->AddInfo(*m_pHardwareSkinEffect, ALCHEMY_NULL);
			}
		}
	}
	else if(SkinType == SKIN_SOFTWARE)
	{
		CVertexDeclaration::LPVERTEXELEMENT pSkinVertexElement;

		ALCHEMY_DEBUG_NEW(pSkinVertexElement, CVertexDeclaration::VERTEXELEMENT[uElementCount]);

		CVertexDeclaration& VertexDeclaration = m_pSourceMesh->GetVertex().GetVertexDeclaration();
		const CVertexDeclaration::VERTEXELEMENT* pSourceVertexElement = VertexDeclaration.GetVertexElements();

		UINT uSourceVertexElementCount = VertexDeclaration.GetVertexElementCount(), uCurrentElement;
		const CEffectCode::ELEMENTPARAMETER* pElementParameter;
		for(i = 0; i < uElementCount; ++ i)
		{
			pElementParameter = &pSkinElementParameters[i];
			uCurrentElement = CVertexDeclaration::FindVertexElement(pElementParameter->Usage, pElementParameter->Type, pElementParameter->uIndex, pSourceVertexElement, uSourceVertexElementCount);

			if(uCurrentElement < uSourceVertexElementCount)
				pSkinVertexElement[i] = pSourceVertexElement[uCurrentElement];
			else
			{
				ALCHEMY_DEBUG_WARNING("CSceneObject::SetSkinInfo: Skin vertex element lookup failed.");

				ALCHEMY_DEBUG_DELETE_ARRAY(pSkinVertexElement);

				return ALCHEMY_NULL;
			}
		}

		m_uSkinningStreamCount = CVertexDeclaration::GetVertexElementStreamCount(pSkinVertexElement, uElementCount);

		ALCHEMY_DEBUG_DELETE_ARRAY(m_puSkinningStream);
		ALCHEMY_DEBUG_NEW(m_puSkinningStream, UINT[m_uSkinningStreamCount]);

		UINT uCurrentStream, j;
		const CVertexDeclaration::VERTEXELEMENT* pCurrentElement;

		m_uSkinningStreamCount = 0;
		for(i = 0; i < uElementCount; ++ i)
		{
			pCurrentElement = pSkinVertexElement + m_uSkinningStreamCount;

			++ m_uSkinningStreamCount;

			uCurrentStream = pSkinVertexElement[i].uStream;
			for(j = 0; j < i; ++ j)
			{
				if(uCurrentStream == pSkinVertexElement[j].uStream)
				{
					-- m_uSkinningStreamCount;

					pCurrentElement = ALCHEMY_NULL;

					break;
				}
			}

			if(pCurrentElement)
				m_puSkinningStream[m_uSkinningStreamCount - 1] = pCurrentElement->uStream;
		}

		ALCHEMY_DEBUG_DELETE_ARRAY(m_ppChannels);
		ALCHEMY_DEBUG_NEW(m_ppChannels, CSkinInfo::LPCHANNEL[m_uSkinningStreamCount]);

		ALCHEMY_DEBUG_DELETE_ARRAY(m_puChannelsCount);
		ALCHEMY_DEBUG_NEW(m_puChannelsCount, UINT[m_uSkinningStreamCount]);

		UINT uStream, uChannelCount;
		CSkinInfo::LPCHANNEL pChannels, pCurrentChannel;
		for(i = 0; i < m_uSkinningStreamCount; ++ i)
		{
			uStream = m_puSkinningStream[i];
			uChannelCount = 0;
			for(j = 0; j < uElementCount; ++ j)
			{
				if(pSkinVertexElement[j].uStream == uStream)
					++ uChannelCount;
			}

			m_puChannelsCount[i] = uChannelCount;
			ALCHEMY_DEBUG_NEW(m_ppChannels[i], CSkinInfo::CHANNEL[uChannelCount]);

			pChannels = m_ppChannels[i];
			uChannelCount = 0;
			for(j = 0; j < uElementCount; ++ j)
			{
				pCurrentElement = &pSkinVertexElement[j];
				if(pCurrentElement->uStream == uStream)
				{
					pCurrentChannel = &pChannels[uChannelCount ++];

					pCurrentChannel->uSourceOffset = pCurrentChannel->uDestinationOffset = pCurrentElement->uOffset;

					pCurrentChannel->bIsNormal = pCurrentElement->Usage == CVertexDeclaration::POSITION ? false : true;
				}
			}
		}

		ALCHEMY_DEBUG_DELETE_ARRAY(pSkinVertexElement);

		UINT uUsage = ALCHEMY_FLAG(VERTEXBUFFER::WRITE_ONLY) | ALCHEMY_FLAG(VERTEXBUFFER::DYNAMIC);
		PUINT puUsage;

		ALCHEMY_DEBUG_NEW(puUsage, UINT[m_uSkinningStreamCount]);

		for(i = 0; i < m_uSkinningStreamCount; ++ i)
			puUsage[i] = uUsage;

		pMesh = m_pSourceMesh->Clone(m_puSkinningStream, m_uSkinningStreamCount, puUsage, false, 0);

		ALCHEMY_DEBUG_DELETE_ARRAY(puUsage);
	}

	if(pMesh)
	{
		if(m_pDestinationMesh)
			m_pDestinationMesh->Release();

		m_pDestinationMesh = pMesh;

		pMesh->AddRef();
	}

	if(m_pRoot)
		m_pRoot->Update();

	return pMesh;
}

CMesh* CSceneObject::SetSkinInfo(SKINTYPE SkinType, CSkinInfo& SkinInfo, CBone& Root, UINT uMaxBoneInfluence, CEffectCode::CPassDefiner* pPassDefiner)
{
	CEffectCode::ELEMENTPARAMETER SkinElementParameters[4];
	CEffectCode::LPELEMENTPARAMETER pSkinElementParameter;

	UINT uSkinElementParameterCount = 0;

	CVertexDeclaration& VertexDeclaration = m_pSourceMesh->GetVertex().GetVertexDeclaration();
	const CVertexDeclaration::VERTEXELEMENT* pSourceVertexElement = VertexDeclaration.GetVertexElements();

	UINT uSourceVertexElementCount = VertexDeclaration.GetVertexElementCount(), uCurrentElement;

	uCurrentElement = CVertexDeclaration::FindVertexElement(CVertexDeclaration::POSITION, CVertexDeclaration::FLOAT4, 0, pSourceVertexElement, uSourceVertexElementCount);
	if(uCurrentElement < uSourceVertexElementCount)
	{
		pSkinElementParameter         = &SkinElementParameters[uSkinElementParameterCount ++ ];
		pSkinElementParameter->Type   = CVertexDeclaration::FLOAT4;
		pSkinElementParameter->Usage  = CVertexDeclaration::POSITION;
		pSkinElementParameter->uIndex = 0;
	}

	uCurrentElement = CVertexDeclaration::FindVertexElement(CVertexDeclaration::NORMAL, CVertexDeclaration::FLOAT3, 0, pSourceVertexElement, uSourceVertexElementCount);
	if(uCurrentElement < uSourceVertexElementCount)
	{
		pSkinElementParameter         = &SkinElementParameters[uSkinElementParameterCount ++ ];
		pSkinElementParameter->Type   = CVertexDeclaration::FLOAT3;
		pSkinElementParameter->Usage  = CVertexDeclaration::NORMAL;
		pSkinElementParameter->uIndex = 0;
	}

	uCurrentElement = CVertexDeclaration::FindVertexElement(CVertexDeclaration::TANGENT, CVertexDeclaration::FLOAT3, 0, pSourceVertexElement, uSourceVertexElementCount);
	if(uCurrentElement < uSourceVertexElementCount)
	{
		pSkinElementParameter         = &SkinElementParameters[uSkinElementParameterCount ++ ];
		pSkinElementParameter->Type   = CVertexDeclaration::FLOAT3;
		pSkinElementParameter->Usage  = CVertexDeclaration::TANGENT;
		pSkinElementParameter->uIndex = 0;
	}

	uCurrentElement = CVertexDeclaration::FindVertexElement(CVertexDeclaration::BINORMAL, CVertexDeclaration::FLOAT3, 0, pSourceVertexElement, uSourceVertexElementCount);
	if(uCurrentElement < uSourceVertexElementCount)
	{
		pSkinElementParameter         = &SkinElementParameters[uSkinElementParameterCount ++ ];
		pSkinElementParameter->Type   = CVertexDeclaration::FLOAT3;
		pSkinElementParameter->Usage  = CVertexDeclaration::BINORMAL;
		pSkinElementParameter->uIndex = 0;
	}

	return SetSkinInfo(SkinType, SkinInfo, Root, SkinElementParameters, uSkinElementParameterCount, uMaxBoneInfluence, pPassDefiner);
}

bool CSceneObject::SetAnimationController(CAnimationController& AnimationController)
{
	if( __IsSkinned() )
	{
		m_pAnimationController = &AnimationController;

		m_pSkinMeshController->GetRootBone()->RegisterAnimationController(AnimationController);

		return true;
	}

	ALCHEMY_DEBUG_WARNING("CSceneObject::SetAnimationController: No skin info.");

	return false;
}

bool CSceneObject::SetShadowEnble(SHADOWTYPE Type, UINT uLightIndex, CCompilerInterface& Compiler)
{
	if(!m_pDestinationMesh || /*!m_pDestinationMesh->pMesh || */!m_pSceneManager)
		return false;

	CLight* pLight = m_pSceneManager->GetLight(uLightIndex);
	if(!pLight)
	{
		ALCHEMY_DEBUG_WARNING("CSceneObject::SetShadowEnble: Error light index.");

		return false;
	}

	if( !pLight->AddShadowObject(*this, Type) )
	{
		ALCHEMY_DEBUG_WARNING("CSceneObject::SetShadowEnble: Add shadow object fail.");

		return false;
	}

	if(Type == SHADOW_MAP)
	{
		if(!m_pShadowMapRenderMethod)
		{
			CEffectCode EffectCode(Compiler);
			CEffectCode::CPassDefiner PassDefiner(EffectCode);
			CDepthEffect DepthEffect;
			CPositionEffect PositionEffect;

			if(m_pHardwareSkinEffect)
				PassDefiner.AddInfo(*m_pHardwareSkinEffect, ALCHEMY_NULL);

			if( PassDefiner.AddInfo(PositionEffect, ALCHEMY_NULL) && PassDefiner.AddInfo(DepthEffect, ALCHEMY_NULL) )
			{
				if( PassDefiner.EndDefine() )
				{
					m_pShadowMapRenderMethod = m_pSceneManager->CreateRenderMethod(EffectCode);

					if(!m_pShadowMapRenderMethod)
					{
						ALCHEMY_DEBUG_WARNING("CSceneObject::CSceneObject: Shadow map render method has been created fail");

						return false;
					}
				}
				else
				{
					ALCHEMY_DEBUG_WARNING("CSceneObject::SetShadowEnble: Pass definer end define fail.");

					return false;
				}
			}
			else
			{
				ALCHEMY_DEBUG_WARNING("CSceneObject::SetShadowEnble: Pass definer add info fail.");

				return false;
			}
		}

		if(!m_pShadowMapVertexDeclaration)
		{
			UINT uVertexElementParameterCount;
			const CEffectCode::ELEMENTPARAMETER* pVertexElementParameters;

			pVertexElementParameters = m_pShadowMapRenderMethod->GetVertexElementParameters(0, &uVertexElementParameterCount);

			m_pShadowMapVertexDeclaration = __CreateVertexDeclaration(pVertexElementParameters, uVertexElementParameterCount);
		}
	}

	return true;
}

CShadowMapDisplayEffect* CSceneObject::SetShadowDisplay(UINT uLightIndex)
{
	if(!m_pMaterial || !m_pSceneManager)
		return false;

	CLight* pLight = m_pSceneManager->GetLight(uLightIndex);
	if(!pLight)
	{
		ALCHEMY_DEBUG_WARNING("CSceneObject::SetShadowDisplay: Error light index.");

		return ALCHEMY_NULL;
	}

	LPTEXTURE pShadowMap = pLight->GetShadowMap(); 
	if(!pShadowMap)
	{
		ALCHEMY_DEBUG_WARNING("CSceneObject::SetShadowDisplay: Current light does not have a shadow map.");

		return ALCHEMY_NULL;
	}

	CShadowMapDisplayEffect** ppShadowMapDisplayEffect = m_ShadowMapDisplayEffectMap.Get(uLightIndex);
	CShadowMapDisplayEffect* pShadowMapDisplayEffect = ALCHEMY_NULL;

	if(ppShadowMapDisplayEffect)
		pShadowMapDisplayEffect = *ppShadowMapDisplayEffect;
	else
	{
		static FLOAT s_Epsion = 0.005f;
		ALCHEMY_DEBUG_NEW( pShadowMapDisplayEffect, CShadowMapDisplayEffect(m_pMaterial->GetTextures().GetLength(), uLightIndex, s_Epsion) );

		m_ShadowMapDisplayEffectMap.Insert(uLightIndex, pShadowMapDisplayEffect);

		m_pMaterial->AddTexture( pShadowMap->GetResourceHandle() );
	}

	return pShadowMapDisplayEffect;
}

//bool CSceneObject::AddShareVertexDestination(CGraphicsResource::HANDLE* pOutputVertexBuffer, const UINT* puStream, UINT uStreamCount, const UINT* puUsage, const CMesh::ATTRIBUTERANGE* pAttributeRange)
//{
//	if( !__IsValuable() )
//		return false;
//
//	CMesh* pMesh = m_pDestinationMesh->pMesh->Clone(puStream, uStreamCount, puUsage, false, 0);
//
//	if(pMesh)
//	{
//		CVertex& Vertex = pMesh->GetVertex();
//		LPVERTEXBUFFER pVertexBuffer;
//		for(UINT i = 0; i < uStreamCount; ++ i)
//		{
//			pVertexBuffer = Vertex.GetVertexBuffer(puStream[i]);
//
//			if(pVertexBuffer)
//				pOutputVertexBuffer[i] = pVertexBuffer->GetResourceHandle();
//			else
//			{
//				pMesh->Release();
//
//				ALCHEMY_DEBUG_WARNING("CSceneObject::AddShareVertexDestination: No stream.");
//
//				return false;
//			}
//		}
//
//		LPDESTINATIONMESH pDestinationMesh = m_pDestinationMesh;
//
//		while(pDestinationMesh->pNext)
//			pDestinationMesh = pDestinationMesh->pNext;
//
//		ALCHEMY_DEBUG_NEW(pDestinationMesh->pNext, DESTINATIONMESH);
//
//		pDestinationMesh          = pDestinationMesh->pNext;
//		pDestinationMesh->pMesh   = pMesh;
//		pDestinationMesh->uSubset = pAttributeRange ? m_ArributeTable.Add(*pAttributeRange) + 1 : 0;
//		pDestinationMesh->pNext   = ALCHEMY_NULL;
//
//		return true;
//	}
//
//	ALCHEMY_DEBUG_WARNING("CSceneObject::AddShareVertexDestination: Mesh clone fail.");
//
//	return false;
//}

bool CSceneObject::AddShareVertexDestination(const CEffectCode::ELEMENTPARAMETER* pVertexElementParameters, UINT uVertexElementParameterCount, const CMesh::ATTRIBUTERANGE* pAttributeRange, bool bIsSavedInfo)
{
	UINT uRenderData = pAttributeRange ? __AddRenderData(*pAttributeRange) : 0;

	if(bIsSavedInfo)
	{
		CEffectCode::LPELEMENTPARAMETER pElementParameters;
		ALCHEMY_DEBUG_NEW(pElementParameters, CEffectCode::ELEMENTPARAMETER[uVertexElementParameterCount]);

		for(UINT i = 0; i < uVertexElementParameterCount; ++ i)
			pElementParameters[i] = pVertexElementParameters[i];

		SHAREVERTEXINFO ShareVertexInfo;
		ShareVertexInfo.pElementParameters     = pElementParameters;
		ShareVertexInfo.uElementParameterCount = uVertexElementParameterCount;
		ShareVertexInfo.uRenderData            = uRenderData;

		m_ShareVertexInfos.Add(ShareVertexInfo);
	}

	return __AddShareVertexDestination(pVertexElementParameters, uVertexElementParameterCount, uRenderData);
}

void CSceneObject::ClearShareVertexInfo()
{
	CVector<SHAREVERTEXINFO>::CIterator Iterator = m_ShareVertexInfos.GetBeginIterator();

	while( CVector<SHAREVERTEXINFO>::IsValuable(Iterator) )
	{
		ALCHEMY_DEBUG_DELETE_ARRAY(Iterator->pElementParameters);

		++ Iterator;
	}

	m_ShareVertexInfos.Clear();
}

void CSceneObject::Update()
{
	if(__IsSkinned() && m_pSceneManager)
	{
		if(m_pAnimationController && m_bIsUpdate)
			m_pAnimationController->Update( m_pSceneManager->GetElapsedTime() );

		if(m_bIsUpdate && m_pSkinMeshController)
			m_pSkinMeshController->Update();


		//UINT i, uBoneCount = m_pSkinInfo->GetBoneCount();
		//CBone* pBone;
		//if(m_pAnimationController)
		//{
		//	m_pAnimationController->Update( m_pSceneManager->GetElapsedTime() );

		//	m_pRoot->Update();
		//}

		//CMatrix4x4* pBoneMatrix;
		//const MATRIX4X4* pOffsetMatrix, * pTemp;
		//for(i = 0; i < uBoneCount; ++ i)
		//{
		//	pBoneMatrix = &m_pBoneMatrices[i];

		//	pBone = m_ppBones[i];
		//	if(pBone)
		//		pTemp = &pBone->GetWorldMatrix();
		//	else
		//	{
		//		(*pBoneMatrix).Identity();

		//		pTemp = pBoneMatrix;
		//	}

		//	pOffsetMatrix = m_pSkinInfo->GetBoneOffsetMatrix(i);

		//	if(pOffsetMatrix)
		//		Multiply4x3(*pBoneMatrix, *pTemp, *pOffsetMatrix);
		//}

		//if(m_SkinType == SKIN_SOFTWARE)
		//{
		//	LPVERTEXBUFFER pSourceVertexBuffer, pDestinationVertexBuffer;
		//	PVOID pSourceVertices, pDestinationVertices;
		//	UINT uStream;
		//	for(i = 0; i < m_uSkinningStreamCount; ++ i)
		//	{
		//		uStream = m_puSkinningStream[i];
		//		pSourceVertexBuffer      = m_pSourceMesh->GetVertex().GetVertexBuffer(uStream);
		//		pDestinationVertexBuffer = m_pDestinationMesh->/*pMesh->*/GetVertex().GetVertexBuffer(uStream);
		//		if(pSourceVertexBuffer && pDestinationVertexBuffer)
		//		{
		//			pSourceVertices      = pSourceVertexBuffer->Lock( ALCHEMY_FLAG(VERTEXBUFFER::READ_ONLY) );
		//			pDestinationVertices = pDestinationVertexBuffer->Lock(0);

		//			if(pSourceVertices && pDestinationVertices)
		//			{
		//				m_pSkinInfo->Skinning(
		//					pSourceVertexBuffer->GetMemberCount(), 
		//					pSourceVertices, 
		//					pSourceVertexBuffer->GetStride(), 
		//					pDestinationVertices, 
		//					pDestinationVertexBuffer->GetStride(), 
		//					m_pBoneMatrices, 
		//					m_pBoneMatrices, 
		//					m_ppChannels[i],
		//					m_puChannelsCount[i]);

		//				pSourceVertexBuffer     ->Unlock();
		//				pDestinationVertexBuffer->Unlock();
		//			}
		//		}
		//	}
		//}
	}

	CSceneNode::Update();
}

bool CSceneObject::ApplyForRenderShadowMap()
{
	if(m_pDestinationMesh && /*m_pDestinationMesh->pMesh && */m_pMaterial && m_pSceneManager && m_pShadowMapRenderMethod && m_pShadowMapVertexDeclaration)
	{
		CEffect& Effect = m_pShadowMapRenderMethod->GetEffect();
		CRenderQueue::RENDERENTRY& Entry = m_pSceneManager->LockRenderEntry(*this);

		Entry.uRenderTarget      = 0;
		Entry.uEffect            = Effect.GetResourceHandle();
		Entry.uRenderPass        = 0;
		Entry.uVertexDeclaration = m_pShadowMapVertexDeclaration->GetResourceHandle();
		Entry.uModel             = m_pDestinationMesh->/*pMesh->*/GetResourceHandle();
		Entry.uModelParameter    = m_uSubset;

		Entry.uUserData          = 0;

		m_pSceneManager->UnlockRenderEntry(CSceneManager::EFFECT_BEFOREHAND);

		m_pDestinationRenderMethod  = m_pShadowMapRenderMethod;

		return true;
	}

	return true;
}

bool CSceneObject::ApplyForRender()
{
	if(!__IsValuable() || !m_pSceneManager || !m_ppDestinationVertexDeclaration)
		return false;

	/*if( !m_pEffectController->RenderEnable() )
		return false;*/

	//CRenderQueue& RenderQueue = m_pSceneManager->GetRenderQueue();
	CEffect& Effect = m_pRenderMethod->GetEffect();
	CRenderQueue::LPRENDERENTRY pEntry;
	LPDESTINATIONVERTEXDECLARATION pDestinationVertexDeclaration;
	CProgramInstance* pProgramInstance = ALCHEMY_NULL;

	UINT uTotalPass = Effect.GetTotalPass(), i;

	for(i = 0; i < uTotalPass; ++ i)
	{
		pDestinationVertexDeclaration = m_ppDestinationVertexDeclaration[i];

		//pProgramInstance = (i < uProgramLen ? m_ProgramInstances[i] : ALCHEMY_NULL);
		pProgramInstance = m_ppPassProgramMap == ALCHEMY_NULL ? ALCHEMY_NULL : m_ppPassProgramMap[i];

		if(pDestinationVertexDeclaration->pNext)
		{
			pDestinationVertexDeclaration = pDestinationVertexDeclaration->pNext;
			while(pDestinationVertexDeclaration)
			{
				if(pDestinationVertexDeclaration->pVertexDeclaration)
				{
					pEntry = &m_pSceneManager->LockRenderEntry(*this);

					//pEntry->uRenderTarget      = 0;
					pEntry->uEffect            = Effect.GetResourceHandle();
					pEntry->uRenderPass        = i;
					pEntry->uVertexDeclaration = pDestinationVertexDeclaration->pVertexDeclaration->GetResourceHandle();
					pEntry->uModel             = m_pDestinationMesh->GetResourceHandle();
					//pEntry->uDepth             = uDepth;
					//pEntry->uModelType         = CRenderQueue::MODEL_TYPE;
					pEntry->uRenderTarget	   = ( pProgramInstance == ALCHEMY_NULL ? 0 : pProgramInstance->GetRenderTarget() );
					pEntry->uModelParameter    = m_uSubset;

					pEntry->uUserData          = pDestinationVertexDeclaration->uRenderData; 

					if(pProgramInstance)
						pProgramInstance->SubmitRenderEntry();
					else
						m_pSceneManager->UnlockRenderEntry(CSceneManager::EFFECT_NORMAL);
				}

				pDestinationVertexDeclaration = pDestinationVertexDeclaration->pNext;
			}
		}
		else if(pDestinationVertexDeclaration->pVertexDeclaration)
		{
			pEntry = &m_pSceneManager->LockRenderEntry(*this);

			//pEntry->uRenderTarget      = 0;
			pEntry->uEffect            = Effect.GetResourceHandle();
			pEntry->uRenderPass        = i;
			pEntry->uVertexDeclaration = pDestinationVertexDeclaration->pVertexDeclaration->GetResourceHandle();
			pEntry->uModel             = m_pDestinationMesh->GetResourceHandle();
			//pEntry->uDepth             = uDepth;
			//pEntry->uModelType         = CRenderQueue::MODEL_TYPE;
			pEntry->uRenderTarget	   = ( pProgramInstance == ALCHEMY_NULL ? 0 : pProgramInstance->GetRenderTarget() );
			pEntry->uModelParameter    = m_uSubset;

			pEntry->uUserData          = pDestinationVertexDeclaration->uRenderData;

			if(pProgramInstance)
				pProgramInstance->SubmitRenderEntry();
			else
				m_pSceneManager->UnlockRenderEntry(CSceneManager::EFFECT_NORMAL);
		}
	}

	if(m_pNoDrawInstance)
	{
		UINT uNoDrawLen = m_pNoDrawInstance->GetLength();
		for (UINT i=0; i<uNoDrawLen; ++i)
		{
			pProgramInstance = *m_pNoDrawInstance->Get(i);

			pEntry = &m_pSceneManager->LockRenderEntry(*this);

			pEntry->uEffect            = 0;
			pEntry->uRenderPass        = i;
			pEntry->uVertexDeclaration = 0;
			pEntry->uModel             = 0;
			pEntry->uRenderTarget	   = pProgramInstance->GetRenderTarget();
			pEntry->uModelParameter    = 0;
			pEntry->uUserData          = reinterpret_cast<UINT>(pProgramInstance); 

			pProgramInstance->SubmitRenderEntry();
		}
	}

	m_pDestinationRenderMethod  = m_pRenderMethod;

	return true;
}

void CSceneObject::Render(const CRenderQueue::RENDERENTRY& Entry, UINT uFlag)
{
	if(!__IsValuable() || !m_pSceneManager || !m_ppDestinationVertexDeclaration)
		return;

	if( ALCHEMY_TEST_BIT(uFlag, CRenderQueue::RENDER_TARGET) )
	{
		if(Entry.uRenderTarget)
			m_pSceneManager->GetRenderSystem().SetRenderTarget(Entry.uRenderTarget, 0, 0);
		else
			m_pSceneManager->SetSceneRenderTarget();
	}

	CProgramInstance* pCurProgramInstance =  ALCHEMY_NULL;
	if(Entry.uEffect == 0)
	{
		pCurProgramInstance = reinterpret_cast<CProgramInstance*>(Entry.uUserData);
		pCurProgramInstance->OnPreRenderSceneObject(this, uFlag);
		return;
	}

	if(m_ppPassProgramMap)
		pCurProgramInstance = m_ppPassProgramMap[Entry.uRenderPass];//  < uProgramLen ? m_ProgramInstances[ Entry.uRenderPass ] : ALCHEMY_NULL;

	if( pCurProgramInstance && !pCurProgramInstance->OnPreRenderSceneObject(this, uFlag) )
		return;

	CMesh* pDestinationMesh;
	if(!Entry.uModel || !( pDestinationMesh = static_cast<CMesh*>( m_pSceneManager->GetResource(Entry.uModel, CGraphicsResource::MESH) ) ) )
		return;

	CRenderMethodInterface* pRenderMethod = m_pDestinationRenderMethod ? m_pDestinationRenderMethod : m_pRenderMethod;

	m_pDestinationRenderMethod = ALCHEMY_NULL;

	CEffect& Effect = pRenderMethod->GetEffect();

	if( ALCHEMY_TEST_BIT(uFlag, CRenderQueue::EFFECT) && !Effect.Begin() )
		return;

	if( ALCHEMY_TEST_BIT(uFlag, CRenderQueue::EFFECT_PASS) )
	{
		if( !Effect.BeginPass(Entry.uRenderPass) )
			return;

		pRenderMethod->Activate(Entry.uRenderPass);
	}

	if(pCurProgramInstance && !pCurProgramInstance->OnRenderSceneObject(this, uFlag))
		return;

	bool bIsCommitEffectParameters = ALCHEMY_TEST_BIT(uFlag, CRenderQueue::PARENT) || ALCHEMY_TEST_BIT(uFlag, CRenderQueue::EFFECT_PASS);
	if(bIsCommitEffectParameters)
	{
		if( __IsSkinned() && m_pSkinMeshController->GetSkinType() == CSkinMeshController::SKIN_HARDWARE && !m_pBoneCombination )
			pRenderMethod->SetArray(CRenderMethodInterface::BONE_MATRIX_ARRAY, m_pBoneMatrices, m_uBoneCount, false);

		/*pRenderMethod->SetWorldMatrix( GetWorldMatrix() );

		pRenderMethod->SetMaterial(*m_pMaterial);

		pRenderMethod->SetCamera( m_pSceneManager->GetCamera() );

		pRenderMethod->CommitTransform();*/
		pRenderMethod->SetParameters( GetWorldMatrix(), m_pSceneManager->GetCamera(), *m_pMaterial );

		//Effect.CommitChange();
	}

	if( ALCHEMY_TEST_BIT(uFlag, CRenderQueue::VERTEX_DECLARATION) && ( !Entry.uVertexDeclaration || !m_pSceneManager->GetRenderSystem().SetVertexDeclaration(Entry.uVertexDeclaration) ) )
		return;

	LPRENDERDATA pRenderData = * ( m_RenderDataPool.Get(Entry.uUserData) ); 
	if(pRenderData)
	{
		if( __IsSkinned() && m_pSkinMeshController && m_pSkinMeshController->GetSkinType() == CSkinMeshController::SKIN_HARDWARE && m_pBoneCombination )
		{
			while(pRenderData != ALCHEMY_NULL)
			{
				pRenderMethod->SetArray(CRenderMethodInterface::BONE_MATRIX_ARRAY, pRenderData->ppBoneMatrix , pRenderData->uBoneCount, true);

				if(pCurProgramInstance && pCurProgramInstance->GetEffectInfoCount() )
					pCurProgramInstance->UpdatePassLogic( this );

				Effect.CommitChange();

				pDestinationMesh->DrawSubset( * (pRenderData->pAttribute) , true, false);

				pRenderData = pRenderData->pNext;
			}
		}
		else
		{
			if( pCurProgramInstance && pCurProgramInstance->GetEffectInfoCount() )
				pCurProgramInstance->UpdatePassLogic( this );

			if(bIsCommitEffectParameters)
				Effect.CommitChange();

			pDestinationMesh->DrawSubset(* (pRenderData->pAttribute), ALCHEMY_TEST_BIT(uFlag, CRenderQueue::MODEL), false);
		}


		if(pCurProgramInstance)
			pCurProgramInstance->OnEndRenderSceneObject(this, uFlag);
	}

	/*if(pRenderData)
	{
		CMesh::LPATTRIBUTERANGE pAttributeRange = m_AttributeTable.Get(pRenderData->uAttribute);

		if(pAttributeRange)
		{
			if(m_SkinType == SKIN_HARDWARE && __IsSkinned() && pRenderData->uStartBoneCombination <= pRenderData->uEndBoneCombination)
			{
				static CMesh::ATTRIBUTERANGE AttributeRange;
				CMesh::LPBONECOMBINATION pBoneCombination = reinterpret_cast<CMesh::LPBONECOMBINATION>( m_pBoneCombination->GetBufferPointer() ) + pRenderData->uStartBoneCombination;
				bool bIsActivate = ALCHEMY_TEST_BIT(uFlag, CRenderQueue::MODEL);
				UINT i, j;
				for(i = pRenderData->uStartBoneCombination; i <= pRenderData->uEndBoneCombination; ++ i)
				{
					for(j = 0; j < pBoneCombination->uBoneCount; ++ j)
						 m_ppBoneCombinationMatrices[j] = &m_pBoneMatrices[pBoneCombination->puBoneIndices[j]];

					pRenderMethod->SetArray(CRenderMethodInterface::BONE_MATRIX_ARRAY, m_ppBoneCombinationMatrices, pBoneCombination->uBoneCount, true);

					if(pCurProgramInstance)
						pCurProgramInstance->UpdatePassLogic( this );

					Effect.CommitChange();

					AttributeRange.uStartIndex = ALCHEMY_MAXINUM(pAttributeRange->uStartIndex, pBoneCombination->uStartIndex);
					AttributeRange.uIndexCount = ALCHEMY_MININUM(pAttributeRange->uStartIndex + pAttributeRange->uIndexCount, pBoneCombination->uStartIndex + pBoneCombination->uIndexCount) - AttributeRange.uStartIndex;


					pDestinationMesh->DrawSubset(AttributeRange, true, false);

					bIsActivate = false;

					++ pBoneCombination;
				}
			}
			else
			{
				if(pCurProgramInstance)
					pCurProgramInstance->UpdatePassLogic( this );

				if(bIsCommitEffectParameters)
					Effect.CommitChange();

				pDestinationMesh->DrawSubset(*pAttributeRange, ALCHEMY_TEST_BIT(uFlag, CRenderQueue::MODEL), false);
			}

			if(pCurProgramInstance)
				pCurProgramInstance->OnEndRenderSceneObject(this, uFlag);
		}
	}*/
}

void CSceneObject::SetSkinMeshController(CSkinMeshController* pSkinMeshController ,bool isUpdate)
{
	if(pSkinMeshController == ALCHEMY_NULL)
		return;

	m_pSkinMeshController = pSkinMeshController;
	m_pBoneMatrices = m_pSkinMeshController->GetBoneMatrix(m_uBoneCount);

	__UpdateRenderData();

	if(m_pDestinationMesh != ALCHEMY_NULL)
		m_pDestinationMesh->Release();

	m_pDestinationMesh = m_pSkinMeshController->GetDesinationMesh();
	m_pDestinationMesh->AddRef();

	m_bIsUpdate = isUpdate;
}

CGraphicsResource::HANDLE CSceneObject::GetMeshHanlde() const
{
	return m_pDestinationMesh->GetResourceHandle();
}

CVertexDeclaration* CSceneObject::__CreateVertexDeclaration(const CEffectCode::ELEMENTPARAMETER* pVertexElementParameters, UINT uVertexElementParameterCount)
{
	if(!m_pDestinationMesh && pVertexElementParameters)
		return ALCHEMY_NULL;

	CVertexDeclaration* pVertexDeclaration = ALCHEMY_NULL;

	CVertexDeclaration& VertexDeclaration = m_pDestinationMesh->GetVertex().GetVertexDeclaration();
	const CVertexDeclaration::VERTEXELEMENT* pVertexElements = VertexDeclaration.GetVertexElements();
	UINT uVertexElement, uVertexElementCount = VertexDeclaration.GetVertexElementCount();
	CVertexDeclaration::LPVERTEXELEMENT pVertexElementsTemp;

	ALCHEMY_DEBUG_NEW(pVertexElementsTemp, CVertexDeclaration::VERTEXELEMENT[uVertexElementParameterCount]);
	for(UINT i = 0; i < uVertexElementParameterCount; ++ i)
	{
		uVertexElement = CVertexDeclaration::FindVertexElement(
			pVertexElementParameters->Usage, 
			pVertexElementParameters->Type, 
			pVertexElementParameters->uIndex, 
			pVertexElements,
			uVertexElementCount);

		if(uVertexElement < uVertexElementCount)
			pVertexElementsTemp[i] = pVertexElements[uVertexElement];
		else
		{
			ALCHEMY_DEBUG_WARNING("CSceneObject::__CreateVertexDeclaration: Set render method fail: vertex stream is not enough creating the render method vertex decalration.");

			ALCHEMY_DEBUG_DELETE_ARRAY(pVertexElementsTemp);

			return ALCHEMY_NULL;
		}

		++ pVertexElementParameters;
	}

	pVertexDeclaration = m_pSceneManager->GetRenderSystem().CreateVertexDeclaration(pVertexElementsTemp, uVertexElementParameterCount);

	ALCHEMY_DEBUG_DELETE_ARRAY(pVertexElementsTemp);

	return pVertexDeclaration;
}

bool CSceneObject::__AddShareVertexDestination(const CEffectCode::ELEMENTPARAMETER* pVertexElementParameters, UINT uVertexElementParameterCount, UINT uRenderData)
{
	if(!m_pRenderMethod || !m_pDestinationMesh || !m_pSceneManager || !m_ppDestinationVertexDeclaration)
		return false;

	UINT i, uPassCount = m_pRenderMethod->GetEffect().GetTotalPass();

	CVertexDeclaration& MeshVertexDeclaration = m_pDestinationMesh->GetVertex().GetVertexDeclaration();
	UINT uMeshVertexElementCount = MeshVertexDeclaration.GetVertexElementCount();
	const CVertexDeclaration::VERTEXELEMENT* pMeshVertexElement = MeshVertexDeclaration.GetVertexElements();
	const CEffectCode::ELEMENTPARAMETER* pVertexElementParameterTemp;
	PUINT puAddVertexElement;

	ALCHEMY_DEBUG_NEW(puAddVertexElement, UINT[uVertexElementParameterCount]);

	for(i = 0; i < uVertexElementParameterCount; ++ i)
	{
		pVertexElementParameterTemp = &pVertexElementParameters[i];

		if( ( puAddVertexElement[i] = CVertexDeclaration::FindVertexElement(pVertexElementParameterTemp->Usage, pVertexElementParameterTemp->Type, pVertexElementParameterTemp->uIndex, pMeshVertexElement, uMeshVertexElementCount) ) >= uMeshVertexElementCount)
		{
			ALCHEMY_DEBUG_WARNING("CSceneObject::AddShareVertexDestination: The vertex element parameter can not correspond to mesh.");
		}
	}

	LPDESTINATIONVERTEXDECLARATION pDestinationVertexDeclaration;
	CVertexDeclaration* pVertexDeclarationTemp;
	UINT j, uVertexElementTemp, uDestinationVertexElementCount, uCurrentVertexElementCount;
	const UINT* puVertexElementOrderMap;
	const CVertexDeclaration::VERTEXELEMENT* pCurrentVertexElement;
	CVertexDeclaration::LPVERTEXELEMENT pDestinationVertexElement;
	for(i = 0; i < uPassCount; ++ i)
	{
		pDestinationVertexDeclaration = m_ppDestinationVertexDeclaration[i];
		if(!pDestinationVertexDeclaration || !pDestinationVertexDeclaration->pVertexDeclaration)
		{
			ALCHEMY_DEBUG_WARNING("CSceneObject::AddShareVertexDestination: No default vertex declaration.");

			continue;
		}

		pVertexDeclarationTemp     = pDestinationVertexDeclaration->pVertexDeclaration;

		puVertexElementOrderMap    = pVertexDeclarationTemp->GetVertexElementOrderMap();
		pCurrentVertexElement      = pVertexDeclarationTemp->GetVertexElements();
		uCurrentVertexElementCount = pVertexDeclarationTemp->GetVertexElementCount();

		ALCHEMY_DEBUG_NEW(pDestinationVertexElement, CVertexDeclaration::VERTEXELEMENT[uCurrentVertexElementCount]);

		for(j = 0; j < uCurrentVertexElementCount; ++ j)
			pDestinationVertexElement[puVertexElementOrderMap[j]] = pCurrentVertexElement[j];

		for(j = 0; j < uVertexElementParameterCount; ++ j)
		{
			pVertexElementParameterTemp = &pVertexElementParameters[j];
			uVertexElementTemp          = CVertexDeclaration::FindVertexElement(pVertexElementParameterTemp->Usage, pVertexElementParameterTemp->Type, 0, pDestinationVertexElement, uCurrentVertexElementCount);
			if(uVertexElementTemp < uCurrentVertexElementCount)
			{
				uDestinationVertexElementCount = puAddVertexElement[j];

				if(uDestinationVertexElementCount < uMeshVertexElementCount)
					pDestinationVertexElement[uVertexElementTemp] = pMeshVertexElement[uDestinationVertexElementCount];
			}
		}

		while(pDestinationVertexDeclaration->pNext)
			pDestinationVertexDeclaration = pDestinationVertexDeclaration->pNext;

		ALCHEMY_DEBUG_NEW(pDestinationVertexDeclaration->pNext, DESTINATIONVERTEXDECLARATION);

		pDestinationVertexDeclaration = pDestinationVertexDeclaration->pNext;

		pDestinationVertexDeclaration->pVertexDeclaration = m_pSceneManager->GetRenderSystem().CreateVertexDeclaration(pDestinationVertexElement, uCurrentVertexElementCount);

		if(pDestinationVertexDeclaration->pVertexDeclaration)
		{
			pDestinationVertexDeclaration->uRenderData = uRenderData;
			pDestinationVertexDeclaration->pNext       = ALCHEMY_NULL;
		}
		else
		{
			pDestinationVertexDeclaration->pNext = ALCHEMY_NULL;

			ALCHEMY_DEBUG_WARNING("CSceneObject::AddShareVertexDestination: Create vertex declaration fail.");

			ALCHEMY_DEBUG_DELETE(pDestinationVertexDeclaration);
		}

		ALCHEMY_DEBUG_DELETE_ARRAY(pDestinationVertexElement);
	}

	ALCHEMY_DEBUG_DELETE_ARRAY(puAddVertexElement);

	return true;
}

void CSceneObject::__DestroyDestinationVertexDeclaration()
{
	if(m_ppDestinationVertexDeclaration)
	{
		if(m_pRenderMethod)
		{
			UINT uPassCount = m_pRenderMethod->GetEffect().GetTotalPass();
			LPDESTINATIONVERTEXDECLARATION pDestinationVertexDeclaration, pTemp;

			for(UINT i = 0; i < uPassCount; ++ i)
			{
				pDestinationVertexDeclaration = m_ppDestinationVertexDeclaration[i];

				while(pDestinationVertexDeclaration)
				{
					if(pDestinationVertexDeclaration->pVertexDeclaration)
						pDestinationVertexDeclaration->pVertexDeclaration->Release();

					pTemp                         = pDestinationVertexDeclaration;
					pDestinationVertexDeclaration = pDestinationVertexDeclaration->pNext;

					ALCHEMY_DEBUG_DELETE(pTemp);
				}
			}
		}

		ALCHEMY_DEBUG_DELETE_ARRAY(m_ppDestinationVertexDeclaration);
	}
}
//
//void CSceneObject::__GetBoneCombinationRange(PUINT puOutputStart, PUINT puOutputEnd, const CMesh::ATTRIBUTERANGE& AttributeRange)
//{
//	if(puOutputStart || puOutputEnd)
//	{
//		if(m_pBoneCombination)
//		{
//			CMesh::LPBONECOMBINATION pBoneCombination = reinterpret_cast<CMesh::LPBONECOMBINATION>( m_pBoneCombination->GetBufferPointer() );
//
//			UINT i, uBoneCombinationEndIndex, uAttributeEndIndex = AttributeRange.uStartIndex + AttributeRange.uIndexCount;
//
//			bool bIsBegin = true;
//			for(i = 0; i < m_uBoneCombinationCount; ++ i)
//			{
//				uBoneCombinationEndIndex = pBoneCombination->uStartIndex + pBoneCombination->uIndexCount;
//
//				if(bIsBegin)
//				{
//					if(uBoneCombinationEndIndex > AttributeRange.uStartIndex && pBoneCombination->uStartIndex < uAttributeEndIndex)
//					{
//						bIsBegin = false;
//
//						if(puOutputStart)
//							*puOutputStart = i;
//
//						if(puOutputEnd)
//						{
//							if(uBoneCombinationEndIndex >= uAttributeEndIndex)
//							{
//								*puOutputEnd = i;
//
//								break;
//							}
//						}
//						else
//							break;
//					}
//				}
//				else if(uBoneCombinationEndIndex >= uAttributeEndIndex)
//				{
//					*puOutputEnd = i;
//
//					break;
//				}
//
//				++ pBoneCombination;
//			}
//
//			if(bIsBegin)
//			{
//				if(puOutputStart)
//					*puOutputStart = 1;
//
//				if(puOutputEnd)
//					*puOutputEnd = 0;
//			}
//			else if(puOutputEnd)
//				*puOutputEnd = i;
//		}
//		else
//		{
//			if(puOutputStart)
//				*puOutputStart = 1;
//
//			if(puOutputEnd)
//				*puOutputEnd = 0;
//		}
//	}
//}

UINT CSceneObject::__AddRenderData(const CMesh::ATTRIBUTERANGE& AttributeRange)
{
	RENDERDATA* pRenderData = ALCHEMY_NULL;
	ALCHEMY_DEBUG_NEW(pRenderData, RENDERDATA);

	m_AttributeTable.Add( AttributeRange );

	__UpdateSingleRenderData( m_AttributeTable.Get( m_AttributeTable.GetLength() - 1 ), *pRenderData );

	return m_RenderDataPool.Add(pRenderData);
}

void CSceneObject::__UpdateRenderData()
{
	if(!m_pSkinMeshController)
		return;

	if(m_pSkinMeshController->GetSkinType() != CSkinMeshController::SKIN_HARDWARE)
		return;

	m_pBoneCombination = m_pSkinMeshController->GetBoneCombination();
	if(m_pBoneCombination == ALCHEMY_NULL)
		return;

	CPool<RENDERDATA*>::CIterator Iter = m_RenderDataPool.GetBeginIterator();

	while( CPool<RENDERDATA*>::IsValuable( Iter  ) )
	{
		__UpdateSingleRenderData( (*Iter)->pAttribute, **Iter );
		++Iter;
	}

	/*CPool<RENDERDATA>::CIterator Iterator = m_RenderDataPool.GetBeginIterator();

	CMesh::LPATTRIBUTERANGE pAttributeRange;

	while( CPool<RENDERDATA>::IsValuable(Iterator) )
	{
		pAttributeRange = m_AttributeTable.Get(Iterator->uAttribute);

		if(pAttributeRange)
			__GetBoneCombinationRange(&Iterator->uStartBoneCombination, &Iterator->uEndBoneCombination, *pAttributeRange);

		++ Iterator;
	}*/
}

void CSceneObject::__UpdateSingleRenderData( CMesh::ATTRIBUTERANGE* pAttributeRange, RENDERDATA& RenderData )
{
	UINT uStartIndex, uEndIndex;

	RenderData.pNext = ALCHEMY_NULL;
	RenderData.ppBoneMatrix = ALCHEMY_NULL;
	RenderData.uBoneCount = 0;
	RenderData.pAttribute = pAttributeRange;

	if(!m_pSkinMeshController)
		return;

	m_pSkinMeshController->GetBoneCombinationRange(&uStartIndex, &uEndIndex, *pAttributeRange);

	if(uStartIndex > uEndIndex)
		return;

	RENDERDATA*  pRenderData = ALCHEMY_NULL, *pLastRenderData = ALCHEMY_NULL;

	const CMesh::BONECOMBINATION* pBoneCombination = m_pBoneCombination + uStartIndex;

	for (UINT i = uStartIndex; i <= uEndIndex; ++i)
	{
		if(pRenderData == ALCHEMY_NULL)
			pRenderData = &RenderData;
		else
			ALCHEMY_DEBUG_NEW(pRenderData, RENDERDATA);

		ALCHEMY_DEBUG_NEW(pRenderData->pAttribute, CMesh::ATTRIBUTERANGE);

		pRenderData->pAttribute->uStartIndex = Maxinum( pAttributeRange->uStartIndex, pBoneCombination->uStartIndex );
		pRenderData->pAttribute->uIndexCount = Mininum( pAttributeRange->uStartIndex + pAttributeRange->uIndexCount, pBoneCombination->uStartIndex + pBoneCombination->uIndexCount ) - pRenderData->pAttribute->uStartIndex;

		ALCHEMY_DEBUG_NEW(pRenderData->ppBoneMatrix, CMatrix4x4*[pBoneCombination->uBoneCount]);
		pRenderData->uBoneCount = pBoneCombination->uBoneCount;

		for(UINT j = 0; j < pBoneCombination->uBoneCount; ++ j)
			pRenderData->ppBoneMatrix[j] = &m_pBoneMatrices[ pBoneCombination->puBoneIndices[j] ];

		pRenderData->pNext = ALCHEMY_NULL;
		if(pLastRenderData)
			pLastRenderData->pNext = pRenderData;

		pLastRenderData = pRenderData;

		++pBoneCombination;
	}
}

void CSceneObject::__BuildProgramInstanceMap()
{
	if(!m_pAssistantInstance)
		return;

	UINT uTotalPass = m_pRenderMethod->GetEffect().GetTotalPass();
	UINT uPorgramInstanceLen = m_pAssistantInstance->GetLength();

	ALCHEMY_DEBUG_DELETE_ARRAY(m_ppPassProgramMap);
	ALCHEMY_DEBUG_NEW(m_ppPassProgramMap, CProgramInstance*[uTotalPass]);

	PROGRAM_INFO* pProgramInfo;

	for (UINT i=0; i<uTotalPass; ++i)
	{
		m_ppPassProgramMap[i] = ALCHEMY_NULL;
		for (UINT j=0; j<uPorgramInstanceLen; ++j)
		{
			pProgramInfo = m_pAssistantInstance->Get(j);
			if(pProgramInfo->nPassIndex == i)
			{
				m_ppPassProgramMap[i] = pProgramInfo->pProgramInstance;
				break;
			}
		}
	}

	ALCHEMY_DEBUG_DELETE(m_pAssistantInstance);
}